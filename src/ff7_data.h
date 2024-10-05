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

#include <functional>

#include "ff7.h"
#include "globals.h"
#include "log.h"
#include "patch.h"

// FF7 game mode definitions
static struct game_mode ff7_modes[] = {
	{FF7_MODE_FIELD,       "MODE_FIELD",       MODE_FIELD,       true  },
	{FF7_MODE_BATTLE,      "MODE_BATTLE",      MODE_BATTLE,      true  },
	{FF7_MODE_WORLDMAP,    "MODE_WORLDMAP",    MODE_WORLDMAP,    true  },
	{FF7_MODE_UNKNOWN4,    "MODE_UNKNOWN4",    MODE_UNKNOWN,     false },
	{FF7_MODE_MENU,        "MODE_MENU",        MODE_MENU,        true  },
	{FF7_MODE_HIGHWAY,     "MODE_HIGHWAY",     MODE_HIGHWAY,     true  },
	{FF7_MODE_CHOCOBO,     "MODE_CHOCOBO",     MODE_CHOCOBO,     true  },
	{FF7_MODE_SNOWBOARD,   "MODE_SNOWBOARD",   MODE_SNOWBOARD,   true  },
	{FF7_MODE_CONDOR,      "MODE_CONDOR",      MODE_CONDOR,      true  },
	{FF7_MODE_SUBMARINE,   "MODE_SUBMARINE",   MODE_SUBMARINE,   true  },
	{FF7_MODE_COASTER,     "MODE_COASTER",     MODE_COASTER,     true  },
	{FF7_MODE_CDCHECK,     "MODE_CDCHECK",     MODE_CDCHECK,     true  },
	{FF7_MODE_UNKNOWN13,   "MODE_UNKNOWN13",   MODE_UNKNOWN,     false },
	{FF7_MODE_SNOWBOARD2,  "MODE_SNOWBOARD2",  MODE_SNOWBOARD,   true  },
	{FF7_MODE_UNKNOWN15,   "MODE_UNKNOWN15",   MODE_UNKNOWN,     false },
	{FF7_MODE_UNKNOWN16,   "MODE_UNKNOWN16",   MODE_UNKNOWN,     false },
	{FF7_MODE_BATTLE_MENU, "MODE_BATTLE_MENU", MODE_MENU,        true  },
	{FF7_MODE_UNKNOWN18,   "MODE_UNKNOWN18",   MODE_UNKNOWN,     false },
	{FF7_MODE_EXIT,        "MODE_EXIT",        MODE_EXIT,        true  },
	{FF7_MODE_MAIN_MENU,   "MODE_MAIN_MENU",   MODE_MENU,        true  },
	{FF7_MODE_UNKNOWN21,   "MODE_UNKNOWN21",   MODE_UNKNOWN,     false },
	{FF7_MODE_INTRO,       "MODE_INTRO",       MODE_INTRO,       true  },
	{FF7_MODE_SWIRL,       "MODE_SWIRL",       MODE_SWIRL,       true  },
	{FF7_MODE_UNKNOWN24,   "MODE_UNKNOWN24",   MODE_UNKNOWN,     false },
	{FF7_MODE_UNKNOWN25,   "MODE_UNKNOWN25",   MODE_UNKNOWN,     false },
	{FF7_MODE_GAMEOVER,    "MODE_GAMEOVER",    MODE_GAMEOVER,    true  },
	{FF7_MODE_CREDITS,     "MODE_CREDITS",     MODE_CREDITS,     true  },
	{FF7_MODE_UNKNOWN28,   "MODE_UNKNOWN28",   MODE_UNKNOWN,     false },
};

inline void ff7_set_main_loop(uint32_t driver_mode, uint32_t main_loop)
{
	uint32_t i;

	for(i = 0; i < num_modes; i++) if(ff7_modes[i].driver_mode == driver_mode) ff7_modes[i].main_loop = main_loop;
}

inline void ff7_find_externals(struct ff7_game_obj* game_object)
{
	uint32_t main_init_loop = (uint32_t)game_object->engine_loop_obj.init;
	uint32_t main_loop = (uint32_t)game_object->engine_loop_obj.main_loop;
	uint32_t main_cleanup_loop = (uint32_t)game_object->engine_loop_obj.cleanup;
	uint32_t field_main_loop;
	uint32_t battle_main_loop;
	uint32_t menu_main_loop;
	uint32_t worldmap_main_loop;
	uint32_t cdcheck_main_loop;
	uint32_t credits_main_loop;
	uint32_t coaster_main_loop;
	uint32_t condor_main_loop;
	uint32_t chocobo_main_loop;
	uint32_t highway_main_loop;
	uint32_t swirl_main_loop;
	uint32_t snowboard_main_loop;
	uint32_t submarine_main_loop;
	uint32_t gameover_main_loop;
	uint32_t movie_module;
	uint32_t file_module;

	if(*((uint32_t *)main_loop) != 0x81EC8B55) ffnx_unexpected("odd main loop prologue\n");

	common_externals.get_time = (uint64_t (*)(uint64_t*))get_relative_call(common_externals.winmain, 0x2BE);
	common_externals.diff_time = (uint64_t (*)(uint64_t*,uint64_t*,uint64_t*))get_relative_call(common_externals.winmain, 0x2F6);

	common_externals.update_movie_sample = get_relative_call(main_loop, 0x67);

	movie_module = common_externals.update_movie_sample - 0x3039;

	ff7_externals.movie_sub_415231 = (void (*)(char*))(movie_module + 0x331);
	common_externals.prepare_movie = movie_module + 0x1A95;
	common_externals.release_movie_objects = movie_module + 0x2859;
	common_externals.start_movie = movie_module + 0x2BB0;
	common_externals.stop_movie = movie_module + 0x2CB2;
	common_externals.get_movie_frame = movie_module + 0x3713;

	ff7_externals.movie_object = (struct movie_obj *)(get_absolute_value(common_externals.prepare_movie, 0x42) - 0xC);

	common_externals._mode = (WORD*)get_absolute_value(main_loop, 0x8C);
	common_externals._previous_mode = (WORD *)get_absolute_value(main_loop, 0x1D8);

	gameover_main_loop = get_absolute_value(main_loop, 0x1FE);
	ff7_set_main_loop(MODE_GAMEOVER, gameover_main_loop);
	swirl_main_loop = get_absolute_value(main_loop, 0x25B);
	ff7_set_main_loop(MODE_SWIRL, swirl_main_loop);
	cdcheck_main_loop = get_absolute_value(main_loop, 0x397);
	ff7_set_main_loop(MODE_CDCHECK, cdcheck_main_loop);
	credits_main_loop = get_absolute_value(main_loop, 0x4CA);
	ff7_set_main_loop(MODE_CREDITS, credits_main_loop);
	menu_main_loop = get_absolute_value(main_loop, 0x62E);
	ff7_set_main_loop(MODE_MENU, menu_main_loop);
	battle_main_loop = get_absolute_value(main_loop, 0x89A);
	ff7_set_main_loop(MODE_BATTLE, battle_main_loop);
	field_main_loop = get_absolute_value(main_loop, 0x8F8);
	ff7_set_main_loop(MODE_FIELD, field_main_loop);
	worldmap_main_loop = get_absolute_value(main_loop, 0x977);
	ff7_set_main_loop(MODE_WORLDMAP, worldmap_main_loop);
	chocobo_main_loop = get_absolute_value(main_loop, 0x9C5);
	ff7_set_main_loop(MODE_CHOCOBO, chocobo_main_loop);
	condor_main_loop = get_absolute_value(main_loop, 0xA13);
	ff7_set_main_loop(MODE_CONDOR, condor_main_loop);
	highway_main_loop = get_absolute_value(main_loop, 0xA61);
	ff7_set_main_loop(MODE_HIGHWAY, highway_main_loop);
	coaster_main_loop = get_absolute_value(main_loop, 0xAAF);
	ff7_set_main_loop(MODE_COASTER, coaster_main_loop);
	submarine_main_loop = get_absolute_value(main_loop, 0xAFD);
	ff7_set_main_loop(MODE_SUBMARINE, submarine_main_loop);
	snowboard_main_loop = get_absolute_value(main_loop, 0xB3E);
	ff7_set_main_loop(MODE_SNOWBOARD, snowboard_main_loop);

	ff7_externals.reset_game_obj_sub_5F4971 = (void (*)(struct game_obj*))get_relative_call(condor_main_loop, 0x5B);
	ff7_externals.engine_exit_game_mode_sub_666C78 = get_relative_call(common_externals.winmain, 0x217);
	ff7_externals.sub_666C13 = (void* (*)(struct game_obj*))get_relative_call(ff7_externals.engine_exit_game_mode_sub_666C78, 0x35);
	ff7_externals.sub_670F9B = (void* (*)(void*))get_relative_call(ff7_externals.engine_exit_game_mode_sub_666C78, 0x47);
	ff7_externals.byte_CC0D89 = (BYTE*)get_absolute_value(main_loop, 0x71E);
	ff7_externals.word_CC0828 = (WORD*)get_absolute_value(main_init_loop, 0x4A5);

	ff7_externals.destroy_field_bk = get_relative_call(field_main_loop, 0x222);
	ff7_externals.destroy_field_tiles = get_relative_call(ff7_externals.destroy_field_bk, 0x1E6);
	ff7_externals.field_layers = (field_layer **)get_absolute_value(ff7_externals.destroy_field_tiles, 0x46);

	ff7_externals.num_field_entities = (WORD *)(((uint32_t)ff7_externals.field_layers) - 0xC);
	ff7_externals.field_objects = (field_object **)(((uint32_t)ff7_externals.field_layers) - 0x10);

	ff7_externals.field_id = (WORD *)get_absolute_value(field_main_loop, 0x326);

	ff7_externals.open_field_file = get_relative_call(field_main_loop, 0x331);
	ff7_externals.field_file_name = (char *)get_absolute_value(ff7_externals.open_field_file, 0x77);
	ff7_externals.read_field_file = get_relative_call(ff7_externals.open_field_file, 0xCF);

	ff7_externals.battle_enter = get_absolute_value(main_loop, 0x8A1);
	ff7_externals.battle_loop = get_relative_call(battle_main_loop, 0x1C8);
	ff7_externals.battle_mode = (DWORD*)get_absolute_value(ff7_externals.battle_loop, 0x18);
	ff7_externals.battle_sub_429AC0 = get_absolute_value(ff7_externals.battle_loop, 0x79);
	ff7_externals.battle_sub_42D808 = get_relative_call(ff7_externals.battle_sub_429AC0, 0xE7);
	ff7_externals.battle_sub_42D992 = get_relative_call(ff7_externals.battle_sub_42D808, 0x30);
	ff7_externals.battle_sub_42DAE5 = get_relative_call(ff7_externals.battle_sub_42D992, 0x7E);
	ff7_externals.battle_fight_end = get_relative_call(ff7_externals.battle_sub_42D992, 0xB7);
	ff7_externals.battle_fanfare_music = get_relative_call(ff7_externals.battle_fight_end, 0x25);
	ff7_externals.battle_sub_427C22 = get_relative_call(ff7_externals.battle_sub_42DAE5, 0xF);
	ff7_externals.battle_menu_update_6CE8B3 = get_relative_call(battle_main_loop, 0x368);
	ff7_externals.battle_sub_6DB0EE = get_relative_call(ff7_externals.battle_menu_update_6CE8B3, 0xD9);
	ff7_externals.is_battle_paused = (char*)get_absolute_value(ff7_externals.battle_menu_update_6CE8B3, 0xC3);
	ff7_externals.battle_actor_data = (battle_actor_data*)get_absolute_value(ff7_externals.battle_sub_6DB0EE, 0x276);
	ff7_externals.battle_menu_state_fn_table = std::span((uint32_t*)get_absolute_value(ff7_externals.battle_sub_6DB0EE, 0x1B4), 64);
	ff7_externals.magic_effects_fn_table = std::span((uint32_t*)get_absolute_value(ff7_externals.battle_sub_427C22, 0xBF), 54);
	ff7_externals.battle_b3ddata_sub_428B12 = get_relative_call(ff7_externals.battle_sub_429AC0, 0x71);
	ff7_externals.graphics_render_sub_68A638 = get_relative_call(ff7_externals.battle_b3ddata_sub_428B12, 0x10A);
	ff7_externals.create_dx_sfx_something = get_relative_call(ff7_externals.graphics_render_sub_68A638, 0xD3);
	ff7_externals.load_p_file = get_relative_call(ff7_externals.create_dx_sfx_something, 0x144);

	ff7_externals.comet2_sub_5A42E5 = get_relative_call(ff7_externals.magic_effects_fn_table[46], 0x1A);
	ff7_externals.comet2_unload_sub_5A4359 = get_absolute_value(ff7_externals.comet2_sub_5A42E5, 0x34);

	ff7_externals.create_polygon_data = (polygon_data* (*)(uint32_t, uint32_t))get_relative_call(ff7_externals.load_p_file, 0x17);
	ff7_externals.create_polygon_lists = (void (*)(polygon_data*))get_relative_call(ff7_externals.load_p_file, 0x35B);
	ff7_externals.free_polygon_data = (void (*)(polygon_data*))get_relative_call(ff7_externals.load_p_file, 0x3C4);

	common_externals.open_file = get_relative_call(ff7_externals.load_p_file, 0x3A);

	file_module = common_externals.open_file - 0xE2;
	common_externals.close_file = file_module + 0xA1;
	common_externals.read_file = file_module + 0x611;
	common_externals.__read_file = file_module + 0x6A7;
	common_externals.write_file = file_module + 0x735;
	common_externals.alloc_read_file = (void* (*)(uint32_t, uint32_t, struct file*))(file_module + 0x830);
	common_externals.get_filesize = file_module + 0x84B;
	common_externals.tell_file = file_module + 0x8A1;
	common_externals.seek_file = file_module + 0x90A;
	common_externals.alloc_get_file = (void* (*)(file_context*, uint32_t*, char*))(file_module + 0xA0E);

	common_externals.destroy_tex = (void (*)(tex_header*))get_relative_call(common_externals.load_tex_file, 0x16D);
	common_externals.destroy_tex_header = get_relative_call((uint32_t)common_externals.destroy_tex, 0x78);

	ff7_externals.play_battle_music_call = main_loop + 0x300;
	ff7_externals.play_battle_end_music = (uint32_t(*)())get_relative_call(ff7_externals.battle_fanfare_music, 0x21);
	ff7_externals.play_battle_music_win_call = ff7_externals.battle_fanfare_music + 0x21;
	ff7_externals.battle_sub_42A0E7 = get_relative_call(ff7_externals.battle_sub_429AC0, 0xA4);
	ff7_externals.load_battle_stage = get_relative_call(ff7_externals.battle_sub_42A0E7, 0x78);
	ff7_externals.load_battle_stage_pc = get_relative_call(ff7_externals.load_battle_stage, 0x151);
	ff7_externals.read_battle_hrc = get_relative_call(ff7_externals.load_battle_stage_pc, 0x25);

	ff7_externals.battle_location_id = (WORD*)get_absolute_value(ff7_externals.battle_sub_42A0E7, 0x5F);

	ff7_externals.battle_regular_chdir = (void (*)(battle_chdir_struc*))get_relative_call(ff7_externals.read_battle_hrc, 0x16);
	ff7_externals.battle_context_chdir = (void (*)(file_context*, battle_chdir_struc*))get_relative_call(ff7_externals.read_battle_hrc, 0x2B);
	ff7_externals.swap_extension = (void (*)(char*, char*, char*))get_relative_call(ff7_externals.read_battle_hrc, 0x43);
	ff7_externals.destroy_battle_hrc = (void (*)(uint32_t, battle_hrc_header*))get_relative_call(ff7_externals.read_battle_hrc, 0xB3);
	ff7_externals.battle_regular_olddir = (void (*)(battle_chdir_struc*))get_relative_call(ff7_externals.read_battle_hrc, 0xD2);
	ff7_externals.battle_context_olddir = (void (*)(file_context*, battle_chdir_struc*))get_relative_call(ff7_externals.read_battle_hrc, 0xE7);

	ff7_externals.field_sub_60DCED = get_relative_call(field_main_loop, 0x37A);
	ff7_externals.field_load_models = get_relative_call(ff7_externals.field_sub_60DCED, 0x168);
	ff7_externals.field_models_eye_to_model = get_relative_call(ff7_externals.field_load_models, 0xA79);
	ff7_externals.field_load_animation = get_relative_call(ff7_externals.field_load_models, 0x8DF);
	ff7_externals.field_load_model_eye_tex = (int (*)(ff7_model_eye_texture_data*,field_animation_data*))get_relative_call(ff7_externals.field_load_models, 0xB90);
	ff7_externals.field_load_model_tex = (p_hundred* (*)(int, int, char*, struc_3*, game_obj*))get_relative_call((uint32_t)ff7_externals.field_load_model_eye_tex, 0x9D);
	ff7_externals.field_unload_model_tex = (void (*)(void*))get_relative_call((uint32_t)ff7_externals.field_load_model_eye_tex, 0x1DE);
	ff7_externals.create_struc_3_info_sub_67455E = (void (*)(struc_3*))get_relative_call((uint32_t)ff7_externals.field_load_model_eye_tex, 0x12);
	ff7_externals.load_animation = get_relative_call(ff7_externals.field_load_animation, 0x16D);
	ff7_externals.destroy_animation = (void (*)(anim_header*))get_relative_call(ff7_externals.load_animation, 0x162);
	ff7_externals.field_unk_909288 = (uint32_t*)get_absolute_value((uint32_t)ff7_externals.field_load_model_eye_tex, 0x1D);

	ff7_externals.field_models_eye_blink_buffer = (ff7_model_eye_texture_data*)get_absolute_value(ff7_externals.field_load_models, 0xB8C);
	ff7_externals.field_models_data = (DWORD*)get_absolute_value(ff7_externals.field_load_models, 0xE);

	ff7_externals.load_lgp = get_relative_call(main_loop, 0x450);
	ff7_externals.open_lgp_file = get_relative_call(ff7_externals.load_lgp, 0x1C);
	ff7_externals.__read = get_relative_call(common_externals.read_file, 0x4A);

	ff7_externals.lgp_open_file = get_relative_call((uint32_t)common_externals.open_file, 0x234);
	ff7_externals.lgp_seek_file = get_relative_call((uint32_t)common_externals.open_file, 0x265);
	ff7_externals.lgp_read = get_relative_call((uint32_t)common_externals.read_file, 0x2E);
	ff7_externals.lgp_get_filesize = get_relative_call((uint32_t)ff7_externals.read_field_file, 0x71);
	ff7_externals.lgp_read_file = get_relative_call((uint32_t)ff7_externals.read_field_file, 0xDD);

	ff7_externals.lzss_decode = (int (*)(char*, char*))get_relative_call((uint32_t)ff7_externals.read_field_file, 0xF2);
	ff7_externals.field_file_buffer = (char**)get_absolute_value((uint32_t)ff7_externals.read_field_file, 0xB2);
	ff7_externals.field_file_section_ptrs = (DWORD*)get_absolute_value((uint32_t)ff7_externals.read_field_file, 0x187);
	ff7_externals.known_field_buffer_size = (uint32_t*)get_absolute_value((uint32_t)ff7_externals.read_field_file, 0xA4);
	ff7_externals.field_CFF268 = (uint32_t*)get_absolute_value((uint32_t)ff7_externals.read_field_file, 0xB);

	ff7_externals.lgp_fds = (FILE **)get_absolute_value(ff7_externals.lgp_seek_file, 0x17);

	ff7_externals.context_chdir = get_relative_call((uint32_t)ff7_externals.battle_context_chdir, 0x3C);
	ff7_externals.lgp_chdir = get_relative_call(ff7_externals.context_chdir, 0x2A);

	ff7_externals.lgp_lookup_tables = (lookup_table_entry **)get_absolute_value(ff7_externals.lgp_open_file, 0x194);
	ff7_externals.lgp_tocs = (lgp_toc_entry **)get_absolute_value(ff7_externals.lgp_open_file, 0x233);
	ff7_externals.lgp_folders = (lgp_folders *)get_absolute_value(ff7_externals.lgp_open_file, 0x42C);

	ff7_externals.battle_sub_437DB0 = get_absolute_value(ff7_externals.battle_loop, 0x8D);
	ff7_externals.sub_5CB2CC = get_relative_call(ff7_externals.battle_sub_437DB0, 0x43);
	ff7_externals.battle_formation_id = (WORD*)get_absolute_value(ff7_externals.battle_sub_437DB0, 0x1FD);
	ff7_externals.battle_scene_bin_sub_5D1050 = get_relative_call(ff7_externals.battle_sub_437DB0, 0x15D);
	ff7_externals.engine_load_bin_file_sub_419210 = (int (*)(char *filename, int offset, int size, char **out_buffer, void (*callback)(void)))(get_relative_call(ff7_externals.battle_scene_bin_sub_5D1050, 0x85));

	ff7_externals.play_midi = (void (*)(uint32_t))common_externals.play_midi;
	common_externals.master_midi_volume = (DWORD *)get_absolute_value(common_externals.set_master_midi_volume, 0x46);
	ff7_externals.midi_volume_control = (uint32_t *)get_absolute_value(common_externals.midi_init, 0x706);
	ff7_externals.midi_initialized = (uint32_t *)get_absolute_value(common_externals.midi_init, 0x3A);

	ff7_externals.menu_sub_6CDA83 = get_relative_call(menu_main_loop, 0x112);
	ff7_externals.menu_sub_6CBD43 = get_relative_call(ff7_externals.menu_sub_6CDA83, 0xAF);
	ff7_externals.menu_sub_701EE4 = get_relative_call(ff7_externals.menu_sub_6CBD43, 0x7);
	ff7_externals.phs_menu_sub = get_relative_call(ff7_externals.menu_sub_701EE4, 0xE3);
	ff7_externals.menu_battle_end_sub_6C9543 = get_relative_call(ff7_externals.menu_sub_6CDA83, 0x20);

	switch(version)
	{
		case VERSION_FF7_102_US:
			ff7_externals.menu_draw_party_member_stats = get_relative_call(ff7_externals.phs_menu_sub, 0x8FF);
			break;
		default:
			ff7_externals.menu_draw_party_member_stats = get_relative_call(ff7_externals.phs_menu_sub, 0x8F5);
			break;
	}

	ff7_externals.party_member_to_char_map = (uint32_t *)get_absolute_value(ff7_externals.menu_draw_party_member_stats, 0x14);

	ff7_externals.menu_start = get_absolute_value(main_loop, 0x627);
	ff7_externals.menu_sub_6CB56A = get_relative_call(ff7_externals.menu_sub_6CDA83, 0xDE);
	ff7_externals.menu_subs_call_table = (uint32_t *)get_absolute_value(ff7_externals.menu_sub_6CB56A, 0x2EC);
	ff7_externals.menu_tutorial_sub_6C49FD = (int (*)())get_relative_call(ff7_externals.menu_sub_6CB56A, 0x2B7);
	ff7_externals.timer_menu_sub = ff7_externals.menu_subs_call_table[0];
	ff7_externals.status_menu_sub = ff7_externals.menu_subs_call_table[5];
	ff7_externals.config_menu_sub = ff7_externals.menu_subs_call_table[8];
	ff7_externals.menu_sub_6FEDB0 = ff7_externals.menu_subs_call_table[10];

  ff7_externals.config_initialize = get_relative_call(main_init_loop, 0x4B0);

	ff7_externals.menu_tutorial_window_state = (BYTE*)get_absolute_value((uint32_t)ff7_externals.menu_tutorial_sub_6C49FD, 0x9);
	ff7_externals.menu_tutorial_window_text_ptr = (DWORD*)get_absolute_value((uint32_t)ff7_externals.menu_tutorial_sub_6C49FD, 0x18);

	switch(version)
	{
		case VERSION_FF7_102_US:
			ff7_externals.write_save_file = (BOOL (*)(char))(get_relative_call(ff7_externals.menu_sub_6FEDB0, 0x1096));
			break;
		case VERSION_FF7_102_DE:
		case VERSION_FF7_102_FR:
			ff7_externals.write_save_file = (BOOL (*)(char))(get_relative_call(ff7_externals.menu_sub_6FEDB0, 0x10B2));
			break;
		case VERSION_FF7_102_SP:
			ff7_externals.write_save_file = (BOOL (*)(char))(get_relative_call(ff7_externals.menu_sub_6FEDB0, 0x10FE));
			break;
	}

	ff7_externals.millisecond_counter = (DWORD *)get_absolute_value(ff7_externals.timer_menu_sub, 0xD06);
	ff7_externals.draw_status_limit_level_stats = get_relative_call(ff7_externals.status_menu_sub, 0x8E);

	ff7_externals.menu_sub_6F5C0C = (void *(*)(uint32_t, uint32_t, uint8_t, uint8_t, uint32_t))(get_relative_call(ff7_externals.timer_menu_sub, 0x72F));
	ff7_externals.menu_sub_6FAC38 = (void *(*)(uint32_t, uint32_t, uint8_t, uint8_t, uint32_t))(get_relative_call(ff7_externals.timer_menu_sub, 0xD77));

	ff7_externals.get_kernel_text = (char* (*)(uint32_t, uint32_t, uint32_t))get_relative_call(ff7_externals.draw_status_limit_level_stats, 0x10C);

	ff7_externals.sub_5CF282 = get_relative_call(ff7_externals.sub_5CB2CC, 0x4E);
	ff7_externals.get_equipment_stats = get_relative_call(ff7_externals.sub_5CF282, 0x2F0);

	ff7_externals.weapon_data_array = (weapon_data *)(get_absolute_value(ff7_externals.get_equipment_stats, 0x50) - 4);
	ff7_externals.armor_data_array = (armor_data *)(get_absolute_value(ff7_externals.get_equipment_stats, 0x78) - 2);

	ff7_externals.field_sub_6388EE = get_relative_call(field_main_loop, 0xFF);
	ff7_externals.field_draw_everything = get_relative_call(ff7_externals.field_sub_6388EE, 0x11);
	ff7_externals.field_draw_pointer_hand_60D4F3 = get_relative_call(ff7_externals.field_draw_everything, 0x1A9);
	ff7_externals.field_submit_draw_pointer_hand_60D572 = get_relative_call(ff7_externals.field_draw_pointer_hand_60D4F3, 0x4F);
	ff7_externals.field_pick_tiles_make_vertices = get_relative_call(ff7_externals.field_draw_everything, 0xC9);
	ff7_externals.field_layer1_pick_tiles = get_relative_call(ff7_externals.field_pick_tiles_make_vertices, 0x2D);
	ff7_externals.field_layer1_tiles_num = (uint32_t *)get_absolute_value(ff7_externals.field_layer1_pick_tiles, 0x8B);
	ff7_externals.field_layer1_palette_sort = (uint32_t **)get_absolute_value(ff7_externals.field_layer1_pick_tiles, 0xA2);
	ff7_externals.field_layer1_tiles = (field_tile **)get_absolute_value(ff7_externals.field_layer1_pick_tiles, 0xBF);
	ff7_externals.field_layer2_pick_tiles = get_relative_call(ff7_externals.field_pick_tiles_make_vertices, 0x48);
	ff7_externals.field_layer2_tiles_num = (uint32_t *)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0x8C);
	ff7_externals.field_layer2_palette_sort = (uint32_t **)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0xA3);
	ff7_externals.field_layer2_tiles = (field_tile **)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0xC0);
	ff7_externals.field_layer3_pick_tiles = get_relative_call(ff7_externals.field_pick_tiles_make_vertices, 0x12);
	ff7_externals.field_layer3_tiles_num = (uint32_t *)get_absolute_value(ff7_externals.field_layer3_pick_tiles, 0xAB);
	ff7_externals.field_layer3_palette_sort = (uint32_t **)get_absolute_value(ff7_externals.field_layer3_pick_tiles, 0xC1);
	ff7_externals.field_layer3_tiles = (field_tile **)get_absolute_value(ff7_externals.field_layer3_pick_tiles, 0xDD);
	ff7_externals.do_draw_layer3_CFFE3C = (int*)get_absolute_value(ff7_externals.field_layer3_pick_tiles, 0x9);
	ff7_externals.field_layer3_flag_CFFE40 = (int*)get_absolute_value(ff7_externals.field_layer3_pick_tiles, 0x3B1);
	ff7_externals.field_layer4_pick_tiles = get_relative_call(ff7_externals.field_pick_tiles_make_vertices, 0x5F);
	ff7_externals.field_layer4_tiles_num = (uint32_t *)get_absolute_value(ff7_externals.field_layer4_pick_tiles, 0x90);
	ff7_externals.field_layer4_palette_sort = (uint32_t **)get_absolute_value(ff7_externals.field_layer4_pick_tiles, 0xA6);
	ff7_externals.field_layer4_tiles = (field_tile **)get_absolute_value(ff7_externals.field_layer4_pick_tiles, 0xC2);
	ff7_externals.do_draw_layer4_CFFEA4 = (int*)get_absolute_value(ff7_externals.field_layer4_pick_tiles, 0x9);
	ff7_externals.field_layer4_flag_CFFEA8 = (int*)get_absolute_value(ff7_externals.field_layer4_pick_tiles, 0x3F3);
	ff7_externals.field_layer_sub_623C0F = (double(*)(rotation_matrix*, int, int, int))get_relative_call(ff7_externals.field_layer3_pick_tiles, 0x7E);
	ff7_externals.field_layer_CFF1D8 = (int *)get_absolute_value(ff7_externals.field_layer4_pick_tiles, 0x264);
	ff7_externals.field_palette_D00088 = (uint16_t *)get_absolute_value(ff7_externals.field_layer4_pick_tiles, 0x28A);
	ff7_externals.field_special_y_offset = (uint32_t *)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0x43);
	ff7_externals.field_bg_multiplier = (uint32_t *)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0x23);
	ff7_externals.add_page_tile = (void (*)(float, float, float, float, float, uint32_t, uint32_t))get_relative_call(ff7_externals.field_layer2_pick_tiles, 0x327);
	ff7_externals.field_triggers_header = (field_trigger_header**)get_absolute_value(ff7_externals.field_layer3_pick_tiles, 0x134);
	ff7_externals.field_camera_rotation_matrix_CFF3D8 = (rotation_matrix*)get_absolute_value(ff7_externals.field_layer3_pick_tiles, 0x7A);
	ff7_externals.field_draw_gray_quads_644E90 = (void(*)())get_relative_call(ff7_externals.field_draw_everything, 0x360);
	ff7_externals.engine_draw_graphics_object = (void(*)(ff7_graphics_object*, ff7_game_obj*))get_relative_call(ff7_externals.field_draw_everything, 0x1D2);

	ff7_externals.field_load_textures = get_relative_call(ff7_externals.field_sub_60DCED, 0x107);
	ff7_externals.field_convert_type2_layers = (void (*)())get_relative_call(ff7_externals.field_load_textures, 0xD);
	ff7_externals.make_struc3 = (void (*)(uint32_t, struc_3*))get_relative_call(ff7_externals.field_load_textures, 0xAC);
	ff7_externals.make_field_tex_header_pal = (void (*)(ff7_tex_header*))get_relative_call(ff7_externals.field_load_textures, 0x21F);
	ff7_externals.make_field_tex_header = (void (*)(ff7_tex_header*))get_relative_call(ff7_externals.field_load_textures, 0x23C);
	ff7_externals._load_texture = (ff7_graphics_object* (*)(uint32_t, uint32_t, struc_3*, char*, void*))get_relative_call(ff7_externals.field_load_textures, 0x2F8);

	ff7_externals.read_field_background_data = get_relative_call(ff7_externals.field_sub_60DCED, 0x8B);
	ff7_externals.layer2_end_page = (WORD *)get_absolute_value(ff7_externals.read_field_background_data, 0x788);

	ff7_externals.create_d3d2_indexed_primitive = get_relative_call((uint32_t)common_externals.generic_load_group, 0x22);
	ff7_externals.destroy_d3d2_indexed_primitive = get_relative_call(ff7_externals.create_d3d2_indexed_primitive, 0x290);

	ff7_externals.enter_main = get_absolute_value(worldmap_main_loop, 0x2AE);

	ff7_externals.kernel_init = get_relative_call(ff7_externals.enter_main, 0xF1);
	ff7_externals.kernel_load_kernel2 = (void (*)(char*))get_relative_call(ff7_externals.kernel_init, 0x1FD);
	ff7_externals.kernel2_reset_counters = get_relative_call((uint32_t)ff7_externals.kernel_load_kernel2, 0x33);

	ff7_externals.sub_4012DA = get_absolute_value(ff7_externals.kernel_init, 0x136);
	ff7_externals.kernel2_add_section = get_relative_call(ff7_externals.sub_4012DA, 0x4D);
	ff7_externals.kernel2_get_text = get_relative_call((uint32_t)ff7_externals.get_kernel_text, 0xF7);
	ff7_externals.kernel_1to9_sections = (char**)get_absolute_value(ff7_externals.sub_4012DA, 0x6F);

	ff7_externals.draw_3d_model = get_relative_call(ff7_externals.field_draw_everything, 0x17F);
	ff7_externals.stack_push = (void (*)(struct stack*))get_relative_call(ff7_externals.draw_3d_model, 0x8E);
	ff7_externals.stack_top = (void* (*)(struct stack*))get_relative_call(ff7_externals.draw_3d_model, 0x9A);
	ff7_externals.stack_pop = (void (*)(struct stack*))get_relative_call(ff7_externals.draw_3d_model, 0x8FD);
	ff7_externals._root_animation = (void (*)(matrix*, anim_frame*, anim_header*, hrc_data*))get_absolute_value(ff7_externals.draw_3d_model, 0xD4);
	ff7_externals._frame_animation = (void (*)(uint32_t, matrix*, vector3<float>*, anim_frame*, anim_header*, hrc_bone*, hrc_data*))get_absolute_value(ff7_externals.draw_3d_model, 0xDB);
	ff7_externals.root_animation = (void (*)(matrix*, anim_frame*, anim_header*, hrc_data*))get_absolute_value(ff7_externals.draw_3d_model, 0xE7);
	ff7_externals.frame_animation = (void (*)(uint32_t, matrix*, vector3<float>*, anim_frame*, anim_header*, hrc_bone*, hrc_data*))get_absolute_value(ff7_externals.draw_3d_model, 0xEE);
	ff7_externals.model_mode = (uint32_t *)get_absolute_value(ff7_externals.draw_3d_model, 0x2A7);

	ff7_externals.name_menu_sub_6CBD32 = get_relative_call(ff7_externals.menu_sub_6CDA83, 0x9A);
	ff7_externals.name_menu_sub_719C08 = get_relative_call(ff7_externals.name_menu_sub_6CBD32, 0x7);

	switch(version)
	{
		case VERSION_FF7_102_FR:
			ff7_externals.menu_sub_71894B = get_relative_call(ff7_externals.name_menu_sub_719C08, 0x2B);
			ff7_externals.menu_sub_718DBE = get_relative_call(ff7_externals.name_menu_sub_719C08, 0x77);
			ff7_externals.menu_sub_719B81 = get_relative_call(ff7_externals.name_menu_sub_719C08, 0xCC);

			ff7_externals.set_default_input_settings_save = get_relative_call(ff7_externals.menu_sub_71894B, 0x189);
			break;
		default:
			ff7_externals.menu_sub_71894B = get_relative_call(ff7_externals.name_menu_sub_719C08, 0x2A);
			ff7_externals.menu_sub_718DBE = get_relative_call(ff7_externals.name_menu_sub_719C08, 0x76);
			ff7_externals.menu_sub_719B81 = get_relative_call(ff7_externals.name_menu_sub_719C08, 0xCB);

			ff7_externals.set_default_input_settings_save = get_relative_call(ff7_externals.menu_sub_71894B, 0x188);
			break;
	}

	ff7_externals.keyboard_name_input = get_relative_call(ff7_externals.menu_sub_718DBE, 0x99);
 	ff7_externals.restore_input_settings = get_relative_call(ff7_externals.menu_sub_719B81, 0x80);

	ff7_externals.dinput_getdata2 = get_relative_call(ff7_externals.keyboard_name_input, 0x1C);
	common_externals.get_keyboard_state = get_relative_call(ff7_externals.keyboard_name_input, 0x6);

	ff7_externals.init_game = get_absolute_value(ff7_externals.init_stuff, 0x336);
	ff7_externals.sub_41A1B0 = get_relative_call(ff7_externals.init_game, 0x85);
	ff7_externals.init_directinput = get_relative_call(ff7_externals.sub_41A1B0, 0x34);
	ff7_externals.dinput_createdevice_mouse = get_relative_call(ff7_externals.init_directinput, 0x48);

	common_externals.dinput_acquire_keyboard = (int (*)())get_relative_call(common_externals.get_keyboard_state, 0x4F);
	common_externals.keyboard_device = (IDirectInputDeviceA**)get_absolute_value(common_externals.get_keyboard_state, 0x06);
	common_externals.keyboard_connected = (uint32_t*)get_absolute_value(common_externals.get_keyboard_state, 0x47);

	ff7_externals.sub_69C69F = (void (*)(matrix*, ff7_light*))get_relative_call(ff7_externals.draw_3d_model, 0x882);

	ff7_externals.coaster_sub_5E9051 = get_relative_call(coaster_main_loop, 0xC6);
	ff7_externals.coaster_sub_5EE150 = get_relative_call(ff7_externals.coaster_sub_5E9051, 0x3);

	ff7_externals.cleanup_game = get_absolute_value(ff7_externals.init_stuff, 0x350);
	common_externals.midi_cleanup = get_relative_call(ff7_externals.cleanup_game, 0x72);

	ff7_externals.sub_60DF96 = get_relative_call(ff7_externals.init_game, 0x42B);
	ff7_externals.sub_60EEB2 = get_relative_call(ff7_externals.sub_60DF96, 0x26);
	ff7_externals.open_flevel_siz = get_relative_call(ff7_externals.sub_60EEB2, 0x79F);
	ff7_externals.field_map_infos = get_absolute_value(ff7_externals.open_flevel_siz, 0xAF) - 0xBC;

	common_externals.sfx_init = get_relative_call(main_init_loop, 0xC3);
	ff7_externals.sfx_initialized = (uint32_t*)get_absolute_value(common_externals.sfx_init, 0x21);
	common_externals.sfx_release = get_relative_call(common_externals.sfx_init, 0x1FC);
	common_externals.sfx_cleanup = get_relative_call(main_cleanup_loop, 0x64);
	common_externals.sfx_load = get_relative_call(main_init_loop, 0xE3);
	common_externals.sfx_unload = get_relative_call(main_cleanup_loop, 0x5C);
	ff7_externals.sound_operation = get_relative_call(ff7_externals.enter_main, 0xE4);
	common_externals.sfx_pause = get_relative_call(ff7_externals.sound_operation, 0x6E3);
	common_externals.sfx_resume = get_relative_call(ff7_externals.sound_operation, 0x6F1);
	common_externals.sfx_stop = get_relative_call(ff7_externals.sound_operation, 0x290);
	common_externals.play_sfx_on_channel = get_relative_call(ff7_externals.sound_operation, 0x2AB);
	common_externals.set_sfx_volume_on_channel = (uint32_t(*)(uint32_t, uint32_t))get_relative_call(ff7_externals.sound_operation, 0x3B3);
	common_externals.set_sfx_volume_trans_on_channel = (uint32_t(*)(uint32_t, uint32_t, uint32_t))get_relative_call(ff7_externals.sound_operation, 0x437);
	common_externals.set_sfx_panning_on_channel = (uint32_t(*)(uint32_t, uint32_t))get_relative_call(ff7_externals.sound_operation, 0x4C7);
	common_externals.set_sfx_panning_trans_on_channel = (uint32_t(*)(uint32_t, uint32_t, uint32_t))get_relative_call(ff7_externals.sound_operation, 0x54B);
	common_externals.set_sfx_frequency_on_channel = (uint32_t(*)(uint32_t, uint32_t))get_relative_call(ff7_externals.sound_operation, 0x5DB);
	common_externals.set_sfx_frequency_trans_on_channel = (uint32_t(*)(uint32_t, uint32_t, uint32_t))get_relative_call(ff7_externals.sound_operation, 0x65F);
	common_externals.dsound_volume_table = (uint32_t*)get_absolute_value(uint32_t(common_externals.set_sfx_volume_on_channel), 0xCC);
	common_externals.play_sfx = (uint32_t(*)(uint32_t))get_relative_call(ff7_externals.sound_operation, 0x703);
	common_externals.play_sfx_effects = (uint32_t(*)(byte, uint32_t, uint32_t, uint32_t, uint32_t))get_relative_call(ff7_externals.sound_operation, 0x327);
	ff7_externals.sfx_play_effects_id_channel_6 = (DWORD *)get_absolute_value((uint32_t)common_externals.play_sfx_effects, 0x119);
	ff7_externals.sound_states = (ff7_field_sfx_state*)get_absolute_value(common_externals.play_sfx_on_channel, 0x28);
	common_externals.master_sfx_volume = (uint32_t*)get_absolute_value(common_externals.play_sfx_on_channel, 0x342);
	ff7_externals.sfx_fmt_header = (ff7_audio_fmt*)get_absolute_value(common_externals.sfx_load, 0x51);

	common_externals.directsound_create = get_relative_call(common_externals.sfx_init, 0x11C);
	common_externals.directsound = (LPLPDIRECTSOUND)get_absolute_value(common_externals.directsound_create, 0x15);
	common_externals.directsound_createsoundbuffer = get_relative_call(common_externals.sfx_init, 0x16C);
	common_externals.directsound_release = get_relative_call(common_externals.sfx_init, 0x182);

	ff7_externals.battle_clear_sound_flags = get_relative_call(ff7_externals.battle_sub_429AC0, 0x6C);
	ff7_externals.swirl_sound_effect = get_relative_call(swirl_main_loop, 0x8B);
	ff7_externals.sfx_load_and_play_with_speed = get_relative_call(ff7_externals.swirl_sound_effect, 0x3E);

	ff7_externals.field_initialize_variables = get_relative_call(ff7_externals.field_sub_60DCED, 0x178);
	ff7_externals.music_lock_clear_fix = ff7_externals.field_initialize_variables + 0x2B8;
	ff7_externals.field_init_player_character_variables = get_relative_call(ff7_externals.field_initialize_variables, 0x31B);
	ff7_externals.field_init_event_wrapper_63BCA7 = get_relative_call(ff7_externals.field_initialize_variables, 0x29D);
	ff7_externals.field_init_event_60BACF = get_relative_call(ff7_externals.field_init_event_wrapper_63BCA7, 0x8);
	ff7_externals.field_init_field_objects_60BCFA = get_relative_call(ff7_externals.field_init_event_60BACF, 0x39);
	ff7_externals.execute_opcode = get_relative_call(ff7_externals.field_init_event_60BACF, 0x80);
	ff7_externals.modules_global_object = (ff7_modules_global_object*)get_absolute_value(ff7_externals.field_init_event_60BACF, 0x20);
	ff7_externals.field_global_object_ptr = (ff7_modules_global_object**)get_absolute_value(ff7_externals.field_init_event_60BACF, 0x1C);

	common_externals.execute_opcode_table = (uint32_t*)get_absolute_value(ff7_externals.execute_opcode, 0x10D);
	ff7_externals.opcode_tutor = common_externals.execute_opcode_table[0x21];
	ff7_externals.opcode_goldu = common_externals.execute_opcode_table[0x39];
	ff7_externals.opcode_dlitm = common_externals.execute_opcode_table[0x59];
	ff7_externals.opcode_smtra = common_externals.execute_opcode_table[0x5B];
	ff7_externals.opcode_akao2 = common_externals.execute_opcode_table[0xDA];
	ff7_externals.opcode_akao = common_externals.execute_opcode_table[0xF2];
	ff7_externals.opcode_bmusc = common_externals.execute_opcode_table[0xF6];
	ff7_externals.opcode_fmusc = common_externals.execute_opcode_table[0xFC];
	ff7_externals.opcode_cmusc = common_externals.execute_opcode_table[0xFD];
	ff7_externals.opcode_gameover = common_externals.execute_opcode_table[0xFF];
	ff7_externals.opcode_message = common_externals.execute_opcode_table[0x40];
	ff7_externals.opcode_ask = common_externals.execute_opcode_table[0x48];
	ff7_externals.opcode_canm1_canm2 = common_externals.execute_opcode_table[0xB1];
	ff7_externals.opcode_wmode = common_externals.execute_opcode_table[0x52];
	ff7_externals.opcode_fade = common_externals.execute_opcode_table[0x6B];
	ff7_externals.opcode_shake = common_externals.execute_opcode_table[0x5E];
	ff7_externals.opcode_setbyte = common_externals.execute_opcode_table[0x80];
	ff7_externals.opcode_biton = common_externals.execute_opcode_table[0x82];
	ff7_externals.opcode_pc = common_externals.execute_opcode_table[0xA0];
	ff7_externals.opcode_kawai = common_externals.execute_opcode_table[0x28];

	ff7_externals.field_opcode_08_sub_61D0D4 = get_relative_call(common_externals.execute_opcode_table[0x08], 0x5A);
	ff7_externals.field_opcode_08_09_set_rotation_61DB2C = (void(*)(short, byte, byte))get_relative_call(ff7_externals.field_opcode_08_sub_61D0D4, 0x196);
	ff7_externals.field_opcode_AA_2A_sub_616476 = get_relative_call(common_externals.execute_opcode_table[0xAA], 0x26);
	ff7_externals.field_opcode_turn_character_sub_616CB5 = get_relative_call(common_externals.execute_opcode_table[0xAB], 0x28);
	ff7_externals.field_get_rotation_final_636515 = (int(*)(vector3<int>*, vector3<int>*, int*))get_relative_call(ff7_externals.field_opcode_turn_character_sub_616CB5, 0x34F);

	ff7_externals.field_event_data_ptr = (field_event_data**)get_absolute_value(ff7_externals.opcode_canm1_canm2, 0xC1);
	ff7_externals.field_animation_data_ptr = (field_animation_data**)get_absolute_value(ff7_externals.opcode_canm1_canm2, 0x199);
	ff7_externals.field_model_id_array = (byte*)get_absolute_value(ff7_externals.opcode_canm1_canm2, 0x12);
	ff7_externals.animation_type_array = (char*)get_absolute_value(ff7_externals.opcode_canm1_canm2, 0x5D);
	ff7_externals.word_DB958A = (WORD *)get_absolute_value(common_externals.execute_opcode_table[0x23], 0x5);

	ff7_externals.field_opcode_message_update_loop_630D50 = get_relative_call(ff7_externals.opcode_message, 0x3B);
	ff7_externals.field_text_box_window_create_631586 = get_relative_call(ff7_externals.field_opcode_message_update_loop_630D50, 0x39);
	ff7_externals.field_text_box_window_opening_6317A9 = (void (*)(short))get_relative_call(ff7_externals.field_opcode_message_update_loop_630D50, 0x5A);
	ff7_externals.field_text_box_window_paging_631945 = (void (*)(short))get_relative_call(ff7_externals.field_opcode_message_update_loop_630D50, 0x6D);;
	ff7_externals.sub_6CB9B8 = (byte* (*)(int))get_relative_call((uint32_t)ff7_externals.field_text_box_window_paging_631945, 0x693);
	ff7_externals.field_text_box_window_reverse_paging_632CAA = get_relative_call(ff7_externals.field_opcode_message_update_loop_630D50, 0x80);
	ff7_externals.field_text_box_window_closing_632EB8 = get_relative_call(ff7_externals.field_opcode_message_update_loop_630D50, 0x235);
	ff7_externals.opcode_message_loop_code = (WORD*)get_absolute_value(ff7_externals.field_opcode_message_update_loop_630D50, 0x12);
	ff7_externals.current_dialog_string_pointer = (DWORD*)get_absolute_value(ff7_externals.field_text_box_window_create_631586, 0x154);
	ff7_externals.current_dialog_message_speed = (WORD*)get_absolute_value(ff7_externals.field_text_box_window_create_631586, 0x1C1);
	ff7_externals.field_entity_id_list = (char*)get_absolute_value(ff7_externals.field_text_box_window_create_631586, 0x1F);

	ff7_externals.field_opcode_ask_update_loop_6310A1 = (int (*)(uint8_t, uint8_t, uint8_t, uint8_t, WORD*))get_relative_call(ff7_externals.opcode_ask, 0x8E);
	ff7_externals.opcode_ask_question_code = (WORD*)get_absolute_value((uint32_t)ff7_externals.field_opcode_ask_update_loop_6310A1, 0x2FE);

	ff7_externals.field_music_helper = get_relative_call(ff7_externals.opcode_cmusc, 0x5E);
	ff7_externals.field_music_id_to_midi_id = (uint32_t (*)(int16_t))get_relative_call(ff7_externals.field_music_helper, 0x3B);
	ff7_externals.field_music_id_to_midi_id_call1 = ff7_externals.field_music_helper + 0x3B;
	ff7_externals.field_music_id_to_midi_id_call2 = ff7_externals.opcode_bmusc + 0x37;
	ff7_externals.field_music_id_to_midi_id_call3 = ff7_externals.opcode_fmusc + 0x37;

	switch(version)
	{
		case VERSION_FF7_102_DE:
			ff7_externals.field_music_helper_sound_op_call = ff7_externals.field_music_helper + 0x147;
			break;
		default:
			ff7_externals.field_music_helper_sound_op_call = ff7_externals.field_music_helper + 0x106;
			break;
	}

	ff7_externals.enter_gameover = get_absolute_value(main_loop, 0x1F7);
	ff7_externals.exit_gameover = get_absolute_value(main_loop, 0x213);
	ff7_externals.start_gameover = (void* (*)())get_relative_call(ff7_externals.enter_gameover, 0xC6);
	ff7_externals.gameover_sub_6C12B1 = (void* (*)())get_relative_call(ff7_externals.exit_gameover, 0x21);
	ff7_externals.on_gameover_enter = ff7_externals.enter_gameover + 0xC6;
	ff7_externals.on_gameover_exit = ff7_externals.exit_gameover + 0x21;

	ff7_externals.enter_field = get_absolute_value(main_loop, 0x90D);
	ff7_externals.field_init_viewport_values = get_relative_call(main_init_loop, 0x375);
	ff7_externals.field_loop_sub_63C17F = get_relative_call(field_main_loop, 0x59);
	ff7_externals.field_update_models_positions = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0x5DD);
	ff7_externals.field_update_single_model_position = (int (*)(int16_t))get_relative_call(ff7_externals.field_update_models_positions, 0x8BC);
	ff7_externals.field_update_model_animation_frame = (void (*)(int16_t))get_relative_call(ff7_externals.field_update_models_positions, 0x68D);
	ff7_externals.field_check_collision_with_target = (int (*)(field_event_data*, short))get_relative_call(ff7_externals.field_update_models_positions, 0x9AA);
	ff7_externals.field_get_linear_interpolated_value = (int (*)(int, int, int, int))get_relative_call(ff7_externals.field_update_models_positions, 0x122);
	ff7_externals.field_get_smooth_interpolated_value = (int (*)(int, int, int, int))get_relative_call(ff7_externals.field_update_models_positions, 0x1EC);
	ff7_externals.field_evaluate_encounter_rate_60B2C6 = (void (*)())get_relative_call(ff7_externals.field_update_models_positions, 0x90F);
	ff7_externals.field_animate_3d_models_6392BB = get_relative_call(field_main_loop, 0xF6);
	ff7_externals.field_blink_3d_model_649B50 = (void(*)(field_animation_data*, field_model_blink_data*))get_relative_call(ff7_externals.field_animate_3d_models_6392BB, 0x8A7);
	ff7_externals.field_sub_6A2736 = (int (*)(ff7_polygon_set*))get_relative_call((uint32_t)ff7_externals.field_blink_3d_model_649B50, 0xC4);
	ff7_externals.field_sub_6A2782 = (p_hundred** (*)(int idx, p_hundred *hundreddata, ff7_polygon_set *polygon_set))(get_relative_call(uint32_t(ff7_externals.field_blink_3d_model_649B50), 0x143));
	ff7_externals.field_model_blink_data_D000C8 = (field_model_blink_data*)get_absolute_value(ff7_externals.field_animate_3d_models_6392BB, 0x7E6);
	ff7_externals.field_apply_kawai_op_64A070 = get_relative_call(ff7_externals.field_animate_3d_models_6392BB, 0x726);
	ff7_externals.sub_64EC60 = get_relative_call(ff7_externals.field_apply_kawai_op_64A070, 0x964);
	ff7_externals.field_player_model_id = (short*)get_absolute_value(ff7_externals.field_update_models_positions, 0x45D);
	ff7_externals.field_n_models = (WORD*)get_absolute_value(ff7_externals.field_update_models_positions, 0x25);
	ff7_externals.field_update_camera_data = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0xFD);
	ff7_externals.field_camera_data = (ff7_camdata**)get_absolute_value(ff7_externals.field_update_camera_data, 0x84);
	ff7_externals.sub_40B27B = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0xEE);
	ff7_externals.word_CC0DD4 = (WORD*)get_absolute_value(ff7_externals.enter_field, 0x124);
	ff7_externals.word_CC1638 = (WORD*)get_absolute_value(ff7_externals.sub_40B27B, 0x25);
	ff7_externals.field_init_scripted_bg_movement = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0x182);
	ff7_externals.field_update_scripted_bg_movement = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0x187);
	ff7_externals.field_update_background_positions = (void (*)())get_relative_call(ff7_externals.field_loop_sub_63C17F, 0x1A6);
	ff7_externals.compute_and_submit_draw_gateways_arrows_64DA3B = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0x62C);
	ff7_externals.field_submit_draw_arrow_63A171 = (void(*)(field_arrow_graphics_data*))get_relative_call(ff7_externals.compute_and_submit_draw_gateways_arrows_64DA3B, 0x357);
	ff7_externals.field_sub_64314F = get_relative_call((uint32_t)ff7_externals.field_update_background_positions, 0x288);
	ff7_externals.set_world_pos_based_on_player_pos_643C86 = (void(*)(vector2<short>*))get_relative_call(ff7_externals.field_update_scripted_bg_movement, 0x3D);
	ff7_externals.field_clip_with_camera_range_6438F6 = (void(*)(vector2<short>*))get_relative_call((uint32_t)ff7_externals.field_update_background_positions, 0x2B7);
	ff7_externals.field_layer3_clip_with_camera_range_643628 = get_relative_call((uint32_t)ff7_externals.field_update_background_positions, 0x2CA);
	ff7_externals.engine_set_game_engine_delta_values_661976 = (void (*)(int, int))get_relative_call(ff7_externals.field_sub_64314F, 0x2D);
	ff7_externals.engine_apply_matrix_product_66307D = get_relative_call(ff7_externals.field_sub_64314F, 0x45);
	ff7_externals.engine_convert_psx_matrix_to_float_matrix_row_version_661465 = (void (*)(rotation_matrix*, float*))get_relative_call(ff7_externals.engine_apply_matrix_product_66307D, 0x23);
	ff7_externals.engine_apply_matrix_product_to_vector_66CF7E = (void (*)(float*, vector3<float>*, vector3<float>*))get_relative_call(ff7_externals.engine_apply_matrix_product_66307D, 0x37);
	ff7_externals.field_bg_offset = (vector2<int>*)get_absolute_value((uint32_t)ff7_externals.field_update_background_positions, 0x3E8);
	ff7_externals.field_curr_delta_world_pos_x = (short*)get_absolute_value((uint32_t)ff7_externals.field_update_background_positions, 0x98F);
	ff7_externals.field_curr_delta_world_pos_y = (short*)get_absolute_value((uint32_t)ff7_externals.field_update_background_positions, 0x9AC);
	ff7_externals.scripted_world_initial_pos_x = (short*)get_absolute_value(ff7_externals.field_update_scripted_bg_movement, 0xB8);
	ff7_externals.scripted_world_initial_pos_y = (short*)get_absolute_value(ff7_externals.field_update_scripted_bg_movement, 0xE5);
	ff7_externals.scripted_world_final_pos_x = (short*)get_absolute_value(ff7_externals.field_update_scripted_bg_movement, 0x215);
	ff7_externals.scripted_world_final_pos_y = (short*)get_absolute_value(ff7_externals.field_update_scripted_bg_movement, 0x243);
	ff7_externals.scripted_world_move_n_steps = (short*)get_absolute_value(ff7_externals.field_update_scripted_bg_movement, 0x8A);
	ff7_externals.scripted_world_move_step_index = (short*)get_absolute_value(ff7_externals.field_update_scripted_bg_movement, 0x102);
	ff7_externals.field_world_pos_x = (short*)get_absolute_value((uint32_t)ff7_externals.field_update_background_positions, 0x403);
	ff7_externals.field_world_pos_y = (short*)get_absolute_value((uint32_t)ff7_externals.field_update_background_positions, 0x424);
	ff7_externals.field_cursor_pos_x = (short*)get_absolute_value((uint32_t)ff7_externals.field_update_background_positions, 0x5C3);
	ff7_externals.field_cursor_pos_y = (short*)get_absolute_value((uint32_t)ff7_externals.field_update_background_positions, 0x5F6);
	ff7_externals.field_viewport_xy_CFF204 = (vector2<int>*)get_absolute_value((uint32_t)ff7_externals.field_sub_64314F, 0x28);
	ff7_externals.field_max_half_viewport_width_height_CFF1F4 = (vector2<int>*)get_absolute_value((uint32_t)ff7_externals.field_sub_64314F, 0x58);
	ff7_externals.field_curr_half_viewport_width_height_CFF1FC = (vector2<int>*)get_absolute_value((uint32_t)ff7_externals.field_update_background_positions, 0x5AD);
	ff7_externals.field_bg_flag_CC15E4 = (WORD*)get_absolute_value((uint32_t)ff7_externals.field_update_background_positions, 0x129);
	ff7_externals.field_sub_640EB7 = get_relative_call(ff7_externals.field_draw_everything, 0x34);
	ff7_externals.field_sub_661B68 = get_relative_call(ff7_externals.field_sub_640EB7, 0x61);
	ff7_externals.field_prev_world_pos_x = (short*)get_absolute_value((uint32_t)ff7_externals.field_sub_640EB7, 0x6);
	ff7_externals.field_prev_world_pos_y = (short*)get_absolute_value((uint32_t)ff7_externals.field_sub_640EB7, 0x18);
	ff7_externals.engine_set_game_engine_world_coord_661B23 = (void (*)(int, int))get_relative_call(ff7_externals.field_sub_661B68, 0x1A);
	ff7_externals.engine_sub_67CCDE = (void (*)(float, float, float, float, float, float, float, ff7_game_obj*))get_relative_call(ff7_externals.field_sub_661B68, 0x72);
	ff7_externals.field_handle_screen_fading = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0x634);

	ff7_externals.sfx_stop_channel_6 = get_relative_call(common_externals.sfx_cleanup, 0x16);
	ff7_externals.sfx_stop_channel_timer_handle = (UINT *)get_absolute_value(ff7_externals.sfx_stop_channel_6, 0x5);

	ff7_externals.current_movie_frame = (WORD*)get_absolute_value(ff7_externals.field_loop_sub_63C17F, 0x133);
	ff7_externals.opening_movie_music_start_frame = (DWORD *)(ff7_externals.field_loop_sub_63C17F + 0x139);
	ff7_externals.opening_movie_play_midi_call = ff7_externals.field_loop_sub_63C17F + 0x145;

	ff7_externals.byte_CC164C = (BYTE *)get_absolute_value(main_loop, 0x32A);
	ff7_externals.word_CC0DC6 = (WORD *)get_absolute_value(main_init_loop, 0x4BD);

	ff7_externals.sub_5F5042 = get_relative_call(condor_main_loop, 0x69);
	ff7_externals.highway_loop_sub_650F36 = get_relative_call(highway_main_loop, 0x53);
	ff7_externals.snowboard_enter_sub_722C10 = get_absolute_value(main_loop, 0xB53);
	ff7_externals.snowboard_loop_sub_72381C = get_relative_call(snowboard_main_loop, 0x7D);
	ff7_externals.snowboard_exit_sub_722C52 = get_absolute_value(main_loop, 0xB5A);
	ff7_externals.sub_779E14 = get_relative_call(chocobo_main_loop, 0x70);

	ff7_externals.condor_enter = get_absolute_value(main_loop, 0xA28);
	ff7_externals.condor_exit = get_absolute_value(main_loop, 0xA2F);
	ff7_externals.sub_5F7756 = get_relative_call(ff7_externals.condor_enter, 0x1B0);
	ff7_externals.sub_5F4273 = get_relative_call(ff7_externals.sub_5F7756, 0xA2);
	ff7_externals.sub_5F342C = get_relative_call(ff7_externals.sub_5F4273, 0xBF);
	ff7_externals.condor_uses_lgp = (DWORD*)get_absolute_value(ff7_externals.sub_5F342C, 0x7A);

	ff7_externals.fps_limiter_swirl = get_relative_call(swirl_main_loop, 0xDE);
	ff7_externals.fps_limiter_battle = get_relative_call(battle_main_loop, 0x1DD);
	ff7_externals.fps_limiter_coaster = get_relative_call(coaster_main_loop, 0x51);
	ff7_externals.fps_limiter_condor = get_relative_call(ff7_externals.sub_5F5042, 0x5F);
	ff7_externals.fps_limiter_field = get_relative_call(ff7_externals.field_sub_6388EE, 0x58);
	ff7_externals.fps_limiter_highway = get_relative_call(ff7_externals.highway_loop_sub_650F36, 0xC3);
	ff7_externals.fps_limiter_snowboard = get_relative_call(ff7_externals.snowboard_loop_sub_72381C, 0x14);
	ff7_externals.fps_limiter_worldmap = get_relative_call(worldmap_main_loop, 0x1D);
	ff7_externals.fps_limiter_chocobo = get_relative_call(ff7_externals.sub_779E14, 0x4D);
	ff7_externals.fps_limiter_submarine = get_relative_call(submarine_main_loop, 0x98);
	ff7_externals.fps_limiter_credits = get_relative_call(credits_main_loop, 0x1C);

	ff7_externals.battle_fps_menu_multiplier = battle_main_loop + 0x335;
	ff7_externals.submarine_minigame_status = (DWORD *)get_absolute_value(ff7_externals.fps_limiter_submarine, 0x48);
	ff7_externals.submarine_last_gametime = (time_t *)get_absolute_value(ff7_externals.fps_limiter_submarine, 0x26);
	ff7_externals.field_limit_fps = (DWORD *)get_absolute_value(ff7_externals.fps_limiter_field, 0x1F);
	ff7_externals.swirl_limit_fps = (DWORD *)get_absolute_value(ff7_externals.fps_limiter_swirl, 0x48);

	ff7_externals.get_bank_value = (int16_t (*)(int16_t, int16_t))get_relative_call(common_externals.execute_opcode_table[0xF1], 0x30);
	ff7_externals.set_bank_value = (int8_t (*)(int16_t, int16_t, int16_t))get_relative_call(common_externals.execute_opcode_table[0xFA], 0x1A);
	ff7_externals.get_char_bank_value = (int8_t (*)(int16_t, int16_t))get_relative_call(common_externals.execute_opcode_table[0x5E], 0x41);
	ff7_externals.sub_611BAE = get_relative_call(common_externals.execute_opcode_table[0x16], 0x4);

	ff7_externals.wait_frames_ptr = (WORD*)get_absolute_value(common_externals.execute_opcode_table[0x24], 0x1C);

	ff7_externals.world_mode_loop_sub_74DB8C = get_relative_call(worldmap_main_loop, 0x114);
	ff7_externals.sub_767039 = (void (*)(DWORD*,DWORD*,DWORD*))get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x613);
	ff7_externals.wm_change_music = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x2CF);
	ff7_externals.wm_play_music_call = ff7_externals.wm_change_music + 0x5A;

	ff7_externals.sub_630C48 = (int (*)(int16_t, int16_t, int16_t, int16_t, int16_t))get_relative_call(common_externals.execute_opcode_table[0x50], 0x174);

	ff7_externals.current_entity_id = (byte*)get_absolute_value(common_externals.execute_opcode_table[0x5F], 0x06); // 0xCC0964
	ff7_externals.field_script_ptr = (byte**)get_absolute_value(ff7_externals.open_field_file, 0xEA); //0xCBF5E8
	ff7_externals.field_curr_script_position = (WORD*)get_absolute_value(common_externals.execute_opcode_table[0x5F], 0xE); //0xCC0CF8
	common_externals.field_game_moment = (WORD*)get_absolute_value(common_externals.execute_opcode_table[0x9D], 0xEA); //0xDC08DC

	ff7_externals.sub_408074 = get_relative_call(main_loop, 0x681);
	ff7_externals.sub_60BB58 = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0x16F);
	common_externals.update_field_entities = get_relative_call(ff7_externals.sub_60BB58, 0x3A); // 0x60C94D

	common_externals.current_field_id = (WORD*)get_absolute_value(ff7_externals.sub_408074, 0x41); // 0xCC15D0
	common_externals.previous_field_id = (WORD*)get_absolute_value(ff7_externals.sub_408074, 0x4F); // 0xCC0DEC
	common_externals.update_entities_call = common_externals.update_field_entities + 0x461; // 0x60CDAE

	ff7_externals.field_level_data_pointer = (byte**)ff7_externals.field_file_buffer; // 0xCFF594

	ff7_externals.sub_408116 = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0x2A);
	ff7_externals.word_CC16E8 = (char *)get_absolute_value(ff7_externals.sub_408116, 0x8E);
	common_externals.current_triangle_id = (int16_t *)((char *)ff7_externals.word_CC16E8 + 136 * ff7_externals.modules_global_object->field_model_id);

	ff7_externals.sub_6499F7 = get_relative_call(ff7_externals.field_loop_sub_63C17F, 0x10C);
	ff7_externals.input_ok_button_status = (DWORD*)get_absolute_value(ff7_externals.sub_6499F7, 0x60);
	ff7_externals.input_run_button_status = (DWORD*)get_absolute_value(ff7_externals.sub_6499F7, 0x55);

	ff7_externals.field_load_models_atoi = ff7_externals.field_load_models + 0x262;

	// auto attack gamehacks
	ff7_externals.handle_actor_ready = ff7_externals.battle_menu_state_fn_table[0];
	ff7_externals.battle_menu_state = (WORD*)get_absolute_value(ff7_externals.handle_actor_ready, 0x17B);
	ff7_externals.set_battle_menu_state_data = get_relative_call(ff7_externals.handle_actor_ready, 0x187);
	ff7_externals.dispatch_chosen_battle_action = get_relative_call(ff7_externals.battle_sub_6DB0EE, 0x50E);
	ff7_externals.set_battle_targeting_data = get_relative_call(ff7_externals.battle_menu_state_fn_table[19], 0x11A);
	ff7_externals.issued_command_id = (byte*)get_absolute_value(ff7_externals.dispatch_chosen_battle_action, 0x12B);
	ff7_externals.issued_action_id = (uint16_t*)get_absolute_value(ff7_externals.dispatch_chosen_battle_action, 0x122);
	ff7_externals.issued_action_target_type = (byte*)get_absolute_value(ff7_externals.set_battle_targeting_data, 0x14E);
	ff7_externals.issued_action_target_index = (byte*)get_absolute_value(ff7_externals.set_battle_targeting_data, 0x164);
	// --------------------------------

	// ---------- 60 FPS feature ------------
	// Camera
	ff7_externals.handle_camera_functions = get_relative_call(ff7_externals.battle_sub_42D992, 0xE3);
	ff7_externals.set_camera_focal_position_scripts = get_relative_call(ff7_externals.handle_camera_functions, 0x35);
	ff7_externals.set_camera_position_scripts = get_relative_call(ff7_externals.handle_camera_functions, 0x4B);
	ff7_externals.execute_camera_functions = get_relative_call(ff7_externals.handle_camera_functions, 0x55);
	ff7_externals.add_fn_to_camera_fn_array = get_relative_call(ff7_externals.set_camera_focal_position_scripts, 0xF40);
	ff7_externals.battle_camera_sub_5C52F8 = get_relative_call(ff7_externals.set_camera_focal_position_scripts, 0x10A8);
	ff7_externals.battle_camera_sub_5C3E6F = get_relative_call(ff7_externals.set_camera_position_scripts, 0x169E);
	ff7_externals.camera_fn_array = std::span((uint32_t*)get_absolute_value(ff7_externals.add_fn_to_camera_fn_array, 0x39), 16);
	ff7_externals.camera_fn_data = std::span((bcamera_fn_data*)get_absolute_value(ff7_externals.add_fn_to_camera_fn_array, 0x4D), 16);
	ff7_externals.battle_camera_position = std::span((bcamera_position*)get_absolute_value(ff7_externals.set_camera_position_scripts, 0x331), 4);
	ff7_externals.battle_camera_focal_point = std::span((bcamera_position*)get_absolute_value(ff7_externals.set_camera_focal_position_scripts, 0x233), 4);
	ff7_externals.battle_camera_focal_scripts_8FEE30 = (byte*)get_absolute_value(ff7_externals.set_camera_focal_position_scripts, 0xC1);
	ff7_externals.battle_camera_position_scripts_8FEE2C = (byte*)get_absolute_value(ff7_externals.set_camera_position_scripts, 0xC1);
	ff7_externals.battle_camera_global_scripts_9A13BC = (DWORD*)get_absolute_value(ff7_externals.set_camera_position_scripts, 0x17);
	ff7_externals.battle_camera_position_scripts_9010D0 = (DWORD*)get_absolute_value(ff7_externals.set_camera_position_scripts, 0xDC);
	ff7_externals.battle_camera_focal_scripts_901270 = (DWORD*)get_absolute_value(ff7_externals.set_camera_focal_position_scripts, 0xDC);
	ff7_externals.battle_camera_script_index = (byte*)get_absolute_value(ff7_externals.set_camera_position_scripts, 0xD2);
	ff7_externals.battle_camera_script_offset = (DWORD*)get_absolute_value(ff7_externals.set_camera_position_scripts, 0x25);
	ff7_externals.camera_fn_index = (WORD*)get_absolute_value(ff7_externals.add_fn_to_camera_fn_array, 0x46);
	ff7_externals.camera_fn_counter = (WORD*)get_absolute_value(ff7_externals.add_fn_to_camera_fn_array, 0x54);

	ff7_externals.battle_camera_position_sub_5C3D0D = get_absolute_value(ff7_externals.set_camera_position_scripts, 0x5DE);
	ff7_externals.battle_camera_position_sub_5C5B9C = get_absolute_value(ff7_externals.set_camera_position_scripts, 0x40A);
	ff7_externals.battle_camera_position_sub_5C557D = get_absolute_value(ff7_externals.set_camera_position_scripts, 0xE28);
	ff7_externals.battle_camera_focal_sub_5C5F5E = get_absolute_value(ff7_externals.set_camera_focal_position_scripts, 0xBDB);
	ff7_externals.battle_camera_focal_sub_5C5714 = get_absolute_value(ff7_externals.set_camera_focal_position_scripts, 0x67D);

	ff7_externals.battle_sub_430DD0 = get_relative_call(ff7_externals.battle_loop, 0x99E);
	ff7_externals.battle_sub_429D8A = get_absolute_value(ff7_externals.battle_loop, 0x59);
	ff7_externals.update_battle_camera_sub_5C20CE = get_relative_call(ff7_externals.battle_sub_42D992, 0xFB);
	ff7_externals.set_battle_camera_sub_5C22BD = get_relative_call(ff7_externals.update_battle_camera_sub_5C20CE, 0x5A);
	ff7_externals.battle_camera_sub_5C22A9 = get_relative_call(ff7_externals.update_battle_camera_sub_5C20CE, 0x97);
	ff7_externals.compute_interpolation_to_formation_camera = get_relative_call(ff7_externals.battle_camera_sub_5C22A9, 0x3);
	ff7_externals.set_battle_camera_sub_5C2350 = get_relative_call(ff7_externals.battle_camera_sub_5C22A9, 0xA);
	ff7_externals.g_battle_camera_position = (vector3<short>*)get_absolute_value(ff7_externals.set_battle_camera_sub_5C22BD, 0x17);
	ff7_externals.g_battle_camera_focal_point = (vector3<short>*)get_absolute_value(ff7_externals.set_battle_camera_sub_5C22BD, 0x5E);
	ff7_externals.formation_camera = std::span((formation_camera*)get_absolute_value(ff7_externals.set_battle_camera_sub_5C22BD, 0x10), 4);
	ff7_externals.curr_formation_camera_idx = (byte*)get_absolute_value(ff7_externals.set_battle_camera_sub_5C22BD, 0x6);
	ff7_externals.battle_enter_frames_to_wait = (byte*)get_absolute_value(ff7_externals.battle_sub_429AC0, 0x14E);
	ff7_externals.g_variation_index = (byte*)get_absolute_value(ff7_externals.update_battle_camera_sub_5C20CE, 0x6F);
	ff7_externals.is_camera_moving_BFB2DC = (byte*)get_absolute_value(ff7_externals.update_battle_camera_sub_5C20CE, 0x1C1);

	// Animation effects
	uint32_t battle_sub_42A5D0 = get_relative_call(ff7_externals.battle_sub_429AC0, 0x1A6);
	ff7_externals.battle_sub_42A5EB = get_relative_call(battle_sub_42A5D0, 0x14);
	ff7_externals.battle_sub_42E275 = get_relative_call(ff7_externals.battle_sub_42D992, 0x6C);
	uint32_t battle_sub_42E3CA = get_relative_call(ff7_externals.battle_sub_42D992, 0x48);
	ff7_externals.battle_sub_42E34A = get_relative_call(battle_sub_42E3CA, 0x70);
	uint32_t battle_sub_42DBD2 = get_relative_call(ff7_externals.battle_sub_42D992, 0x90);
	uint32_t battle_sub_42F21F = get_relative_call(battle_sub_42DBD2, 0x37);
	ff7_externals.battle_sub_5B9EC2 = get_relative_call(battle_sub_42F21F, 0x38);
	ff7_externals.battle_sub_5BD5E9 = get_relative_call(ff7_externals.battle_sub_5B9EC2, 0x41);
	uint32_t battle_sub_42DE61 = get_relative_call(ff7_externals.battle_sub_42D992, 0x9F);
	ff7_externals.run_summon_animations_script_5C1B81 = get_absolute_value(battle_sub_42DE61, 0x17E);
	ff7_externals.run_summon_animations_script_sub_5C1D9A = get_relative_call(ff7_externals.run_summon_animations_script_5C1B81, 0xA4);
	ff7_externals.run_animation_script = get_relative_call(ff7_externals.battle_sub_42A5EB, 0xB8);
	ff7_externals.add_fn_to_effect100_fn = get_relative_call(ff7_externals.run_animation_script, 0x48C2);
	ff7_externals.execute_effect100_fn = get_relative_call(ff7_externals.battle_sub_42D992, 0x12E);
	ff7_externals.add_fn_to_effect60_fn = get_relative_call(ff7_externals.run_animation_script, 0x394);
	ff7_externals.execute_effect60_fn = get_relative_call(ff7_externals.battle_sub_42D992, 0x129);
	ff7_externals.add_fn_to_effect10_fn = get_relative_call(ff7_externals.run_animation_script, 0x825);
	ff7_externals.execute_effect10_fn = get_relative_call(ff7_externals.battle_sub_42D992, 0x4D);
	uint32_t battle_sub_42B66A = get_relative_call(ff7_externals.run_animation_script, 0x460A);
	ff7_externals.battle_update_3d_model_data = get_relative_call(ff7_externals.run_animation_script, 0x623);

	ff7_externals.effect100_array_data = std::span((effect100_data*)get_absolute_value(ff7_externals.add_fn_to_effect100_fn, 0x5D), 100);
	ff7_externals.effect100_array_fn = std::span((uint32_t*)get_absolute_value(ff7_externals.add_fn_to_effect100_fn, 0x48), 100);
	ff7_externals.effect100_counter = (uint16_t*)get_absolute_value(ff7_externals.add_fn_to_effect100_fn, 0x63);
	ff7_externals.effect100_array_idx = (uint16_t*)get_absolute_value(ff7_externals.add_fn_to_effect100_fn, 0x32);
	ff7_externals.effect60_array_data = std::span((effect60_data*)get_absolute_value(ff7_externals.add_fn_to_effect60_fn, 0x5D), 60);
	ff7_externals.effect60_array_fn = std::span((uint32_t*)get_absolute_value(ff7_externals.add_fn_to_effect60_fn, 0x48), 60);
	ff7_externals.effect60_array_idx = (uint16_t*)get_absolute_value(ff7_externals.add_fn_to_effect60_fn, 0x32);
	ff7_externals.effect60_counter = (uint16_t*)get_absolute_value(ff7_externals.add_fn_to_effect60_fn, 0x63);
	ff7_externals.effect10_array_data = std::span((effect10_data*)get_absolute_value(ff7_externals.add_fn_to_effect10_fn, 0x5D), 10);
	ff7_externals.effect10_array_fn = std::span((uint32_t*)get_absolute_value(ff7_externals.add_fn_to_effect10_fn, 0x48), 10);
	ff7_externals.effect10_array_idx = (uint16_t*)get_absolute_value(ff7_externals.add_fn_to_effect10_fn, 0x32);
	ff7_externals.effect10_counter = (uint16_t*)get_absolute_value(ff7_externals.add_fn_to_effect10_fn, 0x63);
	ff7_externals.g_actor_idle_scripts = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x304);
	ff7_externals.g_battle_model_state = std::span((battle_model_state*)get_absolute_value(battle_sub_42B66A, 0xD9), 10);
	ff7_externals.g_small_battle_model_state = std::span((battle_model_state_small*)get_absolute_value(ff7_externals.run_animation_script, 0x2BB9), 10);
	std::function<int(int)> shift_index = [](int index){return index - 0x2E;};
	ff7_externals.animation_script_pointers[shift_index(0x2E)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x81);
	ff7_externals.animation_script_pointers[shift_index(0x2F)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x90);
	ff7_externals.animation_script_pointers[shift_index(0x30)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x9F);
	ff7_externals.animation_script_pointers[shift_index(0x31)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0xAE);
	ff7_externals.animation_script_pointers[shift_index(0x32)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0xBD);
	ff7_externals.animation_script_pointers[shift_index(0x34)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0xCC);
	ff7_externals.animation_script_pointers[shift_index(0x35)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0xD8);
	ff7_externals.animation_script_pointers[shift_index(0x36)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0xE4);
	ff7_externals.animation_script_pointers[shift_index(0x37)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0xF0);
	ff7_externals.animation_script_pointers[shift_index(0x38)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0xFC);
	ff7_externals.animation_script_pointers[shift_index(0x39)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x12D);
	ff7_externals.animation_script_pointers[shift_index(0x3A)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x139);
	ff7_externals.animation_script_pointers[shift_index(0x3B)] = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x145);
	ff7_externals.g_is_effect_loading = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x2D25);
	ff7_externals.g_is_battle_paused = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0xA);
	ff7_externals.special_actor_id = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x286B);
	ff7_externals.g_script_wait_frames = (byte*)get_absolute_value(ff7_externals.run_animation_script, 0x27E8);
	ff7_externals.g_script_args = std::span((int**)get_absolute_value(ff7_externals.run_animation_script, 0x3EA), 8);
	ff7_externals.limit_break_effects_fn_table = std::span((uint32_t*)get_absolute_value(ff7_externals.battle_sub_427C22, 0x40E), 80);
	ff7_externals.enemy_atk_effects_fn_table = std::span((uint32_t*)get_absolute_value(ff7_externals.battle_sub_427C22, 0x4B9), 157);
	ff7_externals.enemy_skill_effects_fn_table = std::span((uint32_t*)get_absolute_value(ff7_externals.battle_sub_427C22, 0x20A), 24);

	// Enemy death animations
	uint32_t battle_run_enemy_deaths_42567E = get_relative_call(ff7_externals.run_animation_script, 0x4869);
	ff7_externals.battle_enemy_death_5BBD24 = get_absolute_value(battle_run_enemy_deaths_42567E, 0x37);
	ff7_externals.battle_enemy_death_sub_5BBE32 = get_relative_call(ff7_externals.battle_enemy_death_5BBD24, 0xF1);
	ff7_externals.battle_iainuki_death_5BCAAA = get_absolute_value(battle_run_enemy_deaths_42567E, 0x72);
	ff7_externals.battle_iainuki_death_sub_5BCBB8 = get_relative_call(ff7_externals.battle_iainuki_death_5BCAAA, 0xF1);
	ff7_externals.battle_boss_death_5BC48C = get_absolute_value(battle_run_enemy_deaths_42567E, 0x16A);
	ff7_externals.battle_boss_death_sub_5BC6ED = get_relative_call(ff7_externals.battle_boss_death_5BC48C, 0x144);
	ff7_externals.battle_boss_death_sub_5BC5EC = get_absolute_value(ff7_externals.battle_boss_death_5BC48C, 0x9F);
	ff7_externals.battle_boss_death_call_5BD436 = get_relative_call(ff7_externals.battle_boss_death_sub_5BC5EC, 0x7C);
	ff7_externals.field_battle_BFB2E0 = (int*)get_absolute_value(ff7_externals.battle_boss_death_sub_5BC5EC, 0x85);
	ff7_externals.battle_melting_death_5BC21F = get_absolute_value(battle_run_enemy_deaths_42567E, 0xF7);
	ff7_externals.battle_melting_death_sub_5BC32D = get_relative_call(ff7_externals.battle_melting_death_5BC21F, 0xF1);
	ff7_externals.battle_disintegrate_2_death_5BBA82 = get_absolute_value(battle_run_enemy_deaths_42567E, 0x132);
	ff7_externals.battle_disintegrate_2_death_sub_5BBBDE = get_relative_call(ff7_externals.battle_disintegrate_2_death_5BBA82, 0xFE);
	ff7_externals.field_float_battle_7B7680 = (float*)get_absolute_value(ff7_externals.battle_disintegrate_2_death_sub_5BBBDE, 0x10F);
	ff7_externals.battle_morph_death_5BC812 = get_absolute_value(battle_run_enemy_deaths_42567E, 0x1A2);
	ff7_externals.battle_morph_death_sub_5BC920 = get_relative_call(ff7_externals.battle_morph_death_5BC812, 0xF1);
	ff7_externals.battle_disintegrate_1_death_5BBF31 = get_absolute_value(battle_run_enemy_deaths_42567E, 0xAD);
	ff7_externals.battle_disintegrate_1_death_sub_5BC04D = get_relative_call(ff7_externals.battle_disintegrate_1_death_5BBF31, 0xFF);
	ff7_externals.battle_sub_42C0A7 = get_relative_call(ff7_externals.battle_disintegrate_1_death_sub_5BC04D, 0x8D);
	ff7_externals.effect10_array_data_8FE1F6 = (short*)get_absolute_value(ff7_externals.battle_disintegrate_1_death_sub_5BC04D, 0x123);

	// Display string for actor actions
	ff7_externals.display_battle_action_text_42782A = get_absolute_value(ff7_externals.run_animation_script, 0x4906);
	ff7_externals.get_n_frames_display_action_string = get_relative_call(ff7_externals.run_animation_script, 0x4918);
	ff7_externals.field_byte_DC0E11 = (byte*)get_absolute_value(ff7_externals.get_n_frames_display_action_string, 0x6);
	uint32_t battle_sub_4351BD = get_relative_call(ff7_externals.battle_loop, 0x47C);
	uint32_t** battle_functions_table_7C2AC0 = (uint32_t**)get_absolute_value(battle_sub_4351BD, 0x3A);
	ff7_externals.battle_sub_434C8B = battle_functions_table_7C2AC0[1][1];
	uint32_t battle_sub_435789 = get_relative_call(ff7_externals.battle_loop, 0x3B8);
	ff7_externals.battle_sub_435D81 = get_relative_call(battle_sub_435789, 0x505);

	// Display battle damage
	ff7_externals.battle_sub_425D29 = get_absolute_value(ff7_externals.run_animation_script, 0x2850);
	ff7_externals.battle_sub_425E5F = get_absolute_value(ff7_externals.battle_sub_425D29, 0xA8);
	ff7_externals.display_battle_damage_5BB410 = get_absolute_value(ff7_externals.battle_sub_425D29, 0x3D);

	// Reduce actor size (for Mini status effect)
	ff7_externals.battle_sub_42CBF9 = get_relative_call(ff7_externals.battle_loop, 0x425);
	ff7_externals.battle_sub_5BDA0F = get_absolute_value(ff7_externals.battle_sub_42CBF9, 0x240);

	// Character fade in/out
	ff7_externals.vincent_limit_fade_effect_sub_5D4240 = ff7_externals.limit_break_effects_fn_table[56];
	ff7_externals.battle_sub_5BD96D = get_absolute_value(ff7_externals.vincent_limit_fade_effect_sub_5D4240, 0x27);
	ff7_externals.battle_sub_5C1C8F = get_absolute_value(ff7_externals.run_summon_animations_script_5C1B81, 0x3F);
	uint32_t handle_fade_character_42C31C = get_relative_call(ff7_externals.battle_sub_5C1C8F, 0xEC);
	ff7_externals.battle_sub_5C18BC = get_absolute_value(ff7_externals.run_summon_animations_script_5C1B81, 0x30);
	ff7_externals.battle_sub_42A72D = get_relative_call(ff7_externals.battle_sub_429AC0, 0xD0);

	// resting positions and rotations
	uint32_t battle_sub_426C9B = get_relative_call(ff7_externals.run_animation_script, 0x14C7);
	ff7_externals.battle_sub_426DE3 = get_absolute_value(battle_sub_426C9B, 0x5);
	ff7_externals.battle_sub_426941 = get_absolute_value(ff7_externals.run_animation_script, 0x1A5D);
	ff7_externals.battle_sub_426899 = get_absolute_value(ff7_externals.run_animation_script, 0x821);
	ff7_externals.battle_sub_4267F1 = get_absolute_value(ff7_externals.run_animation_script, 0xFF6);
	ff7_externals.battle_move_character_sub_426A26 = get_absolute_value(ff7_externals.run_animation_script, 0x1568);
	ff7_externals.field_battle_byte_BF2E1C = (byte*)get_absolute_value(ff7_externals.battle_move_character_sub_426A26, 0x86);
	ff7_externals.field_battle_byte_BE10B4 = (byte*)get_absolute_value(ff7_externals.battle_move_character_sub_426A26, 0x148);
	ff7_externals.battle_move_character_sub_42739D = get_absolute_value(ff7_externals.run_animation_script, 0x248E);
	ff7_externals.battle_move_character_sub_426F58 = get_absolute_value(ff7_externals.run_animation_script, 0x26AF);
	ff7_externals.resting_Y_array_data = (short*)get_absolute_value(ff7_externals.battle_move_character_sub_426F58, 0x122);
	ff7_externals.battle_move_character_sub_4270DE = get_absolute_value(ff7_externals.run_animation_script, 0x2357);

	// aura animations (magic, limit breaks, enemy skill and summon)
	ff7_externals.handle_aura_effects_425520 = get_absolute_value(ff7_externals.run_animation_script, 0x3F7A);
	ff7_externals.run_aura_effects_5C0230 = get_relative_call(ff7_externals.handle_aura_effects_425520, 0x42);
	ff7_externals.limit_break_aura_effects_5C0572 = get_absolute_value(ff7_externals.run_aura_effects_5C0230, 0x72);
	ff7_externals.enemy_skill_aura_effects_5C06BF = get_absolute_value(ff7_externals.run_aura_effects_5C0230, 0x7E);
	ff7_externals.handle_summon_aura_5C0850 = get_absolute_value(ff7_externals.run_aura_effects_5C0230, 0x8F);
	ff7_externals.summon_aura_effects_5C0953 = get_absolute_value(ff7_externals.handle_summon_aura_5C0850, 0x31);

	// effect 60 related
	ff7_externals.battle_sub_4276B6 = get_absolute_value(ff7_externals.run_animation_script, 0x3091);
	ff7_externals.battle_sub_4255B7 = get_absolute_value(ff7_externals.run_animation_script, 0x390);
	ff7_externals.battle_sub_5BCF9D = get_absolute_value(ff7_externals.battle_sub_429AC0, 0xDB);
	ff7_externals.battle_sub_5BD050 = get_relative_call(ff7_externals.battle_sub_5BCF9D, 0x95);
	ff7_externals.battle_sub_425AAD = get_absolute_value(ff7_externals.run_animation_script, 0x413);
	ff7_externals.battle_sub_427A6C = ff7_externals.enemy_atk_effects_fn_table[147];
	ff7_externals.battle_sub_427AF1 = get_absolute_value(ff7_externals.battle_sub_427A6C, 0x56);
	ff7_externals.battle_sub_427737 = get_absolute_value(ff7_externals.run_animation_script, 0x3158);
	ff7_externals.battle_sub_4277B1 = get_absolute_value(ff7_externals.run_animation_script, 0x472B);
	ff7_externals.battle_sub_5BCD42 = get_absolute_value(ff7_externals.run_animation_script, 0x66F);
	uint32_t battle_sub_5BE490 = get_relative_call(ff7_externals.run_animation_script, 0x3E6E);
	ff7_externals.battle_smoke_move_handler_5BE4E2 = get_absolute_value(battle_sub_5BE490, 0x5);
	ff7_externals.battle_sub_6CE81E = (void(*)())get_relative_call(ff7_externals.battle_sub_42D808, 0x117);
	ff7_externals.battle_play_sfx_sound_430D32 = (void(*)(uint16_t, short, char))get_relative_call(ff7_externals.battle_sub_427737, 0x35);

	// Limit breaks
	uint32_t battle_sub_4E1627 = get_relative_call(ff7_externals.run_animation_script, 0x3848);
	ff7_externals.run_tifa_limit_effects = get_relative_call(battle_sub_4E1627, 0xD);
	uint32_t tifa_limit_1_2_main_4E2DF3 = get_absolute_value(ff7_externals.run_tifa_limit_effects, 0x47);
	ff7_externals.tifa_limit_1_2_sub_4E3D51 = get_absolute_value(tifa_limit_1_2_main_4E2DF3, 0x4BB);
	uint32_t tifa_limit_2_1_main_4E401E = get_absolute_value(ff7_externals.run_tifa_limit_effects, 0x67);
	ff7_externals.tifa_limit_2_1_sub_4E48D4 = get_absolute_value(tifa_limit_2_1_main_4E401E, 0x41A);
	uint32_t aerith_limit_2_1_main_45AE80 = ff7_externals.limit_break_effects_fn_table[16];
	uint32_t aerith_limit_2_1_sub_45AEA6 = get_relative_call(aerith_limit_2_1_main_45AE80, 0x1A);
	uint32_t aerith_limit_2_1_sub_45AEE8 = get_absolute_value(aerith_limit_2_1_sub_45AEA6, 0xE);
	uint32_t aerith_limit_2_1_sub_45AF39 = get_absolute_value(aerith_limit_2_1_sub_45AEE8, 0x5);
	ff7_externals.aerith_limit_2_1_sub_45B0CF = get_absolute_value(aerith_limit_2_1_sub_45AF39, 0x4A);
	uint32_t cloud_limit_2_2_main_466A31 = ff7_externals.limit_break_effects_fn_table[3];
	uint32_t cloud_limit_2_2_sub_466A57 = get_relative_call(cloud_limit_2_2_main_466A31, 0x1A);
	uint32_t cloud_limit_2_2_sub_466A7A = get_absolute_value(cloud_limit_2_2_sub_466A57, 0x15);
	uint32_t cloud_limit_2_2_sub_466CD2 = get_absolute_value(cloud_limit_2_2_sub_466A7A, 0x185);
	ff7_externals.cloud_limit_2_2_sub_467256 = get_absolute_value(cloud_limit_2_2_sub_466CD2, 0x38C);
	uint32_t aerith_limit_4_1_sub_473A70 = ff7_externals.limit_break_effects_fn_table[20];
	uint32_t aerith_limit_4_1_sub_473B84 = get_relative_call(aerith_limit_4_1_sub_473A70, 0xAA);
	uint32_t aerith_limit_4_1_sub_473C82 = get_relative_call(aerith_limit_4_1_sub_473B84, 0xB7);
	ff7_externals.aerith_limit_4_1_camera_473CC2 = get_absolute_value(aerith_limit_4_1_sub_473C82, 0x5);
	uint32_t vincent_limit_satan_slam_sub_45C0C0 = ff7_externals.limit_break_effects_fn_table[69];
	uint32_t vincent_limit_satan_slam_sub_45C1EA = get_relative_call(vincent_limit_satan_slam_sub_45C0C0, 0x87);
	uint32_t vincen_limit_satan_slam_sub_45C263 = get_absolute_value(vincent_limit_satan_slam_sub_45C1EA, 0x47);
	uint32_t vincent_limit_satan_slam_sub_45CEEA = get_relative_call(vincen_limit_satan_slam_sub_45C263, 0x228);
	ff7_externals.vincent_limit_satan_slam_camera_45CF2A = get_absolute_value(vincent_limit_satan_slam_sub_45CEEA, 0x5);
	uint32_t barret_limit_4_1_sub_468691 = ff7_externals.limit_break_effects_fn_table[13];
	uint32_t barret_limit_4_1_sub_468725 = get_relative_call(barret_limit_4_1_sub_468691, 0x88);
	uint32_t barret_limit_4_1_sub_468862 = get_relative_call(barret_limit_4_1_sub_468725, 0x11B);
	ff7_externals.barret_limit_4_1_camera_4688A2 = get_absolute_value(barret_limit_4_1_sub_468862, 0x5);
	ff7_externals.barret_limit_4_1_model_movement_4698EF = get_absolute_value(barret_limit_4_1_sub_468725, 0x101);
	ff7_externals.barret_limit_4_1_actor_id = (int*)get_absolute_value(ff7_externals.barret_limit_4_1_model_movement_4698EF, 0x82);

	// Summons
	uint32_t battle_sub_5C0E39 = get_relative_call(ff7_externals.battle_sub_427C22, 0x4DE);
	ff7_externals.run_summon_animations_5C0E4B = get_absolute_value(battle_sub_5C0E39, 0x4);
	uint32_t run_chocomog_main_5099D6 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x223);
	uint32_t run_chocomog_sub_507BAD = get_relative_call(run_chocomog_main_5099D6, 0x2E);
	uint32_t run_chocomog_camera_handler_509AD0 = get_relative_call(run_chocomog_sub_507BAD, 0x9C);
	uint32_t run_chocomog_main_loop_50A9E0 = get_absolute_value(run_chocomog_sub_507BAD, 0x74);
	ff7_externals.run_chocomog_movement_50B1A3 = get_absolute_value(run_chocomog_main_loop_50A9E0, 0xD1);
	ff7_externals.run_chocomog_camera_509B10 = get_absolute_value(run_chocomog_camera_handler_509AD0, 0x5);
	uint32_t run_fat_chocobo_main_507B91 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x245);
	uint32_t run_fat_chocobo_sub_507BAD = get_relative_call(run_fat_chocobo_main_507B91, 0x10);
	uint32_t run_fat_chocobo_camera_handler_507C64 = get_relative_call(run_fat_chocobo_sub_507BAD, 0x87);
	uint32_t run_fat_chocobo_main_loop_508BED = get_absolute_value(run_fat_chocobo_sub_507BAD, 0x5F);
	ff7_externals.run_fat_chocobo_movement_509692 = get_absolute_value(run_fat_chocobo_main_loop_508BED, 0x103);
	ff7_externals.run_fat_chocobo_camera_507CA4 = get_absolute_value(run_fat_chocobo_camera_handler_507C64, 0x5);
	ff7_externals.run_fat_chocobo_camera_shake_5095F5 = get_absolute_value(run_fat_chocobo_main_loop_508BED, 0x18D);
	uint32_t run_summon_shiva_main_58E411 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x267);
	uint32_t run_summon_shiva_sub_58E4D8 = get_relative_call(run_summon_shiva_main_58E411, 0xBB);
	uint32_t run_summon_shiva_camera_handler_58E5CD = get_relative_call(run_summon_shiva_sub_58E4D8, 0xBB);
	ff7_externals.run_shiva_movement_592538 = get_absolute_value(run_summon_shiva_sub_58E4D8, 0x7B);
	ff7_externals.run_shiva_camera_58E60D = get_absolute_value(run_summon_shiva_camera_handler_58E5CD, 0x5);
	uint32_t run_ifrit_main_5927C1 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x3FC);
	uint32_t run_ifrit_sub_5928FE = get_relative_call(run_ifrit_main_5927C1, 0x12F);
	uint32_t run_ifrit_camera_handler_5929F6 = get_relative_call(run_ifrit_sub_5928FE, 0xC5);
	uint32_t run_ifrit_main_loop_593A95 = get_absolute_value(run_ifrit_sub_5928FE, 0x9D);
	ff7_externals.run_ifrit_movement_596702 = get_absolute_value(run_ifrit_main_loop_593A95, 0x15B);
	ff7_externals.run_ifrit_camera_592A36 = get_absolute_value(run_ifrit_camera_handler_5929F6, 0x5);
	uint32_t run_summon_ramuh_main_596FF1 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x311);
	uint32_t run_summon_ramuh_sub_59706F = get_relative_call(run_summon_ramuh_main_596FF1, 0x70);
	uint32_t run_summon_ramuh_sub_5971C6 = get_relative_call(run_summon_ramuh_sub_59706F, 0x13C);
	ff7_externals.run_ramuh_camera_597206 = get_absolute_value(run_summon_ramuh_sub_5971C6, 0x5);
	uint32_t run_titan_main_59B1DA = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x355);
	uint32_t run_titan_sub_59B273 = get_relative_call(run_titan_main_59B1DA, 0x8B);
	uint32_t run_titan_camera_handler_59B470 = get_relative_call(run_titan_sub_59B273, 0x1CA);
	ff7_externals.run_titan_camera_59B4B0 = get_absolute_value(run_titan_camera_handler_59B470, 0x5);
	uint32_t run_summon_odin_gunge_main_4A0AE1 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x3DD);
	uint32_t run_summon_odin_gunge_sub_4A0B6D = get_relative_call(run_summon_odin_gunge_main_4A0AE1, 0x7F);
	uint32_t run_summon_odin_gunge_camera_handler_4A0F12 = get_relative_call(run_summon_odin_gunge_sub_4A0B6D, 0x1C0);
	uint32_t run_summon_odin_gunge_main_loop_4A0B6D = get_absolute_value(run_summon_odin_gunge_sub_4A0B6D, 0x163);
	ff7_externals.run_odin_gunge_movement_4A584D = get_absolute_value(run_summon_odin_gunge_main_loop_4A0B6D, 0x152);
	ff7_externals.run_odin_gunge_camera_4A0F52 = get_absolute_value(run_summon_odin_gunge_camera_handler_4A0F12, 0x5);
	uint32_t run_summon_odin_steel_main_4A5B61 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x3BB);
	uint32_t run_summon_odin_steel_sub_4A5BE5 = get_relative_call(run_summon_odin_steel_main_4A5B61, 0x77);
	uint32_t run_summon_odin_steel_main_loop_4A8B86 = get_absolute_value(run_summon_odin_steel_sub_4A5BE5, 0xA7);
	uint32_t run_summon_odin_steel_camera_handler_4A5CFC = get_relative_call(run_summon_odin_steel_sub_4A5BE5, 0xF3);
	ff7_externals.run_odin_steel_movement_4A6CB8 = get_absolute_value(run_summon_odin_steel_sub_4A5BE5, 0xCB);
	ff7_externals.run_odin_steel_sub_4A9908 = get_absolute_value(run_summon_odin_steel_main_loop_4A8B86, 0x43E);
	ff7_externals.run_odin_steel_camera_4A5D3C = get_absolute_value(run_summon_odin_steel_camera_handler_4A5CFC, 0x5);
	ff7_externals.field_odin_frames_AEEC14 = (WORD*)get_absolute_value(ff7_externals.run_odin_steel_sub_4A9908, 0x316);
	uint32_t run_leviathan_main_5B048B = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x333);
	uint32_t run_leviathan_sub_5B055B = get_relative_call(run_leviathan_main_5B048B, 0xC2);
	uint32_t run_leviathan_camera_handler_5B06D6 = get_relative_call(run_leviathan_sub_5B055B, 0x148);
	ff7_externals.run_leviathan_camera_5B0716 = get_absolute_value(run_leviathan_camera_handler_5B06D6, 0x5);
	uint32_t run_bahamut_main_4978B9 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x201);
	uint32_t run_bahamut_sub_497919 = get_relative_call(run_bahamut_main_4978B9, 0x52);
	uint32_t run_bahamut_camera_handler_4979F7 = get_relative_call(run_bahamut_sub_497919, 0xAB);
	ff7_externals.run_bahamut_movement_49ADEC = get_absolute_value(run_bahamut_sub_497919, 0x76);
	ff7_externals.run_bahamut_camera_497A37 = get_absolute_value(run_bahamut_camera_handler_4979F7, 0x5);
	uint32_t run_kujata_main_4F9891 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x2EF);
	uint32_t run_kujata_sub_4F9937 = get_relative_call(run_kujata_main_4F9891, 0x98);
	uint32_t run_kujata_camera_handler_4F9A0D = get_relative_call(run_kujata_sub_4F9937, 0x96);
	ff7_externals.run_kujata_camera_4F9A4D = get_absolute_value(run_kujata_camera_handler_4F9A0D, 0x5);
	uint32_t run_alexander_main_501491 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x2CD);
	uint32_t run_alexander_sub_501514 = get_relative_call(run_alexander_main_501491, 0x73);
	uint32_t run_alexander_camera_handler_5015F7 = get_relative_call(run_alexander_sub_501514, 0xA3);
	uint32_t run_alexander_main_loop_50265E = get_absolute_value(run_alexander_sub_501514, 0xC4);
	ff7_externals.run_alexander_movement_5078D8 = get_absolute_value(run_alexander_main_loop_50265E, 0x2E2);
	ff7_externals.run_alexander_camera_501637 = get_absolute_value(run_alexander_camera_handler_5015F7, 0x5);
	uint32_t run_summon_phoenix_main_515101 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x2AB);
	uint32_t run_summon_phoenix_sub_515127 = get_relative_call(run_summon_phoenix_main_515101, 0x1A);
	uint32_t run_summon_phoenix_camera_handler_5151F8 = get_relative_call(run_summon_phoenix_sub_515127, 0x91);
	ff7_externals.run_phoenix_main_loop_516297 = get_absolute_value(run_summon_phoenix_sub_515127, 0xB2);
	ff7_externals.run_phoenix_movement_518AFF = get_absolute_value(ff7_externals.run_phoenix_main_loop_516297, 0x310);
	ff7_externals.run_phoenix_camera_515238 = get_absolute_value(run_summon_phoenix_camera_handler_5151F8, 0x5);
	ff7_externals.run_bahamut_neo_main_48C2A1 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x377);
	uint32_t run_bahamut_neo_sub_48C60B = get_relative_call(ff7_externals.run_bahamut_neo_main_48C2A1, 0x35D);
	uint32_t run_bahamut_neo_camera_handler_48C71D = get_relative_call(run_bahamut_neo_sub_48C60B, 0xD2);
	ff7_externals.run_bahamut_neo_movement_48D7BC = get_absolute_value(run_bahamut_neo_sub_48C60B, 0xAA);
	ff7_externals.run_bahamut_neo_camera_48C75D = get_absolute_value(run_bahamut_neo_camera_handler_48C71D, 0x5);
	uint32_t run_hades_main_4B6351 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x289);
	uint32_t run_hades_sub_4B636D = get_relative_call(run_hades_main_4B6351, 0x10);
	uint32_t run_hades_main_loop_4B636D = get_absolute_value(run_hades_sub_4B636D, 0x4);
	uint32_t run_hades_camera_handler_4B6568 = get_relative_call(run_hades_main_loop_4B636D, 0x1B7);
	ff7_externals.run_hades_camera_4B65A8 = get_absolute_value(run_hades_camera_handler_4B6568, 0x5);
	uint32_t run_typhoon_main_4D5751 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x41B);
	uint32_t run_typhoon_sub_4D57EF = get_relative_call(run_typhoon_main_4D5751, 0x91);
	uint32_t run_typhoon_main_loop_4D69A6 = get_absolute_value(run_typhoon_sub_4D57EF, 0x101);
	uint32_t run_typhoon_camera_handler_4D590C = get_relative_call(run_typhoon_sub_4D57EF, 0xF8);
	ff7_externals.run_typhoon_sub_4DA182 = get_absolute_value(run_typhoon_main_loop_4D69A6, 0x319);
	ff7_externals.run_typhoon_camera_4D594C = get_absolute_value(run_typhoon_camera_handler_4D590C, 0x5);
	uint32_t run_bahamut_zero_main_4835C1 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x399);
	uint32_t run_bahamut_zero_sub_483762 = get_relative_call(run_bahamut_zero_main_4835C1, 0x194);
	uint32_t run_bahamut_zero_camera_handler_483826 = get_relative_call(run_bahamut_zero_sub_483762, 0x94);
	ff7_externals.run_bahamut_zero_main_loop_484A16 = get_absolute_value(run_bahamut_zero_sub_483762, 0x5F);
	ff7_externals.run_bahamut_zero_movement_48BBFC = get_absolute_value(run_bahamut_zero_sub_483762, 0x6C);
	ff7_externals.run_bahamut_zero_camera_483866 = get_absolute_value(run_bahamut_zero_camera_handler_483826, 0x5);
	ff7_externals.bahamut_zero_draw_bg_effect_sub_4859AA = get_absolute_value(ff7_externals.run_bahamut_zero_main_loop_484A16, 0x2E8);
	ff7_externals.bahamut_zero_bg_star_graphics_data_7F6748 = get_absolute_value(ff7_externals.bahamut_zero_draw_bg_effect_sub_4859AA, 0x1BC);
	uint32_t run_summon_kotr_main_476842 = get_relative_call(ff7_externals.run_summon_animations_5C0E4B, 0x43A);
	ff7_externals.run_summon_kotr_sub_476857 = get_relative_call(run_summon_kotr_main_476842, 0xB);
	ff7_externals.run_summon_kotr_main_loop_478031 = get_absolute_value(ff7_externals.run_summon_kotr_sub_476857, 0x1AC);
	ff7_externals.run_summon_kotr_knight_script[0] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x2D3);
	ff7_externals.run_summon_kotr_knight_script[1] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x341);
	ff7_externals.run_summon_kotr_knight_script[2] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x3EA);
	ff7_externals.run_summon_kotr_knight_script[3] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x475);
	ff7_externals.run_summon_kotr_knight_script[4] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x532);
	ff7_externals.run_summon_kotr_knight_script[5] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x5BA);
	ff7_externals.run_summon_kotr_knight_script[6] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x666);
	ff7_externals.run_summon_kotr_knight_script[7] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x6F3);
	ff7_externals.run_summon_kotr_knight_script[8] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x79D);
	ff7_externals.run_summon_kotr_knight_script[9] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x869);
	ff7_externals.run_summon_kotr_knight_script[10] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x934);
	ff7_externals.run_summon_kotr_knight_script[11] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0x9CF);
	ff7_externals.run_summon_kotr_knight_script[12] = get_absolute_value(ff7_externals.run_summon_kotr_main_loop_478031, 0xAB8);
	ff7_externals.add_kotr_camera_fn_to_effect100_fn_476AAB = (void(*)(DWORD, DWORD, WORD))get_relative_call(ff7_externals.run_summon_kotr_sub_476857, 0x1C6);
	ff7_externals.run_kotr_camera_476AFB = get_absolute_value((uint32_t)ff7_externals.add_kotr_camera_fn_to_effect100_fn_476AAB, 0xA);

	ff7_externals.battle_sub_661000 = (vector3<int>*(*)(int))get_relative_call(ff7_externals.run_ifrit_movement_596702, 0x38);
	ff7_externals.engine_set_game_engine_rot_matrix_663673 = (void(*)(rotation_matrix*))get_relative_call(ff7_externals.run_ifrit_movement_596702, 0x169);
	ff7_externals.engine_set_game_engine_position_663707 = (void(*)(rotation_matrix*))get_relative_call(ff7_externals.run_ifrit_movement_596702, 0x176);
	ff7_externals.engine_apply_translation_with_delta_662ECC = (void(*)(vector3<short>*, vector3<int>*, int*))get_relative_call(ff7_externals.run_ifrit_movement_596702, 0x193);
	ff7_externals.byte_BCC788 = (byte*)get_absolute_value(ff7_externals.run_ifrit_movement_596702, 0x1C);
	ff7_externals.ifrit_vector3_int_ptr_BCC6A8 = (vector3<int>**)get_absolute_value(ff7_externals.run_ifrit_movement_596702, 0x41);
	ff7_externals.battle_ifrit_model_position = (vector3<short>*)get_absolute_value(ff7_externals.run_ifrit_movement_596702, 0x12D);
	ff7_externals.ifrit_rot_matrix_BCC768 = (rotation_matrix*)get_absolute_value(ff7_externals.run_ifrit_movement_596702, 0x165);

	// Enemy attacks
	uint32_t battle_sub_5C1484 = ff7_externals.magic_effects_fn_table[7];
	uint32_t battle_sub_55FE60 = get_relative_call(battle_sub_5C1484, 0x33E);
	uint32_t battle_handle_chocobuckle_and_confu_sub_55FE9C = get_relative_call(battle_sub_55FE60, 0x12);
	uint32_t handler_chocobuckle_sub_5609DB = get_absolute_value(battle_handle_chocobuckle_and_confu_sub_55FE9C, 0x27);
	uint32_t handler_confu_magic_sub_55FEF2 = get_absolute_value(battle_handle_chocobuckle_and_confu_sub_55FE9C, 0x36);
	ff7_externals.run_chocobuckle_main_loop_560C32 = get_absolute_value(handler_chocobuckle_sub_5609DB, 0x6F);
	ff7_externals.run_confu_main_loop_5600BE = get_absolute_value(handler_confu_magic_sub_55FEF2, 0x6C);
	uint32_t bomb_blast_effects_5373D0 = ff7_externals.enemy_atk_effects_fn_table[67];
	uint32_t bomb_blast_effects_sub_5373E5 = get_relative_call(bomb_blast_effects_5373D0, 0xB);
	ff7_externals.bomb_blast_black_bg_effect_537427 = get_absolute_value(bomb_blast_effects_sub_5373E5, 0x34);
	uint32_t goblin_punch_effects_572C20 = ff7_externals.enemy_skill_effects_fn_table[17];
	uint32_t goblin_punch_effects_sub_572C3A = get_relative_call(goblin_punch_effects_572C20, 0x10);
	uint32_t goblin_punch_handler_572CE0 = get_absolute_value(goblin_punch_effects_sub_572C3A, 0x70);
	uint32_t goblin_punch_main_loop_572DE7 = get_absolute_value(goblin_punch_handler_572CE0, 0x21);
	ff7_externals.goblin_punch_flash_573291 = get_absolute_value(goblin_punch_main_loop_572DE7, 0x4B);
	uint32_t death_sentence_sub_565F50 = get_relative_call(battle_sub_5C1484, 0x15F);
	uint32_t death_sentence_sub_565F9C = get_relative_call(death_sentence_sub_565F50, 0x1A);
	uint32_t death_sentence_handler_566007 = get_absolute_value(death_sentence_sub_565F9C, 0x5B);
	ff7_externals.roulette_skill_main_loop_566287 = get_absolute_value(death_sentence_handler_566007, 0x2E);
	ff7_externals.death_sentence_main_loop_5661A0 = get_absolute_value(death_sentence_handler_566007, 0x138);
	uint32_t battle_sub_561C20 = get_relative_call(battle_sub_5C1484, 0x97);
	uint32_t death_kill_sub_561C3C = get_relative_call(battle_sub_561C20, 0x10);
	uint32_t death_kill_sub_561C87 = get_absolute_value(death_kill_sub_561C3C, 0x21);
	uint32_t death_kill_main_loop_561FAF = get_absolute_value(death_kill_sub_561C87, 0x93);
	ff7_externals.death_kill_sub_loop_562C60 = get_absolute_value(death_kill_main_loop_561FAF, 0x42);
	ff7_externals.death_kill_sub_loop_5624A5 = get_absolute_value(death_kill_main_loop_561FAF, 0x5F);
	uint32_t enemy_atk_sub_439B71 = ff7_externals.enemy_atk_effects_fn_table[143];
	uint32_t enemy_atk_sub_439B86 = get_relative_call(enemy_atk_sub_439B71, 0xB);
	uint32_t enemy_atk_sub_439C6B = get_absolute_value(enemy_atk_sub_439B86, 0x1C);
	uint32_t enemy_atk_sub_439EA0 = get_relative_call(enemy_atk_sub_439C6B, 0x214);
	ff7_externals.enemy_atk_camera_sub_439EE0 = get_absolute_value(enemy_atk_sub_439EA0, 0x5);
	uint32_t enemy_atk_sub_44A5A0 = ff7_externals.enemy_atk_effects_fn_table[150];
	uint32_t enemy_atk_sub_44A719 = get_relative_call(enemy_atk_sub_44A5A0, 0xB0);
	uint32_t enemy_atk_sub_44A792 = get_relative_call(enemy_atk_sub_44A719, 0x41);
	ff7_externals.enemy_atk_camera_sub_44A7D2 = get_absolute_value(enemy_atk_sub_44A792, 0x5);
	uint32_t enemy_atk_sub_44ECB1 = ff7_externals.enemy_atk_effects_fn_table[149];
	uint32_t enemy_atk_sub_44ED00 = get_relative_call(enemy_atk_sub_44ECB1, 0x44);
	uint32_t enemy_atk_sub_44ED80 = get_relative_call(enemy_atk_sub_44ED00, 0x48);
	ff7_externals.enemy_atk_camera_sub_44EDC0 = get_absolute_value(enemy_atk_sub_44ED80, 0x5);
	uint32_t enemy_atk_sub_427A6C = ff7_externals.enemy_atk_effects_fn_table[147];
	uint32_t enemy_atk_sub_4520C1 = get_relative_call(enemy_atk_sub_427A6C, 0x4D);
	uint32_t enemy_atk_sub_452170 = get_relative_call(enemy_atk_sub_4520C1, 0xA3);
	uint32_t enemy_atk_sub_45226D = get_relative_call(enemy_atk_sub_452170, 0xDC);
	ff7_externals.enemy_atk_camera_sub_4522AD = get_absolute_value(enemy_atk_sub_45226D, 0x5);
	uint32_t enemy_atk_sub_457B20 = ff7_externals.enemy_atk_effects_fn_table[142];
	uint32_t enemy_atk_sub_457B4C = get_relative_call(enemy_atk_sub_457B20, 0x22);
	uint32_t enemy_atk_sub_457C20 = get_relative_call(enemy_atk_sub_457B4C, 0xB4);
	ff7_externals.enemy_atk_camera_sub_457C60 = get_absolute_value(enemy_atk_sub_457C20, 0x5);
	ff7_externals.pollensalta_cold_breath_atk_enter_sub_5474F0 = ff7_externals.enemy_atk_effects_fn_table[59];
	uint32_t pollensalta_cold_breath_atk_main_sub_547595 = get_relative_call(ff7_externals.pollensalta_cold_breath_atk_enter_sub_5474F0, 0x99);
	uint32_t pollensalta_cold_breath_atk_callback_sub_5455E7 = get_absolute_value(pollensalta_cold_breath_atk_main_sub_547595, 0x4);
	ff7_externals.pollensalta_cold_breath_atk_main_loop_5476B0 = get_absolute_value(pollensalta_cold_breath_atk_callback_sub_5455E7, 0x7);
	ff7_externals.pollensalta_cold_breath_atk_draw_bg_effect_547B94 = get_absolute_value(ff7_externals.pollensalta_cold_breath_atk_main_loop_5476B0, 0x16F);
	ff7_externals.pollensalta_cold_breath_atk_white_dot_effect_547D56 = get_absolute_value(ff7_externals.pollensalta_cold_breath_atk_main_loop_5476B0, 0x39);
	ff7_externals.pollensalta_cold_breath_atk_draw_white_dots_547E75 = (void(*)(short))get_relative_call(ff7_externals.pollensalta_cold_breath_atk_white_dot_effect_547D56, 0x20);
	ff7_externals.pollensalta_cold_breath_white_dots_pos = std::span((vector4<short>*) get_absolute_value(ff7_externals.pollensalta_cold_breath_atk_white_dot_effect_547D56, 0x79), 400);
	ff7_externals.pollensalta_cold_breath_white_dot_rgb_scalar = (short*) get_absolute_value(ff7_externals.pollensalta_cold_breath_atk_white_dot_effect_547D56, 0x1B);
	ff7_externals.pollensalta_cold_breath_bg_texture_ctx = get_absolute_value(ff7_externals.pollensalta_cold_breath_atk_draw_bg_effect_547B94, 0x2A);
	uint32_t pandora_box_skill_enter_5667E1 = ff7_externals.enemy_skill_effects_fn_table[23];
	uint32_t pandora_box_skill_main_566806 = get_relative_call(pandora_box_skill_enter_5667E1, 0x1A);
	uint32_t pandora_box_skill_sub_566871 = get_absolute_value(pandora_box_skill_main_566806, 0x30);
	uint32_t pandora_box_skill_main_loop_566E61 = get_absolute_value(pandora_box_skill_sub_566871, 0x1F);
	ff7_externals.pandora_box_skill_draw_bg_flash_effect_568371 = get_absolute_value(pandora_box_skill_main_loop_566E61, 0x162);

	// Texture/Material animation
	uint32_t battle_leviathan_sub_5B2F18 = get_absolute_value(ff7_externals.battle_summon_leviathan_loop, 0x50E);
	ff7_externals.battle_animate_material_texture = get_relative_call(battle_leviathan_sub_5B2F18, 0x19F);
	ff7_externals.get_global_model_matrix_buffer_66100D = (rotation_matrix*(*)())get_relative_call(ff7_externals.battle_animate_material_texture, 0x5E);
	ff7_externals.get_draw_chain_68F860 = (struc_84*(*)(struc_49*, graphics_instance*))get_relative_call(ff7_externals.battle_animate_material_texture, 0x85);
	ff7_externals.battle_sub_5D1AAA = (p_hundred*(*)(int, ff7_polygon_set*))get_relative_call(ff7_externals.battle_animate_material_texture, 0xB3);
	ff7_externals.get_alpha_from_transparency_429343 = (int(*)(int))get_relative_call(ff7_externals.battle_animate_material_texture, 0xDF);
	ff7_externals.get_stored_color_66101A = (color_ui8(*)())get_relative_call(ff7_externals.battle_animate_material_texture, 0x110);
	ff7_externals.battle_sub_68CF75 = (void(*)(char, struc_173*))get_relative_call(ff7_externals.battle_animate_material_texture, 0x1CD);
	ff7_externals.create_rot_matrix_from_word_matrix_6617E9 = (void(*)(rotation_matrix*, matrix*))get_relative_call(ff7_externals.battle_animate_material_texture, 0x38B);
	ff7_externals.battle_animate_texture_spt = get_relative_call(ff7_externals.summon_aura_effects_5C0953, 0x16A);
	ff7_externals.get_draw_chain_671C71 = (struc_84*(*)(ff7_graphics_object*))get_relative_call(ff7_externals.battle_animate_texture_spt, 0x15F);
	ff7_externals.palette_extra_data_C06A00 = (palette_extra*)get_absolute_value(ff7_externals.battle_animate_material_texture, 0x2FA);
	ff7_externals.global_game_engine_data = (ff7_game_engine_data**)get_absolute_value((uint32_t)ff7_externals.get_global_model_matrix_buffer_66100D, 0x4);

	// Battle menu
	uint32_t battle_sub_6D83C8 = get_relative_call(ff7_externals.battle_menu_update_6CE8B3, 0x77);
	uint32_t battle_sub_6D82EA = get_relative_call(battle_sub_6D83C8, 0xE0);
	ff7_externals.display_battle_menu_6D797C = get_relative_call(battle_sub_6D82EA, 0x59);
	ff7_externals.display_tifa_slots_handler_6E3135 = (void(*)())get_relative_call(ff7_externals.display_battle_menu_6D797C, 0x1C2);
	ff7_externals.battle_draw_text_ui_graphics_objects_call = battle_main_loop + 0x289;
	ff7_externals.battle_draw_box_ui_graphics_objects_call = battle_main_loop + 0x2CF;
	ff7_externals.battle_draw_call_42908C = (void(*)(int, int))get_relative_call(battle_main_loop, 0x2CF);
	ff7_externals.battle_set_do_render_menu_call = battle_main_loop + 0x32A;
	ff7_externals.battle_set_do_render_menu = get_relative_call(battle_main_loop, 0x32A);
	ff7_externals.g_do_render_menu = (int*)get_absolute_value(ff7_externals.battle_set_do_render_menu, 0x7);
	uint32_t battle_sub_4297B9 = get_relative_call(ff7_externals.battle_sub_42D992, 0x59);
	uint32_t battle_sub_42952E = get_relative_call(battle_sub_4297B9, 0x10);
	ff7_externals.battle_sub_42F3E8 = get_relative_call(battle_sub_42952E, 0xCD);
	uint32_t battle_sub_5B9B30 = get_relative_call(ff7_externals.battle_sub_42F3E8, 0x756);
	ff7_externals.battle_handle_status_effect_anim_5BA7C0 = get_relative_call(battle_sub_5B9B30, 0xB2);
	ff7_externals.battle_handle_player_mark_5B9C8E = get_relative_call(battle_sub_5B9B30, 0x123);
	ff7_externals.battle_update_targeting_info_6E6291 = get_relative_call(ff7_externals.battle_sub_6DB0EE, 0x1F9);
	ff7_externals.targeting_actor_id_DC3C98 = (byte*)get_absolute_value(ff7_externals.battle_update_targeting_info_6E6291, 0x684);
	ff7_externals.battle_menu_closing_window_box_6DAEF0 = get_relative_call(ff7_externals.battle_sub_6DB0EE, 0x1D8);

	// 3D Battleground
	ff7_externals.update_3d_battleground = get_relative_call(ff7_externals.battle_sub_42D992, 0x4);
	ff7_externals.battleground_shake_train_42F088 = (void(*)())get_relative_call(ff7_externals.update_3d_battleground, 0xBF);
	ff7_externals.battleground_vertical_scrolling_42F126 = get_relative_call(ff7_externals.update_3d_battleground, 0x783);
	ff7_externals.battleground_midgar_flashback_rain_5BDC4F = get_relative_call(ff7_externals.update_3d_battleground, 0x3C);

	// World externals
	ff7_externals.world_exit_74BD77 = get_absolute_value(main_loop, 0x993);
	ff7_externals.world_exit_destroy_graphics_objects_75A921 = (void(*)())get_relative_call(ff7_externals.world_exit_74BD77, 0x11);
	ff7_externals.world_init_variables_74E1E9 = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x108);
	ff7_externals.world_sub_7641A7 = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x210);
	ff7_externals.world_init_load_wm_bot_block_7533AF = (void(*)())get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x296);
	ff7_externals.run_world_event_scripts = get_relative_call(ff7_externals.world_sub_7641A7, 0x1D);
	ff7_externals.run_world_event_scripts_system_operations = get_relative_call(ff7_externals.run_world_event_scripts, 0xC7);
	ff7_externals.pop_world_script_stack = (int(*)())get_relative_call(ff7_externals.run_world_event_scripts_system_operations, 0x44);
	ff7_externals.world_animate_all_models = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x5A1);
	ff7_externals.world_animate_single_model = get_relative_call(ff7_externals.world_animate_all_models, 0x20);
	ff7_externals.run_world_snake_ai_script_7562FF = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x5AB);
	ff7_externals.world_sub_75EF46 = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x44E);
	ff7_externals.world_sub_767540 = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x5BE);
	ff7_externals.world_sub_767641 = get_relative_call(ff7_externals.world_sub_767540, 0xCB);
	ff7_externals.get_world_encounter_rate = (int(*)())get_relative_call(ff7_externals.world_sub_767641, 0x110);
	ff7_externals.update_world_snake_position_7564CD = get_relative_call(ff7_externals.run_world_snake_ai_script_7562FF, 0x151);
	ff7_externals.is_update_snake_enabled_7562A9 = get_relative_call(ff7_externals.run_world_snake_ai_script_7562FF, 0x12);
	ff7_externals.animate_world_snake_75692A = get_relative_call(ff7_externals.run_world_snake_ai_script_7562FF, 0x16C);
	ff7_externals.sub_753366 = (bool (*)(short, short))get_relative_call(ff7_externals.animate_world_snake_75692A, 0x93);
	ff7_externals.world_draw_snake_texture_75D544 = (void (*)(short, short, short, short, world_snake_graphics_data*, short))get_relative_call(ff7_externals.animate_world_snake_75692A, 0x159);
	ff7_externals.world_snake_data_position_ptr_E2A18C = (vector4<short> **)get_absolute_value(ff7_externals.update_world_snake_position_7564CD, 0x2F);
	ff7_externals.world_snake_data_position_E29F80 = (vector4<short> *)get_absolute_value((uint32_t)ff7_externals.animate_world_snake_75692A, 0x2A);
	ff7_externals.world_snake_graphics_data_E2A490 = (world_snake_graphics_data *)get_absolute_value(ff7_externals.animate_world_snake_75692A, 0x3A);
	ff7_externals.world_snake_graphics_data_end_E2A6D0 = (world_snake_graphics_data *)get_absolute_value(ff7_externals.animate_world_snake_75692A, 0x4C);
	ff7_externals.snake_position_size_of_array_E2A100 = (vector4<short> *)get_absolute_value(ff7_externals.update_world_snake_position_7564CD, 0x3C);
	ff7_externals.world_opcode_message_sub_75EE86 = get_relative_call(ff7_externals.run_world_event_scripts_system_operations, 0xB6D);
	ff7_externals.world_opcode_ask_sub_75EEBB = get_relative_call(ff7_externals.run_world_event_scripts_system_operations, 0xBA1);
	ff7_externals.world_opcode_message = get_relative_call(ff7_externals.world_sub_75EF46, 0x8C);
	ff7_externals.world_opcode_ask = get_relative_call(ff7_externals.world_sub_75EF46, 0xAF);
	ff7_externals.world_text_box_window_opening_769A66 = get_relative_call(ff7_externals.world_opcode_message, 0x5A);
	ff7_externals.world_text_box_window_paging_769C02 = get_relative_call(ff7_externals.world_opcode_message, 0x6D);
	ff7_externals.world_text_box_reverse_paging_76ABE9 = get_relative_call(ff7_externals.world_opcode_message, 0x80);
	ff7_externals.world_text_box_window_closing_76ADF7 = get_relative_call(ff7_externals.world_opcode_message, 0x235);
	ff7_externals.world_update_player_74EA48 = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x2D7);
	ff7_externals.world_get_player_model_id = (int(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0x1B);
	ff7_externals.world_get_current_key_input_status = (int(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0x4A);
	ff7_externals.world_get_player_walkmap_type = (int(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0x7DF);
	ff7_externals.world_sub_753D00 = (void(*)(vector3<short>*, short))get_relative_call(ff7_externals.run_world_event_scripts_system_operations, 0xDF9);
	ff7_externals.world_update_model_movement_762E87 = (void(*)(int, int))get_relative_call(ff7_externals.world_update_player_74EA48, 0xCDF);
	ff7_externals.engine_apply_rotation_to_transform_matrix_6628DE = (void(*)(vector3<short>*, rotation_matrix*))get_relative_call(ff7_externals.world_update_player_74EA48, 0x733);
	ff7_externals.world_is_player_model_bitmask = (bool(*)(int))get_relative_call(ff7_externals.world_update_player_74EA48, 0x9F);
	ff7_externals.world_copy_player_pos_to_param_762798 = (void(*)(vector4<int>*))get_relative_call(ff7_externals.world_update_player_74EA48, 0x7B8);
	ff7_externals.world_set_current_entity_to_player_entity = (void(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0x16);
	ff7_externals.world_add_y_pos_to_current_entity_761F22 = (void(*)(int))get_relative_call(ff7_externals.world_update_player_74EA48, 0x7FD);
	ff7_externals.world_add_delta_movement_due_to_bridge_7591C2 = (void(*)(int*, int*))get_relative_call(ff7_externals.world_update_player_74EA48, 0xCAA);
	ff7_externals.world_current_entity_model_collision_detection_with_other_models_76296E = (void(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0xCE7);
	ff7_externals.world_get_unknown_flag_75335C = (int(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0x3D);
	ff7_externals.world_get_minimap_mask = (short(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0xBC8);
	ff7_externals.world_set_minimap_mask = (void(*)(short))get_relative_call(ff7_externals.world_update_player_74EA48, 0xC00);
	ff7_externals.world_set_facing_and_direction_to_current_entity = (void(*)(short))get_relative_call(ff7_externals.world_update_player_74EA48, 0x6F3);
	ff7_externals.world_is_current_entity_animated_761F44 = (bool(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0x8D0);
	ff7_externals.world_sub_74D6BB = (void(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0xB71);
	ff7_externals.world_sub_74D6F6 = (void(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0xB82);
	ff7_externals.world_sub_762F75 = (void(*)(short, short, short))get_relative_call(ff7_externals.world_update_player_74EA48, 0x9A3);
	ff7_externals.world_run_special_opcode_7640BC = (void(*)(int))get_relative_call(ff7_externals.world_update_player_74EA48, 0xC30);
	ff7_externals.world_set_camera_fade_speed_755B97 = (void(*)(int))get_relative_call(ff7_externals.world_update_player_74EA48, 0xC56);
	ff7_externals.world_set_world_control_lock_74D438 = (void(*)(int, int))get_relative_call(ff7_externals.world_update_player_74EA48, 0xC62);
	ff7_externals.world_sub_74C980 = (void(*)(int))get_relative_call(ff7_externals.world_update_player_74EA48, 0xC76);
	ff7_externals.world_sub_753BE8 = (void(*)())get_relative_call(ff7_externals.world_update_player_74EA48, 0xC8A);
	ff7_externals.world_music_set_frequency_all_channels_75E6A8 = (void(*)(byte, char))get_relative_call(ff7_externals.world_update_player_74EA48, 0xB05);
	ff7_externals.world_sfx_play_or_stop_75E6CC = (void(*)(int))get_relative_call(ff7_externals.world_update_player_74EA48, 0xCCF);
	ff7_externals.world_set_camera_view_type_74D3D1 = (void(*)(int))get_relative_call(ff7_externals.world_update_player_74EA48, 0x79);

	ff7_externals.world_compute_all_models_data_76323A = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x5A1);
	ff7_externals.world_compute_3d_model_data_76328F = get_relative_call(ff7_externals.world_compute_all_models_data_76323A, 0x20);
	ff7_externals.world_sub_74D319 = get_relative_call(ff7_externals.world_compute_3d_model_data_76328F, 0x7D4);
	ff7_externals.world_sub_762F9A = get_relative_call(ff7_externals.world_compute_3d_model_data_76328F, 0x419);

	ff7_externals.world_update_camera_74E8CE = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x40F);
	ff7_externals.world_snowstorm_get_camera_movement_758B12 = (int(*)(int, int))get_relative_call(ff7_externals.world_update_player_74EA48, 0x38C);
	ff7_externals.world_get_camera_rotation_x_74F916 = (int(*)())get_relative_call(ff7_externals.world_update_camera_74E8CE, 0x6);
	ff7_externals.world_highwind_height_lowerbound_DF5420 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x7EE);
	ff7_externals.world_mode_E045E4 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0xC21);
	ff7_externals.previous_player_direction_DF5434 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0xA5B);
	ff7_externals.world_is_control_enabled_DE6B5C = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x25);
	ff7_externals.world_special_delta_movement_DE6A18 = (short*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x5D6);
	ff7_externals.world_y_player_pos_flag_DE6A14 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x875);
	ff7_externals.world_unk_rotation_value_E045E0 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x972);
	ff7_externals.world_event_current_entity_ptr_E39AD8 = (world_event_data**)get_absolute_value((uint32_t)ff7_externals.world_update_model_movement_762E87, 0x5);
	ff7_externals.world_event_current_entity_ptr_E3A7CC = (world_event_data**)get_absolute_value(ff7_externals.run_world_event_scripts_system_operations, 0x8E6);
	ff7_externals.world_progress_E28CB4 = (int*)get_absolute_value((uint32_t)ff7_externals.world_init_load_wm_bot_block_7533AF, 0xA1);
	ff7_externals.is_wait_frames_zero_E39BC0 = (int*)get_absolute_value(ff7_externals.run_world_event_scripts_system_operations, 0xD46);
	ff7_externals.world_prev_key_input_status_DFC470 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x35A);
	ff7_externals.world_map_type_E045E8 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x66);
	ff7_externals.world_movement_multiplier_DFC480 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0xDA);
	ff7_externals.world_camera_var1_DF542C = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x1BD);
	ff7_externals.world_camera_var2_DE6B4C = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0xD2F);
	ff7_externals.world_camera_var3_DE6A0C = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x179);
	ff7_externals.world_camera_viewtype_DFC4B4 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x54);
	ff7_externals.world_camera_front_DFC484 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0x39E);
	ff7_externals.world_camera_rotation_y_DFC474 = (int*)get_absolute_value(ff7_externals.world_update_player_74EA48, 0xD51);
	ff7_externals.world_camera_position_z_DFC478 = (int*)get_absolute_value(ff7_externals.world_update_camera_74E8CE, 0x79);
	ff7_externals.world_camera_delta_y_DE6A04 = (int*)get_absolute_value(ff7_externals.world_update_camera_74E8CE, 0xAB);
	ff7_externals.world_current_camera_rotation_x_DE7418 = (short*)get_absolute_value(ff7_externals.world_update_camera_74E8CE, 0x117);
	ff7_externals.world_camera_rotation_z_DE6B70 = (int*)get_absolute_value(ff7_externals.world_update_camera_74E8CE, 0x149);
	ff7_externals.world_camera_x_rotation_array_E37120 = std::span((short*)get_absolute_value(ff7_externals.world_update_camera_74E8CE, 0x103), 512);
	ff7_externals.world_camera_position_matrix_DE6A20 = (rotation_matrix*)get_absolute_value(ff7_externals.world_update_camera_74E8CE, 0xBF);
	ff7_externals.world_camera_direction_matrix_DFC448 = (rotation_matrix*)get_absolute_value(ff7_externals.world_update_camera_74E8CE, 0x162);
	ff7_externals.world_sub_75A1C6 = get_relative_call(ff7_externals.world_init_variables_74E1E9, 0x3A);
	ff7_externals.world_load_graphics_objects_75A5D5 = get_relative_call(ff7_externals.world_sub_75A1C6, 0x61);
	ff7_externals.world_init_load_map_meshes_graphics_objects_75A283 = get_relative_call(ff7_externals.world_load_graphics_objects_75A5D5, 0x340);
	ff7_externals.world_wm0_overworld_draw_all_74C179 = (void(*)())get_absolute_value(ff7_externals.world_init_load_map_meshes_graphics_objects_75A283, 0xA7);
	ff7_externals.world_wm2_underwater_draw_all_74C3F0 = (void(*)())get_absolute_value(ff7_externals.world_init_load_map_meshes_graphics_objects_75A283, 0xDF);
	ff7_externals.world_wm3_snowstorm_draw_all_74C589 = (void(*)())get_absolute_value(ff7_externals.world_init_load_map_meshes_graphics_objects_75A283, 0x117);
	ff7_externals.world_draw_all_3d_model_74C6B0 =  get_relative_call((uint32_t)ff7_externals.world_wm0_overworld_draw_all_74C179, 0x155);
	ff7_externals.world_draw_fade_quad_75551A = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x554);
	ff7_externals.world_sub_75079D = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x421);
	ff7_externals.world_sub_751EFC = get_relative_call(ff7_externals.world_sub_75079D, 0x1FB);
	ff7_externals.world_sub_75C02B = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x5A6);
	ff7_externals.world_sub_75C0FD = get_relative_call(ff7_externals.world_sub_75C02B, 0x43);
	ff7_externals.world_sub_75C283 = get_relative_call(ff7_externals.world_sub_75C0FD, 0x175);
	ff7_externals.world_culling_bg_meshes_75F263 = get_relative_call(ff7_externals.world_sub_751EFC, 0x7C6);
	ff7_externals.world_submit_draw_bg_meshes_75F68C = get_relative_call(ff7_externals.world_sub_751EFC, 0x7FD);
	ff7_externals.world_compute_skybox_data_754100 = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x537);
	ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 = get_relative_call(ff7_externals.world_mode_loop_sub_74DB8C, 0x547);
	ff7_externals.world_sub_75F0AD = get_relative_call(ff7_externals.world_sub_751EFC, 0x551);
	ff7_externals.world_sub_75042B = get_relative_call(ff7_externals.world_compute_3d_model_data_76328F, 0x37D);
	ff7_externals.world_player_pos_E04918 = (vector4<int>*)get_absolute_value(ff7_externals.world_sub_75042B, 0xE);
		ff7_externals.sub_74C9A5 = (int (*)())get_relative_call(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6, 0x09);
	ff7_externals.is_meteor_flag_on_E2AAE4 = (int*)get_absolute_value(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6, 0x592);
	ff7_externals.engine_apply_4x4_matrix_product_with_game_obj_matrix_67D2BF = get_relative_call(ff7_externals.world_compute_skybox_data_754100, 0x85);
	ff7_externals.engine_apply_4x4_matrix_product_between_matrices_66C6CD = (void (*)(struct matrix *, struct matrix *, struct matrix *))get_relative_call(ff7_externals.engine_apply_4x4_matrix_product_with_game_obj_matrix_67D2BF, 0x16);
	ff7_externals.world_copy_position_75042B = (void (*)(vector4<int>* a1))get_relative_call(ff7_externals.world_sub_75C0FD, 0x52);
	ff7_externals.get_world_camera_front_rot_74D298 = (int (*)())get_relative_call(ff7_externals.world_sub_75C0FD, 0xF);
	ff7_externals.engine_apply_rotation_to_rot_matrix_662AD8 = (void (*)(vector3<short>*, transform_matrix*))get_relative_call(ff7_externals.world_sub_75C0FD, 0x28);
	ff7_externals.world_get_world_current_camera_rotation_x_74D3C6 = (short (*)())get_relative_call(ff7_externals.world_sub_75C0FD, 0x30);
	ff7_externals.world_submit_draw_effects_75C283 = (int (_stdcall *)(world_texture_data *, int, vector3<short>*, short))get_relative_call(ff7_externals.world_sub_75C0FD, 0x175);
	ff7_externals.dword_E35648 = (world_effect_2d_list_node**)get_absolute_value(ff7_externals.world_sub_75C0FD, 0x5B);
	ff7_externals.byte_96D6A8 = (byte*)get_absolute_value(ff7_externals.world_sub_75C0FD, 0x169);

	// Swirl externals
	ff7_externals.swirl_main_loop = swirl_main_loop;
	ff7_externals.swirl_loop_sub_4026D4 = get_relative_call(swirl_main_loop, 0xC9);
	ff7_externals.swirl_enter_40164E = get_absolute_value(main_loop, 0x254);
	ff7_externals.swirl_enter_sub_401810 = get_relative_call(ff7_externals.swirl_enter_40164E, 0x160);

	// --------------------------------

	// battle dialogues
	ff7_externals.add_text_to_display_queue = get_relative_call(ff7_externals.battle_sub_42CBF9, 0x1C7);
	ff7_externals.update_display_text_queue = get_relative_call(ff7_externals.battle_sub_42D808, 0x2B);
	ff7_externals.set_battle_text_active = get_relative_call(ff7_externals.update_display_text_queue, 0x14A);
	ff7_externals.battle_sfx_play_effect_430D14 = get_relative_call(ff7_externals.update_display_text_queue, 0x46);
	ff7_externals.battle_sub_66C3BF = (int(*)())get_relative_call(ff7_externals.update_display_text_queue, 0x139);
	ff7_externals.battle_sub_43526A = get_relative_call(ff7_externals.battle_loop, 0x475);
	ff7_externals.battle_sub_5C8931 = get_relative_call(ff7_externals.battle_sub_43526A, 0x1F0);
	ff7_externals.run_enemy_ai_script = get_relative_call(ff7_externals.battle_sub_5C8931, 0xA0);
	ff7_externals.enqueue_script_action = get_relative_call(ff7_externals.run_enemy_ai_script, 0xB7F);
	ff7_externals.battle_sub_41B577 = get_relative_call(ff7_externals.battle_enter, 0x17);
	ff7_externals.battle_sub_41CCB2 = get_relative_call(ff7_externals.battle_sub_41B577, 0xB);

	ff7_externals.battle_display_text_queue = std::span((battle_text_data*)get_absolute_value(ff7_externals.add_text_to_display_queue, 0x25), 64);
	ff7_externals.battle_context = (battle_ai_context*)get_absolute_value(ff7_externals.battle_sub_41CCB2, 0x5F);
	ff7_externals.anim_event_queue = std::span((battle_anim_event*)get_absolute_value(ff7_externals.battle_sub_42CBF9, 0x23), 64);
	ff7_externals.anim_event_index = (byte*)get_absolute_value(ff7_externals.battle_sub_42CBF9, 0x19);
	ff7_externals.g_is_battle_running_9AD1AC = (int*)get_absolute_value(battle_main_loop, 0x247);
	ff7_externals.field_battle_word_BF2E08 = (WORD*)get_absolute_value(ff7_externals.update_display_text_queue, 0xA);
	ff7_externals.field_battle_word_BF2032 = (WORD*)get_absolute_value(ff7_externals.update_display_text_queue, 0x12C);
	ff7_externals.g_active_actor_id = (byte*)get_absolute_value(ff7_externals.display_battle_action_text_42782A, 0x52);
	// --------------------------------

	// Widescreen
	ff7_externals.field_culling_model_639252 = get_relative_call(ff7_externals.field_animate_3d_models_6392BB, 0x203);
	ff7_externals.field_sub_63AC66 = get_relative_call(ff7_externals.sub_60DF96, 0xB0);
	ff7_externals.field_sub_63AC3F = (void(*)(int, int, int, int))get_relative_call(ff7_externals.field_sub_63AC66, 0xD5);

	ff7_externals.battle_draw_quad_5BD473 = get_relative_call(ff7_externals.battle_boss_death_call_5BD436, 0x16);
	ff7_externals.battle_sub_5895E0 = ff7_externals.enemy_atk_effects_fn_table[119];
	ff7_externals.battle_sub_589827 = get_relative_call(ff7_externals.battle_sub_5895E0, 0x10D);
	ff7_externals.battle_sub_58AC59 = get_absolute_value(ff7_externals.battle_sub_589827, 0x64);
	ff7_externals.battle_sub_58ACB9 = get_relative_call(ff7_externals.battle_sub_58AC59, 0x22);
	ff7_externals.ifrit_sub_595A05 = get_absolute_value(run_ifrit_main_loop_593A95, 0x51B);
	ff7_externals.engine_draw_sub_66A47E = (void(*)(int))get_relative_call(ff7_externals.ifrit_sub_595A05, 0x930);
	ff7_externals.battle_viewport_height = (int*)get_absolute_value(battle_main_loop, 0x151);
	ff7_externals.neo_bahamut_main_loop_48DA7A = get_absolute_value(run_bahamut_neo_sub_48C60B, 0xF3);
	ff7_externals.neo_bahamut_effect_sub_490F2A = get_absolute_value(ff7_externals.neo_bahamut_main_loop_48DA7A, 0x2CD);
	ff7_externals.odin_gunge_effect_sub_4A4BE6 = get_absolute_value(run_summon_odin_gunge_main_loop_4A0B6D, 0xA7);
	ff7_externals.odin_gunge_effect_sub_4A3A2E = get_absolute_value(run_summon_odin_gunge_main_loop_4A0B6D, 0x2D8);
	ff7_externals.typhoon_sub_4D6FF8 = get_relative_call(run_typhoon_main_loop_4D69A6, 0x4B7);
	ff7_externals.typhoon_effect_sub_4D7044 = get_absolute_value(ff7_externals.typhoon_sub_4D6FF8, 0xF);
	ff7_externals.typhoon_effect_sub_4DB15F = get_absolute_value(run_typhoon_main_loop_4D69A6, 0x416);
	ff7_externals.fat_chocobo_sub_5096F3 = get_absolute_value(run_fat_chocobo_main_loop_508BED, 0x110);
	uint32_t barret_limit_3_1_main_46FF90 = ff7_externals.limit_break_effects_fn_table[11];
	uint32_t barret_limit_3_1_sub_46FFAC = get_relative_call(barret_limit_3_1_main_46FF90, 0x10);
	uint32_t barret_limit_3_1_sub_470031 = get_absolute_value(barret_limit_3_1_sub_46FFAC, 0x44);
	ff7_externals.barret_limit_3_1_sub_4700F7 = get_absolute_value(barret_limit_3_1_sub_470031, 0x36);
	uint32_t shadow_flare_enemy_skill_entry_576FD0 = ff7_externals.enemy_skill_effects_fn_table[22];
	uint32_t shadow_flare_enemy_skill_sub_576FEA = get_relative_call(shadow_flare_enemy_skill_entry_576FD0, 0x10);
	uint32_t shadow_flare_enemy_skill_main_loop_57708E = get_absolute_value(shadow_flare_enemy_skill_sub_576FEA, 0x70);
	ff7_externals.shadow_flare_draw_white_bg_57747E = get_relative_call(shadow_flare_enemy_skill_main_loop_57708E, 0x6C);

	ff7_externals.cdcheck_enter_sub = get_absolute_value(main_loop, 0x390);
	ff7_externals.credits_submit_draw_fade_quad_7AA89B = get_relative_call(credits_main_loop, 0xD9);
	ff7_externals.get_button_pressed = (int(*)(int))get_relative_call(credits_main_loop, 0x14C);
	ff7_externals.credits_main_loop = credits_main_loop;
	ff7_externals.menu_submit_draw_fade_quad_6CD64E = get_relative_call(ff7_externals.menu_battle_end_sub_6C9543, 0x104);
	ff7_externals.highway_submit_fade_quad_659532 = get_relative_call(ff7_externals.highway_loop_sub_650F36, 0x126);
	ff7_externals.chocobo_init_viewport_values_76D320 = get_relative_call(main_init_loop, 0x38B);
	uint32_t chocobo_sub_77C462 = get_relative_call(chocobo_main_loop, 0x5E);
	uint32_t chocobo_sub_77946A = get_relative_call(chocobo_sub_77C462, 0x649);
	ff7_externals.chocobo_submit_draw_fade_quad_77B1CE = get_relative_call(chocobo_sub_77946A, 0x33);
	ff7_externals.chocobo_submit_draw_water_quad_77A7D0 = get_relative_call(chocobo_sub_77C462, 0x30B);
	ff7_externals.generic_submit_quad_graphics_object_671D2A = (void(*)(int, int, int, int, int, int, float, DWORD*))get_relative_call(ff7_externals.chocobo_submit_draw_water_quad_77A7D0, 0x9F);
	ff7_externals.chocobo_fade_quad_data_97A498 = (byte*)get_absolute_value(chocobo_sub_77946A, 0x2F);
	ff7_externals.snowboard_draw_sky_and_mountains_72DAF0 = get_relative_call(ff7_externals.snowboard_loop_sub_72381C, 0x27);
	ff7_externals.snowboard_submit_draw_sky_quad_graphics_object_72E31F = get_relative_call(ff7_externals.snowboard_draw_sky_and_mountains_72DAF0, 0x24D);
	ff7_externals.snowboard_sky_quad_pos_x_7B7DB8 = (float*)get_absolute_value(ff7_externals.snowboard_submit_draw_sky_quad_graphics_object_72E31F, 0x2E);
	uint32_t snowboard_callable_submit_draw_sub_723F60 = get_absolute_value(ff7_externals.snowboard_loop_sub_72381C, 0x10E);
	uint32_t snowboard_callable_draw_black_quad_7241E5 = get_absolute_value(snowboard_callable_submit_draw_sub_723F60, 0x15C);
	uint32_t snowboard_submit_draw_fade_black_quad_729926 = get_relative_call(snowboard_callable_draw_black_quad_7241E5, 0x31);
	ff7_externals.snowboard_submit_draw_black_quad_graphics_object_72DD94 = get_relative_call(snowboard_submit_draw_fade_black_quad_729926, 0xD);
	uint32_t snowboard_callable_draw_white_quad_7240D6 = get_absolute_value(snowboard_callable_submit_draw_sub_723F60, 0x165);
	uint32_t snowboard_submit_draw_white_fade_quad_729912 = get_relative_call(snowboard_callable_draw_white_quad_7240D6, 0x38);
	ff7_externals.snowboard_submit_draw_white_fade_quad_graphics_object_72DD53 = get_relative_call(snowboard_submit_draw_white_fade_quad_729912, 0xD);
	uint32_t snowboard_submit_draw_opaque_quad_72993A = get_relative_call(snowboard_callable_draw_white_quad_7240D6, 0xCC);
	ff7_externals.snowboard_submit_draw_opaque_quad_graphics_object_72DDD5 = get_relative_call(snowboard_submit_draw_opaque_quad_72993A, 0xD);
	ff7_externals.sub_735220 = get_relative_call(ff7_externals.snowboard_loop_sub_72381C, 0xBF);
	ff7_externals.sub_735332 = get_relative_call(ff7_externals.sub_735220, 0xE6);
	ff7_externals.snowboard_parse_model_vertices_732159 = get_relative_call(ff7_externals.sub_735332, 0x29);
	ff7_externals.sub_7322D6 = (char* (*)(tmd_primitive_packet*, int, int))get_relative_call(ff7_externals.snowboard_parse_model_vertices_732159, 0x10E);
	ff7_externals.sub_732429 = (char* (*)(tmd_primitive_packet*, int, int))get_relative_call(ff7_externals.snowboard_parse_model_vertices_732159, 0x12A);
	ff7_externals.sub_732546 = (char* (__thiscall *)(snowboard_this*, tmd_primitive_packet*, int, int))get_relative_call(ff7_externals.snowboard_parse_model_vertices_732159, 0x162);
	ff7_externals.sub_732BB9 = (char* (*)(tmd_primitive_packet*, int, int))get_relative_call(ff7_externals.snowboard_parse_model_vertices_732159, 0x146);
	ff7_externals.sub_733479 = (matrix* (__thiscall *)(void*, const matrix*))get_relative_call(ff7_externals.snowboard_parse_model_vertices_732159, 0x5F);
	ff7_externals.sub_733564 = (point4d* (__thiscall *)(void*, vector3<float>*, point4d*))get_relative_call(ff7_externals.snowboard_parse_model_vertices_732159, 0xDC);
	ff7_externals.snowboard_global_object_off_926290 = (DWORD*)get_absolute_value(ff7_externals.snowboard_parse_model_vertices_732159, 0x55);
	// --------------------------------

	// Steam achievement
	uint32_t sub_434347 = get_relative_call(ff7_externals.battle_loop, 0x484);
	uint32_t* pointer_functions_7C2980 = (uint32_t*)get_absolute_value(sub_434347, 0x19C);
	ff7_externals.battle_enemy_killed_sub_433BD2 = pointer_functions_7C2980[0];
	ff7_externals.battle_sub_5C7F94 = get_relative_call(ff7_externals.battle_enemy_killed_sub_433BD2, 0x2AF);
	ff7_externals.menu_battle_end_mode = (uint16_t*)get_absolute_value(ff7_externals.menu_battle_end_sub_6C9543, 0x2C);
	uint32_t menu_sub_6CBD54 = get_relative_call(ff7_externals.menu_sub_6CDA83, 0xC1);
	ff7_externals.menu_sub_71FF95 = get_relative_call(menu_sub_6CBD54, 0x7);
	ff7_externals.menu_shop_loop = get_relative_call(ff7_externals.menu_sub_71FF95, 0x84);
	if (version == VERSION_FF7_102_US) {
		ff7_externals.get_materia_gil = get_relative_call(ff7_externals.menu_shop_loop, 0x548);
	} else {
		ff7_externals.get_materia_gil = get_relative_call(ff7_externals.menu_shop_loop, 0x5C4);
	}
	ff7_externals.opcode_increase_gil_call = get_relative_call(ff7_externals.opcode_goldu, 0x38);

	ff7_externals.display_battle_action_text_sub_6D71FA = get_relative_call(ff7_externals.display_battle_action_text_42782A, 0x77);

	ff7_externals.opcode_add_materia_inventory_call = get_relative_call(ff7_externals.opcode_smtra, 0x72);
	ff7_externals.menu_sub_6CBCF3 = get_relative_call(ff7_externals.opcode_add_materia_inventory_call, 0x43);
	ff7_externals.menu_sub_705D16 = ff7_externals.menu_subs_call_table[4];
	ff7_externals.menu_sub_6CC17F = get_relative_call(ff7_externals.menu_sub_705D16, 0x1729);

	ff7_externals.menu_decrease_item_quantity = get_relative_call(ff7_externals.opcode_dlitm, 0x38);

	ff7_externals.sub_60FA7D = get_relative_call(ff7_externals.opcode_setbyte, 0x14);

	uint32_t menu_sub_6CBD65 = get_relative_call(ff7_externals.menu_sub_6CDA83, 0x54);
	uint32_t menu_sub_722393 = get_relative_call(menu_sub_6CBD65, 0x4);
	ff7_externals.menu_sub_7212FB = get_relative_call(menu_sub_722393, 0x8B);
	switch(version) {
		case VERSION_FF7_102_US:
		case VERSION_FF7_102_SP:
			ff7_externals.load_save_file = get_relative_call(ff7_externals.menu_sub_7212FB, 0xE9D);
			break;
		case VERSION_FF7_102_DE:
		case VERSION_FF7_102_FR:
			ff7_externals.load_save_file = get_relative_call(ff7_externals.menu_sub_7212FB, 0xEC5);
			break;
	}

	// --------------------------------

	// japanese
	ff7_externals.engine_loop_main_loop_sub_4090E6 = get_absolute_value(ff7_externals.swirl_loop_sub_4026D4, 0x11E);
	ff7_externals.menu_enter_sub_6CD3B0 = get_absolute_value(ff7_externals.engine_loop_main_loop_sub_4090E6, 0x627);
	ff7_externals.engine_load_menu_graphics_objects_6C1468 = (void (*)(int))get_relative_call(ff7_externals.menu_enter_sub_6CD3B0, 0x2B);

	ff7_externals.sub_671082 = (void (*)(ff7_graphics_object**))get_relative_call((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x28);
	ff7_externals.sub_674530 = (int (*)())get_relative_call((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0xF5);
	ff7_externals.sub_67453A = (void (*)(int))get_relative_call((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0xFF);
	ff7_externals.make_struc3_6745E6 = (void (*)(int, struc_3*))get_relative_call((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x10D);
	ff7_externals.engine_set_blendmode_674659 = (void (*)(int, struc_3 *))get_relative_call((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x15B);
	ff7_externals.engine_load_graphics_object_6710AC = (ff7_graphics_object* (*)(int, signed int, struc_3 *, char *, int))get_relative_call((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x1A4);
	ff7_externals.engine_get_viewport_type_404D80 = (int (*)())get_relative_call((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x9);
	ff7_externals.engine_get_game_object_676578 = (ff7_game_obj* (*)())get_relative_call((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x11);

	ff7_externals.unk_DC1074 = (unsigned int*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x12D);
	ff7_externals.aUsfont_a_h_tim = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x16F);
	ff7_externals.aUsfont_a_l_tim = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x17B);
	ff7_externals.aUsfont_b_h_tim = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x1CB);
	ff7_externals.aUsfont_b_l_tim = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x1D7);
	ff7_externals.aBtl_win_a_h_ti = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x219);
	ff7_externals.aBtl_win_a_l_ti = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x225);
	ff7_externals.aBtl_win_b_h_ti = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x2DD);
	ff7_externals.aBtl_win_b_l_ti = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x2E9);	
	ff7_externals.aBtl_win_c_h_ti = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x36D);		
	ff7_externals.aBtl_win_c_l_ti = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x379);		
	ff7_externals.aBtl_win_d_h_ti = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x454);		
	ff7_externals.aBtl_win_d_l_ti = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x460);		
	ff7_externals.aUsfont_h_tim = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x4F7);			
	ff7_externals.aUsfont_l_tim = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x503);			
	ff7_externals.aBtl_win_h_tim = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x545);			
	ff7_externals.aBtl_win_l_tim = (char*)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x551);			
	
	ff7_externals.field_draw_everything_sub_63A60B = get_relative_call(ff7_externals.field_sub_6388EE, 0x11);
	ff7_externals.field_submit_and_draw_text_box_and_text_6EBF2C = get_relative_call(ff7_externals.field_draw_everything_sub_63A60B, 0x39A);
	ff7_externals.field_submit_draw_text_640x480_6E706D = get_relative_call(ff7_externals.field_submit_and_draw_text_box_and_text_6EBF2C, 0x363);

	ff7_externals.field_text_box_curr_n_characters_DC3CB0 = (int*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0xD1);
	ff7_externals.field_text_line_row_DC3CB8 = (int*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0xC4);
	ff7_externals.field_remaining_character_length_DC3CCC = (short*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x4F);	
	ff7_externals.field_current_window_pos_x_DC3CB4 = (int*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x99);	
	ff7_externals.word_91F028 = (short*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x232);	
	ff7_externals.word_DC3CC0 = (short*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x261);	
	ff7_externals.word_DC3CC4 = (short*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x28E);	
	ff7_externals.dword_DC3CD4 = (int*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x2B9);
	ff7_externals.word_DC3CC8 = (short*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x62D);	
	ff7_externals.g_text_spacing_DB958C = (int*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x717);	
	ff7_externals.menu_font_a_graphics_object_DC100C = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x7ED);
	ff7_externals.menu_font_b_graphics_object_DC1010 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x7AC);
	ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x323);
	ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x33A);
	ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x58);
	ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x65);
	ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x72);
	ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x7F);
	ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x8C);
	ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0x99);
	ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0xA6);
	ff7_externals.menu_font_blend_4_graphics_object_DC1048 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0xB5);
	ff7_externals.menu_win_blend_4_graphics_object_DC104C = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0xC2);
	ff7_externals.menu_win_blend_0_graphics_object_DC1050 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0xCF);
	ff7_externals.menu_win_blend_1_graphics_object_DC1054 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0xDC);
	ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, 0xE9);	

	ff7_externals.field_do_draw_character_DC3CEC = (int*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0xA60);
	ff7_externals.field_do_draw_text_boxes_DC3CE8 = (int*)get_absolute_value((uint32_t)ff7_externals.field_submit_draw_text_640x480_6E706D, 0x5AE);

	ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68 = (void (*)())get_relative_call(ff7_externals.field_submit_and_draw_text_box_and_text_6EBF2C, 0xBD); 
	ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95 = (void (*)(int, ff7_game_obj*))get_relative_call((uint32_t) ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x28); 
	ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A = (void (*)(int, int, ff7_game_obj *))get_relative_call((uint32_t) ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x38); 
	ff7_externals.engine_draw_graphics_object_66E641 = (void (*)(ff7_graphics_object*, ff7_game_obj*))get_relative_call((uint32_t) ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x4A); 
	ff7_externals.reset_field_54_graphics_object_66E62C = (void (*)(ff7_graphics_object*))get_relative_call((uint32_t) ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x59); 
	
	ff7_externals.menu_is_small_viewport_320_240_DC130C = (int*)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0xE);	
	ff7_externals.dword_DC3CE0 = (int*)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x29B);	
	ff7_externals.dword_DC3CDC = (int*)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x2CF);	
	ff7_externals.menu_window_bg_graphics_object_DC0FF0 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x54);
	ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x98);
	ff7_externals.text_box_do_draw_menu_win_c_blend_4_DC3CE4 = (int*)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x15E);	
	ff7_externals.text_box_do_draw_black_quad_graphics_object_DC3CF0 = (int*)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x226);	
	ff7_externals.menu_text_box_quad_graphics_object_DC1008 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x251);
	ff7_externals.dword_DC3D00 = (int*)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x26F);	
	ff7_externals.do_draw_text_box_DC3CF8 = (int*)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x283);	
	ff7_externals.should_draw_text_box_black_quad_DC3D04 = (int*)get_absolute_value((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, 0x28C);	
	
	ff7_externals.battle_menu_display_menu_6D82EA = get_relative_call(battle_sub_6D83C8, 0xE0);
	ff7_externals.battle_display_base_menu_6DD041 = get_relative_call(ff7_externals.display_battle_menu_6D797C, 0x29);
	ff7_externals.common_submit_draw_char_from_buffer_6F564E = (int (*)(int, int, int, unsigned __int16, float))get_relative_call(ff7_externals.battle_display_base_menu_6DD041, 0x382); 
	ff7_externals.menu_loop_sub_6CC623 = get_absolute_value(ff7_externals.engine_loop_main_loop_sub_4090E6, 0x62E);
	ff7_externals.menu_draw_everything_6CC9D3 = (void (*)())get_relative_call(ff7_externals.menu_loop_sub_6CC623, 0x117);
	
	ff7_externals.g_get_do_render_menu_6CDBF2 = (int (*)())get_relative_call((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x4);

	ff7_externals.dword_DC12DC = (int*)get_absolute_value((uint32_t)ff7_externals.common_submit_draw_char_from_buffer_6F564E, 0x1D5);

	ff7_externals.engine_gfx_draw_graphics_object_polygon_set_field_80_sub_660E6A = (void (*)(ff7_graphics_object*, ff7_game_obj*))get_relative_call((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x42);
	ff7_externals.engine_gfx_setviewport_sub_66067A = (void (*)(unsigned int, unsigned int, unsigned int, unsigned int, ff7_game_obj*))get_relative_call((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x381);

	ff7_externals.menu_unknown3_graphics_object_DC0FFC = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x3D);
	ff7_externals.dword_DC12EC = (int*)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x129);
	ff7_externals.dword_DC12E4 = (int*)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x132);
	ff7_externals.menu_avatar2_1_graphics_object_DC1020 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x142);
    ff7_externals.menu_avatar2_2_graphics_object_DC1024 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x155);
    ff7_externals.menu_avatar2_3_graphics_object_DC1028 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x168);
    ff7_externals.menu_avatar2_4_graphics_object_DC102C = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x17A);
    ff7_externals.menu_avatar2_5_graphics_object_DC1030 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x18D);
    ff7_externals.menu_avatar2_6_graphics_object_DC1034 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x1A0);
    ff7_externals.menu_avatar2_7_graphics_object_DC1038 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x1B2);
    ff7_externals.menu_avatar2_8_graphics_object_DC103C = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x1C5);
    ff7_externals.menu_avatar2_9_graphics_object_DC1040 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x1D8);
	ff7_externals.menu_avatar_1_graphics_object_DC1014 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x1EC);
    ff7_externals.menu_avatar_2_graphics_object_DC1018 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x1FF);
    ff7_externals.menu_avatar_3_graphics_object_DC101C = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x212);
	ff7_externals.engine_game_mode_word_CBF9DC = (short*)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x222);
	ff7_externals.menu_buster_tex_graphics_object_DC1044 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x241);
    ff7_externals.menu_unknown4_graphics_object_DC1000 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x280);
	ff7_externals.menu_unknown2_graphics_object_DC0FF8 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x2A5);
	ff7_externals.menu_viewport_x_DC105C = (unsigned int*)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x37C);
	ff7_externals.menu_viewport_y_DC1060 = (unsigned int*)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x376);
	ff7_externals.menu_viewport_width_DC1064 = (unsigned int*)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x36F);
	ff7_externals.menu_viewport_view_DC1068 = (unsigned int*)get_absolute_value((uint32_t)ff7_externals.menu_draw_everything_6CC9D3, 0x368);

	
	ff7_externals.battle_loop_sub_41BAB3 = get_absolute_value(ff7_externals.engine_loop_main_loop_sub_4090E6, 0x89A);
	ff7_externals.battle_draw_menu_everything_6CEE84 = (void (*)())get_relative_call(ff7_externals.battle_loop_sub_41BAB3, 0x378); 

	ff7_externals.menu_unknown5_graphics_object_DC1004 = (ff7_graphics_object**)get_absolute_value((uint32_t)ff7_externals.battle_draw_menu_everything_6CEE84, 0x2B9);

	ff7_externals.battle_display_text_6D7245 = get_relative_call(ff7_externals.battle_sub_42D808, 0xAD);
	ff7_externals.draw_text_top_display_6D1CC0 = (void (*)(int, __int16, char, unsigned __int16))get_relative_call(ff7_externals.battle_display_text_6D7245, 0x9A);
	ff7_externals.kernel_get_text_sub_41963C = (attack_name_fixed_buffer* (*)(unsigned int a1, int a2, int a3))get_relative_call((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x124);
	ff7_externals.sub_6D70F1 = (char (*)(int))get_relative_call((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x1F9);

	ff7_externals.battle_menu_data_DC3630 = (battle_menu_data**)get_absolute_value((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x35);
	ff7_externals.byte_DC3640 = (char*)get_absolute_value((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x204);
	ff7_externals.battle_text_buffer_DC208C = (attack_name_fixed_buffer**)get_absolute_value((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x8C);
	ff7_externals.battle_graphics_data_ptr_9ADFD8 = (battle_graphics_data**)get_absolute_value((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x11A9);
	ff7_externals.g_is_battle_paused_DC0E6C = (char*)get_absolute_value((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x119C);
	ff7_externals.dword_91EFC8 = (bgra_byte*)get_absolute_value((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x3239);
	ff7_externals.dword_91EFCC = (bgra_byte*)get_absolute_value((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x32C1);
	ff7_externals.dword_91EFD0 = (bgra_byte*)get_absolute_value((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x3349);
	ff7_externals.dword_91EFD4 = (bgra_byte*)get_absolute_value((uint32_t)ff7_externals.draw_text_top_display_6D1CC0, 0x33E4);

	ff7_externals.engine_field_A58_sub_6C0E2D = get_absolute_value(ff7_externals.init_stuff, 0x391);
	ff7_externals.main_menu_draw_everything_maybe_6C0B91 = (void (*)())get_relative_call(ff7_externals.engine_field_A58_sub_6C0E2D, 0x9A);

	ff7_externals.text_box_window_data_array_CFF5B8 = (text_box_data*)get_absolute_value((uint32_t)ff7_externals.field_text_box_window_create_631586, 0x135);
	ff7_externals.field_text_box_window_entity_id_CC0960 = (byte*)get_absolute_value((uint32_t)ff7_externals.field_text_box_window_opening_6317A9, 0xB);
	ff7_externals.current_entity_id_byte_CC0964 = (byte*)get_absolute_value((uint32_t)ff7_externals.field_text_box_window_opening_6317A9, 0x13);

	ff7_externals.sub_6F54A2 = get_relative_call(ff7_externals.menu_sub_7212FB, 0x14F);
}

inline void ff7_data(struct ff7_game_obj* game_object)
{
	num_modes = sizeof(ff7_modes) / sizeof(ff7_modes[0]);

	ff7_find_externals(game_object);

	memcpy(modes, ff7_modes, sizeof(ff7_modes));

	text_colors[TEXTCOLOR_GRAY] = 0x08;
	text_colors[TEXTCOLOR_BLUE] = 0x01;
	text_colors[TEXTCOLOR_RED] = 0x04;
	text_colors[TEXTCOLOR_PINK] = 0x05;
	text_colors[TEXTCOLOR_GREEN] = 0x02;
	text_colors[TEXTCOLOR_LIGHT_BLUE] = 0x09;
	text_colors[TEXTCOLOR_YELLOW] = 0x0E;
	text_colors[TEXTCOLOR_WHITE] = 0x0F;
}
