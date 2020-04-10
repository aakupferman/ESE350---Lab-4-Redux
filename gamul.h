#include <string.h>
#include <stdint.h>

#ifndef GAMUL_H
#define GAMUL_H

#define SCREEN_WIDTH	64
#define SCREEN_HEIGHT	32
#define FONTSET_SIZE	80

typedef struct gamul8 {
	unsigned char display[SCREEN_WIDTH][SCREEN_HEIGHT];
} gamul8;

typedef struct state {
	// Memory
	uint8_t *Mem;
	// Registers
	uint8_t* Registers;
	// Emulator State
	uint16_t I;
	uint16_t PC;
	uint16_t* Stack;
	int SP;
	uint8_t Delay;
	uint8_t Sound;
	int *key;
	unsigned char isPressed;
} state;


void load(char *filename, state ourSystem);
extern void display_func(gamul8 *gamer);

#endif