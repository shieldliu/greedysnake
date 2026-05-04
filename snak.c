#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>

#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4
#define MAX_SNAKE_LENGTH (PLAYGROUND_SIZE * PLAYGROUND_SIZE)

int getcontrol(void)
{
     struct termios tm, tm_old;
     int fd = 0, ch;

     if (tcgetattr(fd, &tm) < 0) {
          return -1;
     }

     tm_old = tm;
     cfmakeraw(&tm);
     if (tcsetattr(fd, TCSANOW, &tm) < 0) {
          return -1;
     }

     ch = getchar();
     if (ch == 27) { // Escape sequence (arrow keys)
          getchar(); // skip '['
          ch = getchar();
          if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {
               return -1;
          }
          if(ch == 'A') return UP;
          if(ch == 'B') return DOWN;
          if(ch == 'C') return RIGHT;
          if(ch == 'D') return LEFT;
          return -1;
     }

     if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {
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
     return -1;
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

struct position
{
	int x;
	int y;
};

struct position snak_body[PLAYGROUND_SIZE * PLAYGROUND_SIZE];
int snak_length=1;
struct position food={0,0};

void display_snak(int x, int y);
void display_whole_snak(struct position *blocks, int snak_length, struct position food);
struct position gen_random_food(void);

void display_snak(int x, int y)
{
	memset(buffer, ' ', sizeof(buffer));
	buffer[y][x]='X';
	show_to_screen(buffer, PLAYGROUND_SIZE, PLAYGROUND_SIZE);
}

void display_whole_snak(struct position *blocks, int snak_length, struct position food)
{
	memset(buffer, ' ', sizeof(buffer));
	for(int i=0;i<snak_length;i++)
	{
		int x=blocks[i].x;
		int y = blocks[i].y;
		buffer[y][x]='X';
	}
	buffer[food.y][food.x]='O';
	show_to_screen(buffer, PLAYGROUND_SIZE, PLAYGROUND_SIZE);
}

void run_snak()
{
	snak_length=1;
	snak_body[0].x=PLAYGROUND_SIZE/2;
	snak_body[0].y=PLAYGROUND_SIZE/2;
	food = gen_random_food();
	display_whole_snak(snak_body, snak_length, food);

	while (1)
	{
		int key = getcontrol();
		if(key < 0) continue;

		// Save old head position
		struct position old_head = snak_body[0];

		// Calculate new head position
		struct position new_head = old_head;
		switch(key)
		{
			case UP:
				new_head.y = new_head.y - 1;
				break;
			case DOWN:
				new_head.y = new_head.y + 1;
				break;
			case LEFT:
				new_head.x = new_head.x - 1;
				break;
			case RIGHT:
				new_head.x = new_head.x + 1;
				break;
		}

		// Check if hit wall
		if(new_head.x < 0 || new_head.x >= PLAYGROUND_SIZE ||
		   new_head.y < 0 || new_head.y >= PLAYGROUND_SIZE)
			break;

		// Check if hit body
		for(int i=0; i<snak_length; i++)
		{
			if(snak_body[i].x == new_head.x && snak_body[i].y == new_head.y)
				break;
		}

		// Move body: shift each block to previous block's position
		for(int i=snak_length-1; i>=0; i--)
		{
			snak_body[i+1] = snak_body[i];
		}
		snak_body[0] = new_head;

		// Check if eats food
		if(new_head.x == food.x && new_head.y == food.y)
		{
			snak_length++;
			food = gen_random_food();
		}

		// Check if hit body after moving
		for(int i=1; i<snak_length; i++)
		{
			if(snak_body[i].x == new_head.x && snak_body[i].y == new_head.y)
			{
				printf("\nGame Over! You hit yourself!\n");
				return;
			}
		}

		display_whole_snak(snak_body, snak_length, food);
	}
	printf("\nGame Over! Final length: %d\n", snak_length);
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
	srand(time(NULL));
	struct position p;
	do {
		p.x = rand() % PLAYGROUND_SIZE;
		p.y = rand() % PLAYGROUND_SIZE;
		// Check if food is not on snake body
		int on_body = 0;
		for(int i=0; i<snak_length; i++)
		{
			if(snak_body[i].x == p.x && snak_body[i].y == p.y)
			{
				on_body = 1;
				break;
			}
		}
		if(!on_body) break;
	} while(1);
	return p;
}

int main()
{
	run_snak();
	return 0;
}
