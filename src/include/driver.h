#ifndef DRIVE_H
#define DRIVE_H
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
#include "virtualMachine.h"
#include "scanner.h"
#include "symtable.h"
#include "parse.h"
#include "gencode.h"


typedef struct CompilerDriver{
	struct Scanner *scanner;
	struct Parse *parse;
	//struct GenCode *gc;
	struct SymTable *sym;
	struct virtualMachine *vm;
	void (*cleanup)(struct CompilerDriver *this);
	void (*run)(struct CompilerDriver *this,char *path);
}CompilerDriver_t;
#endif
