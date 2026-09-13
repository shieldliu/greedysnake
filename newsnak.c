#include<stdio.h>
#include<string.h>
#ifdef _WIN32
#include <Windows.h> // For Sleep (milliseconds)
#else
#include <unistd.h> // For sleep (seconds)
#endif
#define GRIDE_NUMBER  16

struct position {
	int x;
	int y;
};

/*
* 二维数组里面存储了每一个格子的状态,
* 0: 空的
* 1: 有蛇!
* 2: 有鸟蛋!
*/

/* 方向键定义 */
#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4
#include <conio.h>
int getcontrol(void)
{
	int ch = getch();
	if (ch == 'A' || ch == 'a') return LEFT;
	if (ch == 'D' || ch == 'd') return RIGHT;
	if (ch == 'W' || ch == 'w') return UP;
	if (ch == 'S' || ch == 's') return DOWN;
	if (ch == 27 || ch == -1) return -1;  /* ESC 或出错时返回 -1 表示退出 */
	return(0);  /* 其他按键忽略 */
}

void move_snake(struct position* snake)
{
	int direction = getcontrol();
	struct position p = snake[0];
	switch (direction)
	{
	case LEFT:
		p.x--;
		break;
	case RIGHT:
		p.x++;
		break;
	case UP:
		p.y--;
		break;
	case DOWN:
		p.y++;
		break;
	}
	if(p.x<0 || p.x >= GRIDE_NUMBER)
	{
		exit(0);
	}
	if (p.y < 0 || p.y >= GRIDE_NUMBER)
	{
		exit(0);
	}
	snake[0] = p;
}

void display(int data[GRIDE_NUMBER][GRIDE_NUMBER])
{
	int row, column;
	printf("----------------\n");
	for (row = 0;row < GRIDE_NUMBER;row++)
	{
		printf("|");
		for (column = 0;column < GRIDE_NUMBER;column++)
		{
			int v = data[row][column];
			switch (v)
			{
			case 0:
				printf(" ");
				break;
			case 1:
				printf("+");
				break;
			case 2:
				printf("O");
				break;
			}
		}
		printf("|\n");
	}
	printf("----------------\n\n\n");
}

/*输入: 蛇的坐标数组,比如 [ (0,0), (0,1), (1,1) ], 前面是头,后面是尾巴
*   蛇的长度: 比如3
* 输出: map 比如上面的内容要把map填充为
*   1 1 0 0
*   0 1 0 0
*   0 0 0 0
*   0 0 0 0
*/



void refresh(struct position* snake, int length, int map[GRIDE_NUMBER][GRIDE_NUMBER])
{
	for (int i = 0;i < GRIDE_NUMBER; i++)
		for (int j = 0;j < GRIDE_NUMBER; j++)
			map[i][j] = 0;
	for (int i = 0; i < length; i++)
	{
		struct position p = snake[i];
		map[p.y][p.x] = 1;
	}
}


struct position layegg(void)
{

}

void main()
{
	//定义需要用到的变量
	int MAP[GRIDE_NUMBER][GRIDE_NUMBER];
	struct position snake[GRIDE_NUMBER * GRIDE_NUMBER];
	int snake_size;
	struct position egg;

	//初始化变量
	snake_size = 1;
	snake[0].x = GRIDE_NUMBER / 2;
	snake[0].y = GRIDE_NUMBER / 2;

	while (1)
	{
		refresh(snake, snake_size, MAP);
		display(MAP);
		move_snake(snake);
#ifdef _WIN32
		Sleep(100);
#else
		sleep(1);
#endif
	}
}
