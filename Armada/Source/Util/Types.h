/*
Author:		Daniel Habig
Date:		11-3-13
File:		Types.h
Purpose:	These classes are various combinations of base types
------------
Classes:
Structs:	Type2<Template>, Type3<Template>, Type4<Template>, _Rect<Template>
Enums:		eDIRECTION
Namespaces:	CLR
*/


/*
NAMING CONVENTION
m_	==	member
v	==	vector or array (all vector or array types should use plurals)
f	==	float
d	==	double
b	==	bool
s	==	signed
u	==	unsigned
ch	==	char
sh	==	short
n	==	int
l	==	long
ll	==	long long
p	==	pointer
c	==	class
t	==	struct
str	==	string
k	==	const
e	==	enum
a	==	template
y	==	threading related (mutex, conditional, thread)
i	==	interface class (abstract base class)
*/


#pragma once
#include <cmath>
//#include <cstdlib>//for rand
//#include <ctime>//for rand seed
//#include <Windows.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifdef _WIN64
//define something for Windows (64-bit)
#elif _WIN32
//define something for Windows (32-bit)
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
// define something for simulator   
#elif TARGET_OS_IPHONE
// define something for iphone  
#else
#define TARGET_OS_OSX 1
// define something for OSX
#endif
#elif __linux
// linux
#elif __unix // all unices not caught above
// Unix
#elif __posix
// POSIX
#endif

namespace Type
{
#pragma region TYPEDEFS
	//Unsigned
	typedef unsigned char		u08;
	typedef unsigned short int	u16;
	typedef unsigned long int	u32;
	typedef unsigned long long	u64;
	//Signed
	typedef signed char			s08;
	typedef signed short int	s16;
	typedef signed long int		s32;
	typedef signed long long	s64;

	//Easier Unsigned Names
	typedef unsigned char		uchar;
	typedef unsigned short int	ushort;
	typedef unsigned long int	ulong;
	typedef unsigned long long	ulonglong;
	//Easier Signed Names
	typedef signed char			schar;
	typedef signed short int	sshort;
	typedef signed long int		slong;
	typedef signed long long	slonglong;

	//Int
	typedef unsigned int	uint;
	typedef signed int		sint;

	//Time
	typedef double			Time;
#pragma endregion TYPEDEFS

#pragma region CONSTS
	const uchar SECS_PER_MINUTE = 60;
	const uchar MINS_PER_HOUR = 60;
	const uchar HOURS_PER_DAY = 24;
	const uchar DAYS_PER_WEEK = 7;
	const uchar WEEKS_PER_YEAR = 52;
#pragma endregion CONSTS

#pragma region ENUMS
	enum eDIRECTION
	{
		UP, RIGHT, DOWN, LEFT, NUM_CARDINAL_DIRECTIONS,
		NORTH = UP, EAST, SOUTH, WEST,
		UP_RIGHT, DOWN_RIGHT, DOWN_LEFT, UP_LEFT, NUM_ORDINAL_DIRECTIONS,
		NE = UP_RIGHT, SE, SW, NW,
	};

#pragma endregion ENUMS

#pragma region FUNCS
	template<typename type>
	inline type clamp(type val, type floor, type ceiling)
	{
		return max(floor, min(val, ceiling));
	}
	template<typename type>
	inline type damp(type val, type diff)
	{
		//Decrease the value
		if (val > NULL)
		{
			val -= diff;
			if (val <= NULL)
				val = NULL;
		}
		else //Increase the value
		{
			val += diff;
			if (val >= NULL)
				val = NULL;
		}
		return val;
	}
	template<typename type>
	inline schar posneg(type val)
	{
		if (val >= type())
			return 1;
		else
			return -1;
	}
	//template<typename type>
	//inline type random(type min, type max)
	//{
	//	if (min > max)
	//	{
	//		type temp = min;
	//		min = max;
	//		max = temp;
	//	}
	//	return min + rand() % (max - min);
	//}
	//float randomF(float min, float max)
	//{
	//	if (min > max)
	//	{
	//		float temp = min;
	//		min = max;
	//		max = temp;
	//	}
	//	float ret = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	//	//return min + rand() % (max-min)
	//}
#pragma endregion FUNCS

#pragma region STRUCTS
	template<typename type>
	struct Type2
	{
	private:	//Data
	public:		//Data
		union	// Used to allow the data in the vector to be accessed in different ways
		{
			//Access the vector as an array
			type ary[2];

			//Access the vector's individual components
			struct //2D Position
			{
				type x;
				type y;
			};
			struct //2D Size
			{
				type w; // width
				type h; // height
			};
			struct //2D Texture
			{
				type u;
				type v;
			};
		};

	private:	//Methods
	public:		//Methods
		Type2()
		{
			x = y = type();
		}
		Type2(const type &_x, const type &_y)
		{
			x = _x; y = _y;
		}

		//Operators
		inline Type2 operator+(const Type2 &v) const
		{
			return Type2(x + v.x, y + v.y);
		}
		inline Type2 operator-(const Type2 &v) const
		{
			return Type2(x - v.x, y - v.y);
		}
		inline Type2 operator*(const Type2 &v) const
		{
			return Type2(x * v.x, y * v.y);
		}
		inline Type2 operator/(const Type2 &v) const
		{
			return Type2(x / v.x, y / v.y);
		}
		//Special Operator
		inline Type2 operator*(type v) const
		{
			return Type2(x * v, y * v);
		}
		inline Type2 operator/(type v) const
		{
			return Type2(x / v, y / v);
		}

		//Comparisons
		inline bool operator==(const Type2 &v) const
		{
			return (x == v.x && y == v.y);
		}
		inline bool operator!=(const Type2 &v) const
		{
			return !this->operator==(v);
		}

		//Assignment
		inline Type2& operator=(const Type2 &v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}
		inline Type2& operator+=(const Type2 &v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}
		inline Type2& operator-=(const Type2 &v)
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}
		inline Type2& operator*=(const Type2 &v)
		{
			x *= v.x;
			y *= v.y;
			return *this;
		}
		inline Type2& operator/=(const Type2 &v)
		{
			x /= v.x;
			y /= v.y;
			return *this;
		}
		//Special Assignment
		inline Type2& operator*=(type v)
		{
			x *= v;
			y *= v;
			return *this;
		}
		inline Type2& operator/=(type v)
		{
			x /= v;
			y /= v;
			return *this;
		}

		//Special Funcs
		inline Type2& makeZero()
		{
			x = y = NULL;
			return *this;
		}
		inline Type2& negate()
		{
			x = -x;
			y = -y;
			return *this;
		}
		inline type dotProduct(const Type2 &v) const
		{
			return x * v.x + y * v.y;
		}
		inline friend type DotProduct(const Type2 &a, const Type2 &b)
		{
			return a.dotProduct(b);
		}
		inline type magnitude() const
		{
			return (type)sqrt(dotProduct(*this));
		}
		inline Type2& normalize()
		{
			float invMag = 1 / magnitude();
			*this *= invMag;
			return *this;
		}
	};
	template<typename type>
	struct Type3
	{
	private:	//Data
	public:		//Data
		union	// Used to allow the data in the vector to be accessed in different ways
		{
			//Access the vector as an array
			type ary[3];

			//Access the vector's individual components
			struct //3D Position
			{
				type x;
				type y;
				type z;
			};
			struct //3D Size
			{
				type w;	// width
				type h; // height
				type d; // depth
			};
		};

	private:	//Methods
	public:		//Methods
		Type3()
		{
			x = y = z = type();
		}
		Type3(const type &_x, const type &_y, const type &_z)
		{
			x = _x; y = _y; z = _z;
		}

		//Operators
		inline Type3 operator+(const Type3 &v) const
		{
			return Type3(x + v.x, y + v.y, z + v.z);
		}
		inline Type3 operator-(const Type3 &v) const
		{
			return Type3(x - v.x, y - v.y, z - v.z);
		}
		inline Type3 operator*(const Type3 &v) const
		{
			return Type3(x * v.x, y * v.y, z * v.z);
		}
		inline Type3 operator/(const Type3 &v) const
		{
			return Type3(x / v.x, y / v.y, z / v.z);
		}
		//Special Operator
		inline Type3 operator*(type v) const
		{
			return Type3(x * v, y * v, z * v);
		}
		inline Type3 operator/(type v) const
		{
			return Type3(x / v, y / v, z / v);
		}

		//Comparisons
		inline bool operator==(const Type3 &v) const
		{
			return (x == v.x && y == v.y && z = v.z);
		}
		inline bool operator!=(const Type3 &v) const
		{
			return !this->operator==(v);
		}

		//Assignment
		inline Type3& operator=(const Type3 &v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}
		inline Type3& operator+=(const Type3 &v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		inline Type3& operator-=(const Type3 &v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		inline Type3& operator*=(const Type3 &v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}
		inline Type3& operator/=(const Type3 &v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
			return *this;
		}
		//Special Assignment
		inline Type3& operator*=(type v)
		{
			x *= v;
			y *= v;
			z *= v;
			return *this;
		}
		inline Type3& operator/=(type v)
		{
			x /= v;
			y /= v;
			z /= v;
			return *this;
		}

		//Special Funcs
		inline Type3& makeZero()
		{
			x = y = z = NULL;
			return *this;
		}
		inline Type3& negate()
		{
			x = -x;
			y = -y;
			z = -z;
			return *this;
		}
		inline type dotProduct(const Type3 &v) const
		{
			return x * v.x + y * v.y + z * v.z
		}
		inline friend type DotProduct(const Type3 &a, const Type3 &b)
		{
			return a.dotProduct(b);
		}
		inline type magnitude() const
		{
			return (float)sqrt(dotProduct(*this));
		}
		inline Type3& normalize()
		{
			float invMag = 1 / magnitude();
			*this *= invMag;
			return *this;
		}
	};
	template<typename type>
	struct Type4
	{
	private:	//Data
	public:		//Data
		union	// Used to allow the data in the vector to be accessed in different ways
		{
			//Access the vector as an array
			type ary[4];

			//Access the vector's individual components
			struct // 3D Position
			{
				type x;
				type y;
				type z;
				type w;
			};
			struct // Color
			{
				type r;
				type g;
				type b;
				type a;
			};
			struct // Color Full
			{
				type red;
				type green;
				type blue;
				type alpha;
			};
		};

	private:	//Methods
	public:		//Methods
		Type4()
		{
			x = y = z = w = type();
		}
		Type4(const type &_x, const type &_y, const type &_z, const type &_w)
		{
			x = _x; y = _y; z = _z; w = _w;
		}

		//Operators
		inline Type4 operator+(const Type4 &v) const
		{
			return Type4(x + v.x, y + v.y, z + v.z, w = v.w);
		}
		inline Type4 operator-(const Type4 &v) const
		{
			return Type4(x - v.x, y - v.y, z - v.z, w - v.w);
		}
		inline Type4 operator*(const Type4 &v) const
		{
			return Type4(x * v.x, y * v.y, z * v.z, w * v.w);
		}
		inline Type4 operator/(const Type4 &v) const
		{
			return Type4(x / v.x, y / v.y, z / v.z.w / v.w);
		}
		//Special Operator
		inline Type4 operator*(type v) const
		{
			return Type4(x * v, y * v, z * v, w * v);
		}
		inline Type4 operator/(type v) const
		{
			return Type4(x / v, y / v, z / v, w / v);
		}

		//Comparisons
		inline bool operator==(const Type4 &v) const
		{
			return (x == v.x && y == v.y && z = v.z && w = v.w);
		}
		inline bool operator!=(const Type4 &v) const
		{
			return !this->operator==(v);
		}

		//Assignment
		inline Type4& operator=(const Type4 &v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
			return *this;
		}
		inline Type4& operator+=(const Type4 &v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;
			return *this;
		}
		inline Type4& operator-=(const Type4 &v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			w -= v.w;
			return *this;
		}
		inline Type4& operator*=(const Type4 &v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			w *= v.w;
			return *this;
		}
		inline Type4& operator/=(const Type4 &v)
		{
			x /= v.x;
			y /= v.y;
			z /= v.z;
			w /= v.w;
			return *this;
		}
		//Special Assignment
		inline Type4& operator*=(type v)
		{
			x *= v;
			y *= v;
			z *= v;
			w *= v;
			return *this;
		}
		inline Type4& operator/=(type v)
		{
			x /= v;
			y /= v;
			z /= v;
			w /= v;
			return *this;
		}

		//Special Funcs
		inline Type4& makeZero()
		{
			x = y = z = w = NULL;
			return *this;
		}
		inline Type4& negate()
		{
			x = -x;
			y = -y;
			z = -z;
			w = -w;
			return *this;
		}
		inline type dotProduct(const Type4 &v) const
		{
			return x * v.x + y * v.y + z * v.z + w * v.w;
		}
		inline friend type DotProduct(const Type4 &a, const Type4 &b)
		{
			return a.dotProduct(b);
		}
		inline type magnitude() const
		{
			return (float)sqrt(dotProduct(*this));
		}
		inline Type4& normalize()
		{
			float invMag = 1 / magnitude();
			*this *= invMag;
			return *this;
		}
	};
	template<typename type>
	struct _RECT
	{
	private:	//Data
	public:		//Data
		union	// Used to allow the data in the vector to be accessed in different ways
		{
			//Access the vector as an array
			type ary[4];

			//Access the vector's individual components
			struct // Position & Dimensions
			{
				type x;
				type y;
				type w;
				type h;
			};
		};

	private:	//Members
	public:		//Members
		_RECT()
		{
			x = y = w = h = type();
		}
		_RECT(const type &_x, const type &_y, const type &_w, const type &_h)
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
		}
		_RECT(const Type2<type> &alpha, const Type2<type> &omega)
		{
			x = min(alpha.x, omega.x);
			y = min(alpha.y, omega.y);
			w = abs(omega.x - alpha.x);
			h = abs(omega.y - alpha.y);
		}
		inline _RECT& operator=(const _RECT &v)
		{
			x = v.x;
			y = v.y;
			w = v.w;
			h = v.h;
			return *this;
		}
		inline Type2<type> GetPos(void) const
		{
			return Type2<type>(this->x, this->y);
		}
		inline Type2<type> GetSize(void) const
		{
			return Type2<type>(this->w, this->h);
		}
	};
	template<typename type>
	struct _COLOR
	{
		// Access the vector as an array
		union
		{
			// Access the vector as an array
			type ary[4];
			// Access the vector's individual components as color values
			struct
			{
				type r;
				type g;
				type b;
				type a;
			};
		};
		_COLOR()
		{
			r = g = b = a = type();
		}
		_COLOR(const type &_r, const type &_g, const type &_b, const type &_a)
		{
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}
		inline _COLOR& operator=(const _COLOR &v)
		{
			r = v.r;
			g = v.g;
			b = v.b;
			a = v.a;
			return *this;
		}
		inline _COLOR& operator*=(const float &f)
		{
			r = clamp<type>(r * f, 0, 255);
			g = clamp<type>(g * f, 0, 255);
			b = clamp<type>(b * f, 0, 255);
			return *this;
		}
		inline _COLOR operator*(const float &f) const
		{
			return _COLOR<type>(type(r * f), type(g * f), type(b * f), a);
		}
	};

	template<typename type>
	inline bool operator==(const _RECT<type>& lhs, const _RECT<type>& rhs)
	{
		for (uchar i = 0; i < 4; i++)
		{
			if (lhs.ary[i] != rhs.ary[i])
				return false;
		}
		return true;
	}
	template<typename type>
	inline bool operator!=(const _RECT<type>& lhs, const _RECT<type>& rhs){ return !operator==(lhs, rhs); }
	template<typename type>
	inline bool operator< (const _RECT<type>& lhs, const _RECT<type>& rhs){ return lhs.x <= rhs.x && lhs.y <= rhs.y; }
	template<typename type>
	inline bool operator> (const _RECT<type>& lhs, const _RECT<type>& rhs){ return  operator< (rhs, lhs); }
	template<typename type>
	inline bool operator<=(const _RECT<type>& lhs, const _RECT<type>& rhs){ return !operator> (lhs, rhs); }
	template<typename type>
	inline bool operator>=(const _RECT<type>& lhs, const _RECT<type>& rhs){ return !operator< (lhs, rhs); }
#pragma endregion STRUCTS
}

// common colors
namespace CLR
{
	using namespace Type;

	//Full
	const _COLOR<uchar> WHITE = _COLOR<uchar>(255, 255, 255, 255);
	const _COLOR<uchar> BLACK = _COLOR<uchar>(0, 0, 0, 255);
	const _COLOR<uchar> CLEAR = _COLOR<uchar>(0, 0, 0, 0);
	//Primary
	const _COLOR<uchar> RED = _COLOR<uchar>(255, 0, 0, 255);
	const _COLOR<uchar> GREEN = _COLOR<uchar>(0, 255, 0, 255);
	const _COLOR<uchar> BLUE = _COLOR<uchar>(0, 0, 255, 255);
	//Secondary
	const _COLOR<uchar> YELLOW = _COLOR<uchar>(255, 255, 0, 255);
	const _COLOR<uchar> MAGENTA = _COLOR<uchar>(255, 0, 255, 255);
	const _COLOR<uchar> CYAN = _COLOR<uchar>(0, 255, 255, 255);
	//Tertiary
	const _COLOR<uchar> AZURE = _COLOR<uchar>(0, 127, 255, 255);
	const _COLOR<uchar> VIOLET = _COLOR<uchar>(143, 0, 255, 255);
	const _COLOR<uchar> ROSE = _COLOR<uchar>(255, 0, 127, 255);
	const _COLOR<uchar> ORANGE = _COLOR<uchar>(255, 165, 0, 255);
	const _COLOR<uchar> CHARTREUSE = _COLOR<uchar>(127, 255, 0, 255);
	const _COLOR<uchar> SPRING_GREEN = _COLOR<uchar>(0, 255, 127, 255);
	//Quaternary

	//Browns

	const _COLOR<uchar> IVORY = _COLOR<uchar>(255, 255, 240, 255);
	const _COLOR<uchar> BEIGE = _COLOR<uchar>(245, 245, 220, 255);
	const _COLOR<uchar> WHEAT = _COLOR<uchar>(245, 222, 179, 255);
	const _COLOR<uchar> TAN = _COLOR<uchar>(210, 180, 140, 255);
	const _COLOR<uchar> KHAKI = _COLOR<uchar>(195, 176, 145, 255);
	const _COLOR<uchar> SILVER = _COLOR<uchar>(192, 192, 192, 255);
	const _COLOR<uchar> CHARCOAL = _COLOR<uchar>(70, 70, 70, 255);
	const _COLOR<uchar> NAVY_BLUE = _COLOR<uchar>(0, 0, 128, 255);
	const _COLOR<uchar> ROYAL_BLUE = _COLOR<uchar>(8, 76, 158, 255);
	const _COLOR<uchar> MEDIUM_BLUE = _COLOR<uchar>(0, 0, 205, 255);
	const _COLOR<uchar> DARK_GREY = _COLOR<uchar>(63, 63, 63, 255);
	const _COLOR<uchar> GREY = _COLOR<uchar>(127, 127, 127, 255);
	const _COLOR<uchar> LIGHT_GREY = _COLOR<uchar>(191, 191, 191, 255);
	const _COLOR<uchar> AQUAMARINE = _COLOR<uchar>(127, 255, 212, 255);
	const _COLOR<uchar> TEAL = _COLOR<uchar>(0, 128, 128, 255);
	const _COLOR<uchar> FOREST_GREEN = _COLOR<uchar>(34, 139, 34, 255);
	const _COLOR<uchar> OLIVE = _COLOR<uchar>(128, 128, 0, 255);
	const _COLOR<uchar> LIME = _COLOR<uchar>(191, 255, 0, 255);
	const _COLOR<uchar> GOLDEN = _COLOR<uchar>(255, 215, 0, 255);
	const _COLOR<uchar> GOLDENROD = _COLOR<uchar>(218, 165, 32, 255);
	const _COLOR<uchar> CORAL = _COLOR<uchar>(255, 127, 80, 255);
	const _COLOR<uchar> SALMON = _COLOR<uchar>(250, 128, 114, 255);
	const _COLOR<uchar> HOT_PINK = _COLOR<uchar>(252, 15, 192, 255);
	const _COLOR<uchar> FUSCHSIA = _COLOR<uchar>(255, 119, 255, 255);
	const _COLOR<uchar> PUCE = _COLOR<uchar>(204, 136, 153, 255);
	const _COLOR<uchar> MAUVE = _COLOR<uchar>(224, 176, 255, 255);
	const _COLOR<uchar> LAVENDER = _COLOR<uchar>(181, 126, 220, 255);
	const _COLOR<uchar> PLUM = _COLOR<uchar>(132, 49, 121, 255);
	const _COLOR<uchar> INDIGO = _COLOR<uchar>(75, 0, 130, 255);
	const _COLOR<uchar> MAROON = _COLOR<uchar>(128, 0, 0, 255);
	const _COLOR<uchar> CRIMSON = _COLOR<uchar>(220, 20, 60, 255);
	const _COLOR<uchar> RUSSET = _COLOR<uchar>(158, 70, 27, 255);
	const _COLOR<uchar> SLATE = _COLOR<uchar>(112, 128, 144, 255);
	const _COLOR<uchar> BUFF = _COLOR<uchar>(240, 220, 130, 255);

};

//Grid With Offsets
namespace GRID
{
	using namespace Type;
	const Type2<slong> kOffset = Type2<slong>(16, 44);
	const Type2<slong> kSpace = Type2<slong>(16, 16);
	const Type2<slong> kSize = Type2<slong>(96, 96);
	const Type2<slong> kGridSize = Type2<slong>(17, 9);
	const _RECT<slong> kGrid[17][9] =
	{
		{ _RECT<slong>(16, 44, 96, 96), _RECT<slong>(16, 156, 96, 96), _RECT<slong>(16, 268, 96, 96), _RECT<slong>(16, 380, 96, 96), _RECT<slong>(16, 492, 96, 96), _RECT<slong>(16, 604, 96, 96), _RECT<slong>(16, 716, 96, 96), _RECT<slong>(16, 828, 96, 96), _RECT<slong>(16, 940, 96, 96) },
		{ _RECT<slong>(128, 44, 96, 96), _RECT<slong>(128, 156, 96, 96), _RECT<slong>(128, 268, 96, 96), _RECT<slong>(128, 380, 96, 96), _RECT<slong>(128, 492, 96, 96), _RECT<slong>(128, 604, 96, 96), _RECT<slong>(128, 716, 96, 96), _RECT<slong>(128, 828, 96, 96), _RECT<slong>(128, 940, 96, 96) },
		{ _RECT<slong>(240, 44, 96, 96), _RECT<slong>(240, 156, 96, 96), _RECT<slong>(240, 268, 96, 96), _RECT<slong>(240, 380, 96, 96), _RECT<slong>(240, 492, 96, 96), _RECT<slong>(240, 604, 96, 96), _RECT<slong>(240, 716, 96, 96), _RECT<slong>(240, 828, 96, 96), _RECT<slong>(240, 940, 96, 96) },
		{ _RECT<slong>(352, 44, 96, 96), _RECT<slong>(352, 156, 96, 96), _RECT<slong>(352, 268, 96, 96), _RECT<slong>(352, 380, 96, 96), _RECT<slong>(352, 492, 96, 96), _RECT<slong>(352, 604, 96, 96), _RECT<slong>(352, 716, 96, 96), _RECT<slong>(352, 828, 96, 96), _RECT<slong>(352, 940, 96, 96) },
		{ _RECT<slong>(464, 44, 96, 96), _RECT<slong>(464, 156, 96, 96), _RECT<slong>(464, 268, 96, 96), _RECT<slong>(464, 380, 96, 96), _RECT<slong>(464, 492, 96, 96), _RECT<slong>(464, 604, 96, 96), _RECT<slong>(464, 716, 96, 96), _RECT<slong>(464, 828, 96, 96), _RECT<slong>(464, 940, 96, 96) },
		{ _RECT<slong>(576, 44, 96, 96), _RECT<slong>(576, 156, 96, 96), _RECT<slong>(576, 268, 96, 96), _RECT<slong>(576, 380, 96, 96), _RECT<slong>(576, 492, 96, 96), _RECT<slong>(576, 604, 96, 96), _RECT<slong>(576, 716, 96, 96), _RECT<slong>(576, 828, 96, 96), _RECT<slong>(576, 940, 96, 96) },
		{ _RECT<slong>(688, 44, 96, 96), _RECT<slong>(688, 156, 96, 96), _RECT<slong>(688, 268, 96, 96), _RECT<slong>(688, 380, 96, 96), _RECT<slong>(688, 492, 96, 96), _RECT<slong>(688, 604, 96, 96), _RECT<slong>(688, 716, 96, 96), _RECT<slong>(688, 828, 96, 96), _RECT<slong>(688, 940, 96, 96) },
		{ _RECT<slong>(800, 44, 96, 96), _RECT<slong>(800, 156, 96, 96), _RECT<slong>(800, 268, 96, 96), _RECT<slong>(800, 380, 96, 96), _RECT<slong>(800, 492, 96, 96), _RECT<slong>(800, 604, 96, 96), _RECT<slong>(800, 716, 96, 96), _RECT<slong>(800, 828, 96, 96), _RECT<slong>(800, 940, 96, 96) },
		{ _RECT<slong>(912, 44, 96, 96), _RECT<slong>(912, 156, 96, 96), _RECT<slong>(912, 268, 96, 96), _RECT<slong>(912, 380, 96, 96), _RECT<slong>(912, 492, 96, 96), _RECT<slong>(912, 604, 96, 96), _RECT<slong>(912, 716, 96, 96), _RECT<slong>(912, 828, 96, 96), _RECT<slong>(912, 940, 96, 96) },
		{ _RECT<slong>(1024, 44, 96, 96), _RECT<slong>(1024, 156, 96, 96), _RECT<slong>(1024, 268, 96, 96), _RECT<slong>(1024, 380, 96, 96), _RECT<slong>(1024, 492, 96, 96), _RECT<slong>(1024, 604, 96, 96), _RECT<slong>(1024, 716, 96, 96), _RECT<slong>(1024, 828, 96, 96), _RECT<slong>(1024, 940, 96, 96) },
		{ _RECT<slong>(1136, 44, 96, 96), _RECT<slong>(1136, 156, 96, 96), _RECT<slong>(1136, 268, 96, 96), _RECT<slong>(1136, 380, 96, 96), _RECT<slong>(1136, 492, 96, 96), _RECT<slong>(1136, 604, 96, 96), _RECT<slong>(1136, 716, 96, 96), _RECT<slong>(1136, 828, 96, 96), _RECT<slong>(1136, 940, 96, 96) },
		{ _RECT<slong>(1248, 44, 96, 96), _RECT<slong>(1248, 156, 96, 96), _RECT<slong>(1248, 268, 96, 96), _RECT<slong>(1248, 380, 96, 96), _RECT<slong>(1248, 492, 96, 96), _RECT<slong>(1248, 604, 96, 96), _RECT<slong>(1248, 716, 96, 96), _RECT<slong>(1248, 828, 96, 96), _RECT<slong>(1248, 940, 96, 96) },
		{ _RECT<slong>(1360, 44, 96, 96), _RECT<slong>(1360, 156, 96, 96), _RECT<slong>(1360, 268, 96, 96), _RECT<slong>(1360, 380, 96, 96), _RECT<slong>(1360, 492, 96, 96), _RECT<slong>(1360, 604, 96, 96), _RECT<slong>(1360, 716, 96, 96), _RECT<slong>(1360, 828, 96, 96), _RECT<slong>(1360, 940, 96, 96) },
		{ _RECT<slong>(1472, 44, 96, 96), _RECT<slong>(1472, 156, 96, 96), _RECT<slong>(1472, 268, 96, 96), _RECT<slong>(1472, 380, 96, 96), _RECT<slong>(1472, 492, 96, 96), _RECT<slong>(1472, 604, 96, 96), _RECT<slong>(1472, 716, 96, 96), _RECT<slong>(1472, 828, 96, 96), _RECT<slong>(1472, 940, 96, 96) },
		{ _RECT<slong>(1584, 44, 96, 96), _RECT<slong>(1584, 156, 96, 96), _RECT<slong>(1584, 268, 96, 96), _RECT<slong>(1584, 380, 96, 96), _RECT<slong>(1584, 492, 96, 96), _RECT<slong>(1584, 604, 96, 96), _RECT<slong>(1584, 716, 96, 96), _RECT<slong>(1584, 828, 96, 96), _RECT<slong>(1584, 940, 96, 96) },
		{ _RECT<slong>(1696, 44, 96, 96), _RECT<slong>(1696, 156, 96, 96), _RECT<slong>(1696, 268, 96, 96), _RECT<slong>(1696, 380, 96, 96), _RECT<slong>(1696, 492, 96, 96), _RECT<slong>(1696, 604, 96, 96), _RECT<slong>(1696, 716, 96, 96), _RECT<slong>(1696, 828, 96, 96), _RECT<slong>(1696, 940, 96, 96) },
		{ _RECT<slong>(1808, 44, 96, 96), _RECT<slong>(1808, 156, 96, 96), _RECT<slong>(1808, 268, 96, 96), _RECT<slong>(1808, 380, 96, 96), _RECT<slong>(1808, 492, 96, 96), _RECT<slong>(1808, 604, 96, 96), _RECT<slong>(1808, 716, 96, 96), _RECT<slong>(1808, 828, 96, 96), _RECT<slong>(1808, 940, 96, 96) }
	};
}