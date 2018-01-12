#ifndef _C_CONTOUR_H_
#define _C_CONTOUR_H_

#include "mydefine.h"

#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

class CContour
{
private:
	vector< Point > m_Points;	//��Ե��ļ���
	Point			m_Center;	//��Ե�������
	EContourType	m_Type;		//��Ե������
	Box				m_Box;		//��Ե����Ӿ���
	size_t			m_Size;		//��Ե����

public:
	
	CContour()
	{
		m_Type = CONTOUR;
		m_Size = 0;
	}
	CContour(const Point & center, const Box & box, const EContourType type = CONTOUR)
		: m_Center(center), m_Box(box)
	{
		m_Type = type;
		m_Size = 0;
	}
	CContour(const vector<Point> & points, const EContourType type = CONTOUR);

	~CContour()
	{
		m_Points.clear();
	}

	inline const vector<Point> & points()const
	{
		return m_Points;
	}
	inline vector<Point> & points()
	{
		return m_Points;
	}
	inline const Point & center() const
	{
		return m_Center;
	}
	inline const EContourType & type() const
	{
		return m_Type;
	}
	inline const Box & box() const
	{
		return m_Box;
	}
	inline const int size()const
	{
		return m_Size;
	}
	inline void setType(const EContourType & type)
	{
		m_Type = type;
	}


	inline const Point & operator[](const int index) const
	{
		return m_Points[index];
	}
	inline Point & operator[](const int index)
	{
		return m_Points[index];
	}

	inline void push_back(const Point & point)
	{
		m_Points.push_back(point);
		m_Size++;
	}
	//ɾ��points��Ա���±�Ϊindex��Ԫ��
	inline void erase(const int index)
	{
		m_Points.erase(m_Points.begin() + index);
		m_Size--;
	}
	//�����Ա
	inline void clear()
	{
		m_Points.clear();
		m_Size = 0;
	}

	bool erase_safe(const int index);
	bool erase_multiple(const int start, const int end);

	//�������г�Ա��ֵ
	bool calculate_members();
};

#endif