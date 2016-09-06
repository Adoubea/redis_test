WORKDIR=.

CC=gcc
CPPFLGS= -I$(WORKDIR)/inc/ -I/usr/local/include/hiredis
CFLAGS= -Wall -g
LIBS=-lhiredis -lpthread -lfcgi

src=$(wildcard $(WORKDIR)/src/*.c)
obj= $(patsubst %.c, %.o, $(src))
dir=$(notdir $(obj))


test_main=test_main
example=example
demo=demo 
upload=upload

target=$(example) $(test_main) $(demo) $(upload)

ALL:$(target)



$(dir):%.o:./src/%.c 
	$(CC) -c $< -o $@ $(CPPFLGS) $(CFLAGS)

$(example):example.o
	$(CC) $^ -o $@ $(LIBS)

$(test_main):test_main.o make_log.o redis_op.o
	$(CC) $^ -o $@ $(LIBS)

$(demo):demo.o
	$(CC) $^ -o $@ $(LIBS)

$(upload):upload.o
	$(CC) $^ -o $@ $(LIBS)



.PHONY:clean ALL distclean
clean:
	-rm -rf $(dir) $(target)

distclean:
	-rm -rf $(dir) $(target)

