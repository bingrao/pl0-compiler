#ifndef SYMTABLE_H
#define SYMTABLE_H
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
#include "scanner.h"

typedef struct Symbol DataType;

typedef struct SymbolNode{
	DataType *Value;
	struct SymbolNode *next;
}SymbolNode_t;


typedef struct SymTableNode
{
  /* Key */
  char *Key;
  /* Value */
  struct SymbolNode *SymNodeList;
  struct SymbolNode *tail;

  /*hash Value for the key*/
  int hashValue;

  int numsNode;
  /* Address of next SymTableNode */
  struct SymTableNode *NextNode;
  struct SymTableNode *PreNode;
}SymTableNode_T;

typedef struct SymTable
{
  /* Number of items in SymTable*/
  int numsTable;

  int totalNodes;

  /* Number of buckets in the table*/
  int numsBucket;

  /*The head symtable node*/
  struct SymTableNode *head;
  /*The tail */
  struct SymTableNode *tail;
  
  /* Address of the first bucket*/
  struct SymTableNode **Buckets;



  /*Operation*/
  int (*getHashValue)(struct SymTable *this,char *key);
  
  boolean (*contain)(struct SymTable *this,char *key,DataType *value);
  
  SymTableNode_T *(*getSymbolTable)(struct SymTable *this,char *key);

  
  boolean (*putSymbol)(struct SymTable *this,char *key,DataType *value);
  
  DataType *(*getSymbol)(struct SymTable *this,char *key,int currentLevel);
  int (*getNumsOfsymbol)(struct SymTable *this);
  int (*getNumsOfBuckets)(struct SymTable *this);
  int (*getNumsOfsymbolWithSameKey)(struct SymTable *this,char *key);
  int (*getNumsOfsymbolTable)(struct SymTable *this);

  void (*printinfo)(struct SymTable *this,int option,FILE *out);

  
  void (*exit)(struct SymTable *this);
}SymTable_T;

extern struct SymTable *SymTable_init(int bucketCount);
#endif
