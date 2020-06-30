#include "field.h"
#include "globals.h"
#include "common.h"
#include <imgui.h>
#include "patch.h"

word* current_field_id;
word* previous_field_id;

// Data for debug map jumps
int target_triangle = 0;
int target_field = 0;
uint update_entities_call;
byte map_patch_storage[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Place to store the original bytes so that we can patch back after a map jump
bool map_changing = false;

// FF7 only
int (*old_pc)();
byte* current_entity_id;
byte** level_data_pointer;
short* pending_x;
short* pending_y;
short* pending_triangle;
int* field_ptr_1; //0xCBF5E8
word* field_array_1; //0xCC0CF8

struct FF7SCRIPTHEADER {
	word unknown1;			// Always 0x0502
	char nEntities;			// Number of entities
	char nModels;			// Number of models
	word wStringOffset;		// Offset to strings
	word nAkaoOffsets;		// Specifies the number of Akao/tuto blocks/offsets
	word scale;             // Scale of field. For move and talk calculation (9bit fixed point).
	word blank[3];
	char szCreator[8];      // Field creator (never shown)
	char szName[8];			// Field name (never shown)
};

typedef struct {
	short x, y, z, res;		// short is a 2 byte signed integer
} vertex_3s;

int script_PC_map_change() {
	if (map_changing)
	{
		byte* level_data = *level_data_pointer;
		uint walkmesh_offset = *(uint*)(level_data + 0x16);
		vertex_3s* triangle_data = (vertex_3s*)(level_data + walkmesh_offset + 8 + 24 * target_triangle);

		// Calculates the centroid of the walkmesh triangle
		int x = (triangle_data[0].x + triangle_data[1].x + triangle_data[2].x) / 3;
		int y = (triangle_data[0].y + triangle_data[1].y + triangle_data[2].y) / 3;
		
		*pending_x = x;
		*pending_y = y;
		*pending_triangle = target_triangle;
		map_changing = false;
	}

	return old_pc();
}

void field_init()
{
	if (!ff8)
	{
		// Proxies the PC field opcode to reposition the player after a forced map change
		old_pc = (int (*)())common_externals.execute_opcode_table[0xA0];
		patch_code_dword((uint)&common_externals.execute_opcode_table[0xA0], (DWORD)&script_PC_map_change);

		uint main_loop = ff7_externals.cdcheck + 0xF3;
		uint field_main_loop = get_absolute_value(main_loop, 0x8F8); // 0x60E5B7
		uint sub_408074 = get_relative_call(main_loop, 0x681);
		uint sub_63C17F = get_relative_call(field_main_loop, 0x59);
		uint sub_60BB58 = get_relative_call(sub_63C17F, 0x16F);
		uint update_field_entities = get_relative_call(sub_60BB58, 0x3A); // 0x60C94D
		uint sub_630734 = get_relative_call(ff7_externals.open_field_file, 0xCF);

		level_data_pointer = (byte**)get_absolute_value(sub_630734, 0xB2); // 0xCFF594

		current_entity_id = (byte*)get_absolute_value(common_externals.execute_opcode_table[0x5F], 0x06); // 0xCC0964
		current_field_id = (word*)get_absolute_value(sub_408074, 0x41); // 0xCC15D0
		previous_field_id = (word*)get_absolute_value(sub_408074, 0x4F); // 0xCC0DEC
		update_entities_call = update_field_entities + 0x461; // 0x60CDAE

		pending_x = (short*)get_absolute_value(sub_408074, 0x5D); // 0xCC0D8C
		pending_y = pending_x + 1; // 0xCC0D8E
		pending_triangle = pending_x + 15; // 0xCC0DAA
		field_ptr_1 = (int*)get_absolute_value(ff7_externals.open_field_file, 0xEA); //0xCBF5E8
		field_array_1 = (word*)get_absolute_value(common_externals.execute_opcode_table[0x5F], 0xE); //0xCC0CF8
	}
	else
	{
		uint field_main_loop = get_absolute_value(ff8_externals.main_loop, 0x144); // 0x46FEE0
		uint sub_470250 = get_relative_call(ff8_externals.main_loop, 0x6E7); // 470250
		uint sub_471F70 = get_relative_call(field_main_loop, 0x148); // 0x471F70
		uint sub_4767B0 = get_relative_call(sub_471F70, 0x4FE); // 0x4767B0
		uint update_field_entities = get_relative_call(sub_4767B0, 0x14E); // 0x529FF0
		common_externals.execute_opcode_table = (uint*)get_absolute_value(update_field_entities, 0x65A);

		current_field_id = (word*)get_absolute_value(ff8_externals.main_loop, 0x21F); // 0x1CD2FC0
		previous_field_id = (word*)get_absolute_value(sub_470250, 0x13); // 0x1CE4880
		update_entities_call = update_field_entities + 0x657; // 0x52A647
	}
}

int map_jump_ff7()
{
	// Restores the original field update code
	memcpy_code(update_entities_call, map_patch_storage, 7);

	byte* current_executing_code = (byte*)(field_array_1[*current_entity_id] + *field_ptr_1);

	// Inject MAPJUMP coordinates
	memset(current_executing_code, 0, 10);
	*(current_executing_code) = 0x60; // MAPJUMP
	*(word*)(current_executing_code + 1) = target_field;

	int (*mapjump)() = (int (*)())common_externals.execute_opcode_table[0x60];
	return mapjump();
}

int map_jump_ff8(byte* entity, int arg)
{
	// Forces the entity to be able to trigger a map jump
	entity[0x175] |= 0x01;

	// Restores the original field update code
	memcpy_code(update_entities_call, map_patch_storage, 7);
	map_changing = false;
	
	// Executes the field script to change map
	int (**field_functions)(byte*, uint) = (int (**)(byte*, uint))common_externals.execute_opcode_table;
	field_functions[0xB9](entity, 0); // KILLTIMER
	field_functions[0x07](entity, target_field); // PSHN_L
	field_functions[0x07](entity, target_triangle); // PSHN_L
	return field_functions[0x5C](entity, 0); // MAPJUMP
}

void field_debug(bool *isOpen)
{
	if (!ImGui::Begin("Field Debug", isOpen, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	if (getmode_cached()->driver_mode != MODE_FIELD)
	{
		ImGui::Text("Not currently on a field.");
		ImGui::End();
		return;
	}

	ImGui::Text("Current field ID: %d", *current_field_id);
	ImGui::Text("Previous field ID: %d", *previous_field_id);
	ImGui::Separator();

	// Inputs for changing field map
	ImGui::Text("Switch Field"); ImGui::SetNextItemWidth(100);
	ImGui::InputInt("Field ID", &target_field); ImGui::SameLine(200); ImGui::SetNextItemWidth(100);
	ImGui::InputInt("Triangle ID", &target_triangle);

	if (ImGui::Button("Change") && !map_changing) {
		// Injects a call into where the field entities are checked
		memcpy(map_patch_storage, (void*)update_entities_call, 7); // Make a copy of the existing CALL
		patch_code_dword(update_entities_call, 0x00E89090); // Places 2 NOPs and a CALL
		replace_call(update_entities_call + 2, ff8 ? (void*)&map_jump_ff8 : (void*)&map_jump_ff7);
		map_changing = true;
	}
	ImGui::End();
}
