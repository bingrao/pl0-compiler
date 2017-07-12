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

char *opcode[] = {
	"NUL", 		//0
	"LIT",		//1
	"RTN",		//2
	"LOD",		//3
	"STO",		//4
	"CAL",		//5
	"INC",		//6
	"JMP",		//7
	"JPC",		//8
	"SIO",		//9
	"NUL",		//10
	"NUL",		//11
	"NEG",		//12
	"ADD",		//13
	"SUB",		//14
	"MUL",		//15
	"DIV",		//16
	"ODD",		//17
	"MOD",		//18
	"EQL",		//19
	"NEQ",		//20
	"LSS",		//21
	"LEQ",		//22
	"GTR",		//23
	"GEQ"		//24
};
/****
 * Define VM API
 */	
static inline void run_vm(struct virtualMachine *vm,char *path,FILE *stdout){

	int run_cnt = 1;
	if(vm->status != IDLE){
		loginfo("The current vm is not idle, please double check the state of vm: %d\n",vm->status);
		return;
	}else{
	   /*get the control of vm and set the vm status is running*/
	   vm->out_fp = stdout;
	   int numIR = vm->load_code(vm,path); //load the code to machine
	   if(numIR == -1){
		   vm->status = ERROR;
		   return;
	   }else{
		   vm->numInstructions = numIR;
	   }
		vm->status = RUNNING;
	    vm->prettyinfo(vm,0);
		vm->prettyinfo(vm,1);
		CompilerStdout(vm->out_fp,"The initial state of Virtual machine is:\n");
		CompilerStdout(vm->out_fp,"Line OP   R  L  M\tPC BP SP\n");
	    vm->prettyinfo(vm,2);
		vm->prettyinfo(vm,3);
	#ifdef RPINT_VM_STATE
		CompilerStdout(vm->out_fp,"\nLine	Idx OP	 R	L  M\tPC BP SP\n");
	#endif
	}	
	while(vm->status == RUNNING){
		boolean reg_pre = vm->prefetch(vm);
		if(reg_pre == FALSE){
			vm->status = ERROR;
		}else{
			vm->execute(vm);
			CompilerStdout(vm->out_fp,"%4d  ",run_cnt++);
			vm->prettyinfo(vm,2);
		}
	}
	return;
}


static inline boolean prefetch(struct virtualMachine *vm){
	int cur_pc = vm->pc;
	int new_pc = 0;
	
	if(cur_pc >= MAX_CODE_LENGTH){
		logerror("The code segment is overflow\n");
		return FALSE;
	}else if(cur_pc >= vm->numInstructions){
		logerror("The current instructions are excuted over\n");
		return FALSE;
	}else{
		vm->ir = vm->code[cur_pc];
		new_pc = cur_pc + 1;
	}
	/*update pc to point to next instruction*/
	vm->pc = new_pc;
	vm->pre_pc = cur_pc;
	
	return TRUE;
}


static inline void instruction_execution(struct virtualMachine *vm){
	
	int op = vm->ir.op;
	int i = vm->ir.r;
	int j = vm->ir.l;
	int k = vm->ir.m;
	
	int sp = vm->sp;
	int bp = vm->bp;
	int pc = vm->pc;
	int *r = vm->r;
	int *stack = vm->stack;
	int *frameBreak = vm->frameBreak;
	int frameLevel = vm->frameLevel;
	switch(op){
		case LIT:{
			//Loads a constant value (literal) M into Register R
			r[i] = k;
		}break;
		case RTN:{			
			sp = bp -1;
			//Returns from a subroutine and restore the caller environment
			if(bp == 1){
				vm->status = IDLE;/*Kill the simulation if we're at the base level*/
			}else{
				bp = stack[sp+3];
			}			
			pc = stack[sp+4];
			frameBreak[frameLevel] = 0;
            frameLevel--;
		}break;
		case LOD:{
			/*Load value into a selected register from the stack 
			 *location at offset M from L lexicographical levels down
			 */
			r[i] = stack[vm->base(vm,j,bp)+k];
		}break;
		case STO:{
			/*Store value from a selected register in the 
			 *stack location at offset M from L lexicographical levels down*/
			stack[vm->base(vm,j,bp)+k] = r[i];
		}break;
		case CAL:{ 
			//Call procedure at code index M (generates new Activation Record and pc <- M)
			stack[sp+1] = 0;//space to return value
			stack[sp+2] = vm->base(vm,j,bp); //static link(SL)
			stack[sp+3] = bp;// dynamic link (DL)
			stack[sp+4] = pc;// return address(RA)
			bp = sp +1;
			pc = k;
			frameBreak[frameLevel] = sp;
    		frameLevel++;
		}break;
		case INC:{
			/*
			 *Allocate M locals (increment sp by M).First four are Functional Value, 
			 *Static Link (SL), Dynamic Link (DL), and Return Address (RA)
			 */
			sp = sp + k;
		}break;
		case JMP:{
			//Jump to instruction M
			pc = k;
		}break;
		case JPC:{ //Jump to instruction M if R = 0
			if(r[i] == 0)
				pc = k;
		}break;
		case SIO:{
			switch(k){
				case 1:{ //print(R[i]); Write a register to the screen
					#ifdef OUTPUT_TO_CONSOLE
					loginfo("\tOuput:%d\n",r[i]);
					#endif
				}break;
				case 2:{ //read(R[i]); Read in input from the user and store it in a register
					scanf("%d",&r[i]);
				}break;
				case 3:{//Set Halt flag to one, End of program (program stops running)
					vm->status = IDLE;
				}break;
				default:{
					logerror("The modify code is wrong: %d\n",k);
				}
			}
			
		}break;
		case NEG:{
			r[i] = -r[j];
		}break;
		case ADD:{
			r[i] = r[j]+r[k];
		}break;
		case SUB:{
			r[i] = r[j]-r[k];
		}break;
		case MUL:{
			r[i] = r[j]*r[k];
		}break;
		case DIV:{
			r[i] = r[j]/r[k];
		}break;
		case ODD:{
			r[i] = r[i]%2;
		}break;
		case MOD:{
			r[i] = r[j]%r[k];
		}break;
		case EQL:{
			r[i] = r[j] == r[k];
		}break;
		case NEQ:{
			r[i] = r[j] != r[k];
		}break;
		case LSS:{
			r[i] = r[j] < r[k];
		}break;
		case LEQ:{
			r[i] = r[j] <= r[k];
		}break;
		case GTR:{
			r[i] = r[j] > r[k];
		}break;
		case GEQ:{
			r[i] = r[j] >= r[k];
		}break;
		default:{
			logerror("The op code is fatal to the virtual machine, please double check\n");
		}
	}
	/*update the current pointer values*/
	vm->pc = pc;
	vm->bp = bp;
	vm->sp = sp;
	vm->frameLevel = frameLevel;
}

static inline void prettyprintInstruction(struct virtualMachine *vm,int idx,instruction_t *ir){
	CompilerStdout(vm->out_fp,"%4d %3s %2d %2d %2d",idx,opcode[ir->op],ir->r,ir->l,ir->m);
}
static inline void prettyprintInstructionInDigital(struct virtualMachine *vm,int idx,instruction_t *ir){
	CompilerStdout(vm->out_fp,"%4d %2d %2d %2d %2d",idx,ir->op,ir->r,ir->l,ir->m);
}
static inline int unparseInstruction(struct virtualMachine *vm,int idx,instruction_t *ir){
	return unparseIr(vm->out_fp,idx,ir);
}
static inline void printVMState(struct virtualMachine *vm){
	int i = 0;
	int j = 0;
	prettyprintInstruction(vm,vm->pre_pc,&vm->ir);
	CompilerStdout(vm->out_fp,"\t%2d %2d %2d\t",vm->pc,vm->bp,vm->sp);
	
	/*Print the current register info*/
	CompilerStdout(vm->out_fp,"Register[%d]={",COMMON_REGISTER_NUMBER);
	for(i=0;i<COMMON_REGISTER_NUMBER;i++){
		if(i == COMMON_REGISTER_NUMBER-1)
			CompilerStdout(vm->out_fp,"%d",vm->r[i]);
		else
			CompilerStdout(vm->out_fp,"%d ",vm->r[i]);
	}
	CompilerStdout(vm->out_fp,"}\t");

#if 0
	/*Print the info of current AR*/
	CompilerStdout(vm->out_fp,"FrameLevel[%d]={",MAX_LEXI_LEVELS);
	for(i=0;i<MAX_LEXI_LEVELS;i++){
		if(i == MAX_LEXI_LEVELS -1)
			CompilerStdout(vm->out_fp,"%d",vm->frameBreak[i]);
		else
			CompilerStdout(vm->out_fp,"%d ",vm->frameBreak[i]);
	}
	logpretty("}\t");
#endif
	
	/*Print the current active stack info*/
	int pcnt = vm->sp >=1?vm->sp:1;
	CompilerStdout(vm->out_fp,"Stack[%d]={",pcnt);
	for(i=0;i<pcnt;i++){
		if(i == pcnt-1)
			CompilerStdout(vm->out_fp,"%d",vm->stack[i]);
		else
			CompilerStdout(vm->out_fp,"%d ",vm->stack[i]);

		for(j = 0;j<MAX_LEXI_LEVELS;j++){
			if((i == vm->frameBreak[j])&&(i>1)){
				CompilerStdout(vm->out_fp,"| ");
			}
		}
	}
	CompilerStdout(vm->out_fp,"}\t");	
}


static inline void prettyPrintVMinfo(struct virtualMachine *vm,int option){	
	switch(option){
		case 0:{//print the machine code pretty
			#ifdef PRETTY_PRINT_CODE
			int i = 0;
			instruction_t *code = vm->code;
			//Printing the header
			CompilerStdout(vm->out_fp,"The total number of ir is:%d\n",vm->numInstructions);
			CompilerStdout(vm->out_fp,"Line OP   R  L  M\n");
			int numIr = vm->numInstructions;
			for(i=0;i<numIr;i++){
				prettyprintInstruction(vm,i,&code[i]);
				CompilerStdout(vm->out_fp,"\n");
			}
			#endif
		}break;
		case 1:{//unparse the machine code to assemble code and print them out		
			#ifdef UNPARSER_CODE
			int i = 0;
			instruction_t *code = vm->code;
			//Printing the header
			CompilerStdout(vm->out_fp,"The total number of ir is:%d\n",vm->numInstructions);
			CompilerStdout(vm->out_fp,"Line\tInstruction\n");
			int numIr = vm->numInstructions;
			for(i=0;i<numIr;i++){
				unparseInstruction(vm,i,&code[i]);
				CompilerStdout(vm->out_fp,"\n");
			}
			CompilerStdout(vm->out_fp,"\n");
			#endif
		}break;
		case 2:{//Print current the state of Virtual Machine
			#ifdef RPINT_VM_STATE
			printVMState(vm);
			CompilerStdout(vm->out_fp,"\n");
			#endif
		}break;
		case 3:{
			int i = 0;
			instruction_t *code = vm->code;
			CompilerStdout(vm->out_fp,"The total number of ir is:%d\n",vm->numInstructions);
			CompilerStdout(vm->out_fp,"Line OP  R  L  M\tLine\tInstruction\n");
			int numIr = vm->numInstructions;
			for(i=0;i<numIr;i++){
				prettyprintInstructionInDigital(vm,i,&code[i]);
				CompilerStdout(vm->out_fp,"\t");
				unparseInstruction(vm,i,&code[i]);
				CompilerStdout(vm->out_fp,"\n");
			}
		}break;
		case 4:{
			//TODO
		}break;
		default:{
			logerror("The option[%d] is wrong,please make sure that\n",option);
		}
	}
}
/*
 * looking for the base pointer
 */
static inline int base(struct virtualMachine *vm,int l,int b){
	int base = b;
	int level = l;
	while(level>0){
		base = vm->stack[base + 1];
		level--;
	}
	return base;
}

/*load code from the text file and parse it into the instruction format*/
static inline int load_code(struct virtualMachine *vm,char *path){
	FILE* fin = NULL;
	int pcounter = 0;
	fin = fopen(path,"r");
	if(fin == NULL){
		logerror("Open file failed, please double check: %s\n",path);
		return -1;
	}
	while(!feof(fin)){
		fscanf(fin, "%d ",&vm->code[pcounter].op);
		fscanf(fin, "%d ",&vm->code[pcounter].r);
		fscanf(fin, "%d ",&vm->code[pcounter].l);
		fscanf(fin, "%d ",&vm->code[pcounter].m);
		pcounter++;
		/*Checking for code overflow*/
		if(pcounter >= MAX_CODE_LENGTH){
			logerror("The code size is overflow\n");
			fclose(fin);
			return -1;
		}
	}
	fclose(fin);
	return pcounter;
}

static inline void exit_vm(struct virtualMachine *vm)
{
	return;
}

struct virtualMachine* init_vm(){
	virtualMachine_t *vm = (virtualMachine_t *)calloc(1,sizeof(virtualMachine_t));
	if(vm == NULL){
		logerror("Apply for memory for Virtual Machine failed\n");
		return NULL;
	}
	vm->out_fp = NULL;
	vm->sp = 0;
	vm->bp = 1;
	vm->pc = 0;
	vm->pre_pc = 0;
	vm->ir.op = 0;
	vm->ir.r = 0;
	vm->ir.l = 0;
	vm->ir.m = 0;
	int i = 0;
	/*common register file set 0 initially*/
	for(i=0;i<COMMON_REGISTER_NUMBER;i++)
		vm->r[i] = 0;
	/*set stack with 0 value*/
	for(i=0;i<MAX_STACK_HEIGHT;i++)
		vm->stack[i] = 0;
	/*Set 0 for all the code segment*/
	for(i=0;i<MAX_CODE_LENGTH;i++){
		vm->code[i].op = 0;
		vm->code[i].r = 0;
		vm->code[i].l = 0;
		vm->code[i].m = 0;
	}
	for(i = 0; i < MAX_LEXI_LEVELS; i++)
	{
        vm->frameBreak[i] = 0;
	}
	vm->frameLevel = 0;	
	vm->base = base;
	vm->prettyinfo = prettyPrintVMinfo;
	vm->load_code = load_code;
	vm->execute = instruction_execution;
	vm->prefetch = prefetch;
	vm->run = run_vm;
	vm->exit = exit_vm;	
	vm->status = IDLE;
	logdebug("Inital  a Virtual Machine successfully\n");
	return vm;
}

