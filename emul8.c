/* ESE 350, Spring 2018
 * ------------------
 * Starter code for Lab 4
 *
 * The main purpose is to provide all the FreeGLUT
 * code required to handle display and input.
 * This uses the FreeGLUT libraries installed in your VM.
 * If you choose to use a different set of third party libraries,
 * please consult a TA.
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include "gamul.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

// font set for rendering
const unsigned char fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,        // 0
    0x20, 0x60, 0x20, 0x20, 0x70,        // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,        // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,        // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,        // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,        // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,        // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,        // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,        // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,        // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,        // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,        // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,        // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,        // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,        // E
    0xF0, 0x80, 0xF0, 0x80, 0x80        // F
};

//architecture we are emulating, refer gamul.h
gamul8 gamer;
int global_counter = 0;
char binary[8] = {0};
//Initialize Everything
// Declare the Memory Array
uint8_t memory[4096];
//uint8_t *memPtrH = memory; // point to the first element of memory array.
//memset(memPtrH, 0, (4096*sizeof(uint8_t)));
//memPtr = memPtr + 512;
int isPressed = 0;
state ourSystem;
uint16_t Stack[16] = {0};
//uint16_t Stack[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
uint8_t Registers[16] = {0};
//uint8_t Registers[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
int keys[16] = {0};
//unsigned char iskey = 0;
void initState(){
    // load the fontset into memory, starting at 0 up to 80
    for (int i = 0; i < (FONTSET_SIZE + 1); i++){
        memory[i] = fontset[i];
    }
    ourSystem.Mem = memory;
    ourSystem.Registers = Registers;
    ourSystem.I = 0x0200; // 16-bit
    ourSystem.PC = 0x0200; // 16-bit
    ourSystem.SP = -1;
    ourSystem.Stack = Stack;
    ourSystem.Delay = 0;
    ourSystem.Sound = 0;
    ourSystem.key = keys;
    ourSystem.isPressed = 0;
}
// method to draw a single pixel on the screen
void draw_square(float x_coord, float y_coord);

// update and render logic, called by glutDisplayFunc
void render();

// idling function for animation, called by glutIdleFunc
void idle();

// initializes GL 2D mode and other settings
void initGL();

// function to handle user keyboard input (when pressed)
void your_key_press_handler(unsigned char keyPress, int x, int y);

// function to handle key release
void your_key_release_handler(unsigned char keyPress, int x, int y);


/*    FUNCTION: main
 *  --------------
 *    Main emulation loop. Loads ROM, executes it, and draws to screen.
 *  You may also want to call here the initialization function you have written
 *  that initializes memory and registers.
 *    PARAMETERS:
 *                argv: number of command line arguments
 *                argc[]: array of command line arguments
 *    RETURNS:    usually 0
 */
int main(int argc, char *argv[])
{
    // seed random variable for use in emulation
    srand(time(NULL));

    // initialize GLUT
    glutInit(&argc, argv);

    // initialize display and window
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("ESE 519 - Gamul8r");

    // initialize orthographic 2D view, among other things
    initGL();
   
    // handle key presses and releases
    glutKeyboardFunc(your_key_press_handler);
    glutKeyboardUpFunc(your_key_release_handler);

    // GLUT draw function
    glutDisplayFunc(render);

    //Initialize Everything
    initState();
    if (argc < 2){
        printf("Please load in a game\n");
        exit(1);
    }
    load(argv[1], ourSystem);
    //FILE theFile2;
    //theFile2 = fopen(argv[2], "wb");
    //for (unsigned short i = 0x200; i < (0x200 + 40); i++){
   //     printf("%s\n", );
    //}
   // printState();
    // GLUT idle function, causes screen to redraw
    glutIdleFunc(idle);
    // main loop, all events processed here
    //unsigned short PC = 0x200;
    glutMainLoop();
    //if (global_counter == 150){
    //    fclose(theFile2);
    //}
    return 0;
}

/*    FUNCTION: draw_square
 *  ----------------------
 *    Draws a square. Represents a single pixel
 *  (Up-scaled to a 640 x 320 display for better visibility)
 *    PARAMETERS:
 *    x_coord: x coordinate of the square
 *    y_coord: y coordinate of the square
 *    RETURNS: none
 */
void draw_square(float x_coord, float y_coord)
{
    // draws a white 10 x 10 square with the coordinates passed

    glBegin(GL_QUADS);  //GL_QUADS treats the following group of 4 vertices
                        //as an independent quadrilateral

        glColor3f(1.0f, 1.0f, 1.0f);     //color in RGB
                                        //values between 0 & 1
                                        //E.g. Pure Red = (1.0f, 0.0f, 0.0f)
        glVertex2f(x_coord, y_coord);            //vertex 1
        glVertex2f(x_coord + 10, y_coord);        //vertex 2
        glVertex2f(x_coord + 10, y_coord + 10);    //vertex 3
        glVertex2f(x_coord, y_coord + 10);        //vertex 4

        //glVertex3f lets you pass X, Y and Z coordinates to draw a 3D quad
        //only if you're interested

    glEnd();
}

/*    FUNCTION: render
 *  ----------------
 *    GLUT render function to draw the display. Also emulates one
 *    cycle of emulation.
 *    PARAMETERS: none
 *    RETURNS: none
 */
void render()
{

    // clears screen
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
   
    glLoadIdentity();

    // draw a pixel for each display bit
    int i, j;
    for (i = 0; i < SCREEN_WIDTH; i++) {
        for (j = 0; j < SCREEN_HEIGHT; j++) {
            if (gamer.display[i][j] == 1) {
                draw_square((float)(i * 10), (float)(j * 10));
            }
        }
    }

    // swap buffers, allows for smooth animation
    glutSwapBuffers();
}

/*    FUNCTION: idle
 *  --------------
 *    GLUT idle function. Instructs GLUT window to redraw itself
 *    PARAMETERS: none
 *    RETURNS: none
 */
void idle()
{
    //printState();
    next(&gamer);
    if (ourSystem.Sound > 0){
        ourSystem.Sound = ourSystem.Sound - 1;
        system("paplay beep.aiff &> /dev/null &");
    }
    if (ourSystem.Delay > 0){
        ourSystem.Delay = ourSystem.Delay - 1;
    }
    /*
    if (global_counter < 3000){
        printState();
        next(&gamer);
        if (ourSystem.Sound > 0){
            ourSystem.Sound = ourSystem.Sound - 1;
            system("paplay beep.aiff&> /dev/null &");
        }
        if (ourSystem.Delay > 0){
            ourSystem.Delay = ourSystem.Delay - 1;
        }
        global_counter ++;
    }
    */
    glutPostRedisplay();
}

/*    FUNCTION: initGL
 *  ----------------
 *    Initializes GLUT settings.
 *    PARAMETERS: none
 *    RETURN VALUE: none
 */
void initGL()
{
    // sets up GLUT window for 2D drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // clears screen color (RGBA)
    glClearColor(0.f, 0.f, 0.f, 1.f);
}

/*    FUNCTION: your_key_press_handler
 *  --------------------------------
 *    Handles all keypresses and passes them to the emulator.
 *  This is also where you will be mapping the original GAMUL8
 *  keyboard layout to the layout of your choice
 *  Something like this:
 *  |1|2|3|C|        =>        |1|2|3|4|
 *    |4|5|6|D|        =>        |Q|W|E|R|
 *    |7|8|9|E|        =>        |A|S|D|F|
 *    |A|0|B|F|        =>        |Z|X|C|V|
 *    PARAMETERS:
 *    key: the key that is pressed.
 *    x: syntax required by GLUT
 *    y: syntax required by GLUT
 *  (x & y are callback parameters that indicate the mouse location
 *  on the window. We are not using the mouse, so they won't be used,
 *  but still pass them as glutKeyboardFunc needs it.)
 *    RETURNS: none
 *  NOTE: If you intend to handle this in a different manner, you need not
 *  write this function.
 */
void your_key_press_handler(unsigned char keyPress, int x, int y){
    switch(keyPress) {
        case '1':
            ourSystem.key[0x1] = 1;
            //lastKeyPressed = 0x1;
            break;
        case '2':
            ourSystem.key[0x2] = 1;
            //lastKeyPressed = 0x2;
            break;
        case '3':
            ourSystem.key[0x3] = 1;
            //lastKeyPressed = 0x3;
            break;
        case '4':
            ourSystem.key[0xC] = 1;
            //lastKeyPressed = 0xC;
            break;
        case 'q':
            ourSystem.key[0x4] = 1;
            //lastKeyPressed = 0x4;
            break;
        case 'w':
            ourSystem.key[0x5] = 1;
            //lastKeyPressed = 0x5;
            break;
        case 'e':
            ourSystem.key[0x6] = 1;
            //lastKeyPressed = 0x6;
            break;
        case 'r':
            ourSystem.key[0xD] = 1;
            //lastKeyPressed = 0xD;
            break;
        case 'a':
            ourSystem.key[0x7] = 1;
            //lastKeyPressed = 0x7;
            break;
        case 's':
            ourSystem.key[0x8] = 1;
            //lastKeyPressed = 0x8;
            break;
        case 'd':
            ourSystem.key[0x9] = 1;
            //lastKeyPressed = 0x9;
            break;
        case 'f':
            ourSystem.key[0xE] = 1;
            //lastKeyPressed = 0xE;
            break;
        case 'z':
            ourSystem.key[0xA] = 1;
            //lastKeyPressed = 0xA;
            break;
        case 'x':
            ourSystem.key[0] = 1;
            //lastKeyPressed = 0;
            break;
        case 'c':
            ourSystem.key[0xB] = 1;
            //lastKeyPressed = 0xB;
            break;
        case 'v':
            ourSystem.key[0xF] = 1;
            //lastKeyPressed = 0xF;
            break;
        default:

        ;
    }
    isPressed = 1;
}

/*    FUNCTION: your_key_release_handler
 *  --------------------------------
 *    Tells emulator if any key is released. You can maybe give
 *  a default value if the key is released.
 *    PARAMETERS:
 *    key: the key that is pressed.
 *    x: syntax required by GLUT
 *    y: syntax required by GLUT
 *  (x & y are callback parameters that indicate the mouse location
 *  on the window. We are not using the mouse, so they won't be used,
 *  but still pass them as glutKeyboardFunc needs it.)
 *    RETURNS: none
 *  NOTE: If you intend to handle this in a different manner, you need not
 *  write this function.
 */
void your_key_release_handler(unsigned char keyPress, int x, int y){
    switch(keyPress) {
        case '1':
            ourSystem.key[0x1] = 0;
            //lastKeyPressed = 0x1;
            break;
        case '2':
            ourSystem.key[0x2] = 0;
            //lastKeyPressed = 0x2;
            break;
        case '3':
            ourSystem.key[0x3] = 0;
            //lastKeyPressed = 0x3;
            break;
        case '4':
            ourSystem.key[0xC] = 0;
            //lastKeyPressed = 0xC;
            break;
        case 'q':
            ourSystem.key[0x4] = 0;
            //lastKeyPressed = 0x4;
            break;
        case 'w':
            ourSystem.key[0x5] = 0;
            //lastKeyPressed = 0x5;
            break;
        case 'e':
            ourSystem.key[0x6] = 0;
            //lastKeyPressed = 0x6;
            break;
        case 'r':
            ourSystem.key[0xD] = 0;
            //lastKeyPressed = 0xD;
            break;
        case 'a':
            ourSystem.key[0x7] = 0;
            //lastKeyPressed = 0x7;
            break;
        case 's':
            ourSystem.key[0x8] = 0;
            //lastKeyPressed = 0x8;
            break;
        case 'd':
            ourSystem.key[0x9] = 0;
            //lastKeyPressed = 0x9;
            break;
        case 'f':
            ourSystem.key[0xE] = 0;
            //lastKeyPressed = 0xE;
            break;
        case 'z':
            ourSystem.key[0xA] = 0;
            //lastKeyPressed = 0xA;
            break;
        case 'x':
            ourSystem.key[0] = 0;
            //lastKeyPressed = 0;
            break;
        case 'c':
            ourSystem.key[0xB] = 0;
            //lastKeyPressed = 0xB;
            break;
        case 'v':
            ourSystem.key[0xF] = 0;
            //lastKeyPressed = 0xF;
            break;
        default:
            ;
        }
    isPressed = 0;
}
//void next(state *ourSystem, gamul8 *gamer,){

void next(gamul8 *gamer){
    //uint8_t* memPtr = ourSystem.memPtr;
    unsigned short opcode = (ourSystem.Mem[ourSystem.PC] << 8) | (ourSystem.Mem[(ourSystem.PC) + 1]);
    //printf("Now executing opcode: %04X\n", opcode);
    // Bitwise seperate
    unsigned short B1 = (opcode & 0xF000) >> 12;
    unsigned short B2 = (opcode & 0x0F00) >> 8;
    unsigned short B3 = (opcode & 0x00F0) >> 4;
    unsigned short B4 = (opcode & 0x000F);
    unsigned short NNN = (opcode & 0x0FFF);
    unsigned short NN = (opcode & 0x00FF);
    signed long ovfInterimRegS;

    // Begin logical structure
    // 00E0 and 00EE
    if (B1 == 0x0){
        if (opcode == 0x00E0){
            // Clear the screen
            for (int i=0; i < SCREEN_WIDTH; i++){
                for (int j=0; j < SCREEN_HEIGHT; j++){
                    gamer->display[i][j] = 0;
                }
            }
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        else if (opcode == 0x00EE){
            // Returns from a subroutine
            ourSystem.PC = ourSystem.Stack[(ourSystem.SP)];
            ourSystem.SP = ourSystem.SP - 1;
        }
    }
    // 1NNN
    else if (B1 == 0x1){
        // 1NNN - JMP to NNN
        if ((NNN > 0xFFF) || (NNN < 0x200)){
            printf("Invalid Jump to instruction\n");
            exit(1);
        }
        ourSystem.PC = NNN;
    }
    else if (B1 == 0x2){
        // 2NNN - Call a subroutine
        if ((NNN > 0xFFF) || (NNN < 0x200)){
            printf("Invalid subroutine call\n");
            exit(1);
        }
        ourSystem.SP = ourSystem.SP + 1;
        ourSystem.Stack[ourSystem.SP] = (ourSystem.PC + 2);
        ourSystem.PC = NNN;
    }
    else if (B1 == 0x3){
        // 3XNN - Skip next instruction if VX == NN
        ourSystem.PC = (ourSystem.PC) + 2;
        if ((ourSystem.Registers[B2]) == NN){
            ourSystem.PC = (ourSystem.PC) + 2;
        }
    }
    else if (B1 == 0x4){
        // 4XNN - Skip next instruction if VX != NN
        if ((ourSystem.Registers[B2]) == NN){
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        else{
            ourSystem.PC = (ourSystem.PC) + 4;
        }
    }
    else if (B1 == 0x5){
        // 5XY0 - Skip next instruction if VX == VY
        ourSystem.PC = (ourSystem.PC) + 2;
        if ((ourSystem.Registers[B2]) == (ourSystem.Registers[B3])){
            ourSystem.PC = (ourSystem.PC) + 2;
        }
    }
    else if (B1 == 0x6){
        // 6XNN - Sets VX to NN
        ourSystem.Registers[B2] = NN;
        ourSystem.PC = (ourSystem.PC) + 2;
    }
    else if (B1 == 0x7){
        // 7XNN - Adds NN to VX
        ourSystem.Registers[B2] += NN;
        ourSystem.PC = (ourSystem.PC) + 2;
    }
    else if (B1 == 0x8){
        // 8XY0 - Set VX to VY
        if (B4 == 0x0){
            ourSystem.Registers[B2] = ourSystem.Registers[B3];
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // 8XY1 - Sets VX to VX | VY (Bitwise OR)
        else if (B4 == 0x1){
            ourSystem.Registers[B2] |= ourSystem.Registers[B3];
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // 8XY2 - Sets VX to VX & VY (Bitwise AND)
        else if (B4 == 0x2){
            ourSystem.Registers[B2] &= ourSystem.Registers[B3];
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // 8XY3 - Sets VX to VX XOR VY (Bitwise)
        else if (B4 == 0x3){
            ourSystem.Registers[B2] ^= ourSystem.Registers[B3];
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // 8XY4 - Adds VY to VX. VF is set to 1 if there is a carry, and a 0 otherwise
        else if (B4 == 0x4){
            unsigned short sumOvf = (ourSystem.Registers[B2]) + (ourSystem.Registers[B3]);
            if (sumOvf > 0xFF){
                ourSystem.Registers[0xF] = 1;
            }
            else{
                ourSystem.Registers[0xF] = 0;
            }
            ourSystem.PC = (ourSystem.PC) + 2;
            ourSystem.Registers[B2] += ourSystem.Registers[B3];
        }
        // 8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
        else if (B4 == 0x5){
            signed short diffOvf = (ourSystem.Registers[B2]) - (ourSystem.Registers[B3]);
            if (diffOvf < 0){
                ourSystem.Registers[0xF] = 0;
            }
            else{
                ourSystem.Registers[0xF] = 1;
            }
            ourSystem.PC = (ourSystem.PC) + 2;
            ourSystem.Registers[B2] -= ourSystem.Registers[B3];
        }
        // 8XY6 - Shift VX right by one. VF is set to 0 when there's a borrow, and 1 when there isn't
        else if (B4 == 0x6){
            ourSystem.Registers[0xF] = (ourSystem.Registers[B2] & 0x1);
            ourSystem.Registers[B2] >>= 1;
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // 8XY7 - VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
        else if (B4 == 0x7){
            signed short diffOvf = (ourSystem.Registers[B3]) - (ourSystem.Registers[B2]);
            if (diffOvf < 0){
                ourSystem.Registers[0xF] = 0;
            }
            else{
                ourSystem.Registers[0xF] = 1;
            }
            ourSystem.PC = (ourSystem.PC) + 2;
            ourSystem.Registers[B2] = ourSystem.Registers[B3] - ourSystem.Registers[B2];
        }
        // 8XYE - Shifts VX left by one. VF is set to 0 when there's a borrow, and 1 when there isn't
        else if (B4 == 0xE){
            ourSystem.Registers[0xF] = ourSystem.Registers[B2] >> 7;
            ourSystem.Registers[B2] <<= 1;
            ourSystem.PC = (ourSystem.PC) + 2;
        }
    }
    // 9XY0 - Skips the next instruction if VX doesn't equal VY.
    else if (B1 == 0x9){
        if (ourSystem.Registers[B2] == ourSystem.Registers[B3]){
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        else{
            ourSystem.PC = (ourSystem.PC) + 4;
        }
    }
    // ANNN - Sets I to the address NNN
    else if (B1 == 0xA){
        if (NNN > 0xFFF){
            printf("Invalid read destination\n");
            exit(1);
        }
        ourSystem.I = NNN;
        ourSystem.PC = (ourSystem.PC) + 2;
    }
    // BNNN - Jumps to the address NNN plus V0
    else if (B1 == 0xB){
        if (((NNN + ourSystem.Registers[0]) > 0xFFF) || ((NNN + ourSystem.Registers[0]) < 0x200)){
            printf("Invalid Jump to instruction\n");
            exit(1);
        }
        ourSystem.PC = NNN + ourSystem.Registers[0];
    }
    // CXNN - Sets VX to the result of a bitwise and operation on a random number and NN
    else if (B1 == 0xC){
        //uint8_t randomNum = rand() % 255;
        //randomNum = randomNum + 1;
        //printf("Random number: %02X\n", randomNum);
        //ourSystem.Registers[B2] = NN & randomNum;
        //ourSystem.PC = (ourSystem.PC) + 2;
        ourSystem.Registers[B2] = (NN) & (rand() % 255);
        ourSystem.PC = (ourSystem.PC) + 2;
    }
    // DNNN
    else if (B1 == 0xD){
        Disp(gamer, (ourSystem.Registers[B2]), (ourSystem.Registers[B3]), B4);
        ourSystem.PC = (ourSystem.PC) + 2;
    }
    // ENNN
    else if (B1 == 0xE){
        // EX9E
        if (B4 == 0xE){
            if (ourSystem.key[(ourSystem.Registers[B2])]){
                ourSystem.PC = (ourSystem.PC) + 4;
            }
            else{
                ourSystem.PC = (ourSystem.PC) + 2;
            }
        }
        // EXA1
        else if (B4 == 0x1){
            if (!(ourSystem.key[(ourSystem.Registers[B2])])){
                ourSystem.PC = (ourSystem.PC) + 4;
            }
            else{
                ourSystem.PC = (ourSystem.PC) + 2;
            }
        }
    }
    // FNNN
    else if (B1 == 0xF){
        // FX07
        if ((B3 == 0x0) && (B4 == 0x7)){
            ourSystem.Registers[B2] = ourSystem.Delay;
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // FX0A
        else if ((B3 == 0x0) && (B4 == 0xA)){
            if(isPressed){
               for (unsigned char j = 0; j < 0xF; j++){
                  if (ourSystem.key[j]) {
                      ourSystem.Registers[B2] = j;
                  }
               }
               ourSystem.PC = (ourSystem.PC) + 2;
            }
            /*
            unsigned short x = 0;
            unsigned char c =0;
            while (x != 1){
                for (c; c < 0xF; c++){
                    if (ourSystem.key[c] == 1){
                        x = 1;
                    }
                }
            }
            ourSystem.Registers[B2] = c;
            ourSystem.PC = (ourSystem.PC) + 2;
            */       
        }
        // FX15
        else if ((B3 == 0x1) && (B4 == 0x5)){
            ourSystem.Delay = ourSystem.Registers[B2];
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // FX18
        else if ((B3 == 0x1) && (B4 == 0x8)){
            ourSystem.Sound = ourSystem.Registers[B2];
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // FX1E
        else if ((B3 == 0x1) && (B4 == 0xE)){
            ourSystem.I += ourSystem.Registers[B2];
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // FX29
        else if ((B3 == 0x2) && (B4 == 0x9)){
            unsigned int intertimIreg = ourSystem.Registers[B2];
            intertimIreg = intertimIreg * 5;
            ourSystem.I = intertimIreg;
            //printf("new I: %04X | %d\n", intertimIreg, intertimIreg);
            hex_to_binary(ourSystem.Mem[ourSystem.I]);
            //printf("x:%02X | b:%s\n", ourSystem.Mem[ourSystem.I], binary);
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // FX33
        else if ((B3 == 0x3) && (B4 == 0x3)){
            uint8_t hundreds = floor((ourSystem.Registers[B2])/100);
            uint8_t tens = floor(((ourSystem.Registers[B2])-(hundreds*100))/10);
            uint8_t ones = (ourSystem.Registers[B2]) - (hundreds*100) - (tens*10);
            //printf("hundreds: %d\n", hundreds);
            //printf("tens: %d\n", tens);
            //printf("ones: %d\n", ones);
            ourSystem.Mem[ourSystem.I] = hundreds;
            ourSystem.Mem[ourSystem.I + 1] = tens;
            ourSystem.Mem[ourSystem.I + 2] = ones;
            //printf("reg(x): %d\n", ourSystem.Registers[B2]);
            //printf("I: %d\n", ourSystem.Mem[ourSystem.I]);
            //printf("I+1: %d\n", ourSystem.Mem[ourSystem.I + 1]);
            //printf("I+2: %d\n", ourSystem.Mem[ourSystem.I + 2]);
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // FX55
        else if ((B3 == 0x5) && (B4 == 0x5)){
            for (unsigned char i = 0; i <= B2; i++){
                ourSystem.Mem[(ourSystem.I)+i] = ourSystem.Registers[i];
                //printf("I + %d\n", i);
                hex_to_binary(ourSystem.Mem[(ourSystem.I)+i]);
                //printf("x:%02X | b:%s\n", ourSystem.Mem[(ourSystem.I)+i], binary);
            }
            ourSystem.PC = (ourSystem.PC) + 2;
        }
        // FX65
        else if ((B3 == 0x6) && (B4 == 0x5)){
            for (unsigned char i = 0; i <= B2; i++){
                //printf("I + %d\n", i);
                hex_to_binary(ourSystem.Mem[(ourSystem.I)+i]);
                //printf("x:%02X | b:%s\n", ourSystem.Mem[(ourSystem.I)+i], binary);
                ourSystem.Registers[i] = ourSystem.Mem[(ourSystem.I)+i];
            }
            ourSystem.PC = (ourSystem.PC) + 2;
        }
    }
    else{
        printf("Error: Invalid Opcode: %04X\n", opcode);
        exit(1);
    }
}


void Disp(gamul8 *gamer, uint8_t x, uint8_t y, uint8_t N){
    //uint8_t memPtr* = memory;
    ourSystem.Registers[0xF] = 0;
    for (int i = 0; i < N; i++){
        uint16_t currPixel = ourSystem.Mem[(ourSystem.I)+i];
        //printf("I + %d\n", i);
        hex_to_binary(ourSystem.Mem[(ourSystem.I)+i]);
        //printf("x:%02X | b:%s\n", ourSystem.Mem[(ourSystem.I)+i], binary);
        for (int j = 0; j < 8; j ++){
            unsigned short xcoord = x + j;
            unsigned short ycoord = y + i;
            //uint8_t currBit = (currPixel >> (7-j)) & 0x01;
            // Wrap around for sprites
            if (xcoord > SCREEN_WIDTH){
                xcoord -= SCREEN_WIDTH;
            }
            if (xcoord < 0){
                xcoord += SCREEN_WIDTH;
            }
            if (ycoord > SCREEN_HEIGHT){
                ycoord -= SCREEN_HEIGHT;
            }
            if (ycoord < 0){
               ycoord += SCREEN_HEIGHT;
            }
            unsigned char prevPixel = gamer->display[xcoord][ycoord];
            if ((currPixel >> (7-j)) & 1){
                gamer->display[xcoord][ycoord] = prevPixel ^ 1;
            }
            if ((prevPixel == 1) && (gamer->display[xcoord][ycoord] == 0)){
                ourSystem.Registers[0xF] = 1;
            }
        }
    }
}

void printState(){
    printf("PC: %04X\n", ourSystem.PC);
    printf("I: %04X\n", ourSystem.I);
    printf("Delay: %04X\n", ourSystem.Delay);
    printf("Sound: %04X\n", ourSystem.Sound);
    printf("SP: %d\n", ourSystem.SP);
    if (ourSystem.SP >= 0){
        for (int j = 0; j <= (ourSystem.SP); j++){
            printf("Stack[%d] = %04X\n", j, ourSystem.Stack[j]);
        }
    }
    for (int j = 0; j < 16; j++){
        printf("Register %01X = %02X\n", j, ourSystem.Registers[j]);
    }
    /*
    printf("PC: %04X\n", ourSystem.PC);
    printf("-----Memory-----\n");
    int loop;
    for (loop = 512; loop < 550; loop++){
        printf("%02X\n", ourSystem.Mem[loop]);
    }
    */
}
void print_zz(gamul8 *gamer){
    gamer->display[0][0] = 1;
}
void hex_to_binary(uint8_t hexcode){
    binary[0] = '\0';
    int a;
    for (a = 128; a>0; a >>=1){
        strcat(binary, ((hexcode & a) == a) ? "1" : "0");
    }
}
