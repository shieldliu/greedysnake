#include<stdio.h>
#include<string.h>
#include <stdlib.h>
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
struct SNAKE
{
	struct position body[GRIDE_NUMBER * GRIDE_NUMBER];
	int size;
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

void move_snake(struct SNAKE* snake)
{
	int direction = getcontrol();
	
	for (int i = snake->size ;i>0 ;i--)
	{
		snake->body[i] = snake->body[i-1];
	}

	struct position p = snake->body[0];
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
	snake->body[0] = p;
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

int check(struct SNAKE snakes, struct position where_egg)
{
	int length = snakes.size;
	struct position* p = snakes.body;
	for (int i = 0;i < length;i++)
	{
		if (where_egg.x == p[i].x &&
			where_egg.y == p[i].y)
			return 1;
	}
	return 0;
}

struct position layegg(struct SNAKE reference)
{
	struct position pos;
	int result = 0;
	do {
		pos.x = rand() % GRIDE_NUMBER;
		pos.y = rand() % GRIDE_NUMBER;
		result = check(reference, pos);
	} while (result == 1);
	return pos;
}

/*
* 检查蛇的身体是否撞到自己,检查方法是检查头是不是和身体其他部分重叠
* 
* 返回值:
* 1: 有重叠
* 0: 没有重叠
*/
int overlap(struct SNAKE* snake)
{
	struct position head = snake->body[0];
	for(int i = 1; i<snake->size; i++){
		if (head.x == snake->body[i].x && 
			head.y == snake->body[i].y )
		{
			return 1;
		}
	}
	return 0;
}

void main()
{
	//定义需要用到的变量
	int MAP[GRIDE_NUMBER][GRIDE_NUMBER];
	struct SNAKE snake;
	struct position egg;

	srand(time(NULL));
	//初始化变量
	snake.size = 1;
	snake.body[0].x = GRIDE_NUMBER / 2;
	snake.body[0].y = GRIDE_NUMBER / 2;
	//egg = layegg(snake);
	egg.x = 8; egg.y = 7;
	while (1)
	{
		refresh(snake.body, snake.size, MAP);
		MAP[egg.y][egg.x] = 2;
		display(MAP);
		move_snake(&snake);
		if (egg.x == snake.body[0].x &&
			egg.y == snake.body[0].y)
		{
			snake.size++;
			egg = layegg(snake);
		}
		if (overlap(&snake) == 1) {
			exit(0);
		}
#ifdef _WIN32
		Sleep(100);
#else
		sleep(1);
#endif
	}
}
