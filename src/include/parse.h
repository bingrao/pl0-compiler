#ifndef PARSE_H 
#define PARSE_H
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
#include "dfa.h"
#include "symtable.h"
#include "gencode.h"
#include "ast_node.h"
#include <stdlib.h>   
#include <stdio.h>   
#include <stdarg.h>   





typedef enum {
	NON_BLOCK=0,
	NON_STATEMENT=1,
	NON_CONDITION=2,
	NON_EXPRESSION=3,
	NON_TERM=4,
	NON_FACTOR=5
}NonTerminal_t;

typedef struct snode 
{ 
	Token_t elem; 
	struct snode* next; 
} snode, *symset; 
extern symset phi;
extern symset blockbegsys;
extern symset declbegsys;
extern symset statbegsys;
extern symset condbegsys;
extern symset exprbegsys;
extern symset facbegsys;
extern symset termbegsys;
extern symset relset; 
 
symset createset(Token_t data, .../* SYM_NULL */); 
void destroyset(symset s); 
symset uniteset(symset s1, symset s2); 
int inset(Token_t elem, symset s); 
void showset(symset s);

/*
  *
  **/
typedef struct CurrentToken{
	Token_t cToken;
	union{
  		int numeric;
  		char string[MAX_IDENT_LENGTH + 1];
	}cTokenVal;
}CurrentToken_t;


typedef struct Parse{
	FILE *stdout; // output console, a file or screen
	FILE *tokenFile;// token file
	CurrentToken_t curToken;// to record current token
	int tokenCount;
	struct SymTable *symbolTable;// symbol table
	struct programNode *ast; // root of ast
	int (*parse)(struct Parse *this); // parse funciton
	void (*printAST)(struct Parse *this,FILE *stdout);
	void (*unParse)(struct Parse *this);
	void (*cleanup)(struct Parse *this);
	int (*run)(struct Parse *this,FILE *tFile);
	void (*exit)(struct Parse *this);
	void (*unParsePrint)(struct Parse *this,FILE *stdout);
}Parse_t;
extern char *opSymbol[];
extern struct Parse * parse_init(struct SymTable *syms);
void getToken(struct Parse *this);
Token_t getfollowToken(struct Parse *this);
void scanFollowSymbol(struct Parse *this,NonTerminal_t nonterminal);
void scanFirstSymbol(struct Parse *this,NonTerminal_t nonterminal);
int firstAndFollowSet(struct Parse *this);
void expect(struct Parse *this,Token_t expect);

#endif
