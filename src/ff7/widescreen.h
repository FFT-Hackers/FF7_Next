/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
//    Copyright (C) 2022 Cosmos                                             //
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

#include "common.h"
#include "globals.h"

int wide_viewport_x = -106;
int wide_viewport_y = 0;
int wide_viewport_width = 854;
int wide_viewport_height = 480;

int wide_game_x = 0;
int wide_game_y = 0;
int wide_game_width = 960;
int wide_game_height = 480;

void ff7_widescreen_hook_init();
void ff7_field_draw_gray_quads_sub_644E90();

enum WIDESCREEN_MODE
{
    WM_DISABLED,
    WM_EXTEND_ONLY,
    WM_ZOOM,
};

class Widescreen
{
public:
    void init();
    void initParamsFromConfig();
    void exportConfig();
    void reloadConfig();

    const field_camera_range& getCameraRange();
    WIDESCREEN_MODE getMode();

private:
    void loadConfig();

private:
    // Config
    toml::parse_result config;

    field_camera_range camera_range;
    WIDESCREEN_MODE widescreen_mode = WM_DISABLED;
};

inline const field_camera_range& Widescreen::getCameraRange()
{
    return camera_range;
}

inline WIDESCREEN_MODE Widescreen::getMode()
{
    struct game_mode* mode = getmode_cached();
    if (mode->driver_mode != MODE_FIELD) return WM_DISABLED;

    return widescreen_mode;
}

extern Widescreen widescreen;
