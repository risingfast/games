# -*- makefile -*-
# makefile for mysql-c-library project
# Created: 15-Jun-2021
# Geoffrey Jarman
# Log
#       08-Nov-2021 add bookDetails
#       09-Dec-2021 add bookDelRating
#       10-Dec-2021 add bookDelSeries
#       10-Dec-2021 add bookDetails2
#       20-Sep-2022 add .cgi targets
# $@ Target file
# $^ Dependency files
# $(CC) Compiler executable
# $(CFLAGS) Compiler options
# $@ Souce file
# $^ Dependent files (dependencies)

# Variables

CC=gcc
CFLAGS=-g -o
SQL2FLAGS=-L/usryyp/lib/x86_64-linux-gnu

all: battleShips battleShips.cgi d_getInputTest d_regexWords d_regexWords.cgi guessAnIdiom guessAnIdiom.cgi

battleShips: battleShips.c d_getInput.c
	$(CC) $(CFLAGS) $@ $^ $(SQL2FLAGS)

battleShips.cgi: battleShips.c d_getInput.c
	$(CC) $(CFLAGS) $@ $^ $(SQL2FLAGS)

d_getInputTest: d_getInputTest.c d_getInput.c
	$(CC) $(CFLAGS) $@ $^ $(SQL2FLAGS)

d_regexWords: d_regexWords.c d_getInput.c
	$(CC) $(CFLAGS) $@ $^ $(SQL2FLAGS)

d_regexWords.cgi: d_regexWords.c d_getInput.c
	$(CC) $(CFLAGS) $@ $^ $(SQL2FLAGS)

guessAnIdiom: guessAnIdiom.c d_getInput.c ../shared/rf50.c
	$(CC) $(CFLAGS) $@ $^ $(SQL2FLAGS)

guessAnIdiom.cgi: guessAnIdiom.c d_getInput.c ../shared/rf50.c
	$(CC) $(CFLAGS) $@ $^ $(SQL2FLAGS)

clean:
	rm -f *.o *.s *.i battleShips battleShips.cgi d_getInputTest d_regexWords d_regexWords.cgi guessAnIdiom guessAnIdiom.cgi
