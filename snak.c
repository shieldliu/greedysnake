#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termio.h>

#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4

int getcontrol(void)
{
     struct termios tm, tm_old;
     int fd = 0, ch;

     if (tcgetattr(fd, &tm) < 0) {//保存现在的终端设置
          return -1;
     }

     tm_old = tm;
     cfmakeraw(&tm);//更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
     if (tcsetattr(fd, TCSANOW, &tm) < 0) {//设置上更改之后的设置
          return -1;
     }

     ch = getchar();
     if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {//更改设置为最初的样子
          return -1;
     }

     if(ch == 'A' || ch =='a')
	     return LEFT;
     if(ch == 'D' || ch =='d')
	     return RIGHT;
     if(ch == 'W' || ch =='w')
	     return UP;
     if(ch == 'S' || ch =='s')
	     return DOWN;
}

void show_to_screen(void *buffer, int width, int height)
{
	char * contents = (char*)buffer;
	// Show a segment line
	for(int i=0; i<width+2; i++)
	{
		printf("=");
	}
	printf("\n");

	// Print the contents
	for(int j=0; j<height; j++)
	{
		//This should show on line of contents
		char * line_of_contents = contents + j*width;
		printf("|");
		for(int i=0; i<width; i++)
		{
			printf("%c", line_of_contents[i]);
		}
		printf("|\n");
	}
	// Show a segment line
	for(int i=0; i<width+2; i++)
	{
		printf("=");
	}
	printf("\n");
}

#define PLAYGROUND_SIZE 32
char buffer[PLAYGROUND_SIZE][PLAYGROUND_SIZE];
char snak_body[PLAYGROUND_SIZE^2];
int snak_length=1;
struct position food={0,0};

void display_snak(int x, int y)
{
	memset(buffer, ' ', sizeof(buffer));
	buffer[y][x]='X';
	show_to_screen(buffer, PLAYGROUND_SIZE,PLAYGROUND_SIZE);
}

struct position
{
	int x;
	int y;
};
void display_whole_snak(struct *blocks, int snak_lenth, struct food)
{
	memset(buffer, ' ', sizeof(buffer));
	for(int i=0;i<snak_lenth;i++)
	{
		int x=blocks[i].x;
		int y = blocks[i].y;
		buffer[y][x]='X';
	}
	buffer[food.y][food.x]='O';
	show_to_screen(buffer, PLAYGROUND_SIZE,PLAYGROUND_SIZE);
}

void run_snak()
{
	snak_length=1;
	snak_body[0].x=PLAYGROUND_SIZE/2;
	snak_body[0].y=PLAYGROUND_SIZE/2;
	//Run snak
	while (1)
	{
		int key = getcontrol();
		//First treate the head of snak
		struct position *head=snak_body;
		switch(key)
		{
			case UP:
				y=y-1;
				break;
			case DOWN:
				y=y+1;
				break;
			case LEFT:
				x=x-1;
				break;
			case RIGHT:
				x=x+1;
				break;
		}
		//update each block to the previous position
		//then update head postion
		//check if it eats food
		//erase the last tail if not eat food
		//Do not erase tail means it grows
		//Check if it hit wall or the body, then exit;
	}

}

void test_snak()
{
	int x=PLAYGROUND_SIZE/2;
	int y=PLAYGROUND_SIZE/2;
	display_snak(x,y);

	while (1)
	{
		int key = getcontrol();
		switch(key)
		{
			case UP:
				y=y-1;
				break;
			case DOWN:
				y=y+1;
				break;
			case LEFT:
				x=x-1;
				break;
			case RIGHT:
				x=x+1;
				break;
		}
		//Check if snak is out of playground
		if( x<0 || x>=PLAYGROUND_SIZE)
			return;
		if( y<0 || y>=PLAYGROUND_SIZE)
			return;

		display_snak(x,y);
	}
}

struct position gen_random_food(void)
{
	//randomly generate a food and return the position
	//and loop to generate a new one unless it is not on snak body
	
}

main()
{
	run_snak();
}
