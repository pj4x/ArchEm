#include <GL/glut.h>
#include<iostream>
#include<fstream>
#include<string>
#include"read_input.hpp"

#define WIDTH 256
#define HEIGHT 256
using namespace std;

class Processor {
public:
    Processor() : r1(0), r2(0), r3(0), r4(0), flags({ 0, 0, 0, 0 }) {
        // Initialize the memory with 256 16-bit words, all set to 0
        for (int i = 0; i < 256; ++i) {
            memory[i] = 0;
        }

        // Initialize vmem array with 256x256 pixels
        for (int i = 0; i < 256 * 256; ++i) {
            vmem[i] = { 0, 0, {0, 0, 0} };
        }
    }


    // Define the 16-bit integer registers
    int16_t r1;
    int16_t r2;
    int16_t r3;
    int16_t r4;

    // Define the bitfield for flags
    struct Flags {
        bool zero : 1;
        bool carry : 1;
        bool overflow : 1;
        bool wastrue : 1;
    } flags;

    // Define the memory array with 256 16-bit words
    int16_t memory[256];

    // Define the vmem array for pixel data
    struct Pixel {
        uint8_t x;
        uint8_t y;
        struct Color {
            uint8_t r : 4;
            uint8_t g : 4;
            uint8_t b : 4;
        } color;
    } vmem[256 * 256];
};


const char nop = 0x00; // does nothing
const char add = 0x01; // add r1 and r2 so that r1 = r1 + r2
const char sub = 0x02; // sub r1 and r2 so that r1 = r1 - r2
const char mul = 0x03; // mul r1 and r2 so that r1 = r1 * r2
const char l1i = 0x04; // l1i loads imediate value into r1
const char l2i = 0x05; // l2i loads imediate value into r2
const char mrx = 0x06; // mrx copys value from r1 to r2 or from r2 to r3 or r3 to r4 or from r4 to r1
const char cmp = 0x07; // compares value of r1 to r2 and sets true flag if compare = true
const char mov = 0x08; // copys value from one memory address(8bit) to other address(8bit)
const char jif = 0x09; // jumps if the true flag is true
const char jiz = 0x0A; // jumps if the zero flag is true
const char jic = 0x0B; // jumps if the carry flag is true
const char jio = 0x0C; // jumps if the overflow flag is true
const char mvm = 0x0D; // copys data to one pixel on screen. x and y from r1 and r2 respectivly. r, g and b from memory addresses 0, 1 and 2 color range 0 -> 15
const char ldm = 0x0E; // copys value from r1 into memory address
const char ldr = 0x0F; // copys value from memory address into r1


const char* code = read_input("input.hex");
Processor cpu = Processor();
int pc = 0;


void myinit(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 256.0, 0.0, 256.0);
	
}

void setPixel(GLint x, GLint y){
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

void update() {
    switch (code[pc]) {
        case nop:
            break;
        case add:
            cpu.r1 = cpu.r1 + cpu.r2;
            if (cpu.r1 = 0) cpu.flags.zero = true;
            else cpu.flags.zero = false;
            pc++;
            break;
        case sub:
            cpu.r1 = cpu.r1 - cpu.r2;
            if (cpu.r1 = 0) cpu.flags.zero = true;
            else cpu.flags.zero = false;
            pc++;
            break;
        case mul:
            cpu.r1 = cpu.r1 * cpu.r2;
            if (cpu.r1 = 0) cpu.flags.zero = true;
            else cpu.flags.zero = false;
            pc++;
            break;
        case l1i:
            cpu.r1 = code[pc + 1];
            pc += 2;
            break;
        case l2i:
            cpu.r2 = code[pc + 1];
            pc += 2;
            break;
        case mrx:
            if (code[pc + 1] == 0) cpu.r2 = cpu.r1;
            if (code[pc + 1] == 1) cpu.r3 = cpu.r2;
            if (code[pc + 1] == 2) cpu.r4 = cpu.r3;
            if (code[pc + 1] == 3) cpu.r1 = cpu.r4;
            pc += 2;
            break;
        case cmp:
            if (cpu.r1 == cpu.r2) cpu.flags.wastrue = true;
            else cpu.flags.wastrue = false;
            pc++;
            break;
        case mov:
            cpu.memory[code[pc + 1]] = cpu.memory[code[pc + 2]];
            pc += 2;
            break;
        case jif:
            if (cpu.flags.wastrue) pc = code[pc + 1];
            else pc += 2;
            break;
        case jiz:
            if (cpu.flags.zero) pc = code[pc + 1];
            else pc += 2;
            break;
        case jic:
            if (cpu.flags.carry) pc = code[pc + 1];
            else pc += 2;
            break;
        case jio:
            if (cpu.flags.overflow) pc = code[pc + 1];
            else pc += 2;
            break;
        case mvm:
            cpu.vmem[code[pc + 1]].x = cpu.r1;
            cpu.vmem[code[pc + 1]].y = cpu.r2;
            cpu.vmem[code[pc + 1]].color.r = cpu.memory[0];
            cpu.vmem[code[pc + 1]].color.b = cpu.memory[1];
            cpu.vmem[code[pc + 1]].color.g = cpu.memory[2];
            pc += 2;
            break;
        case ldm:
            cpu.memory[code[pc + 1]] = cpu.r1;
            pc += 2;
            break;
        case ldr:
            cpu.r1 = cpu.memory[code[pc + 1]];
            pc += 2;
            break;
        default:
            std::cerr << "WRONG INSTRUCTION!!! FUCK YOU" << std::endl;
            pc++;
    }
}

void draw() {
    for (int i = 0; i < 256 * 256; i++) {
        glColor3f(cpu.vmem[i].color.r / 15.0, cpu.vmem[i].color.g / 15.0, cpu.vmem[i].color.b / 15.0);
        setPixel(cpu.vmem[i].x, cpu.vmem[i].y);
    }
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT);
    if (pc < 1024) {
        update();
    }
	
	draw();
	glFlush();
	glutPostRedisplay();
	glutSwapBuffers();
}

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("ArchEm");
	myinit();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
