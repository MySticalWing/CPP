//snack���map��Ķ���//

#ifndef _SNACK_H_
#define _SNACK_H_

#include <Windows.h>
#include <iostream>
#include <ctime>
#include <conio.h>
#include <cstdlib>
#include <list>

namespace SNACK
{
	using std::cout;
	using std::endl;
	using std::cin;
#define MAP_SIZE  22
	//�ڵ㶨��//
	struct Node
	{
		int x;
		int y;
		char type;

		//���캯��//
		Node(int x = 0, int y = 0, char t = '*')
		{
			this->x = x; this->y = y; type = t;
		}

		//���ظ�ֵ�����//
		Node & operator=(const Node &n)
		{
			this->x = n.x;
			this->y = n.y;
			this->type = n.type;
			return *this;
		}

		//����<<�����//
		friend std::ostream & operator<<(std::ostream & os, const Node &n)
		{
			os << "(" << n.x << "," << n.y << "," << n.type << ")" << "\t";
			return os;
		}
	};

	//��ͼ��//
	class Map
	{
	private:
		//��ŵ�ͼ������//
		char map[MAP_SIZE][MAP_SIZE];

	public:
		Map();
		~Map() {}
		void MapPrint();
		//���µ�ͼָ�����ֵ//
		void MapUpdata(const Node &n)
		{
			map[n.x][n.y] = n.type;
		}
		//�����ͼ//
		void MapClean();
	};

	//snack��//
	class Snack
	{
	public:
		enum Direction { UP, DOWN, LEFT, RIGHT };
		enum MoveSpeed { LOW = 3, MIDDLE, HIGHT };
	private:
		//ʵ����һ��Node list �����࣬���ڴ��snack���������//
		std::list<Node> snack;

		//��ŵ�ͼ�ĳ�Ա����//
		Map snackMap;

		//�����ͷ����Ϣ//
		Node snackHead;
		Direction dirction;
		MoveSpeed moveSpeed;
		
		//��Ϸ�Ƿ������־//
		bool  gameOver = false;

		//ʳ�������//
		Node food;

	public:
		Snack()
		{
			Node temp(1, 4, '#');
			snackHead = temp;
			snack.push_back(snackHead);
			snackMap.MapUpdata(snackHead);
			for (int i = 1; i <= 3; i++)
			{
				temp.y = snackHead.y - i;
				temp.type = '*';
				snack.push_back(temp);
				snackMap.MapUpdata(temp);
			}
			temp.x = MAP_SIZE/2;
			temp.y = MAP_SIZE/2;
			temp.type = '*';
			food = temp;
			dirction = RIGHT;
			moveSpeed = LOW;
		}
		~Snack() {}
		void readKey();
		void snackMove(void);
		void snackMapUpdata();
		void snackShow(void);
		bool snackGameOver() { return gameOver; }
		void snackGenerateFood();
		bool snackEatFood();

		//���Գ�Ա����//
		void TestsnackShow();
		void TestsnackDirection() { cout << dirction << endl; }
	};
}
#endif
