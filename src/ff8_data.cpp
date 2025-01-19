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

#include <stdio.h>

#include "ff8_data.h"

#include "globals.h"
#include "patch.h"
#include "ff8/battle/effects.h"

void ff8_set_main_loop(uint32_t driver_mode, uint32_t main_loop)
{
	uint32_t i;

	for(i = 0; i < num_modes; i++) if(ff8_modes[i].driver_mode == driver_mode) ff8_modes[i].main_loop = main_loop;
}

void ff8_find_externals()
{
	common_externals.winmain = get_relative_call(ff8_externals.start, 0xDB);
	common_externals.create_window = get_relative_call(common_externals.winmain, 0x114);
	common_externals.engine_wndproc = (WNDPROC)get_absolute_value(common_externals.create_window, 0x34);

	ff8_externals.main_entry = get_relative_call(common_externals.winmain, 0x4D);

	if (JP_VERSION)
	{
		ff8_externals.main_entry = get_relative_call(ff8_externals.main_entry, 0x0);
	}

	ff8_externals.archive_path_prefix = (char *)get_absolute_value(ff8_externals.main_entry, 0x2E);
	ff8_externals.manage_time_engine_sub_569971 = get_relative_call(common_externals.winmain, 0x23);
	ff8_externals.enable_rdtsc_sub_40AA00 = (int (*)(int))get_relative_call(ff8_externals.manage_time_engine_sub_569971, 0x21);
	common_externals.get_time = (uint64_t (*)(uint64_t*))get_relative_call(common_externals.winmain, 0x20E);
	common_externals.diff_time = (uint64_t (*)(uint64_t*,uint64_t*,uint64_t*))get_relative_call(common_externals.winmain, 0x41E);
	ff8_externals.init_config = get_relative_call(ff8_externals.main_entry, 0x73);
	ff8_externals.pubintro_init = get_absolute_value(ff8_externals.main_entry, 0x158);
	ff8_externals.pubintro_cleanup = get_absolute_value(ff8_externals.main_entry, 0x162);

	if (JP_VERSION)
	{
		ff8_externals.init_config = get_relative_call(ff8_externals.init_config, 0x0);
		ff8_externals.pubintro_init = get_relative_call(ff8_externals.pubintro_init, 0x0);
		ff8_externals.pubintro_cleanup = get_relative_call(ff8_externals.pubintro_cleanup, 0x0);
	}

	ff8_externals.sub_467C00 = get_relative_call(ff8_externals.pubintro_init, 0xB5);
	ff8_externals.input_init = get_relative_call(ff8_externals.pubintro_init, 0xBA);
	ff8_externals.ff8input_cfg_read = get_relative_call(ff8_externals.input_init, 0x5);
	ff8_externals.sub_468810 = get_relative_call(ff8_externals.sub_467C00, 0x59);
	ff8_externals.dinput_get_input_device_capabilities_number_of_buttons = get_relative_call(ff8_externals.sub_467C00, 0xA8);
	ff8_externals.sub_468BD0 = get_relative_call(ff8_externals.sub_468810, 0x5B);
	common_externals.dinput_hack1 = ff8_externals.sub_468BD0 + 0x64;

	ff8_externals.pubintro_exit = get_absolute_value(ff8_externals.main_entry, 0x176);
	ff8_externals.pubintro_main_loop = get_absolute_value(ff8_externals.main_entry, 0x180);
	ff8_externals.credits_main_loop = get_absolute_value(ff8_externals.pubintro_main_loop, 0x6D);
	ff8_externals.go_to_main_menu_main_loop = get_absolute_value(ff8_externals.credits_main_loop, 0xE2);
	ff8_externals.main_menu_main_loop = get_absolute_value(ff8_externals.go_to_main_menu_main_loop, 0x2B);

	ff8_set_main_loop(MODE_CREDITS, ff8_externals.credits_main_loop);
	ff8_set_main_loop(MODE_MAIN_MENU, ff8_externals.main_menu_main_loop);

	ff8_externals.config_worldmap_fog_disabled = (uint8_t *)get_absolute_value(ff8_externals.main_entry, 0x1C1);
	ff8_externals.config_worldmap_color_anim_disabled = (uint8_t *)get_absolute_value(ff8_externals.main_entry, 0x1C7);
	ff8_externals.config_worldmap_textured_anim_disabled = (uint8_t *)get_absolute_value(ff8_externals.main_entry, 0x1CC);

	ff8_externals.load_credits_image = get_relative_call(ff8_externals.credits_main_loop, 0xBF);

	ff8_externals.sub_52FE80 = get_relative_call(ff8_externals.load_credits_image, 0xA4);
	ff8_externals.input_fill_keystate = (void(*)())get_relative_call(ff8_externals.sub_52FE80, 0xC8);
	ff8_externals.input_get_keyscan = (int(*)(int,int))get_relative_call(ff8_externals.sub_52FE80, 0xD1);
	ff8_externals.credits_loop_state = (DWORD*)get_absolute_value(ff8_externals.load_credits_image, 0x7);
	ff8_externals.credits_counter = (DWORD *)get_absolute_value(ff8_externals.load_credits_image, 0x59);
	ff8_externals.credits_current_image_global_counter_start = (DWORD *)get_absolute_value(ff8_externals.load_credits_image, 0x1CB);
	ff8_externals.credits_current_step_image = (DWORD *)get_absolute_value(ff8_externals.load_credits_image, 0x1BC);
	ff8_externals.sub_470630 = get_absolute_value(ff8_externals.main_menu_main_loop, 0xE4);
	ff8_externals.main_loop = get_absolute_value(ff8_externals.sub_470630, 0x24);

	ff8_externals.reg_get_data_drive = (uint32_t(*)(char*, DWORD))get_relative_call(ff8_externals.init_config, 0x21);
	ff8_externals.get_disk_number = get_relative_call(ff8_externals.main_loop, 0x1A);
	ff8_externals.disk_data_path = (char*)get_absolute_value(ff8_externals.get_disk_number, 0xF);
	ff8_externals.set_game_paths = (void (*)(int, char*, const char*))get_relative_call(ff8_externals.init_config, 0x3E);
	if (JP_VERSION)
	{
		ff8_externals.set_game_paths = (void (*)(int, char*, const char*))get_relative_call(uint32_t(ff8_externals.set_game_paths), 0x0);
	}
	ff8_externals.app_path = (const char*)get_absolute_value(uint32_t(ff8_externals.set_game_paths), 0x9A);

	ff8_externals.savemap = (uint32_t**)get_absolute_value(ff8_externals.main_loop, 0x21);

	if (JP_VERSION)
	{
		ff8_externals.sm_pc_read = (uint32_t(*)(char*, void*))get_relative_call(ff8_externals.main_loop, 0x9C + 3);
		ff8_externals.cdcheck_main_loop = get_absolute_value(ff8_externals.main_loop, 0xBB + 3);
		common_externals._mode = (WORD *)get_absolute_value(ff8_externals.main_loop, 0x115 + 3);
		ff8_externals.field_main_exit = get_absolute_value(ff8_externals.main_loop, 0x13C + 3);
		ff8_externals.field_main_loop = get_absolute_value(ff8_externals.main_loop, 0x144 + 3);
		common_externals.current_field_id = (WORD*)get_absolute_value(ff8_externals.main_loop, 0x21F + 6);
		ff8_externals.worldmap_enter_main = get_absolute_value(ff8_externals.main_loop, 0x2C0 + 4);
		ff8_externals.worldmap_main_loop = get_absolute_value(ff8_externals.main_loop, 0x2D0 + 4);
		ff8_externals.battle_enter = get_absolute_value(ff8_externals.main_loop, 0x330 + 4);
		ff8_externals.battle_main_loop = get_absolute_value(ff8_externals.main_loop, 0x340 + 4);
		// Search battle sound function to find play/stop midi related methods
		ff8_externals.sm_battle_sound = get_relative_call(ff8_externals.main_loop, 0x487 + 5);
		ff8_externals.swirl_enter = get_absolute_value(ff8_externals.main_loop, 0x493 + 5);
		ff8_externals.swirl_main_loop = get_absolute_value(ff8_externals.main_loop, 0x4A3 + 5);
		ff8_externals.sub_470250 = get_relative_call(ff8_externals.main_loop, 0x6E7 - 15);
	}
	else
	{
		ff8_externals.sm_pc_read = (uint32_t(*)(char*, void*))get_relative_call(ff8_externals.main_loop, 0x9C);
		ff8_externals.cdcheck_main_loop = get_absolute_value(ff8_externals.main_loop, 0xBB);
		common_externals._mode = (WORD *)get_absolute_value(ff8_externals.main_loop, 0x115);
		ff8_externals.field_main_exit = get_absolute_value(ff8_externals.main_loop, 0x13C);
		ff8_externals.field_main_loop = get_absolute_value(ff8_externals.main_loop, 0x144);
		common_externals.current_field_id = (WORD*)get_absolute_value(ff8_externals.main_loop, 0x21F);
		ff8_externals.worldmap_enter_main = get_absolute_value(ff8_externals.main_loop, 0x2C0);
		ff8_externals.worldmap_main_loop = get_absolute_value(ff8_externals.main_loop, 0x2D0);
		ff8_externals.battle_enter = get_absolute_value(ff8_externals.main_loop, 0x330);
		ff8_externals.battle_main_loop = get_absolute_value(ff8_externals.main_loop, 0x340);
		// Search battle sound function to find play/stop midi related methods
		ff8_externals.sm_battle_sound = get_relative_call(ff8_externals.main_loop, 0x487);
		ff8_externals.swirl_enter = get_absolute_value(ff8_externals.main_loop, 0x493);
		ff8_externals.swirl_main_loop = get_absolute_value(ff8_externals.main_loop, 0x4A3);
		ff8_externals.sub_470250 = get_relative_call(ff8_externals.main_loop, 0x6E7);
	}

	ff8_externals.psxvram_texture_pages_free = get_relative_call(ff8_externals.field_main_exit, 0x58);
	ff8_externals.sub_4672C0 = get_relative_call(ff8_externals.psxvram_texture_pages_free, 0x5A);
	ff8_externals.psxvram_texture_page_free = get_relative_call(ff8_externals.psxvram_texture_pages_free, 0x21);
	ff8_externals.psxvram_texture_page_tex_header_free = get_relative_call(ff8_externals.psxvram_texture_page_free, 0x98);
	ff8_externals.engine_set_init_time = get_relative_call(ff8_externals.battle_enter, 0x35);
	ff8_externals.sub_460B60 = get_relative_call(ff8_externals.swirl_enter, 0x9);

	common_externals.debug_print2 = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0x16);
	ff8_externals.moriya_filesystem_open = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0x21);
	ff8_externals.moriya_filesystem_seek = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0x77);
	ff8_externals.moriya_filesystem_read = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0xB7);
	ff8_externals.moriya_filesystem_close = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0xDD);
	ff8_externals.read_or_uncompress_fs_data = get_relative_call(ff8_externals.moriya_filesystem_read, 0x5C);
	ff8_externals.lzs_uncompress = get_relative_call(ff8_externals.read_or_uncompress_fs_data, 0x1E6);
	ff8_externals.free_file_container = (void(*)(ff8_file_container*))get_relative_call(ff8_externals.moriya_filesystem_close, 0x1F);

	ff8_externals.cdcheck_sub_52F9E0 = get_relative_call(ff8_externals.cdcheck_main_loop, 0x95);

	ff8_set_main_loop(MODE_CDCHECK, ff8_externals.cdcheck_main_loop);
	ff8_set_main_loop(MODE_SWIRL, ff8_externals.swirl_main_loop);
	ff8_set_main_loop(MODE_BATTLE, ff8_externals.battle_main_loop);
	ff8_set_main_loop(MODE_FIELD, ff8_externals.field_main_loop);
	ff8_set_main_loop(MODE_WORLDMAP, ff8_externals.worldmap_main_loop);

	ff8_externals.sub_47CCB0 = get_relative_call(ff8_externals.battle_main_loop, 0x1B3);
	ff8_externals.sub_534640 = get_relative_call(ff8_externals.sub_47CCB0, 0xF1);
	ff8_externals.sub_4972A0 = get_relative_call(ff8_externals.sub_534640, 0x51);
	ff8_externals.load_fonts = get_relative_call(ff8_externals.sub_4972A0, 0x16);

	ff8_externals.sub_537F30 = get_relative_call(ff8_externals.sub_534640, 0x2E);
	ff8_externals.sub_5391B0 = get_relative_call(ff8_externals.sub_537F30, 0x58);
	ff8_externals.sub_534560 = get_relative_call(ff8_externals.sub_534640, 0x5B);
	ff8_externals.cardgame_funcs = (uint32_t *)get_absolute_value(ff8_externals.sub_534560, 0x5D);
	ff8_externals.sub_536C30 = ff8_externals.cardgame_funcs[1];
	ff8_externals.sub_535640 = ff8_externals.cardgame_funcs[3];
	ff8_externals.sub_536CB0 = get_absolute_value(ff8_externals.sub_536C30, 0x14);
	ff8_externals.card_texts_off_B96968 = (uint8_t **)get_absolute_value(ff8_externals.sub_536CB0, 0x59);
	ff8_externals.sub_536C80 = get_absolute_value(ff8_externals.sub_536C30, 0x25);
	ff8_externals.sub_5366D0 = get_absolute_value(ff8_externals.sub_535640, 0x42);
	ff8_externals.cardgame_tim_texture_intro = (uint8_t *)get_absolute_value(ff8_externals.sub_536C80, 0x3);
	ff8_externals.cardgame_tim_texture_game = (uint8_t *)get_absolute_value(ff8_externals.sub_5366D0, 0x119);
	ff8_externals.cardgame_tim_texture_cards = (uint8_t *)get_absolute_value(ff8_externals.sub_534640, 0x11B);
	ff8_externals.cardgame_tim_texture_icons = (uint8_t *)get_absolute_value(ff8_externals.sub_534640, 0x125);
	ff8_externals.sub_539500 = get_relative_call(ff8_externals.sub_534640, 0x110);
	ff8_externals.cardgame_tim_texture_font = (uint8_t *)get_absolute_value(ff8_externals.sub_539500, 0x1);
	ff8_externals.is_card_game = (uint32_t*)get_absolute_value(ff8_externals.sub_47CCB0, *(uint32_t *)(ff8_externals.sub_47CCB0 + 0xF2) + 0xF7);

	ff8_externals.loc_47D490 = ff8_externals.sub_47CCB0 + 0xDA + 0x4 + *((int32_t *)(ff8_externals.sub_47CCB0 + 0xDA));
	ff8_externals.sub_500870 = get_relative_call(ff8_externals.loc_47D490, 0x85);
	ff8_externals.sub_500C00 = get_relative_call(ff8_externals.sub_500870, 0x31);
	ff8_externals.sub_500CC0 = get_absolute_value(ff8_externals.sub_500C00, 0x9A);
	ff8_externals.sub_506C90 = get_relative_call(ff8_externals.sub_500CC0, 0x7F);
	ff8_externals.sub_506CF0 = get_absolute_value(ff8_externals.sub_506C90, 0x2F);
	ff8_externals.sub_5084B0 = get_relative_call(ff8_externals.sub_506CF0, 0x2A);
	ff8_externals.battle_open_file_wrapper = get_relative_call(ff8_externals.sub_5084B0, 0x1B);
	ff8_externals.battle_open_file = get_relative_call(ff8_externals.battle_open_file_wrapper, 0x14);
	ff8_externals.battle_filenames = (char **)get_absolute_value(ff8_externals.battle_open_file, 0x11);

	ff8_externals.sub_47D890 = get_relative_call(ff8_externals.sub_506CF0, 0x59);
	ff8_externals.sub_505DF0 = get_relative_call(ff8_externals.sub_506CF0, 0xAA);
	ff8_externals.sub_4A94D0 = get_relative_call(ff8_externals.sub_47D890, 0x9);
	ff8_externals.sub_4BCBE0 = get_relative_call(ff8_externals.sub_4A94D0, 0x1E0);
	ff8_externals.sub_4C8B10 = get_relative_call(ff8_externals.sub_4BCBE0, 0x8E);
	ff8_externals.battle_pause_sub_4CD140 = get_absolute_value(ff8_externals.sub_4C8B10, 0x3);
	ff8_externals.battle_pause_window_sub_4CD350 = get_relative_call(ff8_externals.battle_pause_sub_4CD140, JP_VERSION ? 0x1F1 : 0x225);
	ff8_externals.is_alternative_pause_menu = (uint32_t *)get_absolute_value(ff8_externals.battle_pause_window_sub_4CD350, 0x6B);
	ff8_externals.pause_menu_option_state = (uint32_t *)get_absolute_value(ff8_externals.battle_pause_window_sub_4CD350, 0x9C);
	ff8_externals.battle_menu_state = (void *)get_absolute_value(ff8_externals.battle_pause_window_sub_4CD350, 0x29);
	ff8_externals.sub_4A7210 = get_relative_call(ff8_externals.battle_pause_window_sub_4CD350, 0xC3);

	ff8_externals.battle_load_textures_sub_500900 = get_relative_call(ff8_externals.sub_47CCB0, 0x98D);
	ff8_externals.loc_5005A0 = ff8_externals.battle_load_textures_sub_500900 + 0x9D + 0x4 + *((int32_t *)(ff8_externals.battle_load_textures_sub_500900 + 0x9D));
	ff8_externals.battle_upload_texture_to_vram = get_relative_call(ff8_externals.loc_5005A0, 0xD1);
	ff8_externals.copy_psx_vram_part = get_relative_call(ff8_externals.battle_upload_texture_to_vram, 0x8D);

	ff8_externals.fonts = (font_object **)get_absolute_value(ff8_externals.load_fonts, JP_VERSION ? 0x17 : 0x16);

	common_externals.assert_malloc = (void* (*)(uint32_t, const char*, uint32_t))get_relative_call(ff8_externals.load_fonts, JP_VERSION ? 0x29 : 0x2A);

	ff8_externals.sub_471F70 = get_relative_call(ff8_externals.field_main_loop, 0x148);
	ff8_externals.field_fade_transition_sub_472990 = get_relative_call(ff8_externals.field_main_loop, 0x19E);
	ff8_externals.sub_45CDD0 = get_relative_call(ff8_externals.field_fade_transition_sub_472990, 0x5C);

	if (JP_VERSION)
	{
		ff8_externals.sub_4767B0 = get_relative_call(ff8_externals.sub_471F70, 0x4FE - 2);
		common_externals.update_field_entities = get_relative_call(ff8_externals.sub_4767B0, 0x14E + 1);
		ff8_externals.sub_4789A0 = get_relative_call(ff8_externals.sub_4767B0, 0x40F + 3);
		ff8_externals.stop_cdrom = (uint32_t(*)())get_relative_call(ff8_externals.sub_4767B0, 0xB46 - 13);
		ff8_externals.stop_cdrom_field_call = ff8_externals.sub_4767B0 + 0xB46 - 0xD;
		ff8_externals.sub_47CA90 = (char (*)())get_relative_call(ff8_externals.sub_4789A0, 0x674);
	}
	else
	{
		ff8_externals.sub_4767B0 = get_relative_call(ff8_externals.sub_471F70, 0x4FE);
		common_externals.update_field_entities = get_relative_call(ff8_externals.sub_4767B0, 0x14E);
		ff8_externals.sub_4789A0 = get_relative_call(ff8_externals.sub_4767B0, 0x40F);
		ff8_externals.stop_cdrom = (uint32_t(*)())get_relative_call(ff8_externals.sub_4767B0, 0xB46);
		ff8_externals.stop_cdrom_field_call = ff8_externals.sub_4767B0 + 0xB46;
		ff8_externals.sub_47CA90 = (char (*)())get_relative_call(ff8_externals.sub_4789A0, 0x68B);
	}

	ff8_externals.battle_trigger_field = uint32_t(ff8_externals.sub_47CA90) + 0x15;
	ff8_externals.check_game_is_paused = (int32_t(*)(int32_t))get_relative_call(ff8_externals.field_main_loop, 0x16C);
	ff8_externals.is_game_paused = (DWORD*)get_absolute_value((uint32_t)ff8_externals.check_game_is_paused, 0x78);
	ff8_externals.pause_menu = (int(*)(int))get_relative_call(uint32_t(ff8_externals.check_game_is_paused), 0x88);
	ff8_externals.init_pause_menu = get_relative_call(uint32_t(ff8_externals.check_game_is_paused), 0xE2);
	ff8_externals.pause_menu_with_vibration = (int(*)(int))(ff8_externals.init_pause_menu - 0x290);
	ff8_externals.ff8_draw_icon_or_key1 = get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0x1CF);
	ff8_externals.get_icon_sp1_data = get_relative_call(ff8_externals.ff8_draw_icon_or_key1, 0x2);
	ff8_externals.draw_controller_or_keyboard_icons = get_relative_call(ff8_externals.ff8_draw_icon_or_key1, 0x40);
	ff8_externals.get_command_key = get_relative_call(ff8_externals.draw_controller_or_keyboard_icons, 0x31);
	ff8_externals.sub_49BB30 = get_relative_call(ff8_externals.ff8_draw_icon_or_key1, 0xF6);
	ff8_externals.vibration_apply = get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0xB4);
	ff8_externals.vibration_set_is_enabled = get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0xF3);
	ff8_externals.vibration_get_is_enabled = get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0xA9);
	ff8_externals.get_keyon = (int(*)(int, int))get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0xC9);
	ff8_externals.get_vibration_capability = get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0xE3);
	ff8_externals.vibrate_data_main = (uint8_t **)get_absolute_value(uint32_t(ff8_externals.pause_menu_with_vibration), 0x261);
	ff8_externals.set_vibration = get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0x26A);
	ff8_externals.gamepad_states = (ff8_gamepad_state *)(get_absolute_value(ff8_externals.get_vibration_capability, 0xE + 3) - 0xB);
	ff8_externals.vibration_objects = (ff8_vibrate_struc *)get_absolute_value(ff8_externals.set_vibration, 0x2 + 1);
	ff8_externals.vibration_clear_intensity = get_relative_call(ff8_externals.sub_471F70, 0x276);
	ff8_externals.open_battle_vibrate_vib = get_relative_call(ff8_externals.pubintro_exit, 0x18);
	ff8_externals.vibrate_data_battle = (uint8_t **)get_absolute_value(ff8_externals.open_battle_vibrate_vib, 0x6);

	common_externals.get_movie_frame = get_relative_call(common_externals.update_field_entities, 0x26);

	common_externals.execute_opcode_table = (uint32_t*)get_absolute_value(common_externals.update_field_entities, 0x65A);
	ff8_externals.opcode_pshm_w = common_externals.execute_opcode_table[0x0C];
	ff8_externals.opcode_effectplay2 = common_externals.execute_opcode_table[0x21];
	ff8_externals.opcode_mapjump = common_externals.execute_opcode_table[0x29];
	ff8_externals.opcode_mes = common_externals.execute_opcode_table[0x47];
	ff8_externals.opcode_messync = common_externals.execute_opcode_table[0x48];
	ff8_externals.opcode_ask = common_externals.execute_opcode_table[0x4A];
	ff8_externals.opcode_winclose = common_externals.execute_opcode_table[0x4C];
	ff8_externals.opcode_movie = common_externals.execute_opcode_table[0x4F];
	ff8_externals.opcode_moviesync = common_externals.execute_opcode_table[0x50];
	ff8_externals.opcode_spuready = common_externals.execute_opcode_table[0x56];
	ff8_externals.opcode_amesw = common_externals.execute_opcode_table[0x64];
	ff8_externals.opcode_ames = common_externals.execute_opcode_table[0x65];
	ff8_externals.opcode_battle = common_externals.execute_opcode_table[0x69];
	ff8_externals.opcode_aask = common_externals.execute_opcode_table[0x6F];
	ff8_externals.opcode_setvibrate = common_externals.execute_opcode_table[0xA1];
	ff8_externals.opcode_movieready = common_externals.execute_opcode_table[0xA3];
	ff8_externals.opcode_musicload = common_externals.execute_opcode_table[0xB5];
	ff8_externals.opcode_crossmusic = common_externals.execute_opcode_table[0xBA];
	ff8_externals.opcode_dualmusic = common_externals.execute_opcode_table[0xBB];
	ff8_externals.opcode_musicvoltrans = common_externals.execute_opcode_table[0xC1];
	ff8_externals.opcode_musicvolfade = common_externals.execute_opcode_table[0xC2];
	ff8_externals.opcode_mesmode = common_externals.execute_opcode_table[0x106];
	ff8_externals.opcode_ramesw = common_externals.execute_opcode_table[0x116];
	ff8_externals.opcode_choicemusic = common_externals.execute_opcode_table[0x135];
	ff8_externals.opcode_drawpoint = common_externals.execute_opcode_table[0x137];
	ff8_externals.opcode_musicskip = common_externals.execute_opcode_table[0x144];
	ff8_externals.opcode_musicvolsync = common_externals.execute_opcode_table[0x149];
	ff8_externals.opcode_getmusicoffset = common_externals.execute_opcode_table[0x16F];
	ff8_externals.opcode_tuto = common_externals.execute_opcode_table[0x177];

	ff8_externals.vibrate_data_field = (uint8_t*)get_absolute_value(ff8_externals.opcode_setvibrate, 0x27);
	ff8_externals.current_tutorial_id = (BYTE*)get_absolute_value(ff8_externals.opcode_tuto, 0x2A);

	common_externals.cross_fade_midi = get_relative_call(ff8_externals.opcode_crossmusic, 0x5C);
	ff8_externals.music_load = get_relative_call(ff8_externals.opcode_musicload, 0x8C);

	common_externals.prepare_movie = get_relative_call(ff8_externals.opcode_movieready, 0x99);
	common_externals.release_movie_objects = get_relative_call(common_externals.prepare_movie, 0x19E);
	common_externals.start_movie = get_relative_call(ff8_externals.opcode_movie, 0xC3);
	common_externals.update_movie_sample = get_relative_call(common_externals.start_movie, 0x74);
	ff8_externals.draw_movie_frame = get_relative_call(ff8_externals.opcode_moviesync, 0x1C);
	common_externals.stop_movie = get_relative_call(common_externals.update_movie_sample, 0x3E2);
	ff8_externals.movie_object = (ff8_movie_obj *)get_absolute_value(common_externals.prepare_movie, 0xDB);

	ff8_externals.drawpoint_messages = get_absolute_value(ff8_externals.opcode_drawpoint, 0xD6);

	common_externals.debug_print = get_relative_call(common_externals.update_movie_sample, 0x141);

	ff8_externals._load_texture = get_relative_call(ff8_externals.load_fonts, JP_VERSION ? 0x31B : 0x197);
	ff8_externals.sub_4076B6 = get_relative_call(ff8_externals._load_texture, 0x16D);
	ff8_externals.sub_41AC34 = get_relative_call(ff8_externals.sub_4076B6, 0x46);
	ff8_externals.load_texture_data = get_relative_call(ff8_externals.sub_41AC34, 0x168);
	common_externals.load_tex_file = get_relative_call(ff8_externals.load_texture_data, 0x103);
	common_externals.create_tex_header = (tex_header* (*)())get_relative_call(common_externals.load_tex_file, 0xD);
	common_externals.assert_calloc = (void* (*)(uint32_t, uint32_t, const char*, uint32_t))get_relative_call((uint32_t)common_externals.create_tex_header, 0x15);
	common_externals.open_file = get_relative_call(common_externals.load_tex_file, 0x27);
	common_externals.read_file = get_relative_call(common_externals.load_tex_file, 0x49);
	common_externals.alloc_read_file = (void* (*)(uint32_t, uint32_t, struct file *))get_relative_call(common_externals.load_tex_file, 0xB3);
	common_externals.close_file = get_relative_call(common_externals.load_tex_file, 0x15B);
	common_externals.destroy_tex = (void (*)(tex_header*))get_relative_call(common_externals.load_tex_file, 0x16D);
	common_externals.destroy_tex_header = get_relative_call((uint32_t)common_externals.destroy_tex, 0x78);
	common_externals.assert_free = (void* (*)(void*, const char*, uint32_t))get_relative_call(common_externals.destroy_tex_header, 0x21);
	common_externals.get_game_object = (game_obj* (*)())get_relative_call((uint32_t)common_externals.destroy_tex, 0x6);

	ff8_externals.fs_archive_search_filename = (int(*)(const char *, ff8_file_fi_infos *, const ff8_file_container *))get_relative_call(common_externals.open_file, 0x28D);
	ff8_externals.ff8_fs_archive_search_filename2 = (int(*)(const char *, ff8_file_fi_infos *, const ff8_file_container *))get_relative_call(uint32_t(ff8_externals.fs_archive_search_filename), 0x10);
	ff8_externals.fs_archive_get_fl_filepath = (char *(*)(int, const ff8_file_fl *))get_relative_call(uint32_t(ff8_externals.ff8_fs_archive_search_filename2), 0x40);
	ff8_externals._open = get_relative_call(common_externals.open_file, 0x2CE);
	ff8_externals._sopen = (int(*)(const char*, int, int, ...))get_relative_call(ff8_externals._open, 0xE);
	ff8_externals.fopen = get_relative_call(ff8_externals.ff8input_cfg_read, 0x33);
	ff8_externals._fsopen = (FILE *(*)(const char*, const char*, int))get_relative_call(ff8_externals.fopen, 0xA);
	ff8_externals.strcpy_with_malloc = (char*(*)(const char*))get_relative_call(common_externals.open_file, 0x2F2);

	ff8_externals.dd_d3d_start = get_relative_call(ff8_externals.pubintro_init, 0x75);
	ff8_externals.create_d3d_gfx_driver = get_relative_call(ff8_externals.dd_d3d_start, 0x88);
	ff8_externals.d3d_init = get_absolute_value(ff8_externals.create_d3d_gfx_driver, 0x1B);
	ff8_externals.sub_40BFEB = get_absolute_value(ff8_externals.d3d_init, 0x1370);
	common_externals.create_texture_format = (struct texture_format* (*)())get_relative_call(ff8_externals.sub_40BFEB, 0x2B);

	ff8_externals.tim2tex = get_relative_call(ff8_externals.sub_41AC34, 0xFC);
	ff8_externals.sub_41BC76 = get_relative_call(ff8_externals.tim2tex, 0x72);
	common_externals.make_pixelformat = (void (*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, struct texture_format*))get_relative_call(ff8_externals.sub_41BC76, 0x102);

	common_externals.add_texture_format = (void (*)(struct texture_format*, game_obj*))get_relative_call(ff8_externals.sub_40BFEB, 0xBF);

	ff8_externals.d3d_load_texture = get_absolute_value(ff8_externals.create_d3d_gfx_driver, 0x9D);
	common_externals.create_texture_set = (texture_set* (*)())get_relative_call(ff8_externals.d3d_load_texture, 0x6B);

	common_externals.create_palette_for_tex = (palette* (*)(uint32_t, tex_header*, texture_set*))get_relative_call(ff8_externals.d3d_load_texture, 0x316);

	ff8_externals.movie_hack1 = common_externals.update_movie_sample + 0xA5;
	ff8_externals.movie_hack2 = common_externals.update_movie_sample + 0x1DF;

	ff8_externals.sub_559910 = get_relative_call(ff8_externals.swirl_main_loop, 0x1A);

	ff8_externals.swirl_sub_56D1D0 = get_relative_call(ff8_externals.battle_main_loop, 0x285);
	ff8_externals.swirl_sub_56D390 = get_relative_call(ff8_externals.swirl_sub_56D1D0, 0x2A);
	ff8_externals.swirl_texture1 = (ff8_graphics_object **)get_absolute_value(ff8_externals.swirl_sub_56D1D0, 0x1);

	ff8_externals.sub_45D610 = get_relative_call(ff8_externals.sub_52FE80, 0x90);
	ff8_externals.sub_45D080 = get_relative_call(ff8_externals.sub_45D610, 0x5);
	ff8_externals.sub_464BD0 = get_relative_call(ff8_externals.sub_45D080, 0x208);
	ff8_externals.sub_4653B0 = get_relative_call(ff8_externals.sub_464BD0, 0x79);
	ff8_externals.sub_465720 = get_relative_call(ff8_externals.sub_464BD0, 0xAF);

	ff8_externals.ssigpu_callbacks_1 = (uint32_t *)get_absolute_value(ff8_externals.sub_45D080, 0x21E);
	ff8_externals.ssigpu_callbacks_2 = (uint32_t *)get_absolute_value(ff8_externals.sub_45D080, 0x122);
	ff8_externals.sub_461E00 = ff8_externals.ssigpu_callbacks_1[52];
	ff8_externals.sub_461220 = get_relative_call(ff8_externals.sub_461E00, 0x50);
	ff8_externals.dword_1CA8848 = get_absolute_value(ff8_externals.sub_461E00, 0x56);
	ff8_externals.sub_462AD0 = get_relative_call(ff8_externals.ssigpu_callbacks_1[116], 0x13);
	ff8_externals.sub_462DF0 = get_relative_call(ff8_externals.sub_462AD0, 0x62);
	ff8_externals.ssigpu_tx_select_2_sub_465CE0 = get_relative_call(ff8_externals.sub_462DF0, 0x33);
	ff8_externals.sub_464F70 = (int(*)(struc_50*,texture_page*,int,int,int,int,int,int,int,uint8_t*))get_relative_call(ff8_externals.ssigpu_tx_select_2_sub_465CE0, 0x281);
	ff8_externals.read_vram_1 = (void(*)(uint8_t*,int,uint8_t*,int,signed int,int,int))get_relative_call(uint32_t(ff8_externals.sub_464F70), 0x2C5);
	ff8_externals.sub_464DB0 = get_relative_call(ff8_externals.ssigpu_tx_select_2_sub_465CE0, 0x2CF);
	ff8_externals.write_palette_texture_set_sub_466190 = get_relative_call(ff8_externals.ssigpu_tx_select_2_sub_465CE0, 0x315);
	ff8_externals.read_vram_palette_sub_467370 = get_relative_call(ff8_externals.write_palette_texture_set_sub_466190, 0x2C);
	ff8_externals.write_palette_to_driver_sub_467310 = get_relative_call(ff8_externals.write_palette_texture_set_sub_466190, 0x7E);
	ff8_externals.read_vram_2_paletted = (void(*)(uint8_t*,int,uint8_t*,int,signed int,int,int,uint16_t*))get_relative_call(ff8_externals.sub_464DB0, 0xEC);
	ff8_externals.sub_4649A0 = get_relative_call(ff8_externals.ssigpu_callbacks_2[100], 0x33);
	ff8_externals.read_vram_3_paletted = (void(*)(uint8_t*,uint8_t*,signed int,int,int,uint16_t*))get_relative_call(ff8_externals.sub_4649A0, 0x13F);

	ff8_externals.sub_559E40 = get_relative_call(ff8_externals.swirl_main_loop, 0x28);
	ff8_externals.sub_559F30 = get_relative_call(ff8_externals.sub_559E40, 0xC1);

	if(NV_VERSION)
	{
		ff8_externals.nvidia_hack1 = get_absolute_value(ff8_externals.sub_559F30, 0x3E);
		ff8_externals.nvidia_hack2 = get_absolute_value(ff8_externals.sub_559F30, 0xAC);
	}

	ff8_externals.menu_viewport = (sprite_viewport *)(get_absolute_value(ff8_externals.sub_4972A0, 0x12) - 0x20);

	ff8_externals.sub_497380 = get_relative_call(ff8_externals.main_menu_main_loop, 0xAA);
	ff8_externals.sub_4B3410 = get_relative_call(ff8_externals.sub_497380, 0xAC);
	ff8_externals.sub_4B3310 = get_relative_call(ff8_externals.sub_497380, 0xD3);
	ff8_externals.sub_4B3140 = get_relative_call(ff8_externals.sub_4B3310, 0xC8);
	ff8_externals.sub_4BDB30 = get_relative_call(ff8_externals.sub_4B3140, 0x4);
	ff8_externals.menu_callbacks = (ff8_menu_callback *)get_absolute_value(ff8_externals.sub_4BDB30, 0x11);
	ff8_externals.menu_cards_render = get_absolute_value(uint32_t(ff8_externals.menu_callbacks[7].func), 0x5);
	ff8_externals.sub_534AD0 = get_relative_call(ff8_externals.menu_cards_render, 0x76);
	ff8_externals.card_texts_off_B96504 = (uint8_t **)get_absolute_value(ff8_externals.sub_534AD0, 0xB1);
	ff8_externals.sub_4EFC00 = get_relative_call(ff8_externals.menu_cards_render, 0x2B6);
	ff8_externals.sub_4EFCD0 = get_absolute_value(ff8_externals.sub_4EFC00, 0xB0);
	ff8_externals.menu_config_render = get_absolute_value(uint32_t(ff8_externals.menu_callbacks[8].func), 0x3);
	ff8_externals.menu_config_render_submenu = get_relative_call(ff8_externals.menu_config_render, 0x101);
	ff8_externals.menu_config_controller = get_absolute_value(uint32_t(ff8_externals.menu_callbacks[8].func), 0x8);
	ff8_externals.menu_config_input_desc = (ff8_menu_config_input *)get_absolute_value(uint32_t(ff8_externals.menu_callbacks[8].func), 0x39);
	ff8_externals.menu_config_input_desc_keymap = (ff8_menu_config_input_keymap *)get_absolute_value(uint32_t(ff8_externals.menu_callbacks[8].func), 0x110);
	ff8_externals.main_menu_render_sub_4E5550 = get_absolute_value(uint32_t(ff8_externals.menu_callbacks[16].func), 0x3);
	ff8_externals.main_menu_controller = get_absolute_value(uint32_t(ff8_externals.menu_callbacks[16].func), 0x8);
	ff8_externals.sub_4C2FF0 = get_relative_call(uint32_t(ff8_externals.menu_callbacks[16].func), 0x2B);
	ff8_externals.menu_chocobo_world_controller = get_absolute_value(uint32_t(ff8_externals.menu_callbacks[27].func), 0xB);
	ff8_externals.create_save_file_sub_4C6E50 = get_relative_call(ff8_externals.main_menu_controller, JP_VERSION ? 0x1004 : 0xF8D);
	ff8_externals.create_save_chocobo_world_file_sub_4C6620 = get_relative_call(ff8_externals.menu_chocobo_world_controller, 0x9F6);
	ff8_externals.get_text_data = get_relative_call(ff8_externals.main_menu_render_sub_4E5550, 0x203);
	ff8_externals.sub_4BE4D0 = get_relative_call(ff8_externals.sub_4B3410, 0x68);
	ff8_externals.sub_4BECC0 = get_relative_call(ff8_externals.sub_4BE4D0, 0x39);
	ff8_externals.menu_draw_text = get_relative_call(ff8_externals.sub_4BECC0, 0x127);
	ff8_externals.get_character_width = (uint32_t (*)(uint32_t))get_relative_call(ff8_externals.menu_draw_text, JP_VERSION ? 0x1E1 : 0x1D0);
	ff8_externals.ff8input_cfg_reset = get_relative_call(ff8_externals.menu_config_controller, 0x185);

	ff8_externals.open_lzs_image = get_relative_call(ff8_externals.load_credits_image, 0x27);
	ff8_externals.credits_open_file = (uint32_t (*)(char*,char*))get_relative_call(ff8_externals.open_lzs_image, 0x72);
	ff8_externals.upload_psx_vram = get_relative_call(ff8_externals.open_lzs_image, 0xB9);
	ff8_externals.psxvram_buffer = (uint8_t *)get_absolute_value(ff8_externals.upload_psx_vram, 0x34);
	ff8_externals.sub_464850 = (void (*)(uint32_t, uint32_t, uint32_t, uint32_t))get_relative_call(ff8_externals.upload_psx_vram, 0x8A);

	ff8_externals.psx_texture_pages = (struc_51 *)get_absolute_value(ff8_externals.sub_464BD0, 0x10);

	ff8_externals.read_field_data = get_relative_call(ff8_externals.sub_471F70, 0x23A);
	ff8_externals.upload_mim_file = get_relative_call(ff8_externals.read_field_data, JP_VERSION ? 0x723 : 0x729);
	ff8_externals.upload_pmp_file = get_relative_call(ff8_externals.read_field_data, JP_VERSION ? 0x80C : 0x812);
	ff8_externals.field_filename = (char *)get_absolute_value(ff8_externals.read_field_data, 0xF0);

	ff8_externals.field_scripts_init = (int(*)(int,int,int,int))(get_relative_call(ff8_externals.read_field_data, JP_VERSION ? 0xEDC : 0xE49));
	ff8_externals.field_state_background_count = (uint8_t *)get_absolute_value(uint32_t(ff8_externals.field_scripts_init), 0x2CD + 0x1);
	ff8_externals.field_state_backgrounds = (ff8_field_state_background **)get_absolute_value(uint32_t(ff8_externals.field_scripts_init), 0x50B + 0x2);
	ff8_externals.load_field_models = get_relative_call(ff8_externals.read_field_data, JP_VERSION ? 0xFA2 : 0xF0F);
	ff8_externals.chara_one_read_file = get_relative_call(ff8_externals.load_field_models, 0x15F);
	ff8_externals.chara_one_seek_file = get_relative_call(ff8_externals.load_field_models, 0x582);
	ff8_externals.chara_one_set_data_start = get_relative_call(ff8_externals.load_field_models, 0xAFF);
	ff8_externals.chara_one_data_start = (uint8_t **)get_absolute_value(ff8_externals.chara_one_set_data_start, 0x5);
	ff8_externals.chara_one_upload_texture = get_relative_call(ff8_externals.load_field_models, 0xB72);

	ff8_externals.worldmap_sub_53F310 = get_relative_call(ff8_externals.worldmap_enter_main, 0xA7);

	ff8_externals.engine_eval_process_input = get_relative_call(ff8_externals.pubintro_main_loop, 0x4);
	ff8_externals.engine_eval_keyboard_gamepad_input = (void (*)())get_relative_call(ff8_externals.engine_eval_process_input, 0x16);
	ff8_externals.has_keyboard_gamepad_input = (void (*)())get_relative_call(ff8_externals.engine_eval_process_input, 0x1B);
	ff8_externals.engine_eval_is_button_pressed = get_relative_call((uint32_t)ff8_externals.engine_eval_keyboard_gamepad_input, 0x4A6);
	ff8_externals.engine_input_confirmed_buttons = (uint32_t*)get_absolute_value(ff8_externals.engine_eval_is_button_pressed, 0x62);
	ff8_externals.engine_input_valid_buttons = (uint32_t*)get_absolute_value(ff8_externals.engine_eval_is_button_pressed, 0x3C);
	ff8_externals.engine_gamepad_button_pressed = (BYTE*)get_absolute_value((uint32_t)ff8_externals.has_keyboard_gamepad_input, 0x22);
	ff8_externals.engine_mapped_buttons = (DWORD*)get_absolute_value((uint32_t)ff8_externals.engine_eval_keyboard_gamepad_input, 0xB9);

	common_externals.get_keyboard_state = get_relative_call((uint32_t)ff8_externals.engine_eval_keyboard_gamepad_input, 0x11);
	ff8_externals.dinput_init_gamepad = get_relative_call(ff8_externals.sub_468810, 0xB4);
	ff8_externals.dinput_update_gamepad_status = get_relative_call((uint32_t)ff8_externals.engine_eval_keyboard_gamepad_input, 0x1B);
	ff8_externals.dinput_gamepad_device = (LPDIRECTINPUTDEVICE8A)get_absolute_value(ff8_externals.dinput_update_gamepad_status, 0x16);
	ff8_externals.dinput_gamepad_state = (LPDIJOYSTATE2)get_absolute_value(ff8_externals.dinput_update_gamepad_status, 0x1B);

	common_externals.dinput_acquire_keyboard = (int (*)())get_relative_call(common_externals.get_keyboard_state, 0x34);
	common_externals.keyboard_device = (IDirectInputDeviceA**)get_absolute_value(common_externals.get_keyboard_state, 0x01);
	common_externals.keyboard_connected = (uint32_t*)get_absolute_value(common_externals.get_keyboard_state, 0x2C);

	common_externals.sfx_init = get_relative_call(ff8_externals.pubintro_init, 0xD8); // sub_469640
	common_externals.directsound_create = get_relative_call(common_externals.sfx_init, 0x5D); // sub_46DBF0
	common_externals.directsound = (LPLPDIRECTSOUND)get_absolute_value(common_externals.directsound_create, 0x26);
	common_externals.directsound_release = get_relative_call(common_externals.sfx_init, 0xD5);

	common_externals.midi_init = get_relative_call(ff8_externals.pubintro_init, 0x130);
	ff8_externals.music_path = (char*)get_absolute_value(ff8_externals.pubintro_init, 0x136);
	common_externals.midi_cleanup = get_relative_call(ff8_externals.pubintro_cleanup, 0x44);
	common_externals.sfx_cleanup = get_relative_call(ff8_externals.pubintro_cleanup, 0x49);
	common_externals.wav_cleanup = get_relative_call(common_externals.sfx_cleanup, 0x0);
	ff8_externals.volume_update = get_relative_call(ff8_externals.field_main_loop, 0x28C);
	ff8_externals.volume_music_update = get_relative_call(ff8_externals.volume_update, 0x6);

	ff8_externals.outputdebugstringa = get_absolute_value(ff8_externals.sm_battle_sound, 0x1B);
	ff8_externals.sdmusicplay = get_relative_call(ff8_externals.sm_battle_sound, 0x164);
	ff8_externals.sd_music_play = (uint32_t(*)(uint32_t, char*, uint32_t))get_relative_call(ff8_externals.sdmusicplay, 0x17);
	ff8_externals.current_music_ids = (uint32_t*)get_absolute_value(uint32_t(ff8_externals.sd_music_play), 0x1AA);
	ff8_externals.play_wav = get_relative_call(uint32_t(ff8_externals.sd_music_play), 0x1DC);
	common_externals.play_wav = get_relative_call(ff8_externals.play_wav, 0x73);

	common_externals.play_midi = get_relative_call(uint32_t(ff8_externals.sd_music_play), 0x20C);

	ff8_externals.dmusic_segment_connect_to_dls = get_relative_call(common_externals.play_midi, 0x247);
	ff8_externals.load_midi_segment = get_relative_call(common_externals.midi_init, 0xC8);
	ff8_externals.choice_music = get_relative_call(ff8_externals.opcode_choicemusic, 0x5D);
	ff8_externals.sd_music_play_at = get_relative_call(ff8_externals.opcode_musicskip, 0x46);
	ff8_externals.load_and_play_midi_segment = get_relative_call(ff8_externals.choice_music, 0x99);
	ff8_externals.load_midi_segment_from_id = get_relative_call(ff8_externals.choice_music, 0xD0);
	ff8_externals.stop_midi_segments = get_relative_call(ff8_externals.load_and_play_midi_segment, 0xB);
	ff8_externals.play_midi_segments = get_relative_call(ff8_externals.choice_music, 0x172);

	common_externals.get_midi_name = (char* (*)(uint32_t))get_relative_call(common_externals.play_midi, 0x21C);

	ff8_externals.sub_46B800 = get_relative_call(ff8_externals.sm_battle_sound, 0x52);
	ff8_externals.stop_music = get_relative_call(ff8_externals.sub_46B800, 0xB);
	common_externals.set_midi_volume_trans = get_relative_call(ff8_externals.opcode_musicvoltrans, 0x49); // Formally music_volume_trans
	common_externals.set_midi_volume_fade = get_relative_call(ff8_externals.opcode_musicvolfade, 0x59); // Formally music_volume_fade
	ff8_externals.dmusicperf_set_volume_sub_46C6F0 = (BOOL (*)(uint32_t, int32_t))get_relative_call(ff8_externals.stop_music, 0x22);
	common_externals.set_midi_volume = get_relative_call(ff8_externals.sm_battle_sound, 0x173); // Formally set_music_volume
	common_externals.master_midi_volume = (DWORD*)get_absolute_value(uint32_t(ff8_externals.dmusicperf_set_volume_sub_46C6F0), 0x24);
	common_externals.stop_wav = get_relative_call(ff8_externals.stop_music, 0x14);
	common_externals.stop_midi = get_relative_call(ff8_externals.stop_music, 0x2A);

	ff8_externals.sub_46C050 = get_relative_call(ff8_externals.sm_battle_sound, 0x5C);
	common_externals.remember_midi_playing_time = get_relative_call(ff8_externals.sub_46C050, 0x00);

	common_externals.midi_status = get_relative_call(ff8_externals.sm_battle_sound, 0x14);

	ff8_externals.load_cdrom = (uint32_t(*)())get_relative_call(ff8_externals.opcode_spuready, 0x4B);
	ff8_externals.load_cdrom_call = ff8_externals.opcode_spuready + 0x4B;
	ff8_externals.play_cdrom = (uint32_t(*)(uint32_t, uint32_t, uint32_t))get_relative_call(ff8_externals.opcode_spuready, 0x60);
	ff8_externals.play_cdrom_call = ff8_externals.opcode_spuready + 0x60;
	ff8_externals.stop_cdrom_cleanup_call = ff8_externals.pubintro_init + 0x183;

	// Pause/Resume functions
	ff8_externals.sub_501B60 = get_relative_call(ff8_externals.battle_load_textures_sub_500900, -0x2A2);
	ff8_externals.pause_music_and_sfx = get_relative_call(ff8_externals.sub_501B60, 0x54);
	common_externals.pause_wav = get_relative_call(ff8_externals.pause_music_and_sfx, 0xF);
	common_externals.pause_midi = get_relative_call(ff8_externals.pause_music_and_sfx, 0x17);
	ff8_externals.restart_music_and_sfx = get_relative_call(ff8_externals.sub_501B60, 0xB3);
	common_externals.restart_wav = get_relative_call(ff8_externals.restart_music_and_sfx, 0xF);
	common_externals.restart_midi = get_relative_call(ff8_externals.restart_music_and_sfx, 0x17);

	// SFX
	ff8_externals.sfx_play_to_current_playing_channel = get_relative_call(ff8_externals.opcode_effectplay2, 0x5F);
	common_externals.play_sfx_on_channel = get_relative_call(ff8_externals.sfx_play_to_current_playing_channel, 0x35);
	common_externals.sfx_load = get_relative_call(common_externals.play_sfx_on_channel, 0x89);
	ff8_externals.sfx_unload_all = get_relative_call(common_externals.sfx_cleanup, 0x5);
	common_externals.sfx_unload = get_relative_call(ff8_externals.sfx_unload_all, 0x4);
	ff8_externals.sfx_pause_all = get_relative_call(ff8_externals.pause_music_and_sfx, 0x1C);
	common_externals.sfx_pause = get_relative_call(ff8_externals.sfx_pause_all, 0x1B);
	ff8_externals.sfx_resume_all = get_relative_call(ff8_externals.restart_music_and_sfx, 0x1C);
	common_externals.sfx_resume = get_relative_call(ff8_externals.sfx_resume_all, 0x1B);
	ff8_externals.sfx_stop_all2 = get_relative_call(ff8_externals.sm_battle_sound, 0x18C);
	ff8_externals.sfx_stop_all = get_relative_call(ff8_externals.sfx_stop_all2, 0x0);
	common_externals.sfx_stop = get_relative_call(ff8_externals.sfx_stop_all, 0x1B);
	ff8_externals.sfx_set_volume = get_relative_call(common_externals.play_sfx_on_channel, 0xA1);
	ff8_externals.sfx_get_master_volume = (int(*)())(ff8_externals.sfx_set_volume - 0x10);
	ff8_externals.sfx_set_master_volume = (void(*)(uint32_t))(uint32_t(ff8_externals.sfx_get_master_volume) - 0xE0);
	common_externals.master_sfx_volume = (uint32_t *)get_absolute_value(uint32_t(ff8_externals.sfx_get_master_volume), 0x1);
	ff8_externals.sfx_current_channel_is_playing = get_relative_call(ff8_externals.sfx_play_to_current_playing_channel, 0xB);
	ff8_externals.sfx_is_playing = get_relative_call(ff8_externals.sfx_current_channel_is_playing - 0x88, 0x0);
	ff8_externals.sfx_set_panning = get_relative_call(common_externals.play_sfx_on_channel, 0x115);
	ff8_externals.sfx_audio_fmt = (ff8_audio_fmt **)get_absolute_value(common_externals.sfx_init, 0x21B);
	ff8_externals.sfx_sound_count = (uint16_t *)get_absolute_value(common_externals.sfx_init, 0x22C);

	// Search DirectSoundBuffer initialization
	ff8_externals.directsound_create_secondary_buffer = get_relative_call(common_externals.sfx_load, 0x22A);
	common_externals.directsound_buffer_flags_1 = ff8_externals.directsound_create_secondary_buffer + 0x34 - 2;

	ff8_externals.sub_5304B0 = (int (*)())get_relative_call(common_externals.update_movie_sample, 0x3D9);

	ff8_externals.enable_framelimiter = (uint32_t *)get_absolute_value(common_externals.stop_movie, 0x49);

	ff8_externals.byte_1CE4907 = (unsigned char *)get_absolute_value(common_externals.update_movie_sample, 0x363);
	ff8_externals.byte_1CE4901 = (unsigned char *)get_absolute_value(common_externals.update_movie_sample, 0x37D);
	ff8_externals.byte_1CE490D = (unsigned char *)get_absolute_value(common_externals.update_movie_sample, 0x3BE);

	ff8_externals.sub_45B310 = get_relative_call(ff8_externals.pubintro_init, 0x91);
	ff8_externals.sub_45B460 = get_relative_call(ff8_externals.sub_45B310, 0x0);
	ff8_externals.ssigpu_init = get_relative_call(ff8_externals.sub_45B460, 0x26);
	ff8_externals.sub_blending_capability = (uint32_t *)get_absolute_value(ff8_externals.sub_45B460, 0x19);
	ff8_externals.d3dcaps = (uint32_t *)get_absolute_value(ff8_externals.ssigpu_init, 0x6C);

	if(FF8_US_VERSION)
	{
		ff8_externals.worldmap_sub_53F310_call_24D = ff8_externals.worldmap_sub_53F310 + 0x24D;
		ff8_externals.worldmap_wmset_set_pointers_sub_542DA0 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x24D);
		ff8_externals.worldmap_section17_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_wmset_set_pointers_sub_542DA0, 0x1ED);
		ff8_externals.worldmap_section18_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_wmset_set_pointers_sub_542DA0, 0x20A);
		ff8_externals.worldmap_section38_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x296);
		ff8_externals.worldmap_section39_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x321);
		ff8_externals.worldmap_section40_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x349);
		ff8_externals.worldmap_section41_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x3B0);
		ff8_externals.worldmap_section42_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x3BB);
		ff8_externals.worldmap_prepare_tim_for_upload = (uint32_t(*)(uint8_t*,ff8_tim*))get_relative_call(ff8_externals.worldmap_sub_53F310, 0x2A9);
		ff8_externals.worldmap_sub_548020 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x3C5);
		ff8_externals.worldmap_sub_53F310_call_2A9 = ff8_externals.worldmap_sub_53F310 + 0x2A9;
		ff8_externals.worldmap_sub_53F310_call_30D = ff8_externals.worldmap_sub_53F310 + 0x30D;
		ff8_externals.worldmap_sub_53F310_call_330 = ff8_externals.worldmap_sub_53F310 + 0x330;
		ff8_externals.worldmap_sub_53F310_call_366 = ff8_externals.worldmap_sub_53F310 + 0x366;
		ff8_externals.worldmap_sub_53F310_call_3B5 = ff8_externals.worldmap_sub_53F310 + 0x3B5;
		ff8_externals.worldmap_sub_554AA0 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x3B5);
		ff8_externals.worldmap_sub_554AA0_call_C2 = ff8_externals.worldmap_sub_554AA0 + 0xC2;
		ff8_externals.worldmap_sub_53F310_loc_53F7EE = ff8_externals.worldmap_sub_53F310 + 0x4DE;
		ff8_externals.worldmap_sub_541970_upload_tim = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x330);
		ff8_externals.worldmap_sub_545E20 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x60C);
		ff8_externals.worldmap_chara_one = get_relative_call(ff8_externals.worldmap_sub_545E20, 0x68);
		ff8_externals.worldmap_sub_5531F0 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x614);
		ff8_externals.worldmap_alter_uv_sub_553B40 = get_relative_call(ff8_externals.worldmap_sub_5531F0, 0x25C);
		ff8_externals.open_file_world = (int32_t(*)(const char*, int32_t, uint32_t, void *))get_relative_call(ff8_externals.worldmap_sub_5531F0, 0x395);
		ff8_externals.open_file_world_sub_52D670_texl_call1 = ff8_externals.worldmap_sub_5531F0 + 0x395;
		ff8_externals.open_file_world_sub_52D670_texl_call2 = ff8_externals.worldmap_sub_5531F0 + 0x69A;
		ff8_externals.upload_psxvram_texl_pal_call1 = ff8_externals.worldmap_sub_5531F0 + 0x2F2;
		ff8_externals.upload_psxvram_texl_pal_call2 = ff8_externals.worldmap_sub_5531F0 + 0x3F4;

		ff8_externals.show_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA0);
		ff8_externals.refresh_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA5);
		ff8_externals.worldmap_with_fog_sub_53FAC0 = get_relative_call(ff8_externals.worldmap_main_loop, 0x134);
		ff8_externals.worldmap_input_update_sub_559240 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x1E);
		ff8_externals.sub_554940 = get_relative_call(ff8_externals.worldmap_input_update_sub_559240, 0x23D);
		ff8_externals.sub_554940_call_130 = ff8_externals.sub_554940 + 0x130;
		ff8_externals.sub_541AE0 = get_relative_call(ff8_externals.sub_554940, 0x130);
		ff8_externals.sub_554BC0 = get_relative_call(ff8_externals.worldmap_input_update_sub_559240, 0x25B);
		ff8_externals.sub_557140 = get_relative_call(ff8_externals.worldmap_input_update_sub_559240, 0x263);
		ff8_externals.sub_54B460 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x5D7);

		ff8_externals.sub_549E80 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x1D5);
		ff8_externals.sub_550070 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x278);
		ff8_externals.vibrate_data_world = (uint8_t *)get_absolute_value(ff8_externals.sub_550070, 0xA82);
		ff8_externals.sub_53BB90 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x2D4);
		ff8_externals.sub_53E2A0 = get_relative_call(ff8_externals.sub_53BB90, 0x327);
		ff8_externals.sub_53E6B0 = get_relative_call(ff8_externals.sub_53E2A0, 0x36B);
		ff8_externals.sub_4023D0 = get_relative_call(ff8_externals.sub_53BB90, 0xAB1);
		ff8_externals.worldmap_fog_filter_polygons_in_block_1 = get_relative_call(ff8_externals.sub_53BB90, 0x42D);
		ff8_externals.worldmap_has_polygon_condition_2045C90 = get_absolute_value(ff8_externals.worldmap_fog_filter_polygons_in_block_1, 0x29);
		ff8_externals.worldmap_polygon_condition_2045C8C = get_absolute_value(ff8_externals.worldmap_fog_filter_polygons_in_block_1, 0x59);
		ff8_externals.worldmap_sub_45DF20 = get_relative_call(ff8_externals.worldmap_fog_filter_polygons_in_block_1, 0x1FC);
		ff8_externals.sub_45E3A0 = get_relative_call(ff8_externals.worldmap_fog_filter_polygons_in_block_1, 0x4A4);
		ff8_externals.worldmap_fog_filter_polygons_in_block_2 = get_relative_call(ff8_externals.sub_53BB90, 0x442);
		ff8_externals.sub_53C750 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x2DB);
		ff8_externals.sub_54E9B0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x369);
		ff8_externals.sub_54FDA0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x375);
		ff8_externals.sub_54D7E0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x3C2);
		ff8_externals.world_dialog_assign_text_sub_543790 = (int (*)(int,int,char*))get_relative_call(ff8_externals.sub_54D7E0, 0x72);
		ff8_externals.world_dialog_question_assign_text_sub_5438D0 = (int (*)(int, int, char*, int, int, int, uint8_t))get_relative_call(ff8_externals.sub_54D7E0, 0x119);
		ff8_externals.sub_544630 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x3D2);
		ff8_externals.sub_545EA0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x4BF);
		ff8_externals.sub_5484B0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x5C9);
		ff8_externals.sub_54A230 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x5CF);
		ff8_externals.sub_543CB0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0xA55);

		ff8_externals.sub_545F10 = get_relative_call(ff8_externals.sub_545EA0, 0x20);

		ff8_externals.sub_546100 = get_relative_call(ff8_externals.sub_545F10, 0x58);

		ff8_externals.battle_trigger_worldmap = ff8_externals.worldmap_with_fog_sub_53FAC0 + 0x4E6;
	}
	else
	{
		ff8_externals.worldmap_sub_53F310_call_24D = ff8_externals.worldmap_sub_53F310 + 0x249;
		ff8_externals.worldmap_wmset_set_pointers_sub_542DA0 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x249);
		ff8_externals.worldmap_section17_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_wmset_set_pointers_sub_542DA0, 0x21C);
		ff8_externals.worldmap_section18_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_wmset_set_pointers_sub_542DA0, 0x23C);
		ff8_externals.worldmap_section38_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x292);
		ff8_externals.worldmap_section39_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x33D);
		ff8_externals.worldmap_section40_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x365);
		ff8_externals.worldmap_section41_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x3D1);
		ff8_externals.worldmap_section42_position = (uint32_t **)get_absolute_value(ff8_externals.worldmap_sub_53F310, 0x3DC);
		ff8_externals.worldmap_prepare_tim_for_upload = (uint32_t(*)(uint8_t*,ff8_tim*))get_relative_call(ff8_externals.worldmap_sub_53F310, 0x2AC);
		ff8_externals.worldmap_sub_548020 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x3E6);
		ff8_externals.worldmap_sub_53F310_call_2A9 = ff8_externals.worldmap_sub_53F310 + 0x2AC;
		ff8_externals.worldmap_sub_53F310_call_30D = ff8_externals.worldmap_sub_53F310 + 0x325;
		ff8_externals.worldmap_sub_53F310_call_330 = ff8_externals.worldmap_sub_53F310 + 0x34C;
		ff8_externals.worldmap_sub_53F310_call_366 = ff8_externals.worldmap_sub_53F310 + 0x382;
		ff8_externals.worldmap_sub_53F310_call_3B5 = ff8_externals.worldmap_sub_53F310 + 0x3D6;
		ff8_externals.worldmap_sub_554AA0 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x3D6);
		ff8_externals.worldmap_sub_554AA0_call_C2 = ff8_externals.worldmap_sub_554AA0 + 0xCC;
		ff8_externals.worldmap_sub_53F310_loc_53F7EE = ff8_externals.worldmap_sub_53F310 + 0x507;
		ff8_externals.worldmap_sub_541970_upload_tim = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x34C);
		ff8_externals.worldmap_sub_545E20 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x635);
		ff8_externals.worldmap_chara_one = get_relative_call(ff8_externals.worldmap_sub_545E20, 0x6B);
		ff8_externals.worldmap_sub_5531F0 = get_relative_call(ff8_externals.worldmap_sub_53F310, 0x63D);
		ff8_externals.worldmap_alter_uv_sub_553B40 = get_relative_call(ff8_externals.worldmap_sub_5531F0, 0x253);
		ff8_externals.open_file_world = (int32_t(*)(const char*, int32_t, uint32_t, void *))get_relative_call(ff8_externals.worldmap_sub_5531F0, 0x38F);
		ff8_externals.open_file_world_sub_52D670_texl_call1 = ff8_externals.worldmap_sub_5531F0 + 0x38F;
		ff8_externals.open_file_world_sub_52D670_texl_call2 = ff8_externals.worldmap_sub_5531F0 + 0x68E;
		ff8_externals.upload_psxvram_texl_pal_call1 = ff8_externals.worldmap_sub_5531F0 + 0x2EC;
		ff8_externals.upload_psxvram_texl_pal_call2 = ff8_externals.worldmap_sub_5531F0 + 0x3F0;

		ff8_externals.show_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA3);
		ff8_externals.refresh_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA8);
		ff8_externals.worldmap_with_fog_sub_53FAC0 = get_relative_call(ff8_externals.worldmap_main_loop, 0x137);
		ff8_externals.worldmap_input_update_sub_559240 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x1C);
		ff8_externals.sub_554940 = get_relative_call(ff8_externals.worldmap_input_update_sub_559240, 0x23A);
		ff8_externals.sub_554940_call_130 = ff8_externals.sub_554940 + 0x13C;
		ff8_externals.sub_541AE0 = get_relative_call(ff8_externals.sub_554940, 0x13C);
		ff8_externals.sub_554BC0 = get_relative_call(ff8_externals.worldmap_input_update_sub_559240, 0x258);
		ff8_externals.sub_557140 = get_relative_call(ff8_externals.worldmap_input_update_sub_559240, 0x260);
		ff8_externals.sub_54B460 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x5D9);

		ff8_externals.sub_549E80 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x1D6);
		ff8_externals.sub_550070 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x279);
		ff8_externals.vibrate_data_world = (uint8_t *)get_absolute_value(ff8_externals.sub_550070, 0xAFA);
		ff8_externals.sub_53BB90 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x2D5);
		ff8_externals.sub_53E2A0 = get_relative_call(ff8_externals.sub_53BB90, 0x336);
		ff8_externals.sub_53E6B0 = get_relative_call(ff8_externals.sub_53E2A0, 0x39A);
		ff8_externals.sub_4023D0 = get_relative_call(ff8_externals.sub_53BB90, 0xAE5);
		ff8_externals.worldmap_fog_filter_polygons_in_block_1 = get_relative_call(ff8_externals.sub_53BB90, 0x43B);
		ff8_externals.worldmap_has_polygon_condition_2045C90 = get_absolute_value(ff8_externals.worldmap_fog_filter_polygons_in_block_1, 0x51);
		ff8_externals.worldmap_polygon_condition_2045C8C = get_absolute_value(ff8_externals.worldmap_fog_filter_polygons_in_block_1, 0x5D);
		ff8_externals.worldmap_sub_45DF20 = get_relative_call(ff8_externals.worldmap_fog_filter_polygons_in_block_1, 0x202);
		ff8_externals.sub_45E3A0 = get_relative_call(ff8_externals.worldmap_fog_filter_polygons_in_block_1, 0x4B8);
		ff8_externals.worldmap_fog_filter_polygons_in_block_2 = get_relative_call(ff8_externals.sub_53BB90, 0x450);
		if (JP_VERSION)
		{
			ff8_externals.sub_4023D0 = get_relative_call(ff8_externals.sub_4023D0, 0x0);
		}
		ff8_externals.sub_53C750 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x2DC);
		ff8_externals.sub_54E9B0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x36A);
		ff8_externals.sub_54FDA0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x376);
		ff8_externals.sub_54D7E0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x3C4);
		ff8_externals.world_dialog_assign_text_sub_543790 = (int (*)(int,int,char*))get_relative_call(ff8_externals.sub_54D7E0, 0x6F);
		ff8_externals.world_dialog_question_assign_text_sub_5438D0 = (int (*)(int, int, char*, int, int, int, uint8_t))get_relative_call(ff8_externals.sub_54D7E0, 0x116);
		ff8_externals.sub_544630 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x3D5);
		ff8_externals.sub_545EA0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x4C1);
		ff8_externals.sub_5484B0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x5CB);
		ff8_externals.sub_54A230 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0x5D1);
		ff8_externals.sub_543CB0 = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, 0xA47);

		ff8_externals.sub_545F10 = get_relative_call(ff8_externals.sub_545EA0, 0x1C);

		ff8_externals.sub_546100 = get_relative_call(ff8_externals.sub_545F10, 0x54);

		ff8_externals.sub_54A0D0 = 0x54A0D0;

		ff8_externals.battle_trigger_worldmap = ff8_externals.worldmap_with_fog_sub_53FAC0 + 0x4EA;
	}

	ff8_externals.worldmap_windows_idx_map = (char*)get_absolute_value((uint32_t)ff8_externals.world_dialog_assign_text_sub_543790, 0x3B);

	ff8_externals.sub_548080 = get_relative_call(ff8_externals.worldmap_sub_53F310_loc_53F7EE, 0x9B);
	ff8_externals.sub_541C80 = (int (*)(WORD*))get_relative_call(ff8_externals.battle_trigger_worldmap, 0);

	ff8_externals.sub_558D70 = get_relative_call(ff8_externals.sub_54B460, 0x3F3);

	common_externals.current_field_name = (char*)get_absolute_value(ff8_externals.opcode_effectplay2, 0x75);
	common_externals.previous_field_id = (WORD*)get_absolute_value(ff8_externals.sub_470250, 0x13);
	common_externals.update_entities_call = common_externals.update_field_entities + 0x657;

	ff8_externals.field_get_dialog_string = get_relative_call(ff8_externals.opcode_mes, 0x5D);
	ff8_externals.set_window_object = get_relative_call(ff8_externals.opcode_mes, 0x66);
	ff8_externals.windows = (ff8_win_obj*)get_absolute_value(ff8_externals.set_window_object, 0x11);

	ff8_externals.sub_470440 = get_absolute_value(ff8_externals.credits_main_loop, 0xD2);
	ff8_externals.sub_49ACD0 = get_relative_call(ff8_externals.sub_470440, JP_VERSION ? 0x9C : 0x98);
	ff8_externals.sub_4A0880 = get_relative_call(ff8_externals.sub_49ACD0, 0x58);
	ff8_externals.sub_4A09A0 = get_absolute_value(ff8_externals.sub_4A0880, 0x25);
	ff8_externals.sub_49FC10 = get_relative_call(ff8_externals.sub_4A09A0, 0xCF);
	ff8_externals.ff8_draw_icon_or_key2 = get_relative_call(ff8_externals.sub_49FC10, 0xF4);
	ff8_externals.dword_1D2B808 = (uint32_t *)get_absolute_value(ff8_externals.ff8_draw_icon_or_key2, 0x41);
	ff8_externals.ff8_draw_icon_or_key3 = ff8_externals.ff8_draw_icon_or_key2 + 0x110;
	ff8_externals.ff8_draw_icon_or_key4 = ff8_externals.ff8_draw_icon_or_key3 + 0xF0;
	ff8_externals.ff8_draw_icon_or_key5 = ff8_externals.ff8_draw_icon_or_key4 + 0x120;
	ff8_externals.ff8_draw_icon_or_key6 = ff8_externals.ff8_draw_icon_or_key5 + 0x110;
	ff8_externals.sub_49FE60 = get_relative_call(ff8_externals.ff8_draw_icon_or_key6, 0xC9);
	ff8_externals.sub_4A0C00 = get_absolute_value(ff8_externals.sub_4A0880, 0x33);
	ff8_externals.show_dialog = (char(*)(int32_t, uint32_t, int16_t))get_relative_call(ff8_externals.sub_4A0C00, 0x5F);

	ff8_externals.sub_485460 = get_relative_call(ff8_externals.sub_47CCB0, 0xB13);
	ff8_externals.sub_485610 = get_relative_call(ff8_externals.sub_485460, 0xDC);
	ff8_externals.sub_48D1A0 = get_absolute_value(ff8_externals.sub_485610, 0x78A);
	ff8_externals.sub_4AD7D0 = get_relative_call(ff8_externals.sub_48D1A0, 0x44);
	ff8_externals.sub_4AD8D0 = get_absolute_value(ff8_externals.sub_4AD7D0, 0xC8);
	ff8_externals.sub_4AB4F0 = get_relative_call(ff8_externals.sub_4AD8D0, 0x159);
	ff8_externals.sub_4AB190 = get_relative_call(ff8_externals.sub_4AB4F0, 0x20);

	ff8_externals.sub_4A84E0 = get_relative_call(ff8_externals.battle_main_loop, 0x142);
	ff8_externals.sub_4AD400 = get_relative_call(ff8_externals.sub_4A84E0, 0x2DB);
	ff8_externals.sub_4BB840 = get_relative_call(ff8_externals.sub_4AD400, 0xCB);
	ff8_externals.battle_current_active_character_id = (BYTE*)get_absolute_value(ff8_externals.sub_4BB840, 0x13);
	ff8_externals.battle_new_active_character_id = (BYTE*)get_absolute_value(ff8_externals.sub_4BB840, 0x37);

	ff8_externals.battle_encounter_id = (WORD*)get_absolute_value(ff8_externals.opcode_battle, 0x66);

	ff8_externals.sub_4AB450 = get_relative_call(ff8_externals.sub_47CCB0, 0xA5F);
	ff8_externals.battle_get_monster_name_sub_495100 = get_relative_call(ff8_externals.sub_4AB450, 0x40);
	ff8_externals.battle_char_struct_dword_1D27B10 = (BYTE**)get_absolute_value(ff8_externals.battle_get_monster_name_sub_495100, 0xF);

	ff8_externals.sub_4AA920 = get_relative_call(ff8_externals.sub_4AB190, 0xED);
	ff8_externals.battle_get_actor_name_sub_47EAF0 = get_relative_call(ff8_externals.sub_4AA920, 0x97);
	ff8_externals.byte_1CFF1C3 = (BYTE*)get_absolute_value(ff8_externals.battle_get_actor_name_sub_47EAF0, 0x15);
	ff8_externals.unk_1CFDC70 = (char*)get_absolute_value(ff8_externals.battle_get_actor_name_sub_47EAF0, 0x1E);
	ff8_externals.unk_1CFDC7C = (char*)get_absolute_value(ff8_externals.battle_get_actor_name_sub_47EAF0, 0x28);
	ff8_externals.word_1CF75EC = (WORD*)get_absolute_value(ff8_externals.battle_get_actor_name_sub_47EAF0, 0x39);
	ff8_externals.unk_1CFF84C = (char*)get_absolute_value(ff8_externals.battle_get_actor_name_sub_47EAF0, 0x44);
	ff8_externals.unk_1CF3E48 = (char*)get_absolute_value(ff8_externals.battle_get_actor_name_sub_47EAF0, 0x57);
	ff8_externals.dword_1CF3EE0 = (DWORD*)get_absolute_value(ff8_externals.battle_get_actor_name_sub_47EAF0, 0x4B);

	ff8_externals.battle_current_actor_talking = (DWORD*)get_absolute_value(ff8_externals.sub_485610, 0x36E);

	ff8_externals.sub_502380 = get_relative_call(ff8_externals.sub_500CC0, 0x69);
	ff8_externals.sub_50A790 = get_relative_call(ff8_externals.sub_502380, 0x51);
	ff8_externals.sub_50A9A0 = get_absolute_value(ff8_externals.sub_50A790, 0x7C);
	ff8_externals.battle_read_effect_sub_50AF20 = get_relative_call(ff8_externals.sub_50A9A0, 0xF4);
	ff8_externals.func_off_battle_effects_C81774 = (DWORD*)get_absolute_value(ff8_externals.battle_read_effect_sub_50AF20, 0x2C);
	ff8_externals.battle_magic_id = (int*)get_absolute_value(ff8_externals.battle_read_effect_sub_50AF20, 0x3E);
	ff8_externals.sub_571870 = get_relative_call(ff8_externals.battle_read_effect_sub_50AF20, 0x63);
	ff8_externals.func_off_battle_effect_textures_50AF93 = (DWORD*)get_absolute_value(ff8_externals.battle_read_effect_sub_50AF20, 0x6B);

	ff8_externals.sub_6C3640 = get_relative_call(ff8_externals.func_off_battle_effects_C81774[FF8BattleEffect::Quezacotl], 0x5);
	ff8_externals.sub_6C3760 = get_absolute_value(ff8_externals.sub_6C3640, 0x8B);
	ff8_externals.vibrate_data_summon_quezacotl = (uint8_t **)get_absolute_value(ff8_externals.sub_6C3760, 0xB0);

	ff8_externals.sub_B586F0 = get_absolute_value(ff8_externals.func_off_battle_effects_C81774[FF8BattleEffect::Leviathan], 0x45);
	ff8_externals.sub_B64B80 = get_relative_call(ff8_externals.sub_B586F0, 0x1B5);
	ff8_externals.leviathan_funcs_B64C3C = (DWORD *)get_absolute_value(ff8_externals.sub_B64B80, 0xBF);
	ff8_externals.mag_data_palette_sub_B66560 = get_relative_call(ff8_externals.leviathan_funcs_B64C3C[FF8BattleEffectOpcode::UploadPalette75], 0x13);
	ff8_externals.effect_struct_27973EC = (DWORD **)get_absolute_value(ff8_externals.mag_data_palette_sub_B66560, 0x4);
	ff8_externals.mag_data_dword_2798A68 = (uint8_t **)get_absolute_value(ff8_externals.mag_data_palette_sub_B66560, 0x19);
	ff8_externals.effect_struct_2797624 = (DWORD **)get_absolute_value(ff8_externals.mag_data_palette_sub_B66560, 0x28);
	ff8_externals.battle_set_action_upload_raw_palette_sub_B666F0 = get_relative_call(ff8_externals.leviathan_funcs_B64C3C[FF8BattleEffectOpcode::UploadPalette75], 0xD4);
	ff8_externals.battle_set_action_upload_raw_palette_sub_B66400 = get_relative_call(ff8_externals.battle_set_action_upload_raw_palette_sub_B666F0, 0x141);

	ff8_externals.sub_B63230 = get_relative_call(ff8_externals.leviathan_funcs_B64C3C[FF8BattleEffectOpcode::UploadTexture39], 0x9);
	ff8_externals.mag_data_texture_sub_B66560 = get_relative_call(ff8_externals.sub_B63230, 0xA);
	ff8_externals.dword_27973E8 = (BYTE**)get_absolute_value(ff8_externals.mag_data_texture_sub_B66560, 0x8F);

	ff8_externals.load_magic_data_sub_571B80 = get_relative_call(ff8_externals.func_off_battle_effect_textures_50AF93[0], 0x5);
	ff8_externals.load_magic_data_sub_571900 = get_relative_call(ff8_externals.load_magic_data_sub_571B80, 0x1E);
	ff8_externals.load_magic_data_sub_5718E0 = get_relative_call(ff8_externals.func_off_battle_effect_textures_50AF93[198], 0x5);

	ff8_externals.sub_84D110 = get_absolute_value(ff8_externals.func_off_battle_effects_C81774[FF8BattleEffect::Scan], 0x28);
	ff8_externals.sub_84D1F0 = get_absolute_value(ff8_externals.sub_84D110, 0x14);
	ff8_externals.sub_84D230 = get_absolute_value(ff8_externals.sub_84D1F0, 0x9);
	ff8_externals.sub_84D2C0 = get_absolute_value(ff8_externals.sub_84D230, 0xC);
	ff8_externals.sub_84D4B0 = get_absolute_value(ff8_externals.sub_84D2C0, 0x3C);
	ff8_externals.sub_84F2A0 = get_absolute_value(ff8_externals.sub_84D4B0, 0x39);
	ff8_externals.sub_84F860 = get_absolute_value(ff8_externals.sub_84F2A0, 0x14);
	ff8_externals.sub_84F8D0 = get_absolute_value(ff8_externals.sub_84F860, 0xD);
	ff8_externals.scan_get_text_sub_B687C0 = get_relative_call(ff8_externals.sub_84F8D0, 0x88);
	ff8_externals.battle_entities_1D27BCB = get_absolute_value(ff8_externals.scan_get_text_sub_B687C0, 0x18);
	ff8_externals.scan_text_positions = get_absolute_value(ff8_externals.scan_get_text_sub_B687C0, 0x20);
	ff8_externals.scan_text_data = get_absolute_value(ff8_externals.scan_get_text_sub_B687C0, 0x27);
	ff8_externals.get_card_name = get_relative_call(ff8_externals.sub_4EFCD0, 0x89);
	ff8_externals.card_name_positions = get_absolute_value(ff8_externals.get_card_name, 0xB);

	ff8_externals.fps_limiter = get_relative_call(ff8_externals.field_main_loop, 0x261);
	if (JP_VERSION)
	{
		ff8_externals.fps_limiter = get_relative_call(ff8_externals.fps_limiter, 0x0);
	}
	ff8_externals.time_volume_change_related_1A78BE0 = (double *)get_absolute_value(ff8_externals.fps_limiter, 0x3F);

	ff8_externals.game_mode_obj_1D9CF88 = (uint32_t*)get_absolute_value(uint32_t(ff8_externals.sub_47CA90), 0xCD);
	ff8_externals.field_vars_stack_1CFE9B8 = get_absolute_value(ff8_externals.opcode_pshm_w, 0x1E);

	common_externals.current_triangle_id = 0x0;
	common_externals.field_game_moment = (WORD*)(ff8_externals.field_vars_stack_1CFE9B8 + 0x100); //0x1CFEAB8
}

void ff8_data()
{
	num_modes = sizeof(ff8_modes) / sizeof(ff8_modes[0]);

	ff8_find_externals();

	memcpy(modes, ff8_modes, sizeof(ff8_modes));

	text_colors[TEXTCOLOR_GRAY] = 0x08;
	text_colors[TEXTCOLOR_BLUE] = 0x01;
	text_colors[TEXTCOLOR_RED] = 0x04;
	text_colors[TEXTCOLOR_PINK] = 0x05;
	text_colors[TEXTCOLOR_GREEN] = 0x02;
	text_colors[TEXTCOLOR_LIGHT_BLUE] = 0x09;
	text_colors[TEXTCOLOR_YELLOW] = 0x0E;
	text_colors[TEXTCOLOR_WHITE] = 0x0F;
}
