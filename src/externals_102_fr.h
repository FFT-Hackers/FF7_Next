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

common_externals.start =                      0x40B6F0;
common_externals.debug_print =                0x664DD0;
common_externals.debug_print2 =               0x414EF0;
common_externals.create_tex_header = (tex_header * (*)(void))0x688BE6;
common_externals.midi_init =                  0x6DE000;
common_externals.get_midi_name = (char* (*)(uint32_t))0x6E6950;
common_externals.play_midi =                  0x6DE8D5;
common_externals.use_midi =                   0x6DDF23;
common_externals.stop_midi =                  0x6DF6AB;
common_externals.cross_fade_midi =            0x6DF46E;
common_externals.pause_midi =                 0x6DF5FB;
common_externals.restart_midi =               0x6DF653;
common_externals.midi_status =                0x6DF733;
common_externals.set_master_midi_volume =     0x6DF75A;
common_externals.set_midi_volume =            0x6DF7B7;
common_externals.set_midi_volume_trans =      0x6DF8CC;
common_externals.set_midi_tempo =             0x6DFA3D;
common_externals.draw_graphics_object =       (int (*)(int n_shape, graphics_object *graphics_object))0x66E5E1;
common_externals.font_info =           (char*)0x99FB98;
common_externals.build_dialog_window =        0x774690;
common_externals.load_tex_file =              0x688C36;
common_externals.directsound_buffer_flags_1 = 0x6E6DDD;
common_externals.create_window =              0x6768CA;
common_externals.engine_wndproc =    (WNDPROC)0x6765B9;

ff7_externals.chocobo_fix =                   0x70B4B2;
ff7_externals.midi_fix =                      0x6E03C2;
ff7_externals.snowboard_fix =         (void *)0x94BA48;
ff7_externals.cdcheck =                       0x409003;
ff7_externals.get_inserted_cd_sub =           0x404A8D;
ff7_externals.insertedCD =            (DWORD*)0x9A2328;
ff7_externals.requiredCD =          (uint8_t*)0xF3A91C;
ff7_externals.sub_665D9A = (void (*)(matrix*, nvertex*, indexed_primitive*, p_hundred*, struc_186*, ff7_game_obj*))0x665D3A;
ff7_externals.sub_671742 = (void (*)(uint32_t, p_hundred*, struc_186*))0x6716E2;
ff7_externals.sub_6B27A9 = (void (*)(matrix*, indexed_primitive*, ff7_polygon_set*, p_hundred*, p_group*, void*, ff7_game_obj*))0x6B2749;
ff7_externals.sub_68D2B8 = (void (*)(uint32_t, ff7_polygon_set*, void*))0x68D258;
ff7_externals.sub_665793 = (void (*)(matrix*, uint32_t, indexed_primitive*, ff7_polygon_set*, p_hundred*, p_group*, ff7_game_obj*))0x665733;
ff7_externals.matrix3x4 = (void (*)(matrix*))0x67BBFB;
ff7_externals.matrix4x3_multiply =            0x66CBDB;
ff7_externals.sub_6B26C0 =                    0x6B2660;
ff7_externals.sub_6B2720 =                    0x6B26C0;
ff7_externals.sub_673F5C =                    0x673EFC;
ff7_externals.savemap =               (savemap*)0xF39A78;
ff7_externals.menu_objects =          (menu_objects*)0xF3AD00;
ff7_externals.magic_thread_start =            0x427938;
ff7_externals.destroy_magic_effects = (void (*)(void))0x429332;
ff7_externals.init_stuff =                    0x40A0A1;
ff7_externals.wm_activateapp =                0x409D02;
ff7_externals.get_gamepad =                   0x41F9AE;
ff7_externals.update_gamepad_status =         0x41F7E8;
ff7_externals.gamepad_status = (struct ff7_gamepad_status*)0x9AFC18;
ff7_externals.call_menu_sound_slider_loop_sfx_down = 0x10A5;
ff7_externals.call_menu_sound_slider_loop_sfx_up = 0x10DA;
ff7_externals.sfx_play_summon =               0x6E580F;
ff7_externals.battle_summon_leviathan_loop =  0x5B1785;
ff7_externals.battle_limit_omnislash_loop =   0x46BA1B;
ff7_externals.sub_60B260 = (BYTE(*)())0x60B200;
ff7_externals.sub_767C55 = (BYTE(*)())0x704865;
ff7_externals.field_battle_toggle = 0x60B381;
ff7_externals.worldmap_battle_toggle = 0x704284;
