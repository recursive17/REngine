#pragma once
#include <iostream>
#include <string>

namespace re {
	enum class Prefix {
		RE,
		RENDER,
		VULKAN
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
			default:             return "UNKNOWN";
			}
		}
	public:
		static void error(Prefix pr, std::exception ex) {
			std::cerr << RED << "[" << prefixToString(pr) << "]" << "Exception-> " << ex.what() << std::endl;
			throw ex;
		}

		static void warning(Prefix pr, std::string message) {
			std::cerr << YELLOW << "[" << prefixToString(pr) << "] Warning-> " << message << std::endl;
		}

		static void success(Prefix pr, std::string message) {
			std::cerr << GREEN << "[" << prefixToString(pr) << "] Success-> " << message << std::endl;
		}
	};

}