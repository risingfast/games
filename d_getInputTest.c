// d_getInputTest.c -- drill to test functions in d_getInput.c
// Author -- Geoffrey Jarman
// Started -- 18-Apr-2020
// References:
//        https://www.youtube.com/watch?v=GExnnTaBELk&t=57s -- Barry Brown, makefiles
// Log:
//     18-Apr-2020 started
//     17-Jan-2020 reviewed all
//     17-Jan-2020 consolecode
//  Enhancements(0):
//

// includes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "d_getInput.h"

int main(int argc, char **argv)
{

// declarations

    char *sProgName = argv[0] + 2;

    system("clear");
    printf("%s -- drills to test functions in d_getInput.c", sProgName);
    printf("\n\n");
    
// check for input parameter --help or --h

    for (int i = 1; i < argc; i++) {
        if (strcmp("--help", argv[i]) == 0 || strcmp("--h", argv[i]) == 0) {
            printf("\n%s - there is no help for this program\n", sProgName);
            printf("\n");
        }
    }
    printf("\n");

    // Test get_double() in d_getInput.c to fetch a double from stdin and check against a range
    
    double lfInputVal = 0;

    lfInputVal = get_double("Enter a floating point number between 0 and 1000: ", 0, 1000);
    printf("\nYou entered %lf", lfInputVal);
    printf("\n\n");

    // Test get_String() in d_getInput.c to fetch a string no greater than max characters

    char *strPrompt = "Type some text";
    int nMaxLen = 120;
    int nStrLen = 0;
    char strResponse[nMaxLen];

    nStrLen = get_string(strPrompt, strResponse, nMaxLen);

    if (nStrLen == 0)
    {
        printf("\nResponse invalid\n");
    }
    else
    {
        printf("\nResponse: Length: %d, Response: %s", nStrLen, strResponse);
    }
    printf("\n\n");
    return 0;
}
