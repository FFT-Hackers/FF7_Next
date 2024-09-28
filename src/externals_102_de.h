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

common_externals.start =                      0x40B6E0;
common_externals.debug_print =                0x664E00;
common_externals.debug_print2 =               0x414EE0;
common_externals.create_tex_header = (tex_header * (*)(void))0x688C16;
common_externals.midi_init =                  0x6DE060;
common_externals.get_midi_name = (char* (*)(uint32_t))0x6E69B0;
common_externals.play_midi =                  0x6DE935;
common_externals.use_midi =                   0x6DDF83;
common_externals.stop_midi =                  0x6DF70B;
common_externals.cross_fade_midi =            0x6DF4CE;
common_externals.pause_midi =                 0x6DF65B;
common_externals.restart_midi =               0x6DF6B3;
common_externals.midi_status =                0x6DF793;
common_externals.set_master_midi_volume =     0x6DF7BA;
common_externals.set_midi_volume =            0x6DF817;
common_externals.set_midi_volume_trans =      0x6DF92C;
common_externals.set_midi_tempo =             0x6DFA9D;
common_externals.draw_graphics_object =       (int (*)(int n_shape, graphics_object *graphics_object))0x66E611;
common_externals.font_info =          (char *)0x99EB68;
common_externals.build_dialog_window =        0x7743B0;
common_externals.load_tex_file =              0x688C66;
common_externals.directsound_buffer_flags_1 = 0x6E6E3D;
common_externals.create_window =              0x6768FA;
common_externals.engine_wndproc =    (WNDPROC)0x6765E9;

ff7_externals.chocobo_fix =                   0x70B512;
ff7_externals.midi_fix =                      0x6E0422;
ff7_externals.snowboard_fix =         (void *)0x94BA30;
ff7_externals.cdcheck =                       0x408FF3;
ff7_externals.get_inserted_cd_sub =           0x404A7D;
ff7_externals.insertedCD =            (DWORD*)0x9A12F8;
ff7_externals.requiredCD =          (uint8_t*)0xF3990C;
ff7_externals.sub_665D9A = (void (*)(matrix*, nvertex*, indexed_primitive*, p_hundred*, struc_186*, ff7_game_obj*))0x665D6A;
ff7_externals.sub_671742 = (void (*)(uint32_t, p_hundred*, struc_186*))0x671712;
ff7_externals.sub_6B27A9 = (void (*)(matrix*, indexed_primitive*, ff7_polygon_set*, p_hundred*, p_group*, void*, ff7_game_obj*))0x6B2779;
ff7_externals.sub_68D2B8 = (void (*)(uint32_t, ff7_polygon_set*, void*))0x68D288;
ff7_externals.sub_665793 = (void (*)(matrix*, uint32_t, indexed_primitive*, ff7_polygon_set*, p_hundred*, p_group*, ff7_game_obj*))0x665763;
ff7_externals.matrix3x4 = (void (*)(matrix*))0x67BC2B;
ff7_externals.matrix4x3_multiply =            0x66CC0B;
ff7_externals.sub_6B26C0 =                    0x6B2690;
ff7_externals.sub_6B2720 =                    0x6B26F0;
ff7_externals.sub_673F5C =                    0x673F2C;
ff7_externals.savemap =               (savemap *)0xF38A68;
ff7_externals.menu_objects =          (menu_objects *)0xF39CF0;
ff7_externals.magic_thread_start =            0x427928;
ff7_externals.destroy_magic_effects = (void (*)(void))0x429322;
ff7_externals.init_stuff =                    0x40A091;
ff7_externals.wm_activateapp =                0x409CF2;
ff7_externals.get_gamepad =                   0x41F99E;
ff7_externals.update_gamepad_status =         0x41F7D8;
ff7_externals.gamepad_status = (struct ff7_gamepad_status*)0x9AEBE8;
ff7_externals.call_menu_sound_slider_loop_sfx_down = 0x10AF;
ff7_externals.call_menu_sound_slider_loop_sfx_up = 0x10F3;
ff7_externals.sfx_play_summon =               0x6E586F;
ff7_externals.battle_summon_leviathan_loop =  0x5B1775;
ff7_externals.battle_limit_omnislash_loop =   0x46BA0B;
ff7_externals.sub_60B260 = (BYTE(*)())0x60B1F0;
ff7_externals.sub_767C55 = (BYTE(*)())0x7048C5;
ff7_externals.field_battle_toggle = 0x60B371;
ff7_externals.worldmap_battle_toggle = 0x7042E4;
