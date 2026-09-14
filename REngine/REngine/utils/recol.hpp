#pragma once
#include "revec.hpp"
#include <cstdint>

namespace re {
	struct ReCol {
	private:
		int r, g, b, a;
	public:
		ReCol();
		ReCol(int red, int green, int blue, int alpha);
		ReCol(uint32_t hex);

		ReVec4 colorToVec4();

		int getRed();
		int getGreen();
		int getBlue();
		int getAlpha();

		float getRedf();
		float getGreenf();
		float getBluef();
		float getAlphaf();
	};
}