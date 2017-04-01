OBJS    = y.tab.o lex.yy.o cwatch.o main.o debug.o getchunk.o options.o
LDADD   =  -ll -lpcre
CC	= cc
CFLAGS  = -I/usr/local/include
LDFLAGS = -L/usr/local/lib
MANDIR  = /usr/local/man
BINDIR  = /usr/local/bin
PROGS   = cwatch
MANPAGES= cwatch.1

all: $(PROGS)

cwatch: $(OBJS)
	$(CC) $(LDFLAGS) -o cwatch $(OBJS) $(LDADD)

lex.yy.c: scanner.l y.tab.h 
	/usr/bin/lex -l -i $(LEX_DEBUG) scanner.l

y.tab.h y.tab.c: parser.y
	/usr/bin/yacc $(YACC_DEBUG) -d parser.y

$(OBJS): cwatch.h Makefile

options.o fake: basis/options.c
	$(CC) -c -o options.o $<

clean:
	rm -f $(OBJS) $(LIB)
	rm -f *.o y.tab.h y.tab.c lex.yy.c
	rm -f $(PROGS)

distclean spotless: clean
	rm -f y.output
	rm -f Makefile cwatch.h cwatch.1 config.cmd config.sub config.h config.mak config.log config.md librarian.sh

install: VERSION install.man install.bin

install.bin: $(PROGS)
	install -c -s $(PROGS) $(BINDIR)

install.man: install_manpages
	./install_manpages $(MANDIR) $(MANPAGES)

install_manpages:
	get install_manpages
