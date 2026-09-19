#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

namespace re {
	enum class Prefix {
		RE,
		RENDER,
		VULKAN,
		AUDIO
	};

	class ReLog {
	private:
		inline static const std::string RESET = "\033[0m";
		inline static const std::string RED = "\033[31m";
		inline static const std::string GREEN = "\033[32m";
		inline static const std::string YELLOW = "\033[33m";

		static std::string prefixToString(Prefix pr) {
			switch (pr) {
			case Prefix::RE:     return "RE";
			case Prefix::RENDER: return "RENDER";
			case Prefix::VULKAN: return "VULKAN";
			case Prefix::AUDIO:  return "AUDIO";
			default:             return "UNKNOWN";
			}
		}
	public:
		static void error(Prefix pr, std::string message) {
			std::cerr << RED << "[" << prefixToString(pr) << "][Error] " << message << RESET << std::endl;
			throw std::runtime_error(message);
		}

		static void report(Prefix pr, std::string message) {
			std::cerr << RED << "[" << prefixToString(pr) << "][Error] " << message << RESET << std::endl;
		}

		static void info(Prefix pr, std::string message) {
			std::cerr << "[" << prefixToString(pr) << "][Info] " << message << RESET << std::endl;
		}

		static void warning(Prefix pr, std::string message) {
			std::cerr << YELLOW << "[" << prefixToString(pr) << "][Warning] " << message << RESET << std::endl;
		}

		static void success(Prefix pr, std::string message) {
			std::cerr << GREEN << "[" << prefixToString(pr) << "][Success] " << message << RESET << std::endl;
		}
	};

}