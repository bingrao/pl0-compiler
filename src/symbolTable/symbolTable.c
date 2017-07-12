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
#include "symtable.h"

int hashcode(struct SymTable *this,char *key){
	unsigned long hash = 5381;
	int c;

	while (c = *key++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	  return (hash)%this->numsBucket;
}

boolean compareDataType(DataType *left,DataType *right){
	if((strcmp(left->name,right->name) == 0)&&
	   (left->kind == right->kind)&&
	   (left->level == right->level)&&
	   (left->addr == right->addr)){
	   	logerror("name[%s-%s],kind[%d-%d],level[%d-%d],addr[%d-%d],value[%d,%d]\n",
	   				left->name,right->name,
	   				left->kind,right->kind,
	   				left->level,right->level,
	   				left->addr,right->addr,
	   				left->val,right->val);
		return TRUE;
	}else{
		return FALSE;
	}
}
boolean _contain(struct SymTable *this,char *key,DataType *value){
	SymTableNode_T * symbolTable = this->getSymbolTable(this,key);
	if(symbolTable == NULL){
		return FALSE;
	}else{
		struct SymbolNode *ptr = symbolTable->SymNodeList;
		while(ptr != NULL){
			if(compareDataType(ptr->Value,value) == TRUE)
				return TRUE;
			ptr = ptr->next;
		}
	}	
	return FALSE;
}
boolean _putSymbol(struct SymTable *this,char *key,DataType *value){	
	assert(this != NULL);
	assert(key != NULL);
	assert(value != NULL);

	if(this->numsTable >= this->numsBucket){
		logerror("The buckets is full[%d,%d], please release or expend the buckets\n",
				this->numsTable,this->numsBucket);
		return FALSE;
	}
	
	if(this->contain(this,key,value)){
		loginfo("The key[%s] exist in the table\n",key);
		return FALSE;
	}
	
	SymTableNode_T* symbolTable = this->getSymbolTable(this,key);
	if(symbolTable == NULL){
		// there is no symbletable with this key,so create it
		symbolTable = (SymTableNode_T*)calloc(1,sizeof(SymTableNode_T));
		if(symbolTable == NULL){
			logerror("Aplly for memory for a new symbol Table node failed\n");
			return FALSE;
		}
		symbolTable->Key = strdup(key);
		symbolTable->SymNodeList = NULL;
		symbolTable->tail = NULL;
		symbolTable->numsNode = 0;
		symbolTable->PreNode = symbolTable;
		symbolTable->NextNode = symbolTable;
		
		int hashValue = this->getHashValue(this,key);
		symbolTable->hashValue = hashValue;
		this->Buckets[hashValue] = symbolTable;
		
		if(this->numsTable++ == 0){
			this->head = symbolTable;
			this->tail = symbolTable;
		}else{
			this->tail->NextNode = symbolTable;
			symbolTable->NextNode = this->head;
			this->head->PreNode = symbolTable;
			symbolTable->PreNode = this->tail;
			this->tail = symbolTable;
		}
	}
	struct SymbolNode *newNode = (SymbolNode_t*)calloc(1,sizeof(SymbolNode_t));
	if(newNode == NULL){
		logerror("Aplly for memory for a new symbol node failed\n");
		return FALSE;
	}else{
		newNode->next = NULL;
		newNode->Value = value;
	}
	if(symbolTable->SymNodeList == NULL){
		symbolTable->SymNodeList = newNode;
	}else{
		symbolTable->tail->next = newNode;
	}		
	symbolTable->tail = newNode;
	this->totalNodes += ++symbolTable->numsNode;
	
	return TRUE;
}

SymTableNode_T *_getSymbolTable(struct SymTable *this,char *key){
	int hashValue = this->getHashValue(this,key);
	return this->Buckets[hashValue];
}

DataType* _getSymbol(struct SymTable *this,char *key,int currentLevel){
	SymTableNode_T* symbolTable = this->getSymbolTable(this,key);
	DataType *tmp = NULL;
	if(symbolTable != NULL){
		struct SymbolNode *ptr = symbolTable->SymNodeList;
		while(ptr != NULL){
			if(tmp == NULL){
				tmp = ptr->Value;
			}else if((ptr->Value->level <= currentLevel)&&
					(ptr->Value->level - currentLevel > tmp->level - currentLevel)){
				tmp = ptr->Value;
			}
			ptr = ptr->next;
		}
	}
	return tmp;
}
int _getNumsOfsymbolTable(struct SymTable *this){
	return this->numsTable;
}

int _getNumsOfsymbolWithSameKey(struct SymTable *this,char *key){
	SymTableNode_T* symbolTable = this->getSymbolTable(this,key);
	if(symbolTable == NULL){
		logerror("There is no symbol table with this key[%s]\n",key);
		return 0;
	}else{
		return symbolTable->numsNode;
	}
}
int _getNumsOfsymbol(struct SymTable *this){
	
	return this->totalNodes;
}
int _getNumsOfBuckets(struct SymTable *this){
	return this->numsBucket;
}
#if 0
void *_update(struct SymTable *this,char *key,int lexical,void *value){
	
	if(this->contain(this,key,lexical) == FALSE){
		logerror("The key[%s] does not exist in the table\n",key);
		return NULL;
	}
	int hashValue = this->getHashValue(this,key,lexical);
	void *oldValue = this->Buckets[hashValue]->Value;
	this->Buckets[hashValue]->Value = value;
	return oldValue;
}


void *_remove(struct SymTable *this,char *key,int lexical)
{	
	SymTableNode_T *cur = NULL;
	SymTableNode_T *pre = NULL;
	SymTableNode_T *next = NULL;
	void *oldValue = NULL;
	
	if(this->contain(this,key,lexical) == FALSE){		
		logerror("The key[%s] does not exist in the table\n",key);
		return oldValue;
	}
		
	int hashValue = this->getHashValue(this,key,lexical);
	
	cur = this->Buckets[hashValue];
	pre = cur->PreNode;
	next = cur->NextNode;

	if(cur == this->head)
		this->head = next;
	if(cur == this->tail)
		this->tail = pre;

	oldValue = cur->Value;
	
	pre->NextNode = next;
	next->PreNode = pre;
	cur->PreNode = NULL;
	cur->NextNode = NULL;
	free(cur->Key);
	free(cur);
	this->Buckets[hashValue] = NULL;
	
	this->numsNode--;
	return oldValue;
}
#endif
void SymTable_exit(struct SymTable *this){
	int i = 0;
	SymTableNode_T *curTable = NULL;
	struct SymbolNode *ptr = NULL;
	struct SymbolNode *del = NULL;
	for(i=0;i<this->numsBucket;i++){
		curTable = this->Buckets[i];
		if(curTable != NULL){
			ptr = curTable->SymNodeList;
			while(ptr != NULL){
				del = ptr;
				ptr = ptr->next;
				free(del->Value);
				free(del);
			}
			free(curTable);
		}
	}
	free(this->Buckets);
	return;
}
void SymTable_print(struct SymTable *this,int option,FILE *out){
	int i = 0;
	SymTableNode_T *cur = NULL;
	DataType* symbol = NULL;

	CompilerStdout(out,"The Symbol Table\t");
	CompilerStdout(out,"numOfSymbol:%d,numsOfSymbolTable[%d],numOfBucket:%d\n",
		this->totalNodes,this->numsTable,this->numsBucket);
	if(this->numsTable != 0){
		CompilerStdout(out,"head:{key[%s]}\ttail{key[%s]}\n",this->head->Key,this->tail->Key);
	}
	switch (option){
		case 1:{
			for(i=0;i<this->numsBucket;i++){
				cur = this->Buckets[i];
				if(cur != NULL){
					CompilerStdout(out,"SymbolTable{key[%s],hashValue[%d],numsOfSymbol[%d],previous[%s],next[%s]}}\n",
						cur->Key,cur->hashValue,cur->numsNode,cur->PreNode->Key,cur->NextNode->Key);				
					struct SymbolNode *ptr = cur->SymNodeList;
					while(ptr != NULL){
					CompilerStdout(out,"  |-Symbol {Name[%s],\tkind[%d],\tvalue[%d],\tlevel[%d],\taddr[%d]}\n",
						ptr->Value->name,ptr->Value->kind,ptr->Value->val,ptr->Value->level,ptr->Value->addr);					
						ptr = ptr->next;
					}
					CompilerStdout(out,"\n");
				}
			}
		}break;
		case 2:{
			cur = this->head;
			do{
				if(cur != NULL){					
					CompilerStdout(out,"SymbolTable{key[%s],hashValue[%d],numsOfSymbol[%d],previous[%s],next[%s]}}\n",
						cur->Key,cur->hashValue,cur->numsNode,cur->PreNode->Key,cur->NextNode->Key);
					cur = cur->NextNode;
				}
			}while(cur != this->head);
		}break;
		case 3:{

		}break;
		default:{
			logerror("the option[%d] error\n",option);
		}
	}
}



struct SymTable *SymTable_init(int bucketCount){

	SymTable_T* symTable = NULL;
	int i = 0;
	symTable = (SymTable_T*)calloc(1,sizeof(SymTable_T));
	if(symTable == NULL){
		logerror("Aplly for memory for a new symbol table failed\n");
		return NULL;
	}
	
	symTable->Buckets = (SymTableNode_T **)calloc(bucketCount,sizeof(SymTableNode_T*));
	if(symTable->Buckets == NULL){
		logerror("Apply for memory for an array of buckets to hold symbol nodes failed\n");
		free(symTable);
		return NULL;
	}
	for(i = 0;i<bucketCount;i++){
		symTable->Buckets[i] = NULL;
	}
	symTable->head = NULL;
	symTable->tail = NULL;
	symTable->numsTable = 0;
	symTable->totalNodes = 0;
	symTable->numsBucket = bucketCount;
	/*init operation*/
	symTable->getHashValue = hashcode;
	symTable->contain = _contain;
	symTable->putSymbol= _putSymbol;
	
	symTable->getSymbolTable = _getSymbolTable;
	symTable->getSymbol = _getSymbol;
	symTable->getNumsOfsymbolTable = _getNumsOfsymbolTable;
	symTable->getNumsOfsymbolWithSameKey = _getNumsOfsymbolWithSameKey;
	symTable->getNumsOfsymbol = _getNumsOfsymbol;
	symTable->getNumsOfBuckets = _getNumsOfBuckets;

	symTable->exit = SymTable_exit;
	symTable->printinfo = SymTable_print;
	logdebug("Initial a symbol table successfully\n");
	return symTable;
}


