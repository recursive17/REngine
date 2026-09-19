#include "../audio.hpp"
#include <miniaudio.h>
#include "../utils/log.hpp"
using namespace re;

SoundManager::SoundManager() : engine_(std::make_unique<ma_engine>()) {}

void SoundManager::init() {
	ma_result result = ma_engine_init(nullptr, engine_.get());
	if (result != MA_SUCCESS) {
		re::ReLog::warning(re::Prefix::AUDIO, std::string("failed to init audio engine (") +
			ma_result_description(result) + "), sound is disabled");
		return;
	}
	initialized_ = true;
	re::ReLog::success(re::Prefix::AUDIO, "sound manager is created");
}

Sound* SoundManager::load(const std::string& name, const std::string& path) {
	if (!initialized_) return nullptr;
	if (auto it = sounds_.find(name); it != sounds_.end()) {
		return it->second.get();
	}

	auto sound = std::make_unique<Sound>();
	if (!sound->loadFromFile(engine_.get(), path)) {
		return nullptr;
	}
	Sound* raw = sound.get();
	sounds_[name] = std::move(sound);
	re::ReLog::success(re::Prefix::AUDIO, "sound loaded: " + name);
	return raw;
}

Sound* SoundManager::loadFromMemory(const std::string& name, std::span<const unsigned char> bytes) {
	if (!initialized_) return nullptr;
	if (auto it = sounds_.find(name); it != sounds_.end()) {
		return it->second.get();
	}

	auto sound = std::make_unique<Sound>();
	if (!sound->loadFromMemory(engine_.get(), name, bytes)) {
		return nullptr;
	}
	Sound* raw = sound.get();
	sounds_[name] = std::move(sound);
	re::ReLog::success(re::Prefix::AUDIO, "sound loaded: " + name);
	return raw;
}

Sound* SoundManager::get(const std::string& name) const {
	auto it = sounds_.find(name);
	return it != sounds_.end() ? it->second.get() : nullptr;
}

void SoundManager::play(const std::string& name) {
	if (Sound* s = get(name)) {
		s->play();
	}
	else {
		re::ReLog::warning(re::Prefix::AUDIO, "sound not found: " + name);
	}
}

void SoundManager::setMasterVolume(float volume) {
	if (!initialized_) return;
	ma_engine_set_volume(engine_.get(), volume);
}

SoundManager::~SoundManager() {
	sounds_.clear();   
	if (initialized_) {
		ma_engine_uninit(engine_.get());
	}
}
