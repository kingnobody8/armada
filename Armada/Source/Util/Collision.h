#pragma once
#include "Types.h"
using namespace Type;
#include <Windows.h>

namespace Collision
{
	enum eAXIS { X_AXIS, Y_AXIS, BOTH_AXIS, NUM_AXIS };

#pragma region STRUCTS
	template<typename type>
	struct tAABB
	{
		Type2<type>		high;
		Type2<type>		low;

		tAABB()
		{
			high = low = Type2<type>(0.0f, 0.0f);
		}
		tAABB(type x1, type x2, type y1, type y2)
		{
			high.x = max(x1, x2);
			high.y = max(y1, y2);
			low.x = min(x1, x2);
			low.y = min(y1, y2);
		}
		tAABB(Type2<type> alpha, Type2<type> omega)
		{
			high.x = max(alpha.x, omega.x);
			high.y = max(alpha.y, omega.y);
			low.x = min(alpha.x, omega.x);
			low.y = min(alpha.y, omega.y);
		}
		tAABB(_RECT<type> rect)
		{
			low.x = rect.x;
			low.y = rect.y;
			high.x = max(0, low.x + rect.w - 1);	//Don't include the top - this way buttons don't overlap
			high.y = max(0, low.y + rect.h - 1);	//Don't include the top - this way buttons do't overlap
		}
	};
	template<typename type>
	struct tCircle
	{
		Type2<type>		center;
		type			radius;
	};
#pragma endregion

#pragma region FUNCS
	template<typename type>
	static type	Distance(Type2<type> alpha, Type2<type> omega)
	{
		Type2<type> zeta = alpha - omega;
		return zeta.magnitude();
	}

	//Returns an AABB at an offset
	template<typename type>
	static tAABB<type>	GetAABBFromOffset(tAABB<type> box, Type2<type> off)
	{
		box.high += off;
		box.low += off;
		return box;
	}
	
	template<typename type>
	static bool Intersect_Type2_AABB(Type2<type> &point, tAABB<type> &box)
	{
		if (point.x < box.low.x ||
			point.y < box.low.y ||
			point.x > box.high.x ||
			point.y > box.high.y)
			return false;
		return true;
	}
	template<typename type>
	static bool Intersect_AABB_AABB(tAABB<type> &alpha, tAABB<type> &omega, tAABB<type> &sect)
	{
		if (alpha.high.x < omega.low.x ||
			alpha.high.y < omega.low.y ||
			alpha.low.x > omega.high.x ||
			alpha.low.y > omega.high.y)
			return false;
		sect = tAABB(max(alpha.low.x, omega.low.x), min(alpha.high.x, omega.high.x), max(alpha.low.y, omega.low.y), min(alpha.high.y, omega.high.y));
		return true;
	}
	template<typename type>
	static bool Intersect_AABB_Circle(tAABB<type> &alpha, tCircle<type> &omega)
	{
		//If the center of the cirle lies inside the rect
		//Or if any of the rect's edges lie inside the circle
		if (Intersect_Float2_AABB(omega.center, alpha) ||
			Distance(omega.center, alpha.low) < omega.radius ||
			Distance(omega.center, alpha.high) < omega.radius ||
			Distance(omega.center, Type2<type>(alpha.low.x, alpha.high.y)) < omega.radius ||
			Distance(omega.center, Type2<type>(alpha.high.x, alpha.low.y)) < omega.radius)
			return true;

		//Test the half points as well
		Type2<float> half = alpha.high - alpha.low;
		half = half / 2.0f;
		if (Distance(omega.center, Type2<type>(alpha.low.x + half.x, alpha.low.y)) < omega.radius ||
			Distance(omega.center, Type2<type>(alpha.low.x, alpha.low.y + half.y)) < omega.radius ||
			Distance(omega.center, Type2<type>(alpha.high.x - half.x, alpha.high.y)) < omega.radius ||
			Distance(omega.center, Type2<type>(alpha.high.x, alpha.high.x - half.y)) < omega.radius)
			return true;

		return false;
	}
	template<typename type>
	static bool Intersect_Circle_Circle(tCircle<type> &alpha, tCircle<type> &omega)
	{
		return Float2_Distance<type>(alpha.center, omega.center) < alpha.radius + omega.radius;
	}
#pragma endregion
}