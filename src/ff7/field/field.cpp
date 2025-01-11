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

/*
 * This file contains the changes necessary to support subtractive and 25%
 * blending modes in field backgrounds. Texture pages where these blending
 * modes are used are duplicated and the tile data modified to point to these
 * new pages which have the correct blending mode set.
 */

#include "../../log.h"
#include "../../patch.h"
#include "../../common.h"
#include "../widescreen.h"
#include "../defs.h"

#include "opcode.h"
#include "background.h"
#include "defs.h"
#include "enter.h"
#include "model.h"

namespace ff7::field
{
    // helper function initializes page dst, copies texture from src and applies
    // blend_mode
    void field_load_textures_helper(struct ff7_game_obj *game_object, struct struc_3 *struc_3, uint32_t src, uint32_t dst, uint32_t blend_mode)
    {
        struct ff7_tex_header *tex_header;

        ff7_externals.make_struc3(blend_mode, struc_3);

        tex_header = (struct ff7_tex_header *)common_externals.create_tex_header();

        ff7_externals.field_layers[dst]->tex_header = tex_header;

        if(ff7_externals.field_layers[src]->type == 1) ff7_externals.make_field_tex_header_pal(tex_header);
        if(ff7_externals.field_layers[src]->type == 2) {
            ff7_externals.make_field_tex_header(tex_header);
            tex_header->color_key = 3;
        }

        struc_3->tex_header = tex_header;

        if(src != dst)
        {
            ff7_externals.field_layers[dst]->image_data = external_malloc(256 * 256);
            memcpy(ff7_externals.field_layers[dst]->image_data, ff7_externals.field_layers[src]->image_data, 256 * 256);
        }

        tex_header->image_data = (unsigned char*)ff7_externals.field_layers[dst]->image_data;

        tex_header->file.pc_name = (char*)external_malloc(1024);
        sprintf(tex_header->file.pc_name, "field/%s/%s_%02i", strchr(ff7_externals.field_file_name, '\\') + 1, strchr(ff7_externals.field_file_name, '\\') + 1, src);

        ff7_externals.field_layers[dst]->graphics_object = ff7_externals._load_texture(1, PT_S2D, struc_3, 0, game_object->dx_sfx_something);
        ff7_externals.field_layers[dst]->present = true;
    }

    void field_load_textures(struct ff7_game_obj *game_object, struct struc_3 *struc_3)
    {
        uint32_t i;

        ff7_externals.field_convert_type2_layers();

        for(i = 0; i < 29; i++)
        {
            uint32_t blend_mode = 4;

            if(!ff7_externals.field_layers[i]->present) continue;

            if(ff7_externals.field_layers[i]->type == 1)
            {
                if(i >= 24) blend_mode = 0;
                else if(i >= 15) blend_mode = 1;
            }
            else if(ff7_externals.field_layers[i]->type == 2)
            {
                if(i >= 40) blend_mode = 0;
                else if(i >= 33) blend_mode = 1;
            }
            else ffnx_glitch("unknown field layer type %i\n", ff7_externals.field_layers[i]->type);

            field_load_textures_helper(game_object, struc_3, i, i, blend_mode);

            // these magic numbers have been gleaned from original source data
            // the missing blend modes in question are used in exactly these pages
            // and copying them in this manner does not risk overwriting any other
            // data
            if(i >= 15 && i <= 18 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 14, 2);
            if(i >= 15 && i <= 20 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 18, 3);
        }

        *ff7_externals.layer2_end_page += 18;
    }

    uint32_t field_open_flevel_siz()
    {
        struct lgp_file *f = lgp_open_file("flevel.siz", 1);

        if (0 == f) {
            return 0;
        }

        uint32_t size = lgp_get_filesize(f, 1);
        char* buffer = new char[size];

        lgp_read_file(f, 1, buffer, size);

        // Increase from 787 (field map count) to 1200
        const uint32_t max_map_count = 1200;
        uint32_t* uncompressed_sizes = reinterpret_cast<uint32_t*>(buffer);
        uint32_t count = size / sizeof(uint32_t);
        uint32_t* flevel_sizes = reinterpret_cast<uint32_t*>(ff7_externals.field_map_infos + 0xBC);

        if (count > max_map_count) {
            count = max_map_count;
        }

        for (uint32_t i = 0; i < count; ++i) {
            flevel_sizes[i * 0x34] = uncompressed_sizes[i] + 4000000; // +2 MB compared to the original implementation
        }

        // Force a value if not specified by the flevel.siz
        for (uint32_t i = count; i < max_map_count; ++i) {
            flevel_sizes[i * 0x34] = 4000000;
        }

        delete[] buffer;

        return 1;
    }

    void ff7_field_set_fade_quad_size(int x, int y, int width, int height)
    {
        if(widescreen_enabled)
        {
            x -= abs(wide_viewport_x);
            width += (wide_viewport_width - game_width);
        }
        if(enable_uncrop)
        {
            y -= ff7_field_center ? 16 : 0;
            height += 32;
        }
        ff7_externals.field_sub_63AC3F(x, y, width, height);
    }

    void ff7_field_evaluate_encounter_rate()
    {
        field_event_data* field_event_data_array = (*ff7_externals.field_event_data_ptr);
        int original_movement_speed = field_event_data_array[*ff7_externals.field_player_model_id].movement_speed;
        field_event_data_array[*ff7_externals.field_player_model_id].movement_speed = original_movement_speed / common_frame_multiplier;
        ff7_externals.field_evaluate_encounter_rate_60B2C6();
        field_event_data_array[*ff7_externals.field_player_model_id].movement_speed = original_movement_speed;
    }

    void ff7_field_hook_init()
    {
        std::copy(common_externals.execute_opcode_table, &common_externals.execute_opcode_table[OPCODE_COUNT - 1], &original_opcode_table[0]);
        // Init stuff
        replace_call_function(ff7_externals.field_sub_60DCED + 0x178, ff7_field_initialize_variables);

        // Model movement (walk, run) fps fix + allow footstep sfx
        replace_call_function(ff7_externals.field_loop_sub_63C17F + 0x5DD, ff7_field_update_models_position);
        replace_call_function(ff7_externals.field_update_models_positions + 0x8BC, ff7_field_update_player_model_position);
        replace_call_function(ff7_externals.field_update_models_positions + 0x9E8, ff7_field_update_single_model_position);
        replace_call_function(ff7_externals.field_update_models_positions + 0x9AA, ff7_field_check_collision_with_target);
        replace_call_function(common_externals.execute_opcode_table[OFST] + 0x46, ff7_opcode_multiply_get_bank_value);

        // Model rotation
        byte jump_to_OFST_update[] = {0xE9, 0xE6, 0x01, 0x00, 0x00};
        replace_call_function(ff7_externals.field_update_models_positions + 0x7C, ff7_field_update_models_rotation_new);
        memcpy_code(ff7_externals.field_update_models_positions + 0x81, jump_to_OFST_update, sizeof(jump_to_OFST_update));

        if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
        {
            if(ff7_fps_limiter == FPS_LIMITER_60FPS)
            {
                // Partial animation fps fix
                patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANMX1], (DWORD)&opcode_script_partial_animation_wrapper);
                patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANMX2], (DWORD)&opcode_script_partial_animation_wrapper);
                patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANIM1], (DWORD)&opcode_script_partial_animation_wrapper);
                patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANIM2], (DWORD)&opcode_script_partial_animation_wrapper);

                // Model movement fps fix for ladder and jump
                patch_code_byte(ff7_externals.field_update_models_positions + 0x1041, 0x2 - common_frame_multiplier / 2);
                patch_code_byte(ff7_externals.field_update_models_positions + 0x189A, 0x2 - common_frame_multiplier / 2);
                replace_call_function(common_externals.execute_opcode_table[JUMP] + 0x1F1, ff7_opcode_multiply_get_bank_value);
                patch_divide_code<int>(ff7_externals.field_update_models_positions + 0xC89, common_frame_multiplier * 2);
                patch_divide_code<int>(ff7_externals.field_update_models_positions + 0xE48, common_frame_multiplier * 2);

                // Encounter rate fix
                replace_call_function(ff7_externals.field_update_models_positions + 0x90F, ff7_field_evaluate_encounter_rate);

                // Text box message fix
                patch_code_byte((uint32_t)ff7_externals.field_text_box_window_paging_631945 + 0xFD, 0x5 + common_frame_multiplier / 2);
                patch_divide_code<byte>((uint32_t)ff7_externals.field_text_box_window_paging_631945 + 0x100, common_frame_multiplier);
                patch_divide_code<WORD>((uint32_t)ff7_externals.field_text_box_window_paging_631945 + 0x111, common_frame_multiplier);
                patch_code_byte((uint32_t)ff7_externals.field_text_box_window_paging_631945 + 0x141, 0x4 + common_frame_multiplier / 2);
                patch_code_byte((uint32_t)ff7_externals.field_text_box_window_opening_6317A9 + 0x3D, 0x2 + common_frame_multiplier / 2);
                patch_code_byte((uint32_t)ff7_externals.field_text_box_window_opening_6317A9 + 0xD2, 0x2 + common_frame_multiplier / 2);
                patch_code_byte(ff7_externals.field_text_box_window_closing_632EB8 + 0x64, 0x2 + common_frame_multiplier / 2);
                patch_code_byte(ff7_externals.field_text_box_window_closing_632EB8 + 0xBF, 0x2 + common_frame_multiplier / 2);
                patch_divide_code<short>(ff7_externals.field_text_box_window_reverse_paging_632CAA + 0x42, common_frame_multiplier);
                patch_divide_code<short>(ff7_externals.field_opcode_message_update_loop_630D50 + 0x1AC, common_frame_multiplier);
                patch_divide_code<short>(ff7_externals.field_opcode_message_update_loop_630D50 + 0x2CF, common_frame_multiplier);
                patch_divide_code<short>((uint32_t)ff7_externals.field_opcode_ask_update_loop_6310A1 + 0x1AC, common_frame_multiplier);
                patch_divide_code<byte>((uint32_t)ff7_externals.field_opcode_ask_update_loop_6310A1 + 0x3CC, common_frame_multiplier);

                // Fade in and fade out screen transitions
                patch_divide_code<short>(ff7_externals.field_initialize_variables + 0x123, common_frame_multiplier);
                patch_code_byte(ff7_externals.field_handle_screen_fading + 0x210, 25 * common_frame_multiplier);
                patch_code_int(ff7_externals.field_handle_screen_fading + 0x240, 25 * common_frame_multiplier - 1);
            }

            // Smooth background movement for both 30 fps mode and 60 fps mode
            replace_call_function(ff7_externals.field_draw_everything + 0x34, ff7_field_set_world_coordinate_640EB7);
            replace_call_function(ff7_externals.field_loop_sub_63C17F + 0x1A6, ff7_field_update_background);
            replace_call_function(ff7_externals.compute_and_submit_draw_gateways_arrows_64DA3B + 0x357, ff7_field_submit_draw_arrow);
            replace_call_function(ff7_externals.compute_and_submit_draw_gateways_arrows_64DA3B + 0x63C, ff7_field_submit_draw_arrow);
            replace_call_function(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x284, ff7_field_submit_draw_cursor);
        }

        // Movie model animation fps fix
        replace_call_function(ff7_externals.field_update_models_positions + 0x68D, ff7_field_update_model_animation_frame);
        replace_call_function(ff7_externals.field_update_models_positions + 0x919, ff7_field_update_model_animation_frame);
        replace_call_function(ff7_externals.field_update_models_positions + 0xA2B, ff7_field_update_model_animation_frame);
        replace_call_function(ff7_externals.field_update_models_positions + 0xE8C, ff7_field_update_model_animation_frame);

        // Background scroll fps fix
        replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x34, ff7_opcode_divide_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x4D, ff7_opcode_divide_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x68, ff7_opcode_divide_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x81, ff7_opcode_divide_get_bank_value);
        replace_function(ff7_externals.opcode_shake, opcode_script_SHAKE);

        // Camera fps fix
        replace_call_function(common_externals.execute_opcode_table[SCRLC] + 0x3B, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[SCRLA] + 0x72, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[SCR2DC] + 0x3C, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[SCR2DL] + 0x3C, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[SCRLP] + 0xA7, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[NFADE] + 0x89, ff7_opcode_divide_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[VWOFT] + 0xCC, ff7_opcode_multiply_get_bank_value);
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[FADE], (DWORD)&opcode_script_FADE);

        // Movie fps fix
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[MVIEF], (DWORD)&opcode_script_MVIEF);
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[BGMOVIE], (DWORD)&opcode_script_BGMOVIE);

        // Others fps fix
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[WAIT], (DWORD)&opcode_script_WAIT);
        replace_function(ff7_externals.sub_611BAE, opcode_IFSW_compare_sub);
    }
}
