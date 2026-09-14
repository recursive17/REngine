#include "../recol.hpp"

namespace re {
	ReCol::ReCol() : r(0), b(0), g(0), a(255) {}
	ReCol::ReCol(int red, int green, int blue, int alpha) : r(red), g(green), b(blue), a(alpha) {}
	ReCol::ReCol(uint32_t hex) {
		a = (hex >> 24) & 0xFF;
		r = (hex >> 16) & 0xFF;
		g = (hex >> 8) & 0xFF;
		b = hex & 0xFF;

		if (a == 0) {
			a = 255;
		}
	}

	ReVec4 ReCol::colorToVec4() {
		return ReVec4(
			r / 255.0f,
			g / 255.0f,
			b / 255.0f,
			a / 255.0f
		);
	}

	int ReCol::getRed() {
		return r;
	}

	int ReCol::getGreen() {
		return g;
	}

	int ReCol::getBlue() {
		return b;
	}

	int ReCol::getAlpha() {
		return a;
	}

	float ReCol::getRedf() {
		return r / 255.0f;
	}

	float ReCol::getGreenf() {
		return g / 255.0f;
	}

	float ReCol::getBluef() {
		return b / 255.0f;
	}

	float ReCol::getAlphaf() {
		return a / 255.0f;
	}
}