#  Variables
EXE = reversi

#  Usual  compilation  flags
CFLAGS = -g -pg -std=c11 -Wall -Wextra -O2
CPPFLAGS = -I../include
LDFLAGS =

#  Special  rules  and  targets
.PHONY: all  clean  help

# Rules and targets
all: $(EXE)

$(EXE): reversi.o board.o moves.o player.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

reversi.o: reversi.c reversi.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

board.o: board.c ../include/board.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

moves.o: moves.c ../include/moves.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

player.o: player.c ../include/player.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

clean:
	@rm -f *~ *.o $(EXE)

help:
	@echo "Usage:"
	@echo -e " make [all]\t\Run the whole build of reversi "
	@echo -e " make clean\t\Remove all files produced by the compilation"
	@echo -e " make help\t\Display the main of this Makefile with description"
