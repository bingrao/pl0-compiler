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

void statementPrintAST(statementNode_t *sts,int t,FILE *stdout);
void expressionPrintAST(expressionNode_t *exp,int t,FILE *stdout);
void blockPrintAST(struct blockNode *blk,int t,FILE *stdout);
void identPrintAST(identNode_t *idt,int t,FILE *stdout){
	CompilerStdout(stdout,"%sIdent{ %s }",getSpace(t),idt->string);
}
void numberPrintAST(numberNode_t *num,int t,FILE *stdout){
	CompilerStdout(stdout,"%sNumber{ %d }",getSpace(t),num->value);
}
void relOpNodePrintAST(relOpNode_t *op,int t,FILE *stdout){
	CompilerStdout(stdout,"%sRelOp{ Type[%d] %s }\n",getSpace(t),op->type,opSymbol[op->type]);
}

void ariOpNodepmPrintAST(AriOpNodepm_t *op,int t,FILE *stdout){
	CompilerStdout(stdout,"%sArithOP[+,-]{Type[%d] %s }\n",getSpace(t),op->type,opSymbol[op->type]);
}

void ariOpNodemdPrintAST(AriOpNodemd_t *op,int t,FILE *stdout){
	CompilerStdout(stdout,"%sArithOp[*,/]{Type[%d] %s }\n",getSpace(t),op->type,opSymbol[op->type]);
}


//factor = ident | number | "(" expression ")".
void factorPrintAST(factorNode_t *fn,int t,FILE *stdout){
	CompilerStdout(stdout,"%sFactorExp{Tag[%d]\n",getSpace(t),fn->tag);
	switch (fn->tag){
		case F_IDENT:{
			identPrintAST(&fn->element.ident,t+1,stdout);
			CompilerStdout(stdout,"\n");
		}break;
		case F_DIGIT:{
			numberPrintAST(&fn->element.digit,t+1,stdout);
			CompilerStdout(stdout,"\n");
		}break;
		case F_EXP:{
			if(fn->element.exp != NULL){
				expressionPrintAST(fn->element.exp,t+1,stdout);
			}
		}break;
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));

}
void factorExpPrintAST(factorExp_t *fe,int t,FILE *stdout){
	CompilerStdout(stdout,"%sFactorExp{\n",getSpace(t));
	ariOpNodemdPrintAST(&fe->op,t+1,stdout);
	if(fe->factor != NULL){
		factorPrintAST(fe->factor,t+1,stdout);
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}
void termPrintAST(termNode_t *tm,int t,FILE *stdout){
	CompilerStdout(stdout,"%sTerm{nums[%d]\n",getSpace(t),tm->numsFacorExp);
	factorExp_t * ptr = tm->factorEList;
	while(ptr != NULL){
		factorExpPrintAST(ptr,t+1,stdout);
		ptr = ptr->next;
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}


void termExpPrintAST(termExp_t *te, int t,FILE *stdout){
	CompilerStdout(stdout,"%sTermExp{\n",getSpace(t));
	ariOpNodepmPrintAST(&te->op,t+1,stdout);
	if(te->term != NULL){
		termPrintAST(te->term,t+1,stdout);
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}
/*expression = [ "+"|"-"] term { ("+"|"-") term}.*/
void expressionPrintAST(expressionNode_t *exp,int t,FILE *stdout){
	CompilerStdout(stdout,"%sExpression{numsOfExp[%d]\n",getSpace(t),exp->numsTermExp);
	termExp_t *ptr = exp->termEList;	
	while(ptr != NULL){
		termExpPrintAST(ptr,t+1,stdout);
		ptr = ptr->next;
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));	
}

void conditionPrintAST(conditionNode_t *cond,int t,FILE *stdout){
	CompilerStdout(stdout,"%sCondition{\n",getSpace(t));
	relOpNodePrintAST(&cond->op,t+1,stdout);	
	if(cond->left != NULL){
		expressionPrintAST(cond->left,t+1,stdout);
	}
	if(cond->right != NULL){
		expressionPrintAST(cond->right,t+1,stdout);
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));	
}

void assignSNodePrintAST(assignSNode_t * as,int t,FILE *stdout){
	CompilerStdout(stdout,"%sAssignment{\n",getSpace(t));
	identPrintAST(&as->ident,t+1,stdout);
	CompilerStdout(stdout,"\n");
	if(as->exp != NULL){
		expressionPrintAST(as->exp,t+1,stdout);
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));	
}

void beginSNodePrintAST(beginSNode_t * be,int t,FILE *stdout){
	CompilerStdout(stdout,"%sbegin{numsOfStatement[%d]\n",
		getSpace(t),be->numsStatement);
	statementNode_t *ptr = be->stsList;
	while(ptr != NULL){
		statementPrintAST(ptr,t+1,stdout);
		ptr = ptr->next;
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}

void ifSNodePrintAST(ifSNode_t * ifs, int t,FILE *stdout){
	CompilerStdout(stdout,"%sif{\n",getSpace(t));
	if(ifs->cond != NULL){
		conditionPrintAST(ifs->cond,t+1,stdout);
	}
	if(ifs->ifsts != NULL){
		statementPrintAST(ifs->ifsts,t+1,stdout);
	}
	if(ifs->elsests != NULL){
		statementPrintAST(ifs->elsests,t+1,stdout);
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}
void whileSNodePrintAST(whileSNode_t * wh,int t,FILE *stdout){
	CompilerStdout(stdout,"%sWhile{\n",getSpace(t));
	if(wh->cond != NULL){
		conditionPrintAST(wh->cond,t+1,stdout);
	}
	if(wh->sts != NULL){
		statementPrintAST(wh->sts,t+1,stdout);
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}
void readSNodePrintAST(readSNode_t * rd, int t,FILE *stdout){
	CompilerStdout(stdout,"%sRead{ ",getSpace(t));
	identPrintAST(&rd->ident,1,stdout);
	CompilerStdout(stdout,"%s}\n",getSpace(t));
	
}
void writeSNodePrintAST(writeSNode_t *wr, int t,FILE *stdout){
	CompilerStdout(stdout,"%sWrite{ ",getSpace(t));
	identPrintAST(&wr->ident,1,stdout);
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}
void callNodePrintAST(callNode_t * cl,int t,FILE *stdout){
	CompilerStdout(stdout,"%sCall{ ",getSpace(t));
	identPrintAST(&cl->ident,1,stdout);
	CompilerStdout(stdout,"%s}\n",getSpace(t));
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
void statementPrintAST(statementNode_t *sts,int t,FILE *stdout){
	switch (sts->tag){
		case ASSIGN_S:{
			assignSNodePrintAST(sts->super.assigns_p,t,stdout);
		}break;
		case CALL_S:{
			callNodePrintAST(sts->super.calls_p,t,stdout);
		}break;
		case BEGIN_S:{
			beginSNodePrintAST(sts->super.begins_p,t,stdout);
		}break;
		case IF_S:{
			ifSNodePrintAST(sts->super.ifs_p,t,stdout);
		}break;
		case WHILE_S:{
			whileSNodePrintAST(sts->super.whiles_p,t,stdout);
		}break;
		case READ_S:{
			readSNodePrintAST(sts->super.reads_p,t,stdout);
		}break;
		case WRITE_S:{
			writeSNodePrintAST(sts->super.writes_p,t,stdout);
		}break;
		default:{

		}

	}
	
}

void constDeclPrintAST(constDeclNode_t *del,int t,FILE *stdout){
	CompilerStdout(stdout,"%sConstDeclare{ Lexical[%d],ParentProcedure[%d]",
		getSpace(t),del->lexical,del->paraProc);
	identPrintAST(&del->ident,1,stdout);
	numberPrintAST(&del->numer,1,stdout);
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}

void procDeclPrintAST(prodefNode_t *proc,int t,FILE *stdout){
	CompilerStdout(stdout,"%sProcedure{ Lexical[%d],ParentProcedure[%d],addr[%d]",
		getSpace(t),proc->lexical,proc->paraProc,proc->addr);
	identPrintAST(&proc->ident,1,stdout);
	CompilerStdout(stdout,"\n");
	if(proc->body != NULL){
		blockPrintAST(proc->body,t+1,stdout);
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}
void varDeclPrintAST(varDeclNode_t *var,int t,FILE *stdout){
	CompilerStdout(stdout,"%sVarDeclare{ Lexical[%d],ParentProcedure[%d],addr[%d]",
		getSpace(t),var->lexical,var->paraProc,var->addr);
	identPrintAST(&var->ident,1,stdout);
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}
/*
block = [ "const" ident "=" number {"," ident "=" number} ";"]
            [ "var" ident {"," ident} ";"]
            { "procedure" ident ";" block ";" } statement .

*/
void blockPrintAST(struct blockNode *blk,int t,FILE *stdout){
	CompilerStdout(stdout,"%sBlock{const[%d],var[%d],procedure[%d],statement[%d]\n",
		getSpace(t),blk->numsConstDecl,blk->numsVarDecl,blk->numsProcDef,blk->numsStatement);
	if(blk->numsConstDecl != 0){
		CompilerStdout(stdout,"%sConst{\n",getSpace(t+1));
		constDeclNode_t *ptr = blk->constDecl;
		while(ptr != NULL){
			constDeclPrintAST(ptr,t+2,stdout);			
			ptr = ptr->next;
		}
		CompilerStdout(stdout,"%s}\n",getSpace(t+1));
	}
	if(blk->numsVarDecl != 0){
		CompilerStdout(stdout,"%sVariable{\n",getSpace(t+1));
		varDeclNode_t *ptr = blk->varDecl;
		while(ptr != NULL){
			varDeclPrintAST(ptr,t+2,stdout);
			ptr = ptr->next;
		}
		CompilerStdout(stdout,"%s}\n",getSpace(t+1));
	}
	if(blk->numsProcDef != 0){
		CompilerStdout(stdout,"%sProcedures{\n",getSpace(t+1));
		prodefNode_t *ptr = blk->procDef;
		while(ptr != NULL){
			procDeclPrintAST(ptr,t+2,stdout);
			ptr = ptr->next;
		}
		CompilerStdout(stdout,"%s}\n",getSpace(t+1));
		
	}
	if(blk->numsStatement != 0){
		CompilerStdout(stdout,"%sStatements{\n",getSpace(t+1));
		statementNode_t *ptr = blk->statement;
		while(ptr != NULL){
			statementPrintAST(ptr,t+2,stdout);
			ptr = ptr->next;
		}
		CompilerStdout(stdout,"%s}\n",getSpace(t+1));
	}
	CompilerStdout(stdout,"%s}\n",getSpace(t));
}
//program = block "." .
void programPrintAST(struct Parse *this,FILE *stdout){
	programNode_t *ast = this->ast;
	if(ast != NULL){
		CompilerStdout(stdout,"Program{numBlock: %d\n",ast->numsBlock);
	}else{
		CompilerStdout(stdout,"Program{numBlock: %d}\n",0);
		return;
	}
	blockNode_t *ptr = ast->block;
	while(ptr != NULL){
		blockPrintAST(ptr,1,stdout);
		ptr = ptr->next;
	}
	CompilerStdout(stdout,"}\n");
}
void printAST(struct Parse *this,FILE *stdout){
	this->stdout = stdout;
	programPrintAST(this,this->stdout);
	this->stdout = NULL;
}
