#include "../revec.hpp"

namespace re {
    // ReVec2
    ReVec2 ReVec2::operator+(const ReVec2& other) const {
        return ReVec2(x_ + other.x_, y_ + other.y_);
    }

    ReVec2 ReVec2::operator-(const ReVec2& other) const {
        return ReVec2(x_ - other.x_, y_ - other.y_);
    }

    ReVec2 ReVec2::operator*(const ReVec2& other) const {
        return ReVec2(x_ * other.x_, y_ * other.y_);
    }

    // ReVec3 
    ReVec3 ReVec3::operator+(const ReVec3& other) const {
        return ReVec3(x_ + other.x_, y_ + other.y_, z_ + other.z_);
    }

    ReVec3 ReVec3::operator-(const ReVec3& other) const {
        return ReVec3(x_ - other.x_, y_ - other.y_, z_ - other.z_);
    }

    ReVec3 ReVec3::operator*(const ReVec3& other) const {
        return ReVec3(x_ * other.x_, y_ * other.y_, z_ * other.z_);
    }

    // ReVec4 
    ReVec4 ReVec4::operator+(const ReVec4& other) const {
        return ReVec4(x_ + other.x_, y_ + other.y_, z_ + other.z_, w_ + other.w_);
    }

    ReVec4 ReVec4::operator-(const ReVec4& other) const {
        return ReVec4(x_ - other.x_, y_ - other.y_, z_ - other.z_, w_ - other.w_);
    }

    ReVec4 ReVec4::operator*(const ReVec4& other) const {
        return ReVec4(x_ * other.x_, y_ * other.y_, z_ * other.z_, w_ * other.w_);
    }
}