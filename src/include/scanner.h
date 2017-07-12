#ifndef SCANNER_H
#define SCANNER_H
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
#include "dfa.h"
#include "symtable.h"

typedef struct Scanner{
	struct DFA *dfa;
	struct SymTable *symbolTable;
	int (*run)(struct Scanner *this,char *path);
	int (*putSymbol)(struct Scanner *this);
	void (*exit)(struct Scanner *this);
	void (*printLexmeList)(struct Scanner *this,char *path);
	void (*outputLexmeList)(struct Scanner *this,FILE *out);
}Scanner_t;
extern Scanner_t * scanner_init(struct SymTable * syms);
#endif
