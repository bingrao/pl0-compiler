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
#include "parse.h"

char *opSymbol[] = {
	" ",			//0
	"null",			// nulsym = 1,        // "null"
	"ident",		//identsym = 2,      // a variable
	"number",		//numbersym = 3,     // a number		
	"+",			//plussym = 4,       // "+"
	"-",
	"*",
	"/",
	"odd",
	"=",			// eqlsym = 9,        // "="    19
	"<>",			//neqsym = 10,       // "<>"   20
	
	"<",			//lessym = 11,       // "<"    21			
	"<=",
	">",
	">=",
	"(",
	")",
	",",
	";",
	".",			
	":=",			
	
	"begin",		// beginsym = 21,     // "begin"		
	"end",
	"if",
	"then",
	"while",
	"do",
	"call",
	"const",
	"var",		
	"procedure",	
	
	"write",
	"read",
	"else",
	"commends"
};
void statementUnParse(statementNode_t *sts,int t,FILE *stdout);
void expressionUnParse(expressionNode_t *exp,int t,FILE *stdout);
void blockUnParse(struct blockNode *blk,int t,FILE *stdout);

//factor = ident | number | "(" expression ")".
void factorUnParse(factorNode_t *fn,int t,FILE *stdout){
	switch (fn->tag){
		case F_IDENT:{
			CompilerStdout(stdout,"%s",fn->element.ident.string);
		}break;
		case F_DIGIT:{
			CompilerStdout(stdout,"%d",fn->element.digit.value);
		}break;
		case F_EXP:{
			CompilerStdout(stdout,"(");
			if(fn->element.exp != NULL){
				expressionUnParse(fn->element.exp,0,stdout);
			}
			CompilerStdout(stdout,")");
		}break;
		default:{
			
		}
	}

}
void factorExpUnParse(factorExp_t *fe,int t,FILE *stdout){
	CompilerStdout(stdout,"%s",getSpace(t));
	if((fe->op.type == multsym)||(fe->op.type == slashsym)){
		CompilerStdout(stdout,"%s ",opSymbol[fe->op.type]);
	}
	if(fe->factor != NULL){
		factorUnParse(fe->factor,0,stdout);
	}
}
void termUnParse(termNode_t *tm,int t,FILE *stdout){
	factorExp_t * ptr = tm->factorEList;
	while(ptr != NULL){
		factorExpUnParse(ptr,1,stdout);
		ptr = ptr->next;
	}
}


void termExpUnParse(termExp_t *te, int t,FILE *stdout){
	//CompilerStdout(stdout,"%s",getSpace(t));
	if((te->op.type == plussym || te->op.type == minussym))
		CompilerStdout(stdout,"%s",opSymbol[te->op.type]);
	if(te->term != NULL){
		termUnParse(te->term,0,stdout);
	}


}
/*expression = [ "+"|"-"] term { ("+"|"-") term}.*/
void expressionUnParse(expressionNode_t *exp,int t,FILE *stdout){
	termExp_t *ptr = exp->termEList;	
	while(ptr != NULL){
		termExpUnParse(ptr,t+1,stdout);
		ptr = ptr->next;
	}
	
}
void conditionUnParse(conditionNode_t *cond,int t,FILE *stdout){
	if(cond->op.type == oddsym){
		CompilerStdout(stdout,"odd");
		if(cond->right != NULL){
			expressionUnParse(cond->right,0,stdout);
		}
	}else{		
		if(cond->left != NULL){
			expressionUnParse(cond->left,0,stdout);
		}
		CompilerStdout(stdout," %s",opSymbol[cond->op.type]);
		if(cond->right != NULL){
			expressionUnParse(cond->right,0,stdout);
		}
		
	}
}

void assignSNodeUnParse(assignSNode_t * as,int t,FILE *stdout){
	CompilerStdout(stdout,"%s",getSpace(t));
	CompilerStdout(stdout,"%s :=",as->ident.string);
	if(as->exp != NULL){
		expressionUnParse(as->exp,0,stdout);
	}
	CompilerStdout(stdout,";\n");
	
}

void beginSNodeUnParse(beginSNode_t * be,int t,FILE *stdout){
	CompilerStdout(stdout,"%sbegin\n\n",getSpace(t));
	statementNode_t *ptr = be->stsList;
	while(ptr != NULL){
		statementUnParse(ptr,t+1,stdout);
		ptr = ptr->next;
	}
	CompilerStdout(stdout,"\n%send",getSpace(t));
}

void ifSNodeUnParse(ifSNode_t * ifs, int t,FILE *stdout){
	CompilerStdout(stdout,"%sif",getSpace(t));
	if(ifs->cond != NULL){
		conditionUnParse(ifs->cond,0,stdout);
	}
	CompilerStdout(stdout," then\n");
	if(ifs->ifsts != NULL){
		statementUnParse(ifs->ifsts,t+1,stdout);
	}
	
	if(ifs->elsests != NULL){
		CompilerStdout(stdout,"%selse\n",getSpace(t));
		statementUnParse(ifs->elsests,t+1,stdout);
	}

}
void whileSNodeUnParse(whileSNode_t * wh,int t,FILE *stdout){
	CompilerStdout(stdout,"%s",getSpace(t));
	CompilerStdout(stdout,"while");
	if(wh->cond != NULL){
		conditionUnParse(wh->cond,0,stdout);
	}
	CompilerStdout(stdout," do\n");
	if(wh->sts != NULL){
		statementUnParse(wh->sts,t+1,stdout);
	}
	CompilerStdout(stdout,"\n");
}
void readSNodeUnParse(readSNode_t * rd, int t,FILE *stdout){
	CompilerStdout(stdout,"%s",getSpace(t));
	CompilerStdout(stdout,"read %s;\n",rd->ident.string);
	
}
void writeSNodeUnParse(writeSNode_t *wr, int t,FILE *stdout){
	CompilerStdout(stdout,"%s",getSpace(t));
	CompilerStdout(stdout,"write %s;\n",wr->ident.string);
}
void callNodeUnParse(callNode_t * cl,int t,FILE *stdout){
	CompilerStdout(stdout,"%s",getSpace(t));
	CompilerStdout(stdout,"call %s;\n",cl->ident.string);
}

/*
statement = [ ident ":=" expression 
                   | "call" ident 
                  | "begin" statement {";" statement } "end" 
                  | "if" condition "then" statement 
                  | "while" condition "do" statement
                  |"read" ident
                  |"write" ident
                  |e].

*/
void statementUnParse(statementNode_t *sts,int t,FILE *stdout){
	switch (sts->tag){
		case ASSIGN_S:{
			assignSNodeUnParse(sts->super.assigns_p,t,stdout);
		}break;
		case CALL_S:{
			callNodeUnParse(sts->super.calls_p,t,stdout);
		}break;
		case BEGIN_S:{
			beginSNodeUnParse(sts->super.begins_p,t,stdout);
		}break;
		case IF_S:{
			ifSNodeUnParse(sts->super.ifs_p,t,stdout);
		}break;
		case WHILE_S:{
			whileSNodeUnParse(sts->super.whiles_p,t,stdout);
		}break;
		case READ_S:{
			readSNodeUnParse(sts->super.reads_p,t,stdout);
		}break;
		case WRITE_S:{
			writeSNodeUnParse(sts->super.writes_p,t,stdout);
		}break;
		default:{

		}

	}
	
}

void constDeclUnParse(constDeclNode_t *del,int t,FILE *stdout){
	CompilerStdout(stdout,"%s = %d",del->ident.string,del->numer.value);
}

void procDeclUnParse(prodefNode_t *proc,int t,FILE *stdout){
	CompilerStdout(stdout,"%s",getSpace(t));
	CompilerStdout(stdout,"procedure %s;\n",proc->ident.string);
	if(proc->body != NULL){
		blockUnParse(proc->body,t+1,stdout);
	}
	CompilerStdout(stdout,";\n");
}
void varDeclUnParse(varDeclNode_t *var,int t,FILE *stdout){
	CompilerStdout(stdout,"%s",var->ident.string);
}
/*
block = [ "const" ident "=" number {"," ident "=" number} ";"]
            [ "var" ident {"," ident} ";"]
            { "procedure" ident ";" block ";" } statement .

*/
void blockUnParse(struct blockNode *blk,int t,FILE *stdout){
	if(blk->numsConstDecl != 0){
		CompilerStdout(stdout,"%sconst ",getSpace(t));
		constDeclNode_t *ptr = blk->constDecl;
		while(ptr != NULL){
			constDeclUnParse(ptr,t+1,stdout);			
			if(ptr->next != NULL){
				CompilerStdout(stdout,",");
			}
			ptr = ptr->next;
		}
		CompilerStdout(stdout,";\n");
	}
	if(blk->numsVarDecl != 0){
		CompilerStdout(stdout,"%svar ",getSpace(t));
		varDeclNode_t *ptr = blk->varDecl;
		while(ptr != NULL){
			varDeclUnParse(ptr,1,stdout);
			if(ptr->next != NULL){
				CompilerStdout(stdout,",");
			}
			ptr = ptr->next;
		}
		CompilerStdout(stdout,";\n");
	}
	if(blk->numsProcDef != 0){
		prodefNode_t *ptr = blk->procDef;
		while(ptr != NULL){
			procDeclUnParse(ptr,t,stdout);
			ptr = ptr->next;
		}
		
	}
	if(blk->numsStatement != 0){
		statementNode_t *ptr = blk->statement;
		while(ptr != NULL){
			statementUnParse(ptr,t,stdout);
			ptr = ptr->next;
		}
	}
}
//program = block "." .
void programUnParse(struct Parse *this,FILE *stdout){
	programNode_t *ast = this->ast;
	if(ast == NULL)
		return;
	blockNode_t *ptr = ast->block;
	while(ptr != NULL){
		blockUnParse(ptr,1,stdout);
		ptr = ptr->next;
	}
	CompilerStdout(stdout,".\n");
}

void unParsePrint(struct Parse *this,FILE *stdout){
	this->stdout = stdout;
	programUnParse(this,this->stdout);
	this->stdout = NULL;

}
