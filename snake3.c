#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#define ROWS 10
#define COLS 10
#define START_SPEED 270 // will be slightly faster than this actually
#define MAX_SPEED 0.8 // multiplicative

// will make a 20x20 int grid. Snake head is a 1, body grows in number until max age. 0 is background, -1 is apple.
// With credits to ChatGPT (it taught me how to make milliseconds work)

// v2 makes the snake go faster as you progress
// v3 adds arrow key support

void printDisplay(int ints[ROWS][COLS])
{
	int i, j;
	for (i=0; i<ROWS; i++)
	{
		for (j=0; j<COLS; j++)
		{
			// coord is (i,j)
			if (ints[i][j] == -1)
			{
				printf("a");
			} else if (ints[i][j] == 0)
			{
				printf(".");
			} else if (ints[i][j] == 1)
			{
				printf("Q");
			} else
			{
				printf("O");
			}
			printf(" ");
		}
		printf("\n");
	}
}

void runAGameTick(int ints[ROWS][COLS], int *age, int *direction, int *oldDirection, int *headRow, int *headCol, int* isSnakeAlive)
{
	//update the oldDirection
	*oldDirection = *direction;
	
	int didWeEatSomething = 0;
	int i, j;
	
	// age up the board and remove the last bits of the tail
	for (i=0; i<ROWS; i++)
	{
		for (j=0; j<COLS; j++)
		{
			if (ints[i][j] > 0)
			{			
				ints[i][j]++;
			}
			
			if (ints[i][j] > *age)
			{
				ints[i][j]=0;
			}
		}
	}
	
	// move the head (and check for a collision)
	// so we have a variable for the location of the head
	if (*direction%2==0)
	{
		// left or right
		// apple check
		if (ints[*headRow][*headCol+*direction-1]==-1)
		{
			didWeEatSomething=1;
			*age=*age+1;
		}
		// self-collision check
		if (ints[*headRow][*headCol+*direction-1]>0)
		{
			*isSnakeAlive=0;
		}
		ints[*headRow][*headCol+*direction-1]=1;
		*headCol=*headCol+*direction-1;
	}
	else {
		// up or down
		// apple check
		if (ints[*headRow+*direction-2][*headCol]==-1)
		{
			didWeEatSomething=1;
			*age=*age+1;
		}
		// self-collision check
		if (ints[*headRow+*direction-2][*headCol]>0)
		{
			*isSnakeAlive=0;
		}
		ints[*headRow+*direction-2][*headCol]=1;
		*headRow=*headRow+*direction-2;
	}
	
	// now i guess we gotta put the apple back if applicable
	int appRow = *headRow;
	int appCol = *headCol;
	if (didWeEatSomething == 1)
	{
		// choose a spot for the apple
		while ((ints[appRow][appCol] != 0) && (*age != ROWS*COLS+1))
		{
			appRow = rand()%ROWS;
			appCol = rand()%COLS;
			//printf("age is %d\n", *age);
		}
		ints[appRow][appCol]=-1;
		if (*age == ROWS*COLS+1)
		{
			*isSnakeAlive = 2;
		}
	}
	
	// the snake should die if it goes off the screen. It already handles up/down (out of range) so just check the collision
	if (*headCol < 0 || *headCol > COLS-1)
	{
		*isSnakeAlive=0;
	}
	
	// and then display the board if you ain't dead
	if (*isSnakeAlive == 1)
	{
		printDisplay(ints);
		//printf("score is %d\nThe SnakeAlive variable is %d\n", *age, *isSnakeAlive);
	}
}

int main(void)
{
	// initialise a few things
	int age=3;
	// older than that and it gets removed
	
	int grid[ROWS][COLS]={0};
	int tick=0;
	int headRow = 0;
	int headCol = age;
	grid[(int)(ROWS/2)][(int)(ROWS*0.7)]=-1;		// apple
	grid[headRow][headCol]=1;
	grid[headRow][headCol-1]=2;
	grid[headRow][headCol-2]=3;
	
	int direction = 2;
	int letter = 100;
	int oldDirection = 0;
	// 0 is left (a), 1 is up (w), 2 is right (d), 3 is down (s)
	
	int isSnakeAlive=1;
	
	// and do some maths for when the snake speeds up
	double speed = START_SPEED;
	double finalSpeed = START_SPEED*MAX_SPEED;
	int totalSteps = ROWS*COLS;
	double perStep = (speed-finalSpeed)/totalSteps;
	
	
	
	// now we need to run the gameticks until the snake dies
	// so we need to set up the timer
	clock_t currentTime = clock();
	clock_t newTime;
	double elapsedTime = 0;
	
	while (isSnakeAlive==1)
	{
		//(get a direction)
		if (_kbhit())
		{
			letter = _getch();
		}
		// we'll go wasd
		//	w	a	s	d
		//	119	97	115	100
		if ((letter == 97 || letter == 75) && oldDirection!=2)
		{
			direction=0;
		}
		if ((letter == 119 || letter == 72) && oldDirection!=3)
		{
			direction=1;
		}
		if ((letter == 100 || letter == 77) && oldDirection!=0)
		{
			direction=2;
		}
		if ((letter == 115 || letter == 80) && oldDirection!=1)
		{
			direction=3;
		}
		// get the Time
		newTime = clock();
		elapsedTime = (newTime - currentTime) * 1000 / CLOCKS_PER_SEC;
		
		// and only update if the time has changed
		if (elapsedTime > speed)
		{
			// update the times
			currentTime=newTime;
			
			// increase the counter
			tick++;

			// now we need to run a game tick
			runAGameTick(grid, &age, &direction, &oldDirection, &headRow, &headCol, &isSnakeAlive);

			printf("\n");
			
			// and finally set the speed for the next tick
			speed = START_SPEED - age*perStep;
		}
	}
	
	if (isSnakeAlive == 0)
	{
		printf("you died :(\nYour score is %d\nYou took %d steps.", age, tick);
	}
	
	if (isSnakeAlive == 2)
	{
		printf("you won :)\nYour score is %d\nYou took %d steps.", age, tick);
	}
	
	return 0;
}