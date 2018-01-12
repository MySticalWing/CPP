#ifndef _MY_DEFINE_H_
#define _MY_DEFINE_H_

#include <iostream>
#include <math.h>

////********************�궨��********************////
#define SUSAN_TEMPLATE_SIZE  37		//SUSAN����ģ���С
#ifndef MY_ZERO 
#define MY_ZERO 0.0000001
#endif

#define MY_BLUE   Vector3(255, 0, 0)
#define MY_GREEN  Vector3(0, 255, 0)
#define MY_RED    Vector3(0, 0, 255)
#define MY_PURPLE Vector3(128, 0, 128)
#define MY_WHITE  Vector3(255, 255, 255)

typedef unsigned char uchar;

////********************ö�ٶ���*******************////
enum EContourType
{
	COIN = 0,			//Ӳ��
	NUT = 1,				//��ĸ
	HEX_KEY = 2,			//�����ǰ���
	SCREW = 3,				//�ݶ�
	CONTOUR = 4,			//��Ե�㣬���Ͳ�ȷ��
};

////*******************�ṹ�嶨��*******************////

typedef struct _Point
{
	int x;
	int y;

public:
	_Point(int _x = -1, int _y = -1) :x(_x), y(_y)
	{
		;
	}

	inline bool operator==(const _Point & point) const
	{
		return (point.x == x) && (point.y == y);
	}
	inline _Point & operator=(const _Point &point)
	{
		x = point.x;
		y = point.y;

		return *this;
	}
	inline _Point operator-(const _Point & point) const
	{
		return Point(x - point.x, y - point.y);
	}
	inline _Point absDiff(const _Point & point) const
	{
		return Point(abs(point.x - x), abs(point.y - y));
	}
	inline bool operator<(const _Point & point) const
	{
		return (x < point.x) && (y < point.y);
	}
	inline bool operator>(const _Point & point) const
	{
		return (x > point.x) && (y > point.y);
	}
	inline int operator*(const _Point & point) const
	{
		return point.x*x + point.y*y;
	}
	inline double norm()const
	{
		return sqrt(x*x + y*y);
	}
	friend std::ostream & operator<<(std::ostream & out, const _Point & point);
}Point;

inline std::ostream & operator<<(std::ostream & out, const _Point & point)
{
	out << point.x << ", " << point.y;
	return out;
}

typedef struct _Box
{
	Point vertex[4];	//����
	int width;
	int height;
public:
	_Box()
	{
		width = height = 0;
	}
	_Box(const int minx, const int miny, const int maxx, const int maxy)
	{
		vertex[0].x = minx;
		vertex[0].y = miny;
		vertex[1].x = maxx;
		vertex[1].y = miny;
		vertex[2].x = maxx;
		vertex[2].y = maxy;
		vertex[3].x = minx;
		vertex[3].y = maxy;
		width = maxx - minx;
		height = maxy - miny;
	}
	_Box(const Point & pt0, const Point & pt1, const Point & pt2, const Point & pt3)
	{
		vertex[0] = pt0;
		vertex[1] = pt1;
		vertex[2] = pt2;
		vertex[3] = pt3;
		width = vertex[1].x - vertex[0].x;
		height = vertex[3].y - vertex[0].y;
	}

	inline Point & operator[](const int index)
	{
		return vertex[index];
	}
	inline const Point & operator[](const int index) const
	{
		return vertex[index];
	}
	int scale()
	{
		return width * height;
	}
}Box;

typedef struct _Vector3
{
	uchar v[3];

public:
	_Vector3(const uchar v0 = 0, const uchar v1 = 0, const uchar v2 = 0)
	{
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
	}

	_Vector3(const uchar _v[3])
	{
		v[0] = _v[0];
		v[1] = _v[1];
		v[2] = _v[2];
	}

	_Vector3 & operator=(const _Vector3 & right)
	{
		v[0] = right.v[0];
		v[1] = right.v[1];
		v[2] = right.v[2];

		return *this;
	}
	uchar operator[](const int index) const
	{
		return v[index];
	}

}Vector3;

#endif