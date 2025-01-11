/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

common_externals.start =                      0x40B6E0;
common_externals.debug_print =                0x664E30;
common_externals.debug_print2 =               0x414EE0;
common_externals.create_tex_header =  (tex_header* (*)(void))0x688C46;
common_externals.midi_init =                  0x741780;
common_externals.get_midi_name =      (char* (*)(uint32_t))0x74A0D0;
common_externals.play_midi =                  0x742055;
common_externals.use_midi =                   0x7416A3;
common_externals.stop_midi =                  0x742E2B;
common_externals.cross_fade_midi =            0x742BEE;
common_externals.pause_midi =                 0x742D7B;
common_externals.restart_midi =               0x742DD3;
common_externals.midi_status =                0x742EB3;
common_externals.set_master_midi_volume =     0x742EDA;
common_externals.set_midi_volume =            0x742F37;
common_externals.set_midi_volume_trans =      0x74304C;
common_externals.set_midi_tempo =             0x7431BD;
common_externals.draw_graphics_object =       (int (*)(int n_shape, graphics_object *graphics_object))0x66E272;
common_externals.font_info =          (char *)0x99DDA8;
common_externals.build_dialog_window =        0x6E97E0;
common_externals.load_tex_file =              0x688C96;
common_externals.directsound_buffer_flags_1 = 0x74A55D;
common_externals.create_window =              0x67692A;
common_externals.engine_wndproc =    (WNDPROC)0x676619;

ff7_externals.chocobo_fix =                   0x76EC32;
ff7_externals.midi_fix =                      0x743B42;
ff7_externals.snowboard_fix =         (void *)0x958328;
ff7_externals.cdcheck =                       0x408FF3;
ff7_externals.get_inserted_cd_sub =           0x404A7D;
ff7_externals.insertedCD =            (DWORD*)0x9A0538;
ff7_externals.requiredCD =          (uint8_t*)0xDC0BDC;
ff7_externals.sub_665D9A =            (void (*)(matrix*, nvertex*, indexed_primitive*, p_hundred*, struc_186*, ff7_game_obj*))0x665D9A;
ff7_externals.sub_671742 =            (void (*)(uint32_t, p_hundred*, struc_186*))0x671742;
ff7_externals.sub_6B27A9 =            (void (*)(matrix*, indexed_primitive*, ff7_polygon_set*, p_hundred*, p_group*, void*, ff7_game_obj*))0x6B27A9;
ff7_externals.sub_68D2B8 =            (void (*)(uint32_t, ff7_polygon_set*, void*))0x68D2B8;
ff7_externals.sub_665793 =            (void (*)(matrix*, uint32_t, indexed_primitive*, ff7_polygon_set*, p_hundred*, p_group*, ff7_game_obj*))0x665793;
ff7_externals.matrix3x4 =             (void (*)(matrix*))0x67BC5B;
ff7_externals.matrix4x3_multiply =            0x66CC3B;
ff7_externals.sub_6B26C0 =                    0x6B26C0;
ff7_externals.sub_6B2720 =                    0x6B2720;
ff7_externals.sub_673F5C =                    0x673F5C;
ff7_externals.savemap =               (savemap *)0xDBFD38;
ff7_externals.menu_objects =          (menu_objects *)0xDC0FC0;
ff7_externals.magic_thread_start =            0x427928;
ff7_externals.destroy_magic_effects = (void (*)(void))0x429322;
ff7_externals.init_stuff =                    0x40A091;
ff7_externals.wm_activateapp =                0x409CF2;
ff7_externals.get_gamepad =                   0x41F99E;
ff7_externals.update_gamepad_status =         0x41F7D8;
ff7_externals.gamepad_status =        (struct ff7_gamepad_status*)0x9ADE28;
ff7_externals.call_menu_sound_slider_loop_sfx_down = 0x10A8;
ff7_externals.call_menu_sound_slider_loop_sfx_up = 0x10EC;
ff7_externals.sfx_play_summon =               0x748F8F;
ff7_externals.battle_summon_leviathan_loop =  0x5B1775;
ff7_externals.battle_limit_omnislash_loop =   0x46BA0B;
ff7_externals.sub_60B260 =         (BYTE(*)())0x60B260;
ff7_externals.sub_767C55 =         (BYTE(*)())0x767C55;
ff7_externals.field_battle_toggle =           0x60B3E1;
ff7_externals.worldmap_battle_toggle =        0x767674;
