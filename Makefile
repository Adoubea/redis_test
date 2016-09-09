WORKDIR=.

CC=gcc
CPPFLGS= -I$(WORKDIR)/inc/ -I/usr/local/include/hiredis -I/usr/include/fastdfs/ -I/usr/include/fastcommon/
CFLAGS= -Wall -g
LIBS=-lhiredis -lpthread -lfcgi -lfdfsclient -lm

src=$(wildcard $(WORKDIR)/src/*.c)
obj= $(patsubst %.c, %.o, $(src))
dir=$(notdir $(obj))


#test_main=test_main
#example=example
#demo=demo 
#upload=upload
upload_cgi=upload_cgi
data_cgi=data_cgi

target= $(upload_cgi) $(data_cgi)

ALL:$(target)



$(dir):%.o:./src/%.c 
	$(CC) -c $< -o $@ $(CPPFLGS) $(CFLAGS)

#$(example):example.o
	#$(CC) $^ -o $@ $(LIBS)

#$(test_main):test_main.o make_log.o redis_op.o
	#$(CC) $^ -o $@ $(LIBS)

#$(demo):demo.o
	#$(CC) $^ -o $@ $(LIBS)

#$(upload):upload.o memstr.o
#	$(CC) $^ -o $@ $(LIBS)

$(upload_cgi):upload_cgi.o make_log.o util_cgi.o redis_op.o
	$(CC) $^ -o $@ $(LIBS)

$(data_cgi):data_cgi.o make_log.o util_cgi.o redis_op.o cJSON.o
	$(CC) $^ -o $@ $(LIBS)

.PHONY:clean ALL distclean
clean:
	-rm -rf $(dir) $(target)

distclean:
	-rm -rf $(dir) $(target)

