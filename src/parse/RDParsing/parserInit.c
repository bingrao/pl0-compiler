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


void statementCleanup(statementNode_t *sts);
void expressionCleanup(expressionNode_t *exp);
void blockCleanup(struct blockNode *blk);


void factorCleanup(factorNode_t *fn){
	switch (fn->tag){
		case F_IDENT:{
			
		}break;
		case F_DIGIT:{
			
		}break;
		case F_EXP:{
			if(fn->element.exp != NULL){
				expressionCleanup(fn->element.exp);
				free(fn->element.exp);
			}
		}break;
		default:{
			
		}
	}
}
void factorExpCleanup(factorExp_t *fe){
	if(fe->factor != NULL){
		factorCleanup(fe->factor);
		free(fe->factor);
	}
}
void termCleanup(termNode_t *tm){
	factorExp_t *ptr = tm->factorEList;
	factorExp_t *del = NULL;
	while(ptr != NULL){
		factorExpCleanup(ptr);
		del = ptr;
		ptr = ptr->next;
		free(del);
	}
}


void termExpCleanup(termExp_t *te){
	if(te->term != NULL){
		termCleanup(te->term);
		free(te->term);
	}


}
void expressionCleanup(expressionNode_t *exp){
	termExp_t *ptr = exp->termEList;
	termExp_t *del = NULL;
	while(ptr != NULL){
		termExpCleanup(ptr);
		del= ptr;
		ptr = ptr->next;
		free(del);
	}
	
}
void conditionCleanup(conditionNode_t *cond){
	
	if(cond->left != NULL){
		expressionCleanup(cond->left);
		free(cond->left);
	}
	if(cond->right != NULL){
		expressionCleanup(cond->right);
		free(cond->right);
	}
}

void assignSNodeCleanup(assignSNode_t * as){
	if(as->exp != NULL){
		expressionCleanup(as->exp);
		free(as->exp);
	}
	return;
}

void beginSNodeCleanup(beginSNode_t * be){
	statementNode_t *ptr = be->stsList;
	statementNode_t *del = NULL;
	while(ptr != NULL){
		statementCleanup(ptr);
		del = ptr;
		ptr = ptr->next;
		free(del);
	}
}

void ifSNodeCleanup(ifSNode_t * ifs){
	if(ifs->cond != NULL){
		conditionCleanup(ifs->cond);
		free(ifs->cond);
	}
	if(ifs->ifsts != NULL){
		statementCleanup(ifs->ifsts);
		free(ifs->ifsts);
	}
	if(ifs->elsests != NULL){
		statementCleanup(ifs->elsests);
		free(ifs->ifsts);
	}

}
void whileSNodeCleanup(whileSNode_t * wh){
	if(wh->cond != NULL){
		conditionCleanup(wh->cond);
		free(wh->cond);
	}
	if(wh->sts != NULL){
		statementCleanup(wh->sts);
		free(wh->sts);
	}
}
void readSNodeCleanup(readSNode_t * rd){
	return;
	
}
void writeSNodeCleanup(writeSNode_t *wr){
	return;
}
void callNodeCleanup(callNode_t * cl){
	return;
}
void statementCleanup(statementNode_t *sts){
	switch (sts->tag){
		case ASSIGN_S:{
			assignSNodeCleanup(sts->super.assigns_p);
			free(sts->super.assigns_p);
		}break;
		case BEGIN_S:{
			beginSNodeCleanup(sts->super.begins_p);
			free(sts->super.begins_p);
		}break;
		case IF_S:{
			ifSNodeCleanup(sts->super.ifs_p);
			free(sts->super.ifs_p);
		}break;
		case WHILE_S:{
			whileSNodeCleanup(sts->super.whiles_p);
			free(sts->super.whiles_p);
		}break;
		case READ_S:{
			readSNodeCleanup(sts->super.reads_p);
			free(sts->super.reads_p);
		}break;
		case WRITE_S:{
			writeSNodeCleanup(sts->super.writes_p);
			free(sts->super.writes_p);
		}break;
		case CALL_S:{
			callNodeCleanup(sts->super.calls_p);
			free(sts->super.calls_p);
		}break;
		default:{

		}

	}
	
}

void constDeclCleanup(constDeclNode_t *del){
	return;
}

void procDeclCleanup(prodefNode_t *proc){
	if(proc->body != NULL){
		blockCleanup(proc->body);
	}
	return;

}
void varDeclCleanup(varDeclNode_t *var){
	return;
}

void blockCleanup(struct blockNode *blk){
	if(blk->numsConstDecl != 0){
		constDeclNode_t *ptr = blk->constDecl;
		constDeclNode_t *del = NULL;
		while(ptr != NULL){
			constDeclCleanup(ptr);
			del = ptr;
			ptr = ptr->next;
			free(del);
		}
	}
	if(blk->numsProcDef != 0){
		prodefNode_t *ptr = blk->procDef;
		prodefNode_t *del = NULL;
		while(ptr != NULL){
			procDeclCleanup(ptr);
			del = ptr;
			ptr = ptr->next;
			free(del);
		}
		
	}
	if(blk->numsStatement != 0){
		statementNode_t *ptr = blk->statement;
		statementNode_t *del = NULL;
		while(ptr != NULL){
			statementCleanup(ptr);
			del = ptr;
			ptr = ptr->next;
			free(del);
		}
	}
	if(blk->numsVarDecl != 0){
		varDeclNode_t *ptr = blk->varDecl;
		varDeclNode_t *del = NULL;
		while(ptr != NULL){
			varDeclCleanup(ptr);
			del = ptr;
			ptr = ptr->next;
			free(del);
		}
	}

}

void programCleanup(struct Parse *this){
	programNode_t *ast = this->ast;
	if(ast != NULL){	
		blockNode_t *ptr = ast->block;
		blockNode_t *del = NULL;
		while(ptr != NULL){
			/*clean current node*/
			blockCleanup(ptr);
			del = ptr;
			ptr = ptr->next;
			free(del);
		}
		free(ast);
	}
}
void cleanup(struct Parse *this){
	programCleanup(this);
}

void parse_exit(struct Parse *this){
	this->cleanup(this);
}

int run(struct Parse *this,FILE *tFile){
	int ret = 0;
	this->tokenFile = tFile;
	this->tokenCount = 0;
	ret = this->parse(this);
	this->tokenFile = NULL;
	return ret;
}
extern void unParsePrint(struct Parse *this,FILE *stdout);
extern int parse(struct Parse *this);
extern void printAST(struct Parse *this,FILE *stdout);
struct Parse * parse_init(struct SymTable *syms){
	Parse_t *parser = (Parse_t *)calloc(1,sizeof(Parse_t));
	if(parser == NULL){
		logerror("Apply for parser memory failed\n");
		return NULL;
	}
	parser->ast = NULL;
	parser->tokenFile = NULL;
	parser->stdout = NULL;
	parser->symbolTable = syms;
	parser->tokenCount = 0;
	parser->cleanup = cleanup;
	parser->parse = parse;
	parser->exit = parse_exit;
	parser->run = run;
	parser->unParsePrint = unParsePrint;
	parser->printAST = printAST;
	firstAndFollowSet(parser);
	logdebug("Initial parse and code generator successfully\n");
	return parser;
}
