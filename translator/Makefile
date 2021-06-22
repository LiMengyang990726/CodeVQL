BISON = bison
BISONFLAGS = d
FLEX = flex
CXX = g++

PROGRAM = translator
HELPER = helper

.PHONY : clean

all: ${PROGRAM}
debug: CXXFLAGS += -DDEBUG -g
debug: ${PROGRAM}

${PROGRAM}.tab.c ${PROGRAM}.tab.h: ${PROGRAM}.y
	${BISON} -${BISONFLAGS} $<

lex.yy.c: ${PROGRAM}.l
	${FLEX} $<

${PROGRAM}.tab.o: ${PROGRAM}.tab.c
	${CXX} ${CXXFLAGS} $< -o $@ -c

lex.yy.o: lex.yy.c
	${CXX} ${CXXFLAGS} $< -o $@ -c

${HELPER}/ASTNode.o: ${HELPER}/ASTNode.cpp
	${CXX} ${CXXFLAGS} $< -o $@ -c

${HELPER}/translate.o: ${HELPER}/translate.cpp
	${CXX} ${CXXFLAGS} $< -o $@ -c

${HELPER}/populateMainSouffle.o: ${HELPER}/populateMainSouffle.cpp
	${CXX} ${CXXFLAGS} $< -o $@ -c

${HELPER}/populateRelSouffle.o: ${HELPER}/populateRelSouffle.cpp
	${CXX} ${CXXFLAGS} $< -o $@ -c

${HELPER}/populateVersionSouffle.o: ${HELPER}/populateVersionSouffle.cpp
	${CXX} ${CXXFLAGS} $< -o $@ -c

${HELPER}/symbolStore.o: ${HELPER}/symbolStore.cpp
	${CXX} ${CXXFLAGS} $< -o $@ -c

${HELPER}/utils.o: ${HELPER}/utils.cpp
	${CXX} ${CXXFLAGS} $< -o $@ -c


${PROGRAM}: ${PROGRAM}.tab.o \
			lex.yy.o \
			${HELPER}/ASTNode.o \
			${HELPER}/translate.o \
			${HELPER}/populateMainSouffle.o \
			${HELPER}/populateRelSouffle.o \
			${HELPER}/populateVersionSouffle.o \
			${HELPER}/symbolStore.o \
			${HELPER}/utils.o 
	${CXX} ${CXXFLAGS} -o $@ $^

clean:
	$(RM) ${PROGRAM} \
		  ${PROGRAM}.tab.h \
		  ${PROGRAM}.tab.c \
		  ${PROGRAM}.tab.o \
		  lex.yy.c \
		  lex.yy.o \
		  ${HELPER}/ASTNode.o \
		  ${HELPER}/translate.o \
		  ${HELPER}/populateMainSouffle.o \
		  ${HELPER}/populateRelSouffle.o \
		  ${HELPER}/populateVersionSouffle.o \
		  ${HELPER}/symbolStore.o \
		  ${HELPER}/utils.o 
