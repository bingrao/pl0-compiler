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

#include "dfa.h"

char *resWord[]={"null","begin","call","const","do","else","end",
				 "if","odd","procedure","read","then","var","while","write"};
int wsym[]={nulsym,beginsym,callsym,constsym,dosym,elsesym,endsym,
			ifsym,oddsym,procsym,readsym,thensym,varsym,whilesym,writesym};

/*transverse the lexeme list, update the type info of the reserved word*/
void updateReservedWords(struct DFA* this){
	LexemeEntry_t * lex = &this->curLexeme;
	int i,reg;
	for(i=0;i<MAX_RESERVED_WORD;i++){
		reg = strcmp(resWord[i],lex->buffer);
		if( reg == 0){
			lex->type = wsym[i];
		}
	}
}

/*
 * Get next charactor from the input file,
 * and file pointer point to next next charactor.
 */
static inline int getNextChar(FILE* input){
	return fgetc(input);
}
/*
 * After find a lexeme then reset the DFA machine buffer state
 * and prepare to find next lexeme
 */
void resetMachine(struct DFA* this){
	this->DFAState = D_START;
	this->rewind = FALSE;
	this->MState = M_RUN;

	memset(this->curLexeme.buffer,0,MAX_BUFFER_SIZE);

	this->curLexeme.buffer[0]='\0';
	this->curLexeme.indexBuffer = 0;
	this->curLexeme.type = nulsym;
	this->curLexeme.printable = FALSE;
}

/*
 * If this string has been accepted by DFA, then call this function to 
 * set up the state of machine
 */
void accept(struct DFA* this){
	this->DFAState = D_ACCEPT;
	this->MState =M_HALT;
	this->rewind = TRUE;
}
/*
 * If this string has been rejected by DFA, then call this function to 
 * set up the state of machine
 */

void reject(struct DFA* this){	
	this->curLexeme.type = nulsym;
	this->DFAState =D_REJECT;
	this->MState = M_HALT;
}
/*
 * get the lexeme from the input file, there are two possible return state
 * 1. accept state, find a lexeme successfully
 * 2. reject state, fail to find a lexeme
 */
boolean getNextLexeme(struct DFA* this,FILE *input){
	int inChar;
	this->reset(this);
	LexemeEntry_t *curLex = &this->curLexeme;

	while(this->MState == M_RUN){
		inChar = getNextChar(input);
		if(inChar != EOF){		
			this->column++;
			if(inChar == '\n'){
				this->lastLineLength = this->column;
				this->column = 1;
				this->line++;
			}		
			curLex->buffer[curLex->indexBuffer] = inChar;
			curLex->buffer[++curLex->indexBuffer] = '\0';
			this->transition(this,inChar);
			if(this->rewind == TRUE){
				//re-backward one charactor.
				fseek(input, -1 * sizeof(char), SEEK_CUR);
				curLex->buffer[--curLex->indexBuffer] = '\0';
				this->column--;
				if(this->column <1){
					this->line--;
					this->column = this->lastLineLength;
				}
			}			
		}else{
			break;
		}
	}

	if(this->DFAState == D_ACCEPT){
		
		if(this->curLexeme.type == identsym){
			this->updateReservedWords(this);
		}
		if((this->curLexeme.type == identsym)||(this->curLexeme.type == numbersym))
			// if lexeme is indent or number, then assume it can be printed
			this->curLexeme.printable = TRUE;

		if(this->curLexeme.type != nulsym){
			// add valid lexeme to the LexemeList
			this->putLexeme(this);
		}
		return TRUE;
	}else{
		return FALSE;
	}
}

/*
 * DFA transition function. there are 25 states of DFA, you can find detail in the struct DFA_Q.
 * In this DFA, we assume any reserved keywords as a identifier, after finding a identifier
 * successfully, then call "updateReservedWords" function to update the type info of keywords.
 */
void transition(struct DFA* this,int input){
	switch(this->DFAState){
		case D_START:
		{
			if(isspace(input)||iscntrl(input)){
				this->curLexeme.type = nulsym;
				this->accept(this);
				this->rewind = FALSE;
			}else if(isdigit(input)){
				this->DFAState = D_DIGIT;
				this->curLexeme.type= numbersym;
			}else if(isalpha(input)){
				this->DFAState = D_ALPHA;
				this->curLexeme.type = identsym;
			}else{
				switch(input){
					case '+':
					{
						this->curLexeme.type = plussym;
						this->DFAState = D_PLUS;
					}break;
					case '-':
					{
						this->curLexeme.type = minussym;
						this->DFAState = D_MINUS;
					}break;
					case '*':
					{
						this->curLexeme.type = multsym;
						this->DFAState =D_MULT;
					}break;
					case '/':
					{
						this->curLexeme.type = slashsym;
						this->DFAState = D_DIV;
					}break;
					case '=':
					{
						this->curLexeme.type = eqlsym;
						this->DFAState = D_EQL;
					}break;
					case '<':
					{
						this->curLexeme.type = lessym;
						this->DFAState = D_LESS;
					}break;
					case '>':
					{
						this->curLexeme.type = gtrsym;
						this->DFAState = D_LARE;
					}break;
					case '(':
					{
						this->curLexeme.type = lparentsym;
						this->DFAState = D_LPARE;
					}break;
					case ')':
					{
						this->curLexeme.type = rparentsym;
						this->DFAState =D_RPAPE;
					}break;
					case ',':
					{
						this->curLexeme.type = commasym;
						this->DFAState = D_COMMA;
					}break;
					case ';':
					{
						this->curLexeme.type = semicolonsym;
						this->DFAState = D_SEMI;
					}break;
					case ':':
					{
						this->curLexeme.type = becomessym;
						this->DFAState = D_COLON;
					}break;					
					case '.':
					{
						this->curLexeme.type = periodsym;
						this->DFAState = D_PERIOD;
					}break;
					default:{
						logerror("The input char[%d] is error, the localtion is:[%d,%d]\n",
							input,this->line,this->column);
						this->reject(this);
					}
				}
			}
		}break;
		case D_DIGIT:
		{
			if(isdigit(input)){
				if(this->curLexeme.indexBuffer > MAX_NUMBER_LENGTH){
					logerror("Invalid numerical(%s) length at (%d, %d)\n",
						this->curLexeme.buffer,this->line,this->column);
					this->reject(this);
				}else{
					this->DFAState = D_DIGIT;
				}
			}else if(isalpha(input)){
				logerror("Invalid identifier %s at (%d, %d)\n",
					this->curLexeme.buffer,this->line,this->column);
				this->reject(this);
				
			}else{ // other charactor, then stop and get next lememe.
				this->accept(this);
				this->rewind = TRUE;
			}
			
		}break;
		case D_ALPHA:
		{
			if(isalnum(input)){ // alpha or digital
				if(this->curLexeme.indexBuffer>MAX_IDENT_LENGTH){
					logerror("Identifier too long %s at (%d, %d)\n",
						this->curLexeme.buffer,this->line,this->column);
					this->reject(this);
				}else{
					this->DFAState = D_ALPHA;
				}
			}else{// other charactor
				this->accept(this);
			}
			
		}break;
		case D_PLUS: //"+"
		{
			// final state, and return
			this->accept(this);
		}break;
		case D_MINUS: //"-"
		{
			// final state, and return
			this->accept(this);
		}break;
		case D_MULT: // "*"
		{
			// final state, and return
			this->accept(this);
		}break;
		case D_DIV: // "/"
		{
			if(input == '*'){
				this->DFAState = D_LCOMM;
			}else{
				this->accept(this);
			}
		}break;
		case D_LCOMM:{ // "/*"
			if(input == '*'){ // /*   *
				this->DFAState = D_COMM;
			}else{
				this->DFAState = D_LCOMM;
				this->curLexeme.indexBuffer = 0;
			}
		}break;
		case D_COMM:{// "/* ... *"
			this->curLexeme.indexBuffer = 0;
			if(input == '*')
				this->DFAState = D_COMM;
			else if(input == '/')
				this->DFAState = D_RCOMM;
			else
				this->DFAState = D_LCOMM;			
		}break;
		case D_RCOMM:{// "/*  ... */"
			this->curLexeme.type = commentsym;
			this->curLexeme.buffer[0]='\0';
			this->accept(this);
		}break;
		case D_EQL:
		{
			this->accept(this);
		}break;
		case D_LESS: // <
		{
			if(input == '='){
				this->DFAState = D_LEQ;
			}else if(input == '>'){
				this->DFAState = D_NEQ;
			}else{
				this->accept(this);
			}
		}break;
		case D_LARE: // ">"
		{
			if(input == '='){
				this->DFAState = D_GEQ;
			}else{
				// get the ">"
				this->accept(this);
			}
			
		}break;
		case D_LEQ: // <=
		{
			this->curLexeme.type = leqsym;
			this->accept(this);
		}break;
		case D_GEQ: // >=
		{
			this->curLexeme.type = geqsym;
			this->accept(this);
		}break;
		case D_NEQ: // <>
		{
			this->curLexeme.type = neqsym;
			this->accept(this);
		}break;
		case D_LPARE: // (
		{
			this->accept(this);
		}break;
		case D_RPAPE: // )
		{
			this->accept(this);
		}break;
		case D_COMMA: // ,
		{
			this->accept(this);
		}break;
		case D_SEMI: // ;
		{
			this->accept(this);
		}break;
		case D_PERIOD: // .
		{
			this->accept(this);
		}break;
		case D_COLON: //":"
		{
			if(input == '='){
				this->DFAState = D_BECOMES;
			}else{
				logerror("Invalid input %s at (%d, %d)",
						this->curLexeme.buffer,this->line,this->column);
				this->reject(this);
			}
		}break;
		case D_BECOMES:
		{
			this->curLexeme.type = becomessym;
			this->accept(this);
		}break;
		case D_FINAL:
		{
			this->accept(this);
		}break;
		default:
		{
			logerror("Invalid input %s at (%d, %d)",
						this->curLexeme.buffer,this->line,this->column);
		}
		
	}
}

void _toString(struct DFA* this){
	loginfo("The current DFA Machine state:\n");
	loginfo("Machine State: %d, DFA State: %d\n",this->MState,this->DFAState);
	loginfo("The current location:(%d,%d)\n",this->line,this->column);
	loginfo("The symbol[%d],[%s]\n",this->curLexeme.type,this->curLexeme.buffer);
}

/*Create a new node of lexeme to add it to lexemeList*/
LexemeEntry_t* newNode(char* lexeme, int type, boolean print){
  LexemeEntry_t* retVal=(LexemeEntry_t*)calloc(1,sizeof(LexemeEntry_t));
  strcpy(retVal->buffer,lexeme);
  retVal->type = type;
  retVal->printable = print;
  return retVal;
}
/*Add current lexemed found by DFA to the lexemeList*/
void putLexeme(struct DFA *this){
	if(this->tokenTable == NULL){
		this->tokenTable = newNode(this->curLexeme.buffer,this->curLexeme.type,this->curLexeme.printable);
		this->lastToken = this->tokenTable;
		this->lastToken->next = NULL;
	}else{
		this->lastToken->next = newNode(this->curLexeme.buffer,this->curLexeme.type,this->curLexeme.printable);
		this->lastToken = this->lastToken->next;
		this->lastToken->next = NULL;
	}
}
/*When DFA exit, then clear some resource*/
void dfa_exit(struct DFA *this){
	LexemeEntry_t* root = this->tokenTable;
	LexemeEntry_t* next;
	while(root){
	  next = root->next;
	  free(root);
	  root = next;
	}
}

/*Read a local file, then run DFA until to the end of file*/
int dfa_run(struct DFA *this,char *path){	
	FILE* fin = fopen(path,"r");
	if(fin == NULL){
		logerror("Open file failed, please double check: %s\n",path);
		return -1;
	}

	while(!feof(fin)){
		this->getNextLexeme(this,fin);
	}
	fclose(fin);
	return 0;
}
/*Print the info of current DFA state and lexemes that found by this machine*/
void printLexme(struct DFA *this,char *path){
	LexemeEntry_t* tokenTable = this->tokenTable;
	LexemeEntry_t* currToken = NULL;
	int charIn;
	
    loginfo("\nSource Program:\n");
    FILE* fin = fopen(path,"r");
    while((charIn = fgetc(fin)) != EOF){
      logpretty("%c", (char)charIn);
    }
    fclose(fin);

	if(tokenTable != NULL){
		loginfo("\nLexeme Table:\n");
		logpretty("Lexeme\t\t\tToken\n");
		currToken = tokenTable;
		while(currToken != NULL){
			logpretty("%s\t\t\t%d\n",currToken->buffer,currToken->type);
			currToken = currToken->next;
		}


		currToken = tokenTable;
		while(currToken != NULL){
			if(currToken->printable == TRUE)
				logpretty("%d %s ",currToken->type,currToken->buffer);
			else
				logpretty("%d ",currToken->type);
			currToken = currToken->next;
		}
		logpretty("\n");
	}
}

void outputLexme(struct DFA *this,FILE *out){
	
	LexemeEntry_t* tokenTable = this->tokenTable;
	LexemeEntry_t* currToken = NULL;
	if(tokenTable != NULL){
		currToken = tokenTable;
		while(currToken != NULL){
			if(currToken->printable == TRUE)
				CompilerStdout(out,"%d %s ",currToken->type,currToken->buffer);
			else
				CompilerStdout(out,"%d ",currToken->type);
			currToken = currToken->next;
		}
	}
}
/*Init a DFA, then return a DFA object*/
struct DFA *dfa_init(){
	DFA_T* dfa = (DFA_T*)calloc(1,sizeof(DFA_T));
	if(dfa != NULL){
		dfa->DFAState =D_START;
		dfa->MState = M_IDLE;
		dfa->rewind = FALSE;
		dfa->column = 1;
		dfa->line = 1;
		dfa->lastLineLength = 0;


		memset(dfa->curLexeme.buffer,0,MAX_BUFFER_SIZE);
		dfa->curLexeme.buffer[0]='\0';
		dfa->curLexeme.indexBuffer = 0;
		dfa->curLexeme.type = nulsym;
		dfa->curLexeme.printable = FALSE;

		dfa->transition = transition;
		dfa->reset = resetMachine;
		dfa->accept = accept;
		dfa->reject = reject;
		dfa->exit = dfa_exit;
		dfa->getNextLexeme = getNextLexeme;
		dfa->toString = _toString;
		dfa->updateReservedWords = updateReservedWords;
		dfa->run = dfa_run;
		dfa->putLexeme = putLexeme;
		dfa->printLexme = printLexme;
		dfa->outputLexme = outputLexme;
	}else{
		logerror("Apply for DFA memory failed\n");
	}	
	return dfa;
}
