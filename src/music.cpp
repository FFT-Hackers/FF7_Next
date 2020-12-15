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
#include <unordered_map>

#include "audio.h"
#include "music.h"
#include "patch.h"

bool was_battle_gameover = false;
bool current_music_is_field_resumable = false;
bool next_music_channel = 0;
bool next_music_is_skipped = false;
bool next_music_is_maybe_skipped = false;
std::unordered_map<uint32_t, SoLoud::time> remember_musics;
bool next_music_is_not_multi = false;
bool multi_music_is_playing = false;
double next_music_fade_time = 0.0;

static uint32_t noop() { return 0; }

void music_flush()
{
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

uint32_t music_mode(uint32_t midi)
{
	if (ff8)
	{
		switch (midi)
		{
		case 41: // FIELD (Worldmap theme)
			return MODE_WORLDMAP;
		}
	}
	else
	{
		switch (midi)
		{
		case 13: // TA (Worldmap theme 1)
		case 71: // KITA (Worldmap theme 2)
			return MODE_WORLDMAP;
		}
	}

	return getmode_cached()->driver_mode == MODE_FIELD ? MODE_FIELD : MODE_EXIT;
}

NxAudioEngine::PlayFlags needs_resume(uint32_t midi)
{
	NxAudioEngine::PlayFlags ret = NxAudioEngine::PlayFlagsNone;

	if (external_music_resume)
	{
		uint32_t mode = music_mode(midi);

		switch (mode)
		{
		case MODE_FIELD:
			if (next_music_channel == 0) {
				ret = NxAudioEngine::PlayFlagsIsResumable;

				// Field channels are exclusive
				if (current_music_is_field_resumable) {
					ret = ret | NxAudioEngine::PlayFlagsDoNotPause;
				}
			}
			break;
		case MODE_WORLDMAP:
			ret = NxAudioEngine::PlayFlagsIsResumable;

			// Come from field
			if (current_music_is_field_resumable) {
				ret = ret | NxAudioEngine::PlayFlagsDoNotPause;
			}
			break;
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

char* ff8_format_midi_name(const char* midi_name)
{
	// midi_name format: {num}{type}-{name}.sgt or {name}.sgt or _Missing.sgt
	const char* truncated_name = strchr(midi_name, '-');

	if (nullptr != truncated_name) {
		truncated_name += 1; // Remove "-"
	}
	else {
		truncated_name = midi_name;
	}

	const char* max_midi_name = strchr(truncated_name, '.');

	if (nullptr != max_midi_name) {
		size_t len = max_midi_name - truncated_name;

		if (len < 32) {
			memcpy(ff8_midi, truncated_name, len);
			ff8_midi[len] = '\0';

			return ff8_midi;
		}
	}

	return nullptr;
}

char* ff8_midi_name(uint32_t midi)
{
	if (midi != UINT_MAX)
	{
		const char* midi_name = common_externals.get_midi_name(midi);
		return ff8_format_midi_name(midi_name);
	}

	return nullptr;
}

char* current_midi_name()
{
	uint32_t midi = nxAudioEngine.currentMusicId();
	return ff8 ? ff8_midi_name(midi) : common_externals.get_midi_name(midi);
}

void pause_midi()
{
	if (trace_all || trace_music) trace("%s: midi=%s\n", __func__, current_midi_name());

	nxAudioEngine.pauseMusic();
}

void restart_midi()
{
	if (trace_all || trace_music) trace("%s: midi=%s\n", __func__, current_midi_name());

	nxAudioEngine.resumeMusic();
}

uint32_t ff7_use_midi(uint32_t midi)
{
	char* name = common_externals.get_midi_name(midi);
	uint32_t ret = 0;

	if (nxAudioEngine.canPlayMusic(name)) {
		ret = 1;
	}
	else {
		ret = strcmp(name, "HEART") != 0 && strcmp(name, "SATO") != 0 && strcmp(name, "SENSUI") != 0 && strcmp(name, "WIND") != 0;
	}

	if (ret != 1) {
		next_music_channel = 0;
	}

	return ret;
}

bool play_midi_helper(char* midi_name, uint32_t midi, uint32_t fade_time = 0, SoLoud::time offset = 0, bool noIntro = false)
{
	NxAudioEngine::PlayOptions options = NxAudioEngine::PlayOptions();
	options.fadetime = fade_time;
	options.flags = needs_resume(midi);
	options.noIntro = noIntro;
	options.offsetSeconds = offset;
	bool ret = nxAudioEngine.playMusic(midi_name, midi, options);

	if (ret)
	{
		nxAudioEngine.setMusicLooping(!no_loop(midi));

		if (music_mode(midi) == MODE_FIELD) {
			current_music_is_field_resumable = next_music_channel == 0;
		}
		else {
			current_music_is_field_resumable = false;
		}

		next_music_channel = 0;
	}

	return ret;
}

void ff7_play_midi(uint32_t midi)
{
	if (nxAudioEngine.currentMusicId() != midi)
	{
		if (is_gameover(midi)) music_flush();

		char* midi_name = common_externals.get_midi_name(midi);
		struct game_mode* mode = getmode_cached();

		// Avoid restarting the same music when transitioning from the battle gameover to the gameover screen
		if (mode->driver_mode == MODE_GAMEOVER && was_battle_gameover)
		{
			was_battle_gameover = false;
			next_music_channel = 0;
			return;
		}

		if (mode->driver_mode == MODE_BATTLE && midi == 58) was_battle_gameover = true;

		// Attempt to customize the battle theme flow
		if (strcmp(midi_name, "BAT") == 0)
		{
			// Do only in fields for now
			if (*common_externals._previous_mode == FF7_MODE_FIELD && mode->driver_mode == MODE_SWIRL)
			{
				char battle_name[50];

				sprintf(battle_name, "bat_%d", *ff7_externals.battle_id);

				// Attempt to load theme by Battle ID
				if (!play_midi_helper(battle_name, midi))
				{
					sprintf(battle_name, "bat_%s", get_current_field_name());

					// Attempt to load theme by Field name
					if (!play_midi_helper(battle_name, midi))
						// Nothing worked, switch back to default
						play_midi_helper(midi_name, midi);
				}
			}
			else
				play_midi_helper(midi_name, midi);
		}
		// Attempt to override field music
		else if (mode->driver_mode == MODE_FIELD)
		{
			char field_name[50];

			sprintf(field_name, "field_%d", *ff7_externals.field_id);

			// Attempt to load theme by Field ID
			if (!play_midi_helper(field_name, midi))
				// Nothing worked, switch back to default
				play_midi_helper(midi_name, midi);
		}
		else
			play_midi_helper(midi_name, midi);

		if (trace_all || trace_music) trace("%s: midi_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(), midi_name);
	}
}

void stop_midi()
{
	if (nxAudioEngine.isMusicPlaying())
	{
		if (!ff8)
		{
			struct game_mode* mode = getmode_cached();

			// Do not stop the gameover music if coming from a battle
			if (mode->driver_mode == MODE_GAMEOVER && was_battle_gameover) return;
		}

		if (trace_all || trace_music) trace("%s: midi=%s\n", __func__, current_midi_name());

		nxAudioEngine.stopMusic();
	}
}

void cross_fade_midi(uint32_t midi, uint32_t steps)
{
	char* midi_name = common_externals.get_midi_name(midi);

	/* FIXME: the game uses cross_fade_midi only in two places,
	 * with steps = 4 everytime. In the PS version, theses transitions
	 * last 1-2 seconds fade in / out, no more.
	 * Therefore steps value is clearly wrong here, so the formula
	 * to get the correct time is different than the one in set_midi_volume_trans.
	 */
	double time = (steps & 0xFF) / 4.0;

	if (midi != 0)
	{
		if (midi == 1)
		{
			midi = (nxAudioEngine.currentMusicId() == 2) + 1;
		}
		else if (midi == 2)
		{
			midi = (nxAudioEngine.currentMusicId() != 1) + 1;
		}

		if (nxAudioEngine.currentMusicId() != midi)
		{
			if (nxAudioEngine.isMusicPlaying())
			{
				nxAudioEngine.stopMusic(time);
			}
			else
			{
				nxAudioEngine.setMusicVolume(1.0f); // Target volume
			}

			play_midi_helper(midi_name, midi, time);
		}
		else
		{
			nxAudioEngine.setMusicVolume(1.0f, time);
		}
	}
	else
	{
		stop_midi();
	}

	if (trace_all || trace_music) trace("%s: midi_id=%u, midi=%s, time=%fs\n", __func__, midi, midi_name, time);
}

uint32_t midi_status()
{
	if (trace_all || trace_music) trace("%s: midi=%s\n", __func__, current_midi_name());

	// When the game asks for a music status, you know that it ends eventually
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

void set_midi_volume_trans(uint32_t volume, uint32_t steps)
{
	if (volume > 127) volume = 127;

	double time = (steps & 0xFF) / 64.0;

	if (trace_all || trace_music) trace("%s: volume=%u, steps=%u (=> time=%fs)\n", __func__, volume, steps, time);

	if (steps)
	{
		if (!volume)
		{
			nxAudioEngine.stopMusic(time);
		}
		else
		{
			nxAudioEngine.setMusicVolume(volume / 127.0f, time);
		}
	}
	else if (volume)
	{
		nxAudioEngine.setMusicVolume(volume / 127.0f);
	}
	else
	{
		stop_midi();
	}
}

void set_midi_tempo(int8_t tempo)
{
	if (trace_all || trace_music) trace("%s: tempo=%d\n", __func__, tempo);

	if (tempo == -128) {
		tempo = -127; // Prevent speed to be 0 (can crash with SoLoud)
	}

	float speed = float(tempo) / 128.0f + 1.0f;

	// FIXME: will change the pitch
	nxAudioEngine.setMusicSpeed(speed);
}

uint32_t remember_playing_time()
{
	nxAudioEngine.pauseMusic(0, true);

	return 0;
}

void change_next_music_channel()
{
	if (trace_all || trace_music) trace("%s: set field music channel to 1\n", __func__);

	next_music_channel = 1;
}

uint32_t ff7_music_sound_operation_fix(uint32_t type, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5)
{
	if (trace_all || trace_music) trace("AKAO call type=%X params=(%i %i %i %i)\n", type, param1, param2, param3, param4, param5);

	next_music_channel = 0;
	type &= 0xFF; // The game does not always set this parameter as a 32-bit integer

	if (type == 0xDA) { // Assimilated to stop music (Cid speech in Highwind)
		return ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(0xF0, 0, 0, 0, 0, 0);
	}

	// Play music (channel #2) and Play music with fade (channel #2)
	if (use_external_music && (type == 0x14 || type == 0x19)) {
		uint32_t midi_id = param1;
		if (midi_id > 0 && midi_id <= 0x62) {
			change_next_music_channel();
		}
	}

	return ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(type, param1, param2, param3, param4, param5);
}

uint32_t ff8_opcode_dualmusic_play_music(char* data, uint32_t volume)
{
	change_next_music_channel();
	return ((uint32_t(*)(uint32_t, char*, uint32_t))ff8_externals.sd_music_play)(1, data, volume);
}

uint32_t ff8_cross_fade_midi(char* data, uint32_t steps, uint32_t volume)
{
	double time = (steps & 0xFF) / 64.0;

	if (trace_all || trace_music) trace("%s: steps=%u (time=%fs), volume=%u\n", __func__, steps, time, volume);

	next_music_fade_time = time;

	uint32_t ret = ((uint32_t(*)(uint32_t, char*, uint32_t))ff8_externals.sd_music_play)(0, data, volume);

	next_music_fade_time = 0;

	return ret;
}

uint32_t ff8_play_midi(uint32_t midi, uint32_t volume, uint32_t unused1, uint32_t unused2)
{
	if (nxAudioEngine.currentMusicId() != midi)
	{
		if (is_gameover(midi)) music_flush();

		char* midi_name = ff8_midi_name(midi);

		if (nullptr == midi_name) {
			next_music_channel = 0;
			return 0; // Error
		}

		if (trace_all || trace_music) trace("%s: midi_id=%u, midi=%s, volume=%u\n", __func__, midi, midi_name, volume);

		SoLoud::time offset = 0;
		bool noIntro = false;

		if (next_music_is_maybe_skipped && remember_musics[midi] != 0) {
			offset = remember_musics[midi];
			remember_musics[midi] = 0;
			if (trace_all || trace_music) trace("%s: use remembered music %d s\n", __func__, offset);
		}
		else if (next_music_is_skipped) {
			noIntro = true;
		}
		else if (remember_musics[midi] != 0) {
			if (trace_all || trace_music) trace("%s: discard remembered music\n", __func__);
			remember_musics[midi] = 0;
		}

		next_music_is_maybe_skipped = false;
		next_music_is_skipped = false;

		play_midi_helper(midi_name, midi, next_music_fade_time, offset, noIntro);
		nxAudioEngine.setMusicVolume(volume / 127.0f);
	}

	return 1; // Success
}

uint32_t ff8_play_midi_at(char* midi_data, uint32_t offset)
{
	// We don't know what offset means in seconds
	if (trace_all || trace_music) trace("%s: play midi at %d\n", __func__, offset);
	next_music_is_skipped = true;
	((uint32_t(*)(uint32_t,char*,uint32_t))ff8_externals.sd_music_play)(0, midi_data, *ff8_externals.current_volume);
	return 0;
}

uint32_t ff8_opcode_musicskip_play_midi_at(char* midi_data, uint32_t offset)
{
	if (trace_all || trace_music) trace("%s: music skip, play midi at %d\n", __func__, offset);
	next_music_is_maybe_skipped = true;
	return ff8_play_midi_at(midi_data, offset);
}

uint32_t ff8_opcode_getmusicoffset()
{
	if (trace_all || trace_music) trace("%s: save music %d time %fs\n", __func__, nxAudioEngine.currentMusicId(), nxAudioEngine.getMusicPlayingTime());

	remember_musics[nxAudioEngine.currentMusicId()] = std::fmod(nxAudioEngine.getMusicPlayingTime(), 60.0 * 2);

	return 0;
}

uint32_t ff8_stop_midi()
{
	stop_midi();

	return 0;
}

uint32_t ff8_stop_wav(uint32_t number)
{
	if (ff8_externals.music_channel_type[number] == 2) { // WAV type
		ff8_externals.stop_wav(0);
	}

	return 1;
}

uint32_t ff8_set_midi_volume(int db_volume)
{
	uint32_t volume = *ff8_externals.current_volume;

	if (trace_all || trace_music) trace("%s: set direct volume %d Db (%d)\n", __func__, db_volume, volume);

	set_midi_volume(volume);

	if (multi_music_is_playing && volume == 0) {
		nxAudioEngine.stopMusic();
		multi_music_is_playing = false;
	}

	return 1; // Success
}

std::vector<std::string> musics;

uint32_t ff8_opcode_choicemusic(uint32_t unused, uint32_t instruments)
{
	if (trace_all || trace_music) trace("%s: Clear musics\n", __func__);

	musics.clear();

	return ((uint32_t(*)(uint32_t, uint32_t))ff8_externals.choice_music)(unused, instruments);
}

uint32_t ff8_load_midi_segment(void* directsound, const char* filename)
{
	char* midi_name = ff8_format_midi_name(filename);

	if (next_music_is_not_multi) {
		next_music_is_not_multi = false;
		play_midi_helper(midi_name, 43);
		return 1; // Success
	}

	if (trace_all || trace_music) trace("%s: load music %s (%s)\n", __func__, midi_name, filename);

	musics.push_back(midi_name);

	return 1; // Success
}

uint32_t ff8_play_midi_segments()
{
	if (trace_all || trace_music) trace("%s\n", __func__);

	multi_music_is_playing = true;
	nxAudioEngine.playMusics(musics, 43);
	nxAudioEngine.setMusicLooping(true);

	return 0;
}

uint32_t ff8_load_and_play_midi_segment(uint32_t segment_id)
{
	// In this call we know that we play only one segment
	next_music_is_not_multi = true;
	((uint32_t(*)(uint32_t))ff8_externals.load_midi_segment_from_id)(segment_id);
	next_music_is_not_multi = false;

	return 0; // Fail (to prevent execution of the game's code)
}

uint32_t ff8_stop_midi_segments()
{
	if (trace_all || trace_music) trace("%s\n", __func__);

	nxAudioEngine.stopMusic();
	multi_music_is_playing = false;

	return 0;
}

uint32_t ff8_midi_segments_status()
{
	if (trace_all) trace("%s\n", __func__);

	// Let midi_status() drive this
	return 0;
}

void music_init()
{
	if (!ff8)
	{
		// Fix music stop issue in FF7
		patch_code_dword(ff7_externals.music_lock_clear_fix + 2, 0xCC195C);
		// Fix Cid speech music stop + music channel detection (field only)
		replace_call(ff7_externals.opcode_akao + 0xEA, ff7_music_sound_operation_fix);
		replace_call(ff7_externals.opcode_akao2 + 0xE8, ff7_music_sound_operation_fix);
		replace_call(ff7_externals.field_music_helper_sound_op_call, ff7_music_sound_operation_fix);
	}

	if (use_external_music)
	{
		if (ff8)
		{
			replace_function(common_externals.play_midi, ff8_play_midi);
			replace_function(common_externals.cross_fade_midi, ff8_cross_fade_midi);
			replace_function(common_externals.pause_midi, pause_midi);
			replace_function(common_externals.restart_midi, restart_midi);
			replace_function(common_externals.stop_midi, ff8_stop_midi);
			replace_function(common_externals.midi_status, midi_status);
			replace_function(common_externals.set_midi_volume, ff8_set_midi_volume);
			/* Remember time and resume music */
			replace_function(ff8_externals.sd_music_play_at, ff8_play_midi_at);
			replace_function(common_externals.remember_midi_playing_time, remember_playing_time);
			replace_call(ff8_externals.opcode_musicskip + 0x46, ff8_opcode_musicskip_play_midi_at);
			// getmusicoffset opcode is not implemented, but could be used to skip music with musicskip opcode
			replace_call(ff8_externals.opcode_getmusicoffset, ff8_opcode_getmusicoffset);
			/* MIDI segments */
			// Initialization
			replace_call(ff8_externals.opcode_choicemusic + 0x5D, ff8_opcode_choicemusic);
			replace_function(ff8_externals.load_midi_segment, ff8_load_midi_segment);
			replace_function(ff8_externals.play_midi_segments, ff8_play_midi_segments);
			replace_function(ff8_externals.stop_midi_segments, ff8_stop_midi_segments);
			replace_function(ff8_externals.midi_segments_status, ff8_midi_segments_status);
			// Detect solo "play midi"
			replace_call(ff8_externals.load_and_play_midi_segment + 0x41, ff8_load_and_play_midi_segment);
			/* Nullify MIDI subs */
			replace_function(common_externals.midi_init, noop);
			replace_function(ff8_externals.dmusic_segment_connect_to_dls, noop);
			replace_function(common_externals.midi_cleanup, noop);
			replace_call(ff8_externals.sd_music_play + 0x1B1, ff8_stop_wav); // Removing stop midi call
			/* Music channel detection */
			replace_call(ff8_externals.opcode_dualmusic + 0x58, ff8_opcode_dualmusic_play_music);
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
