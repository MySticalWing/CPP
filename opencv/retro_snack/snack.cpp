#include "snack.h"
namespace SNACK
{
	//map��//
	//���캯��//
	Map::Map()
	{
		for (int i = 0; i < MAP_SIZE; i++)
		{
			map[0][i] = '-';
			map[MAP_SIZE - 1][i] = '-';
		}
		for (int i = 0; i < MAP_SIZE; i++)
		{
			map[i][0] = '|';
			map[i][MAP_SIZE - 1] = '|';
		}
		//��ʼ����ͼ//
		for (int i = 1; i < MAP_SIZE - 1; i++)
			for (int j = 1; j < MAP_SIZE - 1; j++)
				map[i][j] = ' ';
	}
	//��ӡ��ͼ//
	void Map::MapPrint()
	{
		cout << "\t\t\t\t����̨�µ�̰������Ϸ\n";
		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE; j++)
				cout << map[i][j];
			cout << endl;
		}
	}
	//�����ͼ//
	void Map::MapClean()
	{
		for (int i = 0; i < MAP_SIZE; i++)
		{
			map[0][i] = '-';
			map[MAP_SIZE - 1][i] = '-';
		}
		for (int i = 0; i < MAP_SIZE; i++)
		{
			map[i][0] = '|';
			map[i][MAP_SIZE - 1] = '|';
		}
		//��ʼ����ͼ//
		for (int i = 1; i < MAP_SIZE - 1; i++)
			for (int j = 1; j < MAP_SIZE - 1; j++)
				map[i][j] = ' ';
	}

	//snack��//
	//��ȡ�û�����//
	void Snack::readKey()
	{
		//�ӳ�һ��ʱ�䣬�ȴ���������//
		int keyValue;
		std::clock_t start;
		std::clock_t finish = start = clock();
		while ((double)(finish - start) / CLOCKS_PER_SEC < 1.0 / moveSpeed)
		{
			if (_kbhit())
			{
				keyValue = _getch();
				if (keyValue == 224)
				{
					keyValue = _getch();
					switch (keyValue)
					{
					case(72) : dirction = UP;    return; //��
					case(80) : dirction = DOWN;  return; //��
					case(75) : dirction = LEFT;  return; //��
					case(77) : dirction = RIGHT; return; //��
					default  : break;	//������������ַ�
					}
				}
				//����Esc��//
				if (keyValue == 27)
				{
					gameOver = true;
					return;
				}
			}
			finish = clock();
		}
	}
	//�����ƶ�����//
	void Snack::snackMove()
	{
		switch (dirction)
		{
		case(UP)    : snackHead.x -= 1; break;
		case(DOWN)  : snackHead.x += 1; break;
		case(LEFT)  : snackHead.y -= 1; break;
		case(RIGHT) : snackHead.y += 1; break;
		}

		//�ж���Ϸ�Ƿ����//
		//ײ���˱߽�//
		if (snackHead.y == MAP_SIZE - 1 || snackHead.y == 0 || 
			snackHead.x == MAP_SIZE - 1 || snackHead.x == 0)
		{
			gameOver = true;
			return;
		}
		//ײ���Լ�������//
		for (auto p = ++snack.begin(); p != snack.end(); p++)
			if (p->x == snackHead.x && p->y == snackHead.y)
			{
				gameOver = true;
				return;
			}

		//����ͷ����ȫ����־������Ϊ'*'//
		snack.begin()->type = '*';
		//��תsnack��ʹ�ú���ı���snack����������//
		snack.reverse();
		std::list<Node>::iterator p = snack.begin();
		std::list<Node>::iterator pnext = ++snack.begin();

		//����snack�������ÿһ���ڵ����θ�ֵΪ��һ���ڵ��ֵ//
		while (pnext != snack.end())
			*p++ = *pnext++;
		*p = snackHead;
		snack.reverse();
	}
	//��ͼ���º���//
	void Snack::snackMapUpdata()
	{
		//��յ�ͼ//
		snackMap.MapClean();

		//���ͼ���������괦д�������־//
		for (auto p = snack.begin(); p != snack.end(); p++)
		{
			snackMap.MapUpdata(*p);
		}
		//��ʳ��д������//
		snackMap.MapUpdata(food);
	}
	//����ʳ��//
	void Snack::snackGenerateFood()
	{
		//��ʼ�����������//
		srand((unsigned int)time(NULL));
		//���������//
		int x = 0;
		int y = 0;
		while (1)
		{
			x = rand() % (MAP_SIZE - 1);
			y = rand() % (MAP_SIZE - 1);
			//���������鿴ʳ�������Ƿ��������غ�//
			bool flag = false;
			for (auto p = snack.begin(); p != snack.end(); p++)
				//������ʳ���������غ�
				if (x == p->x && y == p->y)
				{
					flag = true;
					break;
				}
			//�Ƿ���߽��غ�//
			if (x == 0 || y == 0) flag = true;
			//�����غ�//
			if (!flag)
			{
				food.x = x;
				food.y = y;
				return;
			}
		}
	}
	//��ʳ��//
	bool Snack::snackEatFood()
	{
		Node newNode;
		//�߳Ե���ʳ��//
		if (snackHead.x == food.x && snackHead.y == food.y)
		{
			//��������������ڵ㣬����β����½ڵ�//
			snack.reverse();
			auto p1 = snack.begin();
			auto p2 = ++snack.begin();

			//�������ڵ�λ��ͬһ��//
			if(p1->x == p2->x)
				//�ж����һ���ڵ�����ڵ����ڶ����ڵ��λ�ù�ϵ//
				//���//
				if (p1->y < p2->y)
					//�½ڵ�λ�����һ���ڵ����//
				{
					Node temp(p1->x, p1->y - 1);
					newNode = temp;
				}
				//�ұ�//
				else
					//�½ڵ�λ�����һ���ڵ��ұ�//
				{
					Node temp(p1->x, p1->y + 1);
					newNode = temp;
				}
			//�������ڵ�λ��ͬһ��//
			else if(p1->y == p2->y)
			{
				//�ж����һ���ڵ�����ڵ����ڶ����ڵ��λ�ù�ϵ//
				//����//
				if (p1->x < p2->x)
					//�½ڵ�λ�����һ���ڵ������//
				{
					Node temp(p1->x - 1, p1->y);
					newNode = temp;
				}
				//����//
				else
					//�½ڵ�λ�����һ���ڵ������//
				{
					Node temp(p1->x + 1, p1->y);
					newNode = temp;
				}
			}
			snack.push_front(newNode);
			snack.reverse();
			return true;
		}
		return false;
	}
	//��Ϸ���̿��ƺ���//
	void Snack::snackShow()
	{
		while (1)
		{
			//����//
			std::system("cls");
			//��Ϸ����//
			if (gameOver)
			{
				cout << "��Ϸ����" << endl;
				return;
			}
			//�߳Ե���ʳ��//
			if (snackEatFood())
				//����ʳ��//
				snackGenerateFood();
			//���µ�ͼ//
			snackMapUpdata();
			//��ӡ��ͼ//
			snackMap.MapPrint();
			//��ȡ�û��ķ���//
			readKey();
			//�ƶ�//
			snackMove();
		}
	}

	//���Ժ���//
	void Snack::TestsnackShow()
	{
		for (auto p = snack.begin(); p != snack.end(); p++)
			std::cout << *p << endl;
	}
}