emulator: 
	gcc emul8.c gamul.c -o emulator -lGL -lGLU -lglut -std=c99 -Wall -lm -w
clean: 
	rm emulator