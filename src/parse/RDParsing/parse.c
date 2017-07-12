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

int globalLevel = 0; //gives current level.
int offsetArray[MAX_LEXI_LEVELS]; //holds the offsets of each lexigraphical level
int procedureCount = 0; //holds the number of sub procedures
int currentProcedure = 0; //holds the current procedure
/*
 * For code generation, we are treating the register file as a stack.
 * The register pointer will point to top of "stack".
 * As calculations are performed, the register pointer will be appropriately incremented
 * and decremented. Calculation results should ultimately be stored in reg[0],
 * after which the result will be appropriately be stored back to memory.
 */
int register_ptr = 0;

int inUse[MAX_CODE_SETS]; //stores whether a set is in use or not.
static inline int getFreeSet()
{
    int i;
    for (i = 0; i < MAX_CODE_SETS; i++) {
        if (inUse[i] == 0) {
            inUse[i] = 1;
            return i;
        }
    }
    return -1;
}


/*
    Initializes offsets array. 
*/
static inline void setOffsets()
{
    int x;
    for (x = 0; x < MAX_LEXI_LEVELS; x++){
    	 /*Because in each AR, We reserve 4 size to store
    	              ******
		      LocalVarible
    	              LocalVarible
		      ReturnAddress
    	              DanamicLink,
		      SaticLink,
    	  	      ReturnValue,
    	  	      */
        offsetArray[x] = 4;
    }
}
/*
    Adds a symbol to the symbol table.

    Input:
        token : The identifier of the symbol.
        type : The kind of the symbol.
*/
static inline void makeSymbol(struct Parse *this,char *token, int type){
	CurrentToken_t *currentToken = &this->curToken;
	SymTable_T *symbolTable = this->symbolTable;
	Symbol_t * node = (Symbol_t *)calloc(1,sizeof(Symbol_t));
	if(node == NULL){
		logerror("create new a symbol node failed\n");
		return;
	}else{
		node->name = strdup(token);
		switch(type){
			case CONST_S:{				
				node->kind = CONST_S;
				node->val = currentToken->cTokenVal.numeric;
				node->level = globalLevel;
			}break;
			case VAR_S:{
				node->kind = VAR_S;
				node->val = 0;
				node->addr = offsetArray[globalLevel];
				offsetArray[globalLevel]++;//increment the offset inside by one, for any future variables
				node->level = globalLevel;
			}break;
			case PROC_S:{
				node->kind = PROC_S;
				node->val = 0;
				node->addr = ++procedureCount;
				currentProcedure = getFreeSet();
				node->level = globalLevel;
			}break;
		}
		symbolTable->putSymbol(symbolTable,node->name,node);
		
	}
}
void getToken(struct Parse *this){
	CurrentToken_t *currentToken = &this->curToken;
	if(feof(this->tokenFile)){
		//if end of file, exit
		currentToken->cToken = nulsym;
		return;
	}
	//read the new token in
	fscanf(this->tokenFile, "%d ", (int*)(&currentToken->cToken));
	// update the content if current token is indent
	if(currentToken->cToken == identsym){
		fscanf(this->tokenFile, "%s ", currentToken->cTokenVal.string);
	}
	// update he content if current token is a number.
	if(currentToken->cToken == numbersym){
		fscanf(this->tokenFile, "%d ", &currentToken->cTokenVal.numeric);
	}	
	if(currentToken->cToken == commentsym){
		getToken(this);
	}
	this->tokenCount++;
}

// tests if error occurs and skips all symbols that do not belongs to s1 or s2.  
void test(struct Parse *this,symset s1, symset s2, int errorType)  
{
    symset s;
  	CurrentToken_t *curToken = &this->curToken;
	//logerror("Before symbol[%s:%d][%d]\n",opSymbol[curToken->cToken],curToken->cToken,errorType);
    if (! inset(curToken->cToken, s1))  
    {
    	logerror("[Recovery][%d] Current[%s]\n",
			errorType,opSymbol[curToken->cToken]);
        showset(s1);  
        showset(s2);  
        throwError(errorType);
        s = uniteset(s1, s2);  
        while(!inset(curToken->cToken,s)){
			if(curToken->cToken == nulsym)
				break;
			else{				
				logerror("[Recovery][%d] Skip Symbol[%s]\n",
					errorType,opSymbol[curToken->cToken]);
          		getToken(this);
			}
		}
        destroyset(s);
	}	
	//logerror("After symbol[%s:%d][%d]\n\n",opSymbol[curToken->cToken],curToken->cToken,errorType);
}


Token_t getfollowToken(struct Parse *this){
	Token_t ret = nulsym;
	if(!feof(this->tokenFile)){
		//read the new token in
		fscanf(this->tokenFile, "%d ", (int*)(&ret));
		// rewind the pointer
		fseek(this->tokenFile, -1 * sizeof(char), SEEK_CUR);
	}
	return ret;
}


/*The entry to parse a program*/
int parse(struct Parse *this){
	int ret = 0;
	ret = programParse(this);
	if(ret != 0){ 
		logerror("\n");return ret;
	}
	return ret;
}
/*
 *program ::= block "."
 */
int programParse(struct Parse *this){
	int ret = 0;
	symset set, set1, set2;
	this->ast= (programNode_t *)calloc(1,sizeof(programNode_t));
	if(this->ast == NULL){
		logerror("initial root ast tree failed\n");
		return -1;
	}else{
		this->ast->numsBlock = 0;
		this->ast->block = (blockNode_t*)calloc(1,sizeof(blockNode_t));
		if(this->ast->block == NULL){
			logerror("inital block failed\n");
			return -1;
		}else{
			this->ast->block->next = NULL;
			this->ast->block->numsConstDecl = 0;
			this->ast->block->numsProcDef= 0;
			this->ast->block->numsStatement= 0;
			this->ast->block->numsVarDecl= 0;
		}
	}
	setOffsets();
	/*get current token*/
	getToken(this);
	CurrentToken_t *curToken = &this->curToken;
    currentProcedure = getFreeSet();
	set1 = createset(periodsym,nulsym);
	set2 = uniteset(declbegsys, statbegsys); 
	set = uniteset(set1, set2);
	ret = blockParse(this,this->ast->block,set);		
	destroyset(set1);     
	destroyset(set2);      
	destroyset(set);  
    destroyset(phi);  
    destroyset(relset);  
    destroyset(declbegsys);  
    destroyset(statbegsys);  
    destroyset(facbegsys);
	if(ret != 0){ 
		logerror("\n");return ret;
	}

	if(curToken->cToken != periodsym){
		if(curToken->cToken == nulsym){
			logerror("[Recovery] expect[%s],current[%s] and recovery it\n",
				opSymbol[periodsym],opSymbol[curToken->cToken]);
		}else{
			logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
			throwError(9);
			return -1;
		}
	}	
	this->ast->numsBlock++;
	//TODO generateVariableSpace
    generateVariableSpace(currentProcedure,this->ast->block->numsVarDecl+ 4);
	
	//TODO generateHalt
    generateHalt(currentProcedure);
	return ret;	
}


/*
block = [ "const" ident "=" number {"," ident "=" number} ";"]
            [ "var" ident {"," ident} ";"]
            { "procedure" ident ";" block ";" } statement .

*/
int blockParse(struct Parse *this,struct blockNode *blk,symset fsys){
	int ret = 0;
	symset set1, set;
	CurrentToken_t *curToken = &this->curToken;
do{	
	/*const variable declare*/
	if(curToken->cToken == constsym){
		 //[ "const" ident "=" number {"," ident "=" number} ";"]
		constDeclNode_t *contP = NULL;
		do{
			constDeclNode_t *cont = calloc(1,sizeof(constDeclNode_t));
			if(cont == NULL){
				logerror("initial const declare failed\n");
				return -1;
			}else{
				cont->next = NULL;
				cont->lexical = 0;
				cont->paraProc = 0;
				cont->addr = 0;
				//add a new node to the list first
				if(blk->numsConstDecl++ == 0){
					blk->constDecl = cont;				
					contP = cont;
				}else{
					contP->next = cont;
					contP = cont;
				}
			}
			getToken(this); // get ident
			if(curToken->cToken != identsym){
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				throwError(4);
				return -1;
			}else{
				cont->ident.string = strdup(curToken->cTokenVal.string);
			}
			getToken(this); // get =
			if(curToken->cToken != eqlsym){
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				if(curToken->cToken == becomessym){
					throwError(1);
				}else{
					throwError(3);
				}
				return -1;
			}
			getToken(this); // get number
			if(curToken->cToken != numbersym){
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				throwError(2);
				return -1;
			}else{
				if(curToken->cTokenVal.numeric > MAX_NUMBER){
					logerror("%d > %d",curToken->cTokenVal.numeric,MAX_NUMBER);
					throwError(25);
					return -1;
				}			
				cont->numer.value = curToken->cTokenVal.numeric;
				cont->lexical = globalLevel;
				cont->paraProc = procedureCount;
				cont->addr = offsetArray[globalLevel];
				// make a const symbol
				makeSymbol(this,cont->ident.string,CONST_S);
			}			
			getToken(this); // get comma or semiconlon
		}while(curToken->cToken == commasym);
		if (curToken->cToken != semicolonsym){			
			logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
			throwError(5);
			return -1;
		}
        getToken(this);
	}
	
	/*varible declare*/
	if(curToken->cToken == varsym){
		// [ "var" ident {"," ident} ";"]
		varDeclNode_t *varP = NULL;
		do {
			varDeclNode_t *var = calloc(1,sizeof(varDeclNode_t));
			if(var == NULL){
				logerror("initial vardecl node failed\n");
				return -1;
			}else{
				var->next = NULL;
				var->addr = 0;
				var->lexical = 0;
				var->paraProc = 0;
				
				// Add a new node to the list before handle it.
				if(blk->numsVarDecl++ == 0){
					blk->varDecl = var;
					varP = var;
				}else{
					varP->next = var;
					varP = var;
				}
			}
			getToken(this);// get ident
			if(curToken->cToken != identsym){
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				throwError(4);
				return -1;
			}else{
				var->ident.string = strdup(curToken->cTokenVal.string);
				var->lexical = globalLevel;
				var->paraProc = procedureCount;
				var->addr = offsetArray[globalLevel];
				// make a symbol
				makeSymbol(this,var->ident.string,VAR_S);
			}
			getToken(this);// get next token, that is comma or semicon
		}while(curToken->cToken == commasym);
		if (curToken->cToken != semicolonsym){
			logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
			throwError(5);
			return -1;
		}
        getToken(this);
	}
    int oldProcedure = currentProcedure;
	/*procedure clause*/
	prodefNode_t *procP = NULL;
	while(curToken->cToken == procsym){
		// { "procedure" ident ";" block ";" }
		prodefNode_t *proc = calloc(1,sizeof(prodefNode_t));
		if(proc == NULL){
			logerror("inital proc def failed\n");
			return -1;
		}else{
			proc->next = NULL;
			proc->addr = 0;
			proc->lexical = 0;
			proc->paraProc = 0;
			
			// add new node to the list first
			if(blk->numsProcDef++ == 0){
				blk->procDef = proc;
				procP = proc;
			}else{
				procP->next = proc;
				procP = proc;
			}		
		}
		getToken(this); // get ident, the name of proc
		if(curToken->cToken != identsym){			
			logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
			throwError(4);
			return -1;
		}else{
			proc->ident.string = strdup(curToken->cTokenVal.string);
			proc->lexical = globalLevel+1;
			proc->paraProc = oldProcedure;
			//TODO make a symbol
			makeSymbol(this,proc->ident.string,PROC_S);
			
			proc->addr = procedureCount;
		}
		getToken(this);// get next token
		if(curToken->cToken != semicolonsym){
			logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
			throwError(6);
			return -1;
		}
		getToken(this);// get next token
		proc->body = calloc(1,sizeof(blockNode_t));
		if(proc->body == NULL){
			logerror("initial procedue body failed\n");
			return -1;
		}else{
			proc->body->numsConstDecl = 0;
			proc->body->numsProcDef= 0;
			proc->body->numsStatement= 0;
			proc->body->numsVarDecl= 0;
		}
		/*
    		  Because a porgram is consist of a block, so when parser step in a blcok, which means
    		  there is a new level.
		 */
        globalLevel++;
		set1 = createset(semicolonsym,nulsym);
		set = uniteset(set1, fsys);
		ret = blockParse(this,proc->body,set);
		destroyset(set1);  
	    destroyset(set);  
        globalLevel--;
		if(ret != 0){ 
			logerror("\n");return ret;
		}
		if (curToken->cToken == semicolonsym){
			getToken(this);
			set1 = createset(identsym,procsym,nulsym);
			set = uniteset(statbegsys, set1);
			test(this,set,fsys, 6);
			destroyset(set1);  
			destroyset(set);				
		}else{			
			// at the end of procedure definition, it should be a semicolon symbol
			logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
			throwError(6);
			return -1;
		}
		// at this point, the definition of a precedure is finished and  generate var space
        generateVariableSpace(currentProcedure, proc->body->numsVarDecl+ 4);
	}
	//restore the curretn procedure pointer.
    currentProcedure = oldProcedure;
	set1 = createset(identsym,nulsym);
	set = uniteset(statbegsys, set1);
	test(this,set,declbegsys,7);	
	destroyset(set1);
	destroyset(set);	
}while(inset(curToken->cToken,declbegsys));
	/*statement clause*/
	blk->statement = calloc(1,sizeof(statementNode_t));
	if(blk->statement == NULL){
		logerror("initial a statement failed\n");
		return -1;
	}else{
		blk->numsStatement++;
		set1 = createset(semicolonsym,endsym,nulsym);
		set = uniteset(set1, fsys);
		ret = statementParse(this,blk->statement,set);
		destroyset(set1);  
    	destroyset(set);  
		if(ret != 0){ 
			logerror("\n");return ret;
		}
	}	
	test(this,fsys,phi,8);// test for error: Follow the statement is an incorrect symbol.
	return ret;
}
/*
 statement	 ::=    [ ident ":=" expression
 			 | "call" ident
			 | "begin" statement { ";" statement } "end" 
			 | "if" condition "then" statement 
		         | "while" condition "do" statement
			 | "read"  ident 
		 	 | "write" ident] .

 */
int statementParse(struct Parse *this,statementNode_t *sts,symset fsys){
	int ret =0;
	symset set1, set;  
	CurrentToken_t *curToken = &this->curToken;
	SymTable_T *symTable = this->symbolTable;
	switch (curToken->cToken){
		case identsym: {
			// assignment statement, it looks like " ident ":=" expression" 
			assignSNode_t *ass = calloc(1,sizeof(assignSNode_t));
			if(ass == NULL){
				logerror("initial a assignment state failed\n");
				return -1;
			}else{
				sts->super.assigns_p = ass;
				sts->tag = ASSIGN_S;
				ass->exp = NULL;
				ass->ident.string = strdup(curToken->cTokenVal.string);
			}
			//TODO get the symbol from table and check
			struct Symbol *sym = symTable->getSymbol(symTable,ass->ident.string,globalLevel);
			if(sym == NULL){
				logerror("Assignment Statement[%s:%d]",ass->ident.string,globalLevel);throwError(11);
			}else{
				if(sym->kind != VAR_S){
					logerror("Assignment Statement");throwError(12);
				}
			}
			getToken(this);// get ":="
			if(curToken->cToken != becomessym){

				if(curToken->cToken == eqlsym)
				{
					logerror("[Recovery] expect[%s],current[%s] and recovery it\n",
						opSymbol[becomessym],opSymbol[curToken->cToken]);
				}else{
					logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
					throwError(30);
					return -1;
				}
			}
			
			getToken(this); //get experession
			ass->exp = calloc(1,sizeof(expressionNode_t));
			if(ass->exp == NULL){
				logerror("initial a expression failed\n");
				return -1;
			}else{
				ass->exp->numsTermExp = 0;
				ass->exp->termEList = NULL;
			}
			ret = expressionParse(this,ass->exp,fsys);
			if(ret != 0){ 
				logerror("\n");return ret;
			}
			// Generate assig code
			generateBecomes(currentProcedure,register_ptr-1,sym,globalLevel);
			register_ptr--;//release register when the value has been store into the variable.			
		}break;
		case callsym: {
			// "call" ident
			callNode_t *call = calloc(1,sizeof(callNode_t));
			if(call == NULL){
				logerror("initial call statement failed\n");
				return -1;
			}else{
				sts->super.calls_p = call;
				sts->tag = CALL_S;
			}
			getToken(this); // get ident
			if(curToken->cToken != identsym){
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				throwError(14);
				return -1;
			}else{
				call->ident.string = strdup(curToken->cTokenVal.string);
			}			
			// get symbol from table and check
			struct Symbol *sym = symTable->getSymbol(symTable,call->ident.string,globalLevel);
			if(sym == NULL){
				logerror("Call Statement [%s,%d]",call->ident.string,globalLevel);
				throwError(11);
				return -1;
			}else{
				if(sym->kind != PROC_S){
					logerror("Call Statement");
					throwError(15);
					return -1;
				}else{
					// generate call code
					generateCall(currentProcedure, sym, globalLevel);
				}
			}
			getToken(this);
		}break;
		case beginsym: {
			//"begin" statement {";" statement } "end" 
			statementNode_t *stsP= NULL;
			set1 = createset(semicolonsym,endsym,nulsym);
			set = uniteset(set1, fsys); 
			beginSNode_t *beginS = calloc(1,sizeof(beginSNode_t));
			if(beginS == NULL){
				logerror("inital begin state failed\n");
				return -1;
			}else{
				sts->super.begins_p = beginS;
				sts->tag = BEGIN_S;
				beginS->numsStatement=0;
				beginS->stsList = NULL;
			}
			beginS->stsList = calloc(1,sizeof(statementNode_t));
			if(beginS->stsList == NULL){
				logerror("initial a statement failed\n");
				return -1;
			}else{
				getToken(this); // get next token
				ret = statementParse(this,beginS->stsList,set);
				if(ret != 0){ 
					logerror("\n");return ret;
				}
				beginS->numsStatement++;
				stsP = beginS->stsList;
			}
		#if 0	
			if((curToken->cToken != semicolonsym)&&(curToken->cToken != endsym)){
				Token_t follow_token = getfollowToken(this);
				if(follow_token != endsym){
					logerror("CurrentToken:[%d,%s],FollowedToken[%d,%s]",
						curToken->cToken,opSymbol[curToken->cToken],
						follow_token,opSymbol[follow_token]);
					throwError(5);
					return -1;
				}
			}
		#endif
			while((curToken->cToken == semicolonsym)||inset(curToken->cToken,statbegsys)){
				if(curToken->cToken == semicolonsym){
					getToken(this); // get next token
				}else{
					logerror("[Recovery] expect[%s],current[%s] and recovery it\n",
						opSymbol[semicolonsym],opSymbol[curToken->cToken]);
					throwError(10);
				}
				statementNode_t *s = calloc(1,sizeof(statementNode_t));
				if(s == NULL){
					logerror("\n");return -1;
				}else{
					stsP->next = s;
					stsP = s;
					beginS->numsStatement++;
				}
				//getToken(this); // get next token
				ret = statementParse(this,s,set);
				if(ret != 0){ 
					logerror("\n");return ret;
				}
			#if 0	
				if((curToken->cToken != semicolonsym)&&(curToken->cToken != endsym)){
					Token_t follow_token = getfollowToken(this);
					if(follow_token != endsym){
						logerror("CurrentToken:[%d,%s],FollowedToken[%d,%s]",
							curToken->cToken,opSymbol[curToken->cToken],
							follow_token,opSymbol[follow_token]);
						throwError(5);
						return -1;
					}
				}
			#endif
			}
			destroyset(set1);  
        	destroyset(set);
			if(curToken->cToken == endsym){				
				getToken(this); // get next token
			}else{
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				throwError(26);
				return -1;
			}
			// generated return function
			if (currentProcedure > 0){
				//no return for main procedure
				generateReturn(currentProcedure);
			}	
		}break;
		case ifsym: {
			// "if" condition "then" statement ["else" statement]
			ifSNode_t *ifs = calloc(1,sizeof(ifSNode_t));
			if(ifs == NULL){
				logerror("initial if statement failed\n");
				return -1;
			}else{
				sts->super.ifs_p = ifs;
				sts->tag = IF_S;
				ifs->cond = NULL;
				ifs->ifsts = NULL;
				ifs->elsests = NULL;
			}			
			getToken(this);
			ifs->cond = calloc(1,sizeof(conditionNode_t));
			if(ifs->cond == NULL){
				logerror("initial condition statement failed\n");
				return -1;
			}else{
				ifs->cond->left = NULL;
				ifs->cond->right = NULL;
			}
			set1 = createset(thensym,dosym,nulsym);
			set = uniteset(set1, fsys);
			ret = conditionParse(this,ifs->cond,set);
			destroyset(set1);  
        	destroyset(set);  
			if(ret != 0){
				logerror("\n");return ret;
			}
			if(curToken->cToken != thensym){
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				throwError(16);
				return -1;
			}else{
				getToken(this);
			}
			// generate a jump function
			int j1 = getCodeLength(currentProcedure);
			generateConditionalJump(currentProcedure,register_ptr-1);
			
			ifs->ifsts = calloc(1,sizeof(statementNode_t));
			if(ifs->ifsts == NULL){
				logerror("initial statement failed\n");
				return -1;
			}else{
				ifs->ifsts->next = NULL;
			}
			ret = statementParse(this,ifs->ifsts,fsys);
			if(ret != 0){ 
				logerror("\n");return ret;
			}
			//when ifsts is executed finsihed, then jump to the end of else statement to execute next statement
			int j2 = getCodeLength(currentProcedure);
			generateJump(currentProcedure);
			
			// update the condition jump to point to the else statment when the condition is false
			changeM(currentProcedure, j1, getCodeLength(currentProcedure));
			
			if(curToken->cToken == elsesym){
				ifs->elsests = calloc(1,sizeof(statementNode_t));
				if(ifs->elsests == NULL){
					logerror("initial statement failed\n");
					return -1;
				}else{
					ifs->elsests->next = NULL;
				}
				getToken(this);
				ret = statementParse(this,ifs->elsests,fsys);
				if(ret != 0){ 
					logerror("\n");return ret;
				}
			}
			register_ptr--;
			// update  the jump statement to point to the next statement.
			changeM(currentProcedure, j2, getCodeLength(currentProcedure));
		}break;
		case whilesym: {
			// "while" condition "do" statement
			whileSNode_t *whileS = calloc(1,sizeof(whileSNode_t));
			if(whileS == NULL){
				logerror("initial while statement failed\n");
				return -1;
			}else{
				sts->super.whiles_p = whileS;
				sts->tag = WHILE_S;
				whileS->cond = NULL;
				whileS->sts = NULL;
			}
			getToken(this);
			int c = getCodeLength(currentProcedure);
			
			whileS->cond = calloc(1,sizeof(conditionNode_t));
			if(whileS->cond == NULL){
				logerror("initial while condition failed\n");
				return -1;
			}else{
				whileS->cond->left = NULL;
				whileS->cond->right = NULL;
			}
		    set1 = createset(dosym,nulsym);
			set = uniteset(set1, fsys);
			ret = conditionParse(this,whileS->cond,set);
			destroyset(set1);  
        	destroyset(set);  
			if(ret != 0){ 
				logerror("\n");return ret;
			}
			if(curToken->cToken == dosym){
				getToken(this);
			}else{				
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				throwError(18);
				return -1;
			}
			// generate a jump  function			
			int j1 = getCodeLength(currentProcedure);
			generateConditionalJump(currentProcedure,register_ptr-1);

			whileS->sts= calloc(1,sizeof(statementNode_t));
			if(whileS->sts == NULL){
				logerror("initial statement failed\n");
				return -1;
			}else{
				whileS->sts->next = NULL;
			}
			ret = statementParse(this,whileS->sts,fsys);
			if(ret != 0){ 
				logerror("\n");return ret;
			}
			// geerate a jubmp function to return the conditon
			int j2 = getCodeLength(currentProcedure);
			generateJump(currentProcedure);
			
			// update both jump function
			changeM(currentProcedure, j1, getCodeLength(currentProcedure));
			changeM(currentProcedure, j2, c);
			register_ptr--;//Done with condition result. Free up register.
		}break;
		case readsym: {
			//"read"  ident 
			readSNode_t *readS = calloc(1,sizeof(readSNode_t));
			if(readS == NULL){
				logerror("initial read statement failed\n");
				return -1;
			}else{
				sts->super.reads_p = readS;
				sts->tag = READ_S;
			}
			getToken(this);
			if(curToken->cToken != identsym){				
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				throwError(27);
				return -1;
			}else{
				readS->ident.string = strdup(curToken->cTokenVal.string);
			}
			//TODO get the symbol from the table and check
			struct Symbol *sym = symTable->getSymbol(symTable,readS->ident.string,globalLevel);
			if(sym == NULL){
				logerror("Read Statement[%s,%d]",readS->ident.string,globalLevel);
				throwError(11);
				return -1;
			}else{
				if(sym->kind != VAR_S){
					logerror("Read Statement");
					throwError(27);
					return -1;
				}else{					
					generateRead(currentProcedure,register_ptr,sym,globalLevel);
				}
			}
			// generate a read function
			getToken(this);
		}break;
		case writesym: {
			//"write"  ident 
			writeSNode_t *writeS = calloc(1,sizeof(writeSNode_t));
			if(writeS == NULL){
				logerror("initial write a statement failed\n");
				return -1;
			}else{			
				sts->super.writes_p = writeS;
				sts->tag = WRITE_S;
			}
			getToken(this);
			if(curToken->cToken != identsym){
				logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
				throwError(28);
				return -1;
			}else{
				writeS->ident.string = strdup(curToken->cTokenVal.string);
			}
			//TODO get the symbol from the table and check
			struct Symbol *sym = symTable->getSymbol(symTable,writeS->ident.string,globalLevel);
			if(sym == NULL){
				logerror("Write Statement[%s,%d]",writeS->ident.string,globalLevel);
				throwError(11);
				return -1;
			}else{
				if((sym->kind == VAR_S)||(sym->kind == CONST_S)){
					// generate a write function
					generateWrite(currentProcedure,register_ptr,sym,globalLevel,sym->kind);
				}else {		
					logerror("Write Statement");
					throwError(27);
					return -1;
				}
			}
			getToken(this);
		}break;
		default: {
			test(this,fsys, phi,19);	
		}
	}
	return ret;
}

/*
condition ::= "odd" expression 
	  	| expression  rel-op  expression.  

*/
int conditionParse(struct Parse *this, conditionNode_t *cond,symset fsys){
	int ret = 0;
	symset set;  
	CurrentToken_t *curToken = &this->curToken;
	cond->right = calloc(1,sizeof(expressionNode_t));
	if(cond->right == NULL){
		logerror("initial experssion left statement failed\n");
		return -1;
	}else{
		cond->right->termEList = NULL;
		cond->right->numsTermExp = 0;
	}
	if(curToken->cToken == oddsym){
		cond->op.type = oddsym;
		getToken(this);// get right experession
		ret = expressionParse(this,cond->right,fsys);
		if(ret != 0){ 
			logerror("\n");return ret;
		}
		// generate a relop
        generateRelOp(currentProcedure,cond->op.type,register_ptr-1,register_ptr-1,0);
	}else{
		cond->left= calloc(1,sizeof(expressionNode_t));
		if(cond->left == NULL){
			logerror("initial experssion right statement failed\n");
			return -1;
		}else{
			cond->left->numsTermExp = 0;
			cond->left->termEList = NULL;
		}
		set = uniteset(relset, fsys);
		ret = expressionParse(this,cond->left,set); // set left experession
		destroyset(set);
		if(ret != 0){ 
			logerror("\n");return ret;
		}
		// generate a relop
		ret = relOp(this);
		if(ret == -1){ 
			logerror("\n");return ret;
		}
		cond->op.type = ret;
		
		getToken(this);//get right experession
		ret = expressionParse(this,cond->right,fsys);
		if(ret != 0){ 
			logerror("[%d]\n",ret);return ret;
		}
        generateRelOp(currentProcedure,cond->op.type,register_ptr-2,register_ptr-2,register_ptr-1);
		register_ptr--;
	}
	return ret;
}

int relOp(struct Parse *this)
{
	int token = this->curToken.cToken;
    if (token != eqlsym && token != neqsym &&
        token != lessym && token != leqsym &&
        token != gtrsym && token != geqsym) {        
		logerror("Token:[%d,%s]\n",token,opSymbol[token]);
        throwError(20);
		return -1;
    }
    return token;
}

/*
expression ::= [ "+"|"-"] term { ("+"|"-") term}.

*/
int expressionParse(struct Parse *this, expressionNode_t *exp,symset fsys){
	int ret = 0;
	int preop = 0;
	CurrentToken_t *curToken = &this->curToken;
	termExp_t *cur = NULL;
	symset set1, set;
	set1=createset(multsym,slashsym,nulsym);
	set = uniteset(fsys,set1);  
    destroyset(set1); 
	
	exp->termEList = calloc(1,sizeof(termExp_t));
	if(exp->termEList == NULL){
		logerror("initial term expe failed\n");
		return -1;
	}else{
		exp->termEList->next = NULL;
		exp->termEList->term = NULL;
	}	
	if((curToken->cToken == plussym)||(curToken->cToken == minussym)){
		 exp->termEList->op.type= curToken->cToken;
		 preop = curToken->cToken;
		 getToken(this);
	}
	exp->termEList->term = calloc(1,sizeof(termNode_t));
	if(exp->termEList->term == NULL){
		logerror("initial a term node failed\n");
		return -1;
	}else{
		exp->termEList->term->factorEList = NULL;
		exp->termEList->term->numsFacorExp = 0;
	}
	ret = termParse(this,exp->termEList->term,preop,set);
	if(ret != 0){ 
		logerror("\n");return ret;
	}
	exp->numsTermExp++;
	cur = exp->termEList;
	while (curToken->cToken== plussym || curToken->cToken == minussym) {		
        int op = curToken->cToken;
		termExp_t *te = calloc(1,sizeof(termExp_t));
		if(te == NULL){			
			logerror("initial term expe failed\n");
			return -1;
		}else{
			cur->next = te;
			cur = te;
			exp->numsTermExp++;
			te->next = NULL;
			te->op.type = op;
			te->term = NULL;
		}		
		te->term = calloc(1,sizeof(termNode_t));
		if(te->term == NULL){
			logerror("initial a term node failed\n");
			return -1;
		}else{
			te->term->factorEList =NULL;
			te->term->numsFacorExp = 0;
		}		
		getToken(this);
		ret = termParse(this,te->term,0,set);
		if(ret != 0){ 
			logerror("\n");return ret;
		}
        generateCalculation(currentProcedure,op,register_ptr-2,register_ptr-2,register_ptr-1);
		register_ptr--;
    }
	destroyset(set);  
	return ret;
}

/*
term ::= factor {("*"|"/") factor}. 

*/
int termParse(struct Parse *this,termNode_t *tm,int preop,symset fsys){
	int ret = 0;
	CurrentToken_t *curToken = &this->curToken;
	factorExp_t *cur = NULL;
	symset set1, set;
	set1= createset(multsym,slashsym,nulsym);
	set = uniteset(fsys,set1);  
    destroyset(set1);
	
	tm->factorEList = calloc(1,sizeof(factorExp_t));
	if(tm->factorEList == NULL){
		logerror("initial a factor exp failed\n");
		return -1;
	}else{
		tm->factorEList->factor = NULL;
		tm->factorEList->next = NULL;
		tm->factorEList->op.type = nulsym;		
	}
	
	tm->factorEList->factor = calloc(1,sizeof(factorNode_t));
	if(tm->factorEList->factor == NULL){
		logerror("initial a factor node failed\n");
		return -1;
	}
	ret = factorParse(this,tm->factorEList->factor,set);
	if(ret != 0){ 
		logerror("\n");return ret;
	}
	tm->numsFacorExp++;
	cur = tm->factorEList;
    switch (preop) {
    case plussym : //plus
        break;
    case minussym: //minus
        generateNegation(currentProcedure,register_ptr-1,register_ptr-1,0);
        break;
    }
    while(curToken->cToken == multsym || curToken->cToken == slashsym) {
		int op = curToken->cToken;
		factorExp_t *fe = calloc(1,sizeof(factorExp_t));
		if(fe == NULL){
			logerror("initial a factor EXP failed\n");
			return -1;
		}else{
			// add the new node to the list first
			cur->next = fe;
			cur = fe;	
			tm->numsFacorExp++;

			
			fe->next = NULL;
			fe->op.type = curToken->cToken; //6 if mult, 7 if div
			fe->factor = calloc(1,sizeof(factorNode_t));
			if(fe->factor == NULL){
				logerror("initial a factor node failed\n");
				return -1;
			}else{
				getToken(this);
				ret = factorParse(this,fe->factor,set);
				if(ret != 0){ 
					logerror("\n");return ret;
				}
				generateCalculation(currentProcedure,op,register_ptr-2,register_ptr-2,register_ptr-1);
				register_ptr--;// release register_ptr-1, and the result store into register_ptr-2
			}
		}
    }
	destroyset(set);
	return ret;
}

/*
 factor ::= ident | number | "(" expression ")".
 */
int factorParse(struct Parse *this,factorNode_t *fn,symset fsys){
	int ret = 0;
	CurrentToken_t *curToken = &this->curToken;
	SymTable_T *symTable = this->symbolTable;
	symset set1, set;
	test(this,facbegsys,fsys, 24);
//while(inset(curToken->cToken,facbegsys)){
	switch (curToken->cToken){
		case identsym:{
			fn->tag = F_IDENT;
			fn->element.ident.string = strdup(curToken->cTokenVal.string);
			//get symbol from the symbol, and check
			struct Symbol *sym = symTable->getSymbol(symTable,fn->element.ident.string,globalLevel);
			if(sym == NULL){
				logerror("Identifier[%s,%d]",fn->element.ident.string,globalLevel);
				throwError(11);
				return -1;
			}else{
				switch (sym->kind){
					case VAR_S:{
						generateLoad(currentProcedure,register_ptr,sym,globalLevel);	
						register_ptr++;//Increment RP.
					}break;
					case CONST_S:{
						generateLiteral(currentProcedure,register_ptr,sym->val);
						register_ptr++;//Increment RP.
					}break;
					case PROC_S:{
						//error
						logerror("Identifier");
						throwError(21);
						return -1;
					}break;
				}
			}
			getToken(this);
    	}break;
		case numbersym:{
			fn->tag = F_DIGIT;
			if(curToken->cTokenVal.numeric > MAX_NUMBER){
				logerror("%d > %d",curToken->cTokenVal.numeric,MAX_NUMBER);
				throwError(25);
				return -1;
			}			
			fn->element.digit.value = curToken->cTokenVal.numeric;
			generateLiteral(currentProcedure,register_ptr,fn->element.digit.value);
			register_ptr++; //Increment RP.
			getToken(this);
    	}break;
		case lparentsym:{
			fn->tag = F_EXP;
			expressionNode_t *exp = calloc(1,sizeof(expressionNode_t));
			if(exp == NULL){
				logerror("initial a expression node failed\n");
				return -1;
			}else{
				fn->element.exp = exp;
				exp->numsTermExp = 0;
				exp->termEList == NULL;
			}			
			getToken(this);
			set1=createset(rparentsym,nulsym);
			set=uniteset(set1,fsys);
			ret = expressionParse(this,exp,set);
			destroyset(set);  
            destroyset(set1);
			if(ret != 0){ 
				logerror("\n");return ret;
			}
			if(curToken->cToken == rparentsym){
				getToken(this);
			}else{				
				set1=createset(lparentsym,nulsym);
				test(this,fsys,set1,22);  
				destroyset(set1);
			}
    	}break;
		default:{
			set1=createset(lparentsym,nulsym);
            test(this,fsys,set1,23);  
            destroyset(set1);  
			logerror("Token:[%d,%s]",curToken->cToken,opSymbol[curToken->cToken]);
			throwError(23);
			return -1;
		}
	}
//}	
	return ret;
}
