/****************************************************************************/
//    Copyright (C) 2024 Cosmos                                             //
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
#include "../globals.h"

#include "../ff7.h"

void engine_load_menu_graphics_objects_6C1468_jp(int a1)
{
  unsigned int v1; // eax
  unsigned int v2; // eax
  unsigned int v3; // ecx
  unsigned int v4; // ecx
  unsigned int v5; // eax
  unsigned int v6; // ecx
  char *menu_win_texture_path; // [esp+0h] [ebp-A4h]
  char *menu_font_texture_path; // [esp+4h] [ebp-A0h]
  char *battle_menu_win_d_texture_path; // [esp+8h] [ebp-9Ch]
  char *battle_menu_win_c_texture_path; // [esp+Ch] [ebp-98h]
  char *battle_menu_win_b_texture_path; // [esp+10h] [ebp-94h]
  char *battle_menu_win_a_texture_path; // [esp+14h] [ebp-90h]
  char *menu_font_b_graphics_object; // [esp+18h] [ebp-8Ch]
  char *menu_font_a_texture_path; // [esp+1Ch] [ebp-88h]
  struc_3 a2; // [esp+24h] [ebp-80h] BYREF
  int viewport_type_404D80; // [esp+98h] [ebp-Ch]
  ff7_game_obj *game_object_676578; // [esp+9Ch] [ebp-8h]
  int v18; // [esp+A0h] [ebp-4h]

  viewport_type_404D80 = ff7_externals.engine_get_viewport_type_404D80();
  game_object_676578 = ff7_externals.engine_get_game_object_676578();
  if ( viewport_type_404D80 == 2 )
  {
    ff7_externals.sub_671082(ff7_externals.menu_font_a_graphics_object_DC100C);
    ff7_externals.sub_671082(ff7_externals.menu_font_b_graphics_object_DC1010);
    ff7_externals.sub_671082(ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8);
    ff7_externals.sub_671082(ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC);
    ff7_externals.sub_671082(ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0);
    ff7_externals.sub_671082(ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4);
    ff7_externals.sub_671082(ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC);
    ff7_externals.sub_671082(ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0);
    ff7_externals.sub_671082(ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4);
    ff7_externals.sub_671082(ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8);
    ff7_externals.sub_671082(ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC);

    // jp
    ff7_externals.sub_671082(&ff7_externals.menu_jafont_1_graphics_object);    
  }
  else
  {
    ff7_externals.sub_671082(ff7_externals.menu_font_blend_4_graphics_object_DC1048);
    ff7_externals.sub_671082(ff7_externals.menu_win_blend_4_graphics_object_DC104C);
    ff7_externals.sub_671082(ff7_externals.menu_win_blend_0_graphics_object_DC1050);
    ff7_externals.sub_671082(ff7_externals.menu_win_blend_1_graphics_object_DC1054);
  }
  ff7_externals.sub_671082(ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8);
  v18 = ff7_externals.sub_674530();
  ff7_externals.sub_67453A(0);
  ff7_externals.make_struc3_6745E6(4, &a2);
  a2.file_context.use_lgp = 1;
  a2.file_context.lgp_num = 4;
  a2.file_context.name_mangler = 0;
  a2.base_directory = (unsigned int)ff7_externals.unk_DC1074;
  a2.field_0 |= 0x10u;
  a2.field_50 |= 1u;
  v1 = a2.field_70;
  //LOBYTE(v1) = LOBYTE(a2.field_70) | 0x20;
  v2 = MAKEWORD(LOBYTE(a2.field_70) | 0x20, HIWORD(v1));
  a2.field_70 = v1;
  if ( viewport_type_404D80 == 2 )
  {
    // Load Japanese font textures
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_1_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(
                                           1,
                                           12,
                                           &a2,
                                           "jafont_1.tim",
                                           (int)game_object_676578->dx_sfx_something);
                                               ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_2_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(
                                           1,
                                           12,
                                           &a2,
                                           "jafont_2.tim",
                                           (int)game_object_676578->dx_sfx_something);
                                               ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_3_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(
                                           1,
                                           12,
                                           &a2,
                                           "jafont_3.tim",
                                           (int)game_object_676578->dx_sfx_something);
                                               ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_4_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(
                                           1,
                                           12,
                                           &a2,
                                           "jafont_4.tim",
                                           (int)game_object_676578->dx_sfx_something);
                                               ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_5_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(
                                           1,
                                           12,
                                           &a2,
                                           "jafont_5.tim",
                                           (int)game_object_676578->dx_sfx_something);
                                               ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    ff7_externals.menu_jafont_6_graphics_object = ff7_externals.engine_load_graphics_object_6710AC(
                                           1,
                                           12,
                                           &a2,
                                           "jafont_6.tim",
                                           (int)game_object_676578->dx_sfx_something);


    ff7_externals.engine_set_blendmode_674659(4, &a2);
    if ( a1 )
      menu_font_a_texture_path = ff7_externals.aUsfont_a_h_tim;
    else
      menu_font_a_texture_path = ff7_externals.aUsfont_a_l_tim;


      
    *ff7_externals.menu_font_a_graphics_object_DC100C = ff7_externals.engine_load_graphics_object_6710AC(
                                           1,
                                           12,
                                           &a2,
                                           menu_font_a_texture_path,
                                           (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    if ( a1 )
      menu_font_b_graphics_object = ff7_externals.aUsfont_b_h_tim;
    else
      menu_font_b_graphics_object = ff7_externals.aUsfont_b_l_tim;
    *ff7_externals.menu_font_b_graphics_object_DC1010 = ff7_externals.engine_load_graphics_object_6710AC(
                                           1,
                                           12,
                                           &a2,
                                           menu_font_b_graphics_object,
                                           (int)game_object_676578->dx_sfx_something);
    if ( a1 )
      battle_menu_win_a_texture_path = ff7_externals.aBtl_win_a_h_ti;
    else
      battle_menu_win_a_texture_path = ff7_externals.aBtl_win_a_l_ti;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8 = ff7_externals.engine_load_graphics_object_6710AC(
                                                  1,
                                                  12,
                                                  &a2,
                                                  battle_menu_win_a_texture_path,
                                                  (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(0, &a2);
    *ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC = ff7_externals.engine_load_graphics_object_6710AC(
                                                  1,
                                                  12,
                                                  &a2,
                                                  battle_menu_win_a_texture_path,
                                                  (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0 = ff7_externals.engine_load_graphics_object_6710AC(
                                                  1,
                                                  12,
                                                  &a2,
                                                  battle_menu_win_a_texture_path,
                                                  (int)game_object_676578->dx_sfx_something);
    if ( a1 )
      battle_menu_win_b_texture_path = ff7_externals.aBtl_win_b_h_ti;
    else
      battle_menu_win_b_texture_path = ff7_externals.aBtl_win_b_l_ti;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC = ff7_externals.engine_load_graphics_object_6710AC(
                                                  1,
                                                  12,
                                                  &a2,
                                                  battle_menu_win_b_texture_path,
                                                  (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4 = ff7_externals.engine_load_graphics_object_6710AC(
                                                  1,
                                                  12,
                                                  &a2,
                                                  battle_menu_win_b_texture_path,
                                                  (int)game_object_676578->dx_sfx_something);
    if ( a1 )
      battle_menu_win_c_texture_path = ff7_externals.aBtl_win_c_h_ti;
    else
      battle_menu_win_c_texture_path = ff7_externals.aBtl_win_c_l_ti;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0 = ff7_externals.engine_load_graphics_object_6710AC(
                                                  1,
                                                  12,
                                                  &a2,
                                                  battle_menu_win_c_texture_path,
                                                  (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8 = ff7_externals.engine_load_graphics_object_6710AC(
                                                  1,
                                                  12,
                                                  &a2,
                                                  battle_menu_win_c_texture_path,
                                                  (int)game_object_676578->dx_sfx_something);
    v2 = a2.field_70;
    //LOBYTE(v2) = a2.field_70 & 0xDF;
    v2 = MAKEWORD(a2.field_70 & 0xDF, HIWORD(v2));
    v3 = v2;
    //LOBYTE(v3) = a2.field_70 & 0x5F | 0x80;
    v3 = MAKEWORD(a2.field_70 & 0x5F | 0x80, HIWORD(v3));
    a2.field_70 = v3;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8 = ff7_externals.engine_load_graphics_object_6710AC(
                                                       1,
                                                       12,
                                                       &a2,
                                                       battle_menu_win_c_texture_path,
                                                       (int)game_object_676578->dx_sfx_something);
    v4 = a2.field_70;
    //LOBYTE(v4) = a2.field_70 & 0x7F;
    v4 = MAKEWORD(a2.field_70 & 0x7F, HIWORD(v4));
    a2.field_70 = v4 | 0x20;
    if ( a1 )
      battle_menu_win_d_texture_path = ff7_externals.aBtl_win_d_h_ti;
    else
      battle_menu_win_d_texture_path = ff7_externals.aBtl_win_d_l_ti;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4 = ff7_externals.engine_load_graphics_object_6710AC(
                                                  1,
                                                  12,
                                                  &a2,
                                                  battle_menu_win_d_texture_path,
                                                  (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC = ff7_externals.engine_load_graphics_object_6710AC(
                                                  1,
                                                  12,
                                                  &a2,
                                                  battle_menu_win_d_texture_path,
                                                  (int)game_object_676578->dx_sfx_something);
  }
  else
  {
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    if ( a1 )
      menu_font_texture_path = ff7_externals.aUsfont_h_tim;
    else
      menu_font_texture_path = ff7_externals.aUsfont_l_tim;
    *ff7_externals.menu_font_blend_4_graphics_object_DC1048 = ff7_externals.engine_load_graphics_object_6710AC(
                                                 1,
                                                 12,
                                                 &a2,
                                                 menu_font_texture_path,
                                                 (int)game_object_676578->dx_sfx_something);
    if ( a1 )
      menu_win_texture_path = ff7_externals.aBtl_win_h_tim;
    else
      menu_win_texture_path = ff7_externals.aBtl_win_l_tim;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_blend_4_graphics_object_DC104C = ff7_externals.engine_load_graphics_object_6710AC(
                                                1,
                                                12,
                                                &a2,
                                                menu_win_texture_path,
                                                (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(0, &a2);
    *ff7_externals.menu_win_blend_0_graphics_object_DC1050 = ff7_externals.engine_load_graphics_object_6710AC(
                                                1,
                                                12,
                                                &a2,
                                                menu_win_texture_path,
                                                (int)game_object_676578->dx_sfx_something);
    ff7_externals.engine_set_blendmode_674659(1, &a2);
    *ff7_externals.menu_win_blend_1_graphics_object_DC1054 = ff7_externals.engine_load_graphics_object_6710AC(
                                                1,
                                                12,
                                                &a2,
                                                menu_win_texture_path,
                                                (int)game_object_676578->dx_sfx_something);
    v5 = a2.field_70;
    //LOBYTE(v5) = a2.field_70 & 0xDF;
    v5 = MAKEWORD(a2.field_70 & 0xDF, HIWORD(v5));
    v6 = v5;
    //LOBYTE(v6) = a2.field_70 & 0x5F | 0x80;
    v6 = MAKEWORD(a2.field_70 & 0x5F | 0x80, HIWORD(v6));
    a2.field_70 = v6;
    ff7_externals.engine_set_blendmode_674659(4, &a2);
    *ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8 = ff7_externals.engine_load_graphics_object_6710AC(
                                                       1,
                                                       12,
                                                       &a2,
                                                       menu_win_texture_path,
                                                       (int)game_object_676578->dx_sfx_something);
  }
  ff7_externals.sub_67453A(v18);
}

int charWidthData[6][256] =
{
    { // Jap - 0
        30, 30, 28, 31, 30, 30, 29, 29, 30, 30, 29, 30, 31, 30, 29, 27,
        30, 29, 29, 29, 31, 30, 28, 23, 30, 30, 30, 31, 29, 31, 30, 30,
        31, 30, 30, 31, 31, 29, 21, 28, 29, 30, 30, 27, 31, 30, 30, 29,
        29, 30, 30, 22, 22, 22, 22, 23, 22, 22, 22, 22, 22, 51, 62, 16,
        28, 29, 24, 30, 26, 29, 29, 29, 28, 29, 26, 29, 29, 28, 25, 23,
        28, 28, 25, 25, 30, 28, 28, 23, 27, 29, 28, 30, 25, 28, 26, 28,
        29, 28, 26, 28, 29, 28, 20, 25, 25, 24, 28, 28, 24, 27, 28, 28,
        28, 29, 29, 29, 29, 27, 23, 30, 29, 31, 24, 29, 27, 27, 25, 30,
        27, 29, 26, 28, 29, 27, 27, 24, 21, 22, 30, 27, 25, 31, 25, 26,
        29, 29, 29, 28, 25, 27, 25, 30, 26, 29, 25, 27, 23, 24, 24, 25,
        24, 24, 21, 23, 24, 23, 21, 23, 22, 20, 24, 24, 24, 25, 11, 21,
        29, 14, 8, 23, 24, 21, 24, 24, 20, 19, 25, 22, 14, 16, 22, 18,
        27, 22, 26, 21, 27, 22, 21, 24, 22, 24, 31, 24, 23, 23, 14, 22,
        28, 27, 27, 29, 30, 12, 25, 22, 11, 0, 27, 23, 23, 23, 12, 22,
        11, 23, 23, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    },{ // Jap - 1
        31, 31, 31, 31, 31, 30, 31, 31, 30, 31, 31, 31, 31, 30, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 28,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31,
        30, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 29, 31, 31, 31,
        30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31,
        31, 31, 30, 31, 31, 31, 29, 31, 31, 31, 31, 31, 30, 30, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 29, 31, 31, 31, 29,
        31, 30, 31, 30, 31, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 30, 31,
        31, 31, 31, 30, 30, 31, 30, 31, 30, 31, 31, 30, 31, 31, 31, 30,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 30, 31, 31, 28, 30, 31, 31, 31, 31, 27,
        31, 30, 28, 31, 31, 29, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        12, 12, 8, 8, 8, 22, 8, 15, 13, 27, 32, 17, 27, 17, 30, 0,
        0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,
    },{ // Jap - 2
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 30, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 31, 30,
        31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 30, 30, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 30, 31, 31, 29, 31, 31,
        31, 31, 31, 29, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 30, 31,
        31, 31, 29, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        30, 31, 31, 31, 28, 30, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31,
        31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 29, 31, 31, 31, 31, 31, 31, 31, 31, 31, 29,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 29, 31, 31,
        31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 29, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 29, 31, 31, 31, 31, 31, 31, 31, 31, 29, 31, 31,
        31, 31, 30, 31, 30, 31, 30, 29, 30, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31,
    },{ // Jap - 3
        29, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31,
        31, 31, 31, 31, 31, 31, 30, 30, 31, 30, 31, 31, 31, 31, 31, 31,
        28, 31, 31, 28, 31, 31, 31, 31, 31, 29, 31, 31, 31, 31, 31, 31,
        31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 30, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 30, 30, 31, 31, 30, 31, 31, 29, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 30, 31, 30, 31, 31, 31, 31, 30, 31, 31, 30, 31,
        31, 31, 31, 31, 31, 31, 31, 30, 30, 31, 31, 31, 31, 30, 31, 30,
        31, 29, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        19, 20, 19, 20, 19, 17, 19, 18, 7, 12, 18, 10, 25, 18, 20, 20,
        20, 14, 17, 16, 18, 19, 27, 18, 19, 17, 0, 0, 0, 0, 0, 0,
    },{ // Jap - 4
        31, 31, 31, 31, 31, 30, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31,
        29, 31, 31, 30, 31, 31, 31, 29, 31, 30, 31, 31, 31, 30, 31, 30,
        31, 30, 31, 31, 31, 31, 31, 31, 31, 30, 29, 30, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31,
        31, 30, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 30, 29, 31,
        31, 31, 31, 31, 30, 31, 31, 31, 31, 30, 30, 31, 31, 31, 31, 31,
        30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30,
        31, 31, 31, 31, 31, 31, 30, 29, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 30, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 31, 31, 31, 31, 31, 31,
        30, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },{ // Jap - 5
        31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 30,
        31, 31, 31, 30, 31, 31, 31, 30, 30, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 30, 30, 31, 31, 31, 31, 31, 31, 30, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
        31, 31, 31, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 29, 31,
        31, 31, 31, 31, 31, 31, 31, 30, 31, 30, 31, 31, 31, 30, 31, 31,
        31, 31, 31, 29, 31, 30, 31, 31, 31, 30, 31, 31, 31, 29, 31, 31,
        31, 31, 31, 30, 31, 30, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 30, 31, 30, 31, 31, 31, 31, 31, 30, 31, 31, 31, 31, 31,
        31, 31, 31, 29, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 31,
        31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30,
        30, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    }
};

bgra_byte get_character_color(int n_shapes)
{
  bgra_byte color = { 255, 255, 255, 255 };
  switch (n_shapes)
  {
    case 0:
      color = { 106, 106, 106, 255 };
    break;
    case 1:
      color = { 189, 98, 7, 255 };
    break;
    case 2:
      color = { 10, 0, 189, 255 };
    break;
    case 3:
      color = { 230, 10, 230, 255 };
    break;
    case 4:
      color = { 124, 230, 90, 255 };
    break;
    case 5:
      color = { 230, 230, 10, 255 };
    break;
    case 6:
      color = { 10, 230, 230, 255 };
    break;
    case 7:
      color = { 230, 230, 230, 255 };
    break;
  }

  return color;
}

/////////////////////////////////////////////////////////////////////
__int16 field_submit_draw_text_640x480_6E706D_jp(
        __int16 character_x,
        __int16 character_y,
        __int16 text_box_right_position,
        byte *buffer_text,
        float z_value)
{
  int special_character_do_draw; // eax
  graphics_vertex *window_vertices; // eax
  int character_do_draw; // eax
  graphics_vertex *character_bottom_right; // [esp+64h] [ebp-6Ch]
  graphics_vertex *character_top_right; // [esp+68h] [ebp-68h]
  graphics_vertex *character_bottom_left; // [esp+6Ch] [ebp-64h]
  graphics_vertex *character_top_left; // [esp+70h] [ebp-60h]
  graphics_vertex *special_character_top_right; // [esp+78h] [ebp-58h]
  graphics_vertex *special_character_bottom_left; // [esp+7Ch] [ebp-54h]
  graphics_vertex *special_character_top_left; // [esp+80h] [ebp-50h]
  __int16 offset_character_x; // [esp+84h] [ebp-4Ch]
  float character_u_width; // [esp+90h] [ebp-40h]
  __int16 current_character; // [esp+94h] [ebp-3Ch]
  float character_v; // [esp+98h] [ebp-38h]
  __int16 character_n_shapes; // [esp+9Ch] [ebp-34h]
  float special_character_u; // [esp+A4h] [ebp-2Ch]
  float character_u; // [esp+A4h] [ebp-2Ch]
  __int16 character; // [esp+A8h] [ebp-28h]
  __int16 i; // [esp+ACh] [ebp-24h]
  ff7_graphics_object *graphics_object; // [esp+B0h] [ebp-20h]
  __int16 text_offset_spacing; // [esp+B4h] [ebp-1Ch]
  __int16 character_x_width; // [esp+B8h] [ebp-18h]
  __int16 chararacter_u_in_byte; // [esp+BCh] [ebp-14h]
  __int16 graphics_object_v_in_byte; // [esp+C0h] [ebp-10h]
  char character_count; // [esp+C4h] [ebp-Ch]
  __int16 offset_u_in_byte; // [esp+C8h] [ebp-8h]
  float character_u_width_in_byte; // [esp+CCh] [ebp-4h]

  bool kanjiDetected = false;
  int charWidth = 16;
  int leftPadding = 0;

  character_count = 0;
  for ( i = 0;
        i < 1024
     && (*ff7_externals.field_remaining_character_length_DC3CCC)
     && *buffer_text != 0xFF
     && *buffer_text != 0xE8
     && *buffer_text != 0xE9;
        ++i )
  {
    if ( *buffer_text == 231 )
    {
      character_x = (*ff7_externals.field_current_window_pos_x_DC3CB4) + 16;
      character_y += 32;
      ++buffer_text;
      ++ff7_externals.field_text_line_row_DC3CB8;
      ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
    }
    else
    {
      switch ( *buffer_text )
      {
        case 0xFAu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object = ff7_externals.menu_jafont_2_graphics_object;
          kanjiDetected = true;
          charWidth = charWidthData[1][*buffer_text] & 0x1F;
          leftPadding = charWidthData[1][*buffer_text] >> 5;
          continue;
        case 0xFBu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object = ff7_externals.menu_jafont_3_graphics_object;
          kanjiDetected = true;
          charWidth = charWidthData[2][*buffer_text] & 0x1F;
          leftPadding = charWidthData[2][*buffer_text] >> 5;
          continue;
        case 0xFCu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object = ff7_externals.menu_jafont_4_graphics_object;
          kanjiDetected = true;
          charWidth = charWidthData[3][*buffer_text] & 0x1F;
          leftPadding = charWidthData[3][*buffer_text] >> 5;
          continue;
        case 0xFDu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object = ff7_externals.menu_jafont_5_graphics_object;
          kanjiDetected = true;
          charWidth = charWidthData[4][*buffer_text] & 0x1F;
          leftPadding = charWidthData[4][*buffer_text] >> 5;
          continue;
        case 0xFEu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object = ff7_externals.menu_jafont_6_graphics_object;
          kanjiDetected = true;
          charWidth = charWidthData[5][*buffer_text] & 0x1F;
          leftPadding = charWidthData[5][*buffer_text] >> 5;
          continue;
        default:
          if(!kanjiDetected)
          {
            graphics_object = ff7_externals.menu_jafont_1_graphics_object;
            charWidth = charWidthData[0][*buffer_text] & 0x1F;
            leftPadding = charWidthData[0][*buffer_text] >> 5;
          }
          kanjiDetected = false;
          break;
      }

      offset_character_x = 0;
      switch ( *buffer_text )
      {
        /*case 0xFAu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object_v_in_byte = 132;
          text_offset_spacing = 231;
          goto LABEL_39;
        case 0xFBu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object_v_in_byte = 0;
          offset_character_x = 16;
          text_offset_spacing = 441;
          goto LABEL_39;
        case 0xFCu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object_v_in_byte = 132;
          offset_character_x = 16;
          text_offset_spacing = 672;
          goto LABEL_39;
        case 0xFDu:
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          ++buffer_text;
          graphics_object_v_in_byte = 132;
          text_offset_spacing = 882;
          goto LABEL_39;
        case 0xFEu:
          ++buffer_text;
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          if ( *buffer_text < 0xD2u )
          {
            graphics_object_v_in_byte = 132;
            offset_character_x = 16;
            text_offset_spacing = 1092;
            goto LABEL_39;
          }
          ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          if ( *buffer_text < 0xDAu )
          {
            (*ff7_externals.word_91F028) = *buffer_text++ - 210;
            break;
          }
          if ( *buffer_text == 218 )
          {
            (*ff7_externals.word_DC3CC0) ^= 1u;
            ++buffer_text;
            break;
          }
          if ( *buffer_text == 219 )
          {
            (*ff7_externals.word_DC3CC4) ^= 1u;
            ++buffer_text;
            break;
          }
          if ( *buffer_text != 233 )
            goto LABEL_39;
          (*ff7_externals.dword_DC3CD4) ^= 1u;
          ++buffer_text;
          break;*/
        default:
          if ( *buffer_text < 0xF6u || *buffer_text > 0xF9u )
          {
            text_offset_spacing = 0;
            graphics_object_v_in_byte = 0;
LABEL_39:
            if ( (*ff7_externals.word_DC3CC0) || (*ff7_externals.word_DC3CC4) )
            {
              if ( (*ff7_externals.word_DC3CC4) )
              {
                character_n_shapes = ((unsigned __int8)((*ff7_externals.word_DC3CC8) >> 2) - character_count) & 7;
              }
              else if ( (((*ff7_externals.word_DC3CC8) >> 2) & 1) != 0 )
              {
                character_n_shapes = (*ff7_externals.word_91F028);
              }
              else
              {
                if ( !(*ff7_externals.word_91F028) )
                {
                  character_x += offset_character_x;
                  break;
                }
                character_n_shapes = 0;
              }
            }
            else
            {
              character_n_shapes = (*ff7_externals.word_91F028);
            }
            current_character = *buffer_text;
            character = current_character;
            //if ( *buffer_text == 0xD2 || *buffer_text == 0xD3 )
              //character = current_character - 78;
            offset_u_in_byte = 32 * (character % 16);
            graphics_object_v_in_byte += 32 * (character / 16);
            /*if ( character_x
               - (*ff7_externals.field_current_window_pos_x_DC3CB4)
               + 2
               * ((*(byte *)((*ff7_externals.g_text_spacing_DB958C) + text_offset_spacing + *buffer_text) & 0x1F)
                + ((int)*(unsigned __int8 *)((*ff7_externals.g_text_spacing_DB958C) + text_offset_spacing + *buffer_text) >> 5)) > text_box_right_position )
            {
              character_x = (*ff7_externals.field_current_window_pos_x_DC3CB4) + 16;
              character_y += 32;
              ++ff7_externals.field_text_line_row_DC3CB8;
            }*/
            if ( !(*ff7_externals.dword_DC3CD4) )
              character_x += leftPadding; //2
                           //* ((int)*(unsigned __int8 *)((*ff7_externals.g_text_spacing_DB958C) + text_offset_spacing + current_character) >> 5);*/
            if ( offset_u_in_byte <= 480 )
            {
              chararacter_u_in_byte = 32 * (character % 16);
              if ( offset_u_in_byte == 480 )
              {
                character_u_width_in_byte = 32.0;
                character_x_width = 16;
              }
              else
              {
                character_u_width_in_byte = 32.0;
                character_x_width = 16;
              }
              character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
            }
            /*else
            {
              chararacter_u_in_byte = offset_u_in_byte - 512;
              character_u_width_in_byte = 32.0;
              character_x_width = 32;
              graphics_object = *ff7_externals.menu_font_b_graphics_object_DC1010;
              character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)*ff7_externals.menu_font_b_graphics_object_DC1010);
            }*/
            if ( character_do_draw )
            {
              auto color = get_character_color(character_n_shapes);

              character_u = (double)chararacter_u_in_byte / 512.0;
              character_v = (double)graphics_object_v_in_byte / 512.0;
              character_u_width = character_u_width_in_byte / 512.0;
              character_top_left = graphics_object->vertex_transform;
              character_top_left->position.x = (float)character_x;
              character_top_left->position.y = (float)character_y;
              character_top_left->position.z = z_value;
              character_top_left->position.w = 1.0;
              character_top_left->color = color;
              character_top_left->alpha_mask = -16777216;
              character_top_left->u = character_u;
              character_top_left->v = character_v;
              character_bottom_left = graphics_object->vertex_transform + 1;
              character_bottom_left->position.x = (float)character_x;
              character_bottom_left->position.y = (double)character_y + 16;
              character_bottom_left->position.z = z_value;
              character_bottom_left->position.w = 1.0;
              character_bottom_left->color = color;
              character_bottom_left->alpha_mask = -16777216;
              character_bottom_left->u = character_u;
              character_bottom_left->v = character_v + 32.0f / 512.0f;
              character_top_right = graphics_object->vertex_transform + 2;
              character_top_right->position.x = (double)character_x + (double)character_x_width;
              character_top_right->position.y = (float)character_y;
              character_top_right->position.z = z_value;
              character_top_right->position.w = 1.0;
              character_top_right->color = color;
              character_top_right->alpha_mask = -16777216;
              character_top_right->u = character_u + character_u_width;
              character_top_right->v = character_v;
              character_bottom_right = graphics_object->vertex_transform + 3;
              character_bottom_right->position.x = (double)character_x + (double)character_x_width;
              character_bottom_right->position.y = (double)character_y + 16;
              character_bottom_right->position.z = z_value;
              character_bottom_right->position.w = 1.0;
              character_bottom_right->color = color;
              character_bottom_right->alpha_mask = -16777216;
              character_bottom_right->u = character_u + character_u_width;
              character_bottom_right->v = character_v + 32.0f / 512.0f;
              *(byte *)graphics_object->curr_total_n_shape = 2 * character_n_shapes;
              graphics_object->field_7C = 2 * character_n_shapes;
              (*ff7_externals.field_do_draw_character_DC3CEC) = 1;
            }
            if ( (*ff7_externals.dword_DC3CD4) )
              character_x += 26;
            else
              character_x += std::ceil(0.5f * charWidth);//2 * (*(byte *)((*ff7_externals.g_text_spacing_DB958C) + text_offset_spacing + current_character) & 0x1F);
            --(*ff7_externals.field_remaining_character_length_DC3CCC);
            ++buffer_text;
            ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
          }
          else
          {
            switch ( *buffer_text )
            {
              case 0xF6u:
                offset_u_in_byte = 192;
                graphics_object_v_in_byte = 128;
                graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8);
                break;
              case 0xF7u:
                offset_u_in_byte = 32;
                graphics_object_v_in_byte = 128;
                graphics_object = *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC;
                special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC);
                break;
              case 0xF8u:
                offset_u_in_byte = 0;
                graphics_object_v_in_byte = 128;
                graphics_object = *ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC;
                special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC);
                break;
              case 0xF9u:
                offset_u_in_byte = 224;
                graphics_object_v_in_byte = 128;
                graphics_object = *ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8;
                goto LABEL_34;
              default:
LABEL_34:
                special_character_do_draw = common_externals.draw_graphics_object(1, (struct graphics_object*)graphics_object);
                break;
            }
            if ( special_character_do_draw )
            {
              auto color = get_character_color(7);

              special_character_u = (double)offset_u_in_byte / 512.0f;
              special_character_top_left = graphics_object->vertex_transform;
              special_character_top_left->position.x = (float)character_x;
              special_character_top_left->position.y = (double)character_y - 4.0;
              special_character_top_left->position.z = z_value;
              special_character_top_left->position.w = 1.0;
              special_character_top_left->color = color;
              special_character_top_left->alpha_mask = -16777216;
              special_character_top_left->u = special_character_u;
              special_character_top_left->v = 0.5;
              special_character_bottom_left = graphics_object->vertex_transform + 1;
              special_character_bottom_left->position.x = (float)character_x;
              special_character_bottom_left->position.y = (double)character_y - 4.0 + 16.0;
              special_character_bottom_left->position.z = z_value;
              special_character_bottom_left->position.w = 1.0;
              special_character_bottom_left->color = color;
              special_character_bottom_left->alpha_mask = -16777216;
              special_character_bottom_left->u = special_character_u;
              special_character_bottom_left->v = 0.5 + 0.125;
              special_character_top_right = graphics_object->vertex_transform + 2;
              special_character_top_right->position.x = (double)character_x + 16.0;
              special_character_top_right->position.y = (double)character_y - 4.0;
              special_character_top_right->position.z = z_value;
              special_character_top_right->position.w = 1.0;
              special_character_top_right->color = color;
              special_character_top_right->alpha_mask = -16777216;
              special_character_top_right->u = special_character_u + 0.125;
              special_character_top_right->v = 0.5;
              window_vertices = graphics_object->vertex_transform;
              window_vertices[3].position.x = (double)character_x + 16.0;
              window_vertices[3].position.y = (double)character_y - 4.0 + 16.0;
              window_vertices[3].position.z = z_value;
              window_vertices[3].position.w = 1.0;
              window_vertices[3].color = color;
              window_vertices[3].alpha_mask = -16777216;
              window_vertices[3].u = special_character_u + 0.125;
              window_vertices[3].v = 0.5 + 0.125;
              *(byte *)graphics_object->curr_total_n_shape = 7;
              graphics_object->field_7C = 7;
            }
            (*ff7_externals.field_do_draw_text_boxes_DC3CE8) = 1;
            ++buffer_text;
            --(*ff7_externals.field_remaining_character_length_DC3CCC);
            ++(*ff7_externals.field_text_box_curr_n_characters_DC3CB0);
            character_x += 32;
          }
          break;
      }
    }
    ++character_count;
  }
  return character_y;
}

void field_draw_text_boxes_and_text_graphics_object_6ECA68_jp()
{
  ff7_game_obj *game_object; // [esp+0h] [ebp-4h]

  game_object = ff7_externals.engine_get_game_object_676578();
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    // 320x240 viewport not needed
  }
  else
  {
    if ( *ff7_externals.dword_DC3CE0 )
    {
      ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
      ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_window_bg_graphics_object_DC0FF0);
      *ff7_externals.dword_DC3CE0 = 0;
    }
    if ( *ff7_externals.dword_DC3CDC )
    {
      ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
      ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4);
      *ff7_externals.dword_DC3CDC = 0;
    }
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    if ( *ff7_externals.field_do_draw_character_DC3CEC )
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);      
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_font_a_graphics_object_DC100C);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_font_b_graphics_object_DC1010);

      // jp
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_1_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_2_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_3_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_4_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_5_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_6_graphics_object, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_1_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_2_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_3_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_4_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_5_graphics_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(ff7_externals.menu_jafont_6_graphics_object);

      *ff7_externals.field_do_draw_character_DC3CEC = 0;
    }
    if ( *ff7_externals.field_do_draw_text_boxes_DC3CE8 || *ff7_externals.text_box_do_draw_menu_win_c_blend_4_DC3CE4 )
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8);
      *ff7_externals.text_box_do_draw_menu_win_c_blend_4_DC3CE4 = 0;
      *ff7_externals.field_do_draw_text_boxes_DC3CE8 = 0;
    }
    if ( *ff7_externals.text_box_do_draw_black_quad_graphics_object_DC3CF0 )
    {
      ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
      ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_text_box_quad_graphics_object_DC1008, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_text_box_quad_graphics_object_DC1008);
      *ff7_externals.dword_DC3D00 = 0;
      *ff7_externals.text_box_do_draw_black_quad_graphics_object_DC3CF0 = 0;
      if ( *ff7_externals.do_draw_text_box_DC3CF8 )
        *ff7_externals.should_draw_text_box_black_quad_DC3D04 = 0;
    }
  }
}

int common_submit_draw_char_from_buffer_6F564E_jp(int x, int vertex_y, int n_shapes, unsigned __int16 letter, float z_value)
{
  graphics_vertex *bottom_right; // [esp+1Ch] [ebp-4Ch]
  graphics_vertex *top_right; // [esp+20h] [ebp-48h]
  graphics_vertex *bottom_left; // [esp+24h] [ebp-44h]
  graphics_vertex *top_left; // [esp+28h] [ebp-40h]
  float vertex_u_width; // [esp+34h] [ebp-34h]
  float vertex_v; // [esp+38h] [ebp-30h]
  float vertex_u; // [esp+40h] [ebp-28h]
  unsigned __int16 character; // [esp+44h] [ebp-24h]
  ff7_graphics_object *character_graphics_object; // [esp+48h] [ebp-20h]
  __int16 offset_text_spacing; // [esp+4Ch] [ebp-1Ch]
  __int16 vertex_width; // [esp+50h] [ebp-18h]
  __int16 image_u; // [esp+54h] [ebp-14h]
  __int16 offset_image_v; // [esp+58h] [ebp-10h]
  __int16 image_v; // [esp+58h] [ebp-10h]
  __int16 offset_image_u; // [esp+5Ch] [ebp-Ch]
  unsigned __int16 *p_letter; // [esp+60h] [ebp-8h]
  float image_u_width; // [esp+64h] [ebp-4h]
  int vertex_x; // [esp+70h] [ebp+8h]

  int charWidth = 16;
  int leftPadding = 0;

  p_letter = &letter;

  switch ( (byte)letter )
  {
    case 0xF8:
      return x;
    case 0xFA:
      p_letter = (unsigned __int16 *)((byte *)&letter + 1);
      character_graphics_object = ff7_externals.menu_jafont_2_graphics_object;
      charWidth = charWidthData[1][*p_letter] & 0x1F;
      leftPadding = charWidthData[1][*p_letter] >> 5;
      //offset_image_v = 132;
      //offset_text_spacing = 231;
      goto LABEL_9;
    case 0xFB:
      p_letter = (unsigned __int16 *)((byte *)&letter + 1);
      character_graphics_object = ff7_externals.menu_jafont_3_graphics_object;
      charWidth = charWidthData[2][*p_letter] & 0x1F;
      leftPadding = charWidthData[2][*p_letter] >> 5;
      //offset_image_v = 0;
      //offset_text_spacing = 441;
      goto LABEL_9;
    case 0xFC:
      p_letter = (unsigned __int16 *)((byte *)&letter + 1);
      character_graphics_object = ff7_externals.menu_jafont_4_graphics_object;
      charWidth = charWidthData[3][*p_letter] & 0x1F;
      leftPadding = charWidthData[3][*p_letter] >> 5;
      //offset_image_v = 132;
      //offset_text_spacing = 672;
      goto LABEL_9;
    case 0xFD:
      p_letter = (unsigned __int16 *)((byte *)&letter + 1);
      character_graphics_object = ff7_externals.menu_jafont_5_graphics_object;
      charWidth = charWidthData[4][*p_letter] & 0x1F;
      leftPadding = charWidthData[4][*p_letter] >> 5;
      //offset_image_v = 132;
      //offset_text_spacing = 882;
      goto LABEL_9;
    case 0xFE:
      p_letter = (unsigned __int16 *)((byte *)&letter + 1);
      character_graphics_object = ff7_externals.menu_jafont_6_graphics_object;
      charWidth = charWidthData[5][*p_letter] & 0x1F;
      leftPadding = charWidthData[5][*p_letter] >> 5;
      //offset_image_v = 132;
      //offset_text_spacing = 1092;
      goto LABEL_9;
    default:
      character_graphics_object = ff7_externals.menu_jafont_1_graphics_object;
      charWidth = charWidthData[0][*p_letter] & 0x1F;
      leftPadding = charWidthData[0][*p_letter] >> 5;
      break;
  }

  switch ( (byte)letter )
  {
    /*case 0xF8:
      return x;
    case 0xFA:
      p_letter = (unsigned __int16 *)((char *)&letter + 1);
      offset_image_v = 132;
      offset_text_spacing = 231;
      goto LABEL_9;
    case 0xFB:
      p_letter = (unsigned __int16 *)((char *)&letter + 1);
      offset_image_v = 0;
      offset_text_spacing = 441;
      goto LABEL_9;
    case 0xFC:
      p_letter = (unsigned __int16 *)((char *)&letter + 1);
      offset_image_v = 132;
      offset_text_spacing = 672;
      goto LABEL_9;
    case 0xFD:
      p_letter = (unsigned __int16 *)((char *)&letter + 1);
      offset_image_v = 132;
      offset_text_spacing = 882;
      goto LABEL_9;
    case 0xFE:
      p_letter = (unsigned __int16 *)((char *)&letter + 1);
      offset_image_v = 132;
      offset_text_spacing = 1092;
      goto LABEL_9;*/
    default:
      offset_text_spacing = 0;
      offset_image_v = 0;
LABEL_9:
      offset_text_spacing = 0;
      offset_image_v = 0;
      letter = *(byte *)p_letter;
      //character = *(byte *)p_letter;
      //if ( *(byte *)p_letter == 0xD2 || *(byte *)p_letter == 0xD3 )
      //  character -= 0x4E;
      offset_image_u = 32 * (letter % 16);
      image_v = 32 * (letter / 16) + offset_image_v;
      image_u = 32 * (letter % 16);
      if ( offset_image_u <= 480 )
      {
        //image_u = 24 * (character % 21);
        if ( offset_image_u == 480 )
        {
          image_u_width = 32.0;
          vertex_width = 16;
        }
        else
        {
          image_u_width = 32.0;
          vertex_width = 16;
        }
        //character_graphics_object = *ff7_externals.menu_font_a_graphics_object_DC100C;
      }
      else
      {
        //image_u = offset_image_u - 256;
        image_u_width = 32.0;
        vertex_width = 16;
        //character_graphics_object = *ff7_externals.menu_font_b_graphics_object_DC1010;
      }
      /*if ( *ff7_externals.dword_DC12DC )
        vertex_x = (__int64)((double)((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + offset_text_spacing + letter) >> 5)
                           * 1.6666666)
                 + x;
      else*/
        vertex_x = x + leftPadding;//2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + offset_text_spacing + letter) >> 5);
      if ( ff7_externals.g_get_do_render_menu_6CDBF2() && common_externals.draw_graphics_object(1, (struct graphics_object*)character_graphics_object) )
      {
        auto color = get_character_color(n_shapes);

        vertex_u = (double)image_u / 512.0f;
        vertex_v = (double)image_v / 512.0f;
        vertex_u_width = image_u_width / 512.0f;
        top_left = character_graphics_object->vertex_transform;
        top_left->position.x = (float)vertex_x;
        top_left->position.y = (float)vertex_y;
        top_left->position.z = z_value;
        top_left->position.w = 1.0;
        top_left->color = color;
        top_left->alpha_mask = 0xFF000000;
        top_left->u = vertex_u;
        top_left->v = vertex_v;
        bottom_left = character_graphics_object->vertex_transform + 1;
        bottom_left->position.x = (float)vertex_x;
        bottom_left->position.y = (double)vertex_y + 16.0;
        bottom_left->position.z = z_value;
        bottom_left->position.w = 1.0;
        bottom_left->color = color;
        bottom_left->alpha_mask = 0xFF000000;
        bottom_left->u = vertex_u;
        bottom_left->v = vertex_v + 32.0f / 512.0f;
        top_right = character_graphics_object->vertex_transform + 2;
        top_right->position.x = (double)vertex_x + (double)vertex_width;
        top_right->position.y = (float)vertex_y;
        top_right->position.z = z_value;
        top_right->position.w = 1.0;
        top_right->color = color;
        top_right->alpha_mask = 0xFF000000;
        top_right->u = vertex_u + vertex_u_width;
        top_right->v = vertex_v;
        bottom_right = character_graphics_object->vertex_transform + 3;
        bottom_right->position.x = (double)vertex_x + (double)vertex_width;
        bottom_right->position.y = (double)vertex_y + 16.0;
        bottom_right->position.z = z_value;
        bottom_right->position.w = 1.0;
        bottom_right->color = color;
        bottom_right->alpha_mask = -16777216;
        bottom_right->u = vertex_u + vertex_u_width;
        bottom_right->v = vertex_v + 32.0f / 512.0f;
        *(byte *)character_graphics_object->curr_total_n_shape = 2 * n_shapes;
        character_graphics_object->field_7C = 2 * n_shapes;
      }
      /*if ( *ff7_externals.dword_DC12DC )                       // Return next x position: basically text spacing
        return vertex_x + std::ceil(0.5f * charWidth) * 1.6666666;//(__int64)((double)(*(byte *)(*ff7_externals.g_text_spacing_DB958C + offset_text_spacing + letter) & 0x1F) * 1.6666666)
             //+ vertex_x;
      else*/
        return vertex_x + std::ceil(0.5f * charWidth);// 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + offset_text_spacing + letter) & 0x1F);
  }
}

void menu_draw_everything_6CC9D3_jp()
{
  ff7_game_obj *game_object; // [esp+0h] [ebp-4h]

  if ( ff7_externals.g_get_do_render_menu_6CDBF2() )
  {
    game_object = ff7_externals.engine_get_game_object_676578();
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_gfx_draw_graphics_object_polygon_set_field_80_sub_660E6A(*ff7_externals.menu_unknown3_graphics_object_DC0FFC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
    if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_4_graphics_object_DC104C, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_blend_4_graphics_object_DC1048, game_object);
    }
    else
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);
      
      // jp
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_1_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_2_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_3_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_4_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_5_graphics_object, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_6_graphics_object, game_object);
    }
    if ( *ff7_externals.dword_DC12EC == 9 || *ff7_externals.dword_DC12E4 )
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_1_graphics_object_DC1020, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_2_graphics_object_DC1024, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_3_graphics_object_DC1028, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_4_graphics_object_DC102C, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_5_graphics_object_DC1030, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_6_graphics_object_DC1034, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_7_graphics_object_DC1038, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_8_graphics_object_DC103C, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar2_9_graphics_object_DC1040, game_object);
    }
    else
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar_1_graphics_object_DC1014, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar_2_graphics_object_DC1018, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_avatar_3_graphics_object_DC101C, game_object);
    }
    if ( *ff7_externals.engine_game_mode_word_CBF9DC == 20 )
    {
      ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 1, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_buster_tex_graphics_object_DC1044, game_object);
      ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_buster_tex_graphics_object_DC1044);
    }
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown4_graphics_object_DC1000, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown2_graphics_object_DC0FF8, game_object);
    if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_0_graphics_object_DC1050, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_1_graphics_object_DC1054, game_object);
    }
    else
    {
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4, game_object);
      ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC, game_object);
    }
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(9, 1, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_text_box_quad_graphics_object_DC1008, game_object);
    ff7_externals.engine_gfx_setviewport_sub_66067A(
      *ff7_externals.menu_viewport_x_DC105C,
      *ff7_externals.menu_viewport_y_DC1060,
      *ff7_externals.menu_viewport_width_DC1064,
      *ff7_externals.menu_viewport_view_DC1068,
      game_object);
  }
}

void battle_draw_menu_everything_6CEE84_jp()
{
  ff7_game_obj *game_object; // [esp+0h] [ebp-4h]

  game_object = ff7_externals.engine_get_game_object_676578();
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_gfx_draw_graphics_object_polygon_set_field_80_sub_660E6A(*ff7_externals.menu_unknown3_graphics_object_DC0FFC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_4_graphics_object_DC104C, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_blend_4_graphics_object_DC1048, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown4_graphics_object_DC1000, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown5_graphics_object_DC1004, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown2_graphics_object_DC0FF8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_0_graphics_object_DC1050, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_1_graphics_object_DC1054, game_object);
  }
  else
  {
    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_gfx_draw_graphics_object_polygon_set_field_80_sub_660E6A(*ff7_externals.menu_unknown3_graphics_object_DC0FFC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);

    // jp
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_1_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_2_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_3_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_4_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_5_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_6_graphics_object, game_object);

    ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
    ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown4_graphics_object_DC1000, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown5_graphics_object_DC1004, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_unknown2_graphics_object_DC0FF8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_1_graphics_object_DC0FE0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_1_graphics_object_DC0FE8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_1_graphics_object_DC0FE4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_1_graphics_object_DC0FEC, game_object);
  }
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_text_box_quad_graphics_object_DC1008, game_object);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_unknown4_graphics_object_DC1000);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_unknown5_graphics_object_DC1004);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_blend_window_bg_graphics_object_DC0FF4);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_unknown2_graphics_object_DC0FF8);
  ff7_externals.reset_field_54_graphics_object_66E62C(*ff7_externals.menu_text_box_quad_graphics_object_DC1008);
}

void draw_text_top_display_6D1CC0_jp(int a1, __int16 menu_box_idx, char a3, unsigned __int16 a4)
{
  __int64 v4; // rax
  __int64 menu_width; // rax
  graphics_vertex *v6; // [esp+1A8h] [ebp-304h]
  graphics_vertex *v7; // [esp+1ACh] [ebp-300h]
  graphics_vertex *v8; // [esp+1B0h] [ebp-2FCh]
  graphics_vertex *v9; // [esp+1B4h] [ebp-2F8h]
  graphics_vertex *v10; // [esp+1B8h] [ebp-2F4h]
  graphics_vertex *v11; // [esp+1BCh] [ebp-2F0h]
  graphics_vertex *v12; // [esp+1C0h] [ebp-2ECh]
  graphics_vertex *v13; // [esp+1C4h] [ebp-2E8h]
  graphics_vertex *v14; // [esp+1C8h] [ebp-2E4h]
  graphics_vertex *v15; // [esp+1CCh] [ebp-2E0h]
  graphics_vertex *v16; // [esp+1D0h] [ebp-2DCh]
  graphics_vertex *v17; // [esp+1D4h] [ebp-2D8h]
  graphics_vertex *v18; // [esp+1D8h] [ebp-2D4h]
  graphics_vertex *v19; // [esp+1DCh] [ebp-2D0h]
  graphics_vertex *v20; // [esp+1E0h] [ebp-2CCh]
  graphics_vertex *v21; // [esp+1E4h] [ebp-2C8h]
  graphics_vertex *v22; // [esp+1E8h] [ebp-2C4h]
  graphics_vertex *v23; // [esp+1ECh] [ebp-2C0h]
  graphics_vertex *v24; // [esp+1F0h] [ebp-2BCh]
  graphics_vertex *v25; // [esp+1F4h] [ebp-2B8h]
  graphics_vertex *v26; // [esp+1F8h] [ebp-2B4h]
  graphics_vertex *v27; // [esp+1FCh] [ebp-2B0h]
  graphics_vertex *v28; // [esp+200h] [ebp-2ACh]
  graphics_vertex *v29; // [esp+204h] [ebp-2A8h]
  graphics_vertex *v30; // [esp+208h] [ebp-2A4h]
  graphics_vertex *v31; // [esp+20Ch] [ebp-2A0h]
  graphics_vertex *v32; // [esp+210h] [ebp-29Ch]
  graphics_vertex *v33; // [esp+214h] [ebp-298h]
  graphics_vertex *v34; // [esp+218h] [ebp-294h]
  graphics_vertex *v35; // [esp+21Ch] [ebp-290h]
  graphics_vertex *v36; // [esp+220h] [ebp-28Ch]
  graphics_vertex *v37; // [esp+224h] [ebp-288h]
  graphics_vertex *v38; // [esp+228h] [ebp-284h]
  graphics_vertex *v39; // [esp+22Ch] [ebp-280h]
  graphics_vertex *v40; // [esp+230h] [ebp-27Ch]
  graphics_vertex *v41; // [esp+234h] [ebp-278h]
  graphics_vertex *v42; // [esp+238h] [ebp-274h]
  graphics_vertex *v43; // [esp+23Ch] [ebp-270h]
  graphics_vertex *v44; // [esp+240h] [ebp-26Ch]
  graphics_vertex *v45; // [esp+244h] [ebp-268h]
  graphics_vertex *v46; // [esp+248h] [ebp-264h]
  graphics_vertex *v47; // [esp+24Ch] [ebp-260h]
  graphics_vertex *v48; // [esp+250h] [ebp-25Ch]
  graphics_vertex *v49; // [esp+254h] [ebp-258h]
  graphics_vertex *v50; // [esp+258h] [ebp-254h]
  graphics_vertex *v51; // [esp+25Ch] [ebp-250h]
  graphics_vertex *v52; // [esp+260h] [ebp-24Ch]
  graphics_vertex *v53; // [esp+264h] [ebp-248h]
  graphics_vertex *v54; // [esp+268h] [ebp-244h]
  graphics_vertex *v55; // [esp+26Ch] [ebp-240h]
  graphics_vertex *v56; // [esp+270h] [ebp-23Ch]
  graphics_vertex *v57; // [esp+274h] [ebp-238h]
  graphics_vertex *v58; // [esp+278h] [ebp-234h]
  graphics_vertex *v59; // [esp+27Ch] [ebp-230h]
  graphics_vertex *v60; // [esp+280h] [ebp-22Ch]
  graphics_vertex *v61; // [esp+284h] [ebp-228h]
  graphics_vertex *v62; // [esp+288h] [ebp-224h]
  graphics_vertex *v63; // [esp+28Ch] [ebp-220h]
  graphics_vertex *v64; // [esp+290h] [ebp-21Ch]
  graphics_vertex *v65; // [esp+294h] [ebp-218h]
  graphics_vertex *v66; // [esp+298h] [ebp-214h]
  graphics_vertex *v67; // [esp+29Ch] [ebp-210h]
  graphics_vertex *v68; // [esp+2A0h] [ebp-20Ch]
  graphics_vertex *v69; // [esp+2A4h] [ebp-208h]
  graphics_vertex *v70; // [esp+2A8h] [ebp-204h]
  graphics_vertex *v71; // [esp+2ACh] [ebp-200h]
  graphics_vertex *v72; // [esp+2B0h] [ebp-1FCh]
  graphics_vertex *v73; // [esp+2B4h] [ebp-1F8h]
  graphics_vertex *v74; // [esp+2B8h] [ebp-1F4h]
  graphics_vertex *v75; // [esp+2BCh] [ebp-1F0h]
  graphics_vertex *v76; // [esp+2C0h] [ebp-1ECh]
  graphics_vertex *v77; // [esp+2C4h] [ebp-1E8h]
  graphics_vertex *v78; // [esp+2C8h] [ebp-1E4h]
  graphics_vertex *v79; // [esp+2CCh] [ebp-1E0h]
  graphics_vertex *v80; // [esp+2D0h] [ebp-1DCh]
  graphics_vertex *v81; // [esp+2D4h] [ebp-1D8h]
  graphics_vertex *v82; // [esp+2D8h] [ebp-1D4h]
  graphics_vertex *v83; // [esp+2DCh] [ebp-1D0h]
  graphics_vertex *v84; // [esp+2E0h] [ebp-1CCh]
  graphics_vertex *v85; // [esp+2E4h] [ebp-1C8h]
  graphics_vertex *v86; // [esp+2E8h] [ebp-1C4h]
  graphics_vertex *v87; // [esp+2ECh] [ebp-1C0h]
  graphics_vertex *v88; // [esp+2F0h] [ebp-1BCh]
  graphics_vertex *vertex_transform; // [esp+2F4h] [ebp-1B8h]
  int v90; // [esp+2F8h] [ebp-1B4h]
  graphics_vertex *v91; // [esp+2FCh] [ebp-1B0h]
  graphics_vertex *v92; // [esp+300h] [ebp-1ACh]
  graphics_vertex *v93; // [esp+304h] [ebp-1A8h]
  graphics_vertex *v94; // [esp+308h] [ebp-1A4h]
  int v95; // [esp+30Ch] [ebp-1A0h]
  __int16 v96; // [esp+314h] [ebp-198h]
  __int16 v97; // [esp+314h] [ebp-198h]
  float v98; // [esp+320h] [ebp-18Ch]
  float v99; // [esp+32Ch] [ebp-180h]
  float v100; // [esp+32Ch] [ebp-180h]
  int offset_y; // [esp+330h] [ebp-17Ch]
  float v102; // [esp+334h] [ebp-178h]
  float v103; // [esp+334h] [ebp-178h]
  int offset_x; // [esp+338h] [ebp-174h]
  __int16 v105; // [esp+350h] [ebp-15Ch]
  __int16 v106; // [esp+354h] [ebp-158h]
  __int16 v107; // [esp+354h] [ebp-158h]
  __int16 v108; // [esp+354h] [ebp-158h]
  __int16 v109; // [esp+354h] [ebp-158h]
  __int16 v110; // [esp+354h] [ebp-158h]
  __int16 v111; // [esp+354h] [ebp-158h]
  __int16 menu_height; // [esp+358h] [ebp-154h]
  __int16 v113; // [esp+358h] [ebp-154h]
  __int16 v114; // [esp+358h] [ebp-154h]
  __int16 v115; // [esp+35Ch] [ebp-150h]
  __int16 v116; // [esp+35Ch] [ebp-150h]
  __int16 v117; // [esp+35Ch] [ebp-150h]
  char v118; // [esp+360h] [ebp-14Ch]
  __int16 j; // [esp+364h] [ebp-148h]
  __int16 v120; // [esp+364h] [ebp-148h]
  __int16 i; // [esp+364h] [ebp-148h]
  __int16 v122; // [esp+364h] [ebp-148h]
  attack_name_fixed_buffer *v123; // [esp+368h] [ebp-144h]
  attack_name_fixed_buffer *v124; // [esp+368h] [ebp-144h]
  ff7_graphics_object *a2 = nullptr; // [esp+36Ch] [ebp-140h]
  __int16 v126; // [esp+370h] [ebp-13Ch]
  __int16 v127; // [esp+380h] [ebp-12Ch]
  int v128; // [esp+384h] [ebp-128h]
  __int16 v129; // [esp+388h] [ebp-124h]
  __int16 v130; // [esp+388h] [ebp-124h]
  int v131; // [esp+490h] [ebp-1Ch]
  __int16 v132; // [esp+494h] [ebp-18h]
  __int16 v133; // [esp+494h] [ebp-18h]
  attack_name_fixed_buffer *text_sub_41963C; // [esp+498h] [ebp-14h]
  attack_name_fixed_buffer *v135; // [esp+498h] [ebp-14h]
  attack_name_fixed_buffer *v136; // [esp+498h] [ebp-14h]
  float v137; // [esp+4A8h] [ebp-4h]

  v131 = *ff7_externals.menu_viewport_x_DC105C;
  v128 = *ff7_externals.menu_viewport_y_DC1060;
  v118 = 0;
  offset_x = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].offset_x;
  offset_y = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].offset_y;
  switch ( menu_box_idx )
  {
    case 22:
      menu_box_idx = 25;
      offset_x = (*ff7_externals.battle_menu_data_DC3630)[25].offset_x;
      offset_y = (*ff7_externals.battle_menu_data_DC3630)[25].offset_y;
      switch ( a3 )
      {
        case 0:
        case 2:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(0, a4, 8);
          break;
        case 3:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(6u, a4, 8);
          break;
        case 4:
        case 8:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(4u, a4, 8);
          break;
        case 7:
          ff7_externals.sub_6D70F1(a4);
          text_sub_41963C = (attack_name_fixed_buffer *)ff7_externals.byte_DC3640;
          break;
        case 13:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(2u, a4, 8);
          break;
        case 20:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(3u, a4, 8);
          break;
        case 32:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(9u, a4, 8);
          break;
        default:
          text_sub_41963C = ff7_externals.kernel_get_text_sub_41963C(0, 0, 8);
          break;
      }
      break;
    case 23:
      text_sub_41963C = (attack_name_fixed_buffer *)*ff7_externals.battle_text_buffer_DC208C;
      break;
    case 25:
      text_sub_41963C = (attack_name_fixed_buffer *)*ff7_externals.battle_text_buffer_DC208C;
      break;
  }
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    // 320x240 viewport not needed
  }
  else
  {
    v95 = 0;
    v123 = text_sub_41963C;
    v106 = 0;
    for ( j = 0; j < 256 && text_sub_41963C->name[0] != 255; ++j )
    {
      int charWidth = 16;
      int leftPadding = 0;
      bool isKanjiDetected = false;
      switch ( text_sub_41963C->name[0] )
      {
        case 0xFAu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[1][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[1][*(byte*)(text_sub_41963C)] >> 5;
          v106 += leftPadding + std::ceil(0.5f * charWidth);
          isKanjiDetected = true;
          //v106 += 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 231) & 0x1F)
          //      + 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 231) >> 5);
          ++v95;
          break;
        case 0xFBu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[2][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[2][*(byte*)(text_sub_41963C)] >> 5;
          v106 += leftPadding + std::ceil(0.5f * charWidth);
          isKanjiDetected = true;
          //v106 += 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 441) & 0x1F)
          //      + 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 441) >> 5);
          ++v95;
          break;
        case 0xFCu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[3][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[3][*(byte*)(text_sub_41963C)] >> 5;
          v106 += leftPadding + std::ceil(0.5f * charWidth);
          isKanjiDetected = true;
          //v106 += 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 672) & 0x1F)
          //      + 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 672) >> 5);
          ++v95;
          break;
        case 0xFDu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[4][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[4][*(byte*)(text_sub_41963C)] >> 5;
          v106 += leftPadding + std::ceil(0.5f * charWidth);
          isKanjiDetected = true;
          //v106 += 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 672) & 0x1F)
          //      + 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 672) >> 5);
          ++v95;
          break;
        case 0xFEu:
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          charWidth = charWidthData[5][*(byte*)(text_sub_41963C)] & 0x1F;
          leftPadding = charWidthData[5][*(byte*)(text_sub_41963C)] >> 5;
          v106 += leftPadding + std::ceil(0.5f * charWidth);
          isKanjiDetected = true;
          //v106 += 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 672) & 0x1F)
          //      + 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0] + 672) >> 5);
          ++v95;
          break;
        case 0xF8u:
          v118 = 1;
          text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
          break;
        default:
          if(!isKanjiDetected)
          {
            charWidth = charWidthData[0][*(byte*)(text_sub_41963C)] & 0x1F;
            leftPadding = charWidthData[0][*(byte*)(text_sub_41963C)] >> 5;
            v106 += leftPadding + std::ceil(0.5f * charWidth);
          }
          isKanjiDetected = false;
          //v106 += 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0]) & 0x1F)
                //+ 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + text_sub_41963C->name[0]) >> 5);
          ++v95;
          break;
      }
      text_sub_41963C = (attack_name_fixed_buffer *)((char *)text_sub_41963C + 1);
    }
    v135 = v123;
    v4 = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width;
    v107 = (((int)v4 - HIWORD(v4)) >> 1) - v106 / 2;
    v120 = 0;
    bool isKanjiDetected = false;
    int charWidth = 16;
    int leftPadding = 0;
    ff7_graphics_object* graphics_object = ff7_externals.menu_jafont_1_graphics_object;
    while ( v120 < 256 && v135->name[0] != 255 )
    {      
      switch ( v135->name[0] )
      {
        case 0xF8u:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 2);
          goto LABEL_31;
        case 0xFAu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          /*v129 = 12 * (v135->name[0] / 21) + 132;
          v108 = 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + v135->name[0] + 231) >> 5) + v107;
          v96 = 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + v135->name[0] + 231) & 0x1F);*/
          graphics_object = ff7_externals.menu_jafont_2_graphics_object;
          charWidth = charWidthData[1][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[1][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
          //goto LABEL_49;
        case 0xFBu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          /*v129 = 12 * (v135->name[0] / 21);
          v108 = 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + v135->name[0] + 441) >> 5) + v107;
          v96 = 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + v135->name[0] + 441) & 0x1F);*/
          graphics_object = ff7_externals.menu_jafont_3_graphics_object;
          charWidth = charWidthData[2][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[2][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
          //goto LABEL_49;
        case 0xFCu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          /*v129 = 12 * (v135->name[0] / 21) + 132;
          v108 = 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + v135->name[0] + 672) >> 5) + v107;
          v96 = 2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + v135->name[0] + 672) & 0x1F);*/
          graphics_object = ff7_externals.menu_jafont_4_graphics_object;
          charWidth = charWidthData[3][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[3][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
          //goto LABEL_49;
        case 0xFDu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          graphics_object = ff7_externals.menu_jafont_5_graphics_object;
          charWidth = charWidthData[4][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[4][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
          //goto LABEL_49;
        case 0xFEu:
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          graphics_object = ff7_externals.menu_jafont_6_graphics_object;
          charWidth = charWidthData[5][v135->name[0]] & 0x1F;
          leftPadding = charWidthData[5][v135->name[0]] >> 5;
          isKanjiDetected = true;
          continue;
          //goto LABEL_49;
        default:
          if (!isKanjiDetected)
          {
            graphics_object = ff7_externals.menu_jafont_1_graphics_object;
            charWidth = charWidthData[0][v135->name[0]] & 0x1F;
            leftPadding = charWidthData[0][v135->name[0]] >> 5;
          }
          isKanjiDetected = false;

          v105 = v135->name[0];
          //if ( v135->name[0] == 210 || v135->name[0] == 211 )
            //v105 -= 78;
          v132 = 32 * (v105 % 16);
          v129 = 32 * (v105 / 16);
          if ( v132 <= 480 )
          {
            v127 = v132;//24 * (v105 % 21);
            if ( v132 == 480 )
            {
              v137 = 32;
              v126 = 16;
            }
            else
            {
              v137 = 32;
              v126 = 16;
            }
            a2 = graphics_object;//(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->menu_font_a_graphics_object;
          }/*
          else
          {
            v127 = v132 - 256;
            v137 = 32.0;
            v126 = 16.0;
            a2 = graphics_object;//(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->menu_font_b_graphics_object;
          //}*/
          v108 = std::ceil(0.5f * charWidth)/* 2 * ((int)*(unsigned __int8 *)(*ff7_externals.g_text_spacing_DB958C + v135->name[0]) >> 5)*/ + v107;
          v96 = leftPadding;//2 * (*(byte *)(*ff7_externals.g_text_spacing_DB958C + v135->name[0]) & 0x1F);
LABEL_49:
          if ( ff7_externals.g_get_do_render_menu_6CDBF2() && !*ff7_externals.g_is_battle_paused_DC0E6C && common_externals.draw_graphics_object(1, (struct graphics_object*)a2) )
          {
            auto color = get_character_color(7);
            color.a = 128;

            v102 = (double)v127 / 512.0;
            v99 = (double)v129 / 512.0;
            v98 = v137 / 512.0;
            v94 = a2->vertex_transform;
            v94->position.x = (double)offset_x + (double)v108;
            v94->position.y = (double)offset_y + (double)12;
            v94->position.z = 0.0;
            v94->position.w = 1.0;
            v94->color = color;
            v94->alpha_mask = -16777216;
            v94->u = v102;
            v94->v = v99;
            v93 = a2->vertex_transform + 1;
            v93->position.x = (double)offset_x + (double)v108;
            v93->position.y = (double)offset_y + (double)12 + 16.0;
            v93->position.z = 0.0;
            v93->position.w = 1.0;
            v93->color = color;
            v93->alpha_mask = -16777216;
            v93->u = v102;
            v93->v = v99 + 32.0f / 512.0f;
            v92 = a2->vertex_transform + 2;
            v92->position.x = (double)offset_x + (double)v108 + (double)v126;
            v92->position.y = (double)offset_y + (double)12;
            v92->position.z = 0.0;
            v92->position.w = 1.0;
            v92->color = color;
            v92->alpha_mask = -16777216;
            v92->u = v102 + v98;
            v92->v = v99;
            v91 = a2->vertex_transform + 3;
            v91->position.x = (double)offset_x + (double)v108 + (double)v126;
            v91->position.y = (double)offset_y + (double)12 + 16.0;
            v91->position.z = 0.0;
            v91->position.w = 1.0;
            v91->color = color;
            v91->alpha_mask = -16777216;
            v91->u = v102 + v98;
            v91->v = v99 + 32.0f / 512.0f;
            *(byte *)a2->curr_total_n_shape = 14;
            a2->field_7C = 14;
          }
          v135 = (attack_name_fixed_buffer *)((char *)v135 + 1);
          v107 = v96 + v108;
LABEL_31:
          ++v120;
          break;
      }
    }
  }
  v115 = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width;
  menu_height = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_height;
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    // 320x240 viewport not needed
  }
  else
  {
    if ( ff7_externals.g_get_do_render_menu_6CDBF2()
      && !*ff7_externals.g_is_battle_paused_DC0E6C
      && common_externals.draw_graphics_object(8, (struct graphics_object*)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object) )
    {
      v85 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v85->position.x = (float)offset_x;
      v85->position.y = (float)offset_y;
      v85->position.z = 0.0101;
      v85->position.w = 1.0;
      v85->color = { 255, 255, 255, 128 };
      v85->alpha_mask = -16777216;
      v85->u = 0.0;
      v85->v = 0.8125;
      v84 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v84->position.x = (float)offset_x;
      v84->position.y = (double)offset_y + 8.0;
      v84->position.z = 0.0101;
      v84->position.w = 1.0;
      v84->color = { 255, 255, 255, 128 };
      v84->alpha_mask = -16777216;
      v84->u = 0.0;
      v84->v = 0.03125 + 0.8125;
      v83 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v83->position.x = (double)offset_x + 8.0;
      v83->position.y = (float)offset_y;
      v83->position.z = 0.0101;
      v83->position.w = 1.0;
      v83->color = { 255, 255, 255, 128 };
      v83->alpha_mask = -16777216;
      v83->u = 0.03125 + 0.0;
      v83->v = 0.8125;
      v82 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v82->position.x = (double)offset_x + 8.0;
      v82->position.y = (double)offset_y + 8.0;
      v82->position.z = 0.0101;
      v82->position.w = 1.0;
      v82->color = { 255, 255, 255, 128 };
      v82->alpha_mask = -16777216;
      v82->u = 0.03125 + 0.0;
      v82->v = 0.03125 + 0.8125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v81 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v81->position.x = (double)offset_x + (double)v115 - 8.0;
      v81->position.y = (float)offset_y;
      v81->position.z = 0.0101;
      v81->position.w = 1.0;
      v81->color = { 255, 255, 255, 128 };
      v81->alpha_mask = -16777216;
      v81->u = 0.09375;
      v81->v = 0.8125;
      v80 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v80->position.x = (double)offset_x + (double)v115 - 8.0;
      v80->position.y = (double)offset_y + 8.0;
      v80->position.z = 0.0101;
      v80->position.w = 1.0;
      v80->color = { 255, 255, 255, 128 };
      v80->alpha_mask = -16777216;
      v80->u = 0.09375;
      v80->v = 0.03125 + 0.8125;
      v79 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v79->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v79->position.y = (float)offset_y;
      v79->position.z = 0.0101;
      v79->position.w = 1.0;
      v79->color = { 255, 255, 255, 128 };
      v79->alpha_mask = -16777216;
      v79->u = 0.03125 + 0.09375;
      v79->v = 0.8125;
      v78 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v78->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v78->position.y = (double)offset_y + 8.0;
      v78->position.z = 0.0101;
      v78->position.w = 1.0;
      v78->color = { 255, 255, 255, 128 };
      v78->alpha_mask = -16777216;
      v78->u = 0.03125 + 0.09375;
      v78->v = 0.03125 + 0.8125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v77 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v77->position.x = (float)offset_x;
      v77->position.y = (double)offset_y + (double)menu_height - 8.0;
      v77->position.z = 0.0101;
      v77->position.w = 1.0;
      v77->color = { 255, 255, 255, 128 };
      v77->alpha_mask = -16777216;
      v77->u = 0.125;
      v77->v = 0.78125;
      v76 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v76->position.x = (float)offset_x;
      v76->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v76->position.z = 0.0101;
      v76->position.w = 1.0;
      v76->color = { 255, 255, 255, 128 };
      v76->alpha_mask = -16777216;
      v76->u = 0.125;
      v76->v = 0.03125 + 0.78125;
      v75 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v75->position.x = (double)offset_x + 8.0;
      v75->position.y = (double)offset_y + (double)menu_height - 8.0;
      v75->position.z = 0.0101;
      v75->position.w = 1.0;
      v75->color = { 255, 255, 255, 128 };
      v75->alpha_mask = -16777216;
      v75->u = 0.03125 + 0.125;
      v75->v = 0.78125;
      v74 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v74->position.x = (double)offset_x + 8.0;
      v74->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v74->position.z = 0.0101;
      v74->position.w = 1.0;
      v74->color = { 255, 255, 255, 128 };
      v74->alpha_mask = -16777216;
      v74->u = 0.03125 + 0.125;
      v74->v = 0.03125 + 0.78125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v73 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v73->position.x = (double)offset_x + (double)v115 - 8.0;
      v73->position.y = (double)offset_y + (double)menu_height - 8.0;
      v73->position.z = 0.0101;
      v73->position.w = 1.0;
      v73->color = { 255, 255, 255, 128 };
      v73->alpha_mask = -16777216;
      v73->u = 0.21875;
      v73->v = 0.78125;
      v72 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v72->position.x = (double)offset_x + (double)v115 - 8.0;
      v72->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v72->position.z = 0.0101;
      v72->position.w = 1.0;
      v72->color = { 255, 255, 255, 128 };
      v72->alpha_mask = -16777216;
      v72->u = 0.21875;
      v72->v = 0.03125 + 0.78125;
      v71 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v71->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v71->position.y = (double)offset_y + (double)menu_height - 8.0;
      v71->position.z = 0.0101;
      v71->position.w = 1.0;
      v71->color = { 255, 255, 255, 128 };
      v71->alpha_mask = -16777216;
      v71->u = 0.03125 + 0.21875;
      v71->v = 0.78125;
      v70 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v70->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v70->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v70->position.z = 0.0101;
      v70->position.w = 1.0;
      v70->color = { 255, 255, 255, 128 };
      v70->alpha_mask = -16777216;
      v70->u = 0.03125 + 0.21875;
      v70->v = 0.03125 + 0.78125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v69 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v69->position.x = (double)offset_x + 8.0;
      v69->position.y = (float)offset_y;
      v69->position.z = 0.0101;
      v69->position.w = 1.0;
      v69->color = { 255, 255, 255, 128 };
      v69->alpha_mask = -16777216;
      v69->u = 0.0;
      v69->v = 0.75;
      v68 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v68->position.x = (double)offset_x + 8.0;
      v68->position.y = (double)offset_y + 8.0;
      v68->position.z = 0.0101;
      v68->position.w = 1.0;
      v68->color = { 255, 255, 255, 128 };
      v68->alpha_mask = -16777216;
      v68->u = 0.0;
      v68->v = 0.75 + 0.03125;
      v67 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v67->position.x = (double)offset_x + 8.0 + (double)v115 - 16.0;
      v67->position.y = (float)offset_y;
      v67->position.z = 0.0101;
      v67->position.w = 1.0;
      v67->color = { 255, 255, 255, 128 };
      v67->alpha_mask = -16777216;
      v67->u = 0.0 + 0.0625;
      v67->v = 0.75;
      v66 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v66->position.x = (double)offset_x + 8.0 + (double)v115 - 16.0;
      v66->position.y = (double)offset_y + 8.0;
      v66->position.z = 0.0101;
      v66->position.w = 1.0;
      v66->color = { 255, 255, 255, 128 };
      v66->alpha_mask = -16777216;
      v66->u = 0.0 + 0.0625;
      v66->v = 0.75 + 0.03125;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v65 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v65->position.x = (double)offset_x + 8.0;
      v65->position.y = (double)offset_y + (double)menu_height - 8.0;
      v65->position.z = 0.0101;
      v65->position.w = 1.0;
      v65->color = { 255, 255, 255, 128 };
      v65->alpha_mask = -16777216;
      v65->u = 0.125;
      v65->v = 0.84375;
      v64 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v64->position.x = (double)offset_x + 8.0;
      v64->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v64->position.z = 0.0101;
      v64->position.w = 1.0;
      v64->color = { 255, 255, 255, 128 };
      v64->alpha_mask = -16777216;
      v64->u = 0.125;
      v64->v = 0.03125 + 0.84375;
      v63 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v63->position.x = (double)offset_x + 8.0 + (double)v115 - 16.0;
      v63->position.y = (double)offset_y + (double)menu_height - 8.0;
      v63->position.z = 0.0101;
      v63->position.w = 1.0;
      v63->color = { 255, 255, 255, 128 };
      v63->alpha_mask = -16777216;
      v63->u = 0.0625 + 0.125;
      v63->v = 0.84375;
      v62 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v62->position.x = (double)offset_x + 8.0 + (double)v115 - 16.0;
      v62->position.y = (double)offset_y + (double)menu_height - 8.0 + 8.0;
      v62->position.z = 0.0101;
      v62->position.w = 1.0;
      v62->color = { 255, 255, 255, 128 };
      v62->alpha_mask = -16777216;
      v62->u = 0.0625 + 0.125;
      v62->v = 0.03125 + 0.84375;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v61 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v61->position.x = (double)offset_x + (double)v115 - 8.0;
      v61->position.y = (double)offset_y + 8.0;
      v61->position.z = 0.0101;
      v61->position.w = 1.0;
      v61->color = { 255, 255, 255, 128 };
      v61->alpha_mask = -16777216;
      v61->u = 0.21875;
      v61->v = 0.875;
      v60 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v60->position.x = (double)offset_x + (double)v115 - 8.0;
      v60->position.y = (double)offset_y + 8.0 + (double)menu_height - 16.0;
      v60->position.z = 0.0101;
      v60->position.w = 1.0;
      v60->color = { 255, 255, 255, 128 };
      v60->alpha_mask = -16777216;
      v60->u = 0.21875;
      v60->v = 0.875 + 0.0625;
      v59 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v59->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v59->position.y = (double)offset_y + 8.0;
      v59->position.z = 0.0101;
      v59->position.w = 1.0;
      v59->color = { 255, 255, 255, 128 };
      v59->alpha_mask = -16777216;
      v59->u = 0.21875 + 0.03125;
      v59->v = 0.875;
      v58 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v58->position.x = (double)offset_x + (double)v115 - 8.0 + 8.0;
      v58->position.y = (double)offset_y + 8.0 + (double)menu_height - 16.0;
      v58->position.z = 0.0101;
      v58->position.w = 1.0;
      v58->color = { 255, 255, 255, 128 };
      v58->alpha_mask = -16777216;
      v58->u = 0.21875 + 0.03125;
      v58->v = 0.875 + 0.0625;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform = (graphics_vertex *)((char *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_offset);
      ++(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape;
      v57 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform;
      v57->position.x = (float)offset_x;
      v57->position.y = (double)offset_y + 8.0;
      v57->position.z = 0.0101;
      v57->position.w = 1.0;
      v57->color = { 255, 255, 255, 128 };
      v57->alpha_mask = -16777216;
      v57->u = 0.0;
      v57->v = 0.875;
      v56 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 1;
      v56->position.x = (float)offset_x;
      v56->position.y = (double)offset_y + 8.0 + (double)menu_height - 16.0;
      v56->position.z = 0.0101;
      v56->position.w = 1.0;
      v56->color = { 255, 255, 255, 128 };
      v56->alpha_mask = -16777216;
      v56->u = 0.0;
      v56->v = 0.0625 + 0.875;
      v55 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 2;
      v55->position.x = (double)offset_x + 8.0;
      v55->position.y = (double)offset_y + 8.0;
      v55->position.z = 0.0101;
      v55->position.w = 1.0;
      v55->color = { 255, 255, 255, 128 };
      v55->alpha_mask = -16777216;
      v55->u = 0.03125 + 0.0;
      v55->v = 0.875;
      v54 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->vertex_transform + 3;
      v54->position.x = (double)offset_x + 8.0;
      v54->position.y = (double)offset_y + 8.0 + (double)menu_height - 16.0;
      v54->position.z = 0.0101;
      v54->position.w = 1.0;
      v54->color = { 255, 255, 255, 128 };
      v54->alpha_mask = -16777216;
      v54->u = 0.03125 + 0.0;
      v54->v = 0.0625 + 0.875;
      *(byte *)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->curr_total_n_shape = 0;
      (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->btl_win_c_menu_border_graphics_object->field_7C = 0;
    }
    v116 = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width - 12;
    v113 = (*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_height - 12;
    if ( v118
      && ff7_externals.g_get_do_render_menu_6CDBF2()
      && !*ff7_externals.g_is_battle_paused_DC0E6C
      && common_externals.draw_graphics_object(1, (struct graphics_object*)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]) )
    {
      v53 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]->vertex_transform;
      v53->position.x = (double)offset_x + 6.0;
      v53->position.y = (double)offset_y + 6.0;
      v53->position.z = 0.0101;
      v53->position.w = 1.0;
      v53->color = { 0, 0, 112, 144 };
      v53->alpha_mask = -16777216;
      v52 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]->vertex_transform + 1;
      v52->position.x = (double)offset_x + 6.0;
      v52->position.y = (double)offset_y + 6.0 + (double)(*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_height - 12.0;
      v52->position.z = 0.0101;
      v52->position.w = 1.0;
      v52->color = { 0, 0, 112, 144 };
      v52->alpha_mask = -16777216;
      v51 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]->vertex_transform + 2;
      v51->position.x = (double)offset_x + 6.0 + (double)(*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width - 12.0;
      v51->position.y = (double)offset_y + 6.0;
      v51->position.z = 0.0101;
      v51->position.w = 1.0;
      v51->color = { 0, 0, 112, 144 };
      v51->alpha_mask = -16777216;
      v50 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->graphics_data_other_array[0]->vertex_transform + 3;
      v50->position.x = (double)offset_x + 6.0 + (double)(*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_width - 12.0;
      v50->position.y = (double)offset_y + 6.0 + (double)(*ff7_externals.battle_menu_data_DC3630)[menu_box_idx].menu_height - 12.0;
      v50->position.z = 0.0101;
      v50->position.w = 1.0;
      v50->color = { 0, 0, 112, 144 };
      v50->alpha_mask = -16777216;
    }
    if ( ff7_externals.g_get_do_render_menu_6CDBF2()
      && !*ff7_externals.g_is_battle_paused_DC0E6C
      && common_externals.draw_graphics_object(1, (struct graphics_object*)(*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object) )
    {
      v49 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object->vertex_transform;
      v49->position.x = (double)offset_x + 6.0;
      v49->position.y = (double)offset_y + 6.0;
      v49->position.z = 0.0101;
      v49->position.w = 1.0;
      v49->color = *ff7_externals.dword_91EFC8;
      v49->alpha_mask = -16777216;
      v48 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object->vertex_transform + 1;
      v48->position.x = (double)offset_x + 6.0;
      v48->position.y = (double)offset_y + (double)v113 + 6.0;
      v48->position.z = 0.0101;
      v48->position.w = 1.0;
      v48->color = *ff7_externals.dword_91EFCC;
      v48->alpha_mask = -16777216;
      v47 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object->vertex_transform + 2;
      v47->position.x = (double)offset_x + (double)v116 + 6.0;
      v47->position.y = (double)offset_y + 6.0;
      v47->position.z = 0.0101;
      v47->position.w = 1.0;
      v47->color = *ff7_externals.dword_91EFD0;
      v47->alpha_mask = -16777216;
      v46 = (*ff7_externals.battle_graphics_data_ptr_9ADFD8)->box_color_graphics_object->vertex_transform + 3;
      v46->position.x = (double)offset_x + (double)v116 + 6.0;
      v46->position.y = (double)offset_y + (double)v113 + 6.0;
      v46->position.z = 0.0101;
      v46->position.w = 1.0;
      v46->color = *ff7_externals.dword_91EFD4;
      v46->alpha_mask = -16777216;
    }
  }
}

void main_menu_draw_everything_maybe_6C0B91_jp()
{
  ff7_game_obj *game_object; // [esp+0h] [ebp-4h]

  game_object = ff7_externals.engine_get_game_object_676578();
  ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(0, game_object);
  ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*(ff7_graphics_object**)ff7_externals.menu_objects, game_object);
  ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_window_bg_graphics_object_DC0FF0, game_object);
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
  {
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_4_graphics_object_DC104C, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_blend_4_graphics_object_DC1048, game_object);
  }
  else
  {
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_4_graphics_object_DC0FC8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_graphics_object_DC0FD0, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_c_blend_4_diff_graphics_object_DC0FD8, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_b_blend_4_graphics_object_DC0FCC, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_d_blend_4_graphics_object_DC0FD4, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_a_graphics_object_DC100C, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_font_b_graphics_object_DC1010, game_object);

    // jp
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_1_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_2_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_3_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_4_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_5_graphics_object, game_object);
    ff7_externals.engine_draw_graphics_object_66E641(ff7_externals.menu_jafont_6_graphics_object, game_object);
  }
  ff7_externals.engine_gfx_draw_predefined_polygon_set_field_84_sub_660E95(1, game_object);
  ff7_externals.engine_gfx_set_single_renderstate_sub_660C3A(2, 0, game_object);
  if ( *ff7_externals.menu_is_small_viewport_320_240_DC130C == 1 )
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_blend_0_graphics_object_DC1050, game_object);
  else
    ff7_externals.engine_draw_graphics_object_66E641(*ff7_externals.menu_win_a_blend_0_graphics_object_DC0FDC, game_object);
  ff7_externals.engine_gfx_setviewport_sub_66067A(
    *ff7_externals.menu_viewport_x_DC105C,
    *ff7_externals.menu_viewport_y_DC1060,
    *ff7_externals.menu_viewport_width_DC1064,
    *ff7_externals.menu_viewport_view_DC1068,
    game_object);
}

void auto_resize_text_box(int16_t WINDOW_ID, int16_t* pOutW, int16_t* pOutH)
{
	int16_t W = 0;
	int16_t H = 0;
	int16_t maxW = 0;
	int16_t maxH = 0;
	byte* buffer_text = (byte*)ff7_externals.current_dialog_string_pointer[WINDOW_ID];
  bool isKanjiDetected = false;
  int charWidth = 0;
  int leftPadding = 0;
	for ( int i = 0;	i < 1024; ++i )
	{
    byte character = buffer_text[i];
    byte next_character = buffer_text[i + 1];

    if(character == 0xFF) break;

    switch ( character )
    {
      case 0xFAu:
        charWidth = charWidthData[1][next_character] & 0x1F;
        leftPadding = charWidthData[1][next_character] >> 5;
        isKanjiDetected = true;
        continue;
      case 0xFBu:

        charWidth = charWidthData[2][next_character] & 0x1F;
        leftPadding = charWidthData[2][next_character] >> 5;          
        isKanjiDetected = true;
        continue;
      case 0xFCu:
        charWidth = charWidthData[3][next_character] & 0x1F;
        leftPadding = charWidthData[3][next_character] >> 5;
        isKanjiDetected = true;
        continue;
      case 0xFDu:
        charWidth = charWidthData[4][next_character] & 0x1F;
        leftPadding = charWidthData[4][next_character] >> 5;
        isKanjiDetected = true;
        continue;
      case 0xFEu:
        charWidth = charWidthData[5][next_character] & 0x1F;
        leftPadding = charWidthData[5][next_character] >> 5;
        isKanjiDetected = true;
        continue;
      default:
        if(!isKanjiDetected)
        {
          charWidth = charWidthData[0][character] & 0x1F;
          leftPadding = charWidthData[0][character] >> 5;
        }
        isKanjiDetected = false;
        break;
    }

    // character names
    if(character >= 0xEA && character <= 0xF5) 
    {
      auto name_buffer = ff7_externals.sub_6CB9B8(character - 0xEA);
      for (int j = 0; j < 9; ++j)
      {
        auto name_char = name_buffer[j];

        if (name_char == 0xFF) break;

        charWidth = charWidthData[0][name_char] & 0x1F;
        leftPadding = charWidthData[0][name_char] >> 5;
        W += leftPadding + std::ceil(0.5f * charWidth);
      }
      
      continue;
    }

		if(character == 0xE7)
		{
      maxW = std::max(maxW, W);
      W = 0;
			H += 32;
      continue;
		}
		if(character == 0xE9 || character == 0xE8)
		{
			maxW = std::max(maxW, W);
			maxH = std::max(maxH, H);
			W = 0;
			H = 0;
      continue;
		}

		W += leftPadding + std::ceil(0.5f * charWidth);
	}
	*pOutW = (std::max(maxW, W) + 40) / 2;
	*pOutH = (std::max(maxH, H) + 50) / 2;
}

void field_text_box_window_opening_6317A9_jp(short WINDOW_ID)
{
  int16_t W = 0;
  int16_t H = 0;
  auto_resize_text_box(WINDOW_ID, &W, &H);
  ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width = W;
  ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height = H;
  
  if ( ff7_externals.field_text_box_window_entity_id_CC0960[WINDOW_ID] == *ff7_externals.current_entity_id_byte_CC0964 )
  {
    ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width += ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width
                                                                       / 4;
    if ( ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width < 8 )
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width = 8;
    if ( ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width > ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width )
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width = ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width;
    ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height += ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height
                                                                        / 4;
    if ( ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height < 8 )
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height = 8;
    if ( ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height > ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height )
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height = ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height;
    if ( ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_width == ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_width
      && ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].current_window_height == ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_height )
    {
      ff7_externals.text_box_window_data_array_CFF5B8[WINDOW_ID].window_mode = 2;
    }
  }
}

int sub_6F54A2_jp(byte *a1)
{
  int v2; // [esp+Ch] [ebp-Ch]
  int v3; // [esp+10h] [ebp-8h]
  int v4; // [esp+14h] [ebp-4h]

  v3 = 0;
  v2 = 0;
  bool kanjiDetected = false;
  int charWidth = 0;
  int leftPadding = 0;
  while ( v3 < 64 /*ff7_externals.g_max_string_length_91F034*/ && a1 && (unsigned __int8)*a1 != 255 )
  {
    /*switch ( *a1 )
    {
      case 250:
        ++a1;
        v4 = 231;
        break;
      case 251:
        ++a1;
        v4 = 441;
        break;
      case 252:
        ++a1;
        v4 = 672;
        break;
      case 253:
        ++a1;
        v4 = 882;
        break;
      case 254:
        ++a1;
        v4 = 1092;
        break;
      default:
        v4 = 0;
        break;
    }*/

    auto next_char = a1 + 1;
    switch ( *a1 )
      {
        case 0xFAu:
          kanjiDetected = true;
          charWidth = charWidthData[1][*next_char] & 0x1F;
          leftPadding = charWidthData[1][*next_char] >> 5;
          ++a1;
          ++v3;
          continue;
        case 0xFBu:
          kanjiDetected = true;
          charWidth = charWidthData[2][*next_char] & 0x1F;
          leftPadding = charWidthData[2][*next_char] >> 5;
          ++a1;
          ++v3;
          continue;
        case 0xFCu:
          kanjiDetected = true;
          charWidth = charWidthData[3][*next_char] & 0x1F;
          leftPadding = charWidthData[3][*next_char] >> 5;
          ++a1;
          ++v3;
          continue;
        case 0xFDu:
          kanjiDetected = true;
          charWidth = charWidthData[4][*next_char] & 0x1F;
          leftPadding = charWidthData[4][*next_char] >> 5;
          ++a1;
          ++v3;
          continue;
        case 0xFEu:

          kanjiDetected = true;
          charWidth = charWidthData[5][*next_char] & 0x1F;
          leftPadding = charWidthData[5][*next_char] >> 5;
          ++a1;
          ++v3;
          continue;
        default:
          if(!kanjiDetected)
          {
            charWidth = charWidthData[0][*a1] & 0x1F;
            leftPadding = charWidthData[0][*a1] >> 5;
          }
          kanjiDetected = false;
          break;
      }

    /*if ( ff7_externals.dword_DC12DC )
      v2 += (__int64)((double)(*(byte *)(ff7_externals.g_text_spacing_DB958C + v4 + (unsigned __int8)*a1) & 0x1F) * 1.6666666)
          + (__int64)((double)((int)*(unsigned __int8 *)(ff7_externals.g_text_spacing_DB958C + v4 + (unsigned __int8)*a1) >> 5)
                    * 1.6666666);
    else
      v2 += 2 * ((int)*(unsigned __int8 *)(ff7_externals.g_text_spacing_DB958C + v4 + (unsigned __int8)*a1) >> 5)
          + 2 * (*(byte *)(ff7_externals.g_text_spacing_DB958C + v4 + (unsigned __int8)*a1) & 0x1F);*/
    v2 += leftPadding + std::ceil(0.5f * charWidth);
    ++a1;
    ++v3;
  }
  return v2;
}