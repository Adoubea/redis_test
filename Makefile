WORKDIR=.

CC=gcc
CPPFLGS= -I$(WORKDIR)/inc/ -I/usr/local/include/hiredis
CFLAGS= -Wall -g
LIBS=-lhiredis -lpthread

src=$(wildcard $(WORKDIR)/src/*.c)
obj= $(patsubst %.c, %.o, $(src))
dir=$(notdir $(obj))


test_main=test_main
example=example
target=$(example) $(test_main)

ALL:$(target)



$(dir):%.o:./src/%.c 
	$(CC) -c $< -o $@ $(CPPFLGS) $(CFLAGS)

$(example):example.o
	$(CC) $^ -o $@ $(LIBS)

$(test_main):test_main.o make_log.o redis_op.o
	$(CC) $^ -o $@ $(LIBS)







.PHONY:clean ALL distclean
clean:
	-rm -rf $(dir) $(target)

distclean:
	-rm -rf $(dir) $(target)

