BISON = bison
BISONFLAGS = d
FLEX = flex
CC = g++

PROGRAM = codeqltosouffle

.PHONY : clean

all: ${PROGRAM}

${PROGRAM}.tab.c ${PROGRAM}.tab.h: ${PROGRAM}.y
	${BISON} -${BISONFLAGS} $<

lex.yy.c: ${PROGRAM}.l
	${FLEX} $<

${PROGRAM}.tab.o: ${PROGRAM}.tab.c
	${CC} $< -o $@ -c

lex.yy.o: lex.yy.c
	${CC} $< -o $@ -c

${PROGRAM}: ${PROGRAM}.tab.o lex.yy.o
	${CC} -o $@ $^

clean:
	$(RM) ${PROGRAM} ${PROGRAM}.tab.h ${PROGRAM}.tab.c ${PROGRAM}.tab.o lex.yy.c lex.yy.o
