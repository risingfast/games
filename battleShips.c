//  battleShips.c - CGI for the battleShips game
//  Author: Geoffrey Jarman
//  Started: 03/26/2020 Started
//  Functions:
//    unencode()  -- decode special characters in the input string
//    makeWaves() -- populate wave characters on the ocean chart
//    placeShip() -- place a single ship (B, C or D type) on the chart
//    showFleet() -- show positions of ships placed on the map
//    showFleetWeb() -- show positions of shipls placed on the map (web page version)
//  Log:
//      29-Jul-2020 make the code run under command line and web automatically
//      30-Jul-2020 add a console menu back into the code
//      30-JUl-2020 change the DATAFILE to a unique name that won't conflict with other files
//      02-Aug-2020 finalize running as a console app
//      02-Aug-2020 create a CGI and publish to the web
//      03-Aug-2020 get working as a web application
//      04-Aug-2020 flag shooting in progress in the webApp and save fleet status during shoot
//      04-Aug-2020 add Remaining Target Count and Remaining Shell Count to web version
//      05-Aug-2020 add logic to end game when shells or targets are used
//      05-Aug-2020 confirm shooting statistics calculate correctly on the console version
//      05-Aug-2020 add an aircraft carrier to the layout
//      05-Aug-2020 add shoot statistics to the Show option
//      05-Aug-2020 add a legend to the show option for B, C, D and A
//      06-Aug-2020 add a console option to show targets hit so far
//      06-Aug-2020 add the Internals button to each action page
//      06-Aug-2020 create multiple (4) fields of action for console and web
//      07-Aug-2020 clean up code layout
//      11-Aug-2020 reformatted comment lines
//      14-Aug-2020 added meta tags to html in <head> block
//      07-Sep-2020 add menu bar links
//      11-Oct-2020 center menu bar links in main heading
//      24-Oct-2020 center the body of the html on the webpage
//      04-Jan-2021 webcode
//      26-Mar-2021 reviewed all
//      26-Sep-2021 remove getenv("CONTENT_LENGTH") as a test for web vs console
//      26-Sep-2021 migrate to webcode for grid layout by removing HTML formatting
//      27-Sep-2021 remove remaining HTML formatting for grid layout
//  Enhancements:
//

// includes and defines

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "d_getInput.h"
#include <time.h>

#define MAXLEN 80                      // maximum length of web content from HTML page
#define MAXSZE MAXLEN + 1              // size of array to store web input including \'0'
#define DATAFILE "battleShips.txt"     // filename
#define SHEL_CNT 40                    // count of shells in the magazine

// external variable declarations

void placeShip(char [][10], int, int, int, int);
void placeFleet(char [][10]);
void placeFleetWeb(char [][10]);
void makeWaves(char [][10]);
void showFleet(char [][10], int);
void showFleetWeb(char [][10], int);
void readFleet(char [][10], int);
void writeFleet(char [][10], int);

// external flag declarations

bool bfConsoleApp = false;             // flag for console or web app

// main function

int main(void) {

// automatic variable declarations

    char *lenstr;
    char input[MAXSZE];                // the StdIn from HTML form post
    char cyChrt[10][10];               // the battlefield
    char *sWebStdInLen;                // length of content in stdin from HTML form post
    int  iWebStdInLen = 0;             // sWebStdInLen as an integer
    int  nRespLen = 0;                 // length of console menu choice input
    char cyRespVal[3];                 // user console menu choice
    char cyRespLat[3];                 // shooter's latitude (0 - 9)
    int  nRespLat = 0;                 // integer version of cyRespLat[]
    char cyRespLng[3];                 // shooter's longitude (0 - 9)
    int  nRespLng = 0;                 // integer version of cyRespLng[]
    int  nRespVal = 0;                 // cyRespVal[] as an integer
    int  nRespValWeb = 0;              // web menu choice
    int  iTrgts = 0;                   // count of remaining targets
    int  iShots = 0;                   // count of shots fired
    int  iStrks = 0;                   // count of strikes made
    int  iLat = 0;                     // latitude (y axis) of a shoot
    int  iLng = 0;                     // longitude (x axis) of a shoot
    int  iShells = SHEL_CNT;           // count of shells
    int  iWebLat = 0;                  // web input Latiude from user
    int  iWebLng = 0;                  // web input Longitude from user

// automatic flag declarations

    bool bExitMain = false;            // flag controlling console main input loop
    bool bExitLat = false;             // flag controlling console latitude input loop
    bool bExitLng = false;             // flag controlling console longitude input loop
    bool bGameInProgress = false;      // flat controlling game initialization for web

// check if program is being called from a webform or the console

    if (getenv("QUERY_STRING") == NULL) {
        bfConsoleApp = true;           // NULL pointer means no QUERY_STRING found
    }                                  // so run as console app
    else {
        iWebStdInLen = strlen(getenv("QUERY_STRING"));
        bfConsoleApp = false; 
    }

// run as a console app (console)

    if (bfConsoleApp == true) {
        
        placeFleet(cyChrt);
        writeFleet(cyChrt, 10);

// get a menu choice (console)

        while (bExitMain == false) {
            while (nRespLen == 0) {
                printf("\nChoose Shoot (1), Show (2), List (3), Reset (4), Options (5) or Quit (6): ");
                nRespLen = get_string("", cyRespVal, 3);
                if (nRespLen == 0) {
                    printf("\nInvalid response - not a single digit");
                    printf("\n");
                }
                else if (nRespLen == 1) {
                    printf("\nYou responded: %s", cyRespVal);
                    printf("\n");
                    printf("\n");
                    nRespVal = atoi(cyRespVal);
                    if (nRespVal > 6 || nRespVal < 1) {
                        nRespLen = 0;
                        printf("\nInvalid response - not a listed choice");
                    printf("\n");
                    } 
                }
            }
            nRespLen = 0;              // reset nRespLen for future reuse

// option 1 shoot (console)

            if (nRespVal == 1) {

// count remaining unsunk targets before shooting (console)

                iTrgts = 0;
                iShots = 0;
                iShells = 0;

                for (int iLat = 0; iLat < 10; iLat++) {
                    for (int iLng = 0; iLng < 10; iLng++) {
                        if ((cyChrt[iLat][iLng] != '^') && (cyChrt[iLat][iLng] != '*')) {
                            iTrgts++;
                        }
                        if (cyChrt[iLat][iLng] == '*') {
                            iShots++;
                        }
                    }
                }

                iShells = SHEL_CNT - iShots;

                printf("Remaining Target Count: %d", iTrgts);
                printf("\n");
                printf("Remaining Shell Count: %d", iShells);
                printf("\n");
                printf("Shots fired: %d", iShots);

// shoot at targets from input (console)

                if (iTrgts > 0) {
                    if (iShells > 0) {
                        printf("\n");
                        printf("\nStart shooting");
                        printf("\n");
                    }
                }
                else if (iTrgts == 0) {
                    printf("\nNo targets remaining. Shooting halted");
                }
                else if (iShells == 0) {
                    printf("\nNo shells remaining. Shooting halted");
                }

// get latitude of shot (console)

                if (iTrgts > 0 && iShells > 0) {
                    bExitLat = false;
                }
                else {
                    bExitLat = true;
                }

                while (bExitLat == false) {
                    nRespLen = get_string("\nLatitude (y) (0-9):", cyRespLat, 3);
                    if (nRespLen == 0) {
                        printf("\nInvalid response - not a single digit");
                        printf("\n");
                    }
                    else if (nRespLen == 1) {
                        if ((strchr("0123456789", cyRespLat[0])) == NULL) {
                            printf("\nInvalid latitude (use 0 -> 9)");
                            printf("\n");
                        }
                        else {
                            printf("\nYou targeted latitude: %s", cyRespLat);
                            printf("\n");
                            nRespLat = atoi(cyRespLat);
                            if (nRespLat > 9 || nRespLat < 0) {
                                nRespLen = 0;
                                printf("\nInvalid latitude - use 0 -> 9");
                                printf("\n");
                            }
                            else {
                                bExitLat = true;
                            }
                        }
                    }
                    nRespLen = 0;
                }

// get longitude of shot (console)

                if (iTrgts > 0 && iShells > 0) {
                    bExitLng = false;
                }
                else {
                    bExitLng = true;
                }

                while (bExitLng == false) {
                    nRespLen = get_string("\nLongitude (x) (0-9):", cyRespLng, 3);
                    if (nRespLen == 0) {
                        printf("\nInvalid response - not a single digit");
                        printf("\n");
                    }
                    else if (nRespLen == 1) {
                        if ((strchr("0123456789", cyRespLng[0])) == NULL) {
                            printf("\nInvalid longitude (use 0 -> 9)");
                            printf("\n");
                        }
                        else {
                            printf("\nYou targeted longitude: %s", cyRespLng);
                            nRespLng = atoi(cyRespLng);
                            if (nRespLng > 9 || nRespLng < 0) {
                                nRespLen = 0;
                                printf("\nInvalid longitude - use 0 -> 9");
                            }
                            else {
                                bExitLng = true;
                            }
                        }
                    }
                    nRespLen = 0;
                }

// show targeting and shooting confirmation (console)

                if (iTrgts > 0 && iShells > 0) {
                    printf("\n");
                    printf("\nShooting latitude %d, longitude %d", nRespLat, nRespLng);
                }

// shoot the target at the entered latitude and longitude (console)

                if (iTrgts > 0 && iShells > 0) {
                    printf("\n");
                    printf("\nStriking %c", cyChrt[nRespLat][nRespLng]); 
                    printf("\n");
                    cyChrt[nRespLat][nRespLng] = '*';
                    iShells--;

// count remaining unsunk targets after shooting (console)

                    iTrgts = 0;
                    iShots = 0;
                    iShells = 0;

                    for (int iLat = 0; iLat < 10; iLat++) {
                        for (int iLng = 0; iLng < 10; iLng++) {
                            if ((cyChrt[iLat][iLng] != '^') && (cyChrt[iLat][iLng] != '*')) {
                                iTrgts++;
                            }
                            if (cyChrt[iLat][iLng] == '*') {
                                iShots++;
                            }
                        }
                    }

                    iShells = SHEL_CNT - iShots;

                    printf("\nRemaining Target Count: %d", iTrgts);
                    printf("\n");
                    printf("Remaining Shell Count: %d", iShells);
                    printf("\n");
                    printf("Shots fired: %d", iShots);
                    printf("\n");
                }
                else {
                    printf("\n");
                    printf("\nTrgts: %d, Shells: %d, Shooting stopped", iTrgts, iShells); 
                    printf("\n");
                }
            }

// option 2 show the fleet (console)

            else if (nRespVal == 2) {
                showFleet(cyChrt, 10);
            }

// option 3 list targets hit so far in game (console)

            else if (nRespVal == 3) {
                iStrks = 0;
                printf("Strikes counted:");
                printf("\n");
                for (int i = 0; i < 10; i++) {
                    for (int j = 0; j < 10; j++) {
                        if (cyChrt[i][j] == '*') {
                            printf("\nStrike at Lat: %d, Lng: %d", i, j);
                            iStrks++;
                        }
                    }
                }
                if (iStrks == 0) {
                    printf("\nNo strikes made so far ...");
                    printf("\n");
                }
                else {
                    printf("\n");
                }
            }

// option 4 reset fleet positions to new game (console)--------------------------------------------

            else if (nRespVal == 4) {
                placeFleet(cyChrt);
                printf("\nGame reset");
                printf("\n");
                iShells = SHEL_CNT;
            }

// option 5 show configuration options (console)

            else if (nRespVal == 5) {
                printf("No Options defined");
                printf("\n");
            }

// option 6 quit (console)

            else if (nRespVal == 6) {
            printf("Quitting action. Fleet reset.");
            printf("\n");
            printf("\n");
                break;
            }
        }
    }

// or run as a web app (web)

    else {

        bGameInProgress = false;

        FILE *fBattleShips = fopen("battleShips.dat", "r");

        if (fBattleShips != NULL) {
            readFleet(cyChrt, 10);
            for (int i = 0; i < 100; i++) {
                if (fgetc(fBattleShips) == '*') {
                    bGameInProgress = true;
                    break;
                }
            }
        }

        fclose(fBattleShips);

        if (bGameInProgress == false) {
            makeWaves(cyChrt);
            placeFleetWeb(cyChrt);
            writeFleet(cyChrt, 10);
            readFleet(cyChrt, 10);
        }

// print the hTML content type and header

        printf("Content-type: text/html\n\n");

// identify the action submitted (web)

        strcpy(input, getenv("QUERY_STRING"));

        if (strstr(input, "Shoot") != NULL) {
            nRespValWeb = 1;
            iWebLat = atoi(strstr(input, "Latitude=") + strlen("Latitude="));
            iWebLng = atoi(strstr(input, "Longitude=") + strlen("Longitude="));
        }
        else if (strstr(input, "Show") != NULL) {
            nRespValWeb = 2;
        }
        else if (strstr(input, "List") != NULL) {
            nRespValWeb = 3;
        }
        else if (strstr(input, "Reset") != NULL) {
            nRespValWeb = 4;
        }
        else if (strstr(input, "Options") != NULL) {
            nRespValWeb = 5;
        }

// count remaining unsunk targets before shooting (web)

                iTrgts = 0;
                iShots = 0;
                iShells = 0;

                for (int iLat = 0; iLat < 10; iLat++) {
                    for (int iLng = 0; iLng < 10; iLng++) {
                        if ((cyChrt[iLat][iLng] != '^') && (cyChrt[iLat][iLng] != '*')) {
                            iTrgts++;
                        }
                        if (cyChrt[iLat][iLng] == '*') {
                            iShots++;
                        }
                    }
                }
                iShells = SHEL_CNT - iShots;

// web option 1 shoot (web)

        if (nRespValWeb == 1) {
            if (iTrgts == 0) {
                printf("No targets remaining. Shot aborted");
            }
            else if (iShells == 0) {
                printf("No shells remaining. Shot aborted");
            }
            else if (cyChrt[iWebLat][iWebLng] == '*') {
                printf("Latitude: %d, Longitude: %d previously targeted. Shot aborted", iWebLat, iWebLng);
            }
            else {
                printf("Shooting latitude %d, longitude %d", iWebLat, iWebLng);
                printf("\nStriking: %c", cyChrt[iWebLat][iWebLng]);
                printf("\n");
                readFleet(cyChrt, 10);
                cyChrt[iWebLat][iWebLng] = '*';
            }

// count remaining unsunk targets after shooting (web)

                iTrgts = 0;
                iShots = 0;
                iShells = 0;

                for (int iLat = 0; iLat < 10; iLat++) {
                    for (int iLng = 0; iLng < 10; iLng++) {
                        if ((cyChrt[iLat][iLng] != '^') && (cyChrt[iLat][iLng] != '*')) {
                            iTrgts++;
                        }
                        if (cyChrt[iLat][iLng] == '*') {
                            iShots++;
                        }
                    }
                }
                iShells = SHEL_CNT - iShots;

// show firing statistics after shooting (web)

            bGameInProgress = true;
            writeFleet(cyChrt, 10);
            printf("\nRemaining Targets: %d", iTrgts);
            printf("\n");
            printf("Shots fired: %d", iShots);
            printf("\n");
            printf("Remaining Shells: %d", iShells);
        }

// option 2 show battlefield (web)

        if (nRespValWeb == 2) {
            readFleet(cyChrt, 10);
            showFleetWeb(cyChrt, 10);
        }

// option 3 list targets hit so far in game (web)

        if (nRespValWeb == 3) {
            iStrks = 0;
            printf("Strikes counted:");
            printf("\n");
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    if (cyChrt[i][j] == '*') {
                        printf("\nStrike at Lat: %d, Lng: %d", i, j);
                        iStrks++;
                    }
                }
            }
            if (iStrks == 0) {
                printf("\nNo strikes made so far ...");
                printf("\n");
            }
            else {
                printf("\n");
            }
        }

// option 4 reset battlefield (web)

        if (nRespValWeb == 4) {
            makeWaves(cyChrt);
            placeFleetWeb(cyChrt);
            writeFleet(cyChrt, 10);
            bGameInProgress = true;
            iShells = SHEL_CNT;
            printf("Battleships reset");
        }

// option 5 show options (web)

        if (nRespValWeb == 5) {
            printf("No options available");
        }
    }

    return 0;
}

// function makeWaves() -- populate wave characters on the ocean chart

void makeWaves(char cyChrt[][10]) {
    int x, y;
    for (x = 0; x < 10; x++) {
       for (y = 0; y < 10; y++) {
           cyChrt[x][y] = '^';
       }
    }
}

// function placeShip() -- place a single ship (B, C or D type) on the chart

void placeShip(char chart[][10],       // place a ship on the chart
               int size,
               int length,
               int latitude,
               int longitude) {
    int i = 0;
    int j = 0;
    int k = 0;
    char shipChar;

    switch (length) {                  // set the ship character for the map
        case 2:
            shipChar = 'D';
            break;
        case 3:
            shipChar = 'C';
            break;
        case 4:
            shipChar = 'B';
            break;
        case 5:
            shipChar = 'A';
        }

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (i == latitude && j == longitude) {
                for (k = 0; k < length; k++) {
                    chart[i][j+k] = shipChar;
                }
            }
        }
    }
}

// function showFleet() -- show positions of ships on the map (console)

void showFleet(char cyChrt[][10], int size) {

    int i = 0;
    int j = 0;
    int x = 0;

    printf("Showing the fleet (A Aircraft Carrier, B Battleship, C Cruiser, D Destoyer)... \n\n");
    printf("  ");
    for (x = 0; x < 10; x++) {
        printf("%d ", x);
    }
    printf("\n");
    for (i = 0; i < size; i++) {
        printf("%d ", i);
        for (j = 0; j < size; j++) {
            printf("%c ", cyChrt[i][j]);
        }
        printf("\n");
    }

// count remaining unsunk targets before shooting (console)

    int iTrgts = 0;
    int iShots = 0;
    int iShells = 0;

    for (int iLat = 0; iLat < 10; iLat++) {
        for (int iLng = 0; iLng < 10; iLng++) {
            if ((cyChrt[iLat][iLng] != '^') && (cyChrt[iLat][iLng] != '*')) {
                iTrgts++;
            }
            if (cyChrt[iLat][iLng] == '*') {
                iShots++;
            }
        }
    }

    iShells = SHEL_CNT - iShots;

    printf("\n");
    printf("Remaining Target Count: %d", iTrgts);
    printf("\n");
    printf("Remaining Shell Count: %d", iShells);
    printf("\n");
    printf("Shots fired: %d", iShots);
    printf("\n");

}

// function showFleetWeb() -- show positions of ships placed on the map (web)

void showFleetWeb(char cyChrt[][10], int size) {

    int i = 0;
    int j = 0;
    int x = 0;

    printf("The fleet ...");
    printf("\n\n");
    printf("  ");
    for (x = 0; x < 10; x++) {
        printf("%d ", x);
    }
    printf("\n");
    for (i = 0; i < size; i++) {
        printf("%d ", i);
        for (j = 0; j < size; j++) {
            printf("%c ", cyChrt[i][j]);
        }
        printf("\n");
    }
}

// function placeFleet() -- place the fleet for a new game (console)

void placeFleet(char cyChrt[][10]) {

    int iField = 0;

    (void) system("clear");
    printf("\nbattleShips.c - battleShips game in text mode (as console app)");
    printf("\n");
    makeWaves(cyChrt);

    srand(time(NULL));
    iField = rand() % 4;

    if (iField == 0) {
        placeShip(cyChrt, 10, 5, 5, 4);          // aircraft carrier
        placeShip(cyChrt, 10, 4, 2, 1);          // battleship
        placeShip(cyChrt, 10, 4, 4, 3);          // battleship
        placeShip(cyChrt, 10, 3, 7, 6);          // cruiser
        placeShip(cyChrt, 10, 3, 8, 0);          // cruiser
        placeShip(cyChrt, 10, 2, 6, 5);          // destroyer
        placeShip(cyChrt, 10, 2, 9, 2);          // destroyer
        placeShip(cyChrt, 10, 2, 0, 8);          // destroyer
    }
    else if (iField == 1) {
        placeShip(cyChrt, 10, 5, 1, 1);          // aircraft carrier
        placeShip(cyChrt, 10, 4, 6, 6);          // battleship
        placeShip(cyChrt, 10, 4, 8, 0);          // battleship
        placeShip(cyChrt, 10, 3, 2, 7);          // cruiser
        placeShip(cyChrt, 10, 3, 8, 7);          // cruiser
        placeShip(cyChrt, 10, 2, 3, 1);          // destroyer
        placeShip(cyChrt, 10, 2, 4, 5);          // destroyer
        placeShip(cyChrt, 10, 2, 6, 1);          // destroyer
    }
    else if (iField == 2) {
        placeShip(cyChrt, 10, 5, 4, 1);          // aircraft carrier
        placeShip(cyChrt, 10, 4, 0, 6);          // battleship
        placeShip(cyChrt, 10, 4, 7, 6);          // battleship
        placeShip(cyChrt, 10, 3, 3, 5);          // cruiser
        placeShip(cyChrt, 10, 3, 5, 5);          // cruiser
        placeShip(cyChrt, 10, 2, 1, 1);          // destroyer
        placeShip(cyChrt, 10, 2, 7, 1);          // destroyer
        placeShip(cyChrt, 10, 2, 8, 5);          // destroyer
    }
    else if (iField == 3) {
        placeShip(cyChrt, 10, 5, 2, 1);          // aircraft carrier
        placeShip(cyChrt, 10, 4, 1, 0);          // battleship
        placeShip(cyChrt, 10, 4, 7, 1);          // battleship
        placeShip(cyChrt, 10, 3, 5, 0);          // cruiser
        placeShip(cyChrt, 10, 3, 5, 6);          // cruiser
        placeShip(cyChrt, 10, 2, 1, 7);          // destroyer
        placeShip(cyChrt, 10, 2, 3, 7);          // destroyer
        placeShip(cyChrt, 10, 2, 9, 1);          // destroyer
    }
}

// function placeFleetWeb() -- place the fleet for a new game (web)

void placeFleetWeb(char cyChrt[][10]) {

    int iField = 0;

    makeWaves(cyChrt);
    
    srand(time(NULL));
    iField = rand() % 4;

    if (iField == 0) {
        placeShip(cyChrt, 10, 5, 5, 4);          // aircraft carrier
        placeShip(cyChrt, 10, 4, 4, 3);          // battleship
        placeShip(cyChrt, 10, 4, 2, 1);          // battleship
        placeShip(cyChrt, 10, 3, 8, 0);          // cruiser
        placeShip(cyChrt, 10, 3, 7, 6);          // cruiser
        placeShip(cyChrt, 10, 2, 0, 8);          // destroyer
        placeShip(cyChrt, 10, 2, 6, 5);          // destroyer
        placeShip(cyChrt, 10, 2, 9, 2);          // destroyer
    }
    else if (iField == 1) {
        placeShip(cyChrt, 10, 5, 1, 1);          // aircraft carrier
        placeShip(cyChrt, 10, 4, 6, 6);          // battleship
        placeShip(cyChrt, 10, 4, 8, 0);          // battleship
        placeShip(cyChrt, 10, 3, 2, 7);          // cruiser
        placeShip(cyChrt, 10, 3, 8, 7);          // cruiser
        placeShip(cyChrt, 10, 2, 3, 1);          // destroyer
        placeShip(cyChrt, 10, 2, 4, 5);          // destroyer
        placeShip(cyChrt, 10, 2, 6, 1);          // destroyer
    }
    else if (iField == 2) {
        placeShip(cyChrt, 10, 5, 4, 1);          // aircraft carrier
        placeShip(cyChrt, 10, 4, 0, 6);          // battleship
        placeShip(cyChrt, 10, 4, 7, 6);          // battleship
        placeShip(cyChrt, 10, 3, 3, 5);          // cruiser
        placeShip(cyChrt, 10, 3, 5, 5);          // cruiser
        placeShip(cyChrt, 10, 2, 1, 1);          // destroyer
        placeShip(cyChrt, 10, 2, 7, 1);          // destroyer
        placeShip(cyChrt, 10, 2, 8, 5);          // destroyer
    }
    else if (iField == 3) {
        placeShip(cyChrt, 10, 5, 2, 1);          // aircraft carrier
        placeShip(cyChrt, 10, 4, 1, 0);          // battleship
        placeShip(cyChrt, 10, 4, 7, 1);          // battleship
        placeShip(cyChrt, 10, 3, 5, 0);          // cruiser
        placeShip(cyChrt, 10, 3, 5, 6);          // cruiser
        placeShip(cyChrt, 10, 2, 1, 7);          // destroyer
        placeShip(cyChrt, 10, 2, 3, 7);          // destroyer
        placeShip(cyChrt, 10, 2, 9, 1);          // destroyer
    }
}

// function writeFleet() - save the fleet positions to a file

void writeFleet(char cyChrt[][10], int iSize) {

    FILE *fBattleShips = fopen("battleShips.dat", "w");

    if (fBattleShips == NULL) {
        printf("\nCannot open BattleShips.dat to save or read the fleet positions");
    }

    for (int iLat = 0; iLat < iSize; iLat++) {
        for (int iLng = 0; iLng < iSize; iLng++) {
            fputc(cyChrt[iLat][iLng], fBattleShips);
        }
    }

    fclose(fBattleShips);
}

// function readFleet() - read fleet positions from a saved file

void readFleet(char cyChrt[][10], int iSize) {

    FILE *fBattleShips = fopen("battleShips.dat", "r");

    if (fBattleShips == NULL) {
        printf("\nCannot open BattleShips.dat to save or read the fleet positions");
    }

    for (int iLat = 0; iLat < iSize; iLat++) {
        for (int iLng = 0; iLng < iSize; iLng++) {
            cyChrt[iLat][iLng] = fgetc( fBattleShips);
        }
    }
    fclose(fBattleShips);
}

