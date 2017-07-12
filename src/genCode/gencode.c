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

@Reference:https://github.com/ZGorlock/PM0-Compiler
*/

#include "gencode.h"




//Global Variables
struct instruction finalCode[MAX_CODE_SETS * MAX_CODE_LENGTH]; //holds the finalized code
int finalInstructionCount = 0;

struct instruction generatedCode[MAX_CODE_SETS][MAX_CODE_LENGTH]; //holds the compiled mcode sets
int instructionCount[MAX_CODE_SETS]; //stores the number of instructions in the code

#define FINAL_CODE -1

#define CodeGenerate

// create a new  instruction.
struct instruction newInstruction(int op,int r,int l,int m)
{
    struct instruction i;
    i.op = op;
    i.l = l;
	i.r = r;
    i.m = m;
    return i;
}
void printProcedureCode(int option,FILE *output){
#ifdef CodeGenerate
	int i,j;
	switch(option){
		case 1:{			
			for(i=0;i<MAX_CODE_SETS;i++){
				for(j=0;j<instructionCount[i];j++){
					unparseIr(output,i,&generatedCode[i][j]);
					CompilerStdout(output,"\n");
				}
				CompilerStdout(output,"\n");
			}
		}break;
		case 2:{			
			for (i = 0; i < finalInstructionCount; i++) {
				unparseIr(output,i,&finalCode[i]);
				CompilerStdout(output,"\n");
			}
		}break;
		default:{
			loginfo("please input option with 1 (print machine code for each set) or 2 (print final machine code)\n");
		}
		

	}
#endif
}



// combine machine code of each procedure into final code
int generateMachineCode(FILE* mcode)
{
    prepareMcode();
	printMcode(mcode);
    return 0;
}

/*
    Combines machine code of each procedure into the final mcode.
*/
void prepareMcode()
{
    int sets = 0;
    int i;
    for (i = 0; i < MAX_CODE_SETS; i++) {
        if (instructionCount[i] == 0) {
            sets = i;
            break;
        }
    }

    if (sets == 0)
        return;

    generateProcedureJumps(FINAL_CODE, sets);

    for (i = sets - 1; i >= 0; i--) {
        changeM(FINAL_CODE, i, finalInstructionCount); //update jump to accurate address

        int j;
        for (j = 0; j < instructionCount[i]; j++) {
            struct instruction inst = generatedCode[i][j];
            appendCode(FINAL_CODE, inst.op, inst.r,inst.l, inst.m);

            if (inst.op == JMP || inst.op == JPC) //increase jumps by procedure offset
                changeM(FINAL_CODE, finalInstructionCount - 1, inst.m+ finalCode[i].m + 1);
        }
    }
}

/*
    Prints the mcode instructions to the mcode file.
*/
void printMcode(FILE* mcode)
{
    int i;
    for (i = 0; i < finalInstructionCount; i++) {
        putInstruction(finalCode[i],mcode);
    }
}

/*
    Prints an instruction to the mcode file.
*/
void putInstruction(struct instruction instruction,FILE* mcode)
{
    CompilerStdout(mcode, "%d %d %d %d\n", instruction.op,instruction.r,instruction.l, instruction.m);
}

/* Adds an instruction to the end of the instruction array.*/
void appendCode(int set, int op,int r,int l,int m)
{
    if (set == FINAL_CODE) {
        finalCode[finalInstructionCount] = newInstruction(op,r,l, m);
        finalInstructionCount++;
    }
    else {
        generatedCode[set][instructionCount[set]] = newInstruction(op,r,l, m);
        instructionCount[set]++;
    }
}

/*Adds an instruction at the beginning of the instruction array*/
void prependCode(int set,int op,int r,int l,int m)
{
    insertCode(set,op,r,l,m,0);
}

/*Adds an instruction at a certain index of the instruction array.*/
void insertCode(int set, int op, int r,int l,int m, int index)
{
    if (set == FINAL_CODE) {
        if (index < 0 || index > finalInstructionCount - 1) {
            if (finalInstructionCount == 0)
                appendCode(set, op,r,l, m);
            return;
        }
    }
    else {
        if (index < 0 || index > instructionCount[set] - 1) {
            if (instructionCount[set] == 0)
                appendCode(set, op,r,l, m);
            return;
        }
    }

    makeSpaceInCode(set, 1, index);

    if (set == FINAL_CODE) {
        finalCode[index] = newInstruction(op,r,l, m);
        finalInstructionCount++;
    }
    else {
        generatedCode[set][index] = newInstruction(op,r,l, m);
        instructionCount[set]++;
    }
}

/* Makes space for new instructions in the instruction array.*/
void makeSpaceInCode(int set, int size, int index)
{
    if (size <= 0)
        return;

    if (set == FINAL_CODE) {
        if (index < 0 || index > finalInstructionCount - 1)
            return;
    }
    else {
        if (index < 0 || index > instructionCount[set] - 1)
            return;
    }

    int i;
    if (set == FINAL_CODE) {
        for (i = finalInstructionCount - 1; i >= index; i--)
            finalCode[i + size] = finalCode[i];
    }
    else {
        for (i = instructionCount[set] - 1; i >= index; i--)
            generatedCode[set][i + size] = generatedCode[set][i];
    }
}

/*
Produces code for a numerical literal which is load a constant value(literal) m into register r.
*/
void generateLiteral(int set,int r,int value)
{
#ifdef CodeGenerate
    appendCode(set,LIT,r,0,value);
#endif
}
/*
    Produces code for a return.

    Input:
        set : The code set, -1 for final code set.
*/
void generateReturn(int set)
{
#ifdef CodeGenerate
   	appendCode(set,RTN,0,0,0);
#endif
}
/*
    Produces code for loading a symbol identifier.
*/
void generateLoad(int set,int r,struct Symbol *ident, int currentLevel)
{
#ifdef CodeGenerate
    appendCode(set,LOD,r,currentLevel - ident->level,ident->addr);
#endif
}

/*
    Produces code for storing a symbol identifier value.
*/
void generateBecomes(int set,int r,struct Symbol *ident, int currentLevel)
{
#ifdef CodeGenerate
    appendCode(set,STO,r,currentLevel - ident->level, ident->addr);
#endif
}
/*
    Produces code for a call.
*/
void generateCall(int set, struct Symbol *ident, int currentLevel)
{
#ifdef CodeGenerate
    appendCode(set, CAL,0,currentLevel - ident->level, ident->addr);
#endif
}
/*
    Produces code for creating variable space.

*/
void generateVariableSpace(int set, int space)
{
#ifdef CodeGenerate
    prependCode(set,INC,0,0,space);
#endif
}
/*
    Produces code for a generic jump.

    Input:
        set : The code set, -1 for final code set.
*/
void generateJump(int set)
{
#ifdef CodeGenerate
    appendCode(set,JMP,0,0,0);
#endif
}

/*
    Produces code for a generic conditional jump.

    Input:
        set : The code set, -1 for final code set.
*/
void generateConditionalJump(int set,int r)
{
#ifdef CodeGenerate
    appendCode(set,JPC,r,0,0);
#endif
}
/*
    Produces code for a IO read.
*/
void generateRead(int set,int r,struct Symbol *ident, int currentLevel)
{
#ifdef CodeGenerate
    appendCode(set,SIO,r,0, 2);
    appendCode(set, STO,r,currentLevel - ident->level, ident->addr);
#endif
}

/*
    Produces code for an IO write.
*/
void generateWrite(int set,int r,struct Symbol *ident, int currentLevel,int type)
{
#ifdef CodeGenerate
	if(type == VAR_S) // VAR_S
    	appendCode(set, LOD,r, currentLevel - ident->level, ident->addr);
	else // CONST_S
		appendCode(set, LOD,r, 0,ident->val);
    appendCode(set, SIO,r, 0, 1);
#endif
}

/*
    Produces code for a halt.
*/
void generateHalt(int set)
{
#ifdef CodeGenerate
    appendCode(set,SIO,0,0,3);
#endif
}

/*
    Produces code for a calculation.
*/
void generateCalculation(int set,int token,int i,int j, int k)
{
#ifdef CodeGenerate
	int op = 0;
	switch(token){
		case plussym: 	op = ADD;	break;
		case minussym: 	op = SUB;	break;
		case multsym:	op = MUL;	break;
		case slashsym:	op = DIV;	break;
	}
	appendCode(set,op,i,j,k);
#endif
}

/*
    Produces code for a relational operator.
*/
void generateRelOp(int set,int token,int i,int j,int k)
{
#ifdef CodeGenerate
	int op = 0;
	switch(token)
	{
		case oddsym:	op = ODD;	break;
		case eqlsym:	op = EQL;	break;
		case neqsym:	op = NEQ;	break;
		case lessym:	op = LSS;	break;
		case leqsym:	op = LEQ;	break;
		case gtrsym:	op = GTR;	break;
		case geqsym:	op = GEQ;	break;
	}
	appendCode(set,op,i,j,k);
#endif	
}

/*
    Produces code for a negation.
*/
void generateNegation(int set,int i,int j,int k)
{
#ifdef CodeGenerate
   appendCode(set,NEG,i,j,k);
#endif
}

void generateProcedureJumps(int set, int space)
{
    int i;
    for (i = 0; i < space; i++)
        generateJump(set);
}
/*
    Returns the code length of a code set.
*/
int getCodeLength(int set)
{
#ifdef CodeGenerate
    if (set == FINAL_CODE)
        return finalInstructionCount;
    else
        return instructionCount[set];
#endif
}

/*
    Edits the M value of an instruction.
*/
void changeM(int set, int index, int m)
{
#ifdef CodeGenerate
    if (set == FINAL_CODE) {
        if (index < 0 || index > finalInstructionCount - 1)
            return;
        finalCode[index].m = m;
    }
    else {
        if (index < 0 || index > instructionCount[set] - 1)
            return;
        generatedCode[set][index].m = m;
    }
#endif
}
