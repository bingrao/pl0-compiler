ROOT_DIR=$(shell pwd)
BIN=compiler
OBJS_DIR=obj
BIN_DIR=bin
SRC_DIR=src
INC_DIR=src/include
TEST_INPUT=input
TEST_OUTPUT=output
CFLAG=-I${ROOT_DIR}/${INC_DIR}
CC=gcc ${CFLAG}
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR INC_DIR

CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:build test compile

################################################               
#		    build all c files          #
################################################
build:check_deps $(SRC_DIR) $(CUR_OBJS)

check_deps:
	@mkdir -p ${ROOT_DIR}/${OBJS_DIR}
	@mkdir -p ${ROOT_DIR}/${BIN_DIR}
	@mkdir -p ${ROOT_DIR}/${TEST_OUTPUT}


#compile the targer recursively
$(SRC_DIR):ECHO
	make -C $@
ECHO:
	@echo $(SRC_DIR)
#if current dir contains *.c file then compile them
$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@



#build final compile file
compile:build build_PL0


###################################################################################	
#Build a driver execute file
###################################################################################	
DRIVEROBJS=${OBJS_DIR}/compilerDriver.o ${OBJS_DIR}/virtualMachine.o ${OBJS_DIR}/scanner.o ${OBJS_DIR}/dfa.o ${OBJS_DIR}/symbolTable.o ${OBJS_DIR}/parse.o ${OBJS_DIR}/parserInit.o ${OBJS_DIR}/unparse.o ${OBJS_DIR}/gencode.o ${OBJS_DIR}/printAST.o ${OBJS_DIR}/parse_common.o
build_PL0:${DRIVEROBJS}
	${CC} -o ${BIN_DIR}/compile $^

###################################################################################
###################################################################################	


# build all test and execute 
test:build test_vm test_scanner test_parse

################################################               
#		    Virtual Machine Test
################################################
VMOBJS=${OBJS_DIR}/test_vm.o ${OBJS_DIR}/virtualMachine.o
test_vm:${VMOBJS}
	${CC} -o ${BIN_DIR}/$@ $^
	${BIN_DIR}/$@ -i ${TEST_INPUT}/vm-default.txt -o ${TEST_OUTPUT}/vm-default-out.txt
	${BIN_DIR}/$@ -i ${TEST_INPUT}/vm-call.txt -o ${TEST_OUTPUT}/vm-call-out.txt
	${BIN_DIR}/$@ -i ${TEST_INPUT}/vm-call.txt

	
################################################               
#		    Scanner Test
################################################
SCANNEROBJS=${OBJS_DIR}/test_scanner.o ${OBJS_DIR}/scanner.o ${OBJS_DIR}/dfa.o ${OBJS_DIR}/symbolTable.o
test_scanner:${SCANNEROBJS}
	${CC} -o ${BIN_DIR}/$@ $^
	${BIN_DIR}/$@ -i ${TEST_INPUT}/dfa.txt


################################################               
#		    parse Test
################################################
PARSEOBJS=${OBJS_DIR}/test_parser.o ${OBJS_DIR}/symbolTable.o ${OBJS_DIR}/parse.o ${OBJS_DIR}/parserInit.o ${OBJS_DIR}/unparse.o ${OBJS_DIR}/printAST.o ${OBJS_DIR}/gencode.o ${OBJS_DIR}/virtualMachine.o ${OBJS_DIR}/parse_common.o
test_parse:${PARSEOBJS}
	${CC} -o ${BIN_DIR}/$@ $^
	${BIN_DIR}/$@ -i ${TEST_INPUT}/dfa.txt

clean:
	@rm -rf $(OBJS_DIR)
	@rm -rf $(BIN_DIR)
	@rm -fr ${TEST_OUTPUT}
