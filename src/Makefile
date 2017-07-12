SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "include") print $$9}')

CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(SUBDIRS) $(CUR_OBJS)

#compile the targer recursively
$(SUBDIRS):ECHO
	make -C $@
ECHO:
	@echo $(SUBDIRS)
#if current dir contains *.c file then compile them
$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@
