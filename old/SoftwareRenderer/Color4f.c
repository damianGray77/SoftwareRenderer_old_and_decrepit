#include "stdafx.h"
#include "Color4f.h"

Color4f& Color4f::operator+=(const Color4f& rhs) {
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	a += rhs.a;

	return *this;
}

Color4f& Color4f::operator-=(const Color4f& rhs) {
	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;
	a -= rhs.a;

	return *this;
}

Color4f& Color4f::operator*=(const FLOAT& rhs) {
	r *= rhs;
	g *= rhs;
	b *= rhs;
	a *= rhs;

	return *this;
}

Color4f& Color4f::operator/=(const FLOAT& rhs) {
	r /= rhs;
	g /= rhs;
	b /= rhs;
	a /= rhs;

	return *this;
}

Color4f& operator=(const DWORD& rhs) {
	r = RF(rhs);
	g = GF(rhs);
	b = BF(rhs);
	a = AF(rhs);

	return *this;
};

inline Color4f Color4f::operator+(Color4f lhs, const Color4f& rhs) {
  lhs += rhs;

  return lhs;
};

inline Color4f Color4f::operator-(Color4f lhs, const Color4f& rhs) {
  lhs -= rhs;

  return lhs;
};

inline Color4f Color4f::operator*(Color4f lhs, const FLOAT& rhs) {
  lhs *= rhs;

  return lhs;
};

inline Color4f Color4f::operator*(Color4f lhs, const LONG& rhs) {
  lhs *= (FLOAT)rhs;

  return lhs;
};

inline Color4f Color4f::operator/(Color4f lhs, const FLOAT& rhs) {
  lhs /= rhs;

  return lhs;
};