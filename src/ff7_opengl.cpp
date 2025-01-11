/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#include "cfg.h"
#include "ff7.h"
#include "patch.h"
#include "ff7/defs.h"
#include "ff7_data.h"
#include "ff7/widescreen.h"
#include "ff7/time.h"
#include "ff7/battle/defs.h"
#include "ff7/field/defs.h"
#include "ff7/world/defs.h"

unsigned char midi_fix[] = {0x8B, 0x4D, 0x14};
WORD snowboard_fix[] = {0x0F, 0x10, 0x0F};

uint32_t ff7_credits_loop_gfx_begin_scene(uint32_t unknown, struct game_obj *game_object)
{
	if (drawFFNxLogoFrame(game_object)) {
		if (ff7_externals.get_button_pressed(-1)) {
			stopDrawFFNxLogo();
		}

		return 0;
	}

	return common_begin_scene(unknown, game_object);
}

void ff7_init_hooks(struct game_obj *_game_object)
{
	struct ff7_game_obj *game_object = (struct ff7_game_obj *)_game_object;

	common_externals.add_texture_format = game_object->externals->add_texture_format;
	common_externals.assert_calloc = game_object->externals->assert_calloc;
	common_externals.assert_malloc = game_object->externals->assert_malloc;
	common_externals.assert_free = game_object->externals->assert_free;
	common_externals.create_palette_for_tex = game_object->externals->create_palette_for_tex;
	common_externals.create_texture_format = game_object->externals->create_texture_format;
	common_externals.create_texture_set = game_object->externals->create_texture_set;
	common_externals.generic_light_polygon_set = game_object->externals->generic_light_polygon_set;
	common_externals.generic_load_group = game_object->externals->generic_load_group;
	common_externals.get_game_object = game_object->externals->get_game_object;
	ff7_externals.sub_6A2865 = game_object->externals->sub_6A2865;
	common_externals.make_pixelformat = game_object->externals->make_pixelformat;

	ff7_data(game_object);

	if (game_width == 1280)
		MessageBoxA(gameHwnd, "Using this driver with the old high-res patch is NOT recommended, there will be glitches.", "Warning", 0);

	game_object->d3d2_flag = 1;
	game_object->nvidia_fix = 0;

	// Load Models atoi function
	replace_call_function(ff7_externals.field_load_models_atoi, ff7_field_load_models_atoi);

	// DirectInput hack, try to reacquire on any error
	memset_code(ff7_externals.dinput_getdata2 + 0x65, 0x90, 9);
	memset_code((uint32_t)common_externals.dinput_acquire_keyboard + 0x31, 0x90, 5);

	// Allow mouse cursor to be shown
	replace_function(ff7_externals.dinput_createdevice_mouse, noop);

	// TODO: Comment this if Chocobo's not visible in race
	// replace_function(ff7_externals.draw_3d_model, draw_3d_model);

	// sub_6B27A9 hack, replace d3d code
	memset_code((uint32_t)ff7_externals.sub_6B27A9 + 25, 0x90, 6);
	replace_function(ff7_externals.sub_6B26C0, draw_single_triangle);
	replace_function(ff7_externals.sub_6B2720, sub_6B2720);

	// replace framebuffer access routine with our own version
	replace_function(ff7_externals.sub_673F5C, sub_673F5C);

	replace_function(ff7_externals.destroy_d3d2_indexed_primitive, destroy_d3d2_indexed_primitive);

	replace_function(ff7_externals.load_animation, load_animation);
	replace_function(ff7_externals.read_battle_hrc, read_battle_hrc);
	replace_function(common_externals.destroy_tex_header, destroy_tex_header);
	replace_function(ff7_externals.load_p_file, load_p_file);
	replace_function(common_externals.load_tex_file, load_tex_file);

	replace_function(ff7_externals.field_load_textures, ff7::field::field_load_textures);
	replace_function(ff7_externals.field_layer1_pick_tiles, ff7::field::field_layer1_pick_tiles);
	replace_function(ff7_externals.field_layer2_pick_tiles, ff7::field::field_layer2_pick_tiles);
	replace_function(ff7_externals.field_layer3_pick_tiles, ff7::field::field_layer3_pick_tiles);
	replace_function(ff7_externals.field_layer4_pick_tiles, ff7::field::field_layer4_pick_tiles);
	patch_code_byte(ff7_externals.field_draw_everything + 0xE2, 0x1D);
	patch_code_byte(ff7_externals.field_draw_everything + 0x353, 0x1D);
	replace_function(ff7_externals.open_flevel_siz, ff7::field::field_open_flevel_siz);
	replace_function(ff7_externals.field_init_scripted_bg_movement, ff7::field::field_init_scripted_bg_movement);
	replace_function(ff7_externals.field_update_scripted_bg_movement, ff7::field::field_update_scripted_bg_movement);

	replace_function(ff7_externals.get_equipment_stats, get_equipment_stats);

	replace_function(common_externals.open_file, open_file);
	replace_function(common_externals.read_file, read_file);
	replace_function(common_externals.__read_file, __read_file);
	replace_function(ff7_externals.__read, __read);
	replace_function(common_externals.write_file, write_file);
	replace_function(common_externals.close_file, close_file);
	replace_function(common_externals.get_filesize, get_filesize);
	replace_function(common_externals.tell_file, tell_file);
	replace_function(common_externals.seek_file, seek_file);
	replace_function(ff7_externals.open_lgp_file, open_lgp_file);
	replace_function(ff7_externals.lgp_chdir, lgp_chdir);
	replace_function(ff7_externals.lgp_open_file, lgp_open_file);
	replace_function(ff7_externals.lgp_read, lgp_read);
	replace_function(ff7_externals.lgp_read_file, lgp_read_file);
	replace_function(ff7_externals.lgp_get_filesize, lgp_get_filesize);
	replace_function(ff7_externals.lgp_seek_file, lgp_seek_file);

	replace_function(ff7_externals.magic_thread_start, ff7::battle::magic_thread_start);

	replace_function(ff7_externals.kernel2_reset_counters, kernel2_reset_counters);
	replace_function(ff7_externals.kernel2_add_section, kernel2_add_section);
	replace_function(ff7_externals.kernel2_get_text, kernel2_get_text);
	patch_code_uint((uint32_t)ff7_externals.kernel_load_kernel2 + 0x1D, 20 * 65536);
	replace_call_function(ff7_externals.kernel_init + 0x1FD, ff7_load_kernel2_wrapper);
	replace_call_function(ff7_externals.battle_scene_bin_sub_5D1050 + 0x85, ff7::battle::load_scene_bin_chunk);

	replace_function(ff7_externals.read_field_file, ff7_read_field_file);

	// prevent FF7 from stopping the movie when the window gets unfocused
	replace_function(ff7_externals.wm_activateapp, ff7_wm_activateapp);

	// required for the soft reset
	replace_function(ff7_externals.engine_exit_game_mode_sub_666C78, ff7_engine_exit_game_mode);

	// required to fix missing gameover music and broken menu sound after playing it
	replace_call_function(ff7_externals.on_gameover_enter, ff7_on_gameover_enter);
	replace_call_function(ff7_externals.on_gameover_exit, ff7_on_gameover_exit);

	// Disable DirectSound creation when using the external SFX layer
	// TODO: We need to hook more functions in the engine as it causes crashes around the game the way it is now.
	// if (use_external_sfx)
	// {
	// 	replace_function(common_externals.directsound_create, ff7_dsound_create);
	// 	replace_function(common_externals.directsound_release, ff7_dsound_release);
	// 	replace_function(common_externals.directsound_createsoundbuffer, ff7_dsound_createsoundbuffer);
	// }

	// ##################################
	// animation glitch fixes
	// ##################################

	// phoenix camera animation glitch
	memset_code(ff7_externals.run_phoenix_main_loop_516297 + 0x3A5, 0x90, 49);
	memset_code(ff7_externals.run_phoenix_main_loop_516297 + 0x3F7, 0x90, 49);

	// ##################################
	// bugfixes to enhance game stability
	// ##################################

	// chocobo crash fix
	memset_code(ff7_externals.chocobo_fix - 12, 0x90, 36);

	// midi transition crash fix
	memcpy_code(ff7_externals.midi_fix, midi_fix, sizeof(midi_fix));
	memset_code(ff7_externals.midi_fix + sizeof(midi_fix), 0x90, 18 - sizeof(midi_fix));

	// snowboard crash fix
	memcpy(ff7_externals.snowboard_fix, snowboard_fix, sizeof(snowboard_fix));

	// coaster aim fix
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x129, 5);
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x14A, 5);
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x16D, 5);
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x190, 5);

	// condor minigame load unit textures fix
	replace_call_function(ff7_externals.sub_5F342C + 0x66E, ff7_condor_fix_unit_texture_load);
	replace_call_function(ff7_externals.sub_5F342C + 0x7B9, ff7_condor_fix_unit_texture_load);
	replace_call_function(ff7_externals.sub_5F342C + 0x904, ff7_condor_fix_unit_texture_load);
	replace_call_function(ff7_externals.sub_5F342C + 0x977, ff7_condor_fix_unit_texture_load);
	replace_call_function(ff7_externals.sub_5F342C + 0x9EA, ff7_condor_fix_unit_texture_load);
	replace_call_function(ff7_externals.sub_5F342C + 0xA35, ff7_condor_fix_unit_texture_load);

	// ##################################
	// menu UI glitch fix
	// ##################################
	replace_call_function(ff7_externals.battle_set_do_render_menu_call, ff7::battle::battle_menu_enter);

	// #####################
	// widescreen / uncrop
	// #####################
	if(widescreen_enabled || enable_uncrop)
		ff7_widescreen_hook_init();

	if (enable_time_cycle)
		ff7::time_hook_init();

	// #####################
	// new timer calibration
	// #####################

	// replace time diff
	replace_function((uint32_t)common_externals.diff_time, qpc_diff_time);

	if (ff7_fps_limiter >= FPS_LIMITER_DEFAULT)
	{
		// replace rdtsc timing
		replace_function((uint32_t)common_externals.get_time, qpc_get_time);

		// override the timer calibration
		QueryPerformanceFrequency((LARGE_INTEGER *)&game_object->_countspersecond);
		game_object->countspersecond = (double)game_object->_countspersecond;

		replace_function(ff7_externals.fps_limiter_swirl, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_battle, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_coaster, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_condor, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_field, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_highway, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_snowboard, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_worldmap, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_chocobo, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_submarine, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_credits, ff7_limit_fps);

		if (ff7_fps_limiter >= FPS_LIMITER_30FPS)
		{
			battle_frame_multiplier = (ff7_fps_limiter == FPS_LIMITER_30FPS) ? 2 : 4;

			patch_divide_code<byte>(ff7_externals.battle_fps_menu_multiplier, battle_frame_multiplier); // Works perfectly only in 30 FPS

			ff7::battle::camera_hook_init();
			ff7::battle::animations_hook_init();

			if(ff7_fps_limiter == FPS_LIMITER_60FPS)
			{
				common_frame_multiplier = 2;

				// Swirl mode 60FPS fix
				patch_multiply_code<byte>(ff7_externals.swirl_main_loop + 0x184, common_frame_multiplier); // wait frames before swirling
				patch_multiply_code<byte>(ff7_externals.swirl_loop_sub_4026D4 + 0x3E, common_frame_multiplier);
				byte swirl_cmp_fix[7] = {0x82, 0xB9, 0x50, 0x11, 0x00, 0x00, 0x9C};
				memcpy_code(ff7_externals.swirl_loop_sub_4026D4 + 0x10B, swirl_cmp_fix, sizeof(swirl_cmp_fix));
				patch_divide_code<double>(get_absolute_value(ff7_externals.swirl_loop_sub_4026D4, 0x1AB), common_frame_multiplier);
				patch_divide_code<double>(get_absolute_value(ff7_externals.swirl_loop_sub_4026D4, 0x1B1), common_frame_multiplier);
				patch_divide_code<double>(get_absolute_value(ff7_externals.swirl_loop_sub_4026D4, 0x1E4), common_frame_multiplier);
				patch_divide_code<double>(get_absolute_value(ff7_externals.swirl_loop_sub_4026D4, 0x1EA), common_frame_multiplier);
			}
		}
	}

	// World fix (60 FPS, night cycle, external mesh)
	ff7::world::world_hook_init();

	// Field FPS fix (60FPS, 30FPS movies)
	ff7::field::ff7_field_hook_init();

	// ##########################
	// field eye to model mapping
	// ##########################
	replace_function(ff7_externals.field_models_eye_to_model, ff7::field::ff7_field_models_eye_to_model);

	// #####################
	// red XIII eye blinking
	// #####################
	byte ff7_redxiii_eye_fix[] = "\xEC\x79\x90\x00\x00\x00\x00\x00";
	memcpy_code((uint32_t)ff7_externals.field_models_eye_blink_buffer + 0x58, ff7_redxiii_eye_fix, sizeof(ff7_redxiii_eye_fix) - 1);

	// ##################
	// field eye blinking
	// ##################
	replace_function(uint32_t(ff7_externals.field_blink_3d_model_649B50), ff7::field::ff7_field_blink_eye_sub_649B50);
	// allow eye condition to always match
	memset_code(ff7_externals.opcode_kawai + 0x275, 0x90, 6);
	memset_code(ff7_externals.opcode_kawai + 0x294, 0x90, 2);
	// allow up to 128 eyes
	memset_code(ff7_externals.opcode_kawai + 0x286, 0x7F, 1);
	memset_code(ff7_externals.opcode_kawai + 0x2A1, 0x7F, 1);

	// #####################
	// field vertical center
	// #####################
	if(ff7_field_center || widescreen_enabled)
	{
		patch_code_byte(ff7_externals.field_init_viewport_values + 0x35, 16);
		patch_code_int(ff7_externals.field_init_viewport_values + 0x6E, 240);
	}

	// ########################
	// field direct color black
	// ########################
	patch_code_short(uint32_t(ff7_externals.field_convert_type2_layers) + 0xE3, 0x8000);

	// #####################
	// worldmap footsteps
	// #####################
	if(ff7_footsteps)
		replace_call_function(ff7_externals.world_update_player_74EA48 + 0xCDF, ff7::world::world_update_model_movement);

	// #####################
	// worldmap fx effects ( forest trail, ocean trail with highwind, etc. )
	// #####################
	switch(version)
		{
			case VERSION_FF7_102_US:
				patch_code_byte(ff7_externals.world_sub_75C283 + 0x2A8, 0x8);
				break;
			case VERSION_FF7_102_DE:
				patch_code_byte(ff7_externals.world_sub_75C283 + 0x2A8, 0x20);
				break;
			case VERSION_FF7_102_FR:
				patch_code_byte(ff7_externals.world_sub_75C283 + 0x2A8, 0x50);
				break;
			case VERSION_FF7_102_SP:
				patch_code_byte(ff7_externals.world_sub_75C283 + 0x2A8, 0xB0);
				break;
		}

	// #####################
	// battle toggle
	// #####################
	replace_call_function(ff7_externals.field_battle_toggle, ff7_toggle_battle_field);
	replace_call_function(ff7_externals.worldmap_battle_toggle, ff7_toggle_battle_worldmap);

	// #####################
	// auto attack toggle
	// #####################
	replace_call_function(ff7_externals.battle_menu_update_6CE8B3 + 0xD9, ff7_battle_menu_sub_6DB0EE);
	replace_call_function(ff7_externals.handle_actor_ready + 0x187, ff7_set_battle_menu_state_data_at_full_atb);

	// #####################
	// gamepad
	// #####################
	replace_function(ff7_externals.get_gamepad, ff7_get_gamepad);
	replace_function(ff7_externals.update_gamepad_status, ff7_update_gamepad_status);

	// ###########################
	// control battle/world camera
	// ###########################
	if(enable_analogue_controls) {
		replace_call_function(ff7_externals.battle_sub_42D992 + 0xFB, ff7::battle::update_battle_camera);
		replace_function((uint32_t)ff7_externals.field_clip_with_camera_range_6438F6, ff7::field::ff7_field_clip_with_camera_range);

		// Disable show targets with R2 in battles
        memset_code(ff7_externals.handle_actor_ready + 0xA8, 0x90, 29);
	}

	// ###########################
	// japanese text
	// ###########################
	if (ff7_japanese_edition)
	{
		replace_function(ff7_externals.field_submit_draw_text_640x480_6E706D, field_submit_draw_text_640x480_6E706D_jp);
		replace_function((uint32_t)ff7_externals.engine_load_menu_graphics_objects_6C1468, engine_load_menu_graphics_objects_6C1468_jp);
		replace_function((uint32_t)ff7_externals.field_draw_text_boxes_and_text_graphics_object_6ECA68, field_draw_text_boxes_and_text_graphics_object_6ECA68_jp);
		replace_function((uint32_t)ff7_externals.common_submit_draw_char_from_buffer_6F564E, common_submit_draw_char_from_buffer_6F564E_jp);
		replace_function((uint32_t)	ff7_externals.menu_draw_everything_6CC9D3, menu_draw_everything_6CC9D3_jp);
		replace_function((uint32_t)	ff7_externals.battle_draw_menu_everything_6CEE84, battle_draw_menu_everything_6CEE84_jp);
		replace_function((uint32_t)	ff7_externals.draw_text_top_display_6D1CC0, draw_text_top_display_6D1CC0_jp);
		replace_function((uint32_t)	ff7_externals.main_menu_draw_everything_maybe_6C0B91, main_menu_draw_everything_maybe_6C0B91_jp);
		//replace_function((uint32_t)	ff7_externals.field_text_box_window_paging_631945, field_text_box_window_paging_631945_jp);
		replace_function((uint32_t)	ff7_externals.field_text_box_window_opening_6317A9, field_text_box_window_opening_6317A9_jp);

		patch_code_byte(0x632C4E, 0xC);
		patch_code_byte(0x632C4E + 0x1, 0xC);
		patch_code_byte(0x632C4E + 0x2, 0xC);
		patch_code_byte(0x632C4E + 0x3, 0xC);
		patch_code_byte(0x632C4E + 0x4, 0xC);

		replace_function(ff7_externals.sub_6F54A2, sub_6F54A2_jp);
	}

	//######################
	// menu rendering fix
	//######################
	replace_call_function(ff7_externals.timer_menu_sub + 0x72F, ff7_menu_sub_6F5C0C);
	replace_call_function(ff7_externals.timer_menu_sub + 0xD77, ff7_menu_sub_6FAC38);

	//######################
	// shadow lighting fix
	//######################
	if(enable_lighting)
	    memset_code(ff7_externals.battle_sub_42F3E8 + 0xD7D, 0x90, 78); // Disable battle shadow draw call

	//######################
	// day night time cycle
	//######################
	if (enable_time_cycle)
	{
		replace_call_function(ff7_externals.battle_draw_text_ui_graphics_objects_call, ff7::battle::draw_ui_graphics_objects_wrapper);
		replace_call_function(ff7_externals.battle_draw_box_ui_graphics_objects_call, ff7::battle::draw_ui_graphics_objects_wrapper);
	}

	if (game_lighting != GAME_LIGHTING_ORIGINAL)
	{
		// Disables unnecesary lighting in Chocobos applied throught the KAWAI op
		memset_code(ff7_externals.field_apply_kawai_op_64A070 + 0x864, 0x90, 5);
		memset_code(ff7_externals.field_apply_kawai_op_64A070 + 0x2E4, 0x90, 5);
		memset_code(ff7_externals.field_apply_kawai_op_64A070 + 0x3A3, 0x90, 5);
		memset_code(ff7_externals.field_apply_kawai_op_64A070 + 0x23C, 0x90, 5);
		// Disables unnecessary lighting in temple of the ancients rolling rocks
		replace_function(ff7_externals.sub_64EC60, noop);
	}

	//#############################################
	// steam save game preservation and other fixes
	//#############################################
	if (steam_edition)
	{
		switch(version)
		{
			case VERSION_FF7_102_US:
				replace_call_function(ff7_externals.menu_sub_6FEDB0 + 0x1096, ff7_write_save_file);
				// Disable "Normal" setting in Controller section of the Config menu (it softlocks on Steam)
				memset_code(ff7_externals.config_menu_sub + 0x8AC, 0x90, 0xE6);
				break;
			case VERSION_FF7_102_DE:
				replace_call_function(ff7_externals.menu_sub_6FEDB0 + 0x10B2, ff7_write_save_file);
				// Disable "Normal" setting in Controller section of the Config menu (it softlocks on Steam)
				memset_code(ff7_externals.config_menu_sub + 0x8B3, 0x90, 0xE6);
				break;
			case VERSION_FF7_102_FR:
				replace_call_function(ff7_externals.menu_sub_6FEDB0 + 0x10B2, ff7_write_save_file);
				// Disable "Normal" setting in Controller section of the Config menu (it softlocks on Steam)
				memset_code(ff7_externals.config_menu_sub + 0x8AC, 0x90, 0xE6);
				break;
			case VERSION_FF7_102_SP:
				replace_call_function(ff7_externals.menu_sub_6FEDB0 + 0x10FE, ff7_write_save_file);
				// Disable "Normal" setting in Controller section of the Config menu (it softlocks on Steam)
				memset_code(ff7_externals.config_menu_sub + 0x8B3, 0x90, 0xE6);
				break;
		}

		// Restore Steam release behavior on character name screen when using gamepads in Steam Input mode
		// Aali driver used to patch out these three functions to fix this issue
		replace_function(ff7_externals.set_default_input_settings_save, noop);
		replace_function(ff7_externals.keyboard_name_input, noop);
		replace_function(ff7_externals.restore_input_settings, noop);

    // Patch the default config bitmask so that "Customize" controller option is enabled by default
    memset_code(ff7_externals.config_initialize + 0x36, 0x45, 1);
	}

	//###############################
	// steam achievement unlock calls
	//###############################
	if(steam_edition || enable_steam_achievements)
	{
		// BATTLE SQUARE
		replace_call_function(ff7_externals.battle_sub_42A0E7 + 0x78, ff7::battle::load_battle_stage);

		// GIL, MASTER MATERIA, BATTLE WON
		replace_call_function(ff7_externals.battle_enemy_killed_sub_433BD2 + 0x2AF, ff7::battle::battle_sub_5C7F94);
		replace_call_function(ff7_externals.menu_sub_6CDA83 + 0x20, ff7_menu_battle_end_sub_6C9543);
		if (version == VERSION_FF7_102_US) {
			replace_call_function(ff7_externals.menu_shop_loop + 0x327B, ff7_get_materia_gil);
		} else {
			replace_call_function(ff7_externals.menu_shop_loop + 0x3373, ff7_get_materia_gil);
		}
		replace_function(ff7_externals.opcode_increase_gil_call, ff7_opcode_increase_gil_call);

		// 1ST LIMIT BREAK
		replace_function(ff7_externals.display_battle_action_text_sub_6D71FA, ff7::battle::display_battle_action_text_sub_6D71FA);

		// MATERIA GOT
		replace_call_function(ff7_externals.opcode_add_materia_inventory_call + 0x43, ff7_menu_sub_6CBCF3);
		replace_call_function(ff7_externals.menu_sub_705D16 + 0x1729, ff7_menu_sub_6CC17F);
		replace_call_function(ff7_externals.menu_sub_705D16 + 0x1819, ff7_menu_sub_6CC17F);

		// LAST LIMIT BREAK
		replace_function(ff7_externals.menu_decrease_item_quantity, ff7_menu_decrease_item_quantity);

		// GOLD CHOCOBO, YUFFIE, VINCENT: called through update_field_entities
		replace_call_function(ff7_externals.opcode_setbyte + 0x14, ff7_chocobo_field_entity_60FA7D);
		replace_call_function(ff7_externals.opcode_biton + 0x3A, ff7_character_regularly_field_entity_60FA7D);

		// INITIALIZATION AT LOAD SAVE FILE
		switch(version) {
			case VERSION_FF7_102_US:
			case VERSION_FF7_102_SP:
				replace_call_function(ff7_externals.menu_sub_7212FB + 0xE9D, ff7_load_save_file);
				break;
			case VERSION_FF7_102_DE:
			case VERSION_FF7_102_FR:
				replace_call_function(ff7_externals.menu_sub_7212FB + 0xEC5, ff7_load_save_file);
				break;
		}
	}

	replace_call(ff7_externals.credits_main_loop + 0xAC, ff7_credits_loop_gfx_begin_scene);

	//######################
	// snowboard .P model vertices limit fix + allow float vertex data type
	//######################
	replace_function(ff7_externals.snowboard_parse_model_vertices_732159, ff7_snowboard_parse_model_vertices);
}

struct ff7_gfx_driver *ff7_load_driver(void* _game_object)
{
	struct ff7_gfx_driver *ret = (ff7_gfx_driver *)external_calloc(1, sizeof(*ret));

	ret->init = common_init;
	ret->cleanup = common_cleanup;
	ret->lock = common_lock;
	ret->unlock = common_unlock;
	ret->flip = common_flip;
	ret->clear = common_clear;
	ret->clear_all= common_clear_all;
	ret->setviewport = common_setviewport;
	ret->setbg = common_setbg;
	ret->prepare_polygon_set = common_prepare_polygon_set;
	ret->load_group = common_load_group;
	ret->setmatrix = common_setmatrix;
	ret->unload_texture = common_unload_texture;
	ret->load_texture = common_load_texture;
	ret->palette_changed = common_palette_changed;
	ret->write_palette = common_write_palette;
	ret->blendmode = common_blendmode;
	ret->light_polygon_set = common_light_polygon_set;
	ret->field_64 = common_field_64;
	ret->setrenderstate = common_setrenderstate;
	ret->_setrenderstate = common_setrenderstate;
	ret->__setrenderstate = common_setrenderstate;
	ret->field_74 = common_field_74;
	ret->field_78 = common_field_78;
	ret->draw_deferred = common_draw_deferred;
	ret->field_80 = common_field_80;
	ret->field_84 = common_field_84;
	ret->begin_scene = common_begin_scene;
	ret->end_scene = common_end_scene;
	ret->field_90 = common_field_90;
	ret->setrenderstate_flat2D = common_setrenderstate_2D;
	ret->setrenderstate_smooth2D = common_setrenderstate_2D;
	ret->setrenderstate_textured2D = common_setrenderstate_2D;
	ret->setrenderstate_paletted2D = common_setrenderstate_2D;
	ret->_setrenderstate_paletted2D = common_setrenderstate_2D;
	ret->draw_flat2D = common_draw_2D;
	ret->draw_smooth2D = common_draw_2D;
	ret->draw_textured2D = common_draw_2D;
	ret->draw_paletted2D = common_draw_paletted2D;
	ret->setrenderstate_flat3D = common_setrenderstate_3D;
	ret->setrenderstate_smooth3D = common_setrenderstate_3D;
	ret->setrenderstate_textured3D = common_setrenderstate_3D;
	ret->setrenderstate_paletted3D = common_setrenderstate_3D;
	ret->_setrenderstate_paletted3D = common_setrenderstate_3D;
	ret->draw_flat3D = common_draw_3D;
	ret->draw_smooth3D = common_draw_3D;
	ret->draw_textured3D = common_draw_3D;
	ret->draw_paletted3D = common_draw_paletted3D;
	ret->setrenderstate_flatlines = common_setrenderstate_2D;
	ret->setrenderstate_smoothlines = common_setrenderstate_2D;
	ret->draw_flatlines = common_draw_lines;
	ret->draw_smoothlines = common_draw_lines;
	ret->field_EC = common_field_EC;

	return ret;
}
