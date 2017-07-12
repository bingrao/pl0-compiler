#ifndef GENCODE_H
#define GENCODE_H
#include "common.h"
#include "parse.h"
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

void generateLiteral(int set,int r, int value);
void generateLoad(int set, int r,struct Symbol *ident, int currentLevel);
void generateBecomes(int set,int r, struct Symbol *ident, int currentLevel);
void generateCalculation(int set,int token,int i,int j, int k);
void generateRelOp(int set, int token,int i,int j, int k);
void generateNegation(int set,int i,int j, int k);
void generatePosification(int set);
void generateVariableSpace(int set, int space);
void generateProcedureJumps(int set, int space);
void generateCall(int set, struct Symbol *ident, int currentLevel);
void generateRead(int set,int r,struct Symbol *ident, int currentLevel);
void generateWrite(int set,int r,struct Symbol *ident, int currentLevel,int type);
void generateJump(int set);
void generateConditionalJump(int set,int r);
void generateReturn(int set);
void generateHalt(int set);

int getCodeLength(int set);
void changeM(int set, int index, int m);
void printProcedureCode();

int generateMachineCode(FILE* mcode);
void prepareMcode();

void printProcedureCode(int option,FILE *output);
void printMcode(FILE* mcode);
void putInstruction(struct instruction instruction,FILE* mcode);

void appendCode(int set, int op,int r,int l,int m);
void prependCode(int set, int op,int r,int l,int m);
void insertCode(int set, int op, int r,int l, int m, int index);
void makeSpaceInCode(int set, int size, int index);
#endif
