#ifndef SOFTWARERENDERER_COLOR4F_H
#define SOFTWARERENDERER_COLOR4F_H

struct Color4f {
	FLOAT r, g, b, a;
	
	Color4f() { };
	Color4f(FLOAT r, FLOAT g, FLOAT b, FLOAT a);

	Color4f& operator+=(const Color4f& rhs);
	Color4f& operator-=(const Color4f& rhs);
	Color4f& operator*=(const FLOAT& rhs);
	Color4f& operator/=(const FLOAT& rhs);
	Color4f& operator=(const DWORD& rhs);
};

inline Color4f operator+(Color4f lhs, const Color4f& rhs);
inline Color4f operator-(Color4f lhs, const Color4f& rhs);
inline Color4f operator*(Color4f lhs, const FLOAT& rhs);
inline Color4f operator*(Color4f lhs, const LONG& rhs);
inline Color4f operator/(Color4f lhs, const FLOAT& rhs);

#endif