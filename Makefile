PERL_CORE=c:/strawberry/perl/lib/core
SRC=edit.c
OBJS=$(SRC:.c=.o)
RC=edit.rc
RES=$(RC:.rc=.res)
PROG=edit.exe
CC=gcc
CFLAGS=-Wall -O3 -I$(PERL_CORE)
LDFLAGS=-mwindows -L$(PERL_CORE) -lperl510
RM=del 

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY : all
all: $(PROG)

$(PROG): $(OBJS) $(RES)
	$(CC) $(OBJS) $(RES) $(LDFLAGS) -o $@

$(RES): $(RC) resource.h
	windres -O omf $(RC) $(RES)

wndproc.o: resource.h

.PHONY : clean
clean:
	$(RM) $(OBJS) $(WINDRES)

