/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Marcin 'Maki' Gomulak                              //
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

#pragma once

#include <ddraw.h>
#include <stdio.h>
#include <array>
#include <span>

#include "common.h"

#define FF7_MAX_NUM_MODEL_ENTITIES 32

/*
 * Primitive types supported by the engine, mostly a 1:1 mapping to PSX GPU
 * capabilities.
 *
 * Key:
 * T/S/L - triangle/square(quad)/line
 * F/G - flat/gouraud
 * T - textured
 * 2D/3D - self explanatory except 2D means no transforms at all, billboard sprites etc are still "3D"
 */
enum polygon_types
{
	PT_TF2D = 0,
	PT_TF3D,
	PT_TG2D,
	PT_TG3D,
	PT_T2D,
	PT_T3D,
	PT_TGT2D,
	PT_TGT3D,
	PT_SF2D,
	PT_SF3D,
	PT_SG2D,
	PT_SG3D,
	PT_S2D,
	PT_S3D,
	PT_SGT2D,
	PT_SGT3D,
	polygon_type_10,
	polygon_type_11,
	PT_LF2D,
	PT_L2D,
};

// FF7 modules, unknowns are either unused or not relevant to rendering
enum ff7_game_modes
{
	FF7_MODE_FIELD = 1,
	FF7_MODE_BATTLE,
	FF7_MODE_WORLDMAP,
	FF7_MODE_UNKNOWN4,
	FF7_MODE_MENU,
	FF7_MODE_HIGHWAY,
	FF7_MODE_CHOCOBO,
	FF7_MODE_SNOWBOARD,
	FF7_MODE_CONDOR,
	FF7_MODE_SUBMARINE,
	FF7_MODE_COASTER,
	FF7_MODE_CDCHECK,
	FF7_MODE_UNKNOWN13,
	FF7_MODE_SNOWBOARD2,
	FF7_MODE_UNKNOWN15,
	FF7_MODE_UNKNOWN16,
	FF7_MODE_BATTLE_MENU,
	FF7_MODE_UNKNOWN18,
	FF7_MODE_EXIT,
	FF7_MODE_MAIN_MENU,
	FF7_MODE_UNKNOWN21,
	FF7_MODE_INTRO,
	FF7_MODE_SWIRL,
	FF7_MODE_UNKNOWN24,
	FF7_MODE_UNKNOWN25,
	FF7_MODE_GAMEOVER,
	FF7_MODE_CREDITS,
	FF7_MODE_UNKNOWN28,
};

// 3D model flags
enum model_modes
{
	MDL_ROOT_ROTATION         = 0x0001,
	MDL_ROOT_ROTATION_NEGX    = 0x0002,
	MDL_ROOT_ROTATION_NEGY    = 0x0004,
	MDL_ROOT_ROTATION_NEGZ    = 0x0008,
	MDL_ROOT_TRANSLATION      = 0x0010,
	MDL_ROOT_TRANSLATION_NEGX = 0x0020,
	MDL_ROOT_TRANSLATION_NEGY = 0x0040,
	MDL_ROOT_TRANSLATION_NEGZ = 0x0080,
	MDL_USE_STRUC110_MATRIX   = 0x4000,
	MDL_USE_CAMERA_MATRIX     = 0x8000,
};

enum class cmd_id
{
	CMD_ATTACK = 0x01,
	CMD_MAGIC = 0x02,
	CMD_SUMMON = 0x03,
	CMD_ITEM = 0x04,
	CMD_STEAL = 0x05,
	CMD_SENSE = 0x06,
	CMD_COIN = 0x07,
	CMD_THROW = 0x08,
	CMD_MORPH = 0x09,
	CMD_DEATHBLOW = 0x0A,
	CMD_MANIPULATE = 0x0B,
	CMD_MIME = 0x0C,
	CMD_ENEMY_SKILL = 0x0D,
	CMD_MUG = 0x11,
	CMD_CHANGE = 0x12,
	CMD_DEFEND = 0x13,
	CMD_LIMIT = 0x14,
	CMD_W_MAGIC = 0x15,
	CMD_W_SUMMON = 0x16,
	CMD_W_ITEM = 0x17,
	CMD_SLASH_ALL = 0x18,
	CMD_DOUBLE_CUT = 0x19,
	CMD_FLASH = 0x1A,
	CMD_QUAD_CUT = 0x1B,
	CMD_ENEMY_ACTION = 0x20,
	CMD_POISONTICK = 0x23
};

// internal structure for menu sprites (global values, may not be a structure at all)
struct menu_objects
{
	struct ff7_graphics_object *unknown1;
	struct ff7_graphics_object *unused;
	struct ff7_graphics_object *btl_win_a;
	struct ff7_graphics_object *btl_win_b;
	struct ff7_graphics_object *btl_win_c;
	struct ff7_graphics_object *btl_win_d;
	struct ff7_graphics_object *_btl_win;
	struct ff7_graphics_object *blend_btl_win_a;
	struct ff7_graphics_object *add_btl_win_a;
	struct ff7_graphics_object *add_btl_win_b;
	struct ff7_graphics_object *add_btl_win_c;
	struct ff7_graphics_object *add_btl_win_d;
	struct ff7_graphics_object *window_bg;
	struct ff7_graphics_object *blend_window_bg;
	struct ff7_graphics_object *unknown2;
	struct ff7_graphics_object *unknown3;
	struct ff7_graphics_object *unknown4;
	struct ff7_graphics_object *unknown5;
	struct ff7_graphics_object *menu_fade;
	struct ff7_graphics_object *font_a;
	struct ff7_graphics_object *font_b;
	struct ff7_graphics_object *menu_avatars[3];
	struct ff7_graphics_object *menu_avatars2[9];
	struct ff7_graphics_object *buster_tex;
	struct ff7_graphics_object *font;
	struct ff7_graphics_object *btl_win;
	struct ff7_graphics_object *blend_btl_win;
	struct ff7_graphics_object *add_btl_win;
};

// file modes
enum
{
	FF7_FMODE_READ = 0,
	FF7_FMODE_READ_TEXT,
	FF7_FMODE_WRITE,
	FF7_FMODE_CREATE,
};

/*
 * This section defines some structures used internally by the FF7 game engine.
 *
 * Documentation for some of them can be found on the Qhimm wiki, a lot of
 * information can be gleaned from the source code to this program but in many
 * cases nothing is known except the size and general layout of the structure.
 *
 * Variable and structure names are mostly based on what they contain rather
 * than what they are for, a lot of names may be wrong, inappropriate or
 * downright misleading. Thread with caution!
 */

struct list
{
	uint32_t use_assert_alloc;
	uint32_t field_4;
	uint32_t nodes;
	struct list_node *head;
	struct list_node *tail;
	void *destructor;
	void *recursive_find_cb;
	uint32_t field_1C;
};

struct list_node
{
	struct list_node *next;
	void *object;
};

struct file_context
{
	uint32_t mode;
	uint32_t use_lgp;
	uint32_t lgp_num;
	void (*name_mangler)(char *, char *);
};

struct ff7_file
{
	char *name;
	struct lgp_file *fd;
	struct file_context context;
};

struct ff7_indexed_vertices
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t count;
	uint32_t vertexcount;
	uint32_t field_10;
	struct nvertex *vertices;
	uint32_t indexcount;
	uint32_t field_1C;
	WORD *indices;
	uint32_t field_24;
	unsigned char *palettes;
	uint32_t field_2C;
	struct ff7_graphics_object *graphics_object;
};

struct graphics_vertex
{
	point4d position;
	bgra_byte color;
	float alpha_mask;
	float u;
	float v;
};

struct attack_name_fixed_buffer
{
	byte name[32];
};

struct box_vertices { 
	unsigned short initial_x ;
	unsigned short initial_y;
	unsigned short final_x;
	unsigned short final_y;
};

struct battle_menu_data 
{
	unsigned short offset_x;
	unsigned short offset_y;
	unsigned short menu_width;
	unsigned short menu_height;
	unsigned short curr_offset_x;
	unsigned short curr_offset_y;
	unsigned short curr_width;
	unsigned short curr_height;
	unsigned short n_boxes;
	box_vertices box_data[16];
	unsigned short field_92;
	unsigned short field_94;
	unsigned short field_96;
};

struct battle_graphics_data
{
uint32_t battleground_rsd_data;
uint32_t rsd_shadow_data_1;
uint32_t rsd_shadow_data_2;
uint32_t rsd_mark_triangle_data;
ff7_graphics_object* s_effect_graphics_objects_1[3];
ff7_graphics_object* s_effect_graphics_objects_2[3];
ff7_graphics_object* btl_win_a_graphics_object;
ff7_graphics_object* btl_win_b_graphics_object;
ff7_graphics_object* btl_win_c_menu_border_graphics_object;
ff7_graphics_object* btl_win_d_damage_number_graphics_object;
ff7_graphics_object* menu_font_a_graphics_object;
ff7_graphics_object* menu_font_b_graphics_object;
ff7_graphics_object* box_color_graphics_object;
ff7_graphics_object* graphics_data_other_array[2];
uint32_t other_data[2];
uint32_t battle_other_graphics_data[292];
};

struct text_box_data
{
byte* text_buffer;
short window_pos_x;
short window_pos_y;
short window_width;
short window_height;
short current_window_width;
short current_window_height;
short current_text_offset_y;
short current_max_character_length;
short current_n_characters;
short current_line_row;
byte current_pos_y;
byte flags;
byte do_draw_menu_win_blend_4;
byte do_draw_menu_win_b_blend_4;
byte unused;
byte max_menu_win_blend_4_length;
short padding;
uint32_t window_menu_win_b_blend_4_uv_related;
short window_menu_win_blend_4_width;
short window_menu_win_blend_4_height;
short window_menu_win_b_blend_4_width;
short window_menu_win_b_blend_4_height;
short window_mode;
short paging_flags;
};

struct ff7_graphics_object
{
	uint32_t polytype;
	uint32_t field_4;
	uint32_t field_8;
	struct p_hundred *hundred_data;
	struct matrix_set *matrix_set;
	struct polygon_set *polygon_set;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	float u_offset;
	float v_offset;
	void *dx_sfx_2C;
	void *graphics_instance;
	uint32_t field_34;
	uint32_t vertices_per_shape;
	uint32_t indices_per_shape;
	uint32_t vertex_offset;
	uint32_t index_offset;
	uint32_t field_48;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t field_5C;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	graphics_vertex* vertex_transform;
	uint32_t field_74;
	int curr_total_n_shape;
	uint32_t field_7C;
	uint32_t field_80;
	uint32_t field_84;
	uint32_t field_88;
	struct ff7_indexed_vertices *indexed_vertices;
	gfx_polysetrenderstate *func_90;
	gfx_draw_vertices *func_94;
	uint32_t use_matrix_pointer;
	struct matrix *matrix_pointer;
	struct matrix matrix;
};

struct polygon_group
{
	uint32_t field_0;
	uint32_t numvert;
	void *driver_data;
	uint32_t field_C;
	uint32_t normindexes;
	uint32_t vertices;
	uint32_t vertex_colors;
	uint32_t texcoords;
	uint32_t texture_set;
};

struct struc_106
{
	uint32_t field_0;
	uint32_t color;
	vector3<float> point;
	struct bgra_color d3dcol;
};

struct ff7_light
{
	uint32_t flags;
	uint32_t field_4;
	struct struc_106 *color_1;
	struct struc_106 *color_2;
	struct struc_106 *color_3;
	struct bgra_color global_light_color_abgr;
	struct bgra_color global_light_color_abgr_norm;
	uint32_t global_light_color_rgba;
	struct matrix field_38;
	struct matrix field_78;
	struct matrix field_B8;
	struct matrix field_F8;
	uint32_t field_138;
	struct matrix normal_matrix;
	uint32_t field_17C;
	uint32_t field_180;
	uint32_t field_184;
	uint32_t field_188;
	uint32_t field_18C;
	uint32_t field_190;
	uint32_t field_194;
	uint32_t field_198;
	struct matrix *matrix_pointer;
	uint32_t field_1A0;
	uint32_t field_1A4[256];
	uint32_t field_5A4;
	bgra_color_ui8 color;
};

struct ff7_polygon_set
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t numgroups;
	struct struc_49 field_14;
	uint32_t field_2C;
	struct polygon_data *polygon_data;
	struct p_hundred *hundred_data;
	uint32_t per_group_hundreds;
	struct p_hundred **hundred_data_group_array;
	struct matrix_set *matrix_set;
	struct ff7_light *light;
	uint32_t field_48;
	void *execute_buffers;			// IDirect3DExecuteBuffer **
	struct indexed_primitive **indexed_primitives;
	uint32_t field_54;
	uint32_t field_58;
	struct polygon_group *polygon_group_array;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t field_7C;
	uint32_t field_80;
	uint32_t field_84;
	uint32_t field_88;
	uint32_t field_8C;
	uint32_t field_90;
	uint32_t has_struc_173;
	uint32_t field_98;
	struct struc_173 *struc_173;
	uint32_t field_A0;
	uint32_t field_A4;
	uint32_t field_A8;
	uint32_t field_AC;
};

struct ff7_tex_header
{
	uint32_t version;
	uint32_t field_4;
	uint32_t color_key;
	uint32_t field_C;
	uint32_t field_10;
	union
	{
		struct
		{
			uint32_t minbitspercolor;
			uint32_t maxbitspercolor;
			uint32_t minalphabits;
			uint32_t maxalphabits;
		} v1_1;

		struct
		{
			uint32_t x;
			uint32_t y;
			uint32_t w;
			uint32_t h;
		} fb_tex;
	};
	union
	{
		struct
		{
			uint32_t minbitsperpixel;
			uint32_t maxbitsperpixel;
		} v1_2;

		struct
		{
			char *psx_name;
			char *pc_name;
		} file;
	};
	uint32_t field_2C;
	uint32_t palettes;					// ?
	uint32_t palette_entries;			// ?
	uint32_t bpp;
	struct texture_format tex_format;
	uint32_t use_palette_colorkey;
	char *palette_colorkey;
	uint32_t reference_alpha;
	uint32_t blend_mode;
	uint32_t field_CC;
	uint32_t palette_index;
	unsigned char *image_data;
	unsigned char *old_palette_data;
	uint32_t field_DC;
	uint32_t field_E0;
	uint32_t *vram_positions;
	uint32_t y;
};

struct ff7_texture_set
{
	union
	{
		struct
		{
			void *ddsurface1;
			void *d3d2texture1;
			void *ddsurface2;
			void *d3d2texture2;
		} d3d;

		struct
		{
			uint32_t external;
			struct gl_texture_set *gl_set;
			uint32_t width;
			uint32_t height;
		} ogl;
	};

	uint32_t field_10;
	uint32_t field_14;
	uint32_t refcount;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t field_5C;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t *texturehandle;
	struct texture_format *texture_format;
	struct tex_header *tex_header;
	uint32_t palette_index;
	struct palette *palette;
	uint32_t field_90;
	uint32_t field_94;
	uint32_t field_98;
	uint32_t field_9C;
};

struct field_layer
{
	struct ff7_tex_header *tex_header;
	void *image_data;
	struct ff7_graphics_object *graphics_object;
	uint32_t present;
	uint32_t field_10;
	uint32_t field_14;
	WORD field_18;
	WORD type;
};

struct field_object
{
	char name[256];
	char field_100[256];
	char hrc_filename[256];
	char field_300[33];
	char field_321;
	char field_322;
	char field_323;
	char field_324;
	char field_325;
	char field_326;
	unsigned char r_ambient;
	unsigned char g_ambient;
	unsigned char b_ambient;
	unsigned char r_light1;
	unsigned char g_light1;
	unsigned char b_light1;
	unsigned char r_light2;
	unsigned char g_light2;
	unsigned char b_light2;
	unsigned char r_light3;
	unsigned char g_light3;
	unsigned char b_light3;
	unsigned char field_333;
	short x_light1;
	short y_light1;
	short z_light1;
	short x_light2;
	short y_light2;
	short z_light2;
	short x_light3;
	short y_light3;
	short z_light3;
	WORD field_346;
	WORD field_348;
	WORD num_animations;
	char anim_filenames[8880];
	char field_25FC[592];
	char field_284C[60];
	uint32_t field_2888;
};

struct struc_110
{
	uint32_t field_0;
	vector3<float> position;
	vector3<float> rotation;
	vector3<float> scale;
	float scale_factor;
	struct matrix matrix;
	vector3<float> *bone_positions;
	struct matrix *bone_matrices;
};

struct battle_actor_data
{
	uint32_t index;
	uint32_t level;
	uint32_t formation_entry;
	uint32_t command_index;
	uint32_t action_index;
	uint32_t field_14;
	uint32_t allowed_targets;
	uint32_t field_1C;
	uint32_t command_animation;
	uint32_t attack_effect;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t self_mask_1;
	uint32_t self_mask_2;
	uint32_t mp_cost;
	uint32_t action_accuracy;
	uint32_t damage_calc;
	uint32_t action_element;
	uint32_t action_power;
	uint32_t attack_power;
	uint32_t action_target_mask;
	uint32_t field_54[131];
};

struct battle_actor_vars
{
	uint32_t statusMask;		   // 0x00
	uint32_t stateFlags;		   // 0x04
	byte index;					   // 0x08
	byte level;					   // 0x09
	byte unknown0;				   // 0x0A
	byte elementDamageMask;		   // 0x0B
	byte characterID;			   // 0x0C
	byte physAtk;				   // 0x0D
	byte magAtk;				   // 0x0E
	byte pEvade;				   // 0x0F
	byte idleAnimScript;		   // 0x10
	byte damageAnimID;			   // 0x11
	byte backDamageMult;		   // 0x12
	byte sizeScale;				   // 0x13
	byte dexterity;				   // 0x14
	byte luck;					   // 0x15
	byte idleAnimHolder;		   // 0x16
	byte lastCovered;			   // 0x17
	uint16_t lastTargets;		   // 0x18
	uint16_t prevAttackerMask;	   // 0x1A
	uint16_t prevPhysAttackerMask; // 0x1C
	uint16_t prevMagAttackerMask;  // 0x1E
	uint16_t defense;			   // 0x20
	uint16_t mDefense;
	uint16_t formationID;
	uint16_t absorbedElementsMask;
	uint16_t currentMP;
	uint16_t maxMP;
	int currentHP;
	int maxHP;
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t unknown4;
	uint32_t initalStatusMasks;
	uint32_t unknown5;
	byte unknown6;
	byte mEvade;
	byte actorRow;
	byte cameraData;
	uint16_t gilStolen;
	uint16_t itemStolen;
	uint16_t unknown7;
	byte missAnimScript;
	byte APValue;
	uint32_t gilValue;
	uint32_t expValue;
	byte unused8;
	byte unused9;
	uint16_t unused10; // This is being used for the tactical elements mod, unused in original game
	byte unused12;
	byte unused13;
	byte unused14;
	byte unused15;
};

struct battle_ai_context
{
	byte lastCommandIdx;  // 00
	byte lastActionIdx;	  // 08
	byte bankAccessValue; // 10
	byte dummyByte;		  // 18
	byte battleType;	  // 20
	byte colorMask;		  // 30
	byte limitLevel;	  // 38
	byte unk3;			  // 40
	byte unk4;			  // 48
	byte pad;
	uint16_t activeActorMask;	 // 50
	uint16_t scriptOwnerMask;	 // 60
	uint16_t actionTargetMask;	 // 70
	uint16_t actorAlliesMask;	 // 80
	uint16_t activeAlliesMask;	 // 90
	uint16_t actorEnemiesMask;	 // A0
	uint16_t activeEnemiesMask;	 // B0
	uint16_t actorPartyMask;	 // C0
	uint16_t enemyActorsMask;	 // D0
	uint16_t allActorsMask;		 // E0
	uint16_t unkMask2;			 // F0
	uint16_t unkMask3;			 // 0x100
	uint16_t endBattleFlags;	 // 2110
	uint16_t lastActionElements; // 2120
	uint16_t unkDword3;			 // 130
	uint16_t battleFormationIdx; // 140
	uint16_t lastAbsActionIdx;	 // 150
	uint16_t unkBattleFlags;	 // 160
	uint16_t specialAttackFlags; // 170
	uint16_t unkLimitDivisor;	 // 180
	uint16_t unkDword;			 // 190
	uint16_t stringArgs;
	uint16_t somethingEmerald; // 1A0
	uint32_t partyGil;		   // 1C0
	battle_actor_vars actor_vars[10];
};

struct battle_anim_event {
    byte attackerID;
    byte activeAllies;
    byte spellEffectID;
    byte commandIndex;
    byte actionFlags;
    byte animationScriptID;
    uint16_t actionIndex;
    uint16_t cameraData;
    uint16_t damageEventQueueIdx;
};

struct formation_camera{
	vector3<short> position;
	vector3<short> focal_point;
};

struct bcamera_position{
	vector3<short> point;
	WORD unused_6;
	short current_position;
	short frames_to_wait;
	byte field_C;
	byte field_D;
};

struct bcamera_fn_data{
	WORD field_0;
	WORD field_2;
	WORD n_frames;
	short field_6;
	short field_8;
	short field_A;
	short field_C;
	short field_E;
	short field_10;
	byte unused_12[6];
	byte field_18;
	byte unused_19[15];
};

struct battle_model_state
{
    uint16_t characterID;     // BE1178, 0
    uint16_t animScriptIndex; // BE117A, 2
    byte actionFlags;
    byte field_5;
    short field_6;
    uint16_t AnimationData; // BE1180, 8
    uint16_t animScriptPtr; // BE1182, 0xA
    uint16_t field_C;
    uint16_t runningAnimIdx; // 0xE
    uint16_t totalBones;
    uint16_t height;
    short field_14;
    uint16_t initialXRotation;
    uint16_t initialYRotation;
    uint16_t initialZRotation;
    uint16_t field_1C;
    uint16_t field_1E;
    uint16_t field_20;
    byte animationEffect; // BE119A, 0x22
    byte commandID;       // BE119B, 0x23
    byte field_24;
    byte field_25;
    byte actorIsNotActing;
    byte field_27;
    byte field_28;
    byte unkActorFlags;
    byte field_2A;
    byte bData0x12[16];         // 0x2B
    byte isScriptExecuting;     // BE11B3, 0x3B
    byte currentScriptPosition; // 0x3C
    byte waitFrames;            // 0x3D
    byte modelEffectFlags;      // 0x3E
    byte field_3F;
    uint32_t field_40;
    uint32_t field_44;
    uint32_t field_48;
    uint32_t field_4C;
    uint32_t field_50;
    uint32_t field_54;
    uint32_t field_58;
    uint32_t field_5C;
    uint32_t field_60;
    uint32_t field_64;
    uint32_t field_68;
    uint32_t field_6C;
    uint32_t field_70;
    uint32_t field_74;
    byte padding3[0xE6];       // 0x78
	vector3<uint16_t> modelRotation; // BE12D8, 0x15E
    uint16_t field_164;
	vector3<short> modelPosition; // BE12DE, 0x166
    uint32_t field_16C;
    uint32_t *field_170;
    uint32_t field_174;
    byte padding5[0xA24];
    uint32_t playedAnimFrames;
    uint32_t currentPlayingFrame;
    uint32_t tableRelativeModelAnimIdx;
    uint32_t *modelDataPtr;
    byte padding4[0xF18];
    uint32_t setForLimitBreaks; // 0x1AC4
    uint32_t field_1AC8;
    float field_1ACC;
    float field_1AD0;
    float field_1AD4;
    uint32_t padding_1AD8;
    float field_1ADC;
    uint32_t padding_1AE0;
    uint32_t field_1AE4;
    uint32_t field_1AE8;
};

struct battle_model_state_small
{
    uint32_t field_0;
    uint16_t bData68[4];
    uint16_t field_C;
    uint16_t bData76[6];
    uint16_t bData88[6];
    uint16_t actorIsNotActing;
    uint16_t field_28;
    uint16_t field_2A;
    uint16_t someHPCopy;
    uint16_t field_2E;
    uint16_t someMPCopy;
    byte modelDataIndex; // 0x032
    byte field_33;
    byte innateStatusMask;
    byte field_35;
    byte field_36;
    byte field_37;
    uint16_t field_38;
    uint16_t field_3A;
    uint16_t field_3C;
    uint16_t actionIdx;
    byte field_40;
    byte field_41;
    byte field_42;
    byte field_43;
    byte field_44;
    byte field_45;
    byte field_46;
    byte field_47;
    byte field_48;
    byte field_49;
    byte field_4A;
    byte field_4B;
    byte field_4C;
    byte field_4D;
    byte field_4E;
    byte field_4F;
    byte field_50;
    byte field_51;
    byte field_52;
    byte field_53;
    byte field_54;
    byte field_55;
    byte field_56;
    byte field_57;
    byte field_58;
    byte field_59;
    byte field_5A;
    byte field_5B;
    byte field_5C;
    byte field_5D;
    byte field_5E;
    byte field_5F;
    byte field_60;
    byte field_61;
    byte field_62;
    byte field_63;
    byte field_64;
    byte field_65;
    byte field_66;
    byte field_67;
    byte field_68;
    byte field_69;
    byte field_6A;
    byte field_6B;
    byte field_6C;
    byte field_6D;
    byte field_6E;
    byte field_6F;
    byte field_70;
    byte field_71;
    byte field_72;
    byte field_73;
};

struct effect100_data
{
    uint16_t field_0;
    short field_2;
    short n_frames;
    short field_6;
    short field_8;
    short field_A;
    short field_C;
    short field_E;
    int field_10;
    int field_14;
    byte field_18;
    byte field_19;
	short field_1A;
    byte field_1C[4];
};

struct effect60_data
{
    uint16_t field_0;
    short field_2;
    short n_frames;
    short field_6;
    short field_8;
    short field_A;
    uint16_t padding;
    short field_E;
    int field_10;
    int field_14;
    byte field_18;
    byte field_19[7];
};

struct effect10_data
{
    uint16_t field_0;
    short field_2;
    short n_frames;
    short field_6;
    short field_8;
    short field_A;
    short field_C;
    short field_E;
    int field_10;
    int field_14;
    byte field_18;
    byte field_19;
    byte field_1A;
    byte field_1B[5];
};

struct material_anim_ctx
{
    uint32_t *materialRSD;
    uint32_t negateColumnFlags;
    WORD field_8;
    short transparency;
    short field_C;
    short paletteIdx;
};

struct palette_extra
{
    int x_offset;
    p_hundred *aux_gfx_ptr;
    int z_offset_2;
    int y_offset;
    int scroll_v;
    int v_offset;
    int z_offset;
    int field_1C;
    int field_20;
    int field_24;
    int field_28;
};

struct page_spt
{
    int field_0;
    short field_4;
    short field_6;
    short uScale;
    short vScale;
    short field_C;
    short palette_something;
    short field_10;
    short field_12;
};

struct tex_page_list
{
    WORD *field_0;
    page_spt *page_spt_ptr;
};

struct texture_spt
{
    int *spt_handle_copy;
    tex_page_list *pages;
    byte *spt_handle;
    int tex_page_count;
    uint32_t field_10[4];
    ff7_graphics_object *game_drawable[4];
};

struct texture_spt_anim_ctx
{
    texture_spt *effect_spt;
    ff7_graphics_object *effectDrawable;
    color_ui8 color;
    WORD field_C;
    WORD field_E;
};

#pragma pack(push, 1)
struct rotation_matrix
{
    short r3_sub_matrix[3][3];
    int position[3];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct transform_matrix
{
  int16_t eye_x;
  int16_t eye_y;
  int16_t eye_z;
  int16_t target_x;
  int16_t target_y;
  int16_t target_z;
  int16_t up_x;
  int16_t up_y;
  int16_t up_z;
  int pos_x;
  int pos_y;
  int pos_z;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ff7_game_engine_data
{
	float scale;
	byte field_4[4];
	double field_8;
	rotation_matrix rot_matrix;
	byte field_2E[2];
	DWORD field_30;
	DWORD field_34;
	float float_delta_x;
	float float_delta_y;
	vector2<int> world_coord;
	int do_not_transpose;
	color_ui8 primary_color;
	color_ui8 secondary_color;
	float field_54;
	float field_58;
};
#pragma pack(pop)

struct battle_text_data
{
	short buffer_idx;
	short field_2;
	byte wait_frames;
	byte n_frames;
};

struct battle_chdir_struc
{
	uint32_t sucess;
	char olddir[200];
};

struct battle_hrc_bone
{
	uint32_t parent;
	float bone_length;
	uint32_t num_rsd;
};

struct battle_hrc_header
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t bones;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t num_textures;
	uint32_t num_animations_1;
	uint32_t animations_2_start_index;
	uint32_t num_weapons;
	uint32_t num_animations_2;
	uint32_t field_2C;
	struct battle_hrc_bone *bone_data;
};

struct anim_frame_header
{
	vector3<float> root_rotation;
	vector3<float> root_translation;
};

struct anim_frame
{
	struct anim_frame_header *header;
	vector3<float> *data;
};

struct anim_header
{
	union
	{
		struct
		{
			uint32_t version;
		} version;

		struct
		{
			char *pc_name;
		} file;
	};
	uint32_t num_frames;
	uint32_t num_bones;
	char rotation_order[4];
	void *frame_data;
	struct anim_frame *anim_frames;
	uint32_t use_matrix_array;
	struct matrix *matrix_array;
	struct matrix *current_matrix_array;
};

struct hrc_data
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t debug;
	uint32_t flags;
	uint32_t num_bones;
	struct hrc_bone *bones;
	uint32_t field_18;
	struct list *bone_list;
	struct ff7_game_obj *game_object;
	struct matrix field_24;
	struct matrix field_64;
	uint32_t *field_A4;
};

struct hrc_bone
{
	char *bone_name;
	char *bone_parent;
	uint32_t parent_index;
	float bone_length;
	uint32_t num_rsd;
	char **rsd_names;
	struct rsd_array_member *rsd_array;
};

struct bone_list_member
{
	WORD bone_type;
	WORD bone_index;
};

struct rsd_array_member
{
	uint32_t field_0;
	struct rsd_data *rsd_data;
};

struct rsd_data
{
	struct matrix_set *matrix_set;
	struct ff7_polygon_set *polygon_set;
	struct pd_data *pd_data;
};

#pragma pack(push, 1)
struct ff7_hrc_polygon_data
{
  int version;
  DWORD dword4;
  DWORD number_of_frames;
  DWORD dwordC;
  DWORD dword10;
  DWORD dword14;
  BYTE gap18[4];
  DWORD dword1C;
  BYTE gap20[8];
  DWORD dword28;
  int fps;
  DWORD dword30;
  DWORD dword34;
  struc_110 struc_110;
  BYTE gapAC[4];
  DWORD dwordB0;
  hrc_data *hrc_data;
  ff7_game_obj *game_obj;
  __int64 lag;
  unsigned __int64 current_time;
  BYTE gapCC[8];
  file_context file_contextD4;
};
#pragma pack(pop)

struct lgp_toc_entry
{
	char name[16];
	uint32_t offset;
	WORD unknown1;
	WORD conflict;
};

struct lookup_table_entry
{
	unsigned short toc_offset;
	unsigned short num_files;
};

struct conflict_entry
{
	char name[128];
	unsigned short toc_index;
};

struct conflict_list
{
	uint32_t num_conflicts;
	struct conflict_entry *conflict_entries;
};

struct lgp_folders
{
	struct conflict_list conflicts[1000];
};

struct hpmp_bar
{
	WORD x;
	WORD y;
	WORD w;
	WORD h;
	WORD value1;
	WORD max_value;
	WORD healing_animation;
	WORD value2;
	uint32_t color;
};

struct savemap_char
{
	char id;
	char level;
	char field_2;
	char field_3;
	char field_4;
	char field_5;
	char dex;
	char field_7;
	char field_8;
	char field_9;
	char field_A;
	char field_B;
	char field_C;
	char field_D;
	char current_limit_level;
	unsigned char current_limit_bar;
	char name[12];
	char equipped_weapon;
	char equipped_armor;
	char field_1E;
	char flags;
	char field_20;
	unsigned char level_progress_bar;
	WORD learned_limit_break;
	WORD num_kills;
	WORD used_n_limit_1_1;
	WORD field_28;
	WORD field_2A;
	WORD hp;
	WORD base_hp;
	WORD mp;
	WORD base_mp;
	uint32_t field_34;
	WORD max_hp;
	WORD max_mp;
	uint32_t current_exp;
	uint32_t equipped_materia[16];
	uint32_t exp_to_next_level;
};

struct chocobo_slot
{
	WORD sprint_speed;
	WORD max_sprint_speed;
	WORD speed;
	WORD max_speed;
	char acceleration;
	char cooperation;
	char intelligence;
	char personality;
	char p_count;
	char n_races_won;
	boolean is_female;
	char type;
};

#pragma pack(push,1)

struct savemap
{
	uint32_t checksum;
	char preview_level;
	char preview_portraits[3];
	char preview_char_name[16];
	WORD preview_hp;
	WORD preview_maxhp;
	WORD preview_mp;
	WORD preview_maxmp;
	uint32_t preview_gil;
	uint32_t preview_seconds;
	char preview_location[32];
	char ul_window_red;
	char ul_window_green;
	char ul_window_blue;
	char ur_window_red;
	char ur_window_green;
	char ur_window_blue;
	char ll_window_red;
	char ll_window_green;
	char ll_window_blue;
	char lr_window_red;
	char lr_window_green;
	char lr_window_blue;
	savemap_char chars[9];
	byte party_members[3];
	char padding_4FB;
	WORD items[320];
	uint32_t materia[200];
	uint32_t stolen_materia[48];
	char field_B5C[32];
	uint32_t gil;
	uint32_t seconds;
	uint32_t countdown_timer;
	char field_B88[12];
	WORD current_mode;
	WORD current_location;
	WORD field_B98;
	WORD x;
	WORD y;
	WORD z_walkmeshtri;
	char field_BA0[467];
	char yuffie_reg_mask;
	char field_D74[80];
	chocobo_slot chocobo_slots_first[4];
	char field_E04[240];
	char vincent_reg_mask;
	char field_EF5[399];
	chocobo_slot chocobo_slots_last[2];
	WORD phs_lock;
	WORD phs_visi;
	char field_10A8[48];
	char battle_speed;
	char battle_msg_speed;
	char config_bitmap_1;
	char config_bitmap_2;
	char controller_mapping[16];
	char message_speed;
	char field_10ED[7];
};

struct weapon_data
{
	char field_0;
	char field_1;
	char field_2;
	char field_3;
	unsigned char attack_stat;
	char field_5;
	char field_6;
	char field_7;
	char field_8;
	char field_9;
	char field_A;
	char field_B;
	WORD field_C;
	WORD field_E;
	WORD field_10;
	WORD field_12;
	char stat_increase_types[4];
	char stat_increase_amounts[4];
	char field_1C[8];
	char field_24;
	char field_25;
	char field_26;
	char field_27;
	WORD field_28;
	WORD field_2A;
};

struct armor_data
{
	char field_0;
	char field_1;
	unsigned char defense_stat;
	unsigned char mdef_stat;
	char field_4;
	char field_5;
	char field_6;
	WORD field_7;
	char field_9[8];
	char field_11;
	WORD field_12;
	WORD field_14;
	WORD field_16;
	uint32_t field_18;
	uint32_t field_1C;
	WORD field_20;
	WORD field_22;
};

struct party_member_data
{
	char field_0;
	char field_1;
	char field_2;
	char field_3;
	char field_4;
	char field_5;
	char field_6;
	char field_7;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	WORD hp;
	WORD max_hp;
	WORD mp;
	WORD max_mp;
	WORD field_18;
	WORD field_1A;
	WORD field_1C;
	WORD field_1E;
	char field_20;
	char field_21;
	char field_22;
	char field_23;
	char field_24[24];
	WORD field_3C;
	WORD field_3E;
	WORD field_40;
	WORD field_42;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C[24];
	char field_AC[8];
	uint32_t field_B4[21];
	uint32_t field_108[112];
	uint32_t field_2C8[32];
	uint32_t field_348[48];
	struct weapon_data weapon_data;
	uint32_t field_434;
	uint32_t field_438;
	uint32_t field_43C;
};

#pragma pack(pop)

struct field_tile
{
	short x;
	short y;
	float z;
	WORD field_8;
	WORD field_A;
	WORD img_x;
	WORD img_y;
	float u;
	float v;
	WORD fx_img_x;
	WORD fx_img_y;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	WORD tile_size_x;
	WORD tile_size_y;
	WORD palette_index;
	WORD flags;
	char anim_group;
	char anim_bitmask;
	WORD field_36;
	char field_38[4096];
	WORD field_1038;
	WORD field_103A;
	uint32_t use_fx_page;
	uint32_t field_1040;
	uint32_t field_1044;
	uint32_t field_1048;
	uint32_t field_104C;
	char field_1050;
	char field_1051;
	char field_1052;
	char field_1053;
	WORD blend_mode;
	WORD page;
	WORD fx_page;
	WORD field_105A;
};

struct struc_3
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t convert_animations;
	uint32_t create_matrix_set;
	uint32_t field_10;
	uint32_t matrix_set_size;
	struct graphics_instance *graphics_instance;
	uint32_t field_1C;
	uint32_t blend_mode;
	uint32_t base_directory;
	struct ff7_tex_header *tex_header;
	uint32_t field_2C;
	uint32_t light;
	uint32_t field_34;
	float bone_scale_factor;
	uint32_t field_3C;
	struct file_context file_context;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t palette_index;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
};

struct wordmatrix
{
	WORD _11;
	WORD _12;
	WORD _13;
	WORD _21;
	WORD _22;
	WORD _23;
	WORD _31;
	WORD _32;
	WORD _33;
	uint32_t _41;
	uint32_t _42;
	uint32_t _43;
};

struct struc_154_2
{
	short field_0;
	WORD field_2;
	WORD field_4;
	WORD field_6;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	uint32_t field_10;
	uint32_t field_14;
	unsigned char field_18[8];
};

struct struc_154_3
{
	short field_0;
	WORD field_2;
	WORD field_4;
	WORD field_6;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	uint32_t field_10;
	uint32_t field_14;
	unsigned char field_18[8];
};

struct struc_154
{
	short field_0;
	WORD field_2;
	WORD field_4;
	WORD field_6;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	uint32_t field_10;
	uint32_t field_14;
	unsigned char field_18[8];
};

struct struc_205
{
	short field_0;
	WORD field_2;
	WORD field_4;
	WORD field_6;
	WORD field_8;
	WORD field_A;
	WORD field_C;
	WORD field_E;
	uint32_t field_10;
	uint32_t field_14;
	unsigned char field_18[16];
};

struct movie_obj
{
	void *ddstream;
	uint32_t field_4;
	void *mediastream;
	uint32_t loop;
	uint32_t field_10;
	DDSURFACEDESC movie_sdesc;
	void *graphbuilder;
	uint32_t movie_surfaceheight;
	uint32_t field_88;
	void *amms;
	void *movie_surface;
	void *sample;
	uint32_t movie_left;
	uint32_t movie_top;
	uint32_t movie_right;
	uint32_t movie_bottom;
	uint32_t target_left;
	uint32_t target_top;
	uint32_t target_right;
	uint32_t target_bottom;
	void *sts1;
	void *vts1;
	void *sts2;
	void *vts2;
	void *st1;
	void *vt1;
	void *st2;
	void *vt2;
	uint32_t vt1handle;
	uint32_t vt2handle;
	uint32_t field_E0;
	uint32_t movie_surfacewidth;
	uint32_t field_E8;
	struct nvertex movie_vt2prim[4];
	struct nvertex movie_vt1prim[4];
	void *mediaseeking;
	uint32_t graphics_mode;
	uint32_t field_1F4;
	uint32_t field_1F8;
	uint32_t is_playing;
	uint32_t movie_end;
	uint32_t global_movie_flag;
};

struct dll_gfx_externals
{
	void *(*assert_free)(void *, const char *, uint32_t);
	void *(*assert_malloc)(uint32_t, const char *, uint32_t);
	void *(*assert_calloc)(uint32_t, uint32_t, const char *, uint32_t);
	struct texture_format *(*create_texture_format)();
	void (*add_texture_format)(struct texture_format *, struct game_obj *);
	struct game_obj *(*get_game_object)();
	uint32_t free_driver;
	uint32_t create_gfx_driver;
	void (*make_pixelformat)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, struct texture_format *);
	uint32_t gltexformat2texformat;
	uint32_t sub_686143;
	uint32_t sub_6861EC;
	uint32_t sub_68631E;
	uint32_t sub_686351;
	uint32_t pal_pixel2bgra;
	uint32_t pal_pixel2bgra_8bit;
	uint32_t texture_set_destroy_pal;
	struct palette *(*create_palette_for_tex)(uint32_t, struct tex_header *, struct texture_set *);
	uint32_t convert_texture;
	uint32_t texture_set_decref;
	struct texture_set *(*create_texture_set)();
	uint32_t write_palette;
	uint32_t rgba2d3dcol;
	uint32_t sub_6A5FEB;
	uint32_t sub_6A604A;
	uint32_t destroy_palette;
	uint32_t create_palette;
	uint32_t call_gfx_write_palette;
	uint32_t call_gfx_palette_changed;
	uint32_t sub_6A5A70;
	uint32_t sub_6A5BA0;
	uint32_t sub_6A5C3B;
	uint32_t sub_6A5CE2;
	void *(*sub_6A2865)(void *);
	gfx_load_group *generic_load_group;
	gfx_light_polygon_set *generic_light_polygon_set;
};

struct ff7_audio_fmt
{
	uint32_t length;
	uint32_t offset;
	uint32_t loop;
	uint32_t count;
	uint32_t loop_start;
	uint32_t loop_end;
	LPWAVEFORMATEX wave_format;
};

struct ff7_game_obj
{
	uint32_t do_quit;
	uint32_t dc_horzres;
	uint32_t dc_vertres;
	uint32_t dc_bitspixel;
	uint32_t window_pos_x;
	uint32_t window_pos_y;
	uint32_t window_size_x;
	uint32_t window_size_y;
	uint32_t window_minimized;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	double countspersecond;
	time_t _countspersecond;
	uint32_t field_40;
	uint32_t field_44;
	double fps;
	uint32_t tsc;
	uint32_t field_54;					// tsc high bits?
	HINSTANCE hinstance;
	HWND hwnd;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	void *dddevice;
	void *dd2interface;
	LPDIRECTDRAWSURFACE front_surface[3];
	DDSURFACEDESC front_surface_desc[3];
	uint32_t field_1CC;
	uint32_t field_1D0;
	IDirectDrawClipper* dd_clipper;
	uint32_t field_1D8;
	DDSURFACEDESC d3d_surfacedesc;
	void *dd_interface;
	uint32_t field_24C;
	DDSURFACEDESC dd_surfacedesc;
	struct list *d3ddev_list;
	void *d3dinterface;
	void *surface_d3ddev;			// IDirect3DDevice
	struct list *textureformat_list;
	void *d3ddev_struct;
	void *d3dviewport;
	void *d3dmaterial;
	uint32_t field_2D8;
	void *d3d2interface;
	void *d3d2device;
	void *d3dviewport2;
	struct list *list_2E8;
	struct polygon_set *polygon_set_2EC;
	struct polygon_set *polygon_set_2F0;
	struct stack *matrix_stack1;
	struct stack *matrix_stack2;
	struct matrix *camera_matrix;
	struct graphics_instance *graphics_instance;
	uint32_t field_304;
	uint32_t field_308;
	uint32_t field_30C;
	uint32_t field_310;
	uint32_t field_314;
	uint32_t field_318;
	uint32_t field_31C;
	uint32_t field_320;
	D3DDEVICEDESC d3d_halcaps;
	D3DDEVICEDESC d3d_helcaps;
	DDCAPS_DX5 halcaps;
	DDCAPS_DX5 helcaps;
	uint32_t field_794;
	uint32_t field_798;
	struct texture_format surface_tex_format;
	uint32_t in_scene;
	struct p_hundred *hundred_array[5];
	void *applog_debug1;
	uint32_t applog_debug2;
	void *dxdbg_file;
	uint32_t field_840;
	uint32_t field_844;
	uint32_t _res_x;
	uint32_t _res_y;
	uint32_t _res_w;
	uint32_t _res_h;
	uint32_t field_858;
	uint32_t field_85C;
	uint32_t field_860;
	uint32_t field_864;
	uint32_t field_868;
	uint32_t field_86C;
	uint32_t field_870;
	uint32_t field_874;
	uint32_t field_878;
	uint32_t field_87C;
	uint32_t field_880;
	uint32_t field_884;
	uint32_t field_888;
	uint32_t field_88C;
	struct matrix matrix_890;
	struct matrix matrix_8D0;
	void *dx_sfx_something;
	struct list *tex_list_pointer;
	struct stack *stack_918;
	uint32_t field_91C;
	void *_3d2d_something;
	uint32_t field_924;
	uint32_t field_928;
	uint32_t field_92C;
	uint32_t field_930;
	struct gfx_driver *gfx_driver;
	void *_3dobject_pool;
	uint32_t field_93C;
	struct p_hundred *current_hundred;
	struct struc_81 *field_944;
	uint32_t field_948;
	uint32_t field_94C;
	uint32_t field_950;
	uint32_t window_width;
	uint32_t window_height;
	uint32_t colordepth;
	uint32_t field_960;
	uint32_t is_fullscreen;
	uint32_t field_968;
	uint32_t no_hardware;
	uint32_t field_970;
	uint32_t show_cursor;
	uint32_t colorkey;
	uint32_t field_97C;
	uint32_t field_980;
	uint32_t d3d2_flag;
	uint32_t field_988;
	uint32_t field_98C;
	uint32_t field_990;
	uint32_t field_994;
	uint32_t matrix_stack_size;
	uint32_t field_99C;
	uint32_t field_9A0;
	uint32_t field_9A4;
	uint32_t field_9A8;
	uint32_t field_9AC;
	uint32_t random_seed;
	char *window_title;
	char *window_class;
	uint32_t use_custom_wndclass;
	WNDCLASSA wndclass_struct;
	uint32_t use_custom_dwStyle;
	DWORD dwStyle;
	struct main_obj engine_loop_obj;
	struct main_obj game_loop_obj;
	void *wm_activate;
	uint32_t field_A2C;
	uint32_t field_A30;
	uint32_t field_A34;
	uint32_t field_A38;
	uint32_t field_A3C;
	uint32_t field_A40;
	uint32_t field_A44;
	uint32_t field_A48;
	uint32_t field_A4C;
	uint32_t field_A50;
	uint32_t gfx_reset;
	uint32_t field_A58;
	uint32_t field_A5C;
	uint32_t current_gfx_driver;
	uint32_t field_A64;
	uint32_t field_A68;
	uint32_t field_A6C;
	uint32_t field_A70;
	uint32_t field_A74;
	uint32_t field_A78;
	void *gfx_driver_data;
	uint32_t field_A80;
	uint32_t field_A84;
	void *create_gfx_driver;
	struct dll_gfx_externals *externals;
	uint32_t nvidia_fix;
	uint32_t tnt_fix;
	uint32_t no_riva_fix;
	uint32_t field_A9C;
};

struct ff7_gamepad_status
{
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t dpad_up;
	uint32_t dpad_down;
	uint32_t dpad_left;
	uint32_t dpad_right;
	uint32_t button1;
	uint32_t button2;
	uint32_t button3;
	uint32_t button4;
	uint32_t button5;
	uint32_t button6;
	uint32_t button7;
	uint32_t button8;
	uint32_t button9;
	uint32_t button10;
	uint32_t button11;
	uint32_t button12;
	uint32_t button13;
	uint32_t button14;
	uint32_t button15;
	uint32_t button16;
};

struct ff7_gfx_driver
{
	gfx_init *init;
	gfx_cleanup *cleanup;
	gfx_lock *lock;
	gfx_unlock *unlock;
	gfx_flip *flip;
	gfx_clear *clear;
	gfx_clear_all *clear_all;
	gfx_setviewport *setviewport;
	gfx_setbg *setbg;
	uint32_t field_24;
	struct bgra_color field_28;			// ?
	uint32_t field_38;
	uint32_t field_3C;
	gfx_prepare_polygon_set *prepare_polygon_set;
	gfx_load_group *load_group;
	gfx_setmatrix *setmatrix;
	gfx_unload_texture *unload_texture;
	gfx_load_texture *load_texture;
	gfx_palette_changed *palette_changed;
	gfx_write_palette *write_palette;
	gfx_blendmode *blendmode;
	gfx_light_polygon_set *light_polygon_set;
	gfx_field_64 *field_64;
	gfx_setrenderstate *setrenderstate;
	gfx_setrenderstate *_setrenderstate;
	gfx_setrenderstate *__setrenderstate;
	gfx_field_74 *field_74;
	gfx_field_78 *field_78;
	gfx_draw_deferred *draw_deferred;
	gfx_field_80 *field_80;
	gfx_field_84 *field_84;
	gfx_begin_scene *begin_scene;
	gfx_end_scene *end_scene;
	gfx_field_90 *field_90;
	gfx_polysetrenderstate *setrenderstate_flat2D;
	gfx_polysetrenderstate *setrenderstate_smooth2D;
	gfx_polysetrenderstate *setrenderstate_textured2D;
	gfx_polysetrenderstate *setrenderstate_paletted2D;
	gfx_polysetrenderstate *_setrenderstate_paletted2D;
	gfx_draw_vertices *draw_flat2D;
	gfx_draw_vertices *draw_smooth2D;
	gfx_draw_vertices *draw_textured2D;
	gfx_draw_vertices *draw_paletted2D;
	gfx_polysetrenderstate *setrenderstate_flat3D;
	gfx_polysetrenderstate *setrenderstate_smooth3D;
	gfx_polysetrenderstate *setrenderstate_textured3D;
	gfx_polysetrenderstate *setrenderstate_paletted3D;
	gfx_polysetrenderstate *_setrenderstate_paletted3D;
	gfx_draw_vertices *draw_flat3D;
	gfx_draw_vertices *draw_smooth3D;
	gfx_draw_vertices *draw_textured3D;
	gfx_draw_vertices *draw_paletted3D;
	gfx_polysetrenderstate *setrenderstate_flatlines;
	gfx_polysetrenderstate *setrenderstate_smoothlines;
	gfx_draw_vertices *draw_flatlines;
	gfx_draw_vertices *draw_smoothlines;
	gfx_field_EC *field_EC;
};

struct ff7_field_sfx_state {
	uint32_t u1;
	uint32_t volume1;
	uint32_t volume2;
	uint32_t u2;
	uint32_t u3;
	uint32_t u4;
	uint32_t pan1;
	uint32_t pan2;
	uint32_t u5;
	uint32_t u6;
	uint32_t u7;
	uint32_t byte;
	uint32_t u9;
	uint32_t u10;
	uint32_t u11;
	uint32_t frequency;
	uint32_t sound_id;
	IDirectSoundBuffer* buffer1;
	IDirectSoundBuffer* buffer2;
	uint32_t is_looped;
	uint32_t u12;
};

struct ff7_camdata
{
	// EYE
	vector3<short> eye;
	// TARGET
	vector3<short> target;
	// UP
	vector3<short> up;
	// FILLER?
	int16_t padding;
	// POSITION
	vector3<int> position;
	// PAN
	int16_t pan_x;
	int16_t pan_y;
	// ZOOM
	int16_t zoom;
	// FILLER?
	int16_t padding2;
};

struct ff7_shake_bg_data
{
	uint8_t do_shake;
	uint8_t shake_phase;
	char amp_index;
	char shake_curr_value;
	short shake_amplitude;
	short shake_initial;
	short shake_final;
	short shake_n_steps;
	short shake_idx;
};

struct ff7_modules_global_object
{
  uint8_t field_0;
  uint8_t game_mode;
  uint16_t battle_id;
  uint16_t field_model_pos_x;
  uint16_t field_model_pos_y;
  uint16_t field_8;
  short field_A;
  short field_C;
  uint16_t field_E;
  int16_t field_10;
  uint8_t field_12;
  uint8_t field_13;
  uint8_t field_14;
  uint8_t field_15;
  short field_16;
  uint16_t field_18;
  uint16_t field_1A;
  uint8_t field_1C;
  byte world_move_mode;
  char world_move_follow_model_id;
  byte world_move_status;
  uint16_t field_20;
  uint16_t field_model_triangle_id;
  uint16_t field_model_anim_id;
  uint16_t previous_game_mode;
  uint16_t num_models;
  uint16_t field_model_id;
  uint16_t field_2C;
  uint16_t field_2E;
  uint16_t field_30;
  uint8_t field_32;
  uint8_t field_33;
  uint8_t field_34;
  uint8_t field_35;
  uint8_t field_36;
  uint8_t SCRLO_flag;
  uint8_t MPDSP_flag;
  uint8_t MVCAM_flag;
  uint8_t BGMOVIE_flag;
  uint8_t BTLON_flag;
  uint8_t field_3C;
  uint8_t field_3D;
  uint16_t field_3E;
  uint16_t field_40;
  uint16_t field_42;
  uint32_t midi_id;
  uint32_t field_48;
  uint16_t fade_type;
  uint16_t fade_adjustment;
  uint16_t fade_speed;
  short fade_r;
  short fade_g;
  short fade_b;
  uint16_t field_58;
  uint16_t field_5A;
  uint16_t field_5C;
  uint16_t nfade_r;
  uint16_t nfade_g;
  uint16_t nfade_b;
  uint16_t field_id;
  uint16_t field_66;
  uint32_t current_key_input_status;
  uint32_t previous_key_input_status;
  uint16_t field_70;
  uint16_t field_72;
  uint16_t field_74;
  uint16_t field_76;
  unsigned short special_current_key_input_status;
  unsigned short field_7A;
  uint16_t field_7C;
  uint32_t field_80;
  uint16_t field_84;
  uint16_t field_86;
  uint16_t MOVIE_frame;
  ff7_shake_bg_data shake_bg_x;
  ff7_shake_bg_data shake_bg_y;
  uint16_t bg2_scroll_speed_x;
  uint16_t bg2_scroll_speed_y;
  uint16_t bg3_scroll_speed_x;
  uint16_t bg3_scroll_speed_y;
  uint16_t field_AE;
  uint16_t field_B0;
  uint8_t field_B2[64];
  uint8_t background_sprite_layer[64];
  uint16_t field_132;
  uint32_t field_134;
};

struct ff7_field_script_header {
	WORD unknown1;			// Always 0x0502
	char nEntities;			// Number of entities
	char nModels;			// Number of models
	WORD wStringOffset;		// Offset to strings
	WORD nAkaoOffsets;		// Specifies the number of Akao/tuto blocks/offsets
	WORD scale;             // Scale of field. For move and talk calculation (9bit fixed point).
	WORD blank[3];
	char szCreator[8];      // Field creator (never shown)
	char szName[8];			// Field name (never shown)
};

struct field_event_data
{
	WORD field_0;
	WORD padding_2;
	DWORD field_4;
	byte field_8;
	byte padding_9;
	WORD blink_wait_frames;
	vector3<int> model_pos;
	vector3<int> model_initial_pos;
	byte field_24[8];
	int field_2C;
	short movement_ladder_jump_steps;
	short movement_step_idx;
	byte padding_34;
	byte field_35;
	byte rotation_value;
	byte field_37;
	byte rotation_curr_value;
	byte rotation_n_steps;
	byte rotation_step_idx;
	byte rotation_steps_type;
	short rotation_initial;
	short rotation_final;
	short offset_position_x;
	short field_42;
	short offset_initial_x;
	short offset_final_x;
	short offset_position_y;
	short field_4A;
	short offset_initial_y;
	short offset_final_y;
	short offset_position_z;
	short field_52;
	short offset_initial_z;
	short offset_final_z;
	short offset_n_steps;
	short offset_step_idx;
	char offset_movement_phase;
	byte entity_id;
	byte field_5E;
	byte field_5F;
	byte field_60;
	byte field_61;
	byte field_62;
	char movement_type;
	char animation_id;
	byte padding_65;
	short animation_speed;
	short currentFrame;
	short lastFrame;
	short character_id;
	short field_direction_or_collision;
	short movement_phase;
	short collision_radius;
	short talk_radius;
	WORD movement_speed;
	short field_triangle_id;
	short field_7A;
	vector3<int> model_final_pos;
};

struct field_animation_data
{
	int field_0;
	int actor_x;
	int actor_y;
	int actor_z;
	byte field_10[16];
	byte eye_texture_idx;
	byte field_24[336];
	WORD field_174;
	WORD field_176;
	ff7_hrc_polygon_data *anim_frame_object;
	uint32_t *field_17C;
	p_hundred* custom_left_eye_tex;
  p_hundred* static_left_eye_tex;
  p_hundred* custom_right_eye_tex;
  p_hundred* static_right_eye_tex;
};

struct field_gateway
{
	vector3<short> v1_exit_line;
	vector3<short> v2_exit_line;
	vector3<short> destination_vertex;
	SHORT field_id;
	byte unknown[4];
};

struct field_trigger
{
	vector3<short> v_corner1;
	vector3<short> v_corner2;
	byte bg_group_id;
	byte bg_frame_id;
	byte behavior;
	byte sound_id;
};

struct field_arrow
{
	int pos_x;
	int pos_y;
	int pos_z;
	int arrow_type;
};

struct field_camera_range
{
	short left;
	short top;
	short right;
	short bottom;
};

struct field_trigger_header
{
	byte field_name[9];
	byte control_direction;
	short focus_height;
	field_camera_range camera_range;
	byte field_14[4];
	short bg3_width;
	short bg3_height;
	short bg4_width;
	short bg4_height;
	short bg3_pos_x;
	short bg3_pos_y;
	short bg4_pos_x;
	short bg4_pos_y;
	short bg3_speed_x;
	short bg3_speed_y;
	short bg4_speed_x;
	short bg4_speed_y;
	short field_30[4];
	field_gateway gateways[12];
	field_trigger triggers[12];
	byte show_arrow_flag[12];
	field_arrow arrows[12];
};

struct field_arrow_graphics_data
{
	ff7_graphics_object *arrow_graphics_object;
	vector2<float> vertices[4];
	vector2<float> texture_uv[4];
	float z_value;
	int n_shapes;
};

struct field_model_blink_data
{
	byte blink_left_eye_mode;
  byte blink_right_eye_mode;
  char blink_mouth_mode;
  char model_id;
};

struct world_event_data
{
	world_event_data *next_ptr;
	world_event_data *player_data_ptr;
	world_event_data *special_data_ptr;
	vector4<int> position;
	vector4<int> prev_position;
	byte field_28[20];
	short facing;
	byte field_42[2];
	short offset_y;
	short curr_script_position;
	byte field_48[2];
	WORD walkmap_type;
	short direction;
	byte field_4E[2];
	byte model_id;
	byte animation_is_loop_mask;
	byte model_id_special;
	byte animation_frame_idx;
	byte field_54;
	byte movement_speed;
	byte wait_frames;
	byte is_function_running_maybe;
	byte animation_speed;
	byte field_59[3];
	char vertical_speed;
	byte animation_id;
	byte field_5E;
	char vertical_speed_2;
	byte field_60[104];
};

// Snowboard

struct tmd_3_fs_fp
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t mode2;
  uint16_t normal0;
  uint16_t vertex0;
  uint16_t vertex1;
  uint16_t vertex2;
};

struct tmd_3_fs_gp
{
  uint8_t r0;
  uint8_t g0;
  uint8_t b0;
  uint8_t mode2;
  uint8_t r1;
  uint8_t g1;
  uint8_t b1;
  uint8_t pad1;
  uint8_t r2;
  uint8_t g2;
  uint8_t b2;
  uint8_t pad2;
  uint16_t normal0;
  uint16_t vertex0;
  uint16_t vertex1;
  uint16_t vertex2;
};

struct tmd_3_gs_fp
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t mode2;
  uint16_t normal0;
  uint16_t vertex0;
  uint16_t normal1;
  uint16_t vertex1;
  uint16_t normal2;
  uint16_t vertex2;
};

struct tmd_3_gs_gp
{
  uint8_t r0;
  uint8_t g0;
  uint8_t b0;
  uint8_t mode2;
  uint8_t r1;
  uint8_t g1;
  uint8_t b1;
  uint8_t pad1;
  uint8_t r2;
  uint8_t g2;
  uint8_t b2;
  uint8_t pad2;
  uint16_t normal0;
  uint16_t vertex0;
  uint16_t normal1;
  uint16_t vertex1;
  uint16_t normal2;
  uint16_t vertex2;
};

struct tmd_3_ns_fp
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t mode2;
  uint16_t vertex0;
  uint16_t vertex1;
  uint16_t vertex2;
  uint16_t pad;
};

struct tmd_3_ns_gp
{
  uint8_t r0;
  uint8_t g0;
  uint8_t b0;
  uint8_t mode2;
  uint8_t r1;
  uint8_t g1;
  uint8_t b1;
  uint8_t pad1;
  uint8_t r2;
  uint8_t g2;
  uint8_t b2;
  uint8_t pad2;
  uint16_t vertex0;
  uint16_t vertex1;
  uint16_t vertex2;
  uint16_t pad;
};

struct tmd_3_tx_fs_np
{
  uint8_t u0;
  uint8_t v0;
  uint16_t cba;
  uint8_t u1;
  uint8_t v1;
  uint16_t tsb;
  uint8_t u2;
  uint8_t v2;
  uint16_t pad;
  uint16_t normal;
  uint16_t vertex0;
  uint16_t vertex1;
  uint16_t vertex2;
};

struct tmd_3_tx_gs_np
{
  uint8_t u0;
  uint8_t v0;
  uint16_t cba;
  uint8_t u1;
  uint8_t v1;
  uint16_t tsb;
  uint8_t u2;
  uint8_t v2;
  uint16_t pad;
  uint16_t normal;
  uint16_t vertex0;
  uint16_t normal1;
  uint16_t vertex1;
  uint16_t normal2;
  uint16_t vertex2;
};

struct tmd_3_tx_ns_fp
{
  uint8_t u0;
  uint8_t v0;
  uint16_t cba;
  uint8_t u1;
  uint8_t v1;
  uint16_t tsb;
  uint8_t u2;
  uint8_t v2;
  uint16_t pad1;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t pad2;
  uint16_t vertex0;
  uint16_t vertex1;
  uint16_t vertex2;
  uint16_t pad;
};

struct tmd_3_tx_ns_gp
{
  uint8_t u0;
  uint8_t v0;
  uint16_t cba;
  uint8_t u1;
  uint8_t v1;
  uint16_t tsb;
  uint8_t u2;
  uint8_t v2;
  uint16_t pad1;
  uint8_t r0;
  uint8_t g0;
  uint8_t b0;
  uint8_t pad2;
  uint8_t r1;
  uint8_t g1;
  uint8_t b1;
  uint8_t pad3;
  uint8_t r2;
  uint8_t g2;
  uint8_t b2;
  uint8_t pad4;
  uint16_t vertex0;
  uint16_t vertex1;
  uint16_t vertex2;
  uint16_t pad;
};

struct tmd_header
{
  int32_t version;
  int32_t flags;
  int32_t nobjects;
};

struct tmd_normal
{
  int16_t nx;
  int16_t ny;
  int16_t nz;
  int16_t pad;
};

struct tmd_primitive_header
{
  uint8_t olen;
  uint8_t ilen;
  uint8_t flag;
  uint8_t mode;
};

struct tmd_primitive_packet
{
  tmd_3_fs_fp tmd3fsfp;
  tmd_3_gs_fp tmd3gsfp;
  tmd_3_fs_gp tmd3fsgp;
  tmd_3_gs_gp tmd3gsgp;
  tmd_3_tx_fs_np tmd3txfsnp;
  tmd_3_tx_gs_np tmd3txgsnp;
  tmd_3_ns_fp tmd3nsfp;
  tmd_3_ns_gp tmd3nsgp;
  tmd_3_tx_ns_fp tmd3txnsfp;
  tmd_3_tx_ns_gp tmd3txnsgp;
};

struct tmd_vertex
{
  int16_t vx;
  int16_t vy;
  int16_t vz;
  int16_t pad;
};

struct tmd_vertex_float
{
	float vx;
	float vy;
	float vz;
	float pad;
};

struct tmd_object
{
  int32_t offsetverts;
  int32_t nverts;
  int32_t offsetnormals;
  int32_t nnormals;
  int32_t offsetprimitives;
  int32_t nprimitives;
  int32_t scale;
  tmd_primitive_header *tmdprimitivelist;
  tmd_primitive_packet *tmdprimitivelistpacket;
  tmd_vertex *tmdvertexlist;
  tmd_normal *tmdnormallist;
};

struct tmdmodel
{
  tmd_header tmdheader;
  tmd_object *tmdobjectlist;
};

struct snowboard_this
{
  DWORD num_objects;
  tmdmodel *model_data;
  DWORD field_8;
  DWORD field_C;
  DWORD field_10;
  float h_scale;
  float v_scale;
};

struct snowboard_object
{
  snowboard_this *_this;
  int actor;
  int actor_num_tmd_blocks;
  char *field_C;
  char *field_10;
  char *field_14;
  char *field_18;
  char *field_1C;
  char *field_20;
  int field_24;
  int field_28;
  int field_2C;
  int field_30;
  int field_34;
  int field_38;
  int field_3C;
  int field_40;
  int field_44;
  int field_48;
  int field_4C;
  int field_50;
  int field_54;
  int field_58;
  int field_5C;
  int field_60;
  int field_64;
  int field_68;
};

struct world_texture_data
{
	int field_0[2];
	short top_left_x;
	short top_left_y;
	uint8_t start_u_multiplier;
	uint8_t start_v_multiplier;
	uint8_t field_E[2];
	short top_right_x;
	short top_right_y;
	uint8_t end_u_multiplier;
	uint8_t field_15[3];
	short bottom_left_x;
	short bottom_left_y;
	uint8_t field_1C;
	uint8_t end_v_multiplier;
	uint8_t field_1E[2];
	short bottom_right_x;
	short bottom_right_y;
	uint8_t field_24[4];
	struct ff7_graphics_object* graphics_object;
	bgra_color_ui8 color;
};

struct world_effect_2d_list_node
{
  world_effect_2d_list_node *next;
  int x;
  int y;
  int z;
  byte field_10[10];
  __int16 rotation_y;
  byte unknown_idx;
  byte field_1C;
  byte apply_rotation_y;
  byte field_1E[5];
  world_texture_data texture_data;
};

struct world_snake_graphics_data
{
	int field_0[2];
    vector2<short> top_left_vertex;
    uint8_t top_left_u;
    uint8_t top_left_v;
    short n_shapes;
    vector2<short> top_right_vertex;
	uint8_t top_right_u;
	uint8_t top_right_v;
	uint8_t field_16[2];
	vector2<short> bottom_left_vertex;
	uint8_t bottom_left_u;
	uint8_t bottom_left_v;
	short delta_x;
	vector2<short> bottom_right_vertex;
	uint8_t bottom_right_u;
	uint8_t bottom_right_v;
	short delta_y;
	struct ff7_graphics_object* graphics_object;
	bgra_color_ui8 color;
};

struct ff7_model_eye_texture_data
{
  int has_eyes;
  char *custom_left_eye_filename;
  char *static_left_eye_filename;
  char *custom_right_eye_filename;
  char *static_right_eye_filename;
};


// --------------- end of FF7 imports ---------------

struct ff7_model_custom_data
{
	int has_mouth;
	int current_mouth_idx;
	char* mouth_tex_filename;
	p_hundred* mouth_tex;
	char *left_eye_tex_filename;
	char *right_eye_tex_filename;
	p_hundred* left_eye_tex;
	p_hundred* right_eye_tex;
};

struct ff7_channel_6_state
{
	float volume;
	float panning;
};

// memory addresses and function pointers from FF7.exe
struct ff7_externals
{
	uint32_t chocobo_fix;
	uint32_t midi_fix;
	void *snowboard_fix;
	uint32_t cdcheck;
	uint32_t cdcheck_enter_sub;
	uint32_t get_inserted_cd_sub;
	DWORD* insertedCD;
	uint8_t* requiredCD;
	struct movie_obj *movie_object;
	void (*movie_sub_415231)(char *);
	void (*sub_665D9A)(struct matrix *, struct nvertex *, struct indexed_primitive *, struct p_hundred *, struct struc_186 *, struct ff7_game_obj *);
	void (*sub_671742)(uint32_t, struct p_hundred *, struct struc_186 *);
	void (*sub_6B27A9)(struct matrix *, struct indexed_primitive *, struct ff7_polygon_set *, struct p_hundred *, struct p_group *, void *, struct ff7_game_obj *);
	void (*sub_68D2B8)(uint32_t, struct ff7_polygon_set *, void *);
	void (*sub_665793)(struct matrix *, uint32_t, struct indexed_primitive *, struct ff7_polygon_set *, struct p_hundred *, struct p_group *, struct ff7_game_obj *);
	void (*matrix3x4)(struct matrix *);
	uint32_t matrix4x3_multiply;
	void *(*sub_6A2865)(void *);
	uint32_t sub_6B26C0;
	uint32_t sub_6B2720;
	uint32_t sub_673F5C;
	struct savemap *savemap;
	struct menu_objects *menu_objects;
	uint32_t magic_thread_start;
	void (*destroy_magic_effects)();
	uint32_t lgp_open_file;
	uint32_t lgp_read_file;
	uint32_t lgp_read;
	uint32_t lgp_get_filesize;
	uint32_t lgp_seek_file;
	int (*lzss_decode)(char*, char*);
	char** field_file_buffer;
	DWORD* field_file_section_ptrs;
	uint32_t* known_field_buffer_size;
	uint32_t* field_CFF268;
	void (*draw_character)(uint32_t, uint32_t, char *, uint32_t, float);
	uint32_t destroy_field_bk;
	uint32_t destroy_field_tiles;
	struct field_layer **field_layers;
	WORD *num_field_entities;
	struct field_object **field_objects;
	uint32_t open_field_file;
	WORD *field_id;
	char *field_file_name;
	uint32_t read_field_file;
	uint32_t battle_enter;
	uint32_t battle_loop;
	battle_actor_data *battle_actor_data;
	DWORD *battle_mode;
	WORD *battle_location_id;
	WORD *battle_formation_id;
	uint32_t battle_sub_429AC0;
	uint32_t battle_sub_42D808;
	uint32_t battle_sub_42D992;
	uint32_t battle_sub_42DAE5;
	uint32_t battle_sub_427C22;
	uint32_t battle_menu_update_6CE8B3;
	uint32_t battle_sub_6DB0EE;
	char* is_battle_paused;
	std::span<uint32_t> battle_menu_state_fn_table;
	std::span<uint32_t> magic_effects_fn_table;
	uint32_t comet2_sub_5A42E5;
	uint32_t comet2_unload_sub_5A4359;
	uint32_t battle_b3ddata_sub_428B12;
	uint32_t graphics_render_sub_68A638;
	uint32_t create_dx_sfx_something;
	uint32_t load_p_file;
	struct polygon_data *(*create_polygon_data)(uint32_t, uint32_t);
	void (*create_polygon_lists)(struct polygon_data *);
	void (*free_polygon_data)(struct polygon_data *);
	uint32_t battle_sub_42A0E7;
	uint32_t load_battle_stage;
	uint32_t load_battle_stage_pc;
	uint32_t read_battle_hrc;
	void (*battle_regular_chdir)(struct battle_chdir_struc *);
	void (*battle_context_chdir)(struct file_context *, struct battle_chdir_struc *);
	void (*swap_extension)(char *, char *, char *);
	void (*destroy_battle_hrc)(uint32_t, struct battle_hrc_header *);
	void (*battle_regular_olddir)(struct battle_chdir_struc *);
	void (*battle_context_olddir)(struct file_context *, struct battle_chdir_struc *);
	uint32_t load_animation;
	uint32_t field_load_animation;
	uint32_t field_load_models;
	uint32_t field_models_eye_to_model;
	ff7_model_eye_texture_data* field_models_eye_blink_buffer;
	DWORD* field_models_data;
	int (*field_load_model_eye_tex)(ff7_model_eye_texture_data *eye_data, field_animation_data *anim_data);
	p_hundred* (*field_load_model_tex)(int idx1, int shademode, char *filename, struc_3 *tex_info, game_obj *game_object);
	void (*field_unload_model_tex)(void* eye_tex);
	void (*create_struc_3_info_sub_67455E)(struc_3 *tex_info);
	uint32_t field_sub_60DCED;
	int (*field_sub_6A2736)(ff7_polygon_set *polygon_set);
	p_hundred** (*field_sub_6A2782)(int idx, p_hundred *hundreddata, ff7_polygon_set *polygon_set);
	uint32_t* field_unk_909288;
	void (*destroy_animation)(struct anim_header *);
	uint32_t context_chdir;
	uint32_t lgp_chdir;
	struct lookup_table_entry **lgp_lookup_tables;
	struct lgp_toc_entry **lgp_tocs;
	struct lgp_folders *lgp_folders;
	uint32_t __read;
	uint32_t load_lgp;
	uint32_t open_lgp_file;
	FILE **lgp_fds;
	uint32_t battle_sub_437DB0;
	uint32_t battle_scene_bin_sub_5D1050;
	int (*engine_load_bin_file_sub_419210)(char *filename, int offset, int size, char **out_buffer, void (*callback)(void));
	uint32_t sub_5CB2CC;
	uint32_t *midi_volume_control;
	uint32_t *midi_initialized;
	uint32_t menu_sub_6CDA83;
	uint32_t menu_sub_6CBD43;
	uint32_t menu_sub_701EE4;
	uint32_t phs_menu_sub;
	uint32_t menu_draw_party_member_stats;
	uint32_t *party_member_to_char_map;
	uint32_t menu_sub_6CB56A;
	uint32_t menu_sub_6FEDB0;
	void *(*menu_sub_6F5C0C)(uint32_t,uint32_t,uint8_t,uint8_t,uint32_t);
	void *(*menu_sub_6FAC38)(uint32_t,uint32_t,uint8_t,uint8_t,uint32_t);
	BOOL (*write_save_file)(char);
	uint32_t *menu_subs_call_table;
	int (*menu_tutorial_sub_6C49FD)();
	BYTE* menu_tutorial_window_state;
	DWORD* menu_tutorial_window_text_ptr;
	uint32_t status_menu_sub;
	uint32_t draw_status_limit_level_stats;
	uint32_t timer_menu_sub;
	DWORD *millisecond_counter;
	char *(*get_kernel_text)(uint32_t, uint32_t, uint32_t);
	uint32_t sub_5CF282;
	uint32_t get_equipment_stats;
	struct weapon_data *weapon_data_array;
	struct armor_data *armor_data_array;
	uint32_t field_sub_6388EE;
	uint32_t field_draw_everything;
	uint32_t field_pick_tiles_make_vertices;
	uint32_t field_layer1_pick_tiles;
	uint32_t *field_layer1_tiles_num;
	uint32_t **field_layer1_palette_sort;
	field_tile **field_layer1_tiles;
	uint32_t field_layer2_pick_tiles;
	uint32_t *field_layer2_tiles_num;
	uint32_t **field_layer2_palette_sort;
	field_tile **field_layer2_tiles;
	uint32_t field_layer3_pick_tiles;
	uint32_t *field_layer3_tiles_num;
	uint32_t **field_layer3_palette_sort;
	field_tile **field_layer3_tiles;
	int *do_draw_layer3_CFFE3C;
	int *field_layer3_flag_CFFE40;
	uint32_t field_layer4_pick_tiles;
	uint32_t *field_layer4_tiles_num;
	uint32_t **field_layer4_palette_sort;
	field_tile **field_layer4_tiles;
	int *do_draw_layer4_CFFEA4;
	int *field_layer4_flag_CFFEA8;
	int *field_layer_CFF1D8;
	uint16_t *field_palette_D00088;
	uint32_t *field_special_y_offset;
	uint32_t *field_bg_multiplier;
	void (*add_page_tile)(float, float, float, float, float, uint32_t, uint32_t);
	double (*field_layer_sub_623C0F)(rotation_matrix*, int, int, int);
	void (*field_draw_gray_quads_644E90)();
	void (*engine_draw_graphics_object)(ff7_graphics_object*, ff7_game_obj*);
	field_trigger_header** field_triggers_header;
	rotation_matrix* field_camera_rotation_matrix_CFF3D8;
	uint32_t field_load_textures;
	void (*field_convert_type2_layers)();
	void (*make_struc3)(uint32_t, struct struc_3 *);
	void (*make_field_tex_header_pal)(struct ff7_tex_header *);
	void (*make_field_tex_header)(struct ff7_tex_header *);
	struct ff7_graphics_object *(*_load_texture)(uint32_t, uint32_t, struct struc_3 *, char *, void *);
	uint32_t read_field_background_data;
	WORD *layer2_end_page;
	uint32_t create_d3d2_indexed_primitive;
	uint32_t destroy_d3d2_indexed_primitive;
	uint32_t enter_main;
	uint32_t kernel_init;
	void (*kernel_load_kernel2)(char* filename);
	uint32_t kernel2_reset_counters;
	uint32_t sub_4012DA;
	uint32_t kernel2_add_section;
	uint32_t kernel2_get_text;
	char **kernel_1to9_sections;
	uint32_t draw_3d_model;
	void (*stack_push)(struct stack *);
	void *(*stack_top)(struct stack *);
	void (*stack_pop)(struct stack *);
	void (*_root_animation)(struct matrix *, struct anim_frame *, struct anim_header *, struct hrc_data *);
	void (*_frame_animation)(uint32_t, struct matrix *, vector3<float> *, struct anim_frame *, struct anim_header *, struct hrc_bone *, struct hrc_data *);
	void (*root_animation)(struct matrix *, struct anim_frame *, struct anim_header *, struct hrc_data *);
	void (*frame_animation)(uint32_t, struct matrix *, vector3<float> *, struct anim_frame *, struct anim_header *, struct hrc_bone *, struct hrc_data *);
	uint32_t *model_mode;
	uint32_t name_menu_sub_6CBD32;
	uint32_t name_menu_sub_719C08;
	uint32_t menu_sub_71894B;
	uint32_t menu_sub_718DBE;
	uint32_t menu_sub_719B81;
	uint32_t set_default_input_settings_save;
	uint32_t keyboard_name_input;
	uint32_t restore_input_settings;
	uint32_t dinput_getdata2;
	uint32_t init_stuff;
	uint32_t init_game;
	uint32_t sub_41A1B0;
	uint32_t init_directinput;
	uint32_t dinput_createdevice_mouse;
	void (*sub_69C69F)(struct matrix *, struct ff7_light *);
	uint32_t coaster_sub_5E9051;
	uint32_t coaster_sub_5EE150;
	uint32_t cleanup_game;
	uint32_t wm_activateapp;
	uint32_t get_gamepad;
	uint32_t update_gamepad_status;
	struct ff7_gamepad_status* gamepad_status;
	uint32_t music_is_locked;
	uint32_t field_initialize_variables;
	uint32_t music_lock_clear_fix;
	uint32_t sub_60DF96;
	uint32_t sub_60EEB2;
	uint32_t open_flevel_siz;
	uint32_t field_map_infos;
	uint32_t sound_operation;
	struct ff7_field_sfx_state* sound_states;
	uint32_t config_menu_sub;
  uint32_t config_initialize;
	uint32_t call_menu_sound_slider_loop_sfx_up;
	uint32_t call_menu_sound_slider_loop_sfx_down;
	uint32_t menu_start;
	uint32_t battle_clear_sound_flags;
	uint32_t swirl_sound_effect;
	uint32_t field_init_player_character_variables;
	uint32_t field_init_event_wrapper_63BCA7;
	uint32_t field_init_event_60BACF;
	uint32_t field_init_field_objects_60BCFA;
	uint32_t execute_opcode;
	uint32_t opcode_goldu;
	uint32_t opcode_dlitm;
	uint32_t opcode_smtra;
	uint32_t opcode_akao;
	uint32_t opcode_akao2;
	uint32_t opcode_bmusc;
	uint32_t opcode_fmusc;
	uint32_t opcode_cmusc;
	uint32_t opcode_canm1_canm2;
	uint32_t opcode_fade;
	uint32_t opcode_shake;
	uint32_t field_opcode_08_sub_61D0D4;
	void (*field_opcode_08_09_set_rotation_61DB2C)(short, byte, byte);
	uint32_t field_opcode_AA_2A_sub_616476;
	uint32_t field_opcode_turn_character_sub_616CB5;
	int (*field_get_rotation_final_636515)(vector3<int>*, vector3<int>*, int*);
	uint32_t field_music_helper;
	uint32_t field_music_helper_sound_op_call;
	uint32_t (*field_music_id_to_midi_id)(int16_t);
	uint32_t field_music_id_to_midi_id_call1;
	uint32_t field_music_id_to_midi_id_call2;
	uint32_t field_music_id_to_midi_id_call3;
	uint32_t opcode_gameover;
	uint32_t opcode_message;
	uint32_t opcode_ask;
	uint32_t opcode_wmode;
	uint32_t opcode_tutor;
	uint32_t opcode_pc;
	uint32_t opcode_kawai;
	uint32_t *sfx_initialized;
	uint32_t sfx_play_summon;
	uint32_t sfx_load_and_play_with_speed;
	ff7_audio_fmt* sfx_fmt_header;
	DWORD *sfx_play_effects_id_channel_6;
	uint32_t sfx_stop_channel_6;
	UINT *sfx_stop_channel_timer_handle;
	uint32_t battle_summon_leviathan_loop;
	uint32_t battle_limit_omnislash_loop;
	void (*reset_game_obj_sub_5F4971)(struct game_obj*);
	uint32_t engine_exit_game_mode_sub_666C78;
	void* (*sub_666C13)(struct game_obj*);
	void* (*sub_670F9B)(void*);
	WORD* word_CC0828;
	BYTE* byte_CC0D89;
	WORD* word_DB958A;
	BYTE* byte_CC164C;
	WORD* word_CC0DC6;
	uint32_t enter_gameover;
	uint32_t exit_gameover;
	void* (*start_gameover)();
	void* (*gameover_sub_6C12B1)();
	uint32_t on_gameover_enter;
	uint32_t on_gameover_exit;
	BYTE (*sub_60B260)();
	BYTE(*sub_767C55)();
	uint32_t field_battle_toggle;
	uint32_t worldmap_battle_toggle;
	uint32_t enter_field;
	uint32_t field_init_viewport_values;
	uint32_t field_loop_sub_63C17F;
	uint32_t field_update_models_positions;
	int (*field_update_single_model_position)(short);
	void (*field_update_model_animation_frame)(short);
	int (*field_check_collision_with_target)(field_event_data*, short);
	int (*field_get_linear_interpolated_value)(int, int, int, int);
	int (*field_get_smooth_interpolated_value)(int, int, int, int);
	void (*field_evaluate_encounter_rate_60B2C6)();
	uint32_t field_animate_3d_models_6392BB;
	uint32_t field_apply_kawai_op_64A070;
	uint32_t sub_64EC60;
	field_model_blink_data* field_model_blink_data_D000C8;
	void (*field_blink_3d_model_649B50)(field_animation_data*, field_model_blink_data*);
	short *field_player_model_id;
	WORD *field_n_models;
	uint32_t field_update_camera_data;
	ff7_camdata** field_camera_data;
	uint32_t sub_40B27B;
	WORD* word_CC0DD4;
	WORD* word_CC1638;
	uint32_t field_init_scripted_bg_movement;
	uint32_t field_update_scripted_bg_movement;
	void (*field_update_background_positions)();
	uint32_t compute_and_submit_draw_gateways_arrows_64DA3B;
	void (*field_submit_draw_arrow_63A171)(field_arrow_graphics_data*);
	uint32_t field_draw_pointer_hand_60D4F3;
	uint32_t field_submit_draw_pointer_hand_60D572;
	uint32_t field_sub_64314F;
	void (*set_world_pos_based_on_player_pos_643C86)(vector2<short>*);
	void(*field_clip_with_camera_range_6438F6)(vector2<short>*);
	uint32_t field_layer3_clip_with_camera_range_643628;
	void (*engine_set_game_engine_delta_values_661976)(int, int);
	uint32_t engine_apply_matrix_product_66307D;
	void (*engine_convert_psx_matrix_to_float_matrix_row_version_661465)(rotation_matrix*, float*);
	void (*engine_apply_rotation_to_transform_matrix_6628DE)(vector3<short>*, rotation_matrix*);
	void (*engine_apply_matrix_product_to_vector_66CF7E)(float*, vector3<float>*, vector3<float>*);
	vector2<int>* field_bg_offset;
	short* field_curr_delta_world_pos_x;
	short* field_curr_delta_world_pos_y;
	short* scripted_world_initial_pos_x;
	short* scripted_world_initial_pos_y;
	short* scripted_world_final_pos_x;
	short* scripted_world_final_pos_y;
	short* scripted_world_move_n_steps;
	short* scripted_world_move_step_index;
	short* field_world_pos_x;
	short* field_world_pos_y;
	short* field_prev_world_pos_x;
	short* field_prev_world_pos_y;
	short* field_cursor_pos_x;
	short* field_cursor_pos_y;
	vector2<int>* field_viewport_xy_CFF204;
	vector2<int>* field_max_half_viewport_width_height_CFF1F4;
	vector2<int>* field_curr_half_viewport_width_height_CFF1FC;
	WORD* field_bg_flag_CC15E4;
	uint32_t field_sub_640EB7;
	uint32_t field_sub_661B68;
	void (*engine_set_game_engine_world_coord_661B23)(int, int);
	void (*engine_sub_67CCDE)(float, float, float, float, float, float, float, ff7_game_obj*);
	uint32_t field_handle_screen_fading;
	uint32_t field_opcode_message_update_loop_630D50;
	uint32_t field_text_box_window_create_631586;
	void (*field_text_box_window_opening_6317A9)(short);
	void (*field_text_box_window_paging_631945)(short);
	uint32_t field_text_box_window_reverse_paging_632CAA;
	uint32_t field_text_box_window_closing_632EB8;
	char* field_entity_id_list; // 0xCC0960
	DWORD* current_dialog_string_pointer; //0xCBF578
	WORD* current_dialog_message_speed; // 0xCC0418
	WORD* opcode_message_loop_code;
	int (*field_opcode_ask_update_loop_6310A1)(uint8_t, uint8_t, uint8_t, uint8_t, WORD*);
	WORD* opcode_ask_question_code;
	void (*play_midi)(uint32_t);
	WORD *current_movie_frame;
	uint32_t opening_movie_play_midi_call;
	DWORD *opening_movie_music_start_frame;
	uint32_t fps_limiter_swirl;
	uint32_t fps_limiter_battle;
	uint32_t fps_limiter_coaster;
	uint32_t fps_limiter_condor;
	uint32_t fps_limiter_field;
	uint32_t fps_limiter_highway;
	uint32_t fps_limiter_snowboard;
	uint32_t fps_limiter_worldmap;
	uint32_t fps_limiter_chocobo;
	uint32_t fps_limiter_submarine;
	uint32_t fps_limiter_credits;
	uint32_t sub_5F5042;
	uint32_t highway_loop_sub_650F36;
	uint32_t sub_779E14;
	uint32_t battle_fps_menu_multiplier;
	DWORD *submarine_minigame_status;
	time_t *submarine_last_gametime;
	DWORD *field_limit_fps;
	DWORD *swirl_limit_fps;
	int16_t (*get_bank_value)(int16_t, int16_t);
	int8_t (*set_bank_value)(int16_t, int16_t, int16_t);
	int8_t (*get_char_bank_value)(int16_t, int16_t);
	uint32_t sub_611BAE;
	byte* current_entity_id;
	byte** field_script_ptr; //0xCBF5E8
	WORD* field_curr_script_position; //0xCC0CF8
	byte* field_model_id_array; //0xCBFB70
	field_event_data** field_event_data_ptr; // 0xCC0B60
	field_animation_data** field_animation_data_ptr; // 0xCFF738
	WORD* wait_frames_ptr; //0xCC0900
	char* animation_type_array; //0xCC0980
	ff7_modules_global_object *modules_global_object; // 0xCC0D88
	ff7_modules_global_object **field_global_object_ptr; // 0xCBF9D8
	void (*sub_767039)(DWORD*,DWORD*,DWORD*);
	uint32_t play_battle_music_call;
	uint32_t (*play_battle_end_music)();
	uint32_t play_battle_music_win_call;
	uint32_t wm_change_music;
	uint32_t wm_play_music_call;
	uint32_t battle_fight_end;
	uint32_t battle_fanfare_music;
	int (*sub_630C48)(int16_t, int16_t, int16_t, int16_t, int16_t);
	uint32_t sub_408074;
	uint32_t sub_60BB58;
	byte** field_level_data_pointer;
	uint32_t sub_408116;
	char *word_CC16E8;
	uint16_t* menu_battle_end_mode;
	uint32_t* pointer_functions_7C2980;
	uint32_t battle_enemy_killed_sub_433BD2;
	uint32_t battle_sub_5C7F94;
	uint32_t menu_battle_end_sub_6C9543;
	uint32_t menu_sub_71FF95, menu_shop_loop, get_materia_gil, opcode_increase_gil_call;
	uint32_t opcode_add_materia_inventory_call, menu_sub_6CBCF3, menu_sub_705D16, menu_sub_6CC17F;
	uint32_t display_battle_action_text_42782A;
	uint32_t display_battle_action_text_sub_6D71FA;
	uint32_t menu_decrease_item_quantity;
	uint32_t opcode_setbyte, opcode_biton;
	uint32_t sub_60FA7D;
	uint32_t menu_sub_7212FB;
	uint32_t load_save_file;
	uint32_t handle_actor_ready;
	WORD* battle_menu_state;
	uint32_t set_battle_menu_state_data;
	uint32_t dispatch_chosen_battle_action;
	uint32_t set_battle_targeting_data;
	uint16_t* issued_action_id;
	byte* issued_command_id;
	byte* issued_action_target_type;
	byte* issued_action_target_index;
	uint32_t field_load_models_atoi;
	uint32_t sub_6499F7;
	DWORD* input_ok_button_status;
	DWORD* input_run_button_status;

	// battle camera script externals
	uint32_t handle_camera_functions;
	uint32_t set_camera_focal_position_scripts;
	uint32_t set_camera_position_scripts;
	uint32_t add_fn_to_camera_fn_array;
	uint32_t execute_camera_functions;
	uint32_t battle_camera_sub_5C52F8;
	uint32_t battle_camera_sub_5C3E6F;
	uint32_t battle_camera_position_sub_5C3D0D;
	uint32_t battle_camera_position_sub_5C557D;
	uint32_t battle_camera_position_sub_5C5B9C;
	uint32_t battle_camera_focal_sub_5C5F5E;
	uint32_t battle_camera_focal_sub_5C5714;
	uint32_t battle_sub_430DD0;
	uint32_t battle_sub_429D8A;
	uint32_t update_battle_camera_sub_5C20CE;
	uint32_t set_battle_camera_sub_5C22BD;
	uint32_t battle_camera_sub_5C22A9;
	uint32_t compute_interpolation_to_formation_camera;
	uint32_t set_battle_camera_sub_5C2350;
	std::span<bcamera_fn_data> camera_fn_data;
	std::span<bcamera_position> battle_camera_position;
	std::span<bcamera_position> battle_camera_focal_point;
	std::span<uint32_t> camera_fn_array;
	byte* battle_camera_focal_scripts_8FEE30;
	byte* battle_camera_position_scripts_8FEE2C;
	DWORD* battle_camera_global_scripts_9A13BC;
	DWORD* battle_camera_position_scripts_9010D0;
	DWORD* battle_camera_focal_scripts_901270;
	byte* battle_camera_script_index;
	DWORD* battle_camera_script_offset;
	WORD* camera_fn_index;
	WORD* camera_fn_counter;
	vector3<short>* g_battle_camera_position;
	vector3<short>* g_battle_camera_focal_point;
	std::span<formation_camera> formation_camera;
	byte* curr_formation_camera_idx;
	byte* battle_enter_frames_to_wait;
	byte* g_variation_index;
	byte* is_camera_moving_BFB2DC;

	// animation script externals
	uint32_t battle_sub_42A5EB;
	uint32_t battle_sub_42E275;
	uint32_t battle_sub_42E34A;
	uint32_t battle_sub_5B9EC2;
	uint32_t battle_sub_5BD5E9;
	uint32_t run_summon_animations_script_5C1B81;
	uint32_t run_summon_animations_script_sub_5C1D9A;
	uint32_t run_animation_script;
	uint32_t add_fn_to_effect100_fn;
	uint32_t execute_effect100_fn;
	uint32_t add_fn_to_effect60_fn;
	uint32_t execute_effect60_fn;
	uint32_t add_fn_to_effect10_fn;
	uint32_t execute_effect10_fn;
	uint32_t battle_enemy_death_5BBD24;
	uint32_t battle_enemy_death_sub_5BBE32;
	uint32_t battle_iainuki_death_5BCAAA;
	uint32_t battle_iainuki_death_sub_5BCBB8;
	uint32_t battle_boss_death_5BC48C;
	uint32_t battle_boss_death_sub_5BC6ED;
	uint32_t battle_boss_death_sub_5BC5EC;
	uint32_t battle_boss_death_call_5BD436;
	uint32_t battle_melting_death_5BC21F;
	uint32_t battle_melting_death_sub_5BC32D;
	uint32_t battle_disintegrate_2_death_5BBA82;
	uint32_t battle_disintegrate_2_death_sub_5BBBDE;
	uint32_t battle_morph_death_5BC812;
	uint32_t battle_morph_death_sub_5BC920;
	uint32_t battle_disintegrate_1_death_5BBF31;
	uint32_t battle_disintegrate_1_death_sub_5BC04D;
	uint32_t battle_sub_42C0A7;
	uint32_t run_summon_animations_5C0E4B;
	uint32_t vincent_limit_fade_effect_sub_5D4240;
	uint32_t battle_sub_5BD96D;
	uint32_t battle_sub_425D29;
	uint32_t display_battle_damage_5BB410;
	uint32_t battle_sub_5BDA0F;
	uint32_t get_n_frames_display_action_string;
	uint32_t battle_sub_434C8B;
	uint32_t battle_sub_435D81;
	uint32_t battle_sub_426DE3;
	uint32_t battle_sub_426941;
	uint32_t battle_sub_426899;
	uint32_t battle_sub_4267F1;
	uint32_t battle_sub_5C1C8F;
	uint32_t battle_move_character_sub_426A26;
	uint32_t battle_move_character_sub_42739D;
	uint32_t battle_move_character_sub_426F58;
	uint32_t battle_move_character_sub_4270DE;
	uint32_t handle_aura_effects_425520;
	uint32_t run_aura_effects_5C0230;
	uint32_t limit_break_aura_effects_5C0572;
	uint32_t enemy_skill_aura_effects_5C06BF;
	uint32_t handle_summon_aura_5C0850;
	uint32_t summon_aura_effects_5C0953;
	uint32_t battle_sub_5C18BC;
	uint32_t battle_sub_4276B6;
	uint32_t battle_sub_4255B7;
	uint32_t battle_sub_425E5F;
	uint32_t battle_sub_5BCF9D;
	uint32_t battle_sub_425AAD;
	uint32_t battle_sub_427A6C;
	uint32_t battle_sub_427AF1;
	uint32_t battle_sub_427737;
	uint32_t battle_sub_4277B1;
	uint32_t battle_sub_5BCD42;
	uint32_t battle_sub_5BD050;
	uint32_t battle_smoke_move_handler_5BE4E2;
	uint32_t battle_sub_42A72D;
	void (*battle_play_sfx_sound_430D32)(uint16_t, short, char);
	uint32_t run_tifa_limit_effects;
	uint32_t tifa_limit_1_2_sub_4E3D51;
	uint32_t tifa_limit_2_1_sub_4E48D4;
	uint32_t aerith_limit_2_1_sub_45B0CF;
	uint32_t cloud_limit_2_2_sub_467256;
	uint32_t vincent_limit_satan_slam_camera_45CF2A;
	uint32_t barret_limit_4_1_camera_4688A2;
	uint32_t barret_limit_4_1_model_movement_4698EF;
	int *barret_limit_4_1_actor_id;
	uint32_t aerith_limit_4_1_camera_473CC2;
	uint32_t run_chocomog_movement_50B1A3;
	uint32_t run_chocomog_camera_509B10;
	uint32_t run_fat_chocobo_movement_509692;
	uint32_t run_fat_chocobo_camera_507CA4;
	uint32_t run_fat_chocobo_camera_shake_5095F5;
	uint32_t run_shiva_movement_592538;
	uint32_t run_shiva_camera_58E60D;
	uint32_t run_ifrit_movement_596702;
	uint32_t run_ifrit_camera_592A36;
	uint32_t run_ramuh_camera_597206;
	uint32_t run_titan_camera_59B4B0;
	uint32_t run_odin_gunge_movement_4A584D;
	uint32_t run_odin_gunge_camera_4A0F52;
	uint32_t run_odin_steel_movement_4A6CB8;
	uint32_t run_odin_steel_sub_4A9908;
	uint32_t run_odin_steel_camera_4A5D3C;
	uint32_t run_leviathan_camera_5B0716;
	uint32_t run_bahamut_movement_49ADEC;
	uint32_t run_bahamut_camera_497A37;
	uint32_t run_kujata_camera_4F9A4D;
	uint32_t run_alexander_movement_5078D8;
	uint32_t run_alexander_camera_501637;
	uint32_t run_phoenix_main_loop_516297;
	uint32_t run_phoenix_movement_518AFF;
	uint32_t run_phoenix_camera_515238;
	uint32_t run_bahamut_neo_main_48C2A1;
	uint32_t run_bahamut_neo_movement_48D7BC;
	uint32_t run_bahamut_neo_camera_48C75D;
	uint32_t run_hades_camera_4B65A8;
	uint32_t run_typhoon_camera_4D594C;
	uint32_t run_typhoon_sub_4DA182;
	uint32_t run_bahamut_zero_main_loop_484A16;
	uint32_t run_bahamut_zero_movement_48BBFC;
	uint32_t run_bahamut_zero_camera_483866;
	uint32_t bahamut_zero_draw_bg_effect_sub_4859AA;
	uint32_t bahamut_zero_bg_star_graphics_data_7F6748;
	uint32_t run_summon_kotr_sub_476857;
	uint32_t run_summon_kotr_main_loop_478031;
	std::array<uint32_t, 13> run_summon_kotr_knight_script;
	void(*add_kotr_camera_fn_to_effect100_fn_476AAB)(DWORD, DWORD, WORD);
	uint32_t run_kotr_camera_476AFB;
	vector3<int>* (*battle_sub_661000)(int);
	void (*engine_set_game_engine_rot_matrix_663673)(rotation_matrix*);
	void (*engine_set_game_engine_position_663707)(rotation_matrix*);
	void (*engine_apply_translation_with_delta_662ECC)(vector3<short>*, vector3<int>*, int*);
	uint32_t run_chocobuckle_main_loop_560C32;
	uint32_t run_confu_main_loop_5600BE;
	uint32_t bomb_blast_black_bg_effect_537427;
	uint32_t goblin_punch_flash_573291;
	uint32_t roulette_skill_main_loop_566287;
	uint32_t death_sentence_main_loop_5661A0;
	uint32_t death_kill_sub_loop_562C60;
	uint32_t death_kill_sub_loop_5624A5;
	uint32_t enemy_atk_camera_sub_439EE0;
	uint32_t enemy_atk_camera_sub_44A7D2;
	uint32_t enemy_atk_camera_sub_44EDC0;
	uint32_t enemy_atk_camera_sub_4522AD;
	uint32_t enemy_atk_camera_sub_457C60;
	uint32_t battle_update_3d_model_data;
	uint32_t battle_animate_material_texture;
	uint32_t battle_animate_texture_spt;
	rotation_matrix* (*get_global_model_matrix_buffer_66100D)();
	struc_84* (*get_draw_chain_68F860)(struc_49*, graphics_instance*);
	p_hundred* (*battle_sub_5D1AAA)(int, ff7_polygon_set*);
	int (*get_alpha_from_transparency_429343)(int);
	color_ui8 (*get_stored_color_66101A)();
	void (*battle_sub_68CF75)(char, struc_173*);
	void (*create_rot_matrix_from_word_matrix_6617E9)(rotation_matrix*, matrix*);
	struc_84* (*get_draw_chain_671C71)(ff7_graphics_object*);
	void (*battle_sub_6CE81E)();
	std::span<battle_model_state> g_battle_model_state;
	std::span<battle_model_state_small> g_small_battle_model_state;
	std::span<uint32_t> effect100_array_fn;
	std::span<effect100_data> effect100_array_data;
	uint16_t* effect100_counter;
	uint16_t* effect100_array_idx;
	std::span<uint32_t> effect60_array_fn;
	std::span<effect60_data> effect60_array_data;
	uint16_t* effect60_counter;
	uint16_t* effect60_array_idx;
	std::span<uint32_t> effect10_array_fn;
	std::span<effect10_data> effect10_array_data;
	uint16_t* effect10_counter;
	uint16_t* effect10_array_idx;
	short* effect10_array_data_8FE1F6;
	std::array<byte*, 14> animation_script_pointers;
	byte* g_is_effect_loading;
	byte* g_is_battle_paused;
	byte* g_actor_idle_scripts;
	byte* g_script_wait_frames;
	std::span<int*> g_script_args;
	byte* special_actor_id;
	int* field_battle_BFB2E0;
	float* field_float_battle_7B7680;
	byte* field_byte_DC0E11;
	byte* field_battle_byte_BF2E1C;
	byte* field_battle_byte_BE10B4;
	short* resting_Y_array_data;
	WORD* field_odin_frames_AEEC14;
	palette_extra* palette_extra_data_C06A00;
	ff7_game_engine_data** global_game_engine_data;
	std::span<uint32_t> limit_break_effects_fn_table;
	std::span<uint32_t> enemy_atk_effects_fn_table;
	std::span<uint32_t> enemy_skill_effects_fn_table;
	byte* byte_BCC788;
	vector3<int>** ifrit_vector3_int_ptr_BCC6A8;
	vector3<short>* battle_ifrit_model_position;
	rotation_matrix* ifrit_rot_matrix_BCC768;
	uint32_t pollensalta_cold_breath_atk_enter_sub_5474F0;
	uint32_t pollensalta_cold_breath_atk_main_loop_5476B0;
	uint32_t pollensalta_cold_breath_atk_draw_bg_effect_547B94;
	uint32_t pollensalta_cold_breath_atk_white_dot_effect_547D56;
	void (*pollensalta_cold_breath_atk_draw_white_dots_547E75)(short);
	std::span<vector4<short>> pollensalta_cold_breath_white_dots_pos;
	short* pollensalta_cold_breath_white_dot_rgb_scalar;
	uint32_t pollensalta_cold_breath_bg_texture_ctx;
	uint32_t pandora_box_skill_draw_bg_flash_effect_568371;

	// battle menu
	uint32_t display_battle_menu_6D797C;
	void (*display_tifa_slots_handler_6E3135)();
	uint32_t battle_draw_text_ui_graphics_objects_call;
	uint32_t battle_draw_box_ui_graphics_objects_call;
	void (*battle_draw_call_42908C)(int, int);
	uint32_t battle_set_do_render_menu_call;
	uint32_t battle_set_do_render_menu;
	int *g_do_render_menu;
	uint32_t battle_sub_42F3E8;
	uint32_t battle_handle_player_mark_5B9C8E;
	uint32_t battle_handle_status_effect_anim_5BA7C0;
	uint32_t battle_update_targeting_info_6E6291;
	byte *targeting_actor_id_DC3C98;
	uint32_t battle_menu_closing_window_box_6DAEF0;

	//battle 3d battleground
	uint32_t update_3d_battleground;
	void (*battleground_shake_train_42F088)();
	uint32_t battleground_vertical_scrolling_42F126;
	uint32_t battleground_midgar_flashback_rain_5BDC4F;

	// battle dialogue
	uint32_t battle_sub_42CBF9;
	uint32_t add_text_to_display_queue;
	uint32_t update_display_text_queue;
	uint32_t set_battle_text_active;
	uint32_t battle_sfx_play_effect_430D14;
	int (*battle_sub_66C3BF)();
	uint32_t battle_sub_43526A;
	uint32_t battle_sub_5C8931;
	uint32_t run_enemy_ai_script;
	uint32_t enqueue_script_action;
	uint32_t battle_sub_41B577;
	uint32_t battle_sub_41CCB2;
	std::span<battle_text_data> battle_display_text_queue;
	battle_ai_context *battle_context;
	std::span<battle_anim_event> anim_event_queue;
	byte* anim_event_index;
	int* g_is_battle_running_9AD1AC;
	WORD* field_battle_word_BF2E08;
	WORD* field_battle_word_BF2032;
	byte* g_active_actor_id;

	// world stuff
	uint32_t world_mode_loop_sub_74DB8C;
	uint32_t world_exit_74BD77;
	void (*world_exit_destroy_graphics_objects_75A921)();
	uint32_t world_init_variables_74E1E9;
	uint32_t world_sub_7641A7;
	void (*world_init_load_wm_bot_block_7533AF)();
	uint32_t run_world_event_scripts;
	uint32_t run_world_event_scripts_system_operations;
	uint32_t world_animate_all_models;
	uint32_t world_animate_single_model;
	uint32_t run_world_snake_ai_script_7562FF;
	uint32_t update_world_snake_position_7564CD;
	uint32_t is_update_snake_enabled_7562A9;
	uint32_t animate_world_snake_75692A;
	bool (*sub_753366)(short, short);
	void (*world_draw_snake_texture_75D544)(short, short, short, short, world_snake_graphics_data*, short);
	vector4<short>** world_snake_data_position_ptr_E2A18C;
	vector4<short>* world_snake_data_position_E29F80;
	vector4<short>* snake_position_size_of_array_E2A100;
	world_snake_graphics_data* world_snake_graphics_data_E2A490;
	world_snake_graphics_data* world_snake_graphics_data_end_E2A6D0;
	uint32_t world_sub_75EF46;
	uint32_t world_sub_767540;
	uint32_t world_sub_767641;
	uint32_t world_opcode_message_sub_75EE86;
	uint32_t world_opcode_ask_sub_75EEBB;
	uint32_t world_opcode_message;
	uint32_t world_opcode_ask;
	uint32_t world_text_box_window_opening_769A66;
	uint32_t world_text_box_window_paging_769C02;
	uint32_t world_text_box_reverse_paging_76ABE9;
	uint32_t world_text_box_window_closing_76ADF7;
	uint32_t world_compute_all_models_data_76323A;
	uint32_t world_compute_3d_model_data_76328F;
	uint32_t world_sub_74D319;
	uint32_t world_sub_762F9A;
	int (*get_world_encounter_rate)();
	int (*pop_world_script_stack)();
	uint32_t world_update_player_74EA48;
	int (*world_get_player_model_id)();
	int (*world_get_current_key_input_status)();
	int (*world_get_player_walkmap_type)();
	void(*world_sub_753D00)(vector3<short>*, short);
	void(*world_update_model_movement_762E87)(int, int);
	bool (*world_is_player_model_bitmask)(int);
	void (*world_copy_player_pos_to_param_762798)(vector4<int>*);
	void (*world_set_current_entity_to_player_entity)();
	void (*world_add_y_pos_to_current_entity_761F22)(int);
	void (*world_add_delta_movement_due_to_bridge_7591C2)(int*, int*);
	void (*world_current_entity_model_collision_detection_with_other_models_76296E)();
	int (*world_get_unknown_flag_75335C)();
	short (*world_get_minimap_mask)();
	void (*world_set_minimap_mask)(short);
	void (*world_set_facing_and_direction_to_current_entity)(short);
	bool (*world_is_current_entity_animated_761F44)();
	void (*world_sub_74D6BB)();
	void (*world_sub_74D6F6)();
	void (*world_sub_762F75)(short, short, short);
	void (*world_run_special_opcode_7640BC)(int);
	void (*world_set_camera_fade_speed_755B97)(int);
	void (*world_set_world_control_lock_74D438)(int, int);
	void (*world_sub_74C980)(int);
	void (*world_sub_753BE8)();
	void (*world_music_set_frequency_all_channels_75E6A8)(byte, char);
	void (*world_sfx_play_or_stop_75E6CC)(int);
	void (*world_set_camera_view_type_74D3D1)(int);
	uint32_t world_update_camera_74E8CE;
	int (*world_snowstorm_get_camera_movement_758B12)(int, int);
	int (*world_get_camera_rotation_x_74F916)();
	int* world_highwind_height_lowerbound_DF5420;
	int* world_mode_E045E4;
	int* previous_player_direction_DF5434;
	int* world_is_control_enabled_DE6B5C;
	short* world_special_delta_movement_DE6A18;
	int* world_y_player_pos_flag_DE6A14;
	int* world_unk_rotation_value_E045E0;
	world_event_data** world_event_current_entity_ptr_E39AD8;
	world_event_data** world_event_current_entity_ptr_E3A7CC;
	int* world_progress_E28CB4;
	int* is_wait_frames_zero_E39BC0;
	int* world_prev_key_input_status_DFC470;
	int* world_map_type_E045E8;
	int* world_movement_multiplier_DFC480;
	int* world_camera_var1_DF542C;
	int* world_camera_var2_DE6B4C;
	int* world_camera_var3_DE6A0C;
	int* world_camera_viewtype_DFC4B4;
	int* world_camera_front_DFC484;
	int* world_camera_rotation_y_DFC474;
	int* world_camera_position_z_DFC478;
	int* world_camera_delta_y_DE6A04;
	int* world_camera_rotation_z_DE6B70;
	short* world_current_camera_rotation_x_DE7418;
	std::span<short> world_camera_x_rotation_array_E37120;
	rotation_matrix* world_camera_position_matrix_DE6A20;
	rotation_matrix* world_camera_direction_matrix_DFC448;
	vector4<int>* world_player_pos_E04918;
	uint32_t world_sub_75A1C6;
	uint32_t world_load_graphics_objects_75A5D5;
	uint32_t world_init_load_map_meshes_graphics_objects_75A283;
	void (*world_wm0_overworld_draw_all_74C179)();
	void (*world_wm2_underwater_draw_all_74C3F0)();
	void (*world_wm3_snowstorm_draw_all_74C589)();
	uint32_t world_draw_all_3d_model_74C6B0;
	uint32_t world_draw_fade_quad_75551A;
	uint32_t world_sub_75079D;
	uint32_t world_sub_751EFC;
	uint32_t world_sub_75C02B;
	uint32_t world_sub_75C0FD;
	uint32_t world_sub_75C283;
	uint32_t world_sub_75F0AD;
	uint32_t world_sub_75042B;
	uint32_t world_culling_bg_meshes_75F263;
	uint32_t world_submit_draw_bg_meshes_75F68C;
	uint32_t world_compute_skybox_data_754100;
	uint32_t world_submit_draw_clouds_and_meteor_7547A6;
	int (*sub_74C9A5)();
	int* is_meteor_flag_on_E2AAE4;
	uint32_t engine_apply_4x4_matrix_product_with_game_obj_matrix_67D2BF;
	void (*engine_apply_4x4_matrix_product_between_matrices_66C6CD)(struct matrix *, struct matrix *, struct matrix *);
	void (*world_copy_position_75042B)(vector4<int>* a1);
	int (*get_world_camera_front_rot_74D298)();
	void (*engine_apply_rotation_to_rot_matrix_662AD8)(vector3<short>*, transform_matrix*);
	short (*world_get_world_current_camera_rotation_x_74D3C6)();
	int (_stdcall *world_submit_draw_effects_75C283)(world_texture_data*, int, vector3<short>*, short);
	world_effect_2d_list_node** dword_E35648;
	byte* byte_96D6A8;

	uint32_t swirl_main_loop;
	uint32_t swirl_loop_sub_4026D4;
	uint32_t swirl_enter_40164E;
	uint32_t swirl_enter_sub_401810;

	uint32_t field_culling_model_639252;
	uint32_t field_sub_63AC66;
	void (*field_sub_63AC3F)(int, int, int, int);
	uint32_t battle_draw_quad_5BD473;
	uint32_t battle_sub_5895E0;
	uint32_t battle_sub_589827;
	uint32_t battle_sub_58AC59;
	uint32_t battle_sub_58ACB9;
	uint32_t ifrit_sub_595A05;
	void (*engine_draw_sub_66A47E)(int);
	int* battle_viewport_height;
	uint32_t neo_bahamut_main_loop_48DA7A;
	uint32_t neo_bahamut_effect_sub_490F2A;
	uint32_t odin_gunge_effect_sub_4A4BE6;
	uint32_t odin_gunge_effect_sub_4A3A2E;
	uint32_t typhoon_effect_sub_4DB15F;
	uint32_t typhoon_sub_4D6FF8;
	uint32_t typhoon_effect_sub_4D7044;
	uint32_t fat_chocobo_sub_5096F3;
	uint32_t barret_limit_3_1_sub_4700F7;
	uint32_t shadow_flare_draw_white_bg_57747E;
	uint32_t credits_submit_draw_fade_quad_7AA89B;
	uint32_t menu_submit_draw_fade_quad_6CD64E;
	int (*get_button_pressed)(int);
	uint32_t credits_main_loop;
	uint32_t highway_submit_fade_quad_659532;
	uint32_t chocobo_enter_76D597;
	uint32_t chocobo_initialize_variables_76BAFD;
	uint32_t chocobo_init_viewport_values_76D320;
	uint32_t chocobo_submit_draw_fade_quad_77B1CE;
	uint32_t chocobo_submit_draw_water_quad_77A7D0;
	void(*generic_submit_quad_graphics_object_671D2A)(int, int, int, int, int, int, float, DWORD*) ;
	byte* chocobo_fade_quad_data_97A498;

	// snowboard
	uint32_t snowboard_enter_sub_722C10;
	uint32_t snowboard_loop_sub_72381C;
	uint32_t snowboard_exit_sub_722C52;
	uint32_t snowboard_draw_sky_and_mountains_72DAF0;
	uint32_t snowboard_submit_draw_sky_quad_graphics_object_72E31F;
	float* snowboard_sky_quad_pos_x_7B7DB8;
	uint32_t snowboard_submit_draw_black_quad_graphics_object_72DD94;
	uint32_t snowboard_submit_draw_white_fade_quad_graphics_object_72DD53;
	uint32_t snowboard_submit_draw_opaque_quad_graphics_object_72DDD5;
	uint32_t snowboard_parse_model_vertices_732159;
	uint32_t sub_735220;
	uint32_t sub_735332;
	char* (*sub_7322D6)(tmd_primitive_packet*, int, int);
	char* (*sub_732429)(tmd_primitive_packet*, int, int);
	char* (*sub_732BB9)(tmd_primitive_packet*, int, int);
	char* (__thiscall *sub_732546)(snowboard_this*, tmd_primitive_packet*, int, int);
	matrix* (__thiscall *sub_733479)(void*, const matrix*);
	point4d* (__thiscall *sub_733564)(void*, vector3<float>*, point4d*);
	DWORD* snowboard_global_object_off_926290;

	// condor
	uint32_t condor_enter;
	uint32_t condor_exit;
	uint32_t sub_5F7756;
	uint32_t sub_5F4273;
	uint32_t sub_5F342C;
	DWORD* condor_uses_lgp;

    // japanese
	int* field_text_box_curr_n_characters_DC3CB0;
	int* field_text_line_row_DC3CB8;
	short* field_remaining_character_length_DC3CCC;
	int* field_current_window_pos_x_DC3CB4;
	short* word_91F028;
	short* word_DC3CC0;
	short* word_DC3CC4;
	int* dword_DC3CD4;
	short* word_DC3CC8;
	int* g_text_spacing_DB958C;
	ff7_graphics_object** menu_font_a_graphics_object_DC100C;
	ff7_graphics_object** menu_font_b_graphics_object_DC1010;
	ff7_graphics_object** menu_win_a_blend_4_graphics_object_DC0FC8;
	ff7_graphics_object** menu_win_b_blend_4_graphics_object_DC0FCC;
	ff7_graphics_object** menu_win_c_blend_4_graphics_object_DC0FD0;
	ff7_graphics_object** menu_win_d_blend_4_graphics_object_DC0FD4;
	ff7_graphics_object** menu_win_a_blend_0_graphics_object_DC0FDC;
	ff7_graphics_object** menu_win_a_blend_1_graphics_object_DC0FE0;
	ff7_graphics_object** menu_win_b_blend_1_graphics_object_DC0FE4;
	ff7_graphics_object** menu_win_c_blend_1_graphics_object_DC0FE8;
	ff7_graphics_object** menu_win_d_blend_1_graphics_object_DC0FEC;
	ff7_graphics_object** menu_font_blend_4_graphics_object_DC1048;
	ff7_graphics_object** menu_win_blend_4_graphics_object_DC104C;
	ff7_graphics_object** menu_win_blend_0_graphics_object_DC1050;
	ff7_graphics_object** menu_win_blend_1_graphics_object_DC1054;
	ff7_graphics_object** menu_win_c_blend_4_diff_graphics_object_DC0FD8;

	unsigned int* unk_DC1074;
	char* aUsfont_a_h_tim;
	char* aUsfont_a_l_tim;
	char* aUsfont_b_h_tim;
	char* aUsfont_b_l_tim;
	char* aBtl_win_a_h_ti;
	char* aBtl_win_a_l_ti;
	char* aBtl_win_b_h_ti;
	char* aBtl_win_b_l_ti;
	char* aBtl_win_c_h_ti;
	char* aBtl_win_c_l_ti;
	char* aBtl_win_d_h_ti;
	char* aBtl_win_d_l_ti;
	char* aUsfont_h_tim;
	char* aUsfont_l_tim;
	char* aBtl_win_h_tim;
	char* aBtl_win_l_tim;
	
	int* field_do_draw_character_DC3CEC;
	int* field_do_draw_text_boxes_DC3CE8;

	uint32_t engine_loop_main_loop_sub_4090E6;
	uint32_t menu_enter_sub_6CD3B0;
	void (*engine_load_menu_graphics_objects_6C1468)(int);
	void (*sub_671082)(ff7_graphics_object**);
	int (*sub_674530)();
	void (*sub_67453A)(int);
	void (*make_struc3_6745E6)(int, struc_3*);
	void (*engine_set_blendmode_674659)(int, struc_3 *);
	ff7_graphics_object* (*engine_load_graphics_object_6710AC)(int, signed int, struc_3*, char*, int);
	int (*engine_get_viewport_type_404D80)();
	ff7_game_obj* (*engine_get_game_object_676578)();

	uint32_t field_draw_everything_sub_63A60B;
	uint32_t field_submit_and_draw_text_box_and_text_6EBF2C;
	uint32_t field_submit_draw_text_640x480_6E706D;

	void (*field_draw_text_boxes_and_text_graphics_object_6ECA68)();
	void (*engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95)(int, ff7_game_obj*);
	void (*engine_gfx_set_single_renderstate_sub_660C3A)(int, int, ff7_game_obj*);
	void (*engine_draw_graphics_object_66E641)(ff7_graphics_object*, ff7_game_obj*);
	void (*reset_field_54_graphics_object_66E62C)(ff7_graphics_object*);

	int* menu_is_small_viewport_320_240_DC130C;
	int* dword_DC3CE0;
	int* dword_DC3CDC;
	ff7_graphics_object** menu_window_bg_graphics_object_DC0FF0;	
	ff7_graphics_object** menu_blend_window_bg_graphics_object_DC0FF4;
	int* text_box_do_draw_menu_win_c_blend_4_DC3CE4;
	int* text_box_do_draw_black_quad_graphics_object_DC3CF0;
	ff7_graphics_object** menu_text_box_quad_graphics_object_DC1008;
	int* dword_DC3D00;
	int* do_draw_text_box_DC3CF8;
	int* should_draw_text_box_black_quad_DC3D04;



	uint32_t battle_menu_display_menu_6D82EA;
	uint32_t battle_display_base_menu_6DD041;
	int (*common_submit_draw_char_from_buffer_6F564E)(int, int, int, unsigned __int16, float);
	uint32_t menu_loop_sub_6CC623;
	void (*menu_draw_everything_6CC9D3)();
	int (*g_get_do_render_menu_6CDBF2)();

	int* dword_DC12DC;

	void (*engine_gfx_setviewport_sub_66067A)(unsigned int, unsigned int, unsigned int, unsigned int, ff7_game_obj*);

	unsigned int* menu_viewport_x_DC105C;
	unsigned int* menu_viewport_y_DC1060;
	unsigned int* menu_viewport_width_DC1064;
	unsigned int* menu_viewport_view_DC1068;

	void (*engine_gfx_draw_graphics_object_polygon_set_field_80_sub_660E6A)(ff7_graphics_object*, ff7_game_obj*);
	ff7_graphics_object** menu_unknown3_graphics_object_DC0FFC;
	int* dword_DC12EC;
	int* dword_DC12E4;
	ff7_graphics_object** menu_avatar2_1_graphics_object_DC1020;
    ff7_graphics_object** menu_avatar2_2_graphics_object_DC1024;
    ff7_graphics_object** menu_avatar2_3_graphics_object_DC1028;
    ff7_graphics_object** menu_avatar2_4_graphics_object_DC102C;
    ff7_graphics_object** menu_avatar2_5_graphics_object_DC1030;
    ff7_graphics_object** menu_avatar2_6_graphics_object_DC1034;
    ff7_graphics_object** menu_avatar2_7_graphics_object_DC1038;
    ff7_graphics_object** menu_avatar2_8_graphics_object_DC103C;
    ff7_graphics_object** menu_avatar2_9_graphics_object_DC1040;
	ff7_graphics_object** menu_avatar_1_graphics_object_DC1014;
    ff7_graphics_object** menu_avatar_2_graphics_object_DC1018;
    ff7_graphics_object** menu_avatar_3_graphics_object_DC101C;
	short* engine_game_mode_word_CBF9DC;
	ff7_graphics_object** menu_buster_tex_graphics_object_DC1044;
	ff7_graphics_object** menu_unknown4_graphics_object_DC1000;
	ff7_graphics_object** menu_unknown2_graphics_object_DC0FF8;

	uint32_t battle_loop_sub_41BAB3;
	void (*battle_draw_menu_everything_6CEE84)();

	ff7_graphics_object** menu_unknown5_graphics_object_DC1004;

	uint32_t battle_display_text_6D7245;
	void (*draw_text_top_display_6D1CC0)(int, __int16, char, unsigned __int16);
	attack_name_fixed_buffer* (*kernel_get_text_sub_41963C)(unsigned int, int, int);
	char (*sub_6D70F1)(int);

	battle_menu_data** battle_menu_data_DC3630;
	char* byte_DC3640;
	attack_name_fixed_buffer** battle_text_buffer_DC208C;
	battle_graphics_data** battle_graphics_data_ptr_9ADFD8;
	char* g_is_battle_paused_DC0E6C;
	bgra_byte* dword_91EFC8;
	bgra_byte* dword_91EFCC;
	bgra_byte* dword_91EFD0;
	bgra_byte* dword_91EFD4;

	uint32_t engine_field_A58_sub_6C0E2D;
	void (*main_menu_draw_everything_maybe_6C0B91)();

	text_box_data* text_box_window_data_array_CFF5B8;
	byte* field_text_box_window_entity_id_CC0960;
	byte* current_entity_id_byte_CC0964;

	// JP Font graphics objects
	ff7_graphics_object* menu_jafont_1_graphics_object;
	ff7_graphics_object* menu_jafont_2_graphics_object;
	ff7_graphics_object* menu_jafont_3_graphics_object;
	ff7_graphics_object* menu_jafont_4_graphics_object;
	ff7_graphics_object* menu_jafont_5_graphics_object;
	ff7_graphics_object* menu_jafont_6_graphics_object;
};

uint32_t ff7gl_load_group(uint32_t group_num, struct matrix_set *matrix_set, struct p_hundred *hundred_data, struct p_group *group_data, struct polygon_data *polygon_data, struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
void ff7gl_field_78(struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
void ff7_init_hooks(struct game_obj *_game_object);
struct ff7_gfx_driver *ff7_load_driver(void *game_object);
