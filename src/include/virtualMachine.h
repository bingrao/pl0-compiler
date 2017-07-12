#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H
/**************************************************************************************************
<It is project about Compiler for PL/0>
Copyright (C) <2017>  <Bingbing Rao> <Bing.Rao@outlook.com>
@https://github.com/CSINUCF


This program is free software: you can redistribute it and/or modify it under the terms 
of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.
*/

#include "common.h"

/*
 * Define "RPINT_VM_STATE" macro to debug the state of 
 * virtual machine when it executed an instruction
 */

#define RPINT_VM_STATE

/*
 *unparse the machine code to assembly code when you define
 * "UNPARSER_CODE" macro
 */

#define UNPARSER_CODE

/*
 * Print the machine code pretty when you define "PRETTY_PRINT_CODE"
 */

//#define PRETTY_PRINT_CODE

/*
 * The VM will output the result to the console when you define "OUTPUT_TO_CONSOLE" macro.
 */
#define OUTPUT_TO_CONSOLE

typedef enum {
	OUTPUT = 1, //print the data to the console
	INPUT = 2, // read data from the console
	HALT = 3 //set halt flag to one
}sio_m;
typedef enum {
	UNINIT = 0,
	IDLE = 1,
	RUNNING = 2,
	HALTING = 3,
	ERROR = 4
}vm_status;
typedef struct virtualMachine{
	int sp; //stack pointer
	int bp; //base pointer 
	int pc; //program counter
	int pre_pc;
	instruction_t ir; //instruction register
	int r[COMMON_REGISTER_NUMBER];// register file
	int stack[MAX_STACK_HEIGHT]; //a stack which store the data
	instruction_t code[MAX_CODE_LENGTH]; //code segment to store the code
	int frameBreak[MAX_LEXI_LEVELS]; // To identify a AR
	int frameLevel;
	int numInstructions;
	int status;

	FILE * out_fp; //console std out file pointer

	/*define some core function to manage and execute vm*/
	int (*init)(struct virtualMachine *vm,int argc, char* argv[]);
	void (*exit)(struct virtualMachine *vm);
	int (*load_code)(struct virtualMachine *vm, char* path);
	void(*stdout)(struct virtualMachine *vm,char *format, ...);
	int (*base)(struct virtualMachine *vm,int level,int base);
	boolean (*prefetch)(struct virtualMachine *vm);
	void (*execute)(struct virtualMachine *vm);
	void (*run)(struct virtualMachine *vm,char *path,FILE *stdout);
	void (*prettyinfo)(struct virtualMachine *vm,int option);
}virtualMachine_t;
extern struct virtualMachine* init_vm();
#endif
