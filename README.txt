Aaron Asaph Kupferman | aakup@seas.upenn.edu
ESE-350: Embedded Systems | Spring '20
Replacement Lab 4 Redux: GAMUL8OR
4/10/20

Files:
-emul8.c
-gamul.c
-gamul.h
-Makefile
-Readme.txt

Utilized Files:
-beep.aiff
-{All of the provided Game Files}

Compilation:
'make' or 'make emulator' to compile and create an executable file called 'emulator'.
'make clean' will remove any previos versions of te 'emulator' executable you still might have in the folder.
Note on compilation: I added the '-w' flag to remove all reported errors, as it didn't affect the running of the proram (non memory leaks and expected program output). However, to observe the error reporting, remove the '-w' flag from the 'emulator' target. 

To Run a Game:
./emulator FILENAME

Breakdown of specifications:
"gamul.c" contains the function used for loading the GAMUL8OR files into the emulator.
"gamul.h" contains the headers for the functions in "gamul.c," in addition to the struct used throughout the program to store the "state" of the simulated system on each cycle/opcode.
"emul8.c" is the main file, which handles majority of the computation associated with this project. First, in the main function, we initialize a struct containing all of the associated values and data structures to simulate this emulator. Then, we traverse through simualted memory (done using an 8-bit array), executing opcodes on each loop within the idle() function. The opcode handling is all conducted within one funciton, called next(). 

Error Handling.
The following errors are handled throughout the code, and will cause in program termination and a display of a helpful error message.
Input File Errors:
-Did not run emulator with input file.
-Attempted to execute emulator with nonexistent file.
-Attempted to load incompatible file size into GAMUL8OR.
-Attempted to load in NULL file.
Runtime Errors:
-Invalid opcode in file.
-Attempted jump to invalid destination.
-Attempted call for invalid subroutine.
-Attempt to read from invalid address.

