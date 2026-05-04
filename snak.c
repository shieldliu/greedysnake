#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termio.h>
#include <stdlib.h>
#include <time.h>

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
	exit(0);
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

#define PLAYGROUND_SIZE 16
char buffer[PLAYGROUND_SIZE][PLAYGROUND_SIZE];



int snake_block_x[PLAYGROUND_SIZE*PLAYGROUND_SIZE];
int snake_block_y[PLAYGROUND_SIZE*PLAYGROUND_SIZE];
int snake_lengh;
int food_pos_x;
int food_pos_y;

void display_snake()
{
	memset(buffer, ' ', sizeof(buffer));
	for(int i=0; i<snake_lengh; i++)
	{
		int x = snake_block_x[i];
		int y = snake_block_y[i];
		buffer[y][x] = 'X';
	}
	buffer[food_pos_y][food_pos_x]='O';
	show_to_screen(buffer, PLAYGROUND_SIZE,PLAYGROUND_SIZE);
}


int on_snake(int x, int y)
{
	//0: not on snake
	//1: on snake
	for(int i=0; i<snake_lengh;i++)
	{
		if(x==snake_block_x[i] && y==snake_block_y[i])
			return 1;
	}
	return 0;
}

void generate_food_position(int* a, int* b)
{
	int x,y;
	do{
		x = rand()%PLAYGROUND_SIZE;
		y = rand()%PLAYGROUND_SIZE;
	} while(on_snake(x,y));
	*a = x;
	*b = y;
}




void run_snake()
{
	snake_lengh=1;
	snake_block_x[0]=PLAYGROUND_SIZE/2;
	snake_block_y[0]=PLAYGROUND_SIZE/2;

	generate_food_position(&food_pos_x, &food_pos_y);

	//Run snake
	while (1)
	{
		int key;
		display_snake();
		key = getcontrol();
		for(int i=snake_lengh; i>0; i--)
		{
			snake_block_x[i]=snake_block_x[i-1];
			snake_block_y[i]=snake_block_y[i-1];
		}
		switch(key)
		{
			case UP: snake_block_y[0]=snake_block_y[0]-1;break;
			case DOWN:snake_block_y[0]=snake_block_y[0]+1;	break;
			case LEFT:snake_block_x[0]=snake_block_x[0]-1;	break;
			case RIGHT:	snake_block_x[0]=snake_block_x[0]+1;break;
		}
		if (snake_block_x[0]<0) return;
		if (snake_block_y[0]<0) return;
		if (snake_block_x[0]>=PLAYGROUND_SIZE) return;
		if (snake_block_y[0]>=PLAYGROUND_SIZE) return;
		if ((snake_block_x[0] == food_pos_x) &&
			 (snake_block_y[0] == food_pos_y))
		{
			//Increase snak
			snake_lengh++;
			generate_food_position(&food_pos_x, &food_pos_y);
		}
		
	}

}

main()
{
	run_snake();
}
