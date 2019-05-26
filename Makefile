#src = $(wildcard ./*.c)
#obj = $(patsubst %.c, %.o, $(src))
obj = agent.o ThreadPool.o taskQueue.o
target = agent
CC = gcc

$(target) : $(obj)
	$(CC) $(obj) -D_REENTRANT -o $(target) -lpthread

%.o:%.c
	$(CC) -c $<  -o $@

#删除所有编译文件包括可执行文件，重新编译
.PHONY:clean
clean:
	@echo "cleanning project"
	-rm -rf $(obj) $(target)
	@echo "clean completed"
