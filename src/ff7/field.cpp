/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

/*
 * This file contains the changes necessary to support subtractive and 25%
 * blending modes in field backgrounds. Texture pages where these blending
 * modes are used are duplicated and the tile data modified to point to these
 * new pages which have the correct blending mode set.
 */

#include "../ff7.h"
#include "../log.h"
#include "../field.h"
#include "../patch.h"
#include "../sfx.h"
#include "defs.h"
#include <unordered_map>
#include <cmath>

constexpr int field_60fps_ratio = 2;

// model movement and animations
constexpr byte JOIN = 0x08;
constexpr byte SPLIT = 0x09;
constexpr byte CANIM1 = 0xB0;
constexpr byte CANM1 = 0xB1;
constexpr byte CANIM2 = 0xBB;
constexpr byte CANM2 = 0xBC;
constexpr byte PTURA = 0x35;
constexpr byte TURA = 0xAB;
constexpr byte TURNGEN = 0xB4;
constexpr byte TURN = 0xB5;
constexpr byte JUMP = 0xC0;

// background opcodes
constexpr byte BGSCR = 0x2D;

// camera movement and others
constexpr byte NFADE = 0x25;
constexpr byte FADE = 0x6B;
constexpr byte SHAKE = 0x5E;
constexpr byte SCRLC = 0x62;
constexpr byte SCRLA = 0x63;
constexpr byte SCR2DC = 0x66;
constexpr byte SCR2DL = 0x68;
constexpr byte SCRLP = 0x6F;

enum class patch_type
{
	BYTE = 0,
	WORD,
	SHORT,
};

enum class patch_operation
{
	DIVISION = 0,
	MULTIPLICATION = 1,
};

struct opcode_patch_info
{
	int offset;
	patch_type var_type;
	patch_operation operation_type;
};

std::array<uint32_t, 256> old_opcode_table;
std::unordered_map<byte, std::vector<opcode_patch_info>> patch_config_for_opcode{
	{JOIN, {opcode_patch_info{0, patch_type::BYTE, patch_operation::MULTIPLICATION}}},
	{SPLIT, {opcode_patch_info{13, patch_type::BYTE, patch_operation::MULTIPLICATION}}},
	{PTURA, {opcode_patch_info{1, patch_type::BYTE, patch_operation::MULTIPLICATION}}},
	{TURA, {opcode_patch_info{1, patch_type::BYTE, patch_operation::MULTIPLICATION}}},
	{FADE, {opcode_patch_info{5, patch_type::BYTE, patch_operation::DIVISION}}},
	{SHAKE, {opcode_patch_info{4, patch_type::BYTE, patch_operation::MULTIPLICATION}, opcode_patch_info{6, patch_type::BYTE, patch_operation::MULTIPLICATION}}},
	{TURN, {opcode_patch_info{4, patch_type::SHORT, patch_operation::MULTIPLICATION}}},
};

// helper function initializes page dst, copies texture from src and applies
// blend_mode
void field_load_textures_helper(struct ff7_game_obj *game_object, struct struc_3 *struc_3, uint32_t src, uint32_t dst, uint32_t blend_mode)
{
	struct ff7_tex_header *tex_header;

	ff7_externals.make_struc3(blend_mode, struc_3);

	tex_header = (struct ff7_tex_header *)common_externals.create_tex_header();

	ff7_externals.field_layers[dst]->tex_header = tex_header;

	if(ff7_externals.field_layers[src]->type == 1) ff7_externals.make_field_tex_header_pal(tex_header);
	if(ff7_externals.field_layers[src]->type == 2) ff7_externals.make_field_tex_header(tex_header);

	struc_3->tex_header = tex_header;

	if(src != dst)
	{
		ff7_externals.field_layers[dst]->image_data = external_malloc(256 * 256);
		memcpy(ff7_externals.field_layers[dst]->image_data, ff7_externals.field_layers[src]->image_data, 256 * 256);
	}

	tex_header->image_data = (unsigned char*)ff7_externals.field_layers[dst]->image_data;

	tex_header->file.pc_name = (char*)external_malloc(1024);
	sprintf(tex_header->file.pc_name, "field/%s/%s_%02i", strchr(ff7_externals.field_file_name, '\\') + 1, strchr(ff7_externals.field_file_name, '\\') + 1, src);

	ff7_externals.field_layers[dst]->graphics_object = ff7_externals._load_texture(1, PT_S2D, struc_3, 0, game_object->dx_sfx_something);
	ff7_externals.field_layers[dst]->present = true;
}

void field_load_textures(struct ff7_game_obj *game_object, struct struc_3 *struc_3)
{
	uint32_t i;

	ff7_externals.field_convert_type2_layers();

	for(i = 0; i < 29; i++)
	{
		uint32_t blend_mode = 4;

		if(!ff7_externals.field_layers[i]->present) continue;

		if(ff7_externals.field_layers[i]->type == 1)
		{
			if(i >= 24) blend_mode = 0;
			else if(i >= 15) blend_mode = 1;
		}
		else if(ff7_externals.field_layers[i]->type == 2)
		{
			if(i >= 40) blend_mode = 0;
			else if(i >= 33) blend_mode = 1;
		}
		else ffnx_glitch("unknown field layer type %i\n", ff7_externals.field_layers[i]->type);

		field_load_textures_helper(game_object, struc_3, i, i, blend_mode);

		// these magic numbers have been gleaned from original source data
		// the missing blend modes in question are used in exactly these pages
		// and copying them in this manner does not risk overwriting any other
		// data
		if(i >= 15 && i <= 18 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 14, 2);
		if(i >= 15 && i <= 20 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 18, 3);
	}

	*ff7_externals.layer2_end_page += 18;
}

void field_layer2_pick_tiles(short x_offset, short y_offset)
{
	int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
	int x_add = (320 - x_offset) * 2;
	int y_add = ((ff7_center_fields ? 232 : 224) - y_offset) * field_bg_multiplier;
	struct field_tile *layer2_tiles = *ff7_externals.field_layer2_tiles;

	if(*ff7_externals.field_special_y_offset > 0 && y_offset <= 8)
		y_add -= *ff7_externals.field_special_y_offset * field_bg_multiplier;

	for(int i = 0; i < *ff7_externals.field_layer2_tiles_num; i++)
	{
		uint32_t tile_index = (*ff7_externals.field_layer2_palette_sort)[i];
		uint32_t page;
		int x;
		int y;

		char anim_group = layer2_tiles[tile_index].anim_group;
		if(anim_group && !(ff7_externals.modules_global_object->background_sprite_layer[anim_group] & layer2_tiles[tile_index].anim_bitmask))
			continue;

		layer2_tiles[tile_index].field_1040 = 1;

		x = layer2_tiles[tile_index].x * field_bg_multiplier + x_add;
		y = layer2_tiles[tile_index].y * field_bg_multiplier + y_add;

		if(layer2_tiles[tile_index].use_fx_page) page = layer2_tiles[tile_index].fx_page;
		else page = layer2_tiles[tile_index].page;

		if(layer2_tiles[tile_index].use_fx_page && layer2_tiles[tile_index].blend_mode == 2) page += 14;
		if(layer2_tiles[tile_index].use_fx_page && layer2_tiles[tile_index].blend_mode == 3) page += 18;

		ff7_externals.add_page_tile((float)x, (float)y, layer2_tiles[tile_index].z, layer2_tiles[tile_index].u, layer2_tiles[tile_index].v, layer2_tiles[tile_index].palette_index, page);
	}
}

uint32_t field_open_flevel_siz()
{
	struct lgp_file *f = lgp_open_file("flevel.siz", 1);

	if (0 == f) {
		return 0;
	}

	uint32_t size = lgp_get_filesize(f, 1);
	char* buffer = new char[size];

	lgp_read_file(f, 1, buffer, size);

	// Increase from 787 (field map count) to 1200
	const uint32_t max_map_count = 1200;
	uint32_t* uncompressed_sizes = reinterpret_cast<uint32_t*>(buffer);
	uint32_t count = size / sizeof(uint32_t);
	uint32_t* flevel_sizes = reinterpret_cast<uint32_t*>(ff7_externals.field_map_infos + 0xBC);

	if (count > max_map_count) {
		count = max_map_count;
	}

	for (uint32_t i = 0; i < count; ++i) {
		flevel_sizes[i * 0x34] = uncompressed_sizes[i] + 4000000; // +2 MB compared to the original implementation
	}

	// Force a value if not specified by the flevel.siz
	for (uint32_t i = count; i < max_map_count; ++i) {
		flevel_sizes[i * 0x34] = 4000000;
	}

	delete[] buffer;

	return 1;
}

int ff7_field_update_single_model_position(short model_id)
{
	field_event_data* field_event_data_array = (*ff7_externals.field_event_data_ptr);
	int original_movement_speed = field_event_data_array[model_id].movement_speed;
	int original_collision_radius = field_event_data_array[model_id].collision_radius;
	field_event_data_array[model_id].movement_speed = original_movement_speed / field_60fps_ratio;
	if(original_collision_radius > 1)
		field_event_data_array[model_id].collision_radius = original_collision_radius / field_60fps_ratio;
	int ret = ff7_externals.field_update_single_model_position(model_id);
	field_event_data_array[model_id].movement_speed = original_movement_speed;
	field_event_data_array[model_id].collision_radius = original_collision_radius;
	return ret;
}

short ff7_opcode_multiply_get_bank_value(short bank, short address)
{
	int16_t ret = ff7_externals.get_bank_value(bank, address);
	ret *= field_60fps_ratio;
	return ret;
}

short ff7_opcode_divide_get_bank_value(short bank, short address)
{
	int16_t ret = ff7_externals.get_bank_value(bank, address);
	ret /= field_60fps_ratio;
	return ret + 1;
}

int opcode_script_partial_animation_wrapper()
{
	byte curr_opcode = get_field_parameter<byte>(-1);
	WORD total_number_of_frames = -1;
	byte curr_model_id = ff7_externals.field_model_id_array[*ff7_externals.current_entity_id];
	byte speed = get_field_parameter<byte>(3);
	WORD first_frame = 16 * get_field_parameter<byte>(1) * field_60fps_ratio / ((curr_opcode == CANIM1 || curr_opcode == CANIM2) ? speed : 1);
	WORD last_frame = (get_field_parameter<byte>(2) * field_60fps_ratio) / speed;
	field_event_data* event_data = *ff7_externals.field_event_data_ptr;
	field_animation_data* animation_data = *ff7_externals.field_animation_data_ptr;
	char animation_type = ff7_externals.animation_type_array[curr_model_id];

	int ret = ((int(*)())old_opcode_table[curr_opcode])();

	if(curr_model_id != 255)
	{
		switch(animation_type)
		{
		case 0:
		case 1:
		case 3:
			if(animation_data)
				total_number_of_frames = animation_data[curr_model_id].anim_frame_object[2] - 1;

			if(last_frame > total_number_of_frames)
				last_frame = total_number_of_frames;

			event_data[curr_model_id].firstFrame = first_frame;
			event_data[curr_model_id].lastFrame = last_frame;
			break;
		default:
			break;
		}
	}
	return ret;
}

int opcode_script_TURNGEN_wrapper()
{
	WORD rotation_n_steps = get_field_parameter<byte>(3);
	rotation_n_steps *= field_60fps_ratio;

	// There are 7 cases in original FF7 where this condition happens (TODO: Transforming this to short is quite hard)
	if(rotation_n_steps > 255)
		rotation_n_steps = 0xFF;

	patch_field_parameter<byte>(3, (byte)rotation_n_steps);

	return ((int(*)())ff7_externals.opcode_turngen)();
}

int opcode_script_patch_wrapper()
{
	byte curr_opcode = get_field_parameter<byte>(-1);

	if(patch_config_for_opcode.contains(curr_opcode))
	{
		auto opcode_patch_config_array = patch_config_for_opcode[curr_opcode];
		for (auto patch_config: opcode_patch_config_array)
		{
			switch(patch_config.var_type)
			{
			case patch_type::BYTE:
				patch_generic_field_parameter<byte>(patch_config.offset, field_60fps_ratio, patch_config.operation_type == patch_operation::MULTIPLICATION);
				break;
			case patch_type::WORD:
				patch_generic_field_parameter<WORD>(patch_config.offset, field_60fps_ratio, patch_config.operation_type == patch_operation::MULTIPLICATION);
				break;
			case patch_type::SHORT:
				patch_generic_field_parameter<SHORT>(patch_config.offset, field_60fps_ratio, patch_config.operation_type == patch_operation::MULTIPLICATION);
				break;
			default:
				break;
			}
		}
	}

	return ((int(*)())old_opcode_table[curr_opcode])();
}

void ff7_field_hook_init()
{
	std::copy(common_externals.execute_opcode_table, &common_externals.execute_opcode_table[0xFF], &old_opcode_table[0]);

	// Model movement fps and animation (walk vs run) fix
	patch_code_byte(ff7_externals.field_update_models_positions + 0x6E5, 0x9 + field_60fps_ratio / 2);
	patch_code_byte(ff7_externals.field_update_models_positions + 0x70B, 0x9 + field_60fps_ratio / 2);
	patch_code_byte(ff7_externals.field_update_models_positions + 0x739, 0x9 + field_60fps_ratio / 2);
	patch_code_byte(ff7_externals.field_update_models_positions + 0x75F, 0xA + field_60fps_ratio / 2);
	replace_call_function(ff7_externals.field_update_models_positions + 0x9E8, ff7_field_update_single_model_position);
	patch_divide_code<int>(ff7_externals.field_opcode_08_sub_61D4B9 + 0x343, field_60fps_ratio);
	patch_code_byte(ff7_externals.field_update_models_positions + 0x1041, 0x2 - field_60fps_ratio / 2);
	patch_code_byte(ff7_externals.field_update_models_positions + 0x189A, 0x2 - field_60fps_ratio / 2);
	replace_call_function(common_externals.execute_opcode_table[JUMP] + 0x1F1, ff7_opcode_multiply_get_bank_value);
	patch_divide_code<int>(ff7_externals.field_update_models_positions + 0xC89, field_60fps_ratio * 2);
	patch_divide_code<int>(ff7_externals.field_update_models_positions + 0xE48, field_60fps_ratio * 2);

	// Background scroll fps fix
	replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x34, ff7_opcode_divide_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x4D, ff7_opcode_divide_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x68, ff7_opcode_divide_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x81, ff7_opcode_divide_get_bank_value);

	// Camera fps fix
	replace_call_function(common_externals.execute_opcode_table[SCRLC] + 0x3B, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[SCRLA] + 0x72, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[SCR2DC] + 0x3C, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[SCR2DL] + 0x3C, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[SCRLP] + 0xA7, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[NFADE] + 0x89, ff7_opcode_divide_get_bank_value);

	// Animation fps fix
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANM1], (DWORD)&opcode_script_partial_animation_wrapper);
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANM2], (DWORD)&opcode_script_partial_animation_wrapper);
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANIM1], (DWORD)&opcode_script_partial_animation_wrapper);
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANIM2], (DWORD)&opcode_script_partial_animation_wrapper);

	// Others
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[TURNGEN], (DWORD)&opcode_script_TURNGEN_wrapper);
	// Fix opcode by changing their parameters if they don't overflow (Ensure this is done at the end)
	for (const auto &pair : patch_config_for_opcode)
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[pair.first], (DWORD)&opcode_script_patch_wrapper);
}