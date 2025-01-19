/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <dinput.h>

#include "common_imports.h"

// FF7 modules, unknowns are either unused or not relevant to rendering
enum ff8_game_modes
{
	FF8_MODE_CREDITS = 0,
	FF8_MODE_FIELD,
	FF8_MODE_WORLDMAP,
	FF8_MODE_SWIRL,
	FF8_MODE_AFTER_BATTLE,
	FF8_MODE_5,
	FF8_MODE_MENU,
	FF8_MODE_7,
	FF8_MODE_CARDGAME, // And battle
	FF8_MODE_9,
	FF8_MODE_TUTO,
	FF8_MODE_11,
	FF8_MODE_INTRO,
	FF8_MODE_100 = 100,
	FF8_MODE_MAIN_MENU = 200,
	FF8_MODE_BATTLE = 999
};

/*
 * This section defines some structures used internally by the FF8 game engine.
 *
 * Documentation for some of them can be found on the Qhimm wiki, a lot of
 * information can be gleaned from the source code to this program but in many
 * cases nothing is known except the size and general layout of the structure.
 *
 * Variable and structure names are mostly based on what they contain rather
 * than what they are for, a lot of names may be wrong, inappropriate or
 * downright misleading. Thread with caution!
 */

struct wm_polygon_source
{
	WORD unknown1;
	WORD unknown2;
	WORD unknown3;
	unsigned char u0;
	unsigned char v0;
	unsigned char u1;
	unsigned char v1;
	unsigned char u2;
	unsigned char v2;
};

struct ssigpu_packet52
{
	uint32_t unknown1;
	unsigned char b0;
	unsigned char g0;
	unsigned char r0;
	unsigned char command;
	WORD y0;
	WORD x0;
	unsigned char v0;
	unsigned char u0;
	WORD clut;
	unsigned char b1;
	unsigned char g1;
	unsigned char r1;
	unsigned char unused1;
	WORD y1;
	WORD x1;
	unsigned char v1;
	unsigned char u1;
	WORD texture;
	unsigned char b2;
	unsigned char g2;
	unsigned char r2;
	unsigned char unused2;
	WORD y2;
	WORD x2;
	unsigned char v2;
	unsigned char u2;
	WORD unused3;
};

struct psxvram_rect
{
	WORD x;
	WORD y;
	WORD w;
	WORD h;
};

struct sprite_viewport
{
	float width;
	float height;
	float field_8;
	float field_C;
	float scale_x;
	float scale_y;
	float offset_x;
	float offset_y;
};

struct font_object
{
	uint32_t dummy1[0x12];
	struct ff8_graphics_object *font_a;
	struct ff8_graphics_object *font_b;
};

struct struc_38
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	struct ff8_graphics_object *graphics_object;
};

struct ff8_file_fi_infos
{
	int size;
	int pos;
	int compression;
};

struct ff8_file_context
{
	int mode; // 0= _O_BINARY, 1= _O_TEXT, 2= _O_BINARY | _O_CREAT | _O_RDWR, 3= _O_BINARY | _O_CREAT | _O_TRUNC | _O_RDWR
	int field_4;
	char* field_8;
	void (*filename_callback)(const char*, char*);
	struct ff8_file_container* file_container;
};

struct ff8_file_fl
{
	int file_count;
	char **filenames_pointers;
	int filenames_data_size;
	char* filenames_data;
	int callback;
};

struct ff8_file
{
	int is_open;
	const char* filename;
	int fd;
	struct ff8_file_context file_context;
	int field_20;
	int current_pos;
	struct ff8_file_fi_infos fi_infos;
	struct ff8_file_container* file_container;
};

struct ff8_file_container
{
	int initialized;
	int first_fi_info_position;
	struct ff8_file* fs_disk_file_metadata;
	struct ff8_file_fl* fl_infos;
	struct ff8_file_fi_infos* fi_infos;
};

struct ff8_indexed_vertices
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
	struct ff8_graphics_object *graphics_object;
};

struct ff8_graphics_object
{
	uint32_t type;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t has_texture_set;
	struct p_hundred *hundred_data;
	struct matrix_set *matrix_set;
	struct polygon_set *polygon_set;
	uint32_t is_tim_header;
	uint32_t field_20;
	uint32_t field_24;
	float u_offset;
	float v_offset;
	void *dx_sfx_2C;
	void *tex_info_filename_and_more;
	uint32_t field_38;
	uint32_t vertices_per_shape;
	uint32_t indices_per_shape;
	uint32_t vertex_offset;
	uint32_t index_offset;
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
	uint32_t field_7C;
	uint32_t field_80;
	uint32_t field_84;
	uint32_t field_88;
	uint32_t field_8C;
	struct ff8_indexed_vertices *indexed_vertices;
	uint32_t field_94;
	uint32_t field_98;
	gfx_polysetrenderstate* setrenderstate;
	gfx_draw_vertices* draw;
	uint32_t use_matrix_pointer;
	struct matrix* matrix_pointer;
	struct matrix matrix;
};

struct ff8_polygon_set
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t numgroups;
	struct struc_49 field_14;
	uint32_t field_2C;
	struct polygon_data *polygon_data;
	struct p_hundred *hundred_data;
	uint32_t per_group_hundreds;
	struct p_hundred **hundred_data_group_array;
	struct matrix_set *matrix_set;
	uint32_t field_44;
	uint32_t field_48;
	void *execute_buffers;			// IDirect3DExecuteBuffer **
	struct indexed_primitive **indexed_primitives;
	uint32_t field_54;
	uint32_t field_58;
	struct p_group *polygon_group_array;
	// unknown up to 0xB0
};

struct ff8_tex_header
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
	uint32_t field_D4;
	unsigned char *image_data;
	unsigned char *old_palette_data;
	uint32_t field_DC;
	uint32_t field_E0;
	uint32_t *vram_positions;
	uint32_t y;
};

struct ff8_texture_set
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
	uint32_t dummy1[5];
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

struct struc_color_texture {
		uint32_t field_0;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t _padding;
		int16_t x;
		int16_t y;
		int16_t w;
		int16_t h;
};

struct texture_page
{
	uint32_t field_0;
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t color_key;
	float u;
	float v;
	uint32_t field_20;
	struct ff8_graphics_object *tri_gfxobj;
	struct ff8_graphics_object *quad_gfxobj;
	struct ff8_graphics_object *noblend_tri_gfxobj;
	struct ff8_graphics_object *noblend_quad_gfxobj;
	struct ff8_graphics_object *avg_tri_gfxobj;
	struct ff8_graphics_object *avg_quad_gfxobj;
	struct ff8_graphics_object *add_tri_gfxobj;
	struct ff8_graphics_object *add_quad_gfxobj;
	struct ff8_graphics_object *sub_tri_gfxobj;
	struct ff8_graphics_object *sub_quad_gfxobj;
	struct ff8_graphics_object *mode3_tri_gfxobj;
	struct ff8_graphics_object *mode3_quad_gfxobj;
	struct ff8_tex_header *tex_header;
	char *image_data;
	struct ff8_tex_header* sub_tex_header;
	char* sub_image_data;
};

struct struc_50
{
	uint32_t initialized;
	struct texture_page texture_page[8];
	uint32_t texture_page_enabled;
	uint32_t field_328;
	uint32_t vram_needs_reload;
	uint32_t palette_index;
	char dummy[256];
	uint32_t vram_x;
	uint32_t vram_y;
	uint32_t vram_width;
	uint32_t vram_height;
	uint16_t *vram_palette_data;
	uint16_t vram_palette_pos; // 10-bit | 6-bit
	uint16_t padding;
};

struct struc_51
{
	struct struc_50 struc_50_array[32];
};

struct ff8_tim
{
	uint16_t img_x;
	uint16_t img_y;
	uint16_t img_w;
	uint16_t img_h;
	uint8_t *img_data;
	uint16_t pal_x;
	uint16_t pal_y;
	uint16_t pal_w;
	uint16_t pal_h;
	uint16_t *pal_data;
};

struct ff8_camdata
{
	// EYE
	int16_t eye_x;
	int16_t eye_y;
	int16_t eye_z;
	// TARGET
	int16_t target_x;
	int16_t target_y;
	int16_t target_z;
	// UP
	int16_t up_x;
	int16_t up_y;
	int16_t up_z;
	// FILLER?
	int16_t padding;
	// POSITION
	int32_t pos_x;
	int32_t pos_y;
	int32_t pos_z;
	// PAN
	int16_t pan_x;
	int16_t pan_y;
	// ZOOM
	int16_t zoom;
	// FILLER?
	int16_t padding2;
	// CONTROL
	uint8_t flag;
	// MARKER 'END'
	uint8_t end[3];
};

struct ff8_movie_obj
{
	WORD movie_current_frame;
	WORD movie_total_frames;
	uint32_t movie_surface_height;
	void *bink_struct;
	struct ff8_camdata *camdata_start;
	unsigned char camdata_buffer[312320];
	unsigned char movie_surface_desc[0x7C];
	void *movie_back_surface;
	uint32_t movie_surface_x;
	struct ff8_camdata *camdata_pointer;
	uint32_t movie_surface_y;
	void *movie_dd_surface;
	struct ff8_game_obj *movie_game_object;
	uint32_t movie_intro_pak;
	uint32_t movie_is_playing;
	uint32_t field_4C4AC;
	uint32_t field_4C4B0;
	uint32_t field_4C4B4;
	uint32_t movie_resolution;
	HANDLE movie_file_handle;
	uint32_t bink_copy_flags;
};

struct ff8_win_obj
{
	uint32_t x;
	uint32_t y;
	char *text_data1;
	char *text_data2;
	uint16_t field_10;
	uint16_t field_12;
	uint16_t field_14;
	uint8_t field_16;
	uint8_t field_17;
	uint8_t win_id;
	uint8_t field_19;
	uint16_t mode1;
	int16_t open_close_transition;
	int16_t field_1E;
	uint32_t field_20;
	uint32_t state;
	uint8_t field_28;
	uint8_t first_question;
	uint8_t last_question;
	uint8_t current_choice_question;
	uint8_t field_2C;
	uint8_t field_2D;
	uint8_t field_2E;
	uint8_t field_2F;
	uint16_t field_30;
	uint8_t field_32;
	uint8_t field_33;
	uint32_t callback1;
	uint32_t callback2;
};

struct ff8_gamepad_vibration_state_entry {
	uint8_t analog_disabled;
	uint8_t analog_flags;
	int16_t keyscan;
	uint8_t analog_rx;
	uint8_t analog_ry;
	uint8_t analog_lx;
	uint8_t analog_ly;
	int16_t field_8;
	int16_t field_A;
	int16_t field_C;
	int16_t field_E;
	int16_t keyon;
	int16_t keyscan_invert;
};

struct ff8_gamepad_vibration_state {
	uint8_t field_0;
	uint8_t field_1;
	uint8_t field_2;
	uint8_t field_3;
	uint16_t field_4;
	uint8_t right_motor_speed;
	uint8_t left_motor_speed;
	uint8_t field_8;
	uint8_t field_9;
	uint8_t vibration_active;
	uint8_t vibration_capable;
	uint8_t field_C[4];
	uint8_t field_10[4];
	uint32_t field_14;
	uint8_t entries_offset;
	uint8_t field_19;
	uint8_t field_1A;
	uint8_t vibrate_option_enabled;
	ff8_gamepad_vibration_state_entry entries[8];
	uint32_t field_BC;
	uint16_t field_C0;
	uint8_t port_id;
	uint8_t gamepad_options; // is_enabled (1-bit) | u40 | u20 | u10 | u8 | analog sensitivity (1 to 4, 3-bits)
};

struct ff8_driver_input_state {
	ff8_gamepad_vibration_state_entry entry;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
};

struct ff8_gamepad_state {
	ff8_gamepad_vibration_state state_by_port[2];
	ff8_driver_input_state driver_state_by_port[2];
};

struct ff8_vibrate_motor_struc
{
	uint8_t *data_start;
	uint8_t *data_end;
	int16_t counter;
	uint16_t data_size;
	uint8_t enabled;
	uint8_t padding_D;
	uint8_t padding_E;
	uint8_t padding_F;
};

struct ff8_vibrate_struc
{
	ff8_vibrate_motor_struc sub_struc_left;
	ff8_vibrate_motor_struc sub_struc_right;
	uint16_t field_20;
	uint8_t intensity;
	uint8_t field_23;
};

struct ff8_menu_config_input {
	int16_t text_id_name;
	uint16_t text_id_value1;
	uint16_t text_id_value2;
	uint16_t type;
	uint16_t mask;
	uint16_t value;
	void(*callback_change_state)();
};

struct ff8_menu_config_input_keymap {
	uint8_t field_0;
	uint8_t field_1;
	uint8_t field_2;
	uint8_t field_3;
	uint8_t text_id;
	uint8_t padd_5;
	uint8_t padd_6;
	uint8_t padd_7;
};

struct ff8_draw_menu_sprite_texture_infos {
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint16_t x_related;
	uint16_t y_related;
	uint32_t field_10;
	uint32_t field_14;
};

struct ff8_draw_menu_sprite_texture_infos_short {
	uint32_t field_0;
	uint32_t field_4;
	uint16_t x_related;
	uint16_t y_related;
	uint32_t field_C;
	uint32_t field_10;
};

struct ff8_audio_fmt
{
	uint32_t length;
	uint32_t offset;
	uint8_t loop;
	uint8_t count;
	uint8_t unk1;
	uint8_t unk2;
	uint32_t loop_start;
	uint32_t loop_end;
	LPWAVEFORMATEX wave_format;
};

struct ff8_game_obj
{
	uint32_t do_quit;
	uint32_t dc_horzres;
	uint32_t dc_vertres;
	uint32_t dc_bitspixel;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
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
	void *front_surface[3];
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
	uint32_t dummy1[0xD];
	void *d3d2device;
	void *d3dviewport2;
	uint32_t dummy2[0x6];
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
	uint32_t dummy3[0xD];
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
	uint32_t dummy4[0x30];
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
	uint32_t dummy5[2];
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
	uint32_t field_BD4;
	uint32_t field_BD8;
	uint32_t field_BDC;
	uint32_t field_BE0;
	uint32_t field_BE4;
	uint32_t field_BE8;
	uint32_t no_8bit_textures;
	uint32_t field_BF0;
	uint32_t tnt_fix;
	uint32_t field_BF8;
	uint32_t field_BFC;
	uint32_t field_C00;
	uint32_t field_C04;
	uint32_t field_C08;
	uint32_t field_C0C;
};

struct ff8_gfx_driver
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
	void *field_54;
	void *field_58;
	void *field_5C;
	void *field_60;
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
	void *field__84;
	void *field_88;
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

struct ff8_field_state_common {
	uint8_t stack_data[0x140];
	uint32_t field_140;
	uint32_t field_144;
	uint32_t field_148;
	uint32_t field_14c;
	uint32_t field_150;
	uint32_t field_154;
	uint32_t field_158;
	uint32_t field_15c;
	uint32_t execution_flags; // bgdraw: 0x10, bganime/rbganime: 0x980 (0x800: animation ongoing)
	uint32_t field_164;
	uint32_t field_168;
	uint32_t field_16c;
	uint32_t field_170;
	uint8_t field_174; // has anim?
	uint8_t field_175; // has anim mask?
	uint16_t current_instruction_position; // field_176
	uint32_t field_178;
	uint32_t field_17c;
	uint32_t field_180;
	uint8_t stack_current_position; // field_184
	uint8_t field_185;
	uint8_t field_186;
	uint8_t field_187;
};

struct ff8_field_state_background {
	ff8_field_state_common common;
	uint16_t bgstate; // field_188, set to -1 if off
	uint16_t field_18a;
	uint16_t bgparam_anim_start; // field_18c
	uint16_t bgparam_anim_end; // field_18e
	uint16_t bgparam_anim_speed1; // field_190
	uint16_t bgparam_anim_speed2; // field_192
	uint16_t bgparam_anim_flags; // field_194
	uint16_t field_196;
	uint32_t field_198;
	uint16_t bgshadeloop_remember_stack_pointer; // field_19c
	uint16_t bgshade_add_value; // field_19e
	uint16_t field_1a0; // bgshadeloop
	uint16_t field_1a2; // bgshadeloop
	uint8_t field_1a4; // bgshadeloop
	uint8_t field_1a5; // bgshadeloop
	uint8_t field_1a6; // bgshadeloop
	uint8_t bgshade_color1r; // field_1a7
	uint8_t bgshade_color1g; // field_1a8
	uint8_t bgshade_color1b; // field_1a9
	uint8_t bgshade_color2r; // field_1aa
	uint8_t bgshade_color2g; // field_1ab
	uint8_t bgshade_color2b; // field_1ac
	uint8_t bgshade_color1r_2; // field_1ad
	uint8_t bgshade_color1g_2; // field_1ae
	uint8_t bgshade_color1b_2; // field_1af
	uint8_t bgshade_color2r_2; // field_1b0
	uint8_t bgshade_color2g_2; // field_1b1
	uint8_t bgshade_color2b_2; // field_1b2
	uint8_t field_1b3;
};

struct ff8_menu_callback {
	void (*func)(int);
	uint32_t field_4;
};

// --------------- end of FF8 imports ---------------

// memory addresses and function pointers from FF8.exe
struct ff8_externals
{
	struct font_object **fonts;
	uint32_t movie_hack1;
	uint32_t movie_hack2;
	uint32_t swirl_sub_56D390;
	uint32_t nvidia_hack1;
	uint32_t nvidia_hack2;
	struct sprite_viewport *menu_viewport;
	uint32_t main_loop;
	uint32_t sub_47CCB0;
	uint32_t sub_534640;
	uint32_t sub_4972A0;
	uint32_t load_fonts;
	uint32_t cdcheck_main_loop;
	uint32_t cdcheck_sub_52F9E0;
	uint32_t main_entry;
	uint8_t *config_worldmap_fog_disabled;
	uint8_t *config_worldmap_color_anim_disabled;
	uint8_t *config_worldmap_textured_anim_disabled;
	uint32_t init_config;
	uint32_t (*reg_get_data_drive)(char*, DWORD);
	void (*set_game_paths)(int, char *, const char *);
	uint32_t (*sm_pc_read)(char*,void*);
	uint32_t get_disk_number;
	char* disk_data_path;
	const char *app_path;
	uint32_t swirl_enter;
	uint32_t swirl_main_loop;
	uint32_t sub_460B60;
	uint32_t field_main_loop;
	uint32_t field_main_exit;
	uint32_t psxvram_texture_pages_free;
	uint32_t psxvram_texture_page_free;
	uint32_t psxvram_texture_page_tex_header_free;
	uint32_t engine_set_init_time;
	uint32_t sub_4672C0;
	uint32_t sub_471F70;
	uint32_t field_fade_transition_sub_472990;
	uint32_t sub_45CDD0;
	uint32_t sub_4767B0;
	uint32_t sub_4789A0;
	char (*sub_47CA90)();
	uint32_t battle_trigger_field;
	uint32_t battle_trigger_worldmap;
	uint32_t _load_texture;
	uint32_t sub_4076B6;
	uint32_t sub_41AC34;
	uint32_t load_texture_data;
	uint32_t pubintro_init;
	uint32_t sub_467C00;
	uint32_t sub_468810;
	uint32_t dinput_get_input_device_capabilities_number_of_buttons;
	uint32_t get_command_key;
	uint32_t sub_468BD0;
	uint32_t pubintro_cleanup;
	uint32_t pubintro_exit;
	uint32_t pubintro_main_loop;
	uint32_t credits_main_loop;
	uint32_t go_to_main_menu_main_loop;
	uint32_t main_menu_main_loop;
	DWORD* credits_loop_state;
	DWORD* credits_counter;
	DWORD* credits_current_image_global_counter_start;
	DWORD* credits_current_step_image;
	uint32_t sub_470630;
	uint32_t dd_d3d_start;
	uint32_t create_d3d_gfx_driver;
	uint32_t d3d_init;
	uint32_t sub_40BFEB;
	uint32_t tim2tex;
	uint32_t sub_41BC76;
	uint32_t d3d_load_texture;
	uint32_t sub_559910;
	uint32_t swirl_sub_56D1D0;
	uint32_t load_credits_image;
	void (*input_fill_keystate)();
	int (*input_get_keyscan)(int, int);
	uint32_t sub_52FE80;
	uint32_t sub_45D610;
	uint32_t sub_45D080;
	uint32_t sub_464BD0;
	uint32_t sub_4653B0;
	uint32_t sub_559E40;
	uint32_t sub_559F30;
	uint32_t sub_497380;
	uint32_t sub_4B3410;
	uint32_t sub_4B3310;
	uint32_t sub_4B3140;
	uint32_t sub_4BDB30;
	ff8_menu_callback *menu_callbacks;
	uint32_t menu_cards_render;
	uint32_t sub_534AD0;
	uint8_t **card_texts_off_B96504;
	uint32_t sub_4EFC00;
	uint32_t sub_4EFCD0;
	uint32_t menu_config_controller;
	uint32_t menu_config_render;
	uint32_t menu_config_render_submenu;
	ff8_menu_config_input *menu_config_input_desc;
	ff8_menu_config_input_keymap *menu_config_input_desc_keymap;
	uint32_t main_menu_render_sub_4E5550;
	uint32_t main_menu_controller;
	uint32_t sub_4C2FF0;
	uint32_t menu_chocobo_world_controller;
	uint32_t create_save_file_sub_4C6E50;
	uint32_t create_save_chocobo_world_file_sub_4C6620;
	uint32_t get_text_data;
	uint32_t sub_4BE4D0;
	uint32_t sub_4BECC0;
	uint32_t menu_draw_text;
	uint32_t (*get_character_width)(uint32_t);
	struct ff8_graphics_object **swirl_texture1;
	uint32_t sub_48D0A0;
	uint32_t open_lzs_image;
	uint32_t (*credits_open_file)(char *, char *);
	uint32_t upload_psx_vram;
	uint32_t copy_psx_vram_part;
	void (*sub_464850)(uint32_t, uint32_t, uint32_t, uint32_t);
	uint8_t *psxvram_buffer;
	struct struc_51 *psx_texture_pages; // One per bpp (bpp 4, 8 and 16)
	uint32_t read_field_data;
	uint32_t upload_mim_file;
	uint32_t upload_pmp_file;
	char *field_filename;
	int (*field_scripts_init)(int, int, int, int);
	uint8_t *field_state_background_count;
	ff8_field_state_background **field_state_backgrounds;
	uint32_t load_field_models;
	uint32_t chara_one_read_file;
	uint32_t chara_one_seek_file;
	uint32_t chara_one_set_data_start;
	uint8_t **chara_one_data_start;
	uint32_t chara_one_upload_texture;
	uint32_t worldmap_main_loop;
	uint32_t worldmap_enter_main;
	uint32_t worldmap_sub_53F310;
	uint32_t worldmap_sub_53F310_call_24D;
	uint32_t worldmap_sub_53F310_call_2A9;
	uint32_t worldmap_sub_53F310_call_30D;
	uint32_t worldmap_sub_53F310_call_330;
	uint32_t worldmap_sub_53F310_call_366;
	uint32_t worldmap_sub_53F310_call_3B5;
	uint32_t worldmap_sub_53F310_loc_53F7EE;
	uint32_t worldmap_wmset_set_pointers_sub_542DA0;
	uint32_t worldmap_sub_541970_upload_tim;
	uint32_t worldmap_sub_548020;
	uint32_t worldmap_sub_5531F0;
	uint32_t worldmap_sub_554AA0;
	uint32_t worldmap_sub_554AA0_call_C2;
	uint32_t worldmap_alter_uv_sub_553B40;
	uint32_t worldmap_sub_545E20;
	uint32_t worldmap_chara_one;
	int32_t (*open_file_world)(const char*, int32_t, uint32_t, void *);
	uint32_t open_file_world_sub_52D670_texl_call1;
	uint32_t open_file_world_sub_52D670_texl_call2;
	uint32_t upload_psxvram_texl_pal_call1;
	uint32_t upload_psxvram_texl_pal_call2;
	uint32_t **worldmap_section17_position;
	uint32_t **worldmap_section18_position;
	uint32_t **worldmap_section38_position;
	uint32_t **worldmap_section39_position;
	uint32_t **worldmap_section40_position;
	uint32_t **worldmap_section41_position;
	uint32_t **worldmap_section42_position;
	uint32_t (*worldmap_prepare_tim_for_upload)(uint8_t *, ff8_tim *);
	uint32_t engine_eval_process_input;
	void (*engine_eval_keyboard_gamepad_input)();
	uint32_t engine_eval_is_button_pressed;
	uint32_t *engine_input_confirmed_buttons;
	uint32_t *engine_input_valid_buttons;
	void (*has_keyboard_gamepad_input)();
	uint32_t dinput_update_gamepad_status;
	LPDIRECTINPUTDEVICE8A dinput_gamepad_device;
	LPDIJOYSTATE2 dinput_gamepad_state;
	uint32_t dinput_init_gamepad;
	BYTE* engine_gamepad_button_pressed;
	DWORD* engine_mapped_buttons;
	uint32_t draw_movie_frame;
	struct ff8_movie_obj *movie_object;
	int (*sub_5304B0)();
	uint32_t *enable_framelimiter;
	unsigned char *byte_1CE4907;
	unsigned char *byte_1CE4901;
	unsigned char *byte_1CE490D;
	uint32_t sub_45B310;
	uint32_t sub_45B460;
	uint32_t ssigpu_init;
	uint32_t *sub_blending_capability;
	uint32_t *d3dcaps;
	uint32_t sub_53BB90;
	uint32_t worldmap_fog_filter_polygons_in_block_1;
	uint32_t worldmap_polygon_condition_2045C8C;
	uint32_t worldmap_has_polygon_condition_2045C90;
	uint32_t worldmap_sub_45DF20;
	uint32_t sub_45E3A0;
	uint32_t worldmap_fog_filter_polygons_in_block_2;
	uint32_t sub_461E00;
	uint32_t dword_1CA8848;
	uint32_t sub_53E2A0;
	uint32_t sub_53E6B0;
	uint32_t sub_4023D0;
	uint32_t sub_53C750;
	uint32_t sub_544630;
	uint32_t sub_548080;
	uint32_t sub_549E80;
	uint32_t sub_546100;
	uint32_t sub_54A0D0;
	uint32_t sub_54D7E0;
	uint32_t sub_54FDA0;
	uint32_t worldmap_with_fog_sub_53FAC0;
	char* worldmap_windows_idx_map;
	int (*world_dialog_assign_text_sub_543790)(int,int,char*);
	int (*world_dialog_question_assign_text_sub_5438D0)(int, int, char*, int, int, int, uint8_t);
	uint32_t sub_543CB0;
	uint32_t sub_5484B0;
	uint32_t sub_54A230;
	uint32_t sub_54E9B0;
	uint32_t sub_550070;
	int (*sub_541C80)(WORD*);
	uint32_t worldmap_input_update_sub_559240;
	uint32_t sub_554940;
	uint32_t sub_554940_call_130;
	uint32_t sub_541AE0;
	uint32_t sub_554BC0;
	uint32_t sub_557140;
	uint32_t sub_54B460;
	uint32_t sub_558D70;
	uint32_t sub_545EA0;
	uint32_t sub_545F10;
	uint32_t sub_465720;
	uint32_t sm_battle_sound;
	uint32_t outputdebugstringa;
	uint32_t sdmusicplay;
	uint32_t(*sd_music_play)(uint32_t, char*, uint32_t);
	uint32_t sd_music_play_at;
	uint32_t* current_music_ids;
	uint32_t sub_46B800;
	uint32_t stop_music;
	uint32_t set_midi_volume;
	BOOL (*dmusicperf_set_volume_sub_46C6F0)(uint32_t, int32_t);
	uint32_t sub_46C050;
	uint32_t sub_501B60;
	uint32_t pause_music_and_sfx;
	uint32_t restart_music_and_sfx;
	uint32_t directsound_create_secondary_buffer;
	uint32_t start;
	uint32_t battle_enter;
	uint32_t battle_main_loop;
	void (*show_vram_window)();
	void (*refresh_vram_window)();
	char* music_path;
	uint32_t opcode_effectplay2;
	uint32_t opcode_mes;
	uint32_t opcode_ames;
	uint32_t opcode_amesw;
	uint32_t opcode_ramesw;
	uint32_t opcode_ask;
	uint32_t opcode_aask;
	uint32_t opcode_messync;
	uint32_t opcode_winclose;
	uint32_t opcode_mesmode;
	uint32_t opcode_movie;
	uint32_t opcode_moviesync;
	uint32_t opcode_spuready;
	uint32_t opcode_movieready;
	uint32_t opcode_setvibrate;
	uint32_t opcode_musicload;
	uint32_t opcode_crossmusic;
	uint32_t opcode_dualmusic;
	uint32_t opcode_choicemusic;
	uint32_t opcode_musicvoltrans;
	uint32_t opcode_musicvolfade;
	uint32_t opcode_musicskip;
	uint32_t opcode_musicvolsync;
	uint32_t opcode_getmusicoffset;
	uint32_t opcode_drawpoint;
	uint32_t opcode_battle;
	uint32_t opcode_tuto;
	uint32_t opcode_mapjump;
	uint32_t opcode_pshm_w;
	BYTE* current_tutorial_id;
	uint32_t dmusic_segment_connect_to_dls;
	uint32_t choice_music;
	uint32_t load_midi_segment;
	uint32_t load_midi_segment_from_id;
	uint32_t play_midi_segments;
	uint32_t load_and_play_midi_segment;
	uint32_t stop_midi_segments;
	uint32_t volume_update;
	uint32_t volume_music_update;
	uint32_t music_load;
	uint32_t play_wav;
	uint32_t (*load_cdrom)();
	uint32_t load_cdrom_call;
	uint32_t (*play_cdrom)(uint32_t, uint32_t, uint32_t);
	uint32_t play_cdrom_call;
	uint32_t (*stop_cdrom)();
	uint32_t stop_cdrom_field_call;
	uint32_t stop_cdrom_cleanup_call;
	uint32_t** savemap;
	int32_t (*check_game_is_paused)(int32_t);
	DWORD* is_game_paused;
	uint32_t sub_470250;
	uint32_t *ssigpu_callbacks_1;
	uint32_t *ssigpu_callbacks_2;
	uint32_t sub_462AD0;
	uint32_t sub_462DF0;
	uint32_t sub_461220;
	uint32_t ssigpu_tx_select_2_sub_465CE0;
	uint32_t write_palette_texture_set_sub_466190;
	uint32_t read_vram_palette_sub_467370;
	uint32_t write_palette_to_driver_sub_467310;
	int (*sub_464F70)(struc_50 *, texture_page *, int, int, int, int, int, int, int, uint8_t *);
	void(*read_vram_1)(uint8_t *, int, uint8_t *, int, signed int, int, int);
	void(*read_vram_2_paletted)(uint8_t *, int, uint8_t *, int, signed int, int, int, uint16_t *);
	void(*read_vram_3_paletted)(uint8_t *, uint8_t *, signed int, int, int, uint16_t *);
	uint32_t sub_464DB0;
	uint32_t sub_4649A0;
	char *archive_path_prefix;
	int(*fs_archive_search_filename)(const char *, ff8_file_fi_infos *, const ff8_file_container *);
	int(*ff8_fs_archive_search_filename2)(const char *, ff8_file_fi_infos *, const ff8_file_container *);
	char *(*fs_archive_get_fl_filepath)(int, const ff8_file_fl *);
	uint32_t _open;
	int(*_sopen)(const char*, int, int, ...);
	uint32_t fopen;
	FILE *(*_fsopen)(const char*, const char*, int);
	uint32_t input_init;
	uint32_t ff8input_cfg_read;
	uint32_t ff8input_cfg_reset;
	char *(*strcpy_with_malloc)(const char *);
	uint32_t moriya_filesystem_open;
	uint32_t moriya_filesystem_seek;
	uint32_t moriya_filesystem_read;
	uint32_t moriya_filesystem_close;
	uint32_t read_or_uncompress_fs_data;
	uint32_t lzs_uncompress;
	void(*free_file_container)(ff8_file_container *);
	uint32_t field_get_dialog_string;
	uint32_t set_window_object;
	ff8_win_obj *windows;
	BYTE* field_dialog_current_choice;
	uint32_t sub_470440;
	uint32_t sub_49ACD0;
	uint32_t sub_4A0880;
	uint32_t sub_4A09A0;
	uint32_t sub_49FC10;
	uint32_t sub_4A0C00;
	uint32_t *dword_1D2B808;
	char(*show_dialog)(int32_t, uint32_t, int16_t);
	int(*pause_menu_with_vibration)(int);
	uint32_t ff8_draw_icon_or_key1;
	uint32_t ff8_draw_icon_or_key2;
	uint32_t ff8_draw_icon_or_key3;
	uint32_t ff8_draw_icon_or_key4;
	uint32_t ff8_draw_icon_or_key5;
	uint32_t ff8_draw_icon_or_key6;
	int(*pause_menu)(int);
	uint32_t init_pause_menu;
	uint32_t sub_49BB30;
	uint32_t sub_49FE60;
	uint32_t get_icon_sp1_data;
	uint32_t draw_controller_or_keyboard_icons;
	uint32_t get_vibration_capability;
	uint32_t vibration_apply;
	uint32_t vibration_set_is_enabled;
	uint32_t vibration_get_is_enabled;
	int(*get_keyon)(int, int);
	uint32_t set_vibration;
	ff8_gamepad_state *gamepad_states;
	ff8_vibrate_struc *vibration_objects;
	uint32_t vibration_clear_intensity;
	uint8_t *vibrate_data_world;
	uint32_t open_battle_vibrate_vib;
	uint8_t **vibrate_data_main;
	uint8_t **vibrate_data_battle;
	uint8_t *vibrate_data_field;
	uint32_t sub_537F30;
	uint32_t sub_5391B0;
	uint32_t sub_534560;
	uint32_t sub_536C30;
	uint32_t sub_535640;
	uint32_t sub_536CB0;
	uint8_t **card_texts_off_B96968;
	uint32_t sub_536C80;
	uint32_t sub_5366D0;
	uint32_t sub_539500;
	uint32_t *cardgame_funcs;
	uint8_t *cardgame_tim_texture_intro;
	uint8_t *cardgame_tim_texture_game;
	uint8_t *cardgame_tim_texture_cards;
	uint8_t *cardgame_tim_texture_icons;
	uint8_t *cardgame_tim_texture_font;
	uint32_t* is_card_game;
	uint32_t sfx_play_to_current_playing_channel;
	uint32_t sfx_unload_all;
	uint32_t sfx_pause_all;
	uint32_t sfx_resume_all;
	uint32_t sfx_stop_all2;
	uint32_t sfx_stop_all;
	void(*sfx_set_master_volume)(uint32_t);
	int(*sfx_get_master_volume)();
	uint32_t sfx_set_volume;
	uint32_t sfx_current_channel_is_playing;
	uint32_t sfx_is_playing;
	uint32_t sfx_set_panning;
	uint16_t *sfx_sound_count;
	ff8_audio_fmt **sfx_audio_fmt;
	uint32_t manage_time_engine_sub_569971;
	int (*enable_rdtsc_sub_40AA00)(int enable);
	uint32_t loc_47D490;
	uint32_t sub_500870;
	uint32_t sub_500C00;
	uint32_t sub_500CC0;
	uint32_t sub_506C90;
	uint32_t sub_506CF0;
	uint32_t sub_5084B0;
	uint32_t battle_open_file_wrapper;
	uint32_t battle_open_file;
	char **battle_filenames;
	uint32_t battle_load_textures_sub_500900;
	uint32_t loc_5005A0;
	uint32_t battle_upload_texture_to_vram;
	uint32_t sub_485460;
	uint32_t sub_485610;
	uint32_t sub_48D1A0;
	uint32_t sub_4AD7D0;
	uint32_t sub_4AD8D0;
	uint32_t sub_4AB4F0;
	uint32_t sub_4AB190;
	uint32_t sub_4A84E0;
	uint32_t sub_4AD400;
	uint32_t sub_4BB840;
	BYTE* battle_current_active_character_id;
	BYTE* battle_new_active_character_id;
	WORD* battle_encounter_id;
	uint32_t sub_4AB450;
	uint32_t battle_get_monster_name_sub_495100;
	uint32_t sub_4AA920;
	uint32_t battle_get_actor_name_sub_47EAF0;
	BYTE** battle_char_struct_dword_1D27B10;
	BYTE* byte_1CFF1C3;
	char* unk_1CFDC70;
	char* unk_1CFDC7C;
	WORD* word_1CF75EC;
	char* unk_1CFF84C;
	char* unk_1CF3E48;
	DWORD* dword_1CF3EE0;
	DWORD* battle_current_actor_talking;
	uint32_t sub_502380;
	uint32_t sub_50A790;
	uint32_t sub_50A9A0;
	uint32_t battle_read_effect_sub_50AF20;
	DWORD* func_off_battle_effects_C81774;
	int* battle_magic_id;
	uint32_t sub_571870;
	DWORD* func_off_battle_effect_textures_50AF93;
	uint32_t sub_6C3640;
	uint32_t sub_6C3760;
	uint8_t **vibrate_data_summon_quezacotl;
	uint32_t load_magic_data_sub_571B80;
	uint32_t load_magic_data_sub_5718E0;
	uint32_t load_magic_data_sub_571900;
	uint32_t sub_47D890;
	uint32_t sub_505DF0;
	uint32_t sub_4A94D0;
	uint32_t sub_4BCBE0;
	uint32_t sub_4C8B10;
	uint32_t battle_pause_sub_4CD140;
	uint32_t *is_alternative_pause_menu;
	uint32_t *pause_menu_option_state;
	void *battle_menu_state;
	uint32_t battle_pause_window_sub_4CD350;
	uint32_t sub_4A7210;
	uint32_t sub_B586F0;
	uint32_t sub_B64B80;
	DWORD* leviathan_funcs_B64C3C;
	uint32_t mag_data_palette_sub_B66560;
	DWORD** effect_struct_27973EC;
	uint8_t** mag_data_dword_2798A68;
	DWORD** effect_struct_2797624;
	uint32_t sub_B63230;
	uint32_t mag_data_texture_sub_B66560;
	BYTE** dword_27973E8;
	uint32_t battle_set_action_upload_raw_palette_sub_B666F0;
	uint32_t battle_set_action_upload_raw_palette_sub_B66400;
	uint32_t sub_84D110;
	uint32_t sub_84D1F0;
	uint32_t sub_84D230;
	uint32_t sub_84D2C0;
	uint32_t sub_84D4B0;
	uint32_t sub_84F2A0;
	uint32_t sub_84F860;
	uint32_t sub_84F8D0;
	uint32_t scan_get_text_sub_B687C0;
	uint32_t battle_entities_1D27BCB;
	uint32_t scan_text_data;
	uint32_t scan_text_positions;
	uint32_t fps_limiter;
	double *time_volume_change_related_1A78BE0;
	uint32_t* game_mode_obj_1D9CF88;
	uint32_t field_vars_stack_1CFE9B8;
	uint32_t get_card_name;
	uint32_t card_name_positions;
	uint32_t drawpoint_messages;
};

void ff8gl_field_78(struct ff8_polygon_set *polygon_set, struct ff8_game_obj *game_object);
void ff8_unload_texture(struct ff8_texture_set *texture_set);
void ff8_init_hooks(struct game_obj *_game_object);
struct ff8_gfx_driver *ff8_load_driver(void* game_object);
LPDIJOYSTATE2 ff8_update_gamepad_status();
bool ff8_skip_movies();
