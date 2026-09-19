#include "../audio.hpp"
#include <miniaudio.h>
#include "../utils/log.hpp"
using namespace re;

Sound::Sound() : sound_(std::make_unique<ma_sound>()) {}

bool Sound::loadFromFile(ma_engine* engine, const std::string& path) {
	ma_result result = ma_sound_init_from_file(engine, path.c_str(),
		MA_SOUND_FLAG_DECODE, nullptr, nullptr, sound_.get());

	if (result != MA_SUCCESS) {
		re::ReLog::warning(re::Prefix::AUDIO, "failed to load sound: " + path +
			" (" + ma_result_description(result) + ")");
		return false;
	}
	loaded_ = true;
	return true;
}

bool Sound::loadFromMemory(ma_engine* engine, const std::string& name,
	std::span<const unsigned char> fileBytes) {
	std::string key = "mem:" + name;
	ma_resource_manager* rm = ma_engine_get_resource_manager(engine);

	ma_result result = ma_resource_manager_register_encoded_data(rm, key.c_str(),
		fileBytes.data(), fileBytes.size());

	if (result != MA_SUCCESS) {
		re::ReLog::warning(re::Prefix::AUDIO, "failed to register sound: " + name +
			" (" + ma_result_description(result) + ")");
		return false;
	}
	return loadFromFile(engine, key);
}

void Sound::play() {
	if (!loaded_) return;
	ma_sound_seek_to_pcm_frame(sound_.get(), 0);
	ma_sound_start(sound_.get());
}

void Sound::stop() {
	if (!loaded_) return;
	ma_sound_stop(sound_.get());
	ma_sound_seek_to_pcm_frame(sound_.get(), 0);
}

void Sound::pause() {
	if (!loaded_) return;
	ma_sound_stop(sound_.get());
}

void Sound::setVolume(float volume) {
	if (!loaded_) return;
	ma_sound_set_volume(sound_.get(), volume);
}

void Sound::setLooping(bool loop) {
	if (!loaded_) return;
	ma_sound_set_looping(sound_.get(), loop ? MA_TRUE : MA_FALSE);
}

bool Sound::isPlaying() const {
	return loaded_ && ma_sound_is_playing(sound_.get());
}

Sound::~Sound() {
	if (loaded_) {
		ma_sound_uninit(sound_.get());
	}
}
