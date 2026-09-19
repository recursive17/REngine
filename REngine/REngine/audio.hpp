#pragma once
#include <memory>
#include <span>
#include <string>
#include <unordered_map>

struct ma_engine;
struct ma_sound;

namespace re {

	class Sound {
	public:
		bool loadFromFile(ma_engine* engine, const std::string& path);
		bool loadFromMemory(ma_engine* engine, const std::string& name,
			std::span<const unsigned char> fileBytes);

		void play();          
		void stop();          
		void pause();         

		void setVolume(float volume);   
		void setLooping(bool loop);
		bool isPlaying() const;

		Sound();
		~Sound();
		Sound(const Sound&) = delete;              
		Sound& operator=(const Sound&) = delete;   

	private:
		std::unique_ptr<ma_sound>	sound_;
		bool						loaded_		{ false };
	};

	class SoundManager {
	public:
		void init();

		Sound* load(const std::string& name, const std::string& path);
		Sound* loadFromMemory(const std::string& name, std::span<const unsigned char> bytes);
		Sound* get(const std::string& name) const;

		void play(const std::string& name);            
		void setMasterVolume(float volume);

		SoundManager();
		~SoundManager();

	private:
		std::unique_ptr<ma_engine>	engine_;
		bool						initialized_	{ false };

		std::unordered_map<std::string, std::unique_ptr<Sound>> sounds_;
	};
}
