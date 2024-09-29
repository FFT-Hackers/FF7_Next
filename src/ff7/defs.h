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

#include "ff7.h"
#include <windows.h>
#include <stdint.h>

// kernel
void kernel2_reset_counters();
char *kernel2_add_section(uint32_t size);
char *kernel2_get_text(uint32_t section_base, uint32_t string_id, uint32_t section_offset);
void ff7_load_kernel2_wrapper(char *filename);

// menu
void ff7_menu_battle_end_sub_6C9543();
void ff7_menu_sub_71AAA3(int param_1);
int ff7_get_materia_gil(uint32_t materia);
void ff7_opcode_increase_gil_call(int param_1);
byte ff7_menu_sub_6CBCF3(uint32_t materia_id);
void ff7_menu_sub_6CC17F(uint32_t materia);
uint32_t ff7_menu_decrease_item_quantity(uint32_t item_data);
void ff7_menu_sub_6CDC09(DWORD param_1);
void ff7_battle_menu_sub_6DB0EE();
void ff7_set_battle_menu_state_data_at_full_atb(short param_1, short param_2, short menu_state);

// misc
void ff7_core_game_loop();
uint32_t get_equipment_stats(uint32_t party_index, uint32_t type);
void ff7_wm_activateapp(bool hasFocus);
int ff7_get_gamepad();
struct ff7_gamepad_status* ff7_update_gamepad_status();
void* ff7_engine_exit_game_mode(ff7_game_obj* game_object);
void ff7_on_gameover_enter();
void ff7_on_gameover_exit();
BYTE ff7_toggle_battle_field();
BYTE ff7_toggle_battle_worldmap();
bool ff7_skip_movies();
void *ff7_menu_sub_6F5C0C(uint32_t param1, uint32_t param2, uint8_t param3, uint8_t param4, uint32_t param5);
void *ff7_menu_sub_6FAC38(uint32_t param1, uint32_t param2, uint8_t param3, uint8_t param4, uint32_t param5);
void ff7_limit_fps();
void ff7_handle_ambient_playback();
void ff7_handle_voice_playback();
BOOL ff7_write_save_file(char slot);
int ff7_field_load_models_atoi(const char* str);
void ff7_chocobo_field_entity_60FA7D(WORD param1, short param2, short param3);
void ff7_character_regularly_field_entity_60FA7D(WORD param1, short param2, short param3);
int ff7_load_save_file(int param_1);

// file
FILE *open_lgp_file(char *filename, uint32_t mode);
void close_lgp_file(FILE *fd);
extern char lgp_names[18][256];
uint32_t lgp_chdir(char *path);
struct lgp_file *lgp_open_file(char *filename, uint32_t lgp_num);
uint32_t lgp_seek_file(uint32_t offset, uint32_t lgp_num);
uint32_t lgp_read(uint32_t lgp_num, char *dest, uint32_t size);
uint32_t lgp_read_file(struct lgp_file *file, uint32_t lgp_num, char *dest, uint32_t size);
uint32_t lgp_get_filesize(struct lgp_file *file, uint32_t lgp_num);
void close_file(struct ff7_file *file);
struct ff7_file *open_file(struct file_context *file_context, char *filename);
uint32_t __read_file(uint32_t count, void *buffer, struct ff7_file *file);
uint32_t read_file(uint32_t count, void *buffer, struct ff7_file *file);
uint32_t __read(FILE *file, char *buffer, uint32_t count);
uint32_t write_file(uint32_t count, void *buffer, struct ff7_file *file);
uint32_t get_filesize(struct ff7_file *file);
uint32_t tell_file(struct ff7_file *file);
void seek_file(struct ff7_file *file, uint32_t offset);
char *make_pc_name(struct file_context *file_context, struct ff7_file *file, char *filename);
int ff7_read_field_file(char* path);

// graphics
void destroy_d3d2_indexed_primitive(struct indexed_primitive *ip);
uint32_t ff7gl_load_group(uint32_t group_num, struct matrix_set *matrix_set, struct p_hundred *_hundred_data, struct p_group *_group_data, struct polygon_data *polygon_data, struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object);
struct tex_header *sub_673F5C(struct struc_91 *struc91);
void draw_single_triangle(struct nvertex *vertices);
void sub_6B2720(struct indexed_primitive *ip);
void draw_3d_model(uint32_t current_frame, struct anim_header *anim_header, struct struc_110 *struc_110, struct hrc_data *hrc_data, struct ff7_game_obj *game_object);
void fill_light_data(struct light_data* pOutLightData, struct ff7_polygon_set *polygon_set);
void update_view_matrix(struct ff7_game_obj *game_object);

// loaders
struct anim_header *load_animation(struct file_context *file_context, char *filename);
struct battle_hrc_header *read_battle_hrc(uint32_t use_file_context, struct file_context *file_context, char *filename);
struct polygon_data *load_p_file(struct file_context *file_context, uint32_t create_lists, char *filename);
void destroy_tex_header(struct ff7_tex_header *tex_header);
struct ff7_tex_header *load_tex_file(struct file_context *file_context, char *filename);

// dsound
int ff7_dsound_create(HWND hwnd, LPGUID guid);
void ff7_dsound_release();
int ff7_dsound_createsoundbuffer(const WAVEFORMATEX *waveFormatEx);

// minigames
void ff7_condor_fix_unit_texture_load(uint32_t unk, struc_3 *struc_3);
void __fastcall ff7_snowboard_parse_model_vertices(snowboard_this* _this, void* edx, const matrix *matrix, int current_obj, int obj_type, int unk);

// japanese
void engine_load_menu_graphics_objects_6C1468_jp(int a1);
//__int16 field_submit_draw_text_640x480_6E706D(__int16 character_x, __int16 character_y, __int16 text_box_right_position, byte *buffer_text, float z_value);
__int16 field_submit_draw_text_640x480_6E706D_jp(__int16 character_x, __int16 character_y, __int16 text_box_right_position, byte *buffer_text, float z_value);
void field_draw_text_boxes_and_text_graphics_object_6ECA68_jp();
//int common_submit_draw_char_from_buffer_6F564E(int x, int vertex_y, int n_shapes, unsigned __int16 letter, float z_value);
int common_submit_draw_char_from_buffer_6F564E_jp(int x, int vertex_y, int n_shapes, unsigned __int16 letter, float z_value);
void menu_draw_everything_6CC9D3_jp();
void battle_draw_menu_everything_6CEE84_jp();
void draw_text_top_display_6D1CC0(int a1, __int16 menu_box_idx, char a3, unsigned __int16 a4);
void draw_text_top_display_6D1CC0_jp(int a1, __int16 menu_box_idx, char a3, unsigned __int16 a4);
void main_menu_draw_everything_maybe_6C0B91_jp();
void field_text_box_window_paging_631945_jp(short);
void field_text_box_window_opening_6317A9_jp(short);
int sub_6F54A2_jp(byte *a1);
