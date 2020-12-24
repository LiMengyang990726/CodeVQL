BISON = bison
BISONFLAGS = d
FLEX = flex
CC = g++

PROGRAM = codeqltosouffle
HELPER = helper

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

${HELPER}/ASTNode.o: ${HELPER}/ASTNode.cpp
	${CC} $< -o $@ -c

${HELPER}/populateMainDL.o: ${HELPER}/populateMainDL.cpp
	${CC} $< -o $@ -c

${HELPER}/symbolStore.o: ${HELPER}/symbolStore.cpp
	${CC} $< -o $@ -c

${HELPER}/translate.o: ${HELPER}/translate.cpp
	${CC} $< -o $@ -c

${HELPER}/utils.o: ${HELPER}/utils.cpp
	${CC} $< -o $@ -c

${HELPER}/populateRelDL.o: ${HELPER}/populateRelDL.cpp
	${CC} $< -o $@ -c

${HELPER}/populateVersionDL.o: ${HELPER}/populateVersionDL.cpp
	${CC} $< -o $@ -c

${PROGRAM}: ${PROGRAM}.tab.o lex.yy.o ${HELPER}/ASTNode.o ${HELPER}/populateMainDL.o ${HELPER}/symbolStore.o ${HELPER}/translate.o ${HELPER}/utils.o ${HELPER}/populateRelDL.o ${HELPER}/populateVersionDL.o
	${CC} -o $@ $^

clean:
	$(RM) ${PROGRAM} ${PROGRAM}.tab.h ${PROGRAM}.tab.c ${PROGRAM}.tab.o lex.yy.c lex.yy.o ${HELPER}/ASTNode.o ${HELPER}/populateMainDL.o ${HELPER}/symbolStore.o ${HELPER}/translate.o ${HELPER}/utils.o ${HELPER}/populateRelDL.o ${HELPER}/populateVersionDL.o
