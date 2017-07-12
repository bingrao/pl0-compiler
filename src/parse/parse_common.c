#include "parse.h"

Token_t **firstSet = NULL;
Token_t **followSet = NULL;

#define NUMS_FIRST  10
Token_t blockFirst[NUMS_FIRST]={constsym,varsym,procsym,identsym,ifsym,callsym,beginsym,whilesym,readsym,writesym};
Token_t statementFirst[NUMS_FIRST]={identsym,callsym,beginsym,ifsym,whilesym,readsym,writesym};
Token_t conditionFirst[NUMS_FIRST]={oddsym,plussym,lparentsym,identsym,numbersym};
Token_t expressionFirst[NUMS_FIRST]={plussym,minussym,lparentsym,identsym,numbersym};
Token_t termFirst[NUMS_FIRST]={identsym,numbersym,lparentsym};
Token_t factorFirst[NUMS_FIRST]={identsym,numbersym,lparentsym};

#define NUMS_FOLLOW	10
Token_t blockFollow[NUMS_FOLLOW]={periodsym,semicolonsym};
Token_t statementFollow[NUMS_FOLLOW]={periodsym,semicolonsym,endsym};
Token_t conditionFollow[NUMS_FOLLOW]={thensym,dosym};
Token_t expressionFollow[NUMS_FOLLOW]={periodsym,semicolonsym,rparentsym,endsym,thensym,dosym};
Token_t termFollow[NUMS_FOLLOW]={periodsym,semicolonsym,rparentsym,plussym,minussym,endsym,thensym,dosym};
Token_t factorFollow[NUMS_FOLLOW]={periodsym,semicolonsym,rparentsym,plussym,minussym,multsym,slashsym,endsym,thensym,dosym};

symset phi = NULL;
symset blockbegsys = NULL;
symset declbegsys = NULL;
symset statbegsys = NULL;
symset condbegsys = NULL;
symset exprbegsys = NULL;
symset facbegsys = NULL;
symset termbegsys = NULL;
symset relset = NULL; 


Token_t *getFirstSet(NonTerminal_t nonTerminal){
	return firstSet[nonTerminal];
}

Token_t *getFollowSet(int nonTerminal){
	return followSet[nonTerminal];
}

int firstAndFollowSet(struct Parse *this){
	firstSet = (Token_t **)calloc(6,sizeof(Token_t *));
	if(firstSet == NULL){
		logerror("initial a memory for first set failed\n");
		return -1;
	}else{
		firstSet[NON_BLOCK] = blockFirst;
		firstSet[NON_STATEMENT] = statementFirst;
		firstSet[NON_CONDITION]=conditionFirst;
		firstSet[NON_EXPRESSION]=expressionFirst;
		firstSet[NON_TERM]=termFirst;
		firstSet[NON_FACTOR]=factorFirst;
		
	}
	followSet = (Token_t **)calloc(6,sizeof(Token_t *));
	if(followSet == NULL){
		logerror("initial a memory for Follow set failed\n");
		return -1;
	}else{
		followSet[NON_BLOCK]= blockFollow;
		followSet[NON_STATEMENT]=statementFollow;
		followSet[NON_CONDITION]=conditionFollow;
		followSet[NON_EXPRESSION]=expressionFollow;
		followSet[NON_TERM]=termFollow;
		followSet[NON_FACTOR]=factorFollow;
	}
	phi = createset(nulsym);
	relset = createset(eqlsym,neqsym,lessym,leqsym,gtrsym,geqsym,nulsym);

	//create begin symbol sets
	blockbegsys =createset(constsym,varsym,procsym,identsym,ifsym,callsym,beginsym,whilesym,readsym,writesym,nulsym);
	declbegsys = createset(constsym, varsym, procsym, nulsym);
	statbegsys = createset(identsym,callsym,beginsym,ifsym,whilesym,readsym,writesym,nulsym);
	condbegsys = createset(oddsym,plussym,lparentsym,identsym,numbersym,nulsym);
	exprbegsys = createset(plussym,minussym,lparentsym,identsym,numbersym,nulsym);
	termbegsys = createset(identsym,numbersym,lparentsym,nulsym);
	facbegsys = createset(identsym, numbersym, lparentsym, nulsym);
	
	return 0;
}

boolean containFirst(Token_t token, NonTerminal_t nonterminal){
	Token_t *first = getFirstSet(nonterminal);
	int i = 0;
	for(i=0;i<NUMS_FIRST;i++){
		logpretty("First[%d][%s]\n",nonterminal,opSymbol[first[i]]);
		if(first[i] == token)
			return TRUE;
	}
	return FALSE;
}
boolean containFollow(Token_t token, NonTerminal_t nonterminal){
	Token_t *follow = getFollowSet(nonterminal);
	int i = 0;
	for(i=0;i<NUMS_FOLLOW;i++){
		logpretty("Follow[%d][%s]\n",nonterminal,opSymbol[follow[i]]);
		if(follow[i] == token){
			return TRUE;
		}
	}
	return FALSE;
}
void scanFollowSymbol(struct Parse *this,NonTerminal_t nonterminal){
	CurrentToken_t *curToken = &this->curToken;
	Token_t expect = curToken->cToken;
	while(!containFollow(curToken->cToken,nonterminal)){
		logerror("[Follow %d]expect Token[%s]-current Token[%s]\n",
			nonterminal,opSymbol[expect],opSymbol[curToken->cToken]);
		getToken(this);
	}
}
void scanFirstSymbol(struct Parse *this,NonTerminal_t nonterminal){
	CurrentToken_t *curToken = &this->curToken;
	Token_t expect = curToken->cToken;
	while(!containFirst(curToken->cToken,nonterminal)){
		logerror("[First %d]expect Token[%s]-current Token[%s]\n",
			nonterminal,opSymbol[expect],opSymbol[curToken->cToken]);
		getToken(this);
	}
}
void expect(struct Parse *this,Token_t expect){
	CurrentToken_t *curToken = &this->curToken;
	while(!(curToken->cToken == expect)&&(curToken->cToken != nulsym)){
		logerror("current Token[%s]-expect Token[%s]\n",opSymbol[curToken->cToken],opSymbol[expect]);
		getToken(this);
	}
}
   
symset uniteset(symset s1, symset s2)   
{   
    symset s;   
    snode* p;   
       
    s = p = (snode*) malloc(sizeof(snode));   
       
    s1 = s1->next; s2 = s2->next;    
    while (s1 && s2)   
    {   
        p->next = (snode*) malloc(sizeof(snode));   
        p = p->next;   
        if (s1->elem < s2->elem)   
        {   
            p->elem = s1->elem;   
            s1 = s1->next;   
        }   
        else   
        {   
            p->elem = s2->elem;   
            s2 = s2->next;   
        }   
    }   
       
    if ( s2 )   s1 = s2;
   
    while (s1)   
    {   
        p->next = (snode*) malloc(sizeof(snode));   
        p = p->next;   
        p->elem = s1->elem;   
        s1 = s1->next;   
           
    }   
    p->next = NULL;   
   
    return s;   
}
   
void setinsert(symset s, Token_t elem)   
{   
    snode* p = s;   
    snode* q;   
   
    while (p->next && p->next->elem < elem)   
    {   
        p = p->next;   
    }   
       
    q = (snode*) malloc(sizeof(snode));   
    q->elem = elem;   
    q->next = p->next;   
    p->next = q;
}
   
symset createset(Token_t elem, .../* SYM_NULL */)
{   
    va_list list;   
    symset s;   
   
    s = (snode*) malloc(sizeof(snode));   
    s->next = NULL;   
   
    va_start(list, elem);   
    while (elem != nulsym)
    {	
        setinsert(s, elem);   
        elem = va_arg(list,Token_t);   
    }   
    va_end(list);   
    return s;
}
   
void destroyset(symset s)   
{   
    snode* p;   
   
    while (s)   
    {   
        p = s;   
        s = s->next;   
        free(p);   
    }   
}
   
int inset(Token_t elem, symset s)
{   
    s = s->next;   
    while (s && s->elem < elem)   
        s = s->next;   
   
    if (s && s->elem == elem)   
        return 1;   
    else   
        return 0;   
}
void showset(symset s)   
{   
    s = s->next;
    while (s){   
        logpretty("%s,",opSymbol[s->elem]);
        s = s->next;
    }   
    logpretty("\n");
}
