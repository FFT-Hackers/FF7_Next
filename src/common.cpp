/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <steamworkssdk/steam_api.h>
#include <hwinfo/hwinfo.h>
#include <regex>
#include <shlwapi.h>
#include <shlobj.h>
#include <psapi.h>
#include <mmsystem.h>
#include <malloc.h>
#include <ddraw.h>
#include <filesystem>
#include <fstream>

#include "renderer.h"
#include "hext.h"
#include "ff8_data.h"

#include "crashdump.h"
#include "macro.h"
#include "ff7.h"
#include "ff8.h"
#include "patch.h"
#include "gl.h"
#include "movies.h"
#include "music.h"
#include "sfx.h"
#include "saveload.h"
#include "gamepad.h"
#include "joystick.h"
#include "input.h"
#include "field.h"
#include "world.h"
#include "gamehacks.h"
#include "audio.h"
#include "voice.h"
#include "metadata.h"
#include "lighting.h"
#include "achievement.h"
#include "game_cfg.h"
#include "exe_data.h"
#include "utils.h"

#include "ff7/defs.h"
#include "ff7/widescreen.h"
#include "ff7/time.h"
#include "ff7/field/defs.h"

#include "ff8/vram.h"
#include "ff8/vibration.h"
#include "ff8/engine.h"
#include "ff8/uv_patch.h"
#include "ff8/ambient.h"
#include "ff8/file.h"

#include "wine.h"

bool proxyWndProc = false;

namespace GameWindowState {
	enum GameWindowState
	{
		CURRENT,
		PRE_FULLSCREEN,
		COUNT
	};
};

struct {
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
} gameWindow[GameWindowState::COUNT];

// global game window handler
RECT gameWindowRect;
HINSTANCE gameHinstance;
HWND gameHwnd;
DEVMODE dmCurrentScreenSettings;
DEVMODE dmNewScreenSettings;
bool gameWindowWasMaximized = false;
bool gameWindowMoving = false;

// global RAM status
MEMORYSTATUSEX last_ram_state = { sizeof(last_ram_state) };

// global FF7/FF8 flag, available after version check
uint32_t ff8 = false;

uint32_t ff7_do_reset = false;

// global FF7/FF8 flag, check if is steam edition
uint32_t steam_edition = false;

// global FF7/FF8 flag, check if using the steam stock launcher
uint32_t steam_stock_launcher = false;

// global FF7 flag, check if is eStore edition
uint32_t estore_edition = false;

// global FF7 flag, check if is japanese edition ( detected as US )
uint32_t ff7_japanese_edition = false;

// window dimensions requested by the game, normally 640x480
uint32_t game_width;
uint32_t game_height;

// offset from screen edge to start of content, for aspect correction
uint32_t x_offset = 0;
uint32_t y_offset = 0;

// widescreen mode enabled
uint32_t widescreen_enabled = false;

// game-specific data, see ff7_data.h/ff8_data.h
uint32_t text_colors[NUM_TEXTCOLORS];
struct game_mode modes[64];
uint32_t num_modes;

// memory locations, replaced functions or patching offsets
// some addresses in FF7 are sourced from static tables in the
// externals_102_xx.h files but most of them are computed at runtime,
// see ff7_data.h/ff8_data.h
struct common_externals common_externals;
struct ff7_externals ff7_externals;
struct ff8_externals ff8_externals;

// various statistics, collected for display purposes only EXCEPT for the
// external cache size
struct driver_stats stats;

// on-screen popup messages
char popup_msg[1024];
uint32_t popup_ttl = 0;
uint32_t popup_color;

// scene stack used to save render state when the game calls begin/end scene
struct driver_state scene_stack[8];
uint32_t scene_stack_pointer = 0;

// global frame counter
uint32_t frame_counter = 0;
double frame_rate = 0;
int battle_frame_multiplier = 1;
int common_frame_multiplier = 1;

// default 32-bit BGRA texture format presented to the game
struct texture_format *texture_format;

// install directory for the current game
char basedir[BASEDIR_LENGTH];

uint32_t version;

bool xinput_connected = false;

bool simulate_OK_button = false;

GamepadAnalogueIntent gamepad_analogue_intent = INTENT_NONE;

uint32_t *image_data_cache = nullptr;
uint32_t image_data_size_cache = 0;

uint32_t noop() { return 0; }
uint32_t noop_a1(uint32_t a1) { return 0; }
uint32_t noop_a2(uint32_t a1, uint32_t a2) { return 0; }
uint32_t noop_a3(uint32_t a1, uint32_t a2, uint32_t a3) { return 0; }

// global data used for profiling macros
#ifdef PROFILE
time_t profile_start;
time_t profile_end;
time_t profile_total;
#endif PROFILE

// support code for the HEAP_DEBUG option
#ifdef HEAP_DEBUG
uint32_t allocs = 0;

void *driver_malloc(uint32_t size)
{
	void *tmp = malloc(size);
	ffnx_trace("%i: malloc(%i) = 0x%x\n", ++allocs, size, tmp);
	return tmp;
}

void *driver_calloc(uint32_t size, uint32_t num)
{
	void *tmp = calloc(size, num);
	ffnx_trace("%i: calloc(%i, %i) = 0x%x\n", ++allocs, size, num, tmp);
	return tmp;
}

void driver_free(void *ptr)
{
	if(!ptr) return;

	ffnx_trace("%i: free(0x%x)\n", --allocs, ptr);
	free(ptr);
}

void *driver_realloc(void *ptr, uint32_t size)
{
	void *tmp = realloc(ptr, size);
	ffnx_trace("%i: realloc(0x%x, %i) = 0x%x\n", allocs, ptr, size, tmp);
	return tmp;
}
#endif

// support code for the NO_EXT_HEAP option
#ifdef NO_EXT_HEAP

void ext_free(void *ptr, const char *file, uint32_t line)
{
	driver_free(ptr);
}

void *ext_malloc(uint32_t size, const char *file, uint32_t line)
{
	return driver_malloc(size);
}

void *ext_calloc(uint32_t size, uint32_t num, const char *file, uint32_t line)
{
	return driver_calloc(size, num);
}
#endif

void ffmpeg_log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
	char msg[4 * 1024]; // 4K
	static int print_prefix = 1;

	av_log_format_line(ptr, level, fmt, vl, msg, sizeof(msg), &print_prefix);

	switch (level) {
	case AV_LOG_VERBOSE:
	case AV_LOG_DEBUG: if (trace_movies) ffnx_trace(msg); break;
	case AV_LOG_INFO:
	case AV_LOG_WARNING: if (trace_movies) ffnx_info(msg); break;
	case AV_LOG_ERROR:
	case AV_LOG_FATAL:
	case AV_LOG_PANIC: ffnx_error(msg); break;
	}

	if (level <= AV_LOG_ERROR) {
		FFNxStackWalker sw;
		sw.ShowCallstack();
	}
}

void ffnx_log_current_pc_specs()
{
	// Start report of PC specs
	ffnx_info("--- PC SPECS ---\n");

	// CPU
	auto cpus = hwinfo::getAllCPUs();
	for (const auto& cpu : cpus) {
		ffnx_info("   CPU: %s\n", cpu.modelName().c_str());
	}

	// GPU
	auto gpus = hwinfo::getAllGPUs();
	for (auto& gpu : gpus) {
		uint16_t vendorId = std::stoi(gpu.vendor_id(), 0, 16), deviceId = std::stoi(gpu.device_id(), 0, 16);
		if (
			(newRenderer.getCaps()->vendorId == vendorId && newRenderer.getCaps()->deviceId == deviceId) ||
			(newRenderer.getCaps()->vendorId == vendorId && renderer_backend == RENDERER_BACKEND_OPENGL)
		)
			ffnx_info("   GPU: %s (%dMB) - Driver: %s - Backend: %s\n", gpu.name().c_str(), (int)(gpu.memory_Bytes() / 1024.0 / 1024.0), gpu.driverVersion().c_str(), newRenderer.currentRenderer.c_str());
	}

	// RAM
	hwinfo::Memory memory;
	ffnx_info("   RAM: %dMB/%dMB (Free: %dMB)\n", (int)((memory.total_Bytes() - memory.free_Bytes()) / 1024.0 / 1024.0), (int)(memory.total_Bytes() / 1024.0 / 1024.0), (int)(memory.free_Bytes() / 1024.0 / 1024.0));

	// OS
	hwinfo::OS os;
	ffnx_info("    OS: %s %s (build %s)\n", os.name().c_str(), (os.is32bit() ? "32 bit" : "64 bit"), os.version().c_str());

	// WINE+PROTON
	const char* env_wineloader = std::getenv("WINELOADER");
	if (env_wineloader != NULL) // Are we running under Wine/Proton?
	{
		ffnx_info("  WINE: v%s\n", GetWineVersion());

		const std::regex proton_regex("([Pp]roton[\\s\\-\\w.()]+)");
		std::smatch base_match;
		std::string s_wineloader = std::string(env_wineloader);
		if (std::regex_search(s_wineloader, base_match, proton_regex))
			ffnx_info("PROTON: %s\n", base_match[1].str().c_str());
	}

	// End report of PC specs
	ffnx_info("----------------\n");
}

// figure out which game module is currently running by looking at the game's
// own mode variable and the address of the current main function
struct game_mode *getmode()
{
	static uint32_t last_mode = 0;
	VOBJ(game_obj, game_object, common_externals.get_game_object());
	uint32_t i;

	// find exact match, mode and main loop both match
	for(i = 0; i < num_modes; i++)
	{
		struct game_mode *m = &modes[i];

		if(m->main_loop == (uint32_t)VREF(game_object, game_loop_obj).main_loop && m->mode == *common_externals._mode)
		{
			if(last_mode != m->mode)
			{
				if(m->trace) ffnx_trace("%s\n", m->name);
				last_mode = m->mode;
			}

			if (trace_all) ffnx_trace("getmode: exact match - driver_mode: %u - mode: %u - name: %s\n", m->driver_mode, m->mode, m->name);

			return m;
		}
	}

	// FF8 has BATTLE and CARDGAME baked inside the same module
	// Only for this case use a custom match logic
	if (ff8)
	{
		for(i = 0; i < num_modes; i++)
		{
			struct game_mode *m = &modes[i];

			if (*ff8_externals.is_card_game == 1 && *common_externals._mode == m->mode)
			{
				if (trace_all) ffnx_trace("getmode: exact match - driver_mode: %u - mode: %u - name: %s\n", m->driver_mode, m->mode, m->name);

				return m;
			}
		}
	}

	// if there is no exact match, try to find a match by main loop only
	for(i = 0; i < num_modes; i++)
	{
		struct game_mode *m = &modes[i];

		if(m->main_loop && m->main_loop == (uint32_t)VREF(game_object, game_loop_obj).main_loop)
		{
			if(last_mode != m->mode)
			{
				if(m->mode != *common_externals._mode && m->trace)
				{
					uint32_t j;
					struct game_mode *_m = NULL;

					for(j = 0; j < num_modes - 1; j++)
					{
						_m = &modes[j];

						if(_m->mode == *common_externals._mode) break;
					}

					if (trace_all) ffnx_trace("getmode: mismatched mode, %s -> %s\n", _m->name, m->name);
				}
				if(m->trace) ffnx_trace("%s\n", m->name);
				last_mode = m->mode;
			}

			if (trace_all) ffnx_trace("getmode: no exact match, found by main loop - driver_mode: %u - mode: %u - name: %s\n", m->driver_mode, m->mode, m->name);

			return m;
		}
	}

	// finally, ignore main loop and try to match by mode only
	for(i = 0; i < num_modes; i++)
	{
		struct game_mode *m = &modes[i];

		if(m->mode == *common_externals._mode)
		{
			if(last_mode != m->mode)
			{
				if(m->trace) ffnx_trace("%s\n", m->name);
				last_mode = m->mode;
			}

			if (trace_all) ffnx_trace("getmode: ignore main loop, match by mode only - driver_mode: %u - mode: %u - name: %s\n", m->driver_mode, m->mode, m->name);

			return m;
		}
	}

	if(*common_externals._mode != last_mode)
	{
		ffnx_unexpected("unknown mode (%i, 0x%x)\n", *common_externals._mode, (uint32_t)VREF(game_object, game_loop_obj).main_loop);
		last_mode = *common_externals._mode;
	}

	if(!ff8) return &modes[4];
	else return &modes[11];
}

// game mode usually doesn't change in the middle of a frame and even if it
// does we usually don't care until the next frame so we can safely cache it to
// avoid constant lookups
struct game_mode *getmode_cached()
{
	static uint32_t last_frame = -1;
	static struct game_mode *last_mode;

	if(frame_counter != last_frame)
	{
		last_mode = getmode();
		last_frame = frame_counter;
	}

	return last_mode;
}

char* get_current_field_name()
{
	if (ff8) {
		return common_externals.current_field_name;
	}

	char *ret = strrchr(ff7_externals.field_file_name, 92);

	if (ret) ret += 1;

	return ret;
}

bool maximized(HWND hwnd) {
	WINDOWPLACEMENT placement;
	if (!GetWindowPlacement(hwnd, &placement)) {
				return false;
	}

	return placement.showCmd == SW_MAXIMIZE;
}

/* Adjust client rect to not spill over monitor edges when maximized.
* rect(in/out): in: proposed window rect, out: calculated client rect
* Does nothing if the window is not maximized.
*/
void adjust_maximized_client_rect(HWND window, RECT& rect) {
	if (!maximized(window)) {
			return;
	}

	auto monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
	if (!monitor) {
			return;
	}

	MONITORINFO monitor_info{};
	monitor_info.cbSize = sizeof(monitor_info);
	if (!GetMonitorInfoW(monitor, &monitor_info)) {
			return;
	}

	// when maximized, make the client area fill just the monitor (without task bar) rect,
	// not the whole window rect which extends beyond the monitor.
	rect = monitor_info.rcWork;
}


bool composition_enabled() {
	BOOL composition_enabled = FALSE;
	bool success = DwmIsCompositionEnabled(&composition_enabled) == S_OK;
	return composition_enabled && success;
}

LRESULT window_hit_test(HWND hwnd, POINT cursor) {
	// identify borders and corners to allow resizing the window.
	// Note: On Windows 10, windows behave differently and
	// allow resizing outside the visible window frame.
	// This implementation does not replicate that behavior.
	const POINT border{
			::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
			::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)
	};
	SetRectEmpty(&gameWindowRect);
	if (!::GetWindowRect(hwnd, &gameWindowRect)) {
			return HTNOWHERE;
	}

	enum region_mask {
			client = 0b0000,
			left   = 0b0001,
			right  = 0b0010,
			top    = 0b0100,
			bottom = 0b1000,
	};

	const auto result =
			left    * (cursor.x <  (gameWindowRect.left   + border.x)) |
			right   * (cursor.x >= (gameWindowRect.right  - border.x)) |
			top     * (cursor.y <  (gameWindowRect.top    + border.y)) |
			bottom  * (cursor.y >= (gameWindowRect.bottom - border.y));

	switch (result) {
			case left          : return HTLEFT;
			case right         : return HTRIGHT;
			case top           : return HTTOP;
			case bottom        : return HTBOTTOM;
			case top | left    : return HTTOPLEFT;
			case top | right   : return HTTOPRIGHT;
			case bottom | left : return HTBOTTOMLEFT;
			case bottom | right: return HTBOTTOMRIGHT;
			case client        : return HTCAPTION;
			default            : return HTNOWHERE;
	}
}

void set_window_shadow() {
	if (composition_enabled()) {
		static const MARGINS shadow_state{1,1,1,1};
		::DwmExtendFrameIntoClientArea(gameHwnd, &shadow_state);
	}
}

void toggle_borderless() {
	DWORD style = borderless ? WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX : WS_OVERLAPPEDWINDOW;

	SetWindowLongPtr(gameHwnd, GWL_STYLE, style | WS_VISIBLE);

	set_window_shadow();

	SetWindowPos(gameHwnd, HWND_TOP, 0, 0, borderless ? window_size_x : gameWindow[GameWindowState::CURRENT].w, borderless ? window_size_y : gameWindow[GameWindowState::CURRENT].h, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	ShowWindow(gameHwnd, SW_SHOW);
}

void calc_window_size(uint32_t width, uint32_t height) {
	SetRectEmpty(&gameWindowRect);

	gameWindowRect.right = width;
	gameWindowRect.bottom = height;

	AdjustWindowRect(&gameWindowRect, WS_OVERLAPPEDWINDOW, false);
	gameWindow[GameWindowState::CURRENT].w = gameWindowRect.right - gameWindowRect.left;
	gameWindow[GameWindowState::CURRENT].h = gameWindowRect.bottom - gameWindowRect.top;

	// Center the window on the screen
	gameWindow[GameWindowState::CURRENT].x = (dmCurrentScreenSettings.dmPelsWidth / 2) - (gameWindow[GameWindowState::CURRENT].w / 2);
	gameWindow[GameWindowState::CURRENT].y = (dmCurrentScreenSettings.dmPelsHeight / 2) - (gameWindow[GameWindowState::CURRENT].h / 2);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (proxyWndProc)
	{
		switch (uMsg)
		{
		case WM_NCCALCSIZE:
			if (wParam == TRUE && borderless) {
					auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
					adjust_maximized_client_rect(hwnd, params.rgrc[0]);
					return 0;
			}
			break;
		case WM_NCHITTEST:
			// When we have no border or title bar, we need to perform our
			// own hit testing to allow resizing and moving.
			if (borderless) {
					return window_hit_test(gameHwnd, POINT{
							GET_X_LPARAM(lParam),
							GET_Y_LPARAM(lParam)
					});
			}
			break;
		case WM_NCACTIVATE:
			if (!composition_enabled()) {
					// Prevents window frame reappearing on window activation
					// in "basic" theme, where no aero shadow is present.
					return 1;
			}
			break;
		case WM_KEYDOWN:
			if ((::GetKeyState(VK_CONTROL) & 0x8000) != 0)
			{
				switch (LOWORD(wParam))
				{
				case VK_F11:
					newRenderer.toggleCaptureFrame();
					break;
				}
			}
			else if ((::GetKeyState(VK_SHIFT) & 0x8000) != 0)
			{
				switch (LOWORD(wParam))
				{
				case VK_LEFT:
					if (!widescreen_enabled)
					{
						aspect_ratio--;
						if (aspect_ratio < 0) aspect_ratio = AR_STRETCH;

						show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Aspect Ratio Mode: %u", aspect_ratio);

						newRenderer.reset();
					}
					break;
				case VK_RIGHT:
					if (!widescreen_enabled)
					{
						aspect_ratio++;
						if (aspect_ratio > AR_STRETCH) aspect_ratio = AR_ORIGINAL;

						show_popup_msg(TEXTCOLOR_LIGHT_BLUE, "Aspect Ratio Mode: %u", aspect_ratio);

						newRenderer.reset();
					}
					break;
				case VK_RETURN:
					if (!fullscreen)
					{
						borderless = !borderless;
						toggle_borderless();
						newRenderer.reset();
					}
					break;
				}
			}
			break;
		case WM_SIZE:
			window_size_x = (long)LOWORD(lParam);
			window_size_y = (long)HIWORD(lParam);

			calc_window_size(window_size_x, window_size_y);

			if (wParam != SIZE_MINIMIZED) newRenderer.reset();

			if (wParam == SIZE_MAXIMIZED) gameWindowWasMaximized = true;
			else if (wParam == SIZE_RESTORED && gameWindowWasMaximized) gameWindowWasMaximized = false;
			else break;
		case WM_ENTERSIZEMOVE:
			gameWindowMoving = true;
			break;
		case WM_MOVE:
			if (gameWindowMoving) {
				if (!ff8){
					if (ff7_externals.movie_object->is_playing) ff7_core_game_loop();
				}
			}
			break;
		case WM_EXITSIZEMOVE:
			gameWindowMoving = false;
			newRenderer.reset();
			break;
		case WM_MENUCHAR:
			if (LOWORD(wParam) == VK_RETURN)
			{
				if (fullscreen)
				{
					// Bring back the original resolution
					ChangeDisplaySettingsEx(0, &dmCurrentScreenSettings, 0, CDS_FULLSCREEN, 0);

					// Move to window
					SetWindowLongPtr(gameHwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
					MoveWindow(gameHwnd, gameWindow[GameWindowState::PRE_FULLSCREEN].x, gameWindow[GameWindowState::PRE_FULLSCREEN].y, gameWindow[GameWindowState::PRE_FULLSCREEN].w, gameWindow[GameWindowState::PRE_FULLSCREEN].h, true);

					// Show the cursor
					while (ShowCursor(true) < 0);

					fullscreen = false;
				}
				else
				{
					// Save current window state
					gameWindow[GameWindowState::PRE_FULLSCREEN] = gameWindow[GameWindowState::CURRENT];

					// Bring back the user resolution
					ChangeDisplaySettingsEx(0, &dmNewScreenSettings, 0, CDS_FULLSCREEN, 0);

					// Move to fullscreen
					SetWindowLongPtr(gameHwnd, GWL_STYLE, WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
					MoveWindow(gameHwnd, 0, 0, dmNewScreenSettings.dmPelsWidth, dmNewScreenSettings.dmPelsHeight, true);

					// Hide the cursor
					if (!enable_devtools) while (ShowCursor(false) >= 0);

					fullscreen = true;
					borderless = false;
				}

				newRenderer.reset();

				return MAKELRESULT(0, MNC_CLOSE);
			}
			break;
		case WM_CLOSE:
		case WM_QUIT:
			if (ff8) ff8_release_movie_objects();
			else ff7_release_movie_objects();

			gl_cleanup_deferred();

			SetWindowLongA(gameHwnd, GWL_WNDPROC, (LONG)common_externals.engine_wndproc);
			break;
		}

		HandleInputEvents(uMsg, wParam, lParam);
	}

	gamehacks.processKeyboardInput(uMsg, wParam, lParam);

	return common_externals.engine_wndproc(hwnd, uMsg, wParam, lParam);
}

int common_create_window(HINSTANCE hInstance, struct game_obj* game_object)
{
	uint32_t ret = FALSE;

	VOBJ(game_obj, game_object, game_object);

	HWND hWnd;
	HDC hdc;
	WNDCLASSA WndClass;

	// Init Steam API
	if(steam_edition || enable_steam_achievements)
	{
		// generate automatically steam_appid.txt
		if(!steam_edition){
			std::ofstream steam_appid_file("steam_appid.txt");
			steam_appid_file << ((ff8) ? FF8_APPID : FF7_APPID);
			steam_appid_file.close();
		}

		if (SteamAPI_RestartAppIfNecessary((ff8) ? FF8_APPID : FF7_APPID))
		{
			MessageBoxA(gameHwnd, "Steam Error - Could not find steam_appid.txt containing the app ID of the game.\n", "Steam App ID Wrong", 0);
			ffnx_error( "Steam Error - Could not find steam_appid.txt containing the app ID of the game.\n" );
			return 1;
		}
		if (!SteamAPI_Init())
		{
			MessageBoxA(gameHwnd, "Steam Error - Steam must be running to play this game with achievements (SteamAPI_Init() failed).\n", "Steam not running error", 0);
			ffnx_error( "Steam Error - Steam must be running to play this game with achievements (SteamAPI_Init() failed).\n" );
			return 1;
		}
		if (ff8)
			g_FF8SteamAchievements = std::make_unique<SteamAchievementsFF8>();
		else
			g_FF7SteamAchievements = std::make_unique<SteamAchievementsFF7>();
	}

	// fetch current user screen settings
	EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dmCurrentScreenSettings);

	// store all settings so we can change only few parameters
	dmNewScreenSettings = dmCurrentScreenSettings;

	// read original resolution
	game_width = VREF(game_object, window_width);
	game_height = VREF(game_object, window_height);

	// Assign a legit name to the Window
	if (ff8)
	{
		VRASS(game_object, window_title, "Final Fantasy VIII");
	}
	else
	{
		VRASS(game_object, window_title, "Final Fantasy VII");
	}

	if (window_size_x == 0 || window_size_y == 0)
	{
		if (fullscreen)
		{
			window_size_x = dmCurrentScreenSettings.dmPelsWidth;
			window_size_y = dmCurrentScreenSettings.dmPelsHeight;
		}
		else
		{
			window_size_x = game_width;
			window_size_y = game_height;
		}
	}
	else
	{
		// custom resolution
		dmNewScreenSettings.dmSize = sizeof(dmNewScreenSettings);
		dmNewScreenSettings.dmPelsWidth = window_size_x;
		dmNewScreenSettings.dmPelsHeight = window_size_y;
		dmNewScreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

		if (refresh_rate)
		{
			dmNewScreenSettings.dmDisplayFrequency = refresh_rate;
			dmNewScreenSettings.dmFields |= DM_DISPLAYFREQUENCY;
		}

		if (fullscreen)
		{
			if (ChangeDisplaySettingsEx(0, &dmNewScreenSettings, 0, CDS_FULLSCREEN, 0) != DISP_CHANGE_SUCCESSFUL)
			{
				MessageBoxA(gameHwnd, "Failed to set the requested fullscreen mode, reverting to the original resolution.\n", "Error", 0);
				ffnx_error("failed to set fullscreen mode\n");
				window_size_x = dmCurrentScreenSettings.dmPelsWidth;
				window_size_y = dmCurrentScreenSettings.dmPelsHeight;
			}
		}
	}

	WndClass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	WndClass.lpfnWndProc = WindowProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
	WndClass.hCursor = LoadCursorA(0, (LPCSTR)IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = 0;
	WndClass.lpszClassName = VREF(game_object, window_class);

	if (RegisterClassA(&WndClass))
	{
		// If fullscreen is the starting mode, use the native game resolution as a window size starting point
		if (fullscreen)
		{
			calc_window_size(game_width, game_height);

			// Save current window state
			gameWindow[GameWindowState::PRE_FULLSCREEN] = gameWindow[GameWindowState::CURRENT];
		}
		// Otherwise if windowed mode is requested on start, use the given resolution as a starting point
		else
			calc_window_size(window_size_x, window_size_y);

		hWnd = CreateWindowExA(
			WS_EX_APPWINDOW,
			VREF(game_object, window_class),
			VREF(game_object, window_title),
			fullscreen ? WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS : WS_OVERLAPPEDWINDOW,
			fullscreen ? 0 : gameWindow[GameWindowState::CURRENT].x,
			fullscreen ? 0 : gameWindow[GameWindowState::CURRENT].y,
			fullscreen ? window_size_x : gameWindow[GameWindowState::CURRENT].w,
			fullscreen ? window_size_y : gameWindow[GameWindowState::CURRENT].h,
			0,
			0,
			hInstance,
			0
		);

		VRASS(game_object, hwnd, hWnd);

		gameHinstance = hInstance;
		gameHwnd = hWnd;

		if (hWnd)
		{
			ret = TRUE;

			ShowWindow(hWnd, SW_SHOWNORMAL);
			UpdateWindow(hWnd);
			hdc = GetDC(hWnd);
			if (hdc)
			{
				VRASS(game_object, dc_horzres, GetDeviceCaps(hdc, HORZRES));
				VRASS(game_object, dc_vertres, GetDeviceCaps(hdc, VERTRES));
				VRASS(game_object, dc_bitspixel, GetDeviceCaps(hdc, BITSPIXEL));
				if (!VREF(game_object, colordepth))
					VRASS(game_object, colordepth, VREF(game_object, dc_bitspixel));
				ReleaseDC(hWnd, hdc);
			}

			if (ret && VREF(game_object, engine_loop_obj.init))
			{
				if (ff8) ff8_init_hooks(game_object);
				else ff7_init_hooks(game_object);

				replace_function(common_externals.get_keyboard_state, &GetGameKeyState);
				// catch all applog messages
				replace_function(common_externals.debug_print, external_debug_print);
				if (more_debug)
				{
					replace_function(common_externals.debug_print2, external_debug_print2);
				}

#ifdef NO_EXT_HEAP
				replace_function((uint32_t)common_externals.assert_free, ext_free);
				replace_function((uint32_t)common_externals.assert_malloc, ext_malloc);
				replace_function((uint32_t)common_externals.assert_calloc, ext_calloc);
#endif

				if (widescreen_enabled || enable_uncrop) widescreen.init();

				// Init renderer
				newRenderer.init();

				// Init GameHacks
				gamehacks.init();

				max_texture_size = newRenderer.getCaps()->limits.maxTextureSize;
				ffnx_info("Max texture size: %ix%i\n", max_texture_size, max_texture_size);

				newRenderer.prepareFFNxLogo();

				newRenderer.prepareEnvBrdf();

				newRenderer.prepareGamutLUTs();

				// perform any additional initialization that requires the rendering environment to be set up
				field_init();
				world_init();
				music_init();
				sfx_init();
				voice_init();

				if (enable_ffmpeg_videos)
				{
					movie_init();
				}
				if (ff8)
				{
					vram_init();
					if (ff8_fix_uv_coords_precision) {
						uv_patch_init();
					}
					vibration_init();
				}

				exe_data_init();

				// Init Day Night Cycle
				if (!ff8 && enable_time_cycle) ff7::time.init();

				// Init Lighting
				if (!ff8 && enable_lighting) lighting.init();

				ffnx_log_current_pc_specs();

				// enable verbose logging for FFMpeg
				av_log_set_level(AV_LOG_VERBOSE);
				av_log_set_callback(ffmpeg_log_callback);

				ffnx_inject_driver(game_object);

				if (VREF(game_object, engine_loop_obj.init)(game_object))
				{
					if (!fullscreen || enable_devtools)
					{
						// Show the cursor
						while (ShowCursor(true) < 0);
					}
					else if (fullscreen)
					{
						// Hide the cursor
						while (ShowCursor(false) >= 0);
					}

					nxAudioEngine.init();

					if (borderless) toggle_borderless();

					if (VREF(game_object, engine_loop_obj.enter_main))
						VREF(game_object, engine_loop_obj.enter_main)(game_object);
				}
				else
				{
					ret = FALSE;
				}
			}
		}
	}

	return ret;
}

// called by the game before rendering starts, after the driver object has been
// created, we use this opportunity to initialize our default OpenGL render
// state
uint32_t common_init(struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: init\n");

	newRenderer.setBlendMode(RendererBlendMode::BLEND_NONE);

	texture_format = common_externals.create_texture_format();
	common_externals.make_pixelformat(32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000, texture_format);
	common_externals.add_texture_format(texture_format, game_object);

	nxAudioEngine.setMusicMasterVolume(external_music_volume / 100.0f);
	nxAudioEngine.setSFXMasterVolume(external_sfx_volume / 100.0f);
	nxAudioEngine.setAmbientMasterVolume(external_ambient_volume / 100.0f);
	nxAudioEngine.setVoiceMasterVolume(external_voice_volume / 100.0f);

	proxyWndProc = true;

	return true;
}

// called by the game just before it exits, we need to make sure the game
// doesn't crash after we're gone
void common_cleanup(struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: cleanup\n");

	if (steam_edition)
	{
		if (!ff8)
		{
			// Write ff7sound.cfg
			char ff7soundPath[260]{0};
			get_userdata_path(ff7soundPath, sizeof(ff7soundPath), false);
			PathAppendA(ff7soundPath, "ff7sound.cfg");
			FILE *ff7sound = fopen(ff7soundPath, "wb");

			if (ff7sound)
			{
				fwrite(&external_sfx_volume, sizeof(DWORD), 1, ff7sound);
				fwrite(&external_music_volume, sizeof(DWORD), 1, ff7sound);
				fclose(ff7sound);
			}
		}
	}

	// Shutdown Steam API
	if(steam_edition || enable_steam_achievements)
		SteamAPI_Shutdown();

	nxAudioEngine.cleanup();
	newRenderer.shutdown();
}

// unused and unnecessary
uint32_t common_lock(uint32_t surface)
{
	if(trace_all) ffnx_trace("dll_gfx: lock %i\n", surface);

	return true;
}

// unused and unnecessary
uint32_t common_unlock(uint32_t surface)
{
	if(trace_all) ffnx_trace("dll_gfx: unlock %i\n", surface);

	return true;
}

// called by the game at the end of each frame to swap the front and back
// buffers
void common_flip(struct game_obj *game_object)
{
	if (trace_all) ffnx_trace("dll_gfx: flip (%i)\n", frame_counter);

	VOBJ(game_obj, game_object, game_object);
	static struct timeb last_frame;
	static uint32_t fps_counters[3] = {0, 0, 0};
	time_t last_seconds = last_frame.time;
	struct game_mode *mode = getmode_cached();

	// Update RAM usage info
	GlobalMemoryStatusEx(&last_ram_state);

	// Draw with lighting
	if (!ff8 && enable_lighting) lighting.draw(game_object);

	// draw any z-sorted content now that we're done drawing everything else
	gl_draw_sorted_deferred();

	newRenderer.drawOverlay();

	if (fullscreen || borderless)
	{
		static uint32_t col = 4;
		uint32_t row = 1;

		if (show_version)
		{
			gl_draw_text(col, row++, text_colors[TEXTCOLOR_GRAY], 255, "Version: " VERSION);
		}

		if (show_renderer_backend)
		{
			gl_draw_text(col, row++, text_colors[TEXTCOLOR_GREEN], 255, "Renderer: %s", newRenderer.currentRenderer.c_str());
		}

		if (show_fps)
		{
			// average last two seconds and round up for our FPS counter
			gl_draw_text(col, row++, text_colors[TEXTCOLOR_YELLOW], 255, "FPS: %2.1lf", frame_rate);
		}

		if (show_stats)
		{
			static uint32_t color = text_colors[TEXTCOLOR_PINK];

#ifdef HEAP_DEBUG
			gl_draw_text(col, row++, color, 255, "Allocations: %u", allocs);
#endif
#ifdef PROFILE
			gl_draw_text(col, row++, color, 255, "Profiling: %I64u us", (time_t)((profile_total * 1000000.0) / VREF(game_object, countspersecond)));
#endif
			gl_draw_text(col, row++, color, 255, "RAM usage: %llu MB / %llu MB", (last_ram_state.ullTotalVirtual - last_ram_state.ullAvailVirtual) / (1024 * 1024), last_ram_state.ullTotalVirtual / ( 1024 * 1024 ));
			gl_draw_text(col, row++, color, 255, "Textures: %u", stats.texture_count);
			gl_draw_text(col, row++, color, 255, "External textures: %u", stats.external_textures);
			gl_draw_text(col, row++, color, 255, "Texture reloads: %u", stats.texture_reloads);
			gl_draw_text(col, row++, color, 255, "Palette writes: %u", stats.palette_writes);
			gl_draw_text(col, row++, color, 255, "Palette changes: %u", stats.palette_changes);
			gl_draw_text(col, row++, color, 255, "Zsort layers: %u", stats.deferred);
			gl_draw_text(col, row++, color, 255, "Vertices: %u", stats.vertex_count);
			gl_draw_text(col, row++, color, 255, "Timer: %I64u", stats.timer);
		}
	}
	else
	{
		char newWindowTitle[1024];

		strcpy_s(newWindowTitle, 1024, VREF(game_object, window_title));

		// Append chosen rendering engine
		if (show_renderer_backend)
		{
			char tmp[64];
			sprintf_s(tmp, 64, " (%s)", newRenderer.currentRenderer.c_str());
			strcat_s(newWindowTitle, 1024, tmp);
		}

		if (show_version)
		{
			char tmp[16];
			sprintf_s(tmp, 16, " " VERSION);
			strcat_s(newWindowTitle, 1024, tmp);
		}

		if (show_stats)
		{
			char tmp[768];
			sprintf_s(tmp, 768, " | RAM: %llu MB / %llu MB | nTex: %u | nExt.Tex: %u", (last_ram_state.ullTotalVirtual - last_ram_state.ullAvailVirtual) / (1024 * 1024), last_ram_state.ullTotalVirtual / (1024 * 1024), stats.texture_count, stats.external_textures);
			strcat_s(newWindowTitle, 1024, tmp);
		}

		if (show_fps)
		{
			char tmp[64];
			sprintf_s(tmp, 64, " | FPS: %2.1lf", frame_rate);
			strcat_s(newWindowTitle, 1024, tmp);
		}

		SetWindowTextA(gameHwnd, newWindowTitle);
	}

	fps_counters[0]++;
	ftime(&last_frame);

	if (last_seconds != last_frame.time)
	{
		fps_counters[2] = fps_counters[1];
		fps_counters[1] = fps_counters[0];
		fps_counters[0] = 0;
	}

	frame_rate = (fps_counters[1] + fps_counters[2] + 1) / 2;

	VRASS(game_object, fps, frame_rate);

	// if there is an active popup message, display it
	if(popup_ttl > 0)
	{
		if(gl_draw_text(4, newRenderer.getStats()->textHeight - 2, popup_color, (popup_ttl * 255) / POPUP_TTL_MAX, popup_msg))
		{
			uint32_t diff = (POPUP_TTL_MAX - popup_ttl) / 10;

			if(diff == 0) popup_ttl--;
			else if(diff > popup_ttl) popup_ttl = 0;
			else popup_ttl -= diff;
		}
	}

	// reset per-frame stats
	stats.texture_reloads = 0;
	stats.palette_writes = 0;
	stats.palette_changes = 0;
	stats.vertex_count = 0;
	stats.deferred = 0;

	newRenderer.show();

	current_state.texture_filter = true;
	current_state.fb_texture = false;

	// fix unresponsive quit menu
	if(!ff8 && VREF(game_object, gfx_reset))
	{
		MSG msg;

		if(PeekMessageA(&msg, 0, 0, 0, 1))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Enable XInput if a compatible gamepad is detected while playing the game, otherwise continue with native DInput
	if (!xinput_connected && gamepad.CheckConnection())
	{
		if (trace_all || trace_gamepad) ffnx_trace("XInput controller: connected.\n");

		xinput_connected = true;

		// Release any previous DirectInput attached controller, if any
		joystick.Clean();
	}
	else if (xinput_connected && !gamepad.CheckConnection())
	{
		if (trace_all || trace_gamepad) ffnx_trace("XInput controller: disconnected.\n");

		xinput_connected = false;
	}

	frame_counter++;

	// We need to process Gamepad input on each frame
	gamehacks.processGamepadInput();

	// Update day night time cycle
	if (!ff8 && enable_time_cycle) ff7::time.update();

	// Handle main menu background music
	handle_mainmenu_playback();

	// FF8 does not clear the screen properly in the card game module
	if (ff8)
	{
		if (mode->driver_mode == MODE_CARDGAME) common_clear_all(0);

		if (ff8_ssigpu_debug) ff8_externals.refresh_vram_window();

		ff8_handle_ambient_playback();
	}
	else
	{
		if (ff7_do_reset)
		{
			if (ff7_externals.movie_object->is_playing)
			{
				ff7_do_reset = false;
			}
			else
			{
				switch(mode->driver_mode)
				{
					// Skip reset on these mode(s)
					case MODE_MENU:
					case MODE_MAIN_MENU:
					case MODE_GAMEOVER:
					case MODE_CREDITS:
						ff7_do_reset = false;
						break;
					case MODE_BATTLE:
						if (*ff7_externals.battle_mode < 6) break;
					default:
						ff7_externals.reset_game_obj_sub_5F4971(game_object);
						break;
				}
			}
		}

		*ff7_externals.swirl_limit_fps = 1;

		ff7_handle_ambient_playback();
		ff7_handle_voice_playback();
		ff7_handle_wmode_reset();
		ff7::field::ff7_field_handle_blink_reset();
	}

	// Steamworks SDK API run callbacks
	if(steam_edition || enable_steam_achievements)
		SteamAPI_RunCallbacks();

}

// called by the game to clear an aspect of the back buffer, mostly called from
// clear_all below
void common_clear(uint32_t clear_color, uint32_t clear_depth, uint32_t unknown, struct game_obj *game_object)
{
	if(gl_defer_clear_buffer(clear_color, clear_depth, game_object)) return;

	uint32_t mode = getmode_cached()->driver_mode;

	if(trace_all) ffnx_trace("dll_gfx: clear %i %i %i\n", clear_color, clear_depth, unknown);

	if (!ff8 && enable_lighting) newRenderer.clearShadowMap();

	newRenderer.setClearFlags(
		clear_color || mode == MODE_MENU || mode == MODE_MAIN_MENU || mode == MODE_CONDOR,
		clear_depth
	);
}

// called by the game to clear the entire back buffer
void common_clear_all(struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: clear_all\n");

	common_clear(true, true, true, game_object);
}

// called by the game to setup a viewport inside the game window, allowing it
// to clip drawing to the requested area
void common_setviewport(uint32_t _x, uint32_t _y, uint32_t _w, uint32_t _h, struct game_obj *game_object)
{
	uint32_t mode = getmode_cached()->driver_mode;

	if(trace_all) ffnx_trace("dll_gfx: setviewport %i %i %i %i\n", _x, _y, _w, _h);

	current_state.viewport[0] = _x;
	current_state.viewport[1] = _y;
	current_state.viewport[2] = _w;
	current_state.viewport[3] = _h;

	newRenderer.setScissor(
		_x,
		_y,
		_w,
		_h
	);

	// emulate the transformation applied by an equivalent Direct3D viewport
	d3dviewport_matrix._11 = (float)_w / (float)game_width;
	// no idea why this is necessary
	if(!ff8 && mode == MODE_BATTLE) d3dviewport_matrix._22 = 1.0f;
	else d3dviewport_matrix._22 = (float)_h / (float)game_height;
	d3dviewport_matrix._41 = (((float)_x + (float)_w / 2.0f) - (float)game_width / 2.0f) / ((float)game_width / 2.0f);
	d3dviewport_matrix._42 = -(((float)_y + (float)_h / 2.0f) - (float)game_height / 2.0f) / ((float)game_height / 2.0f);
}

// called by the game to set the background color which the back buffer will be
// cleared to
void common_setbg(struct bgra_color *color, struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: setbg\n");

	newRenderer.setBackgroundColor(
		color->r,
		color->g,
		color->b,
		ff8 ? color->a : 0.0f
	);
}

// called by the game to initialize a polygon_set structure
// we don't really need to do anything special here
uint32_t common_prepare_polygon_set(struct polygon_set *polygon_set)
{
	VOBJ(polygon_set, polygon_set, polygon_set);

	if(VPTR(polygon_set)) VRASS(polygon_set, indexed_primitives, (indexed_primitive**)external_calloc(VREF(polygon_set, numgroups), 4));

	return true;
}

// called by the game to load a group from a .p file into a renderable format
uint32_t common_load_group(uint32_t group_num, struct matrix_set *matrix_set, struct p_hundred *hundred_data, struct p_group *group_data, struct polygon_data *polygon_data, struct polygon_set *polygon_set, struct game_obj *game_object)
{
	if(!ff8) return ff7gl_load_group(group_num, matrix_set, hundred_data, group_data, polygon_data, (struct ff7_polygon_set *)polygon_set, (struct ff7_game_obj *)game_object);
	else return common_externals.generic_load_group(group_num, matrix_set, hundred_data, group_data, polygon_data, polygon_set, game_object);
}

// called by the game to update one of the matrices in a matrix_set structure
void common_setmatrix(uint32_t unknown, struct matrix *matrix, struct matrix_set *matrix_set, struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: setmatrix\n");

	switch(unknown)
	{
		case 0:
			if(!matrix_set->matrix_world) matrix_set->matrix_world = matrix;
			else memcpy(matrix_set->matrix_world, matrix, sizeof(*matrix));
			break;

		case 1:
			if(!matrix_set->matrix_view) matrix_set->matrix_view = matrix;
			else memcpy(matrix_set->matrix_view, matrix, sizeof(*matrix));
			break;

		case 2:
			if(!matrix_set->matrix_projection) matrix_set->matrix_projection = matrix;
			else memcpy(matrix_set->matrix_projection, matrix, sizeof(*matrix));
			break;
	}
}

// called by the game to apply light information to the current polygon set
void common_light_polygon_set(struct polygon_set *polygon_set, struct light *light)
{
	if(ff8 || game_lighting == GAME_LIGHTING_ORIGINAL) common_externals.generic_light_polygon_set(polygon_set, light);
}

// called by the game to unload a texture
void common_unload_texture(struct texture_set *texture_set)
{
	uint32_t i;
	VOBJ(texture_set, texture_set, texture_set);

	if(trace_all) ffnx_trace("dll_gfx: unload_texture 0x%x\n", VPTR(texture_set));

	if(!VPTR(texture_set)) return;
	if(!VREF(texture_set, texturehandle)) return;
	if(!VREF(texture_set, ogl.gl_set)) return;

	struct gl_texture_set *gl_set = VREF(texture_set, ogl.gl_set);

	// Destroy original static textures
	for (uint32_t idx = 0; idx < VREF(texture_set, ogl.gl_set->textures); idx++)
	{
		newRenderer.deleteTexture(VREF(texture_set, texturehandle[idx]));
	}

	// Destroy animated textures
	if (gl_set->is_animated)
	{
		for(std::map<std::string,uint32_t>::iterator it = gl_set->animated_textures.begin(); it != gl_set->animated_textures.end(); ++it) {
			newRenderer.deleteTexture(it->second);
		}
		gl_set->animated_textures.clear();
	}

	// Destroy additional textures
	for (short slot = RendererTextureSlot::TEX_NML; slot < RendererTextureSlot::COUNT; slot++)
		newRenderer.deleteTexture(gl_set->additional_textures[slot]);

	external_free(VREF(texture_set, texturehandle));
	delete VREF(texture_set, ogl.gl_set);

	VRASS(texture_set, texturehandle, 0);
	VRASS(texture_set, ogl.gl_set, 0);

	stats.texture_count--;

	if(VREF(texture_set, ogl.external)) stats.external_textures--;
	VRASS(texture_set, ogl.external, false); // texture_set can be reused (FF8)

	// remove any other references to this texture
	gl_check_deferred(texture_set);

	for(i = 0; i < scene_stack_pointer; i++)
	{
		if(scene_stack[i].texture_set == VPTR(texture_set)) scene_stack[i].texture_set = NULL;
	}

	if(current_state.texture_set == VPTR(texture_set)) current_state.texture_set = NULL;

	if(ff8) ff8_unload_texture(VPTRCAST(ff8_texture_set, texture_set));
}

// create a texture from an area of the framebuffer, source rectangle is encoded into tex header
// with our fictional version FB_TEXT_VERSION
// return true to short-circuit texture loader
uint32_t create_framebuffer_texture(struct texture_set *texture_set, struct tex_header *tex_header)
{
	VOBJ(texture_set, texture_set, texture_set);
	VOBJ(tex_header, tex_header, tex_header);
	uint32_t texture;

	if(VREF(tex_header, version) != FB_TEX_VERSION) return false;

	if(trace_all) ffnx_trace("create_framebuffer_texture: XY(%u,%u) %ux%u\n", VREF(tex_header, fb_tex.x), VREF(tex_header, fb_tex.y), VREF(tex_header, fb_tex.w), VREF(tex_header, fb_tex.h));

	texture = newRenderer.createBlitTexture(
		VREF(tex_header, fb_tex.x),
		VREF(tex_header, fb_tex.y),
		VREF(tex_header, fb_tex.w),
		VREF(tex_header, fb_tex.h)
	);

	VRASS(texture_set, texturehandle[0], texture);

	return true;
}

void blit_framebuffer_texture(struct texture_set *texture_set, struct tex_header *tex_header)
{
	VOBJ(texture_set, texture_set, texture_set);
	VOBJ(tex_header, tex_header, tex_header);

	if(VREF(tex_header, version) != FB_TEX_VERSION) return;

	if(gl_defer_blit_framebuffer(texture_set, tex_header)) return;

	if(trace_all) ffnx_trace("load_framebuffer_texture: XY(%u,%u) %ux%u\n", VREF(tex_header, fb_tex.x), VREF(tex_header, fb_tex.y), VREF(tex_header, fb_tex.w), VREF(tex_header, fb_tex.h));

	newRenderer.blitTexture(
		VREF(texture_set, texturehandle[0]),
		VREF(tex_header, fb_tex.x),
		VREF(tex_header, fb_tex.y),
		VREF(tex_header, fb_tex.w),
		VREF(tex_header, fb_tex.h)
	);
}

// load modpath texture for tex file, returns true if successful
uint32_t load_external_texture(void* image_data, uint32_t dataSize, struct texture_set *texture_set, struct tex_header *tex_header, uint32_t originalWidth, uint32_t originalHeight, uint32_t saveload_palette_index)
{
	VOBJ(texture_set, texture_set, texture_set);
	VOBJ(tex_header, tex_header, tex_header);
	uint32_t texture = 0;
	struct gl_texture_set *gl_set = VREF(texture_set, ogl.gl_set);
	struct texture_format* tex_format = VREFP(tex_header, tex_format);

	if((uint32_t)VREF(tex_header, file.pc_name) > 32 && !save_textures)
	{
		if(trace_all || trace_loaders) ffnx_trace("texture file name: %s\n", VREF(tex_header, file.pc_name));

		// Don't use palette index on fallback (for keeping compatibility with Tonberry mods)
		if(ff8 && _strnicmp(VREF(tex_header, file.pc_name), "field/mapdata/", strlen("field/mapdata/") - 1) == 0) saveload_palette_index |= 0x80000000;

		texture = load_texture(image_data, dataSize, VREF(tex_header, file.pc_name), saveload_palette_index, VREFP(texture_set, ogl.width), VREFP(texture_set, ogl.height), gl_set);

		if (!ff8)
		{
			if (enable_lighting)
			{
				std::string path = VREF(tex_header, file.pc_name);
				std::string filename = path.substr(path.find_last_of("/") + 1);

				if(lighting.isDisabledLightingTexture(filename))
				{
					gl_set->disable_lighting = true;
				}
			}

			if(!_strnicmp(VREF(tex_header, file.pc_name), "world", strlen("world") - 1)) gl_set->force_filter = true;

			if(!_strnicmp(VREF(tex_header, file.pc_name), "menu/usfont", strlen("menu/usfont") - 1))
			{
				gl_set->force_filter = true;
				gl_set->force_zsort = true;
			}

			if(!_strnicmp(VREF(tex_header, file.pc_name), "menu/btl_win", strlen("menu/btl_win") - 1)) gl_set->force_zsort = true;

			if(!_strnicmp(VREF(tex_header, file.pc_name), "flevel/hand_1", strlen("flevel/hand_1") - 1)) gl_set->force_filter = true;
		}
	}

	if(ff8 && texture == 0)
	{
		bool external = true;
		texture = texturePacker.composeTextures(
			VREF(tex_header, image_data), reinterpret_cast<uint32_t *>(image_data), originalWidth, originalHeight,
			VREF(tex_header, palette_index) / 2,
			VREFP(texture_set, ogl.width), VREFP(texture_set, ogl.height), gl_set, &external
		);

		if (texture == uint32_t(-1))
		{
			return true; // Remove texture
		}

		if (texture == 0)
		{
			if(VREF(texture_set, ogl.external)) stats.external_textures--;
			VRASS(texture_set, ogl.external, false);
		}
		else if (external == false)
		{
			gl_replace_texture(texture_set, VREF(tex_header, palette_index), texture);

			return true;
		}
	}

	if(texture)
	{
		gl_replace_texture(texture_set, VREF(tex_header, palette_index), texture);

		if(!VREF(texture_set, ogl.external)) stats.external_textures++;
		VRASS(texture_set, ogl.external, true);

		return true;
	}

	return false;
}

// convert a single 8-bit paletted pixel to 32-bit BGRA format
_inline uint32_t pal2bgra(uint32_t pixel, uint32_t *palette, uint32_t palette_offset, uint32_t color_key, uint32_t reference_alpha)
{
	if(color_key && pixel == 0) return 0;

	else
	{
		uint32_t color = palette[palette_offset + pixel];
		// FF7 uses a form of alpha keying to emulate PSX blending
		if(BGRA_A(color) == 0xFE) color = (color & 0xFFFFFF) | reference_alpha;
		return color;
	}
}

// convert an entire image from its native format to 32-bit BGRA
void convert_image_data(const unsigned char *image_data, uint32_t *converted_image_data, uint32_t w, uint32_t h, struct texture_format *tex_format, uint32_t invert_alpha, uint32_t color_key, uint32_t palette_offset, uint32_t reference_alpha)
{
	uint32_t i, j, o = 0, c = 0;

	// invalid texture in FF8, do not attempt to convert
	if(ff8 && tex_format->bytesperpixel == 0) return;

	// paletted source data (4-bit palettes are expanded to 8-bit by the game)
	if(tex_format->bytesperpixel == 1)
	{
		if(!tex_format->use_palette)
		{
			ffnx_glitch("unsupported texture format\n");
			return;
		}

		for(i = 0; i < w; i++)
		{
			for(j = 0; j < h; j++)
			{
				if(image_data[o] > tex_format->palette_size)
				{
					ffnx_glitch("texture conversion error\n");
					return;
				}

				converted_image_data[c++] = pal2bgra(image_data[o++], tex_format->palette_data, palette_offset, color_key, reference_alpha);
			}
		}
	}
	// RGB(A) source data
	else
	{
		if(tex_format->use_palette)
		{
			ffnx_glitch("unsupported texture format\n");
			return;
		}

		for(i = 0; i < w; i++)
		{
			for(j = 0; j < h; j++)
			{
				uint32_t pixel = 0;
				uint32_t color = 0;

				switch(tex_format->bytesperpixel)
				{
					// 16-bit RGB(A)
					case 2:
						pixel = *((WORD *)(&image_data[o]));
						break;
					// 24-bit RGB
					case 3:
						pixel = image_data[o] | image_data[o + 1] << 8 | image_data[o + 2] << 16;
						break;
					// 32-bit RGBA or RGBX
					case 4:
						pixel = *((uint32_t *)(&image_data[o]));
						break;

					default:
						ffnx_glitch("unsupported texture format\n");
						return;
				}

				o += tex_format->bytesperpixel;

				// PSX style mask bit
				if((color_key == 1 && (pixel & ~tex_format->alpha_mask) == 0) || (color_key == 3 && pixel == 0))
				{
					converted_image_data[c++] = 0;
					continue;
				}

				// convert source data to 8 bits per channel
				color = tex_format->blue_max > 0 ? ((((pixel & tex_format->blue_mask) >> tex_format->blue_shift) * 255) / tex_format->blue_max) : 0;
				color |= (tex_format->green_max > 0 ? ((((pixel & tex_format->green_mask) >> tex_format->green_shift) * 255) / tex_format->green_max) : 0) << 8;
				color |= (tex_format->red_max > 0 ? ((((pixel & tex_format->red_mask) >> tex_format->red_shift) * 255) / tex_format->red_max) : 0) << 16;

				// special case to deal with poorly converted PSX images in FF7
				if(invert_alpha && pixel != 0x8000) color |= (tex_format->alpha_max > 0 ? (255 - ((((pixel & tex_format->alpha_mask) >> tex_format->alpha_shift) * 255) / tex_format->alpha_max)) : 255) << 24;
				else color |= (tex_format->alpha_max > 0 ? ((((pixel & tex_format->alpha_mask) >> tex_format->alpha_shift) * 255) / tex_format->alpha_max) : 255) << 24;

				converted_image_data[c++] = color;
			}
		}
	}
}

// called by the game to load a texture
// can be called under a wide variety of circumstances, we must figure out what the game wants
struct texture_set *common_load_texture(struct texture_set *_texture_set, struct tex_header *_tex_header, struct texture_format *texture_format)
{
	VOBJ(game_obj, game_object, common_externals.get_game_object());
	VOBJ(texture_set, texture_set, _texture_set);
	VOBJ(tex_header, tex_header, _tex_header);
	struct palette *palette = 0;
	uint32_t color_key = false;
	struct texture_format *tex_format = VREFP(tex_header, tex_format);

	if(trace_all && _texture_set != NULL) ffnx_trace("dll_gfx: load_texture 0x%x\n", _texture_set);

	// no existing texture set, create one
	if (!VPTR(texture_set))
	{
		_texture_set = common_externals.create_texture_set();
		VASS(texture_set, texture_set, _texture_set);
	}

	// allocate space for our private data
	if(!VREF(texture_set, ogl.gl_set))
	{
		VRASS(texture_set, ogl.gl_set, new gl_texture_set());
		for (short slot = RendererTextureSlot::TEX_NML; slot < RendererTextureSlot::COUNT; slot++)
			VRASS(texture_set, ogl.gl_set->additional_textures[slot], 0);
	}

	// texture handle array may not have been initialized
	if(!VREF(texture_set, texturehandle))
	{
		// allocate some more textures just in case, there could be more palettes we don't know about yet
		// FF8 likes to change its mind about just how many palettes a texture has
		VRASS(texture_set, ogl.gl_set->textures, VREF(tex_header, palettes) > 0 ? VREF(tex_header, palettes) * 2 : 1);
		VRASS(texture_set, texturehandle, (uint32_t*)external_calloc(VREF(texture_set, ogl.gl_set->textures), sizeof(uint32_t)));

		if(ff8 && VREF(tex_header, version) != FB_TEX_VERSION)
		{
			external_free(VREF(tex_header, old_palette_data));
			VRASS(tex_header, old_palette_data, 0);
		}

		stats.texture_count++;
	}

	// number of palettes has changed, reload the texture completely
	if(VREF(texture_set, ogl.gl_set->textures) != VREF(tex_header, palettes) * 2 && !(VREF(tex_header, palettes) == 0 && VREF(texture_set, ogl.gl_set->textures) == 1))
	{
		common_unload_texture(_texture_set);

		return common_load_texture(_texture_set, _tex_header, texture_format);
	}

	// make sure the information in the texture set is consistent
	VRASS(texture_set, tex_header, _tex_header);
	VRASS(texture_set, texture_format, texture_format);

	// check if this is suppposed to be a framebuffer texture, we may not have to do anything
	if(create_framebuffer_texture(_texture_set, _tex_header))
	{
		blit_framebuffer_texture(_texture_set, _tex_header);
		return _texture_set;
	}

	// initialize palette index to a sane value if it hasn't been set
	if(VREF(tex_header, palettes) > 0)
	{
		if(VREF(texture_set, palette_index) == -1)
		{
			VRASS(tex_header, palette_index, 0);
		}
		else
		{
			VRASS(tex_header, palette_index, ff8 ? VREF(texture_set, palette_index) & 0x1FFF : VREF(texture_set, palette_index));
		}
	}
	else VRASS(tex_header, palette_index, 0);

	if(VREF(tex_header, palette_index) >= VREF(texture_set, ogl.gl_set->textures))
	{
		ffnx_unexpected("tried to use non-existent palette (%i, %i)\n", VREF(tex_header, palette_index), VREF(texture_set, ogl.gl_set->textures));
		VRASS(tex_header, palette_index, 0);
		return _texture_set;
	}

	// create palette structure if it doesn't exist already
	if(VREF(tex_header, palettes) > 1 && VREF(texture_set, palette) == 0) palette = common_externals.create_palette_for_tex(texture_format->bitsperpixel, _tex_header, _texture_set);

	if(tex_format->palettes == 0) tex_format->palettes = VREF(tex_header, palette_entries);

	// convert texture data from source format and load it
	if(texture_format != 0 && VREF(tex_header, image_data) != 0)
	{
		uint32_t saveload_palette_index = VREF(tex_header, palette_index);

		if (ff8)
		{
			// optimization to not upload textures with undefined VRAM palette
			TexturePacker::TiledTex tiledTex = texturePacker.getTiledTex(VREF(tex_header, image_data));

			if (tiledTex.isValid())
			{
				TexturePacker::TextureInfos pal = tiledTex.palette(VREF(tex_header, palette_index) / 2);

				if (tiledTex.bpp() != Tim::Bpp16 && !pal.isValid())
				{
					if(trace_all || trace_vram) ffnx_trace("dll_gfx: load_texture ignored texture_set=0x%X pointer=0x%X pos=(%d, %d) bpp=%d index=%d\n", _texture_set, VREF(tex_header, image_data), tiledTex.x(), tiledTex.y(), tiledTex.bpp(), VREF(tex_header, palette_index) / 2);

					return _texture_set;
				}

				if (pal.isValid()) {
					saveload_palette_index = uint32_t(0x40000000) | ((uint32_t(pal.y()) & 0x7FFF) << 15) | (uint32_t(pal.x()) & 0x7FFF);
				} else {
					saveload_palette_index = uint32_t(-1);
				}
			}
		}

		// detect changes in palette data for FF8, we can't trust it to notify us
		if(ff8 && VREF(tex_header, palettes) > 0 && VREF(tex_header, version) != FB_TEX_VERSION && tex_format->bytesperpixel == 1)
		{
			if(!VREF(tex_header, old_palette_data))
			{
				VRASS(tex_header, old_palette_data, (unsigned char*)external_malloc(4 * tex_format->palette_size));
				memcpy(VREF(tex_header, old_palette_data), tex_format->palette_data, 4 * tex_format->palette_size);
			}
			else if(memcmp(VREF(tex_header, old_palette_data), tex_format->palette_data, 4 * tex_format->palette_size) != 0)
			{
				for (uint32_t idx = 0; idx < VREF(texture_set, ogl.gl_set->textures); idx++)
					newRenderer.deleteTexture(VREF(texture_set, texturehandle[idx]));

				memset(VREF(texture_set, texturehandle), 0, VREF(texture_set, ogl.gl_set->textures) * sizeof(uint32_t));

				memcpy(VREF(tex_header, old_palette_data), tex_format->palette_data, 4 * tex_format->palette_size);
			}
		}

		// the texture handle for the current palette is missing, convert & load it
		// if we are dealing with an animated palette, load it anyway even if already loaded
		if(!VREF(texture_set, texturehandle[VREF(tex_header, palette_index)]) || VREF(texture_set, ogl.gl_set->is_animated))
		{
			uint32_t c = 0;
			uint32_t w = VREF(tex_header, version) == FB_TEX_VERSION ? VREF(tex_header, fb_tex.w) : tex_format->width;
			uint32_t h = VREF(tex_header, version) == FB_TEX_VERSION ? VREF(tex_header, fb_tex.h) : tex_format->height;
			uint32_t invert_alpha = false;
			uint32_t *image_data;
			// pre-calculate some useful data for palette conversion
			uint32_t palette_offset = VREF(tex_header, palette_index) * VREF(tex_header, palette_entries);
			uint32_t reference_alpha = (VREF(tex_header, reference_alpha) & 0xFF) << 24;

			// detect 16-bit PSX 5551 format with mask bit
			if(tex_format->bitsperpixel == 16 && tex_format->alpha_mask == 0x8000)
			{
				// correct incomplete texture format in FF7
				if(!ff8)
				{
					tex_format->blue_mask =  0x001F;
					tex_format->green_mask = 0x03E0;
					tex_format->red_mask =   0x7C00;
					tex_format->blue_shift =  0;
					tex_format->green_shift = 5;
					tex_format->red_shift =  10;
					tex_format->blue_max =  31;
					tex_format->green_max = 31;
					tex_format->red_max =   31;
				}

				invert_alpha = true;
			}

			if(!ff8)
			{
				// find out if color keying is enabled for this texture
				color_key = VREF(tex_header, color_key);

				// find out if color keying is enabled for this particular palette
				if(VREF(tex_header, use_palette_colorkey)) color_key = VREF(tex_header, palette_colorkey[VREF(tex_header, palette_index)]);
			}

			// allocate PBO
			uint32_t image_data_size = w * h * 4;

			// Allocate with cache
			if (image_data_size_cache == 0 || image_data_size > image_data_size_cache) {
				if (image_data_cache != nullptr) {
					driver_free(image_data_cache);
				}
				image_data_cache = (uint32_t*)driver_malloc(image_data_size);
				image_data_size_cache = image_data_size;
			}

			image_data = image_data_cache;

			// convert source data
			if (image_data != NULL) convert_image_data(VREF(tex_header, image_data), image_data, w, h, tex_format, invert_alpha, color_key, palette_offset, reference_alpha);

			// save texture to modpath if save_textures is enabled
			if(save_textures && (uint32_t)VREF(tex_header, file.pc_name) > 32)
			{
				save_texture(image_data, image_data_size, w, h, saveload_palette_index, VREF(tex_header, file.pc_name), VREF(texture_set, ogl.gl_set->is_animated));
			}

			// check if this texture can be loaded from the modpath, we may not have to do any conversion
			if (!load_external_texture(image_data, image_data_size, _texture_set, _tex_header, w, h, saveload_palette_index))
			{
				// commit PBO and populate texture set
				gl_upload_texture(_texture_set, VREF(tex_header, palette_index), image_data, RendererTextureType::BGRA);
			}
		}
	}
	else ffnx_unexpected("no texture format specified or no source data\n");

	return _texture_set;
}

// called by the game to indicate when a texture has switched to using another palette
// Either palette_entry_mul_index1 or palette_entry_mul_index2 can be filled. Not both! If palette_entry_mul_index1 has a value, then palette_entry_mul_index2 is 0, for eg.
// If it is one or the other filled, means coming from two different points in the engine ( for FF8 at least )
uint32_t common_palette_changed(uint32_t palette_entry_mul_index1, uint32_t palette_entries, uint32_t palette_entry_mul_index2, struct palette *palette, struct texture_set *texture_set)
{
	VOBJ(texture_set, texture_set, texture_set);

	if(trace_all) ffnx_trace("dll_gfx: palette_changed 0x%x %i\n", texture_set, VREF(texture_set, palette_index));

	if(palette == 0 || texture_set == 0) return false;

	// unset current texture
	newRenderer.useTexture(0);

	// texture loader logic handles missing palettes, just make sure the new palette has been loaded
	texture_set = common_load_texture(texture_set, VREF(texture_set, tex_header), VREF(texture_set, texture_format));

	// re-bind texture set to make sure the new palette is active
	gl_bind_texture_set(texture_set);

	stats.palette_changes++;

	return true;
}

// called by the game to write new color data to a palette
// sometimes called just to indicate that the palette has already been changed
// return value?
uint32_t common_write_palette(uint32_t source_offset, uint32_t size, void *source, uint32_t dest_offset, struct palette *palette, struct texture_set *texture_set)
{
	struct game_mode *mode = getmode_cached();

	uint32_t palette_index;
	uint32_t palettes;
	VOBJ(texture_set, texture_set, texture_set);
	VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));

	if(trace_all) ffnx_trace("dll_gfx: write_palette texture_set=0x%x source_offset=%i dest_offset=%i size=%i source=0x%x palette_entry=0x%x image_data=0x%X\n", texture_set, source_offset, dest_offset, size, source, palette->palette_entry, VREF(tex_header, image_data));

	if(palette == 0) return false;

	// if the tex header and texture set are not consistent we shouldn't be touching
	// anything before the texture is reloaded
	if(VREF(texture_set, ogl.gl_set->textures) != VREF(tex_header, palettes) * 2 && !(VREF(tex_header, palettes) == 0 && VREF(texture_set, ogl.gl_set->textures) == 1)) return true;

	palette_index = dest_offset / VREF(tex_header, palette_entries);
	palettes = size / VREF(tex_header, palette_entries);

	if(!ff8)
	{
		// FF7 writes to one palette at a time
		if(palettes > 1) ffnx_unexpected("multipalette write\n");

		if(palette_index >= VREF(texture_set, ogl.gl_set->textures))
		{
			ffnx_unexpected("palette write outside valid palette area (%i, %i)\n", palette_index, VREF(texture_set, ogl.gl_set->textures));
			return false;
		}

		// make sure the palette actually changed to avoid redundant texture reloads
		if(memcmp(((uint32_t *)VREF(tex_header, tex_format.palette_data)) + dest_offset, ((uint32_t *)source + source_offset), size * 4))
		{
			memcpy(((uint32_t *)VREF(tex_header, tex_format.palette_data)) + dest_offset, ((uint32_t *)source + source_offset), size * 4);

			if(!VREF(texture_set, ogl.external))
			{
				newRenderer.deleteTexture(VREF(texture_set, texturehandle[palette_index]));

				VRASS(texture_set, texturehandle[palette_index], 0);
			}

			stats.texture_reloads++;
		}

		// this texture changes in time, flag this as animated
		VRASS(texture_set, ogl.gl_set->is_animated, enable_animated_textures && mode->driver_mode == MODE_FIELD && (std::find(disable_animated_textures_on_field.begin(), disable_animated_textures_on_field.end(), get_current_field_name()) == disable_animated_textures_on_field.end()));
	}
	else
	{
		// FF8 writes multiple palettes in one swath but it always writes whole palettes
		if(palettes > 1 && size % VREF(tex_header, palette_entries)) ffnx_unexpected("unaligned multipalette write\n");

		const TexturePacker::TextureInfos paletteInfos = texturePacker.getTiledTex(VREF(tex_header, image_data)).palette(palette_index / 2);

		if (paletteInfos.isValid())
		{
			if(!VREF(tex_header, vram_positions))
			{
				VRASS(tex_header, vram_positions, (uint32_t*)external_calloc(128, sizeof(uint32_t)));
			}

			const uint32_t pos = VREF(tex_header, vram_positions)[palette_index];
			const uint16_t x = uint16_t(pos & 0xFFFF), y = uint16_t(pos >> 16);

			// We have a texture for this index, but the coordinates do not match
			// In this case, we can search if indexes haven't just swaped
			if (VREF(tex_header, old_palette_data) && VREF(texture_set, texturehandle[palette_index]) && (x > 0 || y > 0) && (x != paletteInfos.x() || y != paletteInfos.y()))
			{
				for (uint32_t idx = 0; idx < VREF(texture_set, ogl.gl_set->textures); idx++)
				{
					if (idx == palette_index || VREF(texture_set, texturehandle[idx]) == 0)
					{
						continue;
					}

					const uint32_t pos2 = VREF(tex_header, vram_positions)[idx];
					const uint16_t x2 = uint16_t(pos2 & 0xFFFF), y2 = uint16_t(pos2 >> 16);

					// Found one texture using the same coordinates
					if ((x2 > 0 || y2 > 0) && paletteInfos.x() == x2 && paletteInfos.y() == y2)
					{
						// Swap handles
						uint32_t old_handle = VREF(texture_set, texturehandle[palette_index]);
						VRASS(texture_set, texturehandle[palette_index], VREF(texture_set, texturehandle[idx]));
						VRASS(texture_set, texturehandle[idx], old_handle);
						// Swap palette data
						uint32_t *tmp_palette = (uint32_t *)external_malloc(size * 4);
						memcpy(tmp_palette, ((uint32_t *)VREF(tex_header, old_palette_data)) + dest_offset, size * 4);
						memcpy(((uint32_t *)VREF(tex_header, old_palette_data)) + dest_offset, ((uint32_t *)VREF(tex_header, old_palette_data)) + idx * size, size * 4);
						memcpy(((uint32_t *)VREF(tex_header, old_palette_data)) + idx * size, tmp_palette, size * 4);
						external_free(tmp_palette);
						// Move vram position
						VREF(tex_header, vram_positions)[idx] = VREF(tex_header, vram_positions)[palette_index];

						break;
					}
				}
			}

			VREF(tex_header, vram_positions)[palette_index] = uint32_t(paletteInfos.x()) | (uint32_t(paletteInfos.y()) << 16);
		}

		if(!VREF(tex_header, old_palette_data)) return false;

		// since FF8 may have already modified the palette itself we need to compare the new data to our backup
		if(memcmp(((uint32_t *)VREF(tex_header, old_palette_data)) + dest_offset, ((uint32_t *)source + source_offset), size * 4) != 0)
		{
			memcpy(((uint32_t *)VREF(tex_header, old_palette_data)) + dest_offset, ((uint32_t *)source + source_offset), size * 4);
			memcpy(((uint32_t *)VREF(tex_header, tex_format.palette_data)) + dest_offset, ((uint32_t *)source + source_offset), size * 4);

			// limit write to the palettes that we are aware of
			if(palette_index >= VREF(texture_set, ogl.gl_set->textures)) palettes = 0;
			else if(palette_index + palettes > VREF(texture_set, ogl.gl_set->textures)) palettes = VREF(texture_set, ogl.gl_set->textures) - palette_index;

			if(dest_offset + size > VREF(tex_header, tex_format.palette_size))
			{
				ffnx_unexpected("palette write outside advertised palette area (0x%x + 0x%x, 0x%x)\n", dest_offset, size, VREF(tex_header, tex_format.palette_size));
			}

			// if there's anything left at this point, reload the affected textures
			if(palettes)
			{
				for (uint32_t idx = 0; idx < palettes; idx++)
					newRenderer.deleteTexture(VREF(texture_set, texturehandle[palette_index + idx]));

				memset(VREFP(texture_set, texturehandle[palette_index]), 0, palettes * sizeof(uint32_t));
			}

			stats.texture_reloads++;
		}
	}

	stats.palette_writes++;

	return true;
}

// blend mode parameters, identical to Direct3D driver
struct blend_mode blend_modes[5] = {      // PSX blend mode:
	{1, 1, 0x80, 5, 0x10, 6, 0x20, 0, 0}, // average
	{1, 0, 0xFF, 2, 2,    2, 2,    0, 0}, // additive blending
	{1, 0, 0xFF, 4, 8,    2, 2,    0, 0}, // subtractive blending
	{1, 0, 0x40, 5, 0x10, 2, 2,    0, 0}, // 25%? incoming color
	{1, 0, 0xFF, 2, 2,    1, 1,    0, 0}, //
};

// called by the game to retrieve blend mode parameters
// only z-sort and vertex alpha are really relevant to us
struct blend_mode *common_blendmode(uint32_t blend_mode, struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: blendmode %i\n", blend_mode);

	switch(blend_mode)
	{
		case 0:
			return &blend_modes[0];
		case 1:
			return &blend_modes[1];
		case 2:
			return &blend_modes[2];
		case 3:
			return &blend_modes[3];
		case 4:
			if(!ff8) ffnx_unexpected("blend mode 4 requested\n");
			return &blend_modes[4];
	}

	ffnx_unexpected("invalid blendmode: %u\n", blend_mode);

	return 0;
}

// helper function to set simple render states (single parameter)
void internal_set_renderstate(uint32_t state, uint32_t option, struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);

	switch(state)
	{
		// wireframe rendering, not used?
		case V_WIREFRAME:
			if (option) newRenderer.setWireframeMode(true);
			else newRenderer.setWireframeMode(false);
			current_state.wireframe = option;
			break;

		// texture filtering, can be disabled globally via config file
		case V_LINEARFILTER:
			if((option && !VREF(game_object, field_988))) current_state.texture_filter = true;
			else current_state.texture_filter = false;
			break;

		// perspective correction should never be turned off
		case V_PERSPECTIVE:
			// noop
			break;

		// color keying is done when textures are converted, not when rendering
		case V_COLORKEY:
			// noop
			break;

		// no dithering necessary in 32-bit color mode
		case V_DITHER:
			// noop
			break;

		// alpha test is used in many places in FF8 instead of color keying
		case V_ALPHATEST:
			if (option) newRenderer.doAlphaTest(true);
			else newRenderer.doAlphaTest(false);
			current_state.alphatest = option;
			break;

		// cull face, does this ever change?
		case V_CULLFACE:
			if (option) newRenderer.setCullMode(RendererCullMode::FRONT);
			else newRenderer.setCullMode(RendererCullMode::BACK);
			current_state.cullface = option;
			break;

		// turn off culling completely, once again unsure if its ever used
		case V_NOCULL:
			if (option) newRenderer.setCullMode(RendererCullMode::DISABLED);
			else newRenderer.setCullMode(RendererCullMode::BACK);
			current_state.nocull = option;
			break;

		// turn depth testing on/off
		case V_DEPTHTEST:
			if (option) newRenderer.doDepthTest(true);
			else newRenderer.doDepthTest(false);
			current_state.depthtest = option;
			break;

		// depth mask, enable/disable writing to the Z-buffer
		case V_DEPTHMASK:
			if (option) newRenderer.doDepthWrite(true);
			else newRenderer.doDepthWrite(false);
			current_state.depthmask = option;
			break;

		// no idea what this is supposed to do
		case V_TEXADDR:
			// noop
			break;

		// function and reference values for alpha test
		case V_ALPHAFUNC:
		case V_ALPHAREF:
			if(state == V_ALPHAFUNC) current_state.alphafunc = option;
			else current_state.alpharef = option;

			switch(current_state.alphafunc)
			{
			case 0: newRenderer.setAlphaRef(RendererAlphaFunc::NEVER, current_state.alpharef / 255.0f); break;
			case 1: newRenderer.setAlphaRef(RendererAlphaFunc::ALWAYS, current_state.alpharef / 255.0f); break;
			case 2: newRenderer.setAlphaRef(RendererAlphaFunc::LESS, current_state.alpharef / 255.0f); break;
			case 3: newRenderer.setAlphaRef(RendererAlphaFunc::LEQUAL, current_state.alpharef / 255.0f); break;
			case 4: newRenderer.setAlphaRef(RendererAlphaFunc::EQUAL, current_state.alpharef / 255.0f); break;
			case 5: newRenderer.setAlphaRef(RendererAlphaFunc::GEQUAL, current_state.alpharef / 255.0f); break;
			case 6: newRenderer.setAlphaRef(RendererAlphaFunc::GREATER, current_state.alpharef / 255.0f); break;
			case 7: newRenderer.setAlphaRef(RendererAlphaFunc::NOTEQUAL, current_state.alpharef / 255.0f); break;
			default: newRenderer.setAlphaRef(RendererAlphaFunc::LEQUAL, current_state.alpharef / 255.0f); break;
			}
			break;

		case V_SHADEMODE:
			current_state.shademode = option;

		default:
			break;
	}
}

// called by the game to set a simple render state
void common_field_64(uint32_t state, uint32_t option, struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: field_64 %i %i\n", state, option);

	internal_set_renderstate(state, option, game_object);
}

// called by the game to apply a set of render states
void common_setrenderstate(struct p_hundred *hundred_data, struct game_obj *game_object)
{
	if(hundred_data == 0) return;

	VOBJ(game_obj, game_object, game_object);

	uint32_t features = hundred_data->features;
	uint32_t options = hundred_data->options;
	struct struc_81 *struc_81 = VREF(game_object, field_944);

	if(trace_all) ffnx_trace("dll_gfx: setrenderstate 0x%x 0x%x\n", features, options);

// helper macro to check if a bit is set
// to be able to tell which bits we haven't handled, this macro will also clear
// a bit after checking it, be extremely careful not to copy/paste any
// invocation of this macro, the second invocation will not work!
#define CHECK_BIT(X, Y) ((X) & BIT((Y))) && (((X &= ~BIT((Y))) || true))

	if(CHECK_BIT(features, V_WIREFRAME)) internal_set_renderstate(V_WIREFRAME, CHECK_BIT(options, V_WIREFRAME), game_object);
	if(CHECK_BIT(features, V_TEXTURE)) gl_bind_texture_set(hundred_data->texture_set);
	if(CHECK_BIT(features, V_LINEARFILTER)) internal_set_renderstate(V_LINEARFILTER, CHECK_BIT(options, V_LINEARFILTER), game_object);
	if(CHECK_BIT(features, V_PERSPECTIVE)) internal_set_renderstate(V_PERSPECTIVE, CHECK_BIT(options, V_PERSPECTIVE), game_object);
	if(CHECK_BIT(features, V_COLORKEY)) internal_set_renderstate(V_COLORKEY, CHECK_BIT(options, V_COLORKEY), game_object);
	if(CHECK_BIT(features, V_DITHER)) internal_set_renderstate(V_DITHER, CHECK_BIT(options, V_DITHER), game_object);
	if(CHECK_BIT(features, V_ALPHABLEND))
	{
		// Safe default
		struc_81->blend_mode = 4;

		if(CHECK_BIT(options, V_ALPHABLEND))
		{
			if(VREF(game_object, field_93C))
			{
				if(VREF(game_object, current_hundred))
					struc_81->blend_mode = VREF(game_object, current_hundred->blend_mode);
			}
			else
				struc_81->blend_mode = hundred_data->blend_mode;
		}

		gl_set_blend_func(struc_81->blend_mode);
	}
	if(CHECK_BIT(features, V_ALPHATEST)) internal_set_renderstate(V_ALPHATEST, CHECK_BIT(options, V_ALPHATEST), game_object);
	if(CHECK_BIT(features, V_CULLFACE)) internal_set_renderstate(V_CULLFACE, CHECK_BIT(options, V_CULLFACE), game_object);
	if(CHECK_BIT(features, V_NOCULL)) internal_set_renderstate(V_NOCULL, CHECK_BIT(options, V_NOCULL), game_object);
	if(CHECK_BIT(features, V_DEPTHTEST)) internal_set_renderstate(V_DEPTHTEST, CHECK_BIT(options, V_DEPTHTEST), game_object);
	if(CHECK_BIT(features, V_DEPTHMASK)) internal_set_renderstate(V_DEPTHMASK, CHECK_BIT(options, V_DEPTHMASK), game_object);
	if(CHECK_BIT(features, V_SHADEMODE)) internal_set_renderstate(V_SHADEMODE, CHECK_BIT(options, V_SHADEMODE) && !VREF(game_object, field_92C) && hundred_data->shademode > 0, game_object);
	if(CHECK_BIT(features, V_UNKNOWNFFFDFFFD))
	{
		// Safe default
		struc_81->blend_mode = 4;

		if(CHECK_BIT(options, V_ALPHABLEND))
		{
			if(VREF(game_object, field_93C))
			{
				if(VREF(game_object, current_hundred))
					struc_81->blend_mode = VREF(game_object, current_hundred->blend_mode);
			}
			else
				struc_81->blend_mode = hundred_data->blend_mode;
		}

		gl_set_blend_func(struc_81->blend_mode);
	}

	// any bits still set in the features and options variables at this point
	// are features that we do not currently handle
}

// called by the game to apply a predetermined set of render states
// one for each blend mode? not sure what this is used for exactly
void common_field_74(uint32_t unknown, struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);

	if(trace_all) ffnx_trace("dll_gfx: field_74\n");

	if(unknown > 4) return;

	common_setrenderstate(VREF(game_object, hundred_array[unknown]), game_object);
}

// called by the game to render a polygon set
// in FF7 this is where most of the 3D rendering happens
// in FF8 this function doesn't do any rendering at all
void common_field_78(struct polygon_set *polygon_set, struct game_obj *game_object)
{
	if(!ff8) ff7gl_field_78((struct ff7_polygon_set *)polygon_set, (struct ff7_game_obj *)game_object);
	else ff8gl_field_78((struct ff8_polygon_set *)polygon_set, (struct ff8_game_obj *)game_object);
}

// called by the game to render an instance that has been deferred by the above
// function, this is a feature of the original game, not to be confused with
// our own deferred rendering!
void common_draw_deferred(struct struc_77 *struc_77, struct game_obj *game_object)
{
	VOBJ(polygon_set, polygon_set, struc_77->polygon_set);
	struct p_hundred *hundred_data = struc_77->hundred_data;
	struct indexed_primitive *ip;

	if(trace_all) ffnx_trace("dll_gfx: draw_deferred\n");

	if(!VREF(polygon_set, indexed_primitives)) return;

	ip = VREF(polygon_set, indexed_primitives[struc_77->current_group]);

	if(!ip) return;

	common_setrenderstate(hundred_data, game_object);

	if(struc_77->use_matrix) gl_set_worldview_matrix(&struc_77->matrix);
	if(struc_77->use_matrix_pointer) gl_set_worldview_matrix(struc_77->matrix_pointer);

	if (!ff8 && enable_lighting) gl_draw_with_lighting(ip, VREF(polygon_set, polygon_data), nullptr, VREF(polygon_set, field_4));
	else gl_draw_without_lighting(ip, VREF(polygon_set, polygon_data), nullptr, VREF(polygon_set, field_4));
}

// called by the game to render a graphics object, basically a wrapper for
// field_78
void common_field_80(struct graphics_object *graphics_object, struct game_obj *game_object)
{
	VOBJ(graphics_object, graphics_object, graphics_object);

	if(trace_all) ffnx_trace("dll_gfx: field_80\n");

	if(!VPTR(graphics_object)) return;

	common_field_78(VREF(graphics_object, polygon_set), game_object);
}

// called by the game to draw some predefined polygon sets, no idea what this
// is really used for
void common_field_84(uint32_t unknown, struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);
	VOBJ(polygon_set, polygon_set_2EC, VREF(game_object, polygon_set_2EC));
	VOBJ(polygon_set, polygon_set_2F0, VREF(game_object, polygon_set_2F0));

	if(trace_all) ffnx_trace("dll_gfx: field_84\n");

	if(!VREF(game_object, in_scene)) return;

	VRASS(game_object, field_928, unknown);

	if(!unknown)
	{
		VRASS(polygon_set_2EC, field_0, true);
		VRASS(polygon_set_2F0, field_0, false);
		common_field_78(VREF(game_object, polygon_set_2EC), game_object);
	}

	else
	{
		VRASS(polygon_set_2EC, field_0, false);
		VRASS(polygon_set_2F0, field_0, true);
		common_field_78(VREF(game_object, polygon_set_2F0), game_object);
	}
}

// called by the game to setup a new scene for rendering
// scenes are not stacked in FF7
// FF8 relies on the ability to stack scenes, saving and later reverting to a previous render state
uint32_t common_begin_scene(uint32_t unknown, struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);

	if(trace_all) ffnx_trace("dll_gfx: begin_scene\n");

	if(scene_stack_pointer == sizeof(scene_stack) / sizeof(scene_stack[0])) ffnx_glitch("scene stack overflow\n");
	else gl_save_state(&scene_stack[scene_stack_pointer++]);

	VRASS(game_object, in_scene, VREF(game_object, in_scene) + 1);

	common_field_84(unknown, game_object);

	return true;
}

// called by the game to end a scene previously setup by the above function
// render state will be restored to what it was before the scene was created
void common_end_scene(struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);

	if(trace_all) ffnx_trace("dll_gfx: end_scene\n");

	if(!scene_stack_pointer) ffnx_glitch("scene stack underflow\n");
	else gl_load_state(&scene_stack[--scene_stack_pointer]);

	if(VREF(game_object, in_scene)) VRASS(game_object, in_scene, VREF(game_object, in_scene) - 1);
}

// noop
void common_field_90(uint32_t unknown)
{
	ffnx_glitch_once("dll_gfx: field_90 (not implemented)\n");
}

// helper function used to draw a set of triangles without palette data
void generic_draw(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object, uint32_t vertextype)
{
	VOBJ(polygon_set, polygon_set, polygon_set);
	VOBJ(indexed_vertices, iv, iv);

	gl_draw_indexed_primitive(RendererPrimitiveType::PT_TRIANGLES, vertextype, VREF(iv, vertices), 0,  VREF(iv, vertexcount), VREF(iv, indices), VREF(iv, indexcount), UNSAFE_VREF(graphics_object, iv, graphics_object), 0, 0, VREF(polygon_set, field_4), true);
}

// helper function used to draw a set of triangles with palette data
void generic_draw_paletted(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object, uint32_t vertextype)
{
	VOBJ(polygon_set, polygon_set, polygon_set);
	VOBJ(indexed_vertices, iv, iv);
	uint32_t count = VREF(iv, count);
	unsigned char *palettes = VREF(iv, palettes);
	struct p_hundred *hundred_data = VREF(polygon_set, hundred_data);
	struct nvertex *vertices;
	struct nvertex *_vertices = VREF(iv, vertices);
	WORD *indices = VREF(iv, indices);

	if(!VREF(polygon_set, field_2C)) return;

	while(count > 0)
	{
		VOBJ(graphics_object, graphics_object, UNSAFE_VREF(graphics_object, iv, graphics_object));
		VOBJ(texture_set, texture_set, hundred_data->texture_set);
		uint32_t palette_index = *palettes++;
		uint32_t var30 = 1;
		uint32_t vertexcount = VREF(graphics_object, vertices_per_shape);
		uint32_t indexcount = VREF(graphics_object, indices_per_shape);

		vertices = _vertices;

		VRASS(texture_set, palette_index, palette_index);

		common_palette_changed(0, 0, 0, VREF(texture_set, palette), hundred_data->texture_set);

		while(var30 < count)
		{
			if(*palettes != palette_index) break;

			palettes++;

			vertexcount += VREF(graphics_object, vertices_per_shape);
			indexcount += VREF(graphics_object, indices_per_shape);

			var30++;
		}

		_vertices = &_vertices[VREF(graphics_object, vertices_per_shape) * var30];

		count -= var30;

		gl_draw_indexed_primitive(RendererPrimitiveType::PT_TRIANGLES, vertextype, vertices, 0, vertexcount, VREF(iv, indices), indexcount, UNSAFE_VREF(graphics_object, iv, graphics_object), 0, 0, VREF(polygon_set, field_4), true);
	}
}

// called by the game to set the render state for a set of 2D triangles
void common_setrenderstate_2D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	VOBJ(polygon_set, polygon_set, polygon_set);

	if(trace_all) ffnx_trace("dll_gfx: setrenderstate_2D\n");

	if(!VREF(polygon_set, field_2C)) return;

	common_setrenderstate(VREF(polygon_set, hundred_data), game_object);
}

// called by the game to draw a set of 2D triangles without palette data
void common_draw_2D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: draw_2D\n");

	generic_draw(polygon_set, iv, game_object, TLVERTEX);
}

// called by the game to draw a set of 2D triangles with palette data
void common_draw_paletted2D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: draw_paletted2D\n");

	generic_draw_paletted(polygon_set, iv, game_object, TLVERTEX);
}

// called by the game to set the render state for a set of 3D triangles
void common_setrenderstate_3D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	VOBJ(polygon_set, polygon_set, polygon_set);
	VOBJ(indexed_vertices, iv, iv);
	VOBJ(graphics_object, graphics_object, UNSAFE_VREF(graphics_object, iv, graphics_object));

	if(trace_all) ffnx_trace("dll_gfx: setrenderstate_3D\n");

	if(!VREF(polygon_set, field_2C)) return;

	common_setrenderstate(VREF(polygon_set, hundred_data), game_object);

	if(VREF(graphics_object, use_matrix_pointer)) gl_set_worldview_matrix(VREF(graphics_object, matrix_pointer));
	else gl_set_worldview_matrix(VREFP(graphics_object, matrix));
}

// called by the game to draw a set of 3D triangles without palette data
void common_draw_3D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: draw_3D\n");

	generic_draw(polygon_set, iv, game_object, LVERTEX);
}

// called by the game to draw a set of 3D triangles with palette data
void common_draw_paletted3D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	if(trace_all) ffnx_trace("dll_gfx: draw_paletted3D\n");

	generic_draw_paletted(polygon_set, iv, game_object, LVERTEX);
}

// called by the game to draw a set of lines
void common_draw_lines(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	VOBJ(polygon_set, polygon_set, polygon_set);
	VOBJ(indexed_vertices, iv, iv);

	if(trace_all) ffnx_trace("dll_gfx: draw_lines\n");

	gl_draw_indexed_primitive(RendererPrimitiveType::PT_LINES, TLVERTEX, VREF(iv, vertices), 0, VREF(iv, vertexcount), VREF(iv, indices), VREF(iv, indexcount), UNSAFE_VREF(graphics_object, iv, graphics_object), 0, 0, VREF(polygon_set, field_4), true);
}

// noop
void common_field_EC(struct game_obj *game_object)
{
	ffnx_glitch_once("dll_gfx: field_EC (not implemented)\n");
}

// create a suitable tex header to be processed by the framebuffer texture loader
struct tex_header *make_framebuffer_tex(uint32_t tex_w, uint32_t tex_h, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color_key)
{
	struct tex_header* _header = common_externals.create_tex_header();

	VOBJ(tex_header, tex_header, _header);

	VRASS(tex_header, bpp, 32);
	VRASS(tex_header, color_key, color_key);
	memcpy(VREFP(tex_header, tex_format), texture_format, sizeof(struct texture_format));

	VRASS(tex_header, tex_format.alpha_max, 0);

	VRASS(tex_header, tex_format.width, tex_w);
	VRASS(tex_header, tex_format.height, tex_h);

	VRASS(tex_header, version, FB_TEX_VERSION);

	VRASS(tex_header, fb_tex.x, x);
	VRASS(tex_header, fb_tex.y, y);
	VRASS(tex_header, fb_tex.w, w);
	VRASS(tex_header, fb_tex.h, h);

	return _header;
}

time_t qpc_get_time(time_t *dest)
{
	QueryPerformanceCounter((LARGE_INTEGER *)dest);

	stats.timer = *dest;

	return *dest;
}

time_t qpc_diff_time(time_t* t1, time_t* t2, time_t* out)
{
	time_t ret = *t1 - *t2;

	if (out != nullptr) *out = ret;

	return ret;
}

// version check reads from a given offset in memory
uint32_t version_check(uint32_t offset)
{
	return (*(uint32_t *)(offset));
}

// figure out if we are running in FF7 or FF8 and detect which version
uint32_t get_version()
{
	uint32_t version_check1 = version_check(0x401004);
	uint32_t version_check2 = version_check(0x401404);

	ffnx_trace("v1: 0x%X, v2: 0x%X\n", version_check1, version_check2);

	if(version_check1 == 0x99CE0805)
	{
		ffnx_info("Auto-detected version: FF7 1.02 US English\n");
		return VERSION_FF7_102_US;
	}
	else if(version_check1 == 0x99EBF805)
	{
		ffnx_info("Auto-detected version: FF7 1.02 French\n");
		return VERSION_FF7_102_FR;
	}
	else if(version_check1 == 0x99DBC805)
	{
		ffnx_info("Auto-detected version: FF7 1.02 German\n");
		return VERSION_FF7_102_DE;
	}
	else if(version_check1 == 0x99F65805)
	{
		ffnx_info("Auto-detected version: FF7 1.02 Spanish\n");
		return VERSION_FF7_102_SP;
	}
	else if(version_check1 == 0x3885048D && version_check2 == 0x159618)
	{
		ffnx_info("Auto-detected version: FF8 1.2 US English\n");
		return VERSION_FF8_12_US;
	}
	else if(version_check1 == 0x3885048D && version_check2 == 0x1597C8)
	{
		ffnx_info("Auto-detected version: FF8 1.2 US English (Nvidia)\n");
		return VERSION_FF8_12_US_NV;
	}
	else if(version_check1 == 0x1085048D && version_check2 == 0x159B48)
	{
		ffnx_info("Auto-detected version: FF8 1.2 French\n");
		return VERSION_FF8_12_FR;
	}
	else if(version_check1 == 0x1085048D && version_check2 == 0x159CF8)
	{
		ffnx_info("Auto-detected version: FF8 1.2 French (Nvidia)\n");
		return VERSION_FF8_12_FR_NV;
	}
	else if(version_check1 == 0xA885048D && version_check2 == 0x159C48)
	{
		ffnx_info("Auto-detected version: FF8 1.2 German\n");
		return VERSION_FF8_12_DE;
	}
	else if(version_check1 == 0xA885048D && version_check2 == 0x159DF8)
	{
		ffnx_info("Auto-detected version: FF8 1.2 German (Nvidia)\n");
		return VERSION_FF8_12_DE_NV;
	}
	else if(version_check1 == 0x8085048D && version_check2 == 0x159C38)
	{
		ffnx_info("Auto-detected version: FF8 1.2 Spanish\n");
		return VERSION_FF8_12_SP;
	}
	else if(version_check1 == 0x8085048D && version_check2 == 0x159DE8)
	{
		ffnx_info("Auto-detected version: FF8 1.2 Spanish (Nvidia)\n");
		return VERSION_FF8_12_SP_NV;
	}
	else if(version_check1 == 0xB885048D && version_check2 == 0x159BC8)
	{
		ffnx_info("Auto-detected version: FF8 1.2 Italian\n");
		return VERSION_FF8_12_IT;
	}
	else if(version_check1 == 0xB885048D && version_check2 == 0x159D78)
	{
		ffnx_info("Auto-detected version: FF8 1.2 Italian (Nvidia)\n");
		return VERSION_FF8_12_IT_NV;
	}
	else if(version_check1 == 0x2885048D && version_check2 == 0x159598)
	{
		ffnx_info("Auto-detected version: FF8 1.2 US English (Eidos Patch)\n");
		return VERSION_FF8_12_US_EIDOS;
	}
	else if(version_check1 == 0x2885048D && version_check2 == 0x159748)
	{
		ffnx_info("Auto-detected version: FF8 1.2 US English (Eidos Patch) (Nvidia)\n");
		return VERSION_FF8_12_US_EIDOS_NV;
	}
	else if(version_check1 == 0x1B6E9CC && version_check2 == 0x7C8DFFC9)
	{
		uint32_t version_check3 = version_check(0x401010);

		if (version_check3 == 0x24AC)
		{
			ffnx_info("Auto-detected version: FF8 1.2 Japanese (Nvidia)\n");
			return VERSION_FF8_12_JP_NV;
		}

		ffnx_info("Auto-detected version: FF8 1.2 Japanese\n");
		return VERSION_FF8_12_JP;
	}

	return 0;
}

void get_data_lang_path(PCHAR buffer)
{
	strcpy(buffer, ff8 ? ff8_externals.app_path : basedir);
	PathAppendA(buffer, R"(data\lang-)");
	switch (version)
	{
	case VERSION_FF7_102_US:
	case VERSION_FF8_12_US_NV:
	case VERSION_FF8_12_US_EIDOS_NV:
		if (ff7_japanese_edition)
			strcat(buffer, "ja");
		else
			strcat(buffer, "en");
		break;
	case VERSION_FF7_102_FR:
	case VERSION_FF8_12_FR_NV:
		strcat(buffer, "fr");
		break;
	case VERSION_FF7_102_DE:
	case VERSION_FF8_12_DE_NV:
		strcat(buffer, "de");
		break;
	case VERSION_FF7_102_SP:
	case VERSION_FF8_12_SP_NV:
		strcat(buffer, "es");
		break;
	case VERSION_FF8_12_IT_NV:
		strcat(buffer, "it");
		break;
	case VERSION_FF8_12_JP:
		strcat(buffer, "jp");
		break;
	}
}

void get_userdata_path(PCHAR buffer, size_t bufSize, bool isSavegameFile)
{
	PWSTR outPath = NULL;

	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &outPath);

	if (SUCCEEDED(hr))
	{
		wcstombs(buffer, outPath, bufSize);

		CoTaskMemFree(outPath);

		if (ff8)
			PathAppendA(buffer, R"(Square Enix\FINAL FANTASY VIII Steam)");
		else
			PathAppendA(buffer, R"(Square Enix\FINAL FANTASY VII Steam)");

		if (isSavegameFile)
		{
			if (!steam_game_userdata.empty())
			{
				// Directly use the given userdata
				PathAppendA(buffer, steam_game_userdata.c_str());
			}
			else
			{
				// Search for the first "user_" match in the game path
				CHAR searchPath[MAX_PATH];
				WIN32_FIND_DATA pathFound;
				HANDLE hFind;

				strcpy(searchPath, buffer);
				strcat(searchPath, R"(\user_*)");
				if (hFind = FindFirstFileA(searchPath, &pathFound))
				{
					PathAppendA(buffer, pathFound.cFileName);
					FindClose(hFind);
				}
			}
		}
	}
}

// cd check
uint32_t ff7_get_inserted_cd(void) {
	int ret = 1;

	if(steam_edition || enable_steam_achievements){
		if (trace_all || trace_achievement)
			ffnx_trace("inserted CD: %d, requiredCD: %d\n", *ff7_externals.insertedCD, *ff7_externals.requiredCD);

		if(*ff7_externals.requiredCD == *ff7_externals.insertedCD + 1)
			g_FF7SteamAchievements->unlockGameProgressAchievement();
	}

	if (*ff7_externals.requiredCD > 0 && *ff7_externals.requiredCD <= 3) ret = *ff7_externals.requiredCD;

	*ff7_externals.insertedCD = ret;

	return ret;
}

MCIERROR __stdcall dotemuMciSendCommandA(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	DWORD mciStatusRet;

	switch (uMsg)
	{
	case MCI_OPEN:
		((LPDWORD)dwParam2)[2] = (DWORD)"waveaudio";
		dwParam1 = 8704;
		((LPDWORD)dwParam2)[3] = (DWORD)R"(Data\Music\eyes_on_me.wav)";
		return mciSendCommandA(mciId, uMsg, dwParam1, dwParam2);
	case MCI_SET:
		((LPDWORD)dwParam2)[1] = 0;
		break;
	case MCI_STATUS:
		mciStatusRet = 0;
		((LPDWORD)dwParam2)[1] = (DWORD)&mciStatusRet;
		return 0;
	case MCI_PLAY:
		((LPDWORD)dwParam2)[2] = 339000;
		break;
	}

	return mciSendCommandA(mciId, uMsg, dwParam1, dwParam2);
}

#if defined(__cplusplus)
extern "C" {
#endif

// main entry point, called by the game to create a graphics driver object
__declspec(dllexport) void *new_dll_graphics_driver(void *game_object)
{
	void *ret;

	// game-specific initialization
	if(!ff8)
		ret = ff7_load_driver(game_object);
	else
		ret = ff8_load_driver(game_object);

	return ret;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Enable the following block if you wish to use VS2019 ReAttach extension
		/*
		while (!IsDebuggerPresent())
		{
			Sleep(100);
		}
		__debugbreak();
		*/

		// Push the limit of how many files we can open at the same time to the maximum available on Windows
		// See https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setmaxstdio?view=msvc-160#remarks
		_setmaxstdio(8192);

		SetProcessDPIAware();

		GetCurrentDirectoryA(BASEDIR_LENGTH, basedir);

		// install crash handler
		open_applog("FFNx.log");
		SetUnhandledExceptionFilter(ExceptionHandler);

		// prevent screensavers
		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);

		ffnx_info("FFNx driver version " VERSION "\n");
		version = get_version();

		if (version >= VERSION_FF8_12_US)
		{
			ff8 = true;
		}
		else if (version == VERSION_FF7_102_US)
		{
#include "externals_102_us.h"
		}
		else if (version == VERSION_FF7_102_FR)
		{
#include "externals_102_fr.h"
		}
		else if (version == VERSION_FF7_102_DE)
		{
#include "externals_102_de.h"
		}
		else if (version == VERSION_FF7_102_SP)
		{
#include "externals_102_sp.h"
		}

		if (!version)
		{
			ffnx_error("no compatible version found\n");
			MessageBoxA(NULL, "Your ff7.exe or ff8.exe is incompatible with this driver and will exit after this message.\n"
				"Possible reasons for this error:\n"
				" - You have the faulty \"1.4 XP Patch\" for FF7.\n"
				" - You have FF7 retail 1.00 version (you need the 1.02 patch).\n"
				" - You have an unsupported translation of FF7. (US English, French, German and Spanish versions are currently supported)\n"
				" - You have FF8 retail 1.0 version (you need the 1.2 patch).\n"
				" - You have an unsupported translation of FF8. (US English, French, German, Spanish, Italian and Japanese versions are currently supported)\n"
				" - You have a conflicting patch applied.\n\n"
				, "Error", MB_ICONERROR | MB_OK);
			exit(1);
		}

		read_cfg();

		// Did user choose to enable Widescreen?
		widescreen_enabled = (aspect_ratio == AR_WIDESCREEN_16X9 || aspect_ratio == AR_WIDESCREEN_16X10);

		// Get current process name
		CHAR parentName[1024];
		GetModuleFileNameA(NULL, parentName, sizeof(parentName));
		_strlwr(parentName);

		// Get our filesystem name
		CHAR dllName[1024];
		GetModuleFileNameA((HMODULE)hinstDLL, dllName, sizeof(dllName));
		_strlwr(dllName);

		if (!ff8)
		{
			common_externals.winmain = get_relative_call(common_externals.start, 0x14D);
			replace_function(ff7_externals.get_inserted_cd_sub, ff7_get_inserted_cd);
			replace_function(common_externals.create_window, common_create_window);

			if (strstr(dllName, "af3dn.p") != NULL)
			{
				if (strstr(parentName, "ff7_ja.exe") != NULL) ff7_japanese_edition = true;

				// Steam edition is usually installed in this path
				if (strstr(basedir, "steamapps") != NULL) {
					ffnx_trace("Detected Steam edition.\n");
					steam_edition = true;

					// Read ff7sound.cfg
					char ff7soundPath[260]{ 0 };
					get_userdata_path(ff7soundPath, sizeof(ff7soundPath), false);
					PathAppendA(ff7soundPath, "ff7sound.cfg");
					FILE* ff7sound = fopen(ff7soundPath, "rb");

					if (ff7sound)
					{
						if (external_sfx_volume < 0) fread(&external_sfx_volume, sizeof(DWORD), 1, ff7sound);
						if (external_music_volume < 0) fread(&external_music_volume, sizeof(DWORD), 1, ff7sound);
						fclose(ff7sound);
					}
				}
				// otherwise it's the eStore edition which has same exe names but installed somewhere else
				else
				{
					ffnx_trace("Detected eStore edition.\n");
					estore_edition = true;
				}

				if (external_voice_volume < 0) external_voice_volume = 100;
				if (external_ambient_volume < 0) external_ambient_volume = 100;
				if (ffmpeg_video_volume < 0) ffmpeg_video_volume = 100;

				use_external_music = true;
				if (external_music_path.empty()) external_music_path = "data/music_ogg";

			}
			else
			{
				HKEY ff7_regkey;
				DWORD regsize = sizeof(DWORD);

				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, R"(Software\Square Soft, Inc.\Final Fantasy VII\1.00\MIDI)", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &ff7_regkey) == ERROR_SUCCESS)
					if (external_music_volume < 0) RegQueryValueEx(ff7_regkey, "MusicVolume", NULL, NULL, (LPBYTE)&external_music_volume, &regsize);

				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, R"(Software\Square Soft, Inc.\Final Fantasy VII\1.00\Sound)", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &ff7_regkey) == ERROR_SUCCESS)
					if (external_sfx_volume < 0) RegQueryValueEx(ff7_regkey, "SFXVolume", NULL, NULL, (LPBYTE)&external_sfx_volume, &regsize);

				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, R"(Software\Square Soft, Inc.\Final Fantasy VII\1.00\FFNx)", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &ff7_regkey) == ERROR_SUCCESS)
				{
					if (external_ambient_volume < 0) RegQueryValueEx(ff7_regkey, "AmbientVolume", NULL, NULL, (LPBYTE)&external_ambient_volume, &regsize);
					if (ffmpeg_video_volume < 0) RegQueryValueEx(ff7_regkey, "MovieVolume", NULL, NULL, (LPBYTE)&ffmpeg_video_volume, &regsize);
					if (external_voice_volume < 0) RegQueryValueEx(ff7_regkey, "VoiceVolume", NULL, NULL, (LPBYTE)&external_voice_volume, &regsize);
				}

				if (external_music_path.empty()) external_music_path = "music/vgmstream";
			}
		}
		else if (ff8)
		{
			// Save start address for later
			switch (version)
			{
			case VERSION_FF8_12_US:
				ff8_externals.start = 0x55AC07;
				break;
			case VERSION_FF8_12_US_NV:
				ff8_externals.start = 0x55ADB7;
				break;
			case VERSION_FF8_12_FR:
				ff8_externals.start = 0x55B137;
				break;
			case VERSION_FF8_12_FR_NV:
				ff8_externals.start = 0x55B2E7;
				break;
			case VERSION_FF8_12_DE:
				ff8_externals.start = 0x55B237;
				break;
			case VERSION_FF8_12_DE_NV:
				ff8_externals.start = 0x55B3E7;
				break;
			case VERSION_FF8_12_SP:
				ff8_externals.start = 0x55B227;
				break;
			case VERSION_FF8_12_SP_NV:
				ff8_externals.start = 0x55B3D7;
				break;
			case VERSION_FF8_12_IT:
				ff8_externals.start = 0x55B1B7;
				break;
			case VERSION_FF8_12_IT_NV:
				ff8_externals.start = 0x55B367;
				break;
			case VERSION_FF8_12_US_EIDOS:
				ff8_externals.start = 0x55AB87;
				break;
			case VERSION_FF8_12_US_EIDOS_NV:
				ff8_externals.start = 0x55AD37;
				break;
			case VERSION_FF8_12_JP:
				ff8_externals.start = 0x55F487;
				break;
			case VERSION_FF8_12_JP_NV:
				ff8_externals.start = 0x55F6E7;
				break;
			}

			if (version == VERSION_FF8_12_US_EIDOS || version == VERSION_FF8_12_US_EIDOS_NV)
			{
				MessageBoxA(NULL, "Old Eidos patch detected, please update to the newer 1.2 patch from Square.\n"
					"The old patch may or may not work properly, it is not supported and has not been tested.",
					"Warning", 0);
			}

			ff8_data();

			replace_function(common_externals.create_window, common_create_window);
			replace_function(ff8_externals.manage_time_engine_sub_569971, ff8_manage_time_engine);

			game_cfg_init();

			if (strstr(dllName, "af3dn.p") != NULL)
			{
				ffnx_trace("Detected Steam edition.\n");

				steam_edition = true;

				// Detect if FF8 Stock Launcher
				if (contains(getCopyrightInfoFromExe("FF8_Launcher.exe"), "SQUARE ENIX CO., LTD"))
				{
					steam_stock_launcher = true;
					ffnx_trace("Detected Steam stock launcher.\n");
				}

				// Steam edition contains movies unpacked
				enable_ffmpeg_videos = true;

				// Eyes on me patch

				DWORD mciSendCommandA;

				switch (version)
				{
				case VERSION_FF8_12_US_NV:
				case VERSION_FF8_12_FR_NV:
				case VERSION_FF8_12_DE_NV:
				case VERSION_FF8_12_SP_NV:
				case VERSION_FF8_12_IT_NV:
					mciSendCommandA = 0xB69388;
					break;
				case VERSION_FF8_12_JP:
					mciSendCommandA = 0x2CA3DC8;
					break;
				}

				patch_code_dword(mciSendCommandA, (DWORD)dotemuMciSendCommandA);
			}

			if (external_music_path.empty()) external_music_path = "data/music/dmusic/ogg";
			if (external_music_volume < 0) external_music_volume = 100;
			if (external_sfx_volume < 0) external_sfx_volume = 100;
			if (external_voice_volume < 0) external_voice_volume = 100;
			if (external_ambient_volume < 0) external_ambient_volume = 100;
			if (ffmpeg_video_volume < 0) ffmpeg_video_volume = 100;
		}

		// Init metadata patcher
		if (steam_edition) metadataPatcher.init();

		// Apply hext patching
		hextPatcher.applyAll();
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		unreplace_functions();
	}

	return TRUE;
}

// Steam compatibility
__declspec(dllexport) LSTATUS __stdcall dotemuRegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegCloseKey(HKEY hKey)
{
	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegFlushKey(HKEY hKey)
{
	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegDeleteValueA(HKEY hKey, LPCSTR lpValueName)
{
	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, LPBYTE lpData, DWORD cbData)
{
	if (ff8)
	{
		if (strcmp(lpValueName, "SFXVolume") == 0 || strcmp(lpValueName, "MusicVolume") == 0)
		{
			if (lpData[0] > 0x64)
				lpData[0] = 0x64;
		}
	}
	else
	{
		if (strcmp(lpValueName, "SFXVolume") == 0)
		{
			if (lpData[0] > 0x64)
				lpData[0] = 0x64;

			external_sfx_volume = lpData[0];
		}
		else if (strcmp(lpValueName, "MusicVolume") == 0)
		{
			if (lpData[0] > 0x64)
				lpData[0] = 0x64;

			external_music_volume = lpData[0];
		}
	}

	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	LSTATUS ret = ERROR_SUCCESS;

	LPSTR buf = new CHAR[*lpcbData]{ 0 };

	/* FF7 */
	// General
	if (strcmp(lpValueName, "AppPath") == 0)
	{
		GetCurrentDirectory(*lpcbData, buf);
		strcat(buf, R"(\)");

		strcpy((CHAR*)lpData, buf);
	}
	else if (strcmp(lpValueName, "DataPath") == 0)
	{
		GetCurrentDirectory(*lpcbData, buf);
		strcat(buf, R"(\data\)");

		strcpy((CHAR*)lpData, buf);
	}
	else if (strcmp(lpValueName, "DataDrive") == 0)
	{
		strcpy((CHAR*)lpData, "CD:");
	}
	else if (strcmp(lpValueName, "MoviePath") == 0)
	{
		GetCurrentDirectory(*lpcbData, buf);
		strcat(buf, R"(\data\movies\)");

		strcpy((CHAR*)lpData, buf);
	}
	else if (strcmp(lpValueName, "FullInstall") == 0)
	{
		lpData[0] = 0x1;
	}
	// Audio
	else if (strcmp(lpValueName, "DD_GUID") == 0 || strcmp(lpValueName, "Sound_GUID") == 0)
	{
		memcpy(lpData, buf, *lpcbData);
	}
	else if (strcmp(lpValueName, "MIDI_DeviceID") == 0)
	{
		lpData[0] = 0x0;
	}
	else if (strcmp(lpValueName, "Sound") == 0 || strcmp(lpValueName, "Midi") == 0 || strcmp(lpValueName, "wave_music") == 0)
	{
		ret = 2;
	}
	else if (strcmp(lpValueName, "SFXVolume") == 0)
	{
		lpData[0] = external_sfx_volume;
	}
	else if (strcmp(lpValueName, "MusicVolume") == 0)
	{
		lpData[0] = external_music_volume;
	}
	// Graphics
	else if (strcmp(lpValueName, "Driver") == 0)
	{
		lpData[0] = 0x3;
	}
	else if (strcmp(lpValueName, "DriverPath") == 0)
	{
		strcpy((CHAR*)lpData, R"(AF3DN.P)");
	}
	else if (strcmp(lpValueName, "Mode") == 0)
	{
		lpData[0] = 0x2;

		// Steam release is somehow requesting this key multiple times.
		// Returning 1 will set 2 internally in the engine
		if (*lpcbData == 256) ret = 1;
	}
	else if (strcmp(lpValueName, "Options") == 0)
	{
		lpData[0] = 0x0;
	}
	/* FF8 */
	else if (strcmp(lpValueName, "GraphicsGUID") == 0 || strcmp(lpValueName, "SoundGUID") == 0 || strcmp(lpValueName, "MIDIGUID") == 0)
	{
		memcpy(lpData, buf, 16);
	}
	else if (strcmp(lpValueName, "SoundOptions") == 0)
	{
		lpData[0] = 0x00000000;
	}
	else if (strcmp(lpValueName, "InstallOptions") == 0)
	{
		lpData[0] = 0x000000ff;
	}
	else if (strcmp(lpValueName, "MidiOptions") == 0)
	{
		lpData[0] = 0x00000001;
	}
	else if (strcmp(lpValueName, "Graphics") == 0)
	{
		lpData[0] = 0x00100021;
	}

	delete[] buf;

	return ret;
}

__declspec(dllexport) HANDLE __stdcall dotemuCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	if (ff8_fs_last_fopen_is_redirected())
	{
		return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	HANDLE ret = INVALID_HANDLE_VALUE;

	if (strstr(lpFileName, "CD:") != NULL)
	{
		CHAR newPath[MAX_PATH]{ 0 };
		uint8_t requiredDisk = *(uint8_t*)(*(DWORD*)ff8_externals.savemap + 0xCC);
		CHAR diskAsChar[2];

		itoa(requiredDisk, diskAsChar, 10);

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		if (strstr(lpFileName, "DISK1") != NULL || strstr(lpFileName, "DISK2") != NULL || strstr(lpFileName, "DISK3") != NULL || strstr(lpFileName, "DISK4") != NULL)
		{
			PathAppendA(newPath, ff8_externals.app_path);
			PathAppendA(newPath, R"(data\disk)");
			PathAppendA(newPath, pos);

			if (strstr(lpFileName, diskAsChar) != NULL)
			{
				ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
			}
		}
	}
	else if (strstr(lpFileName, "app.log") || strstr(lpFileName, "ff8input.cfg"))
	{
		CHAR newPath[MAX_PATH]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, sizeof(newPath), false);
		PathAppendA(newPath, JP_VERSION ? "ff8input_jp.cfg" : pos);

		ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else if (strstr(lpFileName, "temp.fi") || strstr(lpFileName, "temp.fl") || strstr(lpFileName, "temp.fs") || strstr(lpFileName, "temp_evn.") || strstr(lpFileName, "temp_odd."))
	{
		CHAR newPath[MAX_PATH]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, sizeof(newPath), false);
		PathAppendA(newPath, pos);

		ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else if (strstr(lpFileName, ".fi") != NULL || strstr(lpFileName, ".fl") != NULL || strstr(lpFileName, ".fs") != NULL)
	{
		CHAR newPath[MAX_PATH]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_data_lang_path(newPath);
		PathAppendA(newPath, pos);

		ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else if (StrStrIA(lpFileName, R"(SAVE\)") != NULL) // SAVE\SLOTX\saveN or save\chocorpg
	{
		CHAR newPath[MAX_PATH]{ 0 };
		CHAR saveFileName[50]{ 0 };

		// Search for the next character pointer after "SAVE\"
		const char* pos = StrStrIA(lpFileName, R"(SAVE\)") + 5;
		strcpy(saveFileName, pos);
		_strlwr(saveFileName);
		char* posSeparator = strstr(saveFileName, R"(\)");
		if (posSeparator != NULL)
		{
			*posSeparator = '_';
		}
		strcat(saveFileName, R"(.ff8)");

		get_userdata_path(newPath, sizeof(newPath), true);
		PathAppendA(newPath, saveFileName);

		ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else
		ret = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	return ret;
}

__declspec(dllexport) UINT __stdcall dotemuGetDriveTypeA(LPCSTR lpRootPathName)
{
	UINT ret;

	if (strcmp(lpRootPathName, "CD:") == 0)
	{
		ret = DRIVE_CDROM;
	}
	else
	{
		ret = GetDriveTypeA(lpRootPathName);
	}

	return ret;
}

__declspec(dllexport) BOOL __stdcall dotemuDeleteFileA(LPCSTR lpFileName)
{
	BOOL ret = false;

	if (strstr(lpFileName, "app.log"))
	{
		CHAR newPath[260]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, 260, false);
		PathAppendA(newPath, pos);

		ret = DeleteFileA(newPath);
	}
	else if (strstr(lpFileName, "temp.fi") || strstr(lpFileName, "temp.fl") || strstr(lpFileName, "temp.fs") || strstr(lpFileName, "temp_evn.") || strstr(lpFileName, "temp_odd."))
	{
		CHAR newPath[260]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, 260, false);
		PathAppendA(newPath, pos);

		ret = DeleteFileA(newPath);
	}
	else
		ret = DeleteFileA(lpFileName);

	return ret;
}

// FF8 2000 Compatibility
__declspec(dllexport) HRESULT __stdcall EAXDirectSoundCreate(LPGUID guid, LPLPDIRECTSOUND directsound, IUnknown FAR* unk)
{
	typedef HRESULT(FAR PASCAL* LPEAXDIRECTSOUNDCREATE)(LPGUID, LPLPDIRECTSOUND, IUnknown FAR*);
	char eax_dll[MAX_PATH] = {};

	if (fileExists("creative_eax.dll")) {
		// For portable installation, this name can be used to load the official Creative EAX 2.0+ DLL along with FFNx
		snprintf(eax_dll, sizeof(eax_dll), R"(%s\creative_eax.dll)", basedir);
	} else {
		GetSystemDirectoryA(eax_dll, sizeof(eax_dll));
		strcat(eax_dll, R"(\eax.dll)");
	}

	FARPROC procDSoundCreate = NULL;
	HMODULE hDll = LoadLibraryA(eax_dll);
	if (hDll != NULL) {
		procDSoundCreate = GetProcAddress(hDll, "EAXDirectSoundCreate");
	}

	if (procDSoundCreate == NULL) {
		ffnx_warning("%s: Cannot load EAX Library, please install Creative EAX Unified redistribuable version 2.0+\n", __func__);

		hDll = LoadLibraryA("dsound.dll");
		if (hDll != NULL) {
			// EAXDirectSoundCreate is basically DirectSoundCreate with more features
			procDSoundCreate = GetProcAddress(hDll, "DirectSoundCreate");
		}
	}

	return LPEAXDIRECTSOUNDCREATE(procDSoundCreate)(guid, directsound, unk);
}

void ffnx_inject_driver(struct game_obj* game_object)
{
	VOBJ(game_obj, game_object, game_object);

	VRASS(game_object, current_gfx_driver, 2);
	VRASS(game_object, create_gfx_driver, new_dll_graphics_driver);
}

#if defined(__cplusplus)
}
#endif

constexpr int FFNX_LOGO_FRAME_COUNT = 180;
int ffnx_logo_current_frame = 0;

bool drawFFNxLogoFrame(struct game_obj* game_object)
{
	static int was_lighting_enabled = -1;

	if (was_lighting_enabled == -1) was_lighting_enabled = enable_lighting;

	if (ffnx_logo_current_frame >= FFNX_LOGO_FRAME_COUNT) {
		newRenderer.setOverallColorGamut(enable_ntscj_gamut_mode ? COLORGAMUT_NTSCJ : COLORGAMUT_SRGB); // set the gamut back to what it was before newRenderer.drawFFNxLogo() changed it
		enable_lighting = was_lighting_enabled;
		return false;
	}

	VOBJ(game_obj, game_object, game_object);

	int fade_frame_count = FFNX_LOGO_FRAME_COUNT / 3;
	float fade = 0.0;

	if(ffnx_logo_current_frame < fade_frame_count)
		fade = ffnx_logo_current_frame / static_cast<float>(fade_frame_count);
	else if(ffnx_logo_current_frame < 2 * fade_frame_count)
		fade = 1.0f;
	else
		fade = 1.0f - (ffnx_logo_current_frame - 2 * fade_frame_count) / static_cast<float>(fade_frame_count);

	enable_lighting = false;

	newRenderer.drawFFNxLogo(fade);

	common_flip(game_object);

	ffnx_logo_current_frame++;

	return true;
}

void stopDrawFFNxLogo()
{
	ffnx_logo_current_frame = FFNX_LOGO_FRAME_COUNT;
}
