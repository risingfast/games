//  guessAnIdiom.c - idiom guessing game (like Hangman)
//  Author: Geoffrey Jarman
//  Started 02-Mar-2020
//  Functions:
//      chooseNum() -- get a user menu choice (integer) from the console
//  Files:
//      guessAnIdiomG.dat - list of letter guesses in the current game (guesses)
//      guessAnIdiomS.dat - letter guesses in matching positions in the current game (status)
//      guessAnIdiomT.dat - the target idiom for the current game (target)
//  Log: 
//      08/07/2020 creat logic for console vs web
//      08/07/2020 standardiz layout and comments
//      08/08/2020 add web standard functions
//      08/08/2020 count and print the number of guesses at the end
//      08/08/2020 reject any letters already given, correct or incorrect
//      08/09/2020 renam to guessPhrase.c
//      08/10/2020 add web code for guessing using letters
//      08/10/2020 standardize variable prefixes in all code
//      08/10/2020 implement files to save solution and guesses for web
//      08/11/2020 add cgi logic to work on the web
//      08/11/2020 show a period for each unmatched letter in the solution
//      08/12/2020 make the target sentence dynamic and changing
//      08/12/2020 debug after making change for random idiom selection
//      08/12/2020 select an idiom from a file at random for each new game
//      08/12/2020 add a reset option to the console version
//      08/13/2020 fix the internals list of file names
//      08/13/2020 expand the game over messages
//      08/13/2029 fix game reset after guessing correct sentence
//      08/13/2020 fix formatting on game completion messages for web
//      08/13/2020 add a game status option for in-progress game status
//      08/13/2020 standardize the lengths used to 60 and move to a #DEFINE
//      09/08/2020 add menu bar links
//      10/11/2020 center menu links in the main heading bar
//      10/23/2020 center the body of the html on the webpage
//      04-Jan-2021 webcode
//      27-Mar-2021 reviewed all
//      28-Sep-2021 removed HTML output for grid formatting
//      28-Sep-2021 changed from POST to GET string handling
//      15-Sep-2022 add Access-Control-Allow-Origin: * CORS http header
//  Enhancements
//      Create a file to remember sentence guesses and provide a count
//      Use the get_string() function to read the full sentence guess in the console

// includes and defines

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include "../shared/rf50.h"

#define LEN80   80
#define LEN100 100

// main function (common)

int main(void) {

// automatic variable declarations (common)

    char cyTarget[LEN80] = {'\0'};               // target sentence
    long int intLen = 0;                         // target sentence length
    char cySolution[LEN80] = {'\0'};             // cumulative guessed chars in solution
    char cyGuessSnt[LEN80] = {'\0'};             // full sentence guess
    char *strWebStdInLen;                        // length of content in stdin from HTML form post
    int  intWebStdInLen;                         // strWebStdInLen as an int
    int  intRespValWeb = 0;                      // integer of web response action
    char cyGuesses[LEN80] = {'\0'};              // string of characters guessed
    char chrGuess = '@';                         // single char guess
    int  intCount = 0;                           // utility loop counter
    int  intChoice;                              // game option chosen by user
    char cyInput[LEN80];                         // console input of letter or character
    char cyWebContent[LEN100] = {'\0'};          // stdin content
    int  intChrGuessCnt = 0;                     // count of character guesses
    int  intStcGuessCnt = 0;                     // count of sentence guesses
    char chrWebLetterResp;                       // letter guess submitted in the webform
    char cyWebIdiomResp[LEN100] = {'\0'};        // idiom guess submitteed in the webform
    int  intRandom = 0;                          // random number
    bool bShowAudit = false;                     // flag to show audit in fUrlDecode()


// file handles (common)

    FILE *fGuesses;                              // file of letter guesses guessAnIdiomG.dat
    FILE *fSolution;                             // file of user cumulative solution guessAnIdiomS.dat
    FILE *fCandidates;                           // file of idiom candidates guessAnIdiomC.dat
    FILE *fTarget;                               // file of the target answer guessAnIdiomT.dat

// forward function declarations (common)

    int  fChooseNum(int lowNum, int higNum);                            // fn to choose game option

// automatic flag declarations (common)

    bool bfConsoleApp = false;                                       // flag for console or web app
    bool bfGameOver = false;

// load a target idiom with a random candidate if no target exists (common)

    if ((fTarget = fopen("guessAnIdiomT.dat", "r+")) == NULL) {          // cannot open target file
        printf("Cannot open guessAnIdiomT.dat for reading");
    }
    else {
        if (fgets(cyTarget, LEN80, fTarget) == NULL) {                      // target file is empty
            if ((fCandidates = fopen("guessAnIdiomC.dat", "r")) == NULL) { // cannot open candidate
                printf("Cannot open guessAnIdiomC.dat for reading");
            }
            else {                                                    // candidate file is readable
                srand(time(NULL));
                intRandom = rand() % 70;

                for (int intLoop = 0; intLoop <= intRandom; intLoop++) {
                    if ((fgets(cyTarget, LEN80, fCandidates) == NULL)) { // cannot read a candidate
                        printf("Cannot read guessAnIdiomC.dat");
                    }
                }
            }

            fflush(fTarget);
            if ((fputs(cyTarget, fTarget) == EOF)) {            // cannot write new idiom to target
                printf("\nUpdate to guessAnIdiomT.dat failed");
            };

            fclose(fTarget);
            fclose(fCandidates);
        }
        intLen = strlen(cyTarget);
        if (cyTarget[intLen - 1] == '\n') {          // remove any trailing newline from the target
            cyTarget[intLen - 1] = '\0';
        }
    }

// fill the solution string with periods for letters and spaces for spaces (common)

    for (intCount = 0; intCount < intLen - 1; intCount++) {    // fill solution with periods and spaces
        if (*(cyTarget + intCount) != ' ') {
            cySolution[intCount] = '.';
        }
        else {
            cySolution[intCount] = ' ';
        }
    }
    cySolution[intLen] = '\0';

// check if program is called from the web or the console

//    setenv("QUERY_STRING", "http://www.risingfast.com/cgi-bin/guessAnIdiom.cgi?Action=Letter&Letter=A&Idiom=", 1);

     if (getenv("QUERY_STRING") == NULL) {
         bfConsoleApp = true;                         // NULL pointer means no CONTENT_LENGTH found
     }                                                // so run as console app
     else {
         intWebStdInLen = strlen(getenv("QUERY_STRING"));
             bfConsoleApp = false; 
     }

// run as a console app (console)

    if (bfConsoleApp == true) {

// clear the screen and print the banner (console)

        system("clear");
        printf("guessAnIdiom.c - idiom guessing game\n\n");

// start the game loop and get a menu choice (console)

        while (bfGameOver == false) {
            printf("Guess a character (1) or the whole sentence (2) reset (3) or exit (4)\n");
            intChoice = fChooseNum(1, 4);

// option 1 evaluate guessing a letter (console)

            if (intChoice == 1) {
                printf("<DIV style=\"width:80%%; margin:auto; max-width:800px;\">");
                printf("\nPattern: |->%s<-|", cySolution);
                printf("\n");
                printf("\nLetter?: ");
                chrGuess = toupper(getchar());
                fgets(cyInput, 80, stdin);                         // scan and clear trailing chars
                printf("You entered: %c", chrGuess);
                if (strchr(cyGuesses, chrGuess) != NULL) {
                    printf("\n");
                    printf("\nYou already guessed: %c", chrGuess);
                }
                else {
                    intChrGuessCnt++;
                    cyGuesses[intChrGuessCnt - 1] = chrGuess;
                }
                for (intCount = 0; intCount < intLen; intCount++) {
                    if (cyTarget[intCount] == chrGuess) {
                        cySolution[intCount] = chrGuess;
                    }
                }
                if (strcmp(cySolution, cyTarget) == 0) {
                    printf("\n");
                    printf("\nYou matched the target sentence");
                    printf("\n");
                    printf("\nGame over");
                    fTarget = fopen("guessAnIdiomT.dat", "w");          // truncate the target file
                    fclose(fTarget);
                    bfGameOver = true;
                }
                printf("\n");
                printf("\nMatches: |->%s<-|\n", cySolution);
                printf("\nGuesses so far Characters: %s, Sentences: %d\n", cyGuesses, intStcGuessCnt);
                printf("</DIV>");
            }

// option 2 evaluate guessing the full sentence (console)

            else if (intChoice == 2) {
                printf("\nYou chose to guess the sentence");
                printf("\n");
                printf("\nPattern: |->%s<-|", cySolution);
                printf("\n");
                printf("\nWhat is the sentence?: ");
                fgets(cyGuessSnt, intLen + 1, stdin); 
                intStcGuessCnt++;
                for (intCount = 0; intCount < strlen(cyGuessSnt); intCount++) {
                    *(cyGuessSnt + intCount) = toupper(*(cyGuessSnt + intCount));
                }
                printf("\n");
                if ((cyGuessSnt[strlen(cyGuessSnt) - 1]) == '\n') { // remove any trailing newline
                    cyGuessSnt[strlen(cyGuessSnt) - 1] = '\0';
                }
                printf("You entered: %s", cyGuessSnt);
                if (strcmp(cyGuessSnt, cyTarget) == 0) {
                    printf("\n");
                    printf("\nCorrect.");
                    printf("\n");
                    printf("\n");
                    printf("Your guess matches the target sentence");
                    printf("\n");
                    printf("\nGuesses so far Characters: %s, Sentences: %d\n", cyGuesses, intStcGuessCnt);
                    printf("\n");
                    printf("Game over");
                    fTarget = fopen("guessAnIdiomT.dat", "w");          // truncate the target file
                    fclose(fTarget);
                    printf("\n");
                    bfGameOver = true;
                } else {
                    printf("\n");
                    printf("\nGuesses so far Chr: %s, Sentences: %d", cyGuesses, intStcGuessCnt);
                    printf("\n");
                    printf("\nNot a match. Continue ...\n");
                }

// option 3 reset and refresh the game (console)

            } else if (intChoice == 3) {
                printf("\nYou chose to reset");
                printf("\n");

                fTarget = fopen("guessAnIdiomT.dat", "w");              // truncate the target file
                printf("\nTarget file truncated");
                fclose(fTarget);
                for (int intLoop = 0; intLoop < 61; intLoop++) {
                    cySolution[intLoop] = '\0';
                }
                printf("\nTarget variable truncated");

                for (int intLoop = 0; intLoop < 61; intLoop++) {       // truncate the target field
                    cyTarget[intLoop] = '\0';
                }
                printf("\nTarget variable truncated");    // truncate the history of letter guesses
                for (int intLoop = 0; intLoop < 61; intLoop++) {
                    cyGuesses[intLoop] = '\0';
                }
                printf("\nLetter guesses variable truncated");

                for (int intLoop = 0; intLoop < 61; intLoop++) {     // truncate the sentence guess
                    cyGuessSnt[intLoop] = '\0';
                }
                printf("\nSentence guess variable truncated"); 

                intChrGuessCnt = 0;                                        // zero out guess counts
                intStcGuessCnt = 0;
                printf("\nGuess counters zeroed out");

// option 3 refresh starts here (console)

                if ((fTarget = fopen("guessAnIdiomT.dat", "r+")) == NULL) {          // cannot open target file
                    printf("Cannot open guessAnIdiomT.dat for reading");
                }
                else {
                    if (fgets(cyTarget, LEN80, fTarget) == NULL) {                        // target file is empty
                        if ((fCandidates = fopen("guessAnIdiomC.dat", "r")) == NULL) { // cannot open candidate
                            printf("Cannot open guessAnIdiomC.dat for reading");
                        }
                        else {                                                    // candidate file is readable
                            srand(time(NULL));
                            intRandom = rand() % 40;

                            for (int intLoop = 0; intLoop <= intRandom; intLoop++) {
                                if ((fgets(cyTarget, 61, fCandidates) == NULL)) {   // cannot read a candidate
                                    printf("Cannot read guessAnIdiomC.dat");
                                }
                            }
                        }

                        fflush(fTarget);
                        if ((fputs(cyTarget, fTarget) == EOF)) {           // cannot write new idiom to target
                            printf("\nUpdate to guessAnIdiomT.dat failed");
                        };

                        fclose(fTarget);
                        fclose(fCandidates);
                    }
                    intLen = strlen(cyTarget);
                    if (cyTarget[intLen - 1] == '\n') {         // remove any trailing newline from the target
                        cyTarget[intLen - 1] = '\0';
                    }
                }

                for (intCount = 0; intCount < intLen - 1; intCount++) {    // fill solution with periods and spaces
                    if (*(cyTarget + intCount) != ' ') {
                        cySolution[intCount] = '.';
                    }
                    else {
                        cySolution[intCount] = ' ';
                    }
                }
                cySolution[intLen] = '\0';

// option 4 exit the game immediately (console)

            } else if (intChoice == 4) {
                printf("\nYou chose 4, so exiting ...");
                printf("\n");
                bfGameOver = true;
                printf("\nTarget sentence was '%s'\n", cyTarget);
                printf("\n");
                printf("Game over\n");
            }
            printf("\n");
        }
        return(0);
    }

// run as a web app (web)

    else {

// identify the web choice submitted (web)

        strcpy(cyWebContent,getenv("QUERY_STRING"));

        if (strstr(cyWebContent, "Action=Letter") != NULL) {
            intRespValWeb = 1;
        }
        else if (strstr(cyWebContent, "Action=Idiom") != NULL) {
            intRespValWeb = 2;
        }
        else if (strstr(cyWebContent, "Action=Status") != NULL) {
            intRespValWeb = 3;
        }
        else if (strstr(cyWebContent, "Action=Reset") != NULL) {
            intRespValWeb = 4;
        }
        else if (strstr(cyWebContent, "Action=Options") != NULL) {
            intRespValWeb = 5;
        }
        else {
            intRespValWeb = 0;
        }

// print the HTML content type http header and CORS http header

        printf("Content-type: text/html\n");
        printf("Access-Control-Allow-Origin: *\n\n");

// option 1 Letter handler (web)

        if (intRespValWeb == 1) {
            chrWebLetterResp = toupper(*(strstr(cyWebContent, "&Letter=") + 8));
            printf("Letter Response: %c", chrWebLetterResp);

            fGuesses = fopen("guessAnIdiomG.dat", "r");                // cumulative letter guesses
            fSolution = fopen("guessAnIdiomS.dat", "r");             // cumulative guesses solution

            fgets(cyGuesses, LEN80, fGuesses);
            fgets(cySolution, LEN80, fSolution);

            fclose(fSolution);
            fclose(fGuesses);

            intChrGuessCnt = strlen(cyGuesses);

            if (strchr(cyGuesses, chrWebLetterResp) != NULL) {
                printf("\nYou already guessed: %c", chrWebLetterResp);
            }
            else {
                intChrGuessCnt++;
                cyGuesses[intChrGuessCnt - 1] = chrWebLetterResp;
            }

            fGuesses = fopen("guessAnIdiomG.dat", "w");
            fSolution = fopen("guessAnIdiomS.dat", "w");

            fputs(cyGuesses, fGuesses);

            fclose(fGuesses);

            for (intCount = 0; intCount < intLen; intCount++) {
                if (cyTarget[intCount] == chrWebLetterResp) {
                    cySolution[intCount] = chrWebLetterResp;
                }
            }

            fputs(cySolution, fSolution);
            fclose(fSolution);

// test if game is over and truncate the guesses and solution file for a new game (web)

            if (strcmp(cySolution, cyTarget) == 0) {
                printf("\n");
                printf("You matched the target sentence");
                printf("\nGame over\n");
                fGuesses = fopen("guessAnIdiomG.dat", "w");
                fSolution = fopen("guessAnIdiomS.dat", "w");
                fTarget = fopen("guessAnIdiomT.dat", "w");
                fclose(fGuesses);
                fclose(fSolution);
                fclose(fTarget);
                printf("\nGame reset");
                printf("\nA new game will will start with the next guess");
                bfGameOver = true;
            }
            printf("\nMatches: |->%s<-|\n", cySolution);
            printf("\nGuesses so far Chr: %s, Sentences: %d\n", cyGuesses, intStcGuessCnt);
        }

// option 2 Idiom handler (web)

        else if (intRespValWeb == 2) {

// extract the user's guess of the idiom from web content (web)

            strcpy(cyWebIdiomResp, fUrlDecode(strstr(cyWebContent, "&Idiom=") + 7));
            int i = 0;
            while (cyWebIdiomResp[i] != '\0') {
                if (cyWebIdiomResp[i] == '+') {
                    cyWebIdiomResp[i] = ' ';
                }
                i++;
            }

            intStcGuessCnt++;
            for (intCount = 0; intCount < strlen(cyWebIdiomResp); intCount++) {
                *(cyWebIdiomResp + intCount) = toupper(*(cyWebIdiomResp + intCount));
            }
            
            printf("You entered: %s", cyWebIdiomResp);

            fGuesses = fopen("guessAnIdiomG.dat", "r");     // cumulative letter guesses
            fgets(cyGuesses, LEN80, fGuesses);
            fclose(fGuesses);

            if (strcmp(cyWebIdiomResp, cyTarget) == 0) {
                printf("\nCorrect.");
                printf("You matched the target sentence");
                printf("\nGuesses so far Chr: %s, Sentences: %d\n", cyGuesses, intStcGuessCnt);
                printf("\nGame over");
                fGuesses = fopen("guessAnIdiomG.dat", "w");
                fSolution = fopen("guessAnIdiomS.dat", "w");
                fTarget = fopen("guessAnIdiomT.dat", "w");
                fclose(fGuesses);
                fclose(fSolution);
                fclose(fTarget);
                printf("\nGame reset");
                printf("\n");
                printf("\nNew game will start with next guess");
                bfGameOver = true;
            } else {
                printf("\nGuesses so far Chr: %s, Sentences: %d", cyGuesses, intStcGuessCnt);
                printf("\nNot a match.");
            }
        }

// option 3 Status handler (web)

        else if (intRespValWeb == 3) {

            fGuesses = fopen("guessAnIdiomG.dat", "r");                // cumulative letter guesses
            fSolution = fopen("guessAnIdiomS.dat", "r");             // cumulative guesses solution

            fgets(cyGuesses, LEN80, fGuesses);
            fgets(cySolution, LEN80, fSolution);

            fclose(fSolution);
            fclose(fGuesses);

            printf("Game status");
            printf("\nSolution: |->%s<-|", cySolution);
            if (strlen(cyGuesses) == 0) {
                printf("\nLetter guesses: <none>");
            }
            else {
                printf("\nLetter guesses: %s", cyGuesses);
            }
            printf("\nSentence guesses: %d", intStcGuessCnt);
        }
// option 4 Reset handler (web)

        else if (intRespValWeb == 4) {
            fGuesses = fopen("guessAnIdiomG.dat", "w");     // truncate the guesses file
            fSolution = fopen("guessAnIdiomS.dat", "w");     // truncate the solution file
            fTarget = fopen("guessAnIdiomT.dat", "w");      // truncate the solution file

            fclose(fGuesses);
            fclose(fSolution);
            fclose(fTarget);
 
            printf("Game reset");
            printf("\nguessAnIdiomG.dat truncated - the list of letter guesses");
            printf("\nguessAnIdiomS.dat truncated - the solution of letters guessed so far");
            printf("\nguessAnIdiomT.dat truncated - the target idiom");
        }

// option 5 Options handler (web)

        else if (intRespValWeb == 5) {
            printf("No options implemented");
        }

// option unknown handler (web)

        else {
            printf("\nUnknown option detected and not supported");
        }

    }
    return 0;
}

// function to get a valid game option number or keep looping (console)

int fChooseNum(int lowNum, int highNum)                               // choose a number from stdin
{                                                                           // between low and high
    bool bfChoiceOutOfRange;                                            // flag choice out of range
    bool bfChoiceInvalid;                                                 // flag choice is invalie
    char cyInput[80];                                                // input read from the console
    int  intRetValue = 0;                                    // count of choices scanned from input
    int  intChoice = 0;                                                  // user menu choice number
    
    do {
        bfChoiceOutOfRange = false;                                      // clear out of range flag
        bfChoiceInvalid = false;                                              // clear invalid flag
        printf("\nChoose (%d-%d): ", lowNum, highNum);                       // prompt for a choice
        intRetValue = scanf(" %d", &intChoice);                                // scan for a choice
        fgets(cyInput, 80, stdin);                               // scan for trailing invalid chars
        if (intRetValue != 1 || strlen(cyInput) > 1) {            // increment invalid choice count
            bfChoiceInvalid = true;
            printf("\nInvalid choice. ");
            printf("\n");
        } else if (intChoice > highNum || intChoice < lowNum) {              // choice out of range
            bfChoiceOutOfRange = true;
            printf("Out of range.   ");
        }
    } while (intRetValue != 1 || strlen(cyInput) > 1 ||  // keep choosing if choice is not a number
             bfChoiceOutOfRange || bfChoiceInvalid);                  // or out of range or invalid
    return intChoice;
}

