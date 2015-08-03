#include <stdio.h>
#include <stdint.h>

typedef struct Color Color;
typedef struct State State;
typedef struct Exit Exit;

typedef uint8_t (*Color_fn)(uint8_t counter);
//typedef uint8_t (*Condition)(uint8_t counter, uint8_t parameter);

// TODO: Find out a way to do multiple exit conditions
struct State {
	Color_fn red, green, blue;
	uint8_t hold, max;
	State *nextState;
	Exit *exit;
};

struct Exit {
	uint8_t byte;
	uint8_t mask;
	State *exitState;
};

struct Color {
	uint8_t red, green, blue;
};

// Simple color functions
uint8_t linear(uint8_t counter) {
	return counter;
}
uint8_t zero(uint8_t counter) {
	return 0;
}
uint8_t max(uint8_t counter) {
	return 255;
}

// Button checks
uint8_t buttonBuffer[7] = {0,0,0,0,0,0,0};
uint8_t checkButton(uint8_t byte, uint8_t mask) {
	if(buttonBuffer[byte] & mask) 
		return 1;
	else
		return 0;
}

// TODO: Add exit condition processing
State *nextFrame(State *st, uint8_t* counter) {
	if(*counter >= st->max) {
		*counter = 0;
		return st->nextState;
	} else if(*counter >= st->hold && checkButton(st->exit->byte, st->exit->mask)) {
		printf("B pressed!\n");
		*counter = 0;
		return st->exit->exitState;
	} else {
		*counter = *counter + 1;
		return st;
	}
}

Color getColor(State *st, uint8_t counter) {
	uint8_t red = st->red(counter);
	uint8_t green = st->green(counter);
	uint8_t blue = st->blue(counter);
	Color col = {red, green, blue};
	return col;
}

void printState(State *st, uint8_t counter) {
	Color col = getColor(st, counter);
	printf("counter: %u\tred: %u\t green: %u\tblue: %u\n", counter, col.red, col.green, col.blue);
}

State idleState;
State shineState;
Exit shineExit;

uint8_t counter;

int main(int arc, char **argv)
{
	idleState = {&linear, &zero, &zero, 0, 10, &idleState, &shineExit};
	shineState = {&zero, &zero, &max, 2, 5, &idleState, &shineExit};
	shineExit = {1, 0b00000010, &shineState};

	printf("Entering main\n");

	State *curState = &idleState;
	counter = 0;

	// Progress through ten idle frames
	int i;
	for(i = 0; i < 10; i++) {
		curState = nextFrame(curState, &counter);
		printState(curState, counter);
	}

	// Shine for five frames to test hold
	buttonBuffer[1] = 0b00000010;
	curState = nextFrame(curState, &counter);
	printState(curState, counter);
	curState = nextFrame(curState, &counter);
	printState(curState, counter);
	curState = nextFrame(curState, &counter);
	printState(curState, counter);
	curState = nextFrame(curState, &counter);
	printState(curState, counter);
	curState = nextFrame(curState, &counter);
	printState(curState, counter);
	buttonBuffer[1] = 0b00000000;

	// Wait ten more frames
	for(i = 0; i < 10; i++) {
		curState = nextFrame(curState, &counter);
		printState(curState, counter);
	}

	return 0;
}
