/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include "directmusic.h"
#include "audio.h"
#include "music.h"
#include "patch.h"

uint32_t playing_midi = UINT32_MAX;
bool was_battle_gameover = false;

uint32_t midi_fadetime = 0;
uint32_t midi_to_resume = 0;

static uint32_t noop() { return 0; }

void music_flush()
{
	midi_fadetime = 0;
	midi_to_resume = 0;
	nxAudioEngine.flush();
}

bool is_gameover(uint32_t midi)
{
	bool ret = false;

	if (ff8)
	{
		switch (midi)
		{
		case 0: // Lose
			ret = true;
		}
	}
	else
	{
		switch (midi)
		{
		case 58: // OVER2
			ret = true;
		}
	}

	return ret;
}

bool needs_resume(uint32_t midi)
{
	bool ret = false;

	if (external_music_resume)
	{
		if (ff8)
		{
			switch (midi)
			{
			case 41: // FIELD
				ret = true;
			}
		}
		else
		{
			switch (midi)
			{
			case 13: // TA
			case 71: // KITA
				ret = true;
			}
		}
	}

	return ret;
}

uint32_t ff7_no_loop_ids[11] = {
	5, // FANFARE
	14, // TB
	22, // WALZ
	48, // CANNON
	57, // YADO
	89, // RO
	90, // JYRO
	92, // RIKU
	93, // SI
	94, // MOGU
	98, // ROLL
};

bool no_loop(uint32_t midi)
{
	if (ff8) {
		return false; // TODO
	}

	for (int i = 0; i < 11; ++i) {
		if (ff7_no_loop_ids[i] == midi) {
			return true;
		}
	}

	return false;
}

uint32_t ff7_midi_init(uint32_t unknown)
{
	// without this there will be no volume control for music in the config menu
	*ff7_externals.midi_volume_control = true;

	// enable fade function
	*ff7_externals.midi_initialized = true;

	return true;
}

char ff8_midi[32];

char* ff8_midi_name(uint32_t midi)
{
	if (midi != UINT_MAX)
	{
		// midi_name format: {num}{type}-{name}.sgt or {name}.sgt or _Missing.sgt
		char* midi_name = common_externals.get_midi_name(midi),
			* truncated_name;

		truncated_name = strchr(midi_name, '-');

		if (nullptr != truncated_name) {
			truncated_name += 1; // Remove "-"
		}
		else {
			truncated_name = midi_name;
		}

		char* max_midi_name = strchr(truncated_name, '.');

		if (nullptr != max_midi_name) {
			size_t len = max_midi_name - truncated_name;

			if (len < 32) {
				memcpy(ff8_midi, truncated_name, len);
				ff8_midi[len] = '\0';

				return ff8_midi;
			}
		}
	}

	return nullptr;
}

void pause_midi()
{
	if (trace_all || trace_music) trace("%s: midi=%s\n", __func__, ff8 ? ff8_midi_name(playing_midi) : common_externals.get_midi_name(playing_midi));

	nxAudioEngine.pauseMusic();
}

void restart_midi()
{
	if (trace_all || trace_music) trace("%s: midi=%s\n", __func__, ff8 ? ff8_midi_name(playing_midi) : common_externals.get_midi_name(playing_midi));

	nxAudioEngine.resumeMusic();
}

uint32_t ff7_use_midi(uint32_t midi)
{
	char* name = common_externals.get_midi_name(midi);

	if (nxAudioEngine.canPlayMusic(name)) {
		return 1;
	}

	return strcmp(name, "HEART") != 0 && strcmp(name, "SATO") != 0 && strcmp(name, "SENSUI") != 0 && strcmp(name, "WIND") != 0;
}

void ff7_play_midi(uint32_t midi)
{
	if (playing_midi != midi)
	{
		if (is_gameover(midi)) music_flush();

		char* midi_name = common_externals.get_midi_name(midi);
		struct game_mode* mode = getmode_cached();

		// Avoid restarting the same music when transitioning from the battle gameover to the gameover screen
		if (mode->driver_mode == MODE_GAMEOVER && was_battle_gameover)
		{
			was_battle_gameover = false;
			return;
		}

		if (mode->driver_mode == MODE_BATTLE && midi == 58) was_battle_gameover = true;

		if (midi_to_resume == midi && needs_resume(midi))
		{
			playing_midi = midi;
			restart_midi();
			midi_to_resume = 0;
		}
		else
		{
			if (needs_resume(playing_midi)) pause_midi();
			nxAudioEngine.playMusic(midi_name);
			nxAudioEngine.setMusicLooping(!no_loop(midi));
			playing_midi = midi;
		}

		midi_fadetime = 0;

		if (needs_resume(playing_midi)) midi_to_resume = playing_midi;

		if (trace_all || trace_music) trace("%s: midi_id=%u, midi=%s\n", __func__, playing_midi, midi_name);
	}
}

void stop_midi()
{
	if (playing_midi != UINT_MAX)
	{
		if (!ff8)
		{
			struct game_mode* mode = getmode_cached();

			// Do not stop the gameover music if coming from a battle
			if (mode->driver_mode == MODE_GAMEOVER && was_battle_gameover) return;
		}

		if (trace_all || trace_music) trace("%s: midi=%s\n", __func__, ff8 ? ff8_midi_name(playing_midi) : common_externals.get_midi_name(playing_midi));

		if (playing_midi == midi_to_resume) pause_midi();
		else nxAudioEngine.stopMusic();

		playing_midi = UINT_MAX;
	}
}

void cross_fade_midi(uint32_t midi, uint32_t time /* seconds */)
{
	char* midi_name = common_externals.get_midi_name(midi);

	midi_fadetime = midi == 0 ? time : time / 2;

	if (playing_midi != midi)
	{
		if (midi == 1)
		{
			midi = (playing_midi == 2) + 1;
		}
		else if (midi == 2)
		{
			midi = (playing_midi != 1) + 1;
		}

		if (midi_to_resume == midi && needs_resume(midi))
		{
			playing_midi = midi;
			restart_midi();
			midi_to_resume = 0;
		}
		else
		{
			if (needs_resume(playing_midi)) pause_midi();
			nxAudioEngine.playMusic(midi_name, true, midi_fadetime);
			playing_midi = midi;
		}

		if (needs_resume(playing_midi)) midi_to_resume = playing_midi;
	}

	if (trace_all || trace_music) trace("%s: midi_id=%u, midi=%s, time=%us\n", __func__, midi, midi_name, midi_fadetime);
}

uint32_t midi_status()
{
	if (trace_all || trace_music) trace("%s: midi=%s\n", __func__, ff8 ? ff8_midi_name(playing_midi) : common_externals.get_midi_name(playing_midi));

	nxAudioEngine.setMusicLooping(false);

	return nxAudioEngine.isMusicPlaying();
}

void set_master_midi_volume(uint32_t volume)
{
	if (trace_all || trace_music) trace("%s: volume=%u\n", __func__, volume);

	nxAudioEngine.setMusicMasterVolume(volume / 100.0f);
}

void set_midi_volume(uint32_t volume)
{
	if (volume > 127) volume = 127;

	if (trace_all || trace_music) trace("%s: volume=%u\n", __func__, volume);

	nxAudioEngine.setMusicVolume(volume / 127.0f);
}

void set_midi_volume_trans(uint32_t volume, uint32_t step)
{
	if (volume > 127) volume = 127;

	if (trace_all || trace_music) trace("%s: volume=%u, step=%u\n", __func__, volume, step);

	if (step)
	{
		if (!volume)
		{
			if (playing_midi == midi_to_resume) pause_midi();
			else nxAudioEngine.stopMusic(midi_fadetime);
			playing_midi = UINT32_MAX;
		}
		else
		{
			nxAudioEngine.setMusicVolume(volume / 127.0f, midi_fadetime);
		}
	}
	else if (volume)
	{
		nxAudioEngine.setMusicVolume(volume / 127.0f);
	}
	else
	{
		if (playing_midi == midi_to_resume) pause_midi();
		else stop_midi();
	}
}

void set_midi_tempo(char tempo)
{
	if (trace_all || trace_music) trace("%s: tempo=%d\n", __func__, tempo);

	float speed = 1.0;

	if (float(tempo) >= 0.0)
		speed = 1.0 - 0.5 * float(tempo) / 127.0;
	else
		speed = float(tempo) / -128.0 + 1.0;

	nxAudioEngine.setMusicSpeed(speed);
}

uint32_t remember_playing_time()
{
	midi_to_resume = playing_midi;
	
	return 0;
}

uint32_t music_sound_operation_fix(uint32_t type, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5)
{
	if (trace_all) trace("AKAO call type=%X params=(%i %i %i %i)\n", type, param1, param2, param3, param4, param5);

	if (type == 0xDA) { // Assimilated to stop music (Cid speech in Highwind)
		return ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(0xF0, 0, 0, 0, 0, 0);
	}

	return ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(type, param1, param2, param3, param4, param5);
}

uint32_t ff8_play_midi(uint32_t midi, uint32_t volume, uint32_t u1, uint32_t u2)
{
	if (playing_midi != midi)
	{
		if (is_gameover(midi)) music_flush();

		char* midi_name = ff8_midi_name(midi);

		if (nullptr == midi_name) {
			return 0; // Error
		}

		if (trace_all || trace_music) trace("%s: midi_id=%u,midi=%s,volume=%u,u1=%u,u2=%u\n", __func__, midi, midi_name, volume, u1, u2);

		playing_midi = midi;

		nxAudioEngine.playMusic(midi_name);
		nxAudioEngine.setMusicVolume(volume / 127.0f);
	}

	return 1; // Success
}

uint32_t ff8_stop_midi()
{
	stop_midi();

	return 0;
}

uint32_t ff8_set_midi_volume(int volume)
{
	if (trace_all || trace_music) trace("%s: set direct volume %d\n", __func__, volume);

	nxAudioEngine.setMusicVolume((volume + 10000.0f) / 10000.0f);

	return 1; // Success
}

void music_init()
{
	if (!ff8)
	{
		// Fix music stop issue in FF7
		patch_code_dword(ff7_externals.music_lock_clear_fix + 2, 0xCC195C);
		// Fix Cid speech music stop
		replace_call(ff7_externals.opcode_akao + 0xEA, music_sound_operation_fix);
		replace_call(ff7_externals.opcode_akao2 + 0xE8, music_sound_operation_fix);
	}

	if (use_external_music)
	{
		if (ff8)
		{
			replace_function(common_externals.play_midi, ff8_play_midi);
			replace_function(common_externals.pause_midi, pause_midi);
			replace_function(common_externals.restart_midi, restart_midi);
			replace_function(common_externals.stop_midi, ff8_stop_midi);
			replace_function(common_externals.midi_status, midi_status);
			replace_function(common_externals.set_midi_volume, ff8_set_midi_volume);
			replace_function(common_externals.remember_midi_playing_time, remember_playing_time);
		}
		else
		{
			replace_function(common_externals.midi_init, ff7_midi_init);
			replace_function(common_externals.use_midi, ff7_use_midi);
			replace_function(common_externals.play_midi, ff7_play_midi);
			replace_function(common_externals.cross_fade_midi, cross_fade_midi);
			replace_function(common_externals.pause_midi, pause_midi);
			replace_function(common_externals.restart_midi, restart_midi);
			replace_function(common_externals.stop_midi, stop_midi);
			replace_function(common_externals.midi_status, midi_status);
			replace_function(common_externals.set_master_midi_volume, set_master_midi_volume);
			replace_function(common_externals.set_midi_volume, set_midi_volume);
			replace_function(common_externals.set_midi_volume_trans, set_midi_volume_trans);
			replace_function(common_externals.set_midi_tempo, set_midi_tempo);
			replace_function(common_externals.midi_cleanup, noop);
		}
	}
}
