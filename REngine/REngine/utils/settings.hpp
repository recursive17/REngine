#pragma once
#include <string>
#include <cstdint>

namespace re {
	struct Settings {
        uint32_t        windowWidth     {   800     };
        uint32_t        windowHeight    {   600     };
        std::string     windowTitle     { "REngine" };
    };
}