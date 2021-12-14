/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
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

#include "audio.h"

#include "log.h"
#include "gamehacks.h"

#if defined(__cplusplus)
extern "C" {
#endif

#include <libvgmstream/vgmstream.h>

#if defined(__cplusplus)
}
#endif

NxAudioEngine nxAudioEngine;

// PRIVATE

void NxAudioEngine::loadConfig()
{
	char _fullpath[MAX_PATH];

	for (int idx = NxAudioEngineLayer::NXAUDIOENGINE_SFX; idx <= NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT; idx++)
	{
		NxAudioEngineLayer type = NxAudioEngineLayer(idx);

		switch (type)
		{
		case NxAudioEngineLayer::NXAUDIOENGINE_SFX:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_sfx_path.c_str());
			if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MUSIC:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_music_path.c_str());
			if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_voice_path.c_str());
			if (trace_all || trace_voice) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_ambient_path.c_str());
			if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		}

		try
		{
			nxAudioEngineConfig[type] = toml::parse_file(_fullpath);
		}
		catch (const toml::parse_error &err)
		{
			nxAudioEngineConfig[type] = toml::parse("");
		}
	}
}

template <class T>
bool NxAudioEngine::getFilenameFullPath(char *_out, T _key, NxAudioEngineLayer _type)
{
	std::vector<std::string> extensions;

	switch(_type)
	{
		case NxAudioEngineLayer::NXAUDIOENGINE_SFX:
			extensions = external_sfx_ext;
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MUSIC:
			extensions = external_music_ext;
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			extensions = external_voice_ext;
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT:
			extensions = external_ambient_ext;
			break;
	}

	for (const std::string &extension: extensions) {
		switch (_type)
		{
		case NxAudioEngineLayer::NXAUDIOENGINE_SFX:
			sprintf(_out, "%s/%s/%d.%s", basedir, external_sfx_path.c_str(), (int)_key, extension.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MUSIC:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_music_path.c_str(), (const char*)_key, extension.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_voice_path.c_str(), (const char*)_key, extension.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_ambient_path.c_str(), (const char*)_key, extension.c_str());
			break;
		}

		if (fileExists(_out)) {
			return true;
		}
	}

	return false;
}

bool NxAudioEngine::fileExists(const char* filename)
{
	struct stat dummy;

	bool ret = (stat(filename, &dummy) == 0);

	if (!ret && (trace_all || trace_music || trace_sfx || trace_voice || trace_ambient))
		ffnx_warning("NxAudioEngine::%s: Could not find file %s\n", __func__, filename);

	return ret;
}

// PUBLIC

bool NxAudioEngine::init()
{
	if (_engine.init() == 0)
	{
		_engineInitialized = true;

		// 100 -> LOG_LEVEL_ALL: https://github.com/vgmstream/vgmstream/blob/4cda04d02595b381dc8cf98ec39e771c80987d18/src/util/log.c#L20
		if (trace_all || trace_ambient || trace_sfx || trace_music || trace_voice) vgm_log_set_callback(NULL, 100, 0, NxAudioEngineVgmstreamCallback);

		loadConfig();

		if (!he_bios_path.empty()) {
			if (!Psf::initialize_psx_core(he_bios_path.c_str())) {
				ffnx_error("NxAudioEngine::%s couldn't load %s, please verify 'he_bios_path' or comment it\n", __func__, he_bios_path.c_str());
			}
			else {
				_openpsf_loaded = true;
				ffnx_info("NxAudioEngine::%s OpenPSF music plugin loaded using %s\n", __func__, he_bios_path.c_str());
			}
		}

		for (int channel = 0; channel < _sfxTotalChannels; channel++) _sfxChannels[channel] = NxAudioEngineSFX();

		return true;
	}

	return false;
}

void NxAudioEngine::flush()
{
	_engine.stopAll();

	for (int channel = 0; channel < 2; channel++)
	{
		_musics[channel] = NxAudioEngineMusic();
	}
	_musicStack = std::stack<NxAudioEngineMusic>();

	for (int channel = 0; channel < _sfxTotalChannels; channel++)
	{
		_sfxChannels[channel] = NxAudioEngineSFX();
	}

	_currentVoice = NxAudioEngineVoice();

	_currentAmbient = NxAudioEngineAmbient();
}

void NxAudioEngine::cleanup()
{
	_engine.deinit();
}

// SFX
bool NxAudioEngine::canPlaySFX(int id)
{
	char filename[MAX_PATH];

	return getFilenameFullPath<int>(filename, id, NxAudioEngineLayer::NXAUDIOENGINE_SFX);
}

SoLoud::VGMStream* NxAudioEngine::loadSFX(int id, bool loop)
{
	int _curId = id - 1;

	if (_engineInitialized)
	{
		char filename[MAX_PATH];

		bool exists = getFilenameFullPath<int>(filename, id, NxAudioEngineLayer::NXAUDIOENGINE_SFX);

		if (exists)
		{
			std::string _id = std::to_string(id);
			auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_SFX][_id];

			if (node)
			{
				int shouldLoop = node["loop"].value_or(-1);

				// Force loop if requested in the config
				if (shouldLoop != -1) loop = shouldLoop;
			}

			if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: filename=%s,loop=%d\n", __func__, filename, loop);

			SoLoud::VGMStream* sfx = new SoLoud::VGMStream();

			sfx->setLooping(loop);

			SoLoud::result res = sfx->load(filename);
			if (res != SoLoud::SO_NO_ERROR) {
				ffnx_error("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
				delete sfx;
				return nullptr;
			}

			return sfx;
		}
	}

	return nullptr;
}

void NxAudioEngine::unloadSFX(int id)
{
	if (_sfxEffectsHandler.count(id) > 0)
	{
		if (_sfxEffectsHandler[id] != nullptr)
		{
			delete _sfxEffectsHandler[id];

			_sfxEffectsHandler.erase(id);
		}
	}
}

void NxAudioEngine::unloadSFXChannel(int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: channel=%d\n", __func__, channel);

	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (options->stream != nullptr)
	{
		delete options->stream;

		options->stream = nullptr;
	}
}

bool NxAudioEngine::playSFX(const char* name, int id, int channel, float panning, bool loop)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];
	int _curId = id - 1;

	// If channel is known to be reusable
	if (channel <= _sfxReusableChannels)
	{
		// Stop the current channel is already used and the track to be played is different that the one currently playing
		if (options->stream != nullptr && options->id != id)
		{
			stopSFX(channel);
			unloadSFXChannel(channel);
		}
	}
	// If channel is known to lazy unload what is currently playing, save the handler for later
	else if (std::find(_sfxLazyUnloadChannels.begin(), _sfxLazyUnloadChannels.end(), channel) != _sfxLazyUnloadChannels.end())
	{
		_sfxEffectsHandler[options->game_id] = options->stream;

		// invalidate the old channel stream in order to continue loading this new ID
		options->stream = nullptr;
	}
	// Otherwise just unload the channel and allow to load a new track immediately
	else if (channel <= _sfxTotalChannels)
	{
		unloadSFXChannel(channel);
	}

	auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_SFX][name];
	if (node)
	{
		// Shuffle SFX playback, if any entry found for the current id
		toml::array *shuffleIds = node["shuffle"].as_array();
		if (shuffleIds && !shuffleIds->empty() && shuffleIds->is_homogeneous(toml::node_type::integer))
		{
			auto _newId = shuffleIds->get(getRandomInt(0, shuffleIds->size() - 1));

			_curId = _newId->value_or(id) - 1;
		}

		// Sequentially playback new SFX ids, if any entry found for the current id
		toml::array *sequentialIds = node["sequential"].as_array();
		if (sequentialIds && !sequentialIds->empty() && sequentialIds->is_homogeneous(toml::node_type::integer))
		{
			if (_sfxSequentialIndexes.find(name) == _sfxSequentialIndexes.end() || _sfxSequentialIndexes[name] >= sequentialIds->size())
				_sfxSequentialIndexes[name] = 0;

			auto _newId = sequentialIds->get(_sfxSequentialIndexes[name]);

			_sfxSequentialIndexes[name]++;

			_curId = _newId->value_or(id) - 1;
		}
	}

	// Try to load the new ID if it's not already cached
	if (options->stream == nullptr)
	{
		options->game_id = id;
		options->id = _curId + 1;
		options->stream = loadSFX(options->id, loop);
	}

	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: name=%s,id=%d,channel=%d,panning:%f\n", __func__, name, options->id, channel, panning);

	if (options->stream != nullptr)
	{
		SoLoud::handle _handle = _engine.play(
			*options->stream,
			options->volume * getSFXMasterVolume(),
			panning
		);

		options->handle = _handle;
		options->loop = _engine.getLooping(_handle);

		return true;
	}

	return false;
}

void NxAudioEngine::stopSFX(int channel, double time)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s channel=%d\n", __func__, channel);

	if (time > 0.0)
	{
		_engine.fadeVolume(options->handle, 0.0f, time);
		_engine.scheduleStop(options->handle, time);
	}
	else
	{
		_engine.stop(options->handle);
	}

	options->id = 0;
	options->loop = false;
	options->volume = 1.0f;
}

void NxAudioEngine::pauseSFX(int channel)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s channel=%d\n", __func__, channel);

	_engine.setPause(options->handle, true);
}

void NxAudioEngine::resumeSFX(int channel)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s channel=%d\n", __func__, channel);

	_engine.setPause(options->handle, false);
}

bool NxAudioEngine::isSFXPlaying(int channel)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	return _engine.isValidVoiceHandle(options->handle) && !_engine.getPause(options->handle);
}

float NxAudioEngine::getSFXMasterVolume()
{
	return _sfxMasterVolume < 0.0f ? 1.0f : _sfxMasterVolume;
}

void NxAudioEngine::setSFXMasterVolume(float volume, double time)
{
	_sfxMasterVolume = volume;
}

void NxAudioEngine::setSFXVolume(int channel, float volume, double time)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	options->volume = volume * getSFXMasterVolume();

	if (time > 0.0) {
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeVolume(options->handle, volume, time);
	}
	else {
		_engine.setVolume(options->handle, volume);
	}
}

void NxAudioEngine::setSFXSpeed(int channel, float speed, double time)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (time > 0.0) {
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeRelativePlaySpeed(options->handle, speed, time);
	}
	else {
		_engine.setRelativePlaySpeed(options->handle, speed);
	}
}

void NxAudioEngine::setSFXPanning(int channel, float panning, double time)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (time > 0.0) {
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadePan(options->handle, panning, time);
	}
	else {
		_engine.setPan(options->handle, panning);
	}
}

void NxAudioEngine::setSFXReusableChannels(short num)
{
	_sfxReusableChannels = num;
}

void NxAudioEngine::setSFXTotalChannels(short num)
{
	_sfxTotalChannels = num;
}

void NxAudioEngine::addSFXLazyUnloadChannel(int channel)
{
	_sfxLazyUnloadChannels.push_back(channel);
}

// Music
bool NxAudioEngine::canPlayMusic(const char* name)
{
	char filename[MAX_PATH];

	return getFilenameFullPath<const char*>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_MUSIC);
}

void NxAudioEngine::cleanOldAudioSources()
{
	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %d elements in the list before cleaning\n", __func__, _audioSourcesToDeleteLater.size());

	std::list<NxAudioEngineMusicAudioSource>::iterator it = _audioSourcesToDeleteLater.begin();
	while (it != _audioSourcesToDeleteLater.end()) {
		if (!_engine.isValidVoiceHandle((*it).handle)) {
			delete (*it).audioSource;
			it = _audioSourcesToDeleteLater.erase(it);
		}
		else {
			++it;
		}
	}

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %d elements in the list after cleaning\n", __func__, _audioSourcesToDeleteLater.size());
}

SoLoud::AudioSource* NxAudioEngine::loadMusic(const char* name, bool isFullPath, const char* format, SoLoud::time *length)
{
	SoLoud::AudioSource* music = nullptr;
	char filename[MAX_PATH];
	bool exists = false;

	if (length != nullptr) {
		*length = -1;
	}

	if (isFullPath)
	{
		exists = fileExists(name);
		strcpy(filename, name);
	}

	if (!exists)
	{
		exists = getFilenameFullPath<const char*>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_MUSIC);
	}

	if (exists)
	{
		if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, filename);

		cleanOldAudioSources();

		if (_openpsf_loaded && SoLoud::OpenPsf::is_our_path(filename)) {
			SoLoud::OpenPsf* openpsf = new SoLoud::OpenPsf();
			music = openpsf;

			SoLoud::result res = openpsf->load(filename);
			if (res != SoLoud::SO_NO_ERROR) {
				ffnx_error("NxAudioEngine::%s: Cannot load %s with openpsf ( SoLoud error: %u )\n", __func__, filename, res);
				delete openpsf;
				music = nullptr;
			} else if (length != nullptr) {
				*length = openpsf->getLength();
			}
		}

		if (music == nullptr) {
			SoLoud::VGMStream* vgmstream = new SoLoud::VGMStream();
			music = vgmstream;

			SoLoud::result res = vgmstream->load(filename, format);
			if (res != SoLoud::SO_NO_ERROR) {
				ffnx_error("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
				delete vgmstream;
				music = nullptr;
			} else if (length != nullptr) {
				*length = vgmstream->getLength();
			}
		}
	}

	return music;
}

void NxAudioEngine::overloadPlayArgumentsFromConfig(char* name, uint32_t* id, MusicOptions* MusicOptions)
{
	toml::table config = nxAudioEngineConfig[NXAUDIOENGINE_MUSIC];
	std::optional<SoLoud::time> offset_seconds_opt = config[name]["offset_seconds"].value<SoLoud::time>();
	std::optional<std::string> no_intro_track_opt = config[name]["no_intro_track"].value<std::string>();
	std::optional<SoLoud::time> intro_seconds_opt = config[name]["intro_seconds"].value<SoLoud::time>();

	if (MusicOptions->noIntro) {
		if (no_intro_track_opt.has_value()) {
			std::string no_intro_track = *no_intro_track_opt;
			if (trace_all || trace_music) ffnx_info("%s: replaced by no intro track %s\n", __func__, no_intro_track.c_str());

			if (!no_intro_track.empty()) {
				memcpy(name, no_intro_track.c_str(), no_intro_track.size());
				name[no_intro_track.size()] = '\0';
			}
		}
		else if (intro_seconds_opt.has_value()) {
			MusicOptions->offsetSeconds = *intro_seconds_opt;
		}
		else {
			ffnx_info("%s: cannot play no intro track, please configure it in %s/config.toml\n", __func__, external_music_path.c_str());
		}
	} else if (offset_seconds_opt.has_value()) {
		MusicOptions->offsetSeconds = *offset_seconds_opt;
	} else {
		std::optional<std::string> offset_special_opt = config[name]["offset_seconds"].value<std::string>();

		if (offset_special_opt.has_value() && offset_special_opt->compare("sync") == 0) {
			MusicOptions->sync = true;
		}
	}

	// Name to lower case
	for (int i = 0; name[i]; i++) {
		name[i] = tolower(name[i]);
	}
	// Shuffle Music playback, if any entry found for the current music name
	toml::array* shuffleNames = config[name]["shuffle"].as_array();
	if (shuffleNames && !shuffleNames->empty() && shuffleNames->is_homogeneous(toml::node_type::string)) {
		std::optional<std::string> _newName = shuffleNames->get(getRandomInt(0, shuffleNames->size() - 1))->value<std::string>();
		if (_newName.has_value()) {
			memcpy(name, (*_newName).c_str(), (*_newName).size());
			name[(*_newName).size()] = '\0';

			if (trace_all || trace_music) ffnx_info("%s: replaced by shuffle with %s\n", __func__, (*_newName).c_str());
		}
	}
}

bool NxAudioEngine::playMusic(const char* name, uint32_t id, int channel, MusicOptions options)
{
	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %s (%d) on channel #%d\n", __func__, name, id, channel);

	char overloadedName[MAX_PATH];

	strncpy(overloadedName, name, MAX_PATH);

	if (!options.useNameAsFullPath) {
		overloadPlayArgumentsFromConfig(overloadedName, &id, &options);
	}
	// Same music is already playing on this channel
	if (isMusicPlaying(channel) && currentMusicId(channel) == id) {
		if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %s is already playing on channel %d\n", __func__, overloadedName, channel);

		return false;
	}
	// Same music is paused on this channel or in backup channel
	bool restore = !_musicStack.empty() && _musicStack.top().id == id;
	if (!options.sync && ((isChannelValid(channel) && currentMusicId(channel) == id) || restore)) {
		resumeMusic(channel, options.fadetime == 0.0 ? 1.0 : options.fadetime, restore); // Slight fade

		return true;
	}

	SoLoud::time musicLength = -1;
	SoLoud::AudioSource* audioSource = loadMusic(overloadedName, options.useNameAsFullPath, options.format, &musicLength);

	if (audioSource != nullptr) {
		// Different music is playing on this channel
		if (isChannelValid(channel)) {
			stopMusic(channel, options.fadetime == 0.0 ? 0.2 : options.fadetime);
		}

		NxAudioEngineMusic& music = _musics[channel];

		if (options.targetVolume >= 0.0f) {
			music.wantedMusicVolume = options.targetVolume;
		}
		const float initialVolume = options.fadetime > 0.0 ? 0.0f : music.wantedMusicVolume * _musicMasterVolume;
		music.handle = _engine.playBackground(*audioSource, initialVolume, options.offsetSeconds > 0);
		music.id = id;

		// Keep audioSource pointer somewhere to delete it after musicHandle is stopped
		_audioSourcesToDeleteLater.push_back(NxAudioEngineMusicAudioSource(music.handle, audioSource));

		if (options.offsetSeconds > 0 || (options.sync && options.lastMusicOffset > 0)) {
			if (trace_all || trace_music) ffnx_info("%s: seek to time %fs\n", __func__, options.offsetSeconds);
			SoLoud::time offsetSeconds = options.sync ? options.lastMusicOffset : options.offsetSeconds;
			if (musicLength > 0) {
				offsetSeconds = std::fmod(offsetSeconds, musicLength);
			}
			_engine.seek(music.handle, offsetSeconds);
			resumeMusic(channel, options.fadetime == 0.0 ? 1.0 : options.fadetime); // Slight fade
		}
		else if (options.fadetime > 0.0) {
			setMusicVolume(music.wantedMusicVolume, channel, options.fadetime);
		}

		return true;
	}

	return false;
}

void NxAudioEngine::playSynchronizedMusics(const std::vector<std::string>& names, uint32_t id, MusicOptions options)
{
	const int channel = 0;

	if (_musics[channel].id == id) {
		if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: id %d is already playing\n", __func__, id);
		return; // Already playing
	}

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: id %d\n", __func__, id);

	stopMusic(options.fadetime);

	SoLoud::handle groupHandle = _engine.createVoiceGroup();

	if (groupHandle == 0) {
		ffnx_error("NxAudioEngine::%s: cannot allocate voice group\n", __func__);
		return;
	}

	for (const std::string &name: names) {
		SoLoud::AudioSource* audioSource = loadMusic(name.c_str());

		if (audioSource != nullptr) {
			SoLoud::handle musicHandle = _engine.playBackground(*audioSource, -1.0f, true);
			_engine.addVoiceToGroup(groupHandle, musicHandle);
			// Keep audioSource pointer somewhere to delete it after musicHandle is stopped
			_audioSourcesToDeleteLater.push_back(NxAudioEngineMusicAudioSource(musicHandle, audioSource));
		}
	}

	if (!_engine.isVoiceGroupEmpty(groupHandle)) {
		_musics[channel].handle = groupHandle;
		_musics[channel].id = id;
		// Play synchronously
		_engine.setPause(groupHandle, false);
	}
	else {
		_engine.destroyVoiceGroup(groupHandle);
	}
}

void NxAudioEngine::swapChannels()
{
	NxAudioEngineMusic music1 = _musics[0];
	_musics[0] = _musics[1];
	_musics[1] = music1;
}

void NxAudioEngine::stopMusic(double time)
{
	for (int channel = 0; channel < 2; ++channel) {
		stopMusic(channel, time);
	}
}

void NxAudioEngine::stopMusic(int channel, double time)
{
	NxAudioEngineMusic& music = _musics[channel];

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: channel %d, midi %d, time %f\n", __func__, channel, music.id, time);

	if (time > 0.0)
	{
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeVolume(music.handle, 0.0f, time);
		_engine.scheduleStop(music.handle, time);
		_lastVolumeFadeEndTime = _engine.mStreamTime + time;
	}
	else
	{
		_engine.stop(music.handle);
	}

	if (_engine.isVoiceGroup(music.handle)) {
		_engine.destroyVoiceGroup(music.handle);
	}

	music.invalidate();
}

void NxAudioEngine::pauseMusic(double time)
{
	for (int channel = 0; channel < 2; ++channel) {
		pauseMusic(channel, time);
	}
}

void NxAudioEngine::pauseMusic(int channel, double time, bool backup)
{
	NxAudioEngineMusic& music = _musics[channel];

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: midi %d, time %f\n", __func__, music.id, time);

	if (time > 0.0)
	{
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeVolume(music.handle, 0.0f, time);
		_engine.schedulePause(music.handle, time);
		_lastVolumeFadeEndTime = _engine.mStreamTime + time;
	}
	else
	{
		_engine.setPause(music.handle, true);
	}

	if (backup) {
		backupMusic(channel);
	}
}

void NxAudioEngine::backupMusic(int channelSource)
{
	if (!isChannelValid(channelSource)) {
		return;
	}

	NxAudioEngineMusic& music = _musics[channelSource];

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: backup music %d for later usage\n", __func__, music.id);

	NxAudioEngineMusic backup = NxAudioEngineMusic();
	// Save for later usage
	backup.id = music.id;
	backup.handle = music.handle;

	_musicStack.push(backup);

	// Invalidate the current handle
	music.invalidate();
}

void NxAudioEngine::restoreMusic(int channelDest, double stopTime)
{
	NxAudioEngineMusic& music = _musics[channelDest];

	if (_musicStack.empty()) {
		return;
	}

	stopMusic(channelDest, stopTime);

	const NxAudioEngineMusic &backup = _musicStack.top();

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: restore music %d\n", __func__, backup.id);

	// Restore
	music.id = backup.id;
	music.handle = backup.handle;

	_musicStack.pop();
}

void NxAudioEngine::resumeMusic(double time)
{
	for (int channel = 0; channel < 2; ++channel) {
		resumeMusic(channel, time);
	}
}

void NxAudioEngine::resumeMusic(int channel, double time, bool restore)
{
	if (restore) {
		restoreMusic(channel, time);
	}

	NxAudioEngineMusic& music = _musics[channel];

	time /= gamehacks.getCurrentSpeedhack();

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: midi %d, time %f\n", __func__, music.id, time);

	// Play it again from where it was left off
	if (time > 0.0) {
		_engine.setVolume(music.handle, 0.0);
	}
	resetMusicVolume(channel, time);
	_engine.setPause(music.handle, false);
}

bool NxAudioEngine::isChannelValid(int channel)
{
	const NxAudioEngineMusic& music = _musics[channel];

	return (_engine.isValidVoiceHandle(music.handle)
		|| _engine.isVoiceGroup(music.handle));
}

bool NxAudioEngine::isMusicPlaying(int channel)
{
	const NxAudioEngineMusic& music = _musics[channel];

	return isChannelValid(channel) && !_engine.getPause(music.handle);
}

uint32_t NxAudioEngine::currentMusicId(int channel)
{
	return isMusicPlaying(channel) ? _musics[channel].id : 0;
}

SoLoud::time NxAudioEngine::getMusicOffsetSeconds(int channel)
{
	return _engine.getStreamTime(_musics[channel].handle);
}

void NxAudioEngine::setMusicMasterVolume(float volume, double time)
{
	_previousMusicMasterVolume = _musicMasterVolume;

	_musicMasterVolume = volume;

	resetMusicVolume(time);
}

void NxAudioEngine::restoreMusicMasterVolume(double time)
{
	if (_previousMusicMasterVolume != _musicMasterVolume && _previousMusicMasterVolume >= 0.0f)
	{
		_musicMasterVolume = _previousMusicMasterVolume;

		// Set them equally so if this API is called again, nothing will happen
		_previousMusicMasterVolume = _musicMasterVolume;

		resetMusicVolume(time);
	}
}

float NxAudioEngine::getMusicVolume(int channel)
{
	return _musics[channel].wantedMusicVolume;
}

bool NxAudioEngine::isMusicVolumeFadeFinished()
{
	return _engine.mStreamTime >= _lastVolumeFadeEndTime;
}

float NxAudioEngine::getMusicMasterVolume()
{
	return _musicMasterVolume < 0.0f ? 1.0f : _musicMasterVolume;
}

void NxAudioEngine::setMusicVolume(float volume, int channel, double time)
{
	_musics[channel].wantedMusicVolume = volume;

	resetMusicVolume(channel, time);
}

void NxAudioEngine::resetMusicVolume(double time)
{
	for (int channel = 0; channel < 2; ++channel) {
		resetMusicVolume(channel, time);
	}
}

void NxAudioEngine::resetMusicVolume(int channel, double time)
{
	const NxAudioEngineMusic& music = _musics[channel];
	const float volume = music.wantedMusicVolume * getMusicMasterVolume();

	if (time > 0.0) {
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeVolume(music.handle, volume, time);
		_lastVolumeFadeEndTime = _engine.mStreamTime + time;
	}
	else {
		_engine.setVolume(music.handle, volume);
	}
}

void NxAudioEngine::setMusicSpeed(float speed, int channel)
{
	_engine.setRelativePlaySpeed(_musics[channel].handle, speed);
}

void NxAudioEngine::setMusicLooping(bool looping, int channel)
{
	_engine.setLooping(_musics[channel].handle, looping);
}

// Voice
bool NxAudioEngine::canPlayVoice(const char* name)
{
	char filename[MAX_PATH];

	return getFilenameFullPath<const char*>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
}

bool NxAudioEngine::playVoice(const char* name, float volume)
{
	char filename[MAX_PATH];

	bool exists = getFilenameFullPath<const char *>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_VOICE);

	_currentVoice.volume = volume;

	auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_VOICE][name];
	if (node)
	{
		// Set volume for the current track
		toml::node *trackVolume = node["volume"].as_integer();
		if (trackVolume)
		{
			_currentVoice.volume = trackVolume->value_or(100) / 100.0f;
		}
	}

	if (trace_all || trace_voice) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, filename);

	// Stop any previously playing voice
	if (_engine.isValidVoiceHandle(_currentVoice.handle))
	{
		_engine.stop(_currentVoice.handle);

		delete _currentVoice.stream;

		_currentVoice.handle = NXAUDIOENGINE_INVALID_HANDLE;
	}

	if (exists)
	{
		_currentVoice.stream = new SoLoud::VGMStream();

		SoLoud::result res = _currentVoice.stream->load(filename);
		if (res != SoLoud::SO_NO_ERROR) {
			ffnx_error("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
			delete _currentVoice.stream;
			return false;
		}

		_currentVoice.handle = _engine.play(*_currentVoice.stream, _currentVoice.volume);

		return _engine.isValidVoiceHandle(_currentVoice.handle);
	}
	else
		return false;
}

void NxAudioEngine::stopVoice(double time)
{
	if (time > 0.0)
	{
		_engine.fadeVolume(_currentVoice.handle, 0, time);
		_engine.scheduleStop(_currentVoice.handle, time);
	}
	else
	{
		_engine.stop(_currentVoice.handle);
	}
}

bool NxAudioEngine::isVoicePlaying()
{
	return _engine.isValidVoiceHandle(_currentVoice.handle) && !_engine.getPause(_currentVoice.handle);
}

// Ambient
bool NxAudioEngine::canPlayAmbient(const char* name)
{
	char filename[MAX_PATH];

	return getFilenameFullPath<const char*>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
}

bool NxAudioEngine::playAmbient(const char* name, float volume, double time)
{
	char filename[MAX_PATH];
	bool exists = false;

	// Reset state
	_currentAmbient.fade_in = 0.0f;
	_currentAmbient.fade_out = 0.0f;
	_currentAmbient.volume = 1.0f;

	auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT][name];
	if (node)
	{
		// Shuffle SFX playback, if any entry found for the current id
		toml::array *shuffleIds = node["shuffle"].as_array();
		if (shuffleIds && !shuffleIds->empty() && shuffleIds->is_homogeneous(toml::node_type::string))
		{
			auto _newName = shuffleIds->get(getRandomInt(0, shuffleIds->size() - 1));

			exists = getFilenameFullPath<const char *>(filename, _newName->value_or(""), NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
		}

		// Sequentially playback new SFX ids, if any entry found for the current id
		toml::array *sequentialIds = node["sequential"].as_array();
		if (sequentialIds && !sequentialIds->empty() && sequentialIds->is_homogeneous(toml::node_type::string))
		{
			// If the key doesn't exist already, add it
			if (_ambientSequentialIndexes.count(name) == 0) _ambientSequentialIndexes[name] = NULL;

			if (_ambientSequentialIndexes.find(name) == _ambientSequentialIndexes.end() || _ambientSequentialIndexes[name] >= sequentialIds->size())
				_ambientSequentialIndexes[name] = 0;

			auto _newName = sequentialIds->get(_ambientSequentialIndexes[name]);

			_ambientSequentialIndexes[name]++;

			exists = getFilenameFullPath<const char *>(filename, _newName->value_or(""), NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
		}

		// Fade In time for this track, if configured
		toml::node *fadeInTime = node["fade_in"].as_floating_point();
		if (fadeInTime)
		{
			_currentAmbient.fade_in = fadeInTime->value_or(0.0f);

			time = _currentAmbient.fade_in;
		}

		// Fade Out time for this track, if configured
		toml::node *fadeOutTime = node["fade_out"].as_floating_point();
		if (fadeOutTime)
		{
			_currentAmbient.fade_out = fadeOutTime->value_or(0.0f);
		}
	}
	else
		exists = getFilenameFullPath<const char *>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);

	if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, filename);

	// Stop any previously playing ambient
	if (_engine.isValidVoiceHandle(_currentAmbient.handle))
	{
		_engine.stop(_currentAmbient.handle);

		delete _currentAmbient.stream;

		_currentAmbient.handle = NXAUDIOENGINE_INVALID_HANDLE;
	}

	if (exists)
	{
		_currentAmbient.volume = volume;

		_currentAmbient.stream = new SoLoud::VGMStream();

		SoLoud::result res = _currentAmbient.stream->load(filename);
		if (res != SoLoud::SO_NO_ERROR) {
			ffnx_error("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
			delete _currentAmbient.stream;
			return false;
		}

		_currentAmbient.handle = _engine.play(*_currentAmbient.stream, time > 0.0f ? 0.0f : volume, 0.0f, time > 0.0f);

		if (time > 0.0f) resumeAmbient(time);

		return _engine.isValidVoiceHandle(_currentAmbient.handle);
	}
	else
		return false;
}

void NxAudioEngine::stopAmbient(double time)
{
	if (_currentAmbient.fade_out > 0.0f)
	{
		time = _currentAmbient.fade_out;

		if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f ( overridden through config.toml )\n", __func__, time);
	}
	else if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f\n", __func__, time);

	if (time > 0.0)
	{
		_engine.fadeVolume(_currentAmbient.handle, 0, time);
		_engine.scheduleStop(_currentAmbient.handle, time);
	}
	else
	{
		_engine.stop(_currentAmbient.handle);
	}
}

void NxAudioEngine::pauseAmbient(double time)
{
	if (_currentAmbient.fade_out > 0.0f)
	{
		time = _currentAmbient.fade_out;

		if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f ( overridden through config.toml )\n", __func__, time);
	}
	else if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f\n", __func__, time);

	if (time > 0.0)
	{
		_engine.fadeVolume(_currentAmbient.handle, 0, time);
		_engine.schedulePause(_currentAmbient.handle, time);
	}
	else
	{
		_engine.setPause(_currentAmbient.handle, true);
	}
}

void NxAudioEngine::resumeAmbient(double time)
{
	if (_currentAmbient.fade_in > 0.0f)
	{
		time = _currentAmbient.fade_in;

		if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f ( overridden through config.toml )\n", __func__, time);
	}
	else if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f\n", __func__, time);

	if (time > 0.0)
	{
		_engine.setPause(_currentAmbient.handle, false);
		_engine.fadeVolume(_currentAmbient.handle, _currentAmbient.volume, time);
	}
	else
	{
		_engine.setVolume(_currentAmbient.handle, _currentAmbient.volume);
		_engine.setPause(_currentAmbient.handle, false);
	}
}

bool NxAudioEngine::isAmbientPlaying()
{
	return _engine.isValidVoiceHandle(_currentAmbient.handle) && !_engine.getPause(_currentAmbient.handle);
}
