#include <stdio.h>
#include <stdint.h>

typedef struct Color Color;
typedef struct State State;
typedef struct Exit Exit;

typedef uint8_t (*Color_fn)(uint8_t counter);

enum STICK_POSITIONS {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

struct State {
	uint8_t red, green, blue;
	uint8_t hold, max;
	uint8_t nextState;
	uint8_t exits[8];
};

struct Exit {
	uint16_t digitalMask;
	uint8_t stick;
};

struct Color {
	uint8_t red, green, blue;
};

// Simple color functions
uint8_t zero(uint8_t counter) {
	return 0;
}
uint8_t linear(uint8_t counter) {
	return counter;
}
uint8_t max(uint8_t counter) {
	return 255;
}

static const State stateLookup[] = {
	{	// Idle
		1,	1,	1,
		0,	10,	0,
		1,	0,	0,	0,	0,	0,	0,	0	// Can exit into shine
	}, {	// Shine
		0,	0,	2,
		3,	5,	0,
		0,	0,	0,	0,	0,	0,	0,	0	// No other exits, must timeout into idle
	}
};

static const Exit exitLookup[] = {
	{ 0,	0 },
	{ 0b0000000000000010,	0b00000100 }
};

Color_fn colorLookup[] = {&zero, &linear, &max};


// Button checks
uint8_t checkExit(uint8_t exit, uint16_t buttons, uint8_t stick) {
	// If the C-Stick is in the correct position
	if(stick & 0xf0)
		return 1;

	Exit ex = exitLookup[exit];
	// If the D-Stick is correctly in neutral, or in the correct direction
	if(!(ex.stick & 0x0f) & !(stick & 0x0f) || (ex.stick & stick & 0x0f))
		// And the correct button is pressed
		if(buttons & ex.digitalMask) 
			return 1;

	return 0;
}

// Processes the next frame, given the current controller state
uint8_t nextFrame(uint8_t st, uint8_t* counter, uint16_t buttons, uint8_t stick) {
	// Check states exit conditions
	if(*counter >= stateLookup[st].hold) {
		// Loop through eight possible exits
		for(uint8_t i = 0; i < 8; i++) {
			// Lookup the current exit being evaluated
			uint8_t exit = stateLookup[st].exits[i];
			// If exit is zero, there are no more valid exit conditions
			if(exit == 0) 
				break;

			// Check if the given exit has its conditions met
			if(checkExit(exit, buttons, stick)) {
				*counter = 0;
				return exit;
			}
		}
	}

	// Skip to nextState if held for too long
	if(*counter >= stateLookup[st].max) {
		*counter = 0;
		return stateLookup[st].nextState;
	} 

	// Otherwise, increment and continue
	*counter = *counter + 1;
	return st;
}

// Gets a color from current state and counter value
Color getColor(uint8_t st, uint8_t counter) {
	// Lookup the current state and color function ID, then call it with counter as a parameter
	uint8_t red = colorLookup[stateLookup[st].red](counter);
	uint8_t green = colorLookup[stateLookup[st].green](counter);
	uint8_t blue = colorLookup[stateLookup[st].blue](counter);
	Color col = {red, green, blue};
	return col;
}

// Debug function, prints current status
void printState(uint8_t st, uint8_t counter) {
	Color col = getColor(st, counter);
	printf("state: %u\tcounter: %u\tred: %u\t green: %u\tblue: %u\n", st, counter, col.red, col.green, col.blue);
}

uint8_t counter;

int main(int arc, char **argv)
{
	printf("Entering main\n");

	// Initialize in the idle state
	uint8_t st = 0;

	// Progress through ten idle frames
	int i;
	for(i = 0; i < 10; i++) {
		st = nextFrame(st, &counter, 0, 0);
		printState(st, counter);
	}

	// Shine for five frames to test hold
	for(i = 0; i < 10; i++) {
		st = nextFrame(st, &counter, 0xFFFF, 0x04);
		printState(st, counter);
	}

	// Wait ten more frames
	for(i = 0; i < 10; i++) {
		st = nextFrame(st, &counter, 0, 0);
		printState(st, counter);
	}

	return 0;
}
