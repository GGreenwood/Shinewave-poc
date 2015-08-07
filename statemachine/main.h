#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef struct Color Color;
typedef struct State State;
typedef struct Exit Exit;

typedef uint8_t (*Color_fn)(uint8_t counter);

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


static const uint8_t NUM_STATES = 2;

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


static const uint8_t NUM_COLORS = 3;

Color_fn colorLookup[] = {&zero, &linear, &max};

#endif
