//�Զ���ͼƬ��������ֻ�ܴ���ͨ���Ҷ�ͼ��
//
//
#ifndef _CIMAGE_PROC_H_
#define _CIMAGE_PROC_H_

#define MAX_CLASS_COUNT 5

#include "mydefine.h"
#include "ccontour.h"
#include <vector>
#include <stack>

using namespace std;

class CImageProc
{
public:
	typedef unsigned char uchar;
private:
	uchar *m_Bin;
	const int width;
	const int height;
	vector<CContour> m_Contours;		//��Ե��
	size_t           m_ContourCount;
	
	//��̬��Ա����
	static int dx[9];
	static int dy[9];
	static Vector3 COLOR_TABLE[MAX_CLASS_COUNT];	//��ɫ��

public:
	////************************************************���졢������������*************************************************////
	//@param imageData : ���ͼƬ����
	CImageProc(const int width, const int height, const int channels, const uchar * imageData);
	~CImageProc();

private:
	////********************************************��ʼ����������*************************************************////
	//RGBͼת�Ҷ�ͼ
	//@param color : ��ڲ����������ɫͼƬ����
	//@param gray  : ���ڲ���������ת����ĻҶ�ͼ����
	void cvtRGB2Gray(const uchar *color, uchar * gray)const; //cbc

															 //����OTSU��������ԭͼ���ݶ�ֵ���󱣴浽bin��Ա��
															 //@param data : ��ڲ������Ҷ�ͼ����
	void otsuCut(const uchar *data);

	//����OTSU�ָ����ֵ��ͨ������ֵ����
	//@param data : ��ڲ������Ҷ�ͼ����
	//@return : ����ֵ��������ֵ
	int getOtshThrshold(const uchar * data) const;

	//����Ҷ�ֱ��ͼ
	//@param data : ��ڲ������Ҷ�ͼ����
	//@param hist[256] : ���ڲ��������ػҶ�ֱ��ͼ
	bool normalizedHist(const uchar * data, double hist[256]) const;

	//��ֵͼ��ĸ�ʴ����ı�m_Bin��Ա��ֵ
	void erode();

	//��ֵͼ������
	//@param times: ��ڲ��������͵�������
	void dilate(const int times = 1);

	//��ʼ����Ա������׼����ʶ����Ҫ�����ݣ���ı����зǳ������Ա����
	void initMembers();

	//�ڶ�ֵͼ��Ѱ�ұպϱ�Ե��������
	//@param threshold: ��ڲ�������Ե����С�ڸò����ı�Ե������
	bool findContours(int threshold = 30);

	//�ж��Ƿ���ڼ�ճ���������������д����������κδ���
	//@param distanceThreshold : �������Ͻǵ�֮��Ŀռ������ֵ
	void divide(double distanceThreshold = 30.0);

	////************************************************���г�Ա��������*************************************************////
public:
	//�����Ӻ�������m_Contour�еı�Ե���з���
	void run();

	//�������Ľ����д�������result��
	//@param result : ���ڲ��������б�Ե�㰴���ͬ������ֵΪ��ͬ����ɫ
	//@param channels : ��ڲ�����result���������ͼƬ��ͨ����
	//@param count[5] : ���ڲ��������ص�ǰͼƬ��ÿһ�����Ŀ
	void writeResult(uchar * result, int channels, int count[5]);

	//��m_Contours�еı�Ե����m_Bin��
	void writeContour();

	//����������������Ӿ���
	vector<Box> getBox() const ;

	//������������������
	vector<EContourType> getContourType() const ;

	//������������
	vector< vector<Point> > getContour() const;

	//��ȡm_Bin��Ա
	const uchar * getBin() const
	{
		return m_Bin;
	}

	////************************************************˽�г�Ա��������**************************************************////
private:

	//Ѱ����ĸ
	//ͨ���ж�������Ե���ĵ��غϻ���Ӿ������㳤����ұ�Ե���ĵ㼰��8-����Ϊ������
	void findNut();	//cbc

	//Ѱ��Ӳ��
	//ͨ����Ӿ��γ���ȵõ�Ӳ�ҵĺ�ѡ��������ͨ���ж������㵽���ĵ����-��С�����Լ�ƽ�������ϵ���ҳ�������Ӳ��
	bool findCoin();	//cbc

	//Ѱ��L�����ǰ���
	//ԭ��Ҫʶ���Ŀ���У���L�����ǰ����⣬����Ŀ���Ե��Ϊ͹����Σ������Ե�����Ķ�λ�ڱ�Ե��
	//     L�����ǰ��ֱ�ԵΪ������Σ������Ե����λ��������
	bool findHexKey();

	//Ѱ���ݶ�
	//ͨ���жϽǵ����Ƿ������ֵ�ж��ǹ�Ϊ�ݶ�
	bool findScrew();

	//�жϵ��Ƿ�λ�������⣬�Ƿ���true�����򷵻�false
	//@param index : ��ڲ�����Ҫ����ı�Ե���±�
	bool isOutContour(const int index);

	//��ֵͼ���SUSAN�ǵ����㷨
	//@param contour : ��ڲ�������Ե��ļ���
	//@parame index : ���ڲ���������ÿ���ǵ���contour�����е��±�
	//@param upper : ��ڲ������Ͻ�
	//@param lower : ��ڲ������½�
	//@param method : ��ڲ�����true: Ѱ�Һ�ֵͬ����; false: Ѱ�Һ˲�ֵͬ����
	//@param radius : ��ڲ�����ģ��뾶
	bool binSUSAN(const vector<Point> & contour, vector<int> & index, int upper, int lower = 1, bool method = true, int radius = 4) const; //cbc

	//�������ܣ�
	//		����a��b����ľ���
	inline double distance(const Point & a, const Point & b)
	{
		return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
	}
	//����㵽ֱ�ߵľ��룬ֱ�߷���Ϊy=k*x+b
	inline double distance(const Point & a, const double & k, const double & b) const
	{
		return abs(k*a.x - a.y + b) / sqrt(k*k + 1);
	}
};
#endif
