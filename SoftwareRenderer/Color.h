#ifndef SOFTWARERENDERER_COLOR_H
#define SOFTWARERENDERER_COLOR_H

struct Color4b {
	BYTE r, g, b, a;
};

struct Color4f {
	FLOAT r, g, b, a;

	Color4f& operator+=(const Color4f& rhs) {
		r += rhs.r;
		g += rhs.g;
		b += rhs.b;
		a += rhs.a;

		return *this;
	}

	Color4f& operator-=(const Color4f& rhs) {
		r -= rhs.r;
		g -= rhs.g;
		b -= rhs.b;
		a -= rhs.a;

		return *this;
	}

	Color4f& operator*=(const FLOAT& rhs) {
		r *= rhs;
		g *= rhs;
		b *= rhs;
		a *= rhs;

		return *this;
	}

	Color4f& operator/=(const FLOAT& rhs) {
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
	}
};

inline Color4f operator+(Color4f lhs, const Color4f& rhs) {
  lhs += rhs;

  return lhs;
};

inline Color4f operator-(Color4f lhs, const Color4f& rhs) {
  lhs -= rhs;

  return lhs;
};

inline Color4f operator*(Color4f lhs, const FLOAT& rhs) {
  lhs *= rhs;

  return lhs;
};

inline Color4f operator*(Color4f lhs, const LONG& rhs) {
  lhs *= (FLOAT)rhs;

  return lhs;
};

inline Color4f operator/(Color4f lhs, const FLOAT& rhs) {
  lhs /= rhs;

  return lhs;
};

DWORD RGBAColor4f(const Color4f& c);
DWORD RGBA4f(const FLOAT& r, const FLOAT& g, const FLOAT& b, const FLOAT& a);
DWORD RGBAf(const FLOAT& c);

inline DWORD RGBAColor4f(const Color4f& c) {
	DWORD dr = (DWORD)(c.r * 255);
	DWORD dg = (DWORD)(c.g * 255);
	DWORD db = (DWORD)(c.b * 255);
	DWORD da = (DWORD)(c.a * 255);
	return da << 24 | dr << 16 | dg << 8 | db;
};

inline DWORD RGBA4f(const FLOAT& r, const FLOAT& g, const FLOAT& b, const FLOAT& a) {
	DWORD dr = (DWORD)(r * 255);
	DWORD dg = (DWORD)(g * 255);
	DWORD db = (DWORD)(b * 255);
	DWORD da = (DWORD)(a * 255);
	return da << 24 | dr << 16 | dg << 8 | db;
};

inline DWORD RGBAf(const FLOAT& c) {
	DWORD dr = (DWORD)(c * 255);
	DWORD dg = (DWORD)(c * 255);
	DWORD db = (DWORD)(c * 255);
	DWORD da = (DWORD)(255);
	return da << 24 | dr << 16 | dg << 8 | db;
};

#endif