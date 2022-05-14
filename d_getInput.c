//  d_getInput.c -- Input drill
//  Author -- Geoffrey Jarman
//  Started -- 18-Apr-2020
//  Description:
//      Tested via d_getInputTest.c
//  References:
//      https://www.youtube.com/watch?v=GExnnTaBELk&t=57s -- Barry Brown, Makefiles
//  Log: 
//      18-Apr-2020 started
//      17-Jan-2020 reviewed all
//      17-Jan-2020 consolecode

// includes

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// function to fetch a long double from stdin and check for bounds

double get_double(char* prompt, double lfMin, double lfMax) {

    double lfInput;

    do {
        printf("%s", prompt);
        scanf(" %lf", &lfInput);
        if (lfInput < lfMin) printf("Must be at least %.2lf\n", lfMin);
        if (lfInput > lfMax) printf("Must be at most %.2lf\n",  lfMax);
    } while (lfInput < lfMin || lfInput > lfMax);

    while(getchar() != '\n')             // clear stdin of trailing characters
    {
        ;
    }

    return lfInput;
}

// function to fetch characters from stdin up to a max length and reject longer input

int get_string(char* prompt, char* strInput, int iMaxLen) {

    char chC;
    int  iRetVal = 0;
    bool bNewLineFound = false;
    bool bNonNumericInput = false;
    bool bValidInput = false;

// Initialize the input array to all '\0's
    for (int i = 0; i < iMaxLen; i++) {
        strInput[i] = '\0';
    }

    if (iMaxLen - 2 == 1) {
        printf("%s (Type only one character): ", prompt);
    } else {
        printf("%s (%d chars): ", prompt, iMaxLen - 2);
    }

// Get a string of text from the console

    fgets(strInput, iMaxLen, stdin);

// Reset the flags before entering the input loop

    bNewLineFound = false;
    bNonNumericInput = false;
    bValidInput = false;

// Check for a newline in the input string up to (not including) the last position

    for (int nCntr = 0; nCntr < iMaxLen - 1; nCntr++) {
        if (strInput[nCntr] == '\n') {
            bNewLineFound = true;
            bValidInput = true;
        }
    }

// Clear the stdin buffer of excess input exceeding iMaxLen then empty StrInput[]

    if (bNewLineFound == false) {
        bValidInput = false;
        do {
            chC = getchar();
        } while (chC != '\n');
        strInput[0] = '\0';
    }

// Replace the '\n' from the string with '\0'
    if (bValidInput == true) {
        for (int i = 0; i < strlen(strInput); i++) {
            if (strInput[i] == '\n') {
                strInput[i] = '\0';
            }
        }
    }

    // Set the return strlen or 0 if invalid

    if (bValidInput == true) {
        iRetVal = strlen(strInput);
    } else {
       iRetVal = 0;
    }

    return iRetVal;
}

