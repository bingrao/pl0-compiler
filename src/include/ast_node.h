#ifndef AST_NODE_H
#define AST_NODE_H
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

typedef struct identNode{
// identifer node
	char *string;
}identNode_t;
typedef struct numberNode{
	// numeral node
	int value;
}numberNode_t;

typedef struct relOpNode{
	/*"="|"<>"|"<"|"<="|">"|">="*/ 
	/*odd*/
	Token_t type;
}relOpNode_t;
typedef struct AriOpNodepm{
	/*"+"|"-"*/
	Token_t type;
}AriOpNodepm_t;
typedef struct AriOpNodemd{
	/*"*"|"/"*/
	Token_t type;
}AriOpNodemd_t;

typedef struct factorNode{
	enum{
		F_IDENT=1,
		F_DIGIT=2,
		F_EXP=3
	}tag;	
	/*factor ::= ident | number | "(" expression ")".*/
	union{
		struct identNode ident;
		struct numberNode digit;
		struct expressionNode *exp;
	}element;
}factorNode_t;
typedef struct factorExp{
	/*{("*"|"/") factor}*/
	struct AriOpNodemd op;
	struct factorNode *factor;
	struct factorExp *next;
}factorExp_t;

typedef struct termNode{
	/*term ::= factor {("*"|"/") factor}. */
	struct factorExp *factorEList;
	int numsFacorExp;
}termNode_t;


typedef struct termExp{
	/* ("+"|"-") term*/
	struct AriOpNodepm op;
	struct termNode *term;
	struct termExp *next;
}termExp_t;

typedef struct expressionNode{
/*
  *expression ::= [ "+"|"-"] term { ("+"|"-") term}.
  */	
	struct termExp *termEList;
	int numsTermExp;
}expressionNode_t;



typedef struct conditionNode{
/*
condition ::= "odd" expression 
	  	| expression  rel-op  expression.
*/	
	struct relOpNode op;
	struct expressionNode *left;
	struct expressionNode *right;
}conditionNode_t;




typedef struct callNode{
	// call statement node
	struct identNode ident;
}callNode_t;

typedef struct writeSNode{
	/*"write"  ident */
	struct identNode ident;
}writeSNode_t;

typedef struct readSNode{
	/*"read" ident */
	struct identNode ident;
}readSNode_t;

typedef struct whileSNode{
	/*"while" condition "do" statement*/
	struct conditionNode *cond;
	struct statementNode *sts;
}whileSNode_t;

typedef struct ifSNode{
	/*"if" condition "then" statement*/
	struct conditionNode *cond;
	struct statementNode *ifsts;
	struct statementNode *elsests;
}ifSNode_t;

typedef struct beginSNode{
	/*"begin" statement { ";" statement } "end" */
	struct statementNode *stsList;
	int numsStatement;
}beginSNode_t;

typedef struct assignSNode{
	/*ident ":=" expression*/
	struct identNode ident;
	struct expressionNode *exp;
}assignSNode_t;

typedef struct statementNode{
	enum{
		ASSIGN_S=1,
		BEGIN_S=2,
		IF_S=3,
		WHILE_S=4,
		READ_S=5,
		WRITE_S=6,
		CALL_S=7
	}tag;
	union{
		struct assignSNode *assigns_p; // assignment statement
		struct beginSNode *begins_p; // begin statement
		struct ifSNode *ifs_p; // if statement
		struct whileSNode *whiles_p; // while statement
		struct readSNode *reads_p; // read statement
		struct writeSNode * writes_p;// write statement
		struct callNode *calls_p; // call statement
	}super;
	struct statementNode *next;
}statementNode_t;


typedef struct prodefNode{
	struct identNode ident; // name of procedure
	struct blockNode *body; // the body part of procedure
	struct prodefNode *next; // point to next procedure definition
	int lexical; // lexical level for this procedure
	int addr; // the address of this procedure
	int paraProc; // the pararent procedure address
}prodefNode_t;


typedef struct varDeclNode{
	// variable define statement
	struct identNode ident;
	int lexical; // lexical level
	int paraProc; // pararent address
	int addr; // address
	struct varDeclNode *next;
}varDeclNode_t;


typedef struct constDeclNode{
	struct identNode ident; // name
	struct numberNode numer; // value
	int lexical; //lexical level
	int paraProc; //parent procedure 
	int addr; // address
	struct constDeclNode *next;
}constDeclNode_t;

typedef struct blockNode{
	struct constDeclNode *constDecl;
	int numsConstDecl;
	
	struct varDeclNode *varDecl;
	int numsVarDecl;
	
	struct prodefNode *procDef;
	int numsProcDef;
	
	struct statementNode *statement;
	int numsStatement;
	
	struct blockNode *next;
}blockNode_t;

typedef struct programNode{
	struct blockNode *block;
	int numsBlock;
}programNode_t;


#endif
