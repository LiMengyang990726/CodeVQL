# This Makefile is designed to be simple and readable.  It does not
# aim at portability.  It requires GNU Make.

BASE = codeqltosouffle
BISON = bison
FLEX = flex

all: $(BASE)

$(BASE).tab.c $(BASE).tab.h: $(BASE).y
        $(BISON) $(BISONFLAGS) -o $@ $<

$(BASE).lex.c: $(BASE).l
        $(FLEX) $(FLEXFLAGS) -o $@ $<

$(BASE).o: $(BASE).c $(BASE).tab.h
        $(CC) $(CFLAGS) -o $@ $^

run: $(BASE)
        @echo "Translate CodeQL into Souffle.  Quit with ctrl-d."
        ./$<
