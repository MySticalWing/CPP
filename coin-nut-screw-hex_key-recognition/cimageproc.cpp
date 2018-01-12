#include "cimageproc.h"
#include "mydefine.h"
#include "mydebug.h"

#include <iostream>
#include <algorithm>
#include <stack>
#include <math.h>
#include <time.h>
#include <float.h>
#include <vector>

#include <highgui.h>

using namespace std;

#define DEBUG false

////******�ྲ̬��Ա��ʼ��********////
int CImageProc::dx[9] = { 1,  1,  0, -1, -1, -1, 0, 1, 0 };
int CImageProc::dy[9] = { 0, -1, -1, -1,  0,  1, 1, 1, 0 };

Vector3 CImageProc::COLOR_TABLE[MAX_CLASS_COUNT] =
{
	Vector3(0, 0, 255),	//Ӳ�� ��ɫ
	Vector3(0, 255, 0),	//��ĸ ��ɫ
	Vector3(255, 0, 0),	//L�Ͱ��� ��ɫ
	Vector3(128, 0, 128),	//�ݶ� ��ɫ
	Vector3(255, 255, 255)	//���� ��ɫ
};

////************************************************���졢������������*************************************************////
CImageProc::CImageProc(const int width, const int height, const int channels, const uchar * imageData)
	:width(width), height(height)
{
	m_Bin = new uchar[width*height];
	if (channels == 1)
	{
		otsuCut(imageData);
	}
	else if (channels == 3)
	{
		uchar *_gray = new uchar[width*height];
		cvtRGB2Gray(imageData, _gray);
#if DEBUG
		imshow("gray", width, height, 1, _gray);
#endif
		otsuCut(_gray);
		delete[] _gray;
	}

	erode();
	dilate();
#if DEBUG
	imshow("bin", width, height, 1, m_Bin);
#endif
	initMembers();
}
CImageProc::~CImageProc()
{
	delete[] m_Bin;
}


////********************************************��ʼ����������*************************************************////
void CImageProc::cvtRGB2Gray(const uchar * color, uchar * gray) const
{
	size_t i, j;
	const uchar *pColor = NULL;
	uchar * pGray = NULL;
	int widthStep = width * 3;
	for (i = 0; i < height; i++)
	{
		pColor = color + i*widthStep;
		pGray = gray + i*width;
		for (j = 0; j < width; j++)
		{
			pGray[j] = (pColor[3 * j] * 19595 + pColor[3 * j + 1] * 38469 + pColor[3 * j + 2] * 7472) >> 16;
		}
	}
}

void CImageProc::otsuCut(const uchar * data)
{
	int t = getOtshThrshold(data);
	size_t i, j;
	const uchar *pSrc = NULL;
	uchar *pBin = NULL;
	for (i = 0; i < height; i++)
	{
		pSrc = data + i*width;
		pBin = m_Bin + i*width;
		for (j = 0; j < width; j++)
		{
			pBin[j] = pSrc[j]>t ? 255 : 0;
		}
	}
}

int CImageProc::getOtshThrshold(const uchar * data) const
{
	double hist[256] = { 0.0 };
	normalizedHist(data, hist);

	double omega[256];
	double mu[256];
	omega[0] = hist[0];
	mu[0] = 0;
	int i;
	for (i = 1; i < 256; i++)
	{
		omega[i] = omega[i - 1] + hist[i]; //�ۻ��ֲ�����
		mu[i] = mu[i - 1] + i * hist[i];
	}
	double mean = mu[255];// �Ҷ�ƽ��ֵ
	double max = 0;
	double PA, PB, MA, MB, value;
	int k_max = 0;
	for (i = 1; i < 255; i++)
	{
		PA = omega[i]; // A����ռ�ı���
		PB = 1 - omega[i]; //B����ռ�ı���
		if (fabs(PA) > 0.001 && fabs(PB) > 0.001)
		{
			MA = mu[i] / PA; //A ��ĻҶȾ�ֵ
			MB = (mean - mu[i]) / PB;//B��ҶȾ�ֵ
			value = PA * (MA - mean) * (MA - mean) + PB * (MB - mean) * (MB - mean);//��䷽��
			if (value > max)
			{
				max = value;
				k_max = i;
			}
		}
	}

	return k_max;
}

bool CImageProc::normalizedHist(const uchar * data, double hist[256]) const
{
	size_t i, j;
	const uchar *p = NULL;
	for (i = 0; i < height; i++)
	{
		p = data + i*width;
		for (j = 0; j < width; j++)
		{
			hist[p[j]]++;
		}
	}
	int N = height * width;
	for (i = 0; i < 256; i++)
	{
		hist[i] = hist[i] / N;
	}
	return true;
}

void CImageProc::erode()
{
	uchar *_temp = new uchar[width*height];
	::memcpy(_temp, m_Bin, width*height*sizeof(uchar));
	int x, y, k;
	uchar *p = NULL;
	uchar *pCur = NULL;
	for (y = 1; y < height - 1; y++)
	{
		pCur = m_Bin + y*width;
		p = _temp + y*width;
		for (x = 1; x < width - 1; x++)
		{
			//����������Ĵ���
			if (pCur[x])
			{
				continue;
			}
			//��ǰ����8-�����ڴ��ڱ����㣬�򽫸�ǰ������Ϊ������
			for (k = 0; k < 8; k++)
			{
				if (m_Bin[x + dx[k] + (y + dy[k])*width])
				{
					p[x] = 255;
				}
			}
		}
	}
	::memcpy(m_Bin, _temp, width*height*sizeof(uchar));

	delete[] _temp;
}

void CImageProc::dilate(const int times)
{
	uchar *_temp = new uchar[width*height];
	int x, y, i, k, accum;
	::memcpy(_temp, m_Bin, width*height*sizeof(uchar));
	uchar *pCur = NULL;
	uchar *p = NULL;
	for (i = 0; i < times; i++)
	{
		for (y = 1; y < height - 1; y++)
		{
			pCur = m_Bin + y*width;
			p = _temp + y*width;
			for (x = 1; x < width - 1; x++)
			{
				//����ǰ����
				if (pCur[x] == 0)
				{
					continue;
				}
				accum = 0;
				for (k = 0; k < 8; k++)
				{
					if (m_Bin[x + dx[k] + (y + dy[k])*width] == 0)
					{
						if (++accum >= 4)
						{
							p[x] = 0;
							break;
						}
					}
				}
			}
		}
		::memcpy(m_Bin, _temp, width*height*sizeof(uchar));
	} // for (i = 0; i < times; i++)

	delete[] _temp;
}

void CImageProc::initMembers()
{
#if DEBUG
	IplImage *rawContours = cvCreateImage(cvSize(width, height), 8, 3);
#endif
	findContours();
	m_ContourCount = m_Contours.size();
#if DEBUG
	imshow("rawContouors", rawContours);
#endif
}

bool CImageProc::findContours(int threshold)
{
	bool *_grad = new bool[width*height];
	bool *p = NULL;
	uchar *pCur = NULL;
	uchar *pLow = NULL;
	uchar * pUp = NULL;
	size_t x, y;
	//ɨ��ͼ����ͼ����ݶȷ�ֵ�Ķ�ֵͼ
	for (y = 1; y < height - 1; y++)
	{
		pUp = m_Bin + width*(y - 1);
		pCur = m_Bin + width*y;
		pLow = m_Bin + width*(y + 1);
		p = _grad + y*width;
		for (x = 1; x < width - 1; x++)
		{
			//����ǰ����
			if (pCur[x] > 0)
			{
				p[x] = false;
				continue;
			}
			//�������ص��4-�����ڴ�������ֵ��ͬ�����㣬������ص�һ��Ϊ��Ե��
			p[x] = ((pCur[x - 1] != pCur[x + 1]) || (pUp[x] != pLow[x]));
		}
	}

	stack<Point> seeds;
	Point seed;
	CContour contour;
	size_t k, curx, cury;
	//�����ݶ�ͼ��ֵͼ��ͨ��������������ȡ�պϱ�Ե
	for (y = 1; y < height - 1; y++)
	{
		p = _grad + y*width;
		for (x = 1; x < width - 1; x++)
		{
			//�ҵ�һ����Ե���ӵ㣬��ջ����
			if (p[x])
			{
				contour.clear();
				seeds.push(Point(x, y));
				_grad[x] = false; //���ݶ�ͼ��ɾ����Ե��
				//�Ե�ǰ��Ϊ���ӵ㣬����������������ȡ�����պϱ�Ե
				while (!seeds.empty())
				{
					seed = seeds.top();
					seeds.pop();
					//�����Ե��
					contour.push_back(seed);
					//�������ӵ�8-����
					for (k = 0; k < 8; k++)
					{
						curx = seed.x + dx[k];
						cury = seed.y + dy[k];
						if (curx >= width || curx < 0 || cury >= height || cury < 0)
						{
							continue;
						}
						if (_grad[cury*width + curx])
						{
							seeds.push(Point(curx, cury));	//�����ӵ���ջ����
							_grad[cury*width + curx] = false;	    //���ݶ�ͼ��ɾȥ���ӵ�
						}
					}
				} // while (!seeds.empty())
				//ֻ�����ϳ��ı�Ե
				if (contour.size() > threshold)
				{
					contour.calculate_members();
					m_Contours.push_back(contour);
				}
			} //if (p[x] > 0)
		} // for (x = 1; x < width - 1; x++)
	} // for (y = 1; y < height - 1; y++)

	delete[] _grad;
	return true;
}

void CImageProc::divide(double distanceThreshold)
{
#if DEBUG
	IplImage *test = cvCreateImage(cvSize(width, height), 8, 3);
		vector<Point> corners;
#endif
	int i;
	int indexThreshold = INT_MAX;
	//�ҵ�������������̵�����
	for (i = 1; i < (int)m_ContourCount; i++)
	{
		if (m_Contours[i].type() != CONTOUR)
		{
			continue;
		}

		if (indexThreshold > m_Contours[i].size())
		{
			indexThreshold = m_Contours[i].size();
		}
	}
	//�����������Ϊ�±���ֵ����
	indexThreshold *= 0.8;

	vector<int> index;
	CContour newContour;
	double minDistance, dist;
	int gap, startIndex, endIndex, j, k;
	//����������������ÿ��������ǵ㣬��ͨ���ǵ����Լ��ǵ�֮��Ŀռ������±�����ϵ�ж��Ƿ����ճ��
	//����ճ������вü����������κδ���
	for (i = 1; i < (int)m_ContourCount; i++)
	{
		if (m_Contours[i].type() != CONTOUR)
		{
			continue;
		}
		CContour & contour = m_Contours[i];
		if (contour.size() <= 0) continue;
		//Ѱ�����������з�����ֵ�����Ľǵ�
		index.clear();
		binSUSAN(contour.points(), index, 15, 1, false, 4);
		//���ǵ�������2����Խǵ��������ƥ�䣬Ѱ��������ֵ���������Žǵ��
		//(���Žǵ�ԣ��ǵ���±���� > indexThreshold && �ռ���� < distanceThreshold ����¿ռ��������Ľǵ��)
		//���������Žǵ�ԣ����ı�Ե�ü�������������
		if (index.size() <= 1)
		{
			continue;
		}
		minDistance = DBL_MAX;
		startIndex = endIndex = -1;
		for (j = 0; j < index.size() - 1; j++)
		{
			for (k = j + 1; k < index.size(); k++)
			{
				dist = distance(contour[index[j]], contour[index[k]]);
				gap = abs(index[j] - index[k]);
				//�ҵ�������±�������ֵ�����ҿռ��������Ľǵ��
				if ((dist < distanceThreshold) && (gap > indexThreshold) && ((m_Contours[i].size() - gap) > indexThreshold))
				{
					if (dist < minDistance)
					{
						minDistance = dist;
						startIndex = index[j];
						endIndex = index[k];
					}
					break;
				}
			}
		} // �Խǵ���й���
		//��startIndex�±�Ϊ��ʼֵ�����ʾ������ճ������������ѭ������ʼ������һ����Ե
		if ((startIndex <= -1) || (endIndex >= contour.size()))
			continue;
		//����ճ�����
		//��֤startIndex < endIndex
		k = startIndex;
		startIndex = k < endIndex ? k : endIndex;
		endIndex = k > endIndex ? k : endIndex;
		newContour.clear();
		//�ӵ�ǰ��Ե�㼯�У�ɾ���ǵ���±�֮��ĵ㣬����֮��Ϊ��������ӵ�m_Contours������
		for (k = endIndex; k >= startIndex; k--)
		{
			newContour.push_back(contour[k]);
		}
		contour.erase_multiple(startIndex, endIndex);
		//�������Ա����
		contour.calculate_members();
		newContour.calculate_members();
		m_Contours.push_back(newContour);
	}

	m_ContourCount = m_Contours.size();

#if DEBUG
	for (i = 1; i < m_Contours.size(); i++)
	{
		drawPoints(test, m_Contours[i].points(), cvScalar(255, 255, 255));
		drawPoint(test, m_Contours[i].center() , cvScalar(255, 0, 0), 8);
		drawBox(test, m_Contours[i].box(), cvScalar(0, 0, 255), 2);
		drawPoints(test, corners, cvScalar(0, 255, 0), 4);
	}
	imshow("divide", test);
	cvReleaseImage(&test);
#endif
}

////************************************************���г�Ա��������*************************************************////
void CImageProc::run()
{
	if (m_Contours.size() <= 1) return;
	
	//����ճ�����
	divide();

	//����ĸ
	findNut();

	//��Ӳ�Һ���ĸ
	findCoin();
	
	//��L�Ͱ���
	findHexKey();

	//���ݶ�
	findScrew();
}

void CImageProc::writeResult(uchar * result, int channels, int _count[5])
{
	int widthStep = width*channels;
	size_t i, j, k, c, count;
	int x, y, index;
	Vector3 color;
	::memset(_count, 0, 5 * sizeof(int));
	for (i = 0; i < m_ContourCount; i++)
	{
		_count[(int)m_Contours[i].type()]++;
		color = COLOR_TABLE[(int)m_Contours[i].type()];
		const vector<Point> & contour = m_Contours[i].points();
		count = contour.size();
		for (j = 0; j < count; j++)
		{
			for (k = 0; k < 8; k += 2)
			{
				x = contour[j].x + dx[k];
				y = contour[j].y + dy[k];
				if (y < 0 || y >= height || x < 0 || x >= width)
				{
					continue;
				}
				index = y*widthStep + x*channels;
				for (c = 0; c < channels; c++)
				{
					result[index + c] = color[c];
				}
			}
		}
	}
}

void CImageProc::writeContour()
{
	::memset(m_Bin, 0, width*height*sizeof(uchar));
	size_t i, j, count;
	int x, y;
	uchar value = 0;
	for (i = 0; i < m_ContourCount; i++)
	{
		value = m_Contours[i].type();
		const vector<Point> & contour = m_Contours[i].points();
		count = contour.size();
		for (j = 0; j < count; j++)
		{
			x = contour[j].x;
			y = contour[j].y;
			m_Bin[y*width + x] = value;
		}
	}
}

vector<EContourType> CImageProc::getContourType() const
{
	vector<EContourType> contourType;
	for (int i = 0; i < (int)m_ContourCount; i++)
	{
		contourType.push_back(m_Contours[i].type());
	}
	return contourType;
}

vector<vector<Point>> CImageProc::getContour() const
{
	vector< vector<Point> > contours;
	for (int i = 0; i < (int)m_ContourCount; i++)
	{
		contours.push_back(m_Contours[i].points());
	}
	return contours;
}

vector<Box> CImageProc::getBox() const
{
	vector<Box> boxes;
	for (int i = 0; i < (int)m_ContourCount; i++)
	{
		boxes.push_back(m_Contours[i].box());
	}
	return boxes;
}

void CImageProc::findNut()
{
	//������ĸ�����Ե���ĵ��غϵ�������Ϊ�о�
	int i, j, k, x, y, accum;
	Point center;
	Point t(5, 5);
	bool flag = false;
	double temp;
	double lower = 0.7;
	double upper = 1.3;
	for (i = 1; i < m_ContourCount; i++)
	{
		if (m_Contours[i].type() != CONTOUR)
		{
			continue;	//ֻ����δ����ı�Ե
		}
		center = m_Contours[i].center();
		flag = false;
		for (j = i + 1; j < m_ContourCount; j++)
		{
			if (center.absDiff(m_Contours[j].center()) < t)
			{
				flag = true;
				//ɾȥ�϶̵��ڱ�Ե
				if (m_Contours[i].size() > m_Contours[j].size())
				{
					m_Contours[i].setType(NUT);
					m_Contours.erase(m_Contours.begin() + j);
					m_ContourCount--;
				}
				else
				{
					m_Contours[j].setType(NUT);
					m_Contours.erase(m_Contours.begin() + i--);
					m_ContourCount--;
				}
				break;
			}
		}
		if (!flag)
		{
			//����Ӿ��γ���Ⱥ��������ص�ֵΪ�оݣ��ٴν����ж�
			temp = (double)m_Contours[i].box().width / m_Contours[i].box().height;
			if (lower < temp && temp < upper)
			{
				//�����Ե���ĵ㼰������������ֵ
				accum = 0;
				for (k = 0; k < 9; k++)
				{
					x = center.x + dx[k];
					y = center.y + dy[k];
					if (m_Bin[y*width + x])
					{
						accum++;
					}
				}
				if (accum >= 4)
				{
					m_Contours[i].setType(NUT);
				}
			}
		} // 	if(!flag)
	}
}

bool CImageProc::findCoin()
{
	size_t i, k;
	double lower = 0.7;
	double upper = 1.3;
	double temp = 0.0;
	//���ȸ��ݰ�Χ��Ե�������Ӿ��εĳ����ɸѡ����ѡ��Ե
	for (i = 1; i < m_ContourCount; i++)
	{
		if (m_Contours[i].type() != CONTOUR)
		{
			continue;	//ֻ����δ����ı�Ե
		}
		temp = (double)m_Contours[i].box().width / m_Contours[i].box().height;
		if (lower < temp && temp < upper)
		{
			m_Contours[i].setType(COIN);
		}
	}
	//�����������ĵ㵽�������ƽ������������С���룬�Ƚ����-��С����֮����ƽ������Ĳ�ֵ����ֵ�������޳��ñ�Ե
	//�޳����в���������������֮��ʣ���������Ϊ����Բ������
	size_t count;
	Point center;
	double average, segma;
	vector<double> dist;
	for (i = 1; i < m_ContourCount; i++)
	{
		if (COIN == m_Contours[i].type())
		{
			const vector<Point> & contour = m_Contours[i].points();
			count = contour.size();
			if (count <= 0)
			{
				m_Contours[i].setType(CONTOUR);
				continue;
			}
			center = m_Contours[i].center();
			dist.clear();
			//���������㵽���ĵ�ķ���
			average = 0.0;
			for (k = 0; k < count; k++)
			{
				temp = distance(center, contour[k]);
				dist.push_back(temp);
				average += temp;
			}
			average /= count;
			segma = 0.0;
			for (k = 0; k < dist.size(); k++)
			{
				segma += ((dist[k] - average)*(dist[k] - average));
			}
			segma /= (count - 1.0);
			if (segma > 10.0)
			{
				m_Contours[i].setType(CONTOUR);
			}
		} // if (COIN == m_ContourType[i])
	} // for (i = 1; i < m_ContourCount; i++)
	
	return true;
}

bool CImageProc::findHexKey()
{
	size_t i;
	for (i = 1; i < m_ContourCount; i++)
	{
		if (m_Contours[i].type() != CONTOUR)
		{
			continue;
		}
		if (isOutContour(i))
		{
			m_Contours[i].setType(HEX_KEY);
		}
	}
	return true;
}

bool CImageProc::findScrew()
{
	size_t i, t = 3;
	vector<int> index;
	for (i = 1; i < m_ContourCount; i++)
	{
		if (m_Contours[i].type() != CONTOUR)
		{
			continue;
		}
		//ͨ�����ǵ������ж��Ƿ������ݶ�
		index.clear();
		binSUSAN(m_Contours[i].points(), index, 20);
		if (index.size() >= t)
		{
			m_Contours[i].setType(SCREW);
		}
	}

	return true;
}

//////************************************************˽�г�Ա��������**************************************************////

/////**************findHexKey��������ظ�������******************//////
bool CImageProc::isOutContour(const int index)
{
	const vector<Point> & contour = m_Contours[index].points();
	int x = m_Contours[index].center().x;
	int y = m_Contours[index].center().y;

	//ͨ���жϸõ�ͬ�С�ͬ���Ƿ񱻱�Ե���Χ��ȷ���õ��Ƿ���������ĵ�
	//��������Χ����һ����������ĵ㣬��������������ڵĵ�
	vector<int> coly;	//��¼ͬһ�е�y����
	vector<int> rowx;	//��¼ͬһ�е�x����
	size_t i, count = contour.size();
	int temp1, temp2;
	//���������㣬�ҵ���pointͬ��ͬ�е������㣬���������Ӧ��x��y����
	for (i = 0; i < count; i++)
	{
		temp1 = contour[i].x;
		temp2 = contour[i].y;
		//��ͬһ�еĵ��y����
		if (abs(temp1 - x) <= 1)
			coly.push_back(temp2);
		//��ͬһ�е�x����
		if (abs(temp2 - y) <= 1)
			rowx.push_back(temp1);
	}
	//�ж�point��ˮƽ�������Ƿ񱻰�Χ
	temp1 = temp2 = 0;
	for (i = 0; i < rowx.size(); i++)
	{
		if (rowx[i] > x)
			temp1++;
		else
			temp2++;
	}
	if (!temp1 || !temp2)
	{
		return true;	//ͬһ����ֻ��һ���������㣬��˵���õ���ˮƽ�����ϲ�����Χ
	}
	//�ж�point�ڴ�ֱ�������Ƿ񱻰�Χ
	temp1 = temp2 = 0;
	for (i = 0; i < coly.size(); i++)
	{
		if (coly[i] > y)
			temp1++;
		else
			temp2++;
	}
	if (!temp1 || !temp2)
	{
		return true; //ͬһ����ֻ��һ���������㣬��˵���õ��ڴ�ֱ�����ϲ�����Χ
	}
	return false;
}

/////***************findScrew��������ظ�������******************//////
bool CImageProc::binSUSAN(const vector<Point> & contour, vector<int> & _index, int upper, int lower, bool method, int r) const
{
	if (lower <= 0) lower = 1;
	int x, y, same;
	size_t i, j, k, contourSize = contour.size();
	int *_susan = new int[contourSize];
	::memset(_susan, 0, contourSize*sizeof(int));
	uchar curGray;
	//����ÿ�����������SUSAN����ֵ,ȷ����ʼ�ǵ�ֵ
	for (i = 0; i < contourSize; i++)
	{
		x = contour[i].x;
		y = contour[i].y;

		//���Ա߽����صĴ���
		if (x<r || x>(width - r) || y<r || y>(height - r))
		{
			continue;
		}

		//���㵱ǰ��SUSAN����ֵ
		curGray = method ? m_Bin[y*width + x] : 255 - m_Bin[y*width + x];
		same = 0;

		for (j = x - r; j < x + r; j++)
		{
			for (k = y - r; k < y + r; k++)
			{
				double dist = (j - x) * (j - x) + (k - y) * (k - y);
				if (dist > r * r)
					continue;
				if (m_Bin[k*width + j] == curGray)
					same++;
			}
		}
		if (lower <= same && same <= upper)
		{
			_susan[i] = same;
		}
	}

	//����ֲ���Сֵ�������õ���Ϊ�ǵ㱣��
	int index, min;
	for (i = 0; i < contourSize; i++)
	{
		if (_susan[i] == 0)
		{
			continue;
		}
		//�ҵ��ֲ���Сֵ
		min = _susan[i];
		index = i;
		for (i++; i < contourSize; i++)
		{
			if (_susan[i] == 0)
			{
				break;
			}
			if (_susan[i] < min)
			{
				min = _susan[i];
				index = i;
			}
		}
		_index.push_back(i);
	}

	delete[] _susan;
	return true;
}
