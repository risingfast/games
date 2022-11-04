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
#       02-Nov-2022 add rRoulette
# $@ Target file
# $^ Dependency files
# $(CC) Compiler executable
# $(CFLAGS) Compiler options
# $@ Souce file
# $^ Dependent files (dependencies)

# Variables

CC=gcc
CFLAGS=-g -o
SQL1FLAGS=-I/usr/include/mysql
SQL2FLAGS=-L/usryyp/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl -lresolv

all: battleShips battleShips.cgi d_getInputTest d_regexWords d_regexWords.cgi guessAnIdiom guessAnIdiom.cgi rRoulette rRoulette.cgi

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

rRoulette: rRoulette.c
	$(CC) $(CFLAGS) $@ $(SQL1FLAGS) $^ $(SQL2FLAGS) -ljson-c

rRoulette.cgi: rRoulette.c
	$(CC) $(CFLAGS) $@ $(SQL1FLAGS) $^ $(SQL2FLAGS) -ljson-c

clean:
	rm -f *.o *.s *.i battleShips battleShips.cgi d_getInputTest d_regexWords d_regexWords.cgi guessAnIdiom guessAnIdiom.cgi rRoulette rRoulette.cgi
