/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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
#include <mmsystem.h>
#include <dinput.h>
#include <dsound.h>

#include "common_imports.h"

// all known OFFICIAL versions of FF7 & FF8 released for the PC
#define VERSION_FF7_102_US          1
#define VERSION_FF7_102_FR          2
#define VERSION_FF7_102_DE          3
#define VERSION_FF7_102_SP          4
#define VERSION_FF8_12_US           5
#define VERSION_FF8_12_US_NV        6
#define VERSION_FF8_12_FR           7
#define VERSION_FF8_12_FR_NV        8
#define VERSION_FF8_12_DE           9
#define VERSION_FF8_12_DE_NV       10
#define VERSION_FF8_12_SP          11
#define VERSION_FF8_12_SP_NV       12
#define VERSION_FF8_12_IT          13
#define VERSION_FF8_12_IT_NV       14
#define VERSION_FF8_12_US_EIDOS    15
#define VERSION_FF8_12_US_EIDOS_NV 16
#define VERSION_FF8_12_JP          17
#define VERSION_FF8_12_JP_NV       18

// Steam app id of FF7 & FF8
#define FF7_APPID 39140
#define FF8_APPID 39150

#define NV_VERSION (!(version & 1))
#define JP_VERSION (version == VERSION_FF8_12_JP || version == VERSION_FF8_12_JP_NV)
#define FF8_US_VERSION (version == VERSION_FF8_12_US || version == VERSION_FF8_12_US_NV || version == VERSION_FF8_12_US_EIDOS || version == VERSION_FF8_12_US_EIDOS_NV)
#define FF8_SP_VERSION (version == VERSION_FF8_12_SP || version == VERSION_FF8_12_SP_NV)
#define FF8_IT_VERSION (version == VERSION_FF8_12_IT || version == VERSION_FF8_12_IT_NV)

// FF8 does not support BLUE text!
enum
{
	TEXTCOLOR_GRAY,
	TEXTCOLOR_BLUE,
	TEXTCOLOR_RED,
	TEXTCOLOR_PINK,
	TEXTCOLOR_GREEN,
	TEXTCOLOR_LIGHT_BLUE,
	TEXTCOLOR_YELLOW,
	TEXTCOLOR_WHITE,
	NUM_TEXTCOLORS
};

enum game_modes
{
	MODE_FIELD = 0,
	MODE_BATTLE,
	MODE_WORLDMAP,
	MODE_MENU,
	MODE_HIGHWAY,
	MODE_CHOCOBO,
	MODE_SNOWBOARD,
	MODE_CONDOR,
	MODE_SUBMARINE,
	MODE_COASTER,
	MODE_CDCHECK,
	MODE_EXIT,
	MODE_SWIRL,
	MODE_GAMEOVER,
	MODE_CREDITS,
	MODE_INTRO,
	MODE_CARDGAME,
	MODE_UNKNOWN,
	MODE_AFTER_BATTLE,
	MODE_MAIN_MENU,
};

enum AspectRatioMode
{
	AR_ORIGINAL = 0,
	AR_STRETCH,
	AR_WIDESCREEN_16X9,
	AR_WIDESCREEN_16X10,
	AR_COUNT
};

enum GamepadAnalogueIntent
{
	INTENT_NONE,
	INTENT_WALK,
	INTENT_RUN
};

// popup lifetime in frames
#define POPUP_TTL_MAX 10000

// dummy TEX version for framebuffer textures
#define FB_TEX_VERSION 100

struct game_mode
{
	uint32_t mode;
	char *name;
	uint32_t driver_mode;
	uint32_t trace;
	uint32_t main_loop;
};

struct light_data
{
	bgra_color global_light_color;
	vector3<float> light_dir_1;
	bgra_color light_color_1;
	vector3<float> light_dir_2;
	bgra_color light_color_2;
	vector3<float> light_dir_3;
	bgra_color light_color_3;
	bgra_color scripted_light_color;
};

gfx_init common_init;
gfx_cleanup common_cleanup;
gfx_lock common_lock;
gfx_unlock common_unlock;
gfx_flip common_flip;
gfx_clear common_clear;
gfx_clear_all common_clear_all;
gfx_setviewport common_setviewport;
gfx_setbg common_setbg;
gfx_prepare_polygon_set common_prepare_polygon_set;
gfx_load_group common_load_group;
gfx_setmatrix common_setmatrix;
gfx_light_polygon_set common_light_polygon_set;
gfx_unload_texture common_unload_texture;
gfx_load_texture common_load_texture;
gfx_palette_changed common_palette_changed;
gfx_write_palette common_write_palette;
gfx_blendmode common_blendmode;
gfx_light_polygon_set ff7gl_light_polygon_set;
gfx_field_64 common_field_64;
gfx_setrenderstate common_setrenderstate;
gfx_field_74 common_field_74;
gfx_field_78 common_field_78;
gfx_draw_deferred common_draw_deferred;
gfx_field_80 common_field_80;
gfx_field_84 common_field_84;
gfx_begin_scene common_begin_scene;
gfx_end_scene common_end_scene;
gfx_field_90 common_field_90;
gfx_polysetrenderstate common_setrenderstate_2D;
gfx_draw_vertices common_draw_2D;
gfx_draw_vertices common_draw_paletted2D;
gfx_polysetrenderstate common_setrenderstate_3D;
gfx_draw_vertices common_draw_3D;
gfx_draw_vertices common_draw_paletted3D;
gfx_draw_vertices common_draw_lines;
gfx_field_EC common_field_EC;

/*
 * This structure holds memory addresses and function pointers of the original
 * engine used within this program. Not all of them are currently used for both
 * games, MIDI functions for example are only used for FF7 but they could
 * concievably be used for FF8 aswell.
 */

struct common_externals
{
	WORD *_mode;
	WORD *_previous_mode;
	uint32_t dinput_hack1;
	gfx_load_group *generic_load_group;
	gfx_light_polygon_set *generic_light_polygon_set;
	void *(*assert_free)(void *, const char *, uint32_t);
	void *(*assert_malloc)(uint32_t, const char *, uint32_t);
	void *(*assert_calloc)(uint32_t, uint32_t, const char *, uint32_t);
	IDirectSound **directsound;
	uint32_t directsound_create;
	uint32_t directsound_createsoundbuffer;
	uint32_t directsound_release;
	struct palette *(*create_palette_for_tex)(uint32_t, struct tex_header *, struct texture_set *);
	struct game_obj *(*get_game_object)();
	struct texture_format *(*create_texture_format)();
	void (*add_texture_format)(struct texture_format *, struct game_obj *);
	void (*make_pixelformat)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, struct texture_format *);
	struct texture_set *(*create_texture_set)();
	uint32_t debug_print;
	uint32_t debug_print2;
	uint32_t prepare_movie;
	uint32_t release_movie_objects;
	uint32_t start_movie;
	uint32_t update_movie_sample;
	uint32_t stop_movie;
	uint32_t get_movie_frame;
	struct tex_header *(*create_tex_header)();
	uint64_t (*get_time)(uint64_t*);
	uint64_t (*diff_time)(uint64_t*,uint64_t*,uint64_t*);
	uint32_t midi_init;
	uint32_t midi_cleanup;
	uint32_t wav_cleanup;
	char *(*get_midi_name)(uint32_t);
	uint32_t use_midi;
	uint32_t play_midi;
	uint32_t play_wav;
	uint32_t stop_midi;
	uint32_t stop_wav;
	uint32_t cross_fade_midi;
	uint32_t pause_midi;
	uint32_t pause_wav;
	uint32_t restart_midi;
	uint32_t restart_wav;
	uint32_t midi_status;
	DWORD* master_midi_volume;
	uint32_t set_master_midi_volume;
	uint32_t set_midi_volume;
	uint32_t set_midi_volume_trans;
	uint32_t set_midi_volume_fade;
	uint32_t set_midi_tempo;
	uint32_t remember_midi_playing_time;
	int (*draw_graphics_object)(int n_shape, graphics_object *graphics_object);
	char *font_info;
	uint32_t build_dialog_window;
	uint32_t write_file;
	uint32_t close_file;
	uint32_t open_file;
	uint32_t read_file;
	uint32_t __read_file;
	uint32_t get_filesize;
	uint32_t tell_file;
	uint32_t seek_file;
	void *(*alloc_read_file)(uint32_t, uint32_t, struct file *);
	void *(*alloc_get_file)(struct file_context *, uint32_t *, char *);
	void (*destroy_tex)(struct tex_header *);
	uint32_t destroy_tex_header;
	uint32_t start;
	uint32_t winmain;
	uint32_t load_tex_file;
	uint32_t directsound_buffer_flags_1;
	uint32_t sfx_init;
	uint32_t sfx_cleanup;
	uint32_t sfx_load;
	uint32_t sfx_unload;
	uint32_t sfx_pause;
	uint32_t sfx_resume;
	uint32_t sfx_stop;
	uint32_t sfx_release;
	uint32_t (*play_sfx)(uint32_t);
	uint32_t (*play_sfx_effects)(byte, uint32_t, uint32_t, uint32_t, uint32_t);
	uint32_t play_sfx_on_channel;
	uint32_t (*set_sfx_volume_on_channel)(uint32_t, uint32_t);
	uint32_t (*set_sfx_volume_trans_on_channel)(uint32_t, uint32_t, uint32_t);
	uint32_t (*set_sfx_panning_on_channel)(uint32_t, uint32_t);
	uint32_t (*set_sfx_panning_trans_on_channel)(uint32_t, uint32_t, uint32_t);
	uint32_t (*set_sfx_frequency_on_channel)(uint32_t, uint32_t);
	uint32_t (*set_sfx_frequency_trans_on_channel)(uint32_t, uint32_t, uint32_t);
	uint32_t *master_sfx_volume;
	uint32_t* dsound_volume_table;
	IDirectInputDeviceA **keyboard_device;
	uint32_t get_keyboard_state;
	uint32_t *keyboard_connected;
	int (*dinput_acquire_keyboard)();
	uint32_t create_window;
	WNDPROC engine_wndproc;
	uint32_t* execute_opcode_table;
	uint32_t update_field_entities;
	WORD* current_field_id;
	char* current_field_name;
	WORD* previous_field_id;
	uint32_t update_entities_call;
	int16_t* current_triangle_id;
	WORD* field_game_moment;
};

// heap allocation wrappers
// driver_* functions are to be used for data internal to the driver, memory which is never allocated or free'd by the game
// external_* functions must be used for memory which could be allocated or free'd by the game
#ifndef NO_EXT_HEAP
#define external_free(x) common_externals.assert_free(x, "", 0)
#define external_malloc(x) common_externals.assert_malloc(x, "", 0)
#define external_calloc(x, y) common_externals.assert_calloc(x, y, "", 0)
#else
void ext_free(void *ptr, const char *file, uint32_t line);
void *ext_malloc(uint32_t size, const char *file, uint32_t line);
void *ext_calloc(uint32_t size, uint32_t num, const char *file, uint32_t line);

#define external_free(x) ext_free(x, "", 0)
#define external_malloc(x) ext_malloc(x, "", 0)
#define external_calloc(x, y) ext_calloc(x, y, "", 0)
#endif

#ifndef HEAP_DEBUG
#define driver_malloc(x) malloc(x)
#define driver_calloc(x, y) calloc(x, y)
#define driver_free(x) free(x)
#define driver_realloc(x, y) realloc(x, y)
#else
void *driver_malloc(uint32_t size);
void *driver_calloc(uint32_t size, uint32_t num);
void driver_free(void *ptr);
void *driver_realloc(void *ptr, uint32_t size);
#endif

// profiling routines
#ifdef PROFILE
#define PROFILE_START() qpc_get_time(&profile_start)
#define PROFILE_END() { qpc_get_time(&profile_end); profile_total += profile_end - profile_start; }

extern time_t profile_start;
extern time_t profile_end;
extern time_t profile_total;
#endif PROFILE

struct driver_stats
{
	uint32_t texture_count;
	uint32_t external_textures;
	uint32_t ext_cache_size;
	uint32_t texture_reloads;
	uint32_t palette_writes;
	uint32_t palette_changes;
	uint32_t vertex_count;
	uint32_t deferred;
	time_t timer;
};

time_t qpc_get_time(time_t *dest);
time_t qpc_diff_time(time_t* t1, time_t* t2, time_t* out);
uint32_t get_version();
struct game_mode *getmode();
struct game_mode *getmode_cached();
struct tex_header *make_framebuffer_tex(uint32_t tex_w, uint32_t tex_h, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color_key);
void internal_set_renderstate(uint32_t state, uint32_t option, struct game_obj *game_object);
uint32_t create_framebuffer_texture(struct texture_set *texture_set, struct tex_header *tex_header);
void blit_framebuffer_texture(struct texture_set *texture_set, struct tex_header *tex_header);

void get_data_lang_path(PCHAR buffer);
void get_userdata_path(PCHAR buffer, size_t bufSize, bool isSavegameFile);

#if defined(__cplusplus)
extern "C" {
#endif

void ffnx_inject_driver(struct game_obj* game_object);

#if defined(__cplusplus)
}
#endif

bool drawFFNxLogoFrame(struct game_obj* game_object);
void stopDrawFFNxLogo();
