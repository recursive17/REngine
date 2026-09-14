#pragma once

namespace re {
	struct ReVec2 {
		float x_;
		float y_;

		ReVec2() : x_(0), y_(0) {}
		ReVec2(float x, float y) : x_(x), y_(y) {}

		ReVec2 operator+(const ReVec2& other) const;
		ReVec2 operator-(const ReVec2& other) const;
		ReVec2 operator*(const ReVec2& other) const;
	};

	struct ReVec3 {
		float x_;
		float y_;
		float z_;

		ReVec3() : x_(0), y_(0), z_(0) {}
		ReVec3(float x, float y, float z) : x_(x), y_(y), z_(z) {}

		ReVec3 operator+(const ReVec3& other) const;
		ReVec3 operator-(const ReVec3& other) const;
		ReVec3 operator*(const ReVec3& other) const;
	};

	struct ReVec4 {
		float x_;
		float y_;
		float z_;
		float w_;

		ReVec4() : x_(0), y_(0), z_(0), w_(0) {}
		ReVec4(float x, float y, float z, float w) : x_(x), y_(y), z_(z), w_(w) {}

		ReVec4 operator+(const ReVec4& other) const;
		ReVec4 operator-(const ReVec4& other) const;
		ReVec4 operator*(const ReVec4& other) const;
	};
}