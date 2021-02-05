
hist_out= lab2

hist_headers = 
hist_source  = $(hist_headers:.h=.c) hist.c
hist_objects = $(hist_source:.c=.o)		

HEB=	-lpthread
CC     = gcc
CFLAGS = -Wall

depends = .depends

build : $(hist_out) 

$(hist_out) : $(hist_objects)
	$(CC) $(CFLAGS) -o $@ $^ -lm $(HEB)		

$(objects) :
	$(CC) $(CFLAGS) -c -o $@ $*.c -lm $(HEB)

$(depends) : $(hist_source) $(hist_headers)
	@$(CC) -MM $(hist_source) > $@


clean :
	$(RM) $(hist_out) $(hist_objects) $(zipfile) $(depends)

.PHONY : build zip clean

sinclude $(depends)
