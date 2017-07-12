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

#include "driver.h"

boolean stdoutToken = FALSE;
boolean stdoutParse = FALSE;
boolean stdoutSymbol = FALSE;
boolean stdoutAST = FALSE;
boolean stdoutUnparse = FALSE;
boolean stdoutVirtm = FALSE;

void driver_run(struct CompilerDriver *this,char *path){
	int ret = 0;
	char *tokenFile = "output/tokenFile.txt";remove(tokenFile);
	char *unParseFile = "output/unParseSrc.txt";remove(unParseFile);
	char *astFile = "output/ast.txt";remove(astFile);
	char *mcodeFile = "output/machinecode.txt";remove(mcodeFile);
	char *mcodeFiletmp = "output/unparsemachinecode.txt";remove(mcodeFiletmp);
	char *symFile = "output/symbol_table.txt";remove(symFile);
	char *vmFile = "output/virtualmachine.txt";remove(vmFile);
	// run scanner to scan the source code and generate token file
	ret = this->scanner->run(this->scanner,path);
	if(ret != 0){
		logerror("Scanner get a wrong lexem\n");
		return;
	}
	if(stdoutToken == TRUE){
		this->scanner->printLexmeList(this->scanner,path);
	}
	
	//write the current token list to the file which is the input of parser.
	logdebug("output token list to %s\n",tokenFile);
	FILE *tokenF = fileCreateAndOpen(tokenFile);
	this->scanner->outputLexmeList(this->scanner,tokenF);
	fclose(tokenF);

	// run parser on the token file
	tokenF = fileCreateAndOpen(tokenFile);
	ret = this->parse->run(this->parse,tokenF);
	fclose(tokenF);

	
	if(stdoutUnparse == TRUE){
		this->parse->unParsePrint(this->parse,NULL);
	}else{
		FILE *unParseF = fileCreateAndOpen(unParseFile);
		this->parse->unParsePrint(this->parse,unParseF);
		fclose(unParseF);
	}
	
	if(stdoutAST == TRUE){
		this->parse->printAST(this->parse,NULL);
	}else{
		FILE *astF = fileCreateAndOpen(astFile);
		this->parse->printAST(this->parse,astF);
		fclose(astF);
	}

	
	if(stdoutSymbol == TRUE){
		// Output the details about symbol table to a file
		this->sym->printinfo(this->sym,1,NULL);
	}else{		
		// Output the details about symbol table to a file
		FILE *symF = fileCreateAndOpen(symFile);
		this->sym->printinfo(this->sym,1,symF);
		fclose(symF);
	}
		
	if(ret != 0){
		logerror("Parser encounter an error\n");
		return;
	}


	/*generate machine code*/
	FILE *mcode = fileCreateAndOpen(mcodeFile);
	generateMachineCode(mcode); //combine machine code for each procedures into final machine code
	fclose(mcode);

	

	if(stdoutParse == TRUE){
		// print machien code to the console
		printProcedureCode(2,NULL);
	}else{
		// write the machine code to a file
		FILE *mcodetmp = fileCreateAndOpen(mcodeFiletmp);
		
		CompilerStdout(mcodetmp,"The machine code for each procedure:\n");
		printProcedureCode(1,mcodetmp);

		CompilerStdout(mcodetmp,"\nThe final machine code:\n");
		printProcedureCode(2,mcodetmp);
		
		fclose(mcodetmp);
	}
	
	if(stdoutVirtm == TRUE){
		// output the vm trace to the screen
		this->vm->run(this->vm,mcodeFile,NULL);
	}else{		
		FILE *vmF = fileCreateAndOpen(vmFile);
		this->vm->run(this->vm,mcodeFile,vmF);
		fclose(vmF);
	}
	return;	
}

void driver_cleanup(struct CompilerDriver *this){
	struct Scanner *scanner = this->scanner;
	struct Parse *parse = this->parse;
	struct SymTable *sym = this->sym;
	struct virtualMachine *vm = this->vm;

	if(scanner != NULL){
		scanner->exit(scanner);
	}
	
	if(parse != NULL){
		parse->exit(parse);
	}
	
	if(sym != NULL){
		sym->exit(sym);
	}
	
	if(vm != NULL){
		vm->exit(vm);
	}
	return;

}
struct CompilerDriver *init_driver(){

	#define SYMBOL_MAX_NUMS	100
	CompilerDriver_t *driver = (CompilerDriver_t *)calloc(1,sizeof(CompilerDriver_t));
	if(driver == NULL){
		logerror("initial a compiler driver object failed\n");
	}else{
		
		driver->cleanup = driver_cleanup;
		driver->run = driver_run;
		
		driver->sym = SymTable_init(SYMBOL_MAX_NUMS);
		if(driver->sym  == NULL){
			logerror("init a symbol table failed\n");
			driver->cleanup(driver);
			return NULL;
		}
		driver->scanner = scanner_init(driver->sym);
		if(driver->scanner == NULL){
			logerror("init a scanner failed\n");
			driver->cleanup(driver);
			return NULL;
		}
		driver->parse = parse_init(driver->sym);
		if(driver->parse == NULL){
			logerror("init a parse failed\n");
			driver->cleanup(driver);
			return NULL;
		}
		driver->vm = init_vm();
		if(driver->vm == NULL){
			logerror("init a vm failed\n");
			driver->cleanup(driver);
			return NULL;
		}
		logdebug("Initial PL/0 Compiler sucessfully\n");
	}	
	return driver;
}

static inline void help()
{
	#define USAGE1 "  1 Usage: ./compile -h \n"
	#define USAGE2 "  3 Usage: ./compile -i inFile [-l|-a|-v|-s|-t|-u]\n"
	logpretty("\n\n*************************************************************\n");
	logpretty("The help instruction for Compile for PL\\0 Language\n");
	logpretty("\nUsage: ./compile [OPTION] ... [FILE] ...\n");
	logpretty("Options:\n");
	logpretty("   -h\t\tprint out the help info\n");
	logpretty("   -i inFile\tspecific the input files that compiler will execute\n");
	logpretty("   -l \t\tprint the list of lexemes/tokens (scanner output) to the screen\n");
	logpretty("   -a \t\tprint the generated assembly code (parser/codegen output) to the screen\n");
	logpretty("   -v \t\tprint virtual machine execution trace (virtual machine output) to the screen\n");
	logpretty("   -s \t\tprint symbol table to the screen\n");
	logpretty("   -t \t\tprint abstract syntax tree to the screen\n");	
	logpretty("   -u \t\tprint the result of unparse for source program to the screen\n");
	logpretty("Example:\n");
	logpretty(USAGE1);
	logpretty(USAGE2);
	logpretty("*************************************************************\n");
}

void printCommand(int argc, char* argv[]){
	int i = 0;
	for(i=0;i<argc;i++){
		logpretty("%s ",argv[i]);
	}
	logpretty("\n");
}

int main(int argc, char* argv[]){
	char *path = NULL;
	int i = 0;
	//check the input parameters
	for(i=1;i<argc;i++){
		if(strcmp(argv[i],"-h") == 0){
			help();
			return -1;
		}else if(strcmp(argv[i],"-i") == 0){
			if((i+1)<argc){
				path = argv[++i];
			}else{
				logerror("input parameter error\n");
				printCommand(argc,argv);
				help();
				return -1;
			}
		}else if(strcmp(argv[i],"-l") == 0){
			stdoutToken = TRUE;// print tokens to the screen
		}else if(strcmp(argv[i],"-a") == 0){
			stdoutParse = TRUE; //print the assembly code to the screen
		}else if(strcmp(argv[i],"-s") == 0){
			stdoutSymbol = TRUE;
		}else if(strcmp(argv[i],"-t") == 0){
			stdoutAST = TRUE;
		}else if(strcmp(argv[i],"-u") == 0){
			stdoutUnparse = TRUE;
		}else if(strcmp(argv[i],"-v") == 0){
			stdoutVirtm = TRUE; // Print the machine execution trace to the screen
		}else{
			logerror("input parameter error\n");
			printCommand(argc,argv);
			help();
			return -1;
		}
	}
	if(path == NULL){
		logerror("input parameter error\n");
		printCommand(argc,argv);
		help();
		return -1;
	}	
	loginfo("The input file is:%s\n",path);
	struct CompilerDriver *driver = init_driver();
	if(driver != NULL){
		driver->run(driver,path);
		driver->cleanup(driver);
	}
	free(driver);
	return 0;
}

