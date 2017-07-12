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
int main(int argc, char* argv[]){
	char *path = NULL;
	switch(argc){
		case 1:{
			path = "../input/dfa.txt";
		}break;
		case 2:{
			
		}break;
		case 3:{
			path = argv[2];
		}break;
		default:{
			logerror("The input parameters error\n");
			return -1;
		}
	}

	
	
	FILE* fin = fopen(path,"r");
	if(fin == NULL){
		logerror("Open file failed, please double check: %s\n",path);
		return -1;
	}
	loginfo("read file successfuly\n");

	struct Parse *p = parse_init(NULL);
	if(p != NULL){
		p->run(p,fin);
		p->unParsePrint(p,NULL);
		p->exit(p);
	}else{
		logerror("init a parse failed\n");
	}

	free(p);

	
	fclose(fin);
	
	return 0;
}

