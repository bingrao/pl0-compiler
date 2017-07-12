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

#include "virtualMachine.h"

static inline void help()
{
	#define USAGE1 "  1 Usage: ./vm -h \n"
	#define USAGE2 "  2 Usage: ./vm -i inFile\n"
	#define USAGE3 "  3 Usage: ./vm -i inFile -o outFile\n"
	#define USAGE4 "  4 Usage: ./vm -o outFile -i inFile\n"
	logpretty("\n\n*************************************************************\n");
	loginfo("The help instruction for Virtual Machine\n");
	logpretty("\nUsage: ./vm [OPTION] ... [FILE] ...\n");
	logpretty("Options:\n");
	logpretty("   -h\t\tprint out the help info\n");
	logpretty("   -i inFile\tspecific the input files that p-machine virtual machine will execute\n");
	logpretty("   -o outFile\tspecific the output files, if there is no output files, the virtual machine will output to system stdout\n");
	logpretty("Example:\n");
	logpretty(USAGE1);
	logpretty(USAGE2);
	logpretty(USAGE3);
	logpretty(USAGE4);
	logpretty("*************************************************************\n");
}
static inline void usage(){
	logpretty("\n\nUsage: ./vm [OPTION] ... [FILE] ...\n\n");
}
int main(int argc, char* argv[]){
#if 1

		switch(argc){
		case 1:{
			logerror("The command parameter does not work properly,please make sure, option[%d]\n",argc);
			help();
			return -1;
		}break;
		case 2:{
			/*./vm -h*/
			
			if(strcmp(argv[1],"-h") == 0){
				help();	
			}else{
				logerror("The command parameter does not work properly,please make sure, option[%d]\n",argc);
				usage();
			}
			return -1;
		}break;
		case 3:{
			//TODO
		}break;
		case 4:{
			logerror("The command parameter does not work properly,please make sure, option[%d]\n",argc);
			help();
			return -1;
		}break;		
		case 5:{
			//TODO
		}break;
		default:{
			int i = 0;
			logerror("The command parameter does not work properly,please make sure, option[%d]\n",argc);
			for(i=0;i<argc;i++)
				logerror("argc[%d] %s\n",i,argv[i]);
			logerror("\n");
			help();
			return -1;
		}
	}
	
	int in_idx = findIndex(argc,argv,"-i");
	int out_idx = findIndex(argc,argv,"-o");

	char *in_path = NULL;
	char *out_path = NULL;
	FILE *out_fp = NULL;
	if((in_idx != -1)&&(in_idx + 1 <argc)){
		in_path = argv[in_idx+1];
	}else{
		logerror("input file failed\n");
		return -1;
	}
	
	if((out_idx != -1)&&(out_idx+1<argc)){
		out_path = argv[out_idx+1];
		if( access(out_path, F_OK ) == -1){
			//ftruncate(vm->out_path,0);
			fclose(fopen(out_path, "w"));
		}
		
		out_fp = fopen(out_path, "ab+");
	}

	struct virtualMachine* vm = init_vm();
	if(vm != NULL){
		vm->run(vm,in_path,out_fp);
		vm->exit(vm);
	}else{
		logerror("inital a vm failed\n");
	}
	if(out_fp != NULL){
		fclose(out_fp);
	}	
#endif
	return 0;
}
