#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termio.h>
#include <stdlib.h>
#include <time.h>

/* 方向键定义 */
#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4

/*
 * get_input_char — 获取单个键盘输入
 *
 * 功能：
 *   在 Linux 下将终端切换为原始模式，逐字节读取键盘输入而不需要用户按回车。
 *   在 MSDOS 下直接调用 getch() 实现相同效果。
 *
 * 参数：无
 *
 * 返回值：
 *   读取到的按键 ASCII 码
 *   -1 — 获取终端属性或设置失败
 *
 * 注意：
 *   函数执行后会恢复终端的原始设置，不会影响后续终端使用。
 */
#ifdef MSDOS
#include <conio.h>
int get_input_char(void)
{
	return getch();
}
#else
int get_input_char(void)
{
	struct termios tm, tm_old;
    int fd = 0, ch;

    /* 保存当前终端设置 */
    if (tcgetattr(fd, &tm) < 0) {
        return -1;
    }

    tm_old = tm;
    cfmakeraw(&tm);  /* 切换为原始模式：逐字节读取，不回显 */
    if (tcsetattr(fd, TCSANOW, &tm) < 0) {
        return -1;
    }

    ch = getchar();  /* 读取一个按键 */
    /* 恢复原始终端设置 */
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {
        return -1;
    }
	return ch;
}
#endif

/*
 * getcontrol — 将按键映射为游戏方向控制
 *
 * 功能：
 *   调用 get_input_char() 获取用户按键，将 WASD 键映射为 UP/DOWN/LEFT/RIGHT
 *   方向常量，ESC 键和出错时返回退出信号。
 *
 * 参数：无
 *
 * 返回值：
 *   UP(1)   — 按 W 或 w，向上移动
 *   LEFT(2) — 按 A 或 a，向左移动
 *   DOWN(3) — 按 S 或 s，向下移动
 *   RIGHT(4)— 按 D 或 d，向右移动
 *   -1      — 按 ESC(ASCII 27) 或输入出错，表示请求退出
 *   0       — 其他按键，忽略本次输入
 */
int getcontrol(void)
{
	int ch=get_input_char();
    if(ch == 'A' || ch =='a') return LEFT;
    if(ch == 'D' || ch =='d') return RIGHT;
    if(ch == 'W' || ch =='w') return UP;
    if(ch == 'S' || ch =='s') return DOWN;
	if(ch == 27 || ch==-1) return -1;  /* ESC 或出错时返回 -1 表示退出 */
	return(0);  /* 其他按键忽略 */
}

/*
 * show_to_screen — 将游戏缓冲区渲染到终端
 *
 * 功能：
 *   接收一个二维字符缓冲区，在其上下两侧打印 "=" 分隔线，
 *   左右两侧打印 "|" 边框，形成封闭的矩形游戏画面。
 *
 * 参数：
 *   buffer — 指向游戏缓冲区（二维数组）的指针
 *   width  — 游戏区域的列数
 *   height — 游戏区域的行数
 *
 * 返回值：无
 *
 * 输出示例：
 *   ======================
 *   |                    |
 *   |        X           |
 *   |            O       |
 *   |                    |
 *   ======================
 */
void show_to_screen(void *buffer, int width, int height)
{
	char * contents = (char*)buffer;
	/* 打印上边界 */
	for(int i=0; i<width+2; i++)
	{
		printf("=");
	}
	printf("\n");

	/* 逐行打印游戏内容，每行两侧加 | 边框 */
	for(int j=0; j<height; j++)
	{
		char * line_of_contents = contents + j*width;
		printf("|");
		for(int i=0; i<width; i++)
		{
			printf("%c", line_of_contents[i]);
		}
		printf("|\n");
	}
	/* 打印下边界 */
	for(int i=0; i<width+2; i++)
	{
		printf("=");
	}
	printf("\n");
}

/* 游戏区域大小（16x16 网格） */
#define PLAYGROUND_SIZE 16
char buffer[PLAYGROUND_SIZE][PLAYGROUND_SIZE];

/* 蛇身坐标数组（最大可占满整个区域） */
int snake_block_x[PLAYGROUND_SIZE*PLAYGROUND_SIZE];
int snake_block_y[PLAYGROUND_SIZE*PLAYGROUND_SIZE];
int snake_lengh;     /* 当前蛇身长度 */
int food_pos_x;      /* 食物 X 坐标 */
int food_pos_y;      /* 食物 Y 坐标 */

/*
 * display_snake — 绘制并输出当前游戏帧
 *
 * 功能：
 *   先通过 ANSI 转义码清空终端屏幕并将光标复位到左上角，
 *   然后清空游戏缓冲区，将所有蛇身节绘制为 'X'、食物绘制为 'O'，
 *   最后调用 show_to_screen() 将缓冲区渲染到终端。
 *
 * 参数：无（所有数据从全局变量读取）
 *
 * 返回值：无
 *
 * 调用依赖：
 *   - snake_block_x[] / snake_block_y[] — 蛇身各节坐标
 *   - snake_lengh — 蛇身当前长度
 *   - food_pos_x / food_pos_y — 食物坐标
 *   - show_to_screen() — 渲染输出函数
 */
void display_snake()
{
	printf("\033[2J\033[H");  /* ANSI 转义：清屏 + 光标归位 */
	memset(buffer, ' ', sizeof(buffer));
	/* 绘制所有蛇身节为 'X' */
	for(int i=0; i<snake_lengh; i++)
	{
		int x = snake_block_x[i];
		int y = snake_block_y[i];
		buffer[y][x] = 'X';
	}
	/* 绘制食物为 'O' */
	buffer[food_pos_y][food_pos_x]='O';
	show_to_screen(buffer, PLAYGROUND_SIZE,PLAYGROUND_SIZE);
}

/*
 * on_snake — 判断指定坐标是否与蛇身重叠
 *
 * 功能：
 *   遍历蛇身所有节的坐标，检测给定的 (x, y) 是否落在蛇身上。
 *   用于食物生成时避免与蛇身重叠。
 *
 * 参数：
 *   x — 待检测的 X 坐标（列位置）
 *   y — 待检测的 Y 坐标（行位置）
 *
 * 返回值：
 *   0 — 该坐标没有被蛇身占据
 *   1 — 该坐标与蛇身某节重叠
 */
int on_snake(int x, int y)
{
	for(int i=0; i<snake_lengh;i++)
	{
		if(x==snake_block_x[i] && y==snake_block_y[i])
			return 1;
	}
	return 0;
}

/*
 * generate_food_position — 随机生成食物位置
 *
 * 功能：
 *   在游戏区域 (0~PLAYGROUND_SIZE-1) 范围内随机生成一组坐标，
 *   并通过 on_snake() 验证确保食物不会出现在蛇身上。
 *   若生成在蛇身上则循环重试，直到找到空闲位置。
 *
 * 参数：
 *   a — 输出参数，指向存储食物 X 坐标的变量（指针）
 *   b — 输出参数，指向存储食物 Y 坐标的变量（指针）
 *
 * 返回值：无
 *
 * 注意：
 *   当蛇身几乎占满整个区域时，此函数可能循环多次，
 *   但游戏设计上蛇身可占满全部 1600 格，届时将陷入死循环。
 */
void generate_food_position(int* a, int* b)
{
	int x,y;
	do{
		x = rand()%PLAYGROUND_SIZE;
		y = rand()%PLAYGROUND_SIZE;
	} while(on_snake(x,y));  /* 如果生成在蛇身上则重新生成 */
	*a = x;
	*b = y;
}

/*
 * run_snake — 贪吃蛇游戏主循环
 *
 * 功能：
 *   初始化蛇身（长度=1，位置在正中央）和第一个食物，
 *   然后进入游戏主循环，每轮迭代依次执行：
 *     1. 绘制当前游戏画面
 *     2. 获取玩家方向键输入
 *     3. 检测 ESC/退出信号
 *     4. 蛇身移位（从尾部向头部逐节复制）
 *     5. 根据方向移动蛇头
 *     6. 边界碰撞检测
 *     7. 自身碰撞检测
 *     8. 食物碰撞检测与身体增长
 *
 * 参数：无
 *
 * 返回值：无（游戏结束后通过 return 退出）
 *
 * 退出条件：
 *   - 玩家按 ESC 键
 *   - 蛇头撞到墙壁（超出 PLAYGROUND_SIZE 边界）
 */
void run_snake()
{
	/* 初始化：蛇长=1，蛇头在正中央，随机生成第一个食物 */
	snake_lengh=1;
	snake_block_x[0]=PLAYGROUND_SIZE/2;
	snake_block_y[0]=PLAYGROUND_SIZE/2;

	generate_food_position(&food_pos_x, &food_pos_y);

	while (1)
	{
		int key;

		/* 1. 绘制当前画面 */
		display_snake();

		/* 2. 获取玩家按键 */
		key = getcontrol();

		/* 3. 按 ESC 或出错则退出游戏 */
		if(key==-1) return;

		/* 4. 蛇身移位：从尾部向头部，每节继承前一节的位置
		      注意从 snake_lengh 开始循环（多移一节），
		      这样吃到食物时只需 snake_lengh++ 即可自动增长 */
		for(int i=snake_lengh; i>0; i--)
		{
			snake_block_x[i]=snake_block_x[i-1];
			snake_block_y[i]=snake_block_y[i-1];
		}

		/* 5. 根据方向键移动蛇头 */
		switch(key)
		{
			case UP:    snake_block_y[0]--; break;
			case DOWN:  snake_block_y[0]++; break;
			case LEFT:  snake_block_x[0]--; break;
			case RIGHT: snake_block_x[0]++; break;
		}

		/* 6. 边界碰撞检测：蛇头超出游戏区域则游戏结束 */
		if (snake_block_x[0]<0) return;
		if (snake_block_y[0]<0) return;
		if (snake_block_x[0]>=PLAYGROUND_SIZE) return;
		if (snake_block_y[0]>=PLAYGROUND_SIZE) return;

		/* 7. 吃食物检测：蛇头碰到食物则蛇身增长并生成新食物 */
		if ((snake_block_x[0] == food_pos_x) &&
			 (snake_block_y[0] == food_pos_y))
		{
			snake_lengh++;
			generate_food_position(&food_pos_x, &food_pos_y);
		}
	}
}

/*
 * main — 程序入口
 *
 * 功能：
 *   初始化随机数种子以确保每次运行时食物位置不同，
 *   然后调用 run_snake() 启动游戏。
 *
 * 参数：无
 *
 * 返回值：始终返回 0（程序正常结束）
 */
int main(void)
{
	srand(time(NULL));  /* 初始化随机数种子，确保每次运行食物位置不同 */
	run_snake();
}
