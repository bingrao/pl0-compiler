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
#include "scanner.h"

void scanner_exit(struct Scanner *this){
	struct DFA *dfa = this->dfa;
	dfa->exit(dfa);
}


static inline int putSymbol(struct Scanner *this){

	LexemeEntry_t* currToken = this->dfa->tokenTable;
	LexemeEntry_t* nextToken = NULL;
	LexemeEntry_t* pToken = NULL;
	LexemeEntry_t* next2Token = NULL;
	struct SymTable *symbolTable = this->symbolTable;
	int numLex = 0;
	boolean errFlag = FALSE;
	Symbol_t * node = NULL;

	while(currToken != NULL){
		nextToken = currToken;
		numLex = 0;
		while((nextToken->type != semicolonsym)&&(nextToken->type != periodsym)){
			numLex++;
			nextToken = nextToken->next;
		}
		switch(currToken->type){
			case constsym:
			{
				if(numLex%4 == 0){
					pToken = currToken->next;
					while(pToken != nextToken->next){
						node = (Symbol_t *)calloc(1,sizeof(Symbol_t));
						if(node == NULL){
							logerror("new a symbol node failed constsym\n");
							errFlag = TRUE;
							break;
						}
						node->kind = 1;
						if(pToken->type == identsym){
							node->name = strdup(pToken->buffer);
							pToken = pToken->next->next;
						}else{
							logerror("Shoule be indent constsym\n");
							errFlag = TRUE;
							free(node);
							break;
						}
						if(pToken->type == numbersym){
							node->val = atoi(pToken->buffer);
							pToken = pToken->next->next;
						}else{
							logerror("Shoule be numbers constsym\n");
							free(node);
							errFlag = TRUE;
							break;
						}
						symbolTable->putSymbol(symbolTable,node->name,node);
						node = NULL;
					}
				}else{
					errFlag = TRUE;
					logerror("There are some error in const definition\n");
				}
			}break;
			case varsym:
			{
				if(numLex%2 == 0){
					pToken = currToken->next;
					while(pToken != nextToken->next){
						node = (Symbol_t *)calloc(1,sizeof(Symbol_t));
						if(node == NULL){
							logerror("new a symbol node failed varsym\n");
							break;
						}
						node->kind = 2;
						if(pToken->type == identsym){
							node->name = strdup(pToken->buffer);
							pToken = pToken->next->next;
						}else{
							logerror("Shoule be indent varsym\n");
							free(node);
							errFlag = TRUE;
							break;
						}
						symbolTable->putSymbol(symbolTable,node->name,node);
						node = NULL;
					}
				}else{
					errFlag = TRUE;
					logerror("There are some error in var definition\n");

				}
			}break;
			case procsym:
			{
				if(numLex == 2){
					pToken = currToken->next;
					while(pToken != nextToken->next){
						node = (Symbol_t *)calloc(1,sizeof(Symbol_t));
						if(node == NULL){
							logerror("new a symbol node failed procsym\n");
							break;
						}
						node->kind = 3;
						if(pToken->type == identsym){
							node->name = strdup(pToken->buffer);
							pToken = pToken->next->next;
						}else{
							logerror("Shoule be indent in [procsym]\n");
							free(node);
							errFlag = TRUE;
							break;
						}
						symbolTable->putSymbol(symbolTable,node->name,node);
						node = NULL;
					}
				}else{
					errFlag = TRUE;
					logerror("There are some error in procedure definition\n");
				}
			}break;
		}
		if(errFlag == TRUE){
			loginfo("numsOfLex:%3d ",numLex);
			while(currToken != nextToken->next){
				logpretty("<%s,%d> ",currToken->buffer,currToken->type);
				currToken = currToken->next;
			}
			logpretty("\n");
		}
		currToken = nextToken->next;
	}
	return 0;
}

static inline void _printLexmeList(struct Scanner *this,char *path){
	DFA_T *dfa = this->dfa;
	dfa->printLexme(dfa,path);
}


static inline void _outputLexmeList(struct Scanner *this,FILE *out){
	DFA_T *dfa = this->dfa;
	dfa->outputLexme(dfa,out);
}

int scanner_run(struct Scanner *this,char *path){
	struct DFA *dfa = this->dfa;

	FILE* fin = fopen(path,"r");
	if(fin == NULL){
		logerror("Open file failed, please double check: %s\n",path);
		return -1;
	}

	while(!feof(fin)){
		dfa->getNextLexeme(dfa,fin);
	}
	fclose(fin);
	#if 0	
	if(this->symbolTable != NULL)
		this->putSymbol(this);
	#endif
	return 0;
}

Scanner_t * scanner_init(struct SymTable * syms){
	Scanner_t *scanner = (Scanner_t *)calloc(1,sizeof(Scanner_t));
	if(scanner == NULL){
		logerror("Apply for scanner memory failed\n");
		return NULL;
	}
	scanner->dfa = dfa_init();
	if(scanner->dfa == NULL){
		logerror("Init DFA failed\n");
		free(scanner);
		return NULL;
	}	
	scanner->symbolTable = syms;
	scanner->run = scanner_run;
	scanner->exit = scanner_exit;
	scanner->printLexmeList = _printLexmeList;
	scanner->putSymbol = putSymbol;
	scanner->outputLexmeList = _outputLexmeList;
	logdebug("Initial Scanner successfully\n");
	return scanner;
}
