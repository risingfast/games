//  d_regex.c -- regex drill
//  Geoffrey Jarman
//  Ref: https://www.educative.io/edpresso/how-to-write-regular-expressions-in-c
//       https://www.gnu.org/software/libc/manual/html_node/String-Input-Conversions.html
//       http://sekrit.de/webdocs/c/beginners-guide-away-from-scanf.html
//  Log: 07/10/2020 started
//       07/10/2020 Aaded regex fixed pattern and dictionary file to search
//       07/10/2020 ignore case in the word comparison to the dictionary
//       07/11/2020 allow a user to input a simple matching pattern of letters and wildcard period
//       07/11/2020 limit the number of results
//       07/11/2020 check the pattern for coformance with a limited valid regular expression
//       07/11/2020 show the pattern entered and corresponding regex pattern
//       07/11/2020 show the total number of words scanned
//       07/12/2020 create a main loop and options loop
//       07/12/2020 add an option to change the max number of returned results
//       07/13/2020 fix external variable behaviour for options so they don't reset
//       07/13/2020 suppress apostrophed words from output as an option
//       07/13/2020 make the app switchable between a console app and a web app
//       07/14/2020 parse the GET string for webApp input
//       07/15/2020 split fFindWords into two parts: fetching the console pattern and finding matches
//       07/16/2020 add a handler for no matches
//       07/16/2020 add style to the results page on the web
//       07/16/2020 add a handler for no limit from HTML by limiting input to 1-100 range
//       07/17/2020 make a Yes/No selector field for Apostrphes in HTML
//       07/17/2020 handle no matches on the console application
//       07/17/2020 set the max length of the matching pattern input to 20
//       07/17/2020 show max character positions for pattern at input
//       07/19/2020 change the footer anchor from d_regexWords.html to back
//       07/20/2020 analyzed and fixed a bug returning small result sets
//       07/20/2020 handled null value for row limit as 9999 rows max
//       07/28/2020 expanded the <head> block for html output
//       07/20/2020 format web page output
//       08/14/2020 add meta tags to the <head> region of html outpt
//       08/14/2020 extend comment lines to 100 chars width
//       09/08/2020 add menu bar links
//       10/11/2020 center menu bar links in the main heading
//       10/24/2020 center the body of the html on the webpage
//       04-Jan-2021 webcode
//       27-Mar-2021 reviewed all 
//       26-Sep-2021 remove HTML and print results in new grid layout
//  Enhancements:
//       Handle null value in number of returned results
//

// defines and includes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include "d_getInput.h"
#include <stdbool.h>

#define MAX_LEN 4012

int fFindWords();
int fGetWordPttrn();
int fSetOptions();
void fShowOptions();
    
// external declarations

bool bOptExcludeApost;                                             // Option to exclude apostrophes
bool bOptLimitResults;                                                   // Option to limit results
int iWordLimit;                                                        // Limit on returned results
int iPttrnLimit;                                           // Limit on length of user input pattern
char cyPttrnInp [MAX_LEN] = "";                                               // User input pattern
char cyPttrn[MAX_LEN] = "";                                          // Matching pattern from input
int iPttrnLen;                                                      // Length of pattern from input
bool bfConsoleApp = true;                                        // Flag for console app or web app

int main(void) {

    bOptExcludeApost = true;                                        // Option to exclude apostrphes
    bOptLimitResults = true;                                             // Option to limit results
    iWordLimit = 23;                                                   // Limit on returned results
    iPttrnLimit = 20;                                      // Limit on length of user input pattern

//  automatic declarations for Main()

    char cyMenuChoice[3] = "0";                                               // User choice string
    char cMenuChoice = '0';                                        // User choice converted to char
    int iMenuChoiceLen = 0;                                                // Length of user choice
    bool bExitChoicesNow = false;                                 // Exit flag for user choice loop
    char *sWebGetData;                                               // Pointer to web query string
    char cyWebDelim[] = "&";                                   // Token(s) for splitting GET string
    char *sGetToken;

// check if program is being called from a webform

    if ((sWebGetData = getenv("QUERY_STRING")) == NULL) {
        bfConsoleApp = true;
    }
    else {
        bfConsoleApp = false;
    };

// run as a console app
    
    if(bfConsoleApp == true) {

// clear the screen and show a bannner page
        
        (void) system("clear");
        printf("d_regexWords.c -- Regular expression dictionary scan (as console app)");

// get a user choice as input from the console

        do {
            printf("\n\nChoose:");
            printf("\n\nFind Words Matching a Regex Pattern (1), Options (9), Exit (X): ");
            iMenuChoiceLen = get_string("", cyMenuChoice, 3);
            cMenuChoice = toupper(cyMenuChoice[0]);

            if (iMenuChoiceLen != 1 ) {
                printf("\nInvalid Input. Enter only one number or letter ...");
            } else if (strchr("19X", cMenuChoice) == NULL) {
                printf("\nInvalid Choice. Try again ...");
            }
            else if (cMenuChoice == '1') {
                printf("\nYou entered %c", cMenuChoice);
                if (fGetWordPttrn() == 0) {
                fFindWords();
                }
                else {
                    exit (-1);
                }
            }
            else if (cMenuChoice == '9') {
                printf("\nYou entered %c", cMenuChoice);
                fSetOptions();
            }
            else if (cMenuChoice == 'X') {
                printf("\nYou entered %c", cMenuChoice);
                bExitChoicesNow = true;
            }
         } while (bExitChoicesNow == false);

        printf("\n\nExiting ...\n\n");
        return (0);

    }

// run as a web app

    else {

// tokenize the GET string and assign to variables

        sGetToken = strtok(sWebGetData, cyWebDelim);

        while(sGetToken != NULL) {
            if (strstr(sGetToken, "wp=") !=NULL) {
                strcpy(cyPttrnInp, sGetToken + 3);
                iPttrnLen = strlen(cyPttrnInp);
            } 
            else if (strstr(sGetToken, "rl=") !=NULL) {
                iWordLimit = atoi(sGetToken + 3);
                if (iWordLimit == 0) {
                    iWordLimit = 9999;
                }
            }
            else if (strstr(sGetToken, "sa=") !=NULL) { 
                if (strcmp(sGetToken + 3, "Yes") == 0){
                    bOptExcludeApost = true;
                }
                else if (strcmp(sGetToken + 3, "No") ==0) {
                    bOptExcludeApost = false;
                }
            }
            sGetToken = strtok(NULL, cyWebDelim);
        }

// print the html page content type and <head> block

        printf("Content-type: text/html\n\n");

        fFindWords();

    }
    return 0;
}

// function to set options from console input

int fSetOptions() {

// External declarations

    extern bool bOptExcludeApost;           // Option to exclude apostrophes
    extern bool bOptLimitResults;           // Option to limit results

// Automatic declarations for fSetOptions()

    bool bExitOptionsNow = false;
    int iOptionChoiceLen = 0;
    char cyOptionChoice[3] = "0";
    char cOptionChoice = '0';

// show options and get option changes from the console

    printf("\n\nSet Options");
    do {
        fShowOptions();
        iOptionChoiceLen = get_string("", cyOptionChoice, 3);
        cOptionChoice = toupper(cyOptionChoice[0]);
        if (iOptionChoiceLen != 1 ) {
            printf("\nInvalid Input. Enter only one letter ...");
        } else if (strchr("ABX", cOptionChoice) == NULL) {
            printf("\nInvalid Choice. Try again ...");
        }
        else if (cOptionChoice == 'A') {
            printf("\nYou entered %c", cOptionChoice);
            if (bOptLimitResults == true) {
                bOptLimitResults = false;
            }
            else {
                bOptLimitResults = true;
            }
        }
        else if (cOptionChoice == 'B') {
            printf("\nYou entered %c", cOptionChoice);
            if (bOptExcludeApost == true) {
                bOptExcludeApost = false;
            }
            else {
                bOptExcludeApost = true;
            }
        }
        else if (cOptionChoice == 'X') {
            printf("\nYou entered %c", cOptionChoice);
            bExitOptionsNow = true;
        }
    } while (bExitOptionsNow == false);
}

// function to display options and their settings

void fShowOptions() {

    // External declarations

    extern bool bOptExcludeApost;                                  // Option to exclude apostrophes
    extern bool bOptLimitResults;                                        // Option to limit results
    extern int iWordLimit;                                             // Limit on returned results

// show options and settings on the console

    printf("\n\nOptions:");
    if (bOptLimitResults == true) {
        printf("\n\nA. Results Limit: %d", iWordLimit);
    }
    else {
        printf("\n\nA. Results Limit: No");
    }
    if (bOptExcludeApost == true) {
        printf("\nB. Suppress results with apostraphes: Yes");
    }
    else {
        printf("\nB. Suppress results with apostraphes: No");
    }
    printf("\nX. Exit Options: ");
    printf("\n\nChoose: ");
}

int fGetWordPttrn() {

// External declarations

    extern bool bOptExcludeApost;                                  // Option to exclude apostrophes
    extern bool bOptLimitResults;                                        // Option to limit results
    extern int iPttrnLimit;                                   // Limit on user input pattern length
    extern char cyPttrnInp [MAX_LEN];                                         // User input pattern
    extern int iPttrnLen;                                                // Length of pattern input
    extern bool bfConsoleApp;                                    // Flag for console app or web app

// get a word pattern from the console in simple regex format

    cyPttrnInp[0] = '\0';                                  
    printf("\n\nEnter a word pattern using letters and dots (like M..day)");
    printf("\nDots are wildcards, e.g: 'l.v.' matches 'love', 'live' and 'lava')");
    printf("\nEnter one pattern character for each letter in the target word.");
    iPttrnLen = get_string("\n\nWord pattern", cyPttrnInp, iPttrnLimit + 2);
    if (iPttrnLen == 0) {
        printf("\nPattern too long or too short. Exiting ...");
        printf("\n\n");
        return (-1);
    }

// covert the pattern to lower case then check for unwanted characters

    for (int i = 0; i < iPttrnLen; i++) {
        cyPttrnInp[i] = tolower(cyPttrnInp[i]);
        if ((cyPttrnInp[i] > 122 || cyPttrnInp[i] < 97) && cyPttrnInp[i] != 46) {
            printf("\nOnly alpha characters and periods allowed. Exiting ...\n\n");
            exit (1);
        }
    }
    return 0;
}

// Function to find words in the dictionary file matching the word pattern

int fFindWords() {

// External declarations

    extern bool bOptExcludeApost;                                  // Option to exclude apostrophes
    extern bool bOptLimitResults;                                        // Option to limit results
    extern int iWordLimit;                                             // Limit on returned results
    extern char cyPttrnInp[MAX_LEN];                                 // Matching pattern from input
    extern char cyPttrn[MAX_LEN];                                    // Matching pattern from input
    extern int iPttrnLen;                                                // Length of pattern input
    extern bool bfConsoleApp;                                    // Flag for console app or web app

// Automatic declarations for fFindWords()

    regex_t rWldCrd;
    int iRtnVal1 = 1;
    FILE *fptr;
    char cyDctWrd[100];
    int iWrdMtch = 0;
    int iRtnVal2 = 1;
    int iWrdCnt = 0;

// add Regex leading and trailing word markers to the pattern

    strcpy(cyPttrn, cyPttrnInp);
    cyPttrn[iPttrnLen+1] = '$';
    cyPttrn[iPttrnLen+2] = '\0';
    cyPttrn[0] = '^';
    for (int i = 0; i < iPttrnLen; i++) {
        cyPttrn[i+1] = cyPttrnInp[i];
    }

// compile the regular expression

    iRtnVal1 = regcomp(&rWldCrd, cyPttrn, 0);
    if (iRtnVal1 == 0) {
    }
    else {
        printf("\nRegular expression %s did not compile.", cyPttrn);
        printf("\n\nExiting");
        printf("\n\n");
        return 1;
    }

// open the dictionary file

    if ((fptr = fopen("Words.dat", "r")) == NULL) {
        printf("\n\nError opening ./Words.dat dictionary file\n\nExiting");
        printf("\n\n");
        return 1;
    }

// read each line in the dictionary and compare to the compiled regex

    while (fgets(cyDctWrd, MAX_LEN, fptr)) {
        iWrdCnt++;
        cyDctWrd[strcspn(cyDctWrd, "\n")] = '\0';

// lowercase the initial letters

        cyDctWrd[0] = tolower(cyDctWrd[0]);

// print result if wordlimit not exceeded and no apostophes found

        if((iRtnVal2 = regexec(&rWldCrd, cyDctWrd, 0, NULL, 0)) == 0) {;
            if (strlen(cyDctWrd) == 0) {
                continue;
            }
            else if (iWrdMtch >= iWordLimit && bOptLimitResults == true) {
                break;
            }
            else if ((strchr(cyDctWrd, '\'') != NULL) && bOptExcludeApost == true) {
                continue;
            }
            else {
                if (bfConsoleApp == false) {
                    printf("%3i: %s\n", iWrdMtch + 1, cyDctWrd);
                }
                else {
                    printf("%3i: %s\n", iWrdMtch + 1, cyDctWrd);
                }
                iWrdMtch++;
            }
        }
    }
    if (iWrdMtch == 0) {
        printf("\nNo matches found for pattern %s", cyPttrn);
    }
    fclose(fptr);

    return 0;
}
