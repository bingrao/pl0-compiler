#ifndef COMMON_H
#define COMMON_H
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>



#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_CODE_SETS 	100
#define MAX_LEXI_LEVELS 5
#define COMMON_REGISTER_NUMBER 16

#define MAX_IDENT_LENGTH 11
#define MAX_RESERVED_WORD 15
#define MAX_BUFFER_SIZE (MAX_IDENT_LENGTH+2)
#define MAX_NUMBER_LENGTH 6
#define MAX_NUMBER	90000
#define MAX_SYMBOL_CNT 500


#define loginfo(format, ...)\
	printf("INFO %s " format,__func__,##__VA_ARGS__)

#define logpretty(format, ...)\
	printf(format,##__VA_ARGS__)

#define logerror(format, ...)\
	printf("ERROR %s:%s:%d " format,__FILE__,__func__,__LINE__,##__VA_ARGS__)


//#define DEBUG
#ifdef DEBUG
#define logdebug(format, ...)\
	printf("DEBUG %s:%s:%d " format,__FILE__,__func__,__LINE__,##__VA_ARGS__)
#else
#define logdebug(format, ...)
#endif
/*
  * out_fp:  a file pointer to point to a open file
  */
#define CompilerStdout(out_fp,format, ...) do{\
		if(out_fp != NULL){\
			fprintf(out_fp,format,##__VA_ARGS__);\
		}\
		else\
			printf(format,##__VA_ARGS__);\
	}while(0)

typedef struct instruction{
	int op; /*Operation Code*/
	int r; /*Register*/
	int l; /*Lexicographical Level*/
	int m; /*Modifier*/
}instruction_t;

typedef enum {
	LIT = 1,
	RTN = 2,
	LOD = 3,
	STO = 4,
	CAL = 5,
	INC = 6,
	JMP = 7,
	JPC = 8,
	SIO = 9,

	//There are another two instructions with SIO Prefix
	SIO2 = 10,  // Reserve
	SIO3 = 11,  // Reserve
	
	NEG = 12,
	ADD = 13,
	SUB = 14,
	MUL = 15,
	DIV = 16,
	ODD = 17,
	MOD = 18,
	EQL = 19,
	NEQ = 20,
	LSS = 21,
	LEQ = 22,
	GTR = 23,
	GEQ = 24
}opcode_e;


//Lexeme Types
typedef enum token {
    nulsym = 1,        // "null"
    identsym = 2,      // a variable
    numbersym = 3,     // a number
    plussym = 4,       // "+"
    minussym = 5,      // "-"
    multsym = 6,       // "*"
    slashsym = 7,      // "/"
    oddsym = 8,        // "odd" 17
    eqlsym = 9,        // "="    19
    neqsym = 10,       // "<>"   20
    lessym = 11,       // "<"    21
    leqsym = 12,       // "<="   22
    gtrsym = 13,       // ">"    23
    geqsym = 14,       // ">="  24
    lparentsym = 15,   // "("
    rparentsym = 16,   // ")"
    commasym = 17,     // ","
    semicolonsym = 18, // ";"
    periodsym = 19,    // "."
    becomessym = 20,   // ":="
    beginsym = 21,     // "begin"
    endsym = 22,       // "end"
    ifsym = 23,        // "if"
    thensym = 24,      // "then"
    whilesym = 25,     // "while"
    dosym = 26,        // "do"
    callsym = 27,      // "call"
    constsym = 28,     // "const"
    varsym = 29,       // "var"
    procsym = 30,      // "procedure"
    writesym = 31,     // "write"
    readsym = 32,      // "read"
    elsesym = 33,      // "else"
    commentsym = 34    // "comments"
}Token_t;

typedef enum {
	CONST_S = 1,
	VAR_S =2,
	PROC_S=3
}SymbolType;

typedef struct Symbol{ 
	int kind; 			// const = 1, var = 2, proc = 3
	char *name;			// name up to 11 chars
	int val; 			// number (ASCII value)
	int level; 			// L level
	int addr; 			// M address
}Symbol_t; 


typedef enum {
	FALSE = 0,
	TRUE = 1
}boolean;
extern char *opcode[];
static inline int unparseIr(FILE *out_fp,int idx,instruction_t *ir){
	int op = ir->op;
	int r = ir->r;
	int l = ir->l;
	int m = ir->m;
	switch(op){
		case LIT:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],%d,%d",idx,opcode[op],r,l,m);
		}break;
		case RTN:{
			CompilerStdout(out_fp,"%4d\t%s %d,%d,%d",idx,opcode[op],r,l,m);
		}break;
		case LOD:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],%d,%d",idx,opcode[op],r,l,m);
		}break;
		case STO:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],%d,%d",idx,opcode[op],r,l,m);
		}break;
		case CAL:{
			CompilerStdout(out_fp,"%4d\t%s %d,%d,%d",idx,opcode[op],r,l,m);
		}break;
		case INC:{
			CompilerStdout(out_fp,"%4d\t%s %d,%d,%d",idx,opcode[op],r,l,m);
		}break;
		case JMP:{
			CompilerStdout(out_fp,"%4d\t%s %d,%d,%d",idx,opcode[op],r,l,m);
		}break;
		case JPC:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],%d,%d",idx,opcode[op],r,l,m);
		}break;
		case SIO:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],%d,%d",idx,opcode[op],r,l,m);
		}break;
		case NEG:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],%d",idx,opcode[op],r,l,m);
		}break;
		case ADD:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case SUB:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case MUL:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case DIV:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case ODD:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],%d,%d",idx,opcode[op],r,l,m);
		}break;
		case MOD:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case EQL:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case NEQ:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case LSS:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case LEQ:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case GTR:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		case GEQ:{
			CompilerStdout(out_fp,"%4d\t%s R[%d],R[%d],R[%d]",idx,opcode[op],r,l,m);
		}break;
		default:{
			logerror("The opcode is wrong,please double check");
			return -1;
		}
	}
	return 0;
}


static inline char *repeatString(char *str,int count) {
    if (count == 0) return NULL;
    char *ret = calloc(1,strlen(str)*count+count);
    if (ret == NULL) return NULL;
    strcpy (ret, str);
    while (--count > 0) {
        strcat (ret, " ");
        strcat (ret, str);
    }
    return ret;
}


static inline char *getIndent(int count){
	return repeatString("\t",count);
}

static inline char *getSpace(int count){
	return repeatString(" ",count);
}


static inline int findIndex(int argc, char* argv[],char *p){
	int i,idx = -1;
	for(i=0;i<argc;i++){
		if(strcmp(argv[i],p) == 0){
			idx = i;
			break;
		}
	}
	return idx;
}

static inline FILE *fileCreateAndOpen(char *path){
	if(access(path,F_OK ) == -1){
		fclose(fopen(path, "w"));
	}
	return fopen(path, "ab+");
}

static inline void throwError(int errorType)
{

    logpretty("\tError %d: ", errorType);

    switch (errorType) {
    case 1:
        logpretty("Use = instead of :=.\n");
        break;
    case 2:
        logpretty("= must be followed by a number.\n");
        break;
    case 3:
        logpretty("Identifier must be followed by = .\n");
        break;
    case 4:
        logpretty("const, var, procedure must be followed by identifier.\n");
        break;
    case 5:
        logpretty("Semicolon or comma missing.\n");
        break;
    case 6:
        logpretty("Incorrect symbol after procedure declaration.\n");
        break;
    case 7:
        logpretty("Statement expected.\n");
        break;
    case 8:
        logpretty("Incorrect symbol after statement part in block.\n");
        break;
    case 9:
        logpretty("Period expected.\n");
        break;
    case 10:
        logpretty("Semicolon between statements missing.\n");
        break;
    case 11:
        logpretty("Undeclared identifier.\n");
        break;
    case 12:
        logpretty("Assignment to constant or procedure is not allowed.\n");
        break;
    case 13:
        logpretty("Assignment operator expected.\n");
        break;
    case 14:
        logpretty("call must be followed by an identifier.\n");
        break;
    case 15:
        logpretty("Call of a constant or variable is meaningless.\n");
        break;
    case 16:
        logpretty("then expected.\n");
        break;
    case 17:
        logpretty("Semicolon or } expected.\n");
        break;
    case 18:
        logpretty("do expected.\n");
        break;
    case 19:
        logpretty("Incorrect symbol following statement.\n");
        break;
    case 20:
        logpretty("Relational operator expected.\n");
        break;
    case 21:
        logpretty("Expression must not contain a procedure identifier.\n");
        break;
    case 22:
        logpretty("Right parenthesis missing.\n");
        break;
    case 23:
        logpretty("The preceding factor cannot begin with this symbol.\n");
        break;
    case 24:
        logpretty("An expression cannot begin with this symbol.\n");
        break;
    case 25:
        logpretty("This number is too large.\n");
        break;
    case 26:
        logpretty("end expected.\n");
        break;
    case 27:
        logpretty("read must be followed by an variable identifier.\n");
        break;
    case 28:
        logpretty("write must be followed by an identifier.\n");
        break;
    case 29:
        logpretty("Symbol out of scope.\n");
        break;
	case 30:
		logpretty("Assignment must be followed by :=.\n");
		break;
    }
}
#endif
