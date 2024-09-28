/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include <windows.h>
#include <stdint.h>

#include "matrix.h"

/*
 * Render states supported by the graphics engine
 *
 * Not all of these were implemented in the original games, even fewer are
 * actually required. Named after corresponding Direct3D states.
 */
enum effects
{
	V_WIREFRAME,			      // 0x1
	V_TEXTURE,				      // 0x2
	V_LINEARFILTER,			    // 0x4
	V_PERSPECTIVE,			    // 0x8
	V_TMAPBLEND,			      // 0x10
	V_WRAP_U,				        // 0x20
	V_WRAP_V,				        // 0x40
	V_UNKNOWN80,			      // 0x80
	V_COLORKEY,				      // 0x100
	V_DITHER,				        // 0x200
	V_ALPHABLEND,			      // 0x400
	V_ALPHATEST,			      // 0x800
	V_ANTIALIAS,			      // 0x1000
	V_CULLFACE,				      // 0x2000
	V_NOCULL,				        // 0x4000
	V_DEPTHTEST,			      // 0x8000
	V_DEPTHMASK,			      // 0x10000
	V_SHADEMODE,			      // 0x20000
	V_SPECULAR,				      // 0x40000
	V_LIGHTSTATE,			      // 0x80000
	V_FOG,					        // 0x100000
	V_TEXADDR,				      // 0x200000
	V_UNKNOWN400000,        // 0x400000
	V_UNKNOWN800000,        // 0x800000
	V_ALPHAFUNC,            // 0x1000000
	V_ALPHAREF,             // 0x2000000
	V_UNKNOWNFFFDFBFD,      // 0xFFFDFBFD
	V_UNKNOWNFFFDFFFD,      // 0xFFFDFFFD
};

// helper definitions for all the different functions which should be provided by the graphics driver
typedef uint32_t (gfx_init)(struct game_obj *);
typedef void (gfx_cleanup)(struct game_obj *);
typedef uint32_t (gfx_lock)(uint32_t);
typedef uint32_t (gfx_unlock)(uint32_t);
typedef void (gfx_flip)(struct game_obj *);
typedef void (gfx_clear)(uint32_t, uint32_t, uint32_t, struct game_obj *);
typedef void (gfx_clear_all)(struct game_obj *);
typedef void (gfx_setviewport)(uint32_t, uint32_t, uint32_t, uint32_t, struct game_obj *);
typedef void (gfx_setbg)(struct bgra_color *, struct game_obj *);
typedef uint32_t (gfx_prepare_polygon_set)(struct polygon_set *);
typedef uint32_t (gfx_load_group)(uint32_t, struct matrix_set *, struct p_hundred *, struct p_group *, struct polygon_data *, struct polygon_set *, struct game_obj *);
typedef void (gfx_setmatrix)(uint32_t, struct matrix *, struct matrix_set *, struct game_obj *);
typedef void (gfx_unload_texture)(struct texture_set *);
typedef struct texture_set *(gfx_load_texture)(struct texture_set *, struct tex_header *, struct texture_format *);
typedef uint32_t (gfx_palette_changed)(uint32_t, uint32_t, uint32_t, struct palette *, struct texture_set *);
typedef uint32_t (gfx_write_palette)(uint32_t, uint32_t, void *, uint32_t, struct palette *, struct texture_set *);
typedef struct blend_mode *(gfx_blendmode)(uint32_t, struct game_obj *);
typedef void (gfx_light_polygon_set)(struct polygon_set *, struct light *);
typedef void (gfx_field_64)(uint32_t, uint32_t, struct game_obj *);
typedef void (gfx_setrenderstate)(struct p_hundred *, struct game_obj *);
typedef void (gfx_field_74)(uint32_t, struct game_obj *);
typedef void (gfx_field_78)(struct polygon_set *, struct game_obj *);
typedef void (gfx_draw_deferred)(struct struc_77 *, struct game_obj *);
typedef void (gfx_field_80)(struct graphics_object *, struct game_obj *);
typedef void (gfx_field_84)(uint32_t, struct game_obj *);
typedef uint32_t (gfx_begin_scene)(uint32_t, struct game_obj *);
typedef void (gfx_end_scene)(struct game_obj *);
typedef void (gfx_field_90)(uint32_t);
typedef void (gfx_polysetrenderstate)(struct polygon_set *, struct indexed_vertices *, struct game_obj *);
typedef void (gfx_draw_vertices)(struct polygon_set *, struct indexed_vertices *, struct game_obj *);
typedef void (gfx_field_EC)(struct game_obj *);

/*
 * This section defines some structures used internally by both game engines.
 *
 * Documentation for some of them can be found on the Qhimm wiki, a lot of
 * information can be gleaned from the source code to this program but in many
 * cases nothing is known except the size and general layout of the structure.
 *
 * Variable and structure names are mostly based on what they contain rather
 * than what they are for, a lot of names may be wrong, inappropriate or
 * downright misleading. Thread with caution!
 */

struct color_ui8
{
    byte r;
    byte g;
    byte b;
    byte a;
};

struct bgra_color_ui8
{
    byte b;
    byte g;
    byte r;
    byte a;
};

struct bgra_color
{
	float b;
	float g;
	float r;
	float a;
};

struct bgra_byte
{
	byte b;
	byte g;
	byte r;
	byte a;
};

struct rgba_color
{
	float r;
	float g;
	float b;
	float a;
};

typedef struct {
	short x, y, z, res;		// short is a 2 byte signed integer
} vertex_3s;

struct struc_81
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t blend_mode;
	uint32_t vertexcolor;
	struct texture_set *texture_set;
};

struct struc_173
{
	unsigned char color_op;
	unsigned char field_1;
	unsigned char scroll_uv;
	unsigned char scroll_v;
	unsigned char change_palette;
	unsigned char setrenderstate;
	unsigned char add_offsets;
	unsigned char field_7;
	color_ui8 color;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t x_offset;
	uint32_t y_offset;
	uint32_t z_offset;
	uint32_t z_offset2;
	float u_offset;
	float v_offset;
	uint32_t palette_index;
	struct p_hundred *hundred_data;
	unsigned char field_34[16];
};

struct struc_77
{
	struct struc_77 *next;
	uint32_t current_group;
	struct polygon_set *polygon_set;
	struct p_hundred *hundred_data;
	uint32_t use_matrix;
	struct matrix matrix;
	uint32_t use_matrix_pointer;
	struct matrix *matrix_pointer;
	struct struc_173 struc_173;
};

struct heap
{
	struct heap *next;
	uint32_t size;					// ?
	struct heap *last;			// ?
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	int (*fn_something)(uint32_t, int);
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	void *callback_data;
	void *callback;
};

struct graphics_instance
{
	uint32_t frame_counter;
	struct heap *heap;
};

struct p_edge
{
	WORD vertex1;
	WORD vertex2;
};

struct texcoords
{
	float u;
	float v;
};

struct p_polygon
{
	WORD field_0;
	WORD vertex1;
	WORD vertex2;
	WORD vertex3;
	WORD normals[3];
	WORD edges[3];
	uint32_t field_14;
};

struct p_group
{
	uint32_t polytype;
	uint32_t offpoly;
	uint32_t numpoly;
	uint32_t offvert;
	uint32_t numvert;
	uint32_t offedge;
	uint32_t numedge;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t offtex;
	uint32_t textured;
	uint32_t texid;
};

struct boundingbox
{
	uint32_t field_0;
	float max_x;
	float max_y;
	float max_z;
	float min_x;
	float min_y;
	float min_z;
};

struct nvertex
{
	vector3<float> _;

	union
	{
		struct
		{
			float w;
			union
			{
				uint32_t color;
				struct
				{
					unsigned char b;
					unsigned char g;
					unsigned char r;
					unsigned char a;
				};
			};
			uint32_t specular;
		} color;

		vector3<float> normal;
	};

	float u;
	float v;
};

struct struc_186
{
	struct graphics_object *graphics_object;
	uint32_t polytype;
	uint32_t field_8;
	vector3<float> vertices[4];
	struct texcoords texcoords[4];
	color_ui8 colors[4];
	float w[4];
	struct nvertex *nvertex_pointer;
	uint32_t palette_index;
};

struct struc_84
{
	struct struc_84 *next;
	uint32_t field_4;
	struct struc_186 *struc_186;
	struct matrix matrix;
	struct struc_173 struc_173;
};

struct struc_49
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t frame_counter;
	struct struc_84 *struc_84;
	struct graphics_instance *graphics_instance;
};

struct indexed_primitive
{
	uint32_t field_0;
	uint32_t vertex_size;
	uint32_t primitivetype;
	uint32_t vertextype;
	struct nvertex *vertices;
	uint32_t vertexcount;
	WORD *indices;
	uint32_t indexcount;
	uint32_t flags;
	uint32_t field_24;
};

struct matrix_set
{
	uint32_t field_0;
	uint32_t size;
	uint32_t field_8;
	uint32_t field_C;
	struct matrix *matrix_array;
	struct matrix *matrix_world;
	struct matrix *matrix_view;
	struct matrix *matrix_projection;
	void *d3dmatrix_world;
	void *d3dmatrix_view;
	void *d3dmatrix_projection;
};

struct polygon_data
{
	uint32_t version;
	uint32_t field_4;
	uint32_t vertextype;
	uint32_t numverts;
	uint32_t numnormals;
	uint32_t field_14;
	uint32_t numtexcoords;
	uint32_t numvertcolors;
	uint32_t numedges;
	uint32_t numpolys;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t numhundreds;
	uint32_t numgroups;
	uint32_t numboundingboxes;
	uint32_t has_normindextable;
	vector3<float> *vertdata;
	vector3<float> *normaldata;
	vector3<float> *field_48;
	struct texcoords *texcoorddata;
	uint32_t *vertexcolordata;
	uint32_t *polycolordata;
	struct p_edge *edgedata;
	struct p_polygon *polydata;
	char *pc_name;
	void *field_64;
	struct p_hundred *hundredsdata;
	struct p_group *groupdata;
	struct boundingbox *boundingboxdata;
	uint32_t *normindextabledata;
	uint32_t field_78;
	struct polygon_lists *lists;
};

struct p_hundred
{
	uint32_t field_0;
	uint32_t field_4;
	uint32_t options;
	uint32_t features;
	uint32_t field_10;
	struct texture_set *texture_set;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t shademode;
	uint32_t lightstate_ambient;
	uint32_t field_2C;
	void *lightstate_material_pointer;
	uint32_t srcblend;
	uint32_t destblend;
	uint32_t field_3C;
	uint32_t alpharef;
	uint32_t blend_mode;
	uint32_t zsort;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t vertex_alpha;
	uint32_t field_60;
};

struct texture_format
{
	uint32_t width;
	uint32_t height;
	uint32_t bytesperrow;
	void *field_C;
	uint32_t use_palette;
	uint32_t bitsperindex;
	uint32_t indexed_to_8bit;
	uint32_t palette_size;				// ?
	uint32_t palettes;					// ?
	uint32_t *palette_data;
	uint32_t bitsperpixel;
	uint32_t bytesperpixel;
	uint32_t red_bits;
	uint32_t green_bits;
	uint32_t blue_bits;
	uint32_t alpha_bits;
	uint32_t red_mask;
	uint32_t green_mask;
	uint32_t blue_mask;
	uint32_t alpha_mask;
	uint32_t red_shift;
	uint32_t green_shift;
	uint32_t blue_shift;
	uint32_t alpha_shift;
	uint32_t red_truecolor_bitdiff;		// 8 - red_bits
	uint32_t green_truecolor_bitdiff;	// 8 - green_bits
	uint32_t blue_truecolor_bitdiff;	// etc
	uint32_t alpha_truecolor_bitdiff;
	uint32_t red_max;
	uint32_t green_max;
	uint32_t blue_max;
	uint32_t alpha_max;
};

struct struc_91
{
	uint32_t field_0;
	uint32_t x_offset;
	uint32_t y_offset;
	uint32_t width;
	uint32_t height;
	uint32_t xscale;
	uint32_t yscale;
	uint32_t color_key;
	uint32_t width2;
	uint32_t height2;
	uint32_t pitch2;
	uint32_t bytesperpixel2;
	void *image_data2;
	struct texture_format tex_format;
	void *image_data;
};

struct palette
{
	uint32_t field_0;
	uint32_t palette_size;
	uint32_t bitsperpixel;
	uint32_t total_palettes;
	uint32_t palette_entries;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	void *d3dcolorpal;
	void *rgbapal;
	void *palette_entry;
	uint32_t ddpalette;
	tex_header* tex_header;
	texture_set* texture_set;
	uint32_t field_3C;
};

struct blend_mode
{
	uint32_t field_0;
	uint32_t zsort;
	uint32_t vertex_alpha;
	uint32_t srcblendmode;
	uint32_t srcblendcaps;
	uint32_t destblendmode;
	uint32_t destblendcaps;
	uint32_t field_1C;
	uint32_t field_20;
};

typedef struct
{
	char dummy[0xCC];
} D3DDEVICEDESC;

typedef void* main_obj_fn(struct game_obj*);

struct main_obj
{
	main_obj_fn *init;
	main_obj_fn *cleanup;
	main_obj_fn *enter_main;
	main_obj_fn *exit_main;
	main_obj_fn *main_loop;
	main_obj_fn *field_14;
	main_obj_fn *field_18;
};
