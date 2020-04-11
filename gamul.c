#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gamul.h"

/*    FUNCTION: load_file
 *  -------------------
 *    Loads the given program/game
 *    PARAMETERS:
 *  file_name: name of file to be loaded
 *  buffer: system memory which will hold program
 *    RETURNS: 0 if successful, -1 if file error
 */
int load_file(char *file_name, unsigned char *buffer)
{
    FILE *file;
    int file_size;

    // open file stream in binary read-only mode
    file = fopen(file_name, "rb");    //man 3 fopen
   
    fseek(file, 0, SEEK_END);    //man 3 fseek

    file_size = ftell(file);    //man 3 ftell

    rewind(file);                //man 3 rewind
   
    fread(buffer, 1, file_size, file);    //man 3 fread
    return 0;

}


/*    FUNCTION: display_func
 *  ----------------------
 *    Sample function that displays a pixel on the screen
 *    PARAMETERS:
 *  gamer: architecture to be emulated, defined in gamul.h
 *    RETURNS: none
 */
void display_func(gamul8 *gamer)
{
    gamer->display[40][40] = 1;
    gamer->display[10][30] = 0;
}

void load(char *filename, state ourSystem){
    unsigned short opcode = 0;
    unsigned short opcode_interim = 0;
    FILE *theFile;
    if (!(theFile = fopen(filename, "rb"))){
        printf("No such file.\n");
        exit(0);
    }
    theFile = fopen(filename, "rb");
    int file_size = ftell(theFile);
    if (file_size > (0xFFF-0x200)){
        printf("Too big of a file; invalid write to ROM\n");
        exit(1);
    }
    if (theFile == NULL){
        printf("Couldn't open file\n");
    }
    unsigned int i = 0x200;
    while (fread(&opcode, sizeof(unsigned short), 1, theFile) == 1){
        opcode_interim = opcode & 0x00FF; // top two bytes.
        //opcode_interim = opcode_interim << 8;
        opcode = opcode & 0xFF00;
        opcode = opcode >> 8; // bottom two bytes
        //opcode = opcode | opcode_interim;
        ourSystem.Mem[i] = opcode_interim;
        i++;
        ourSystem.Mem[i] = opcode;
        i++;
    }
    fclose(theFile);
}
