// rRolette.c -- game of Russion Roulette
// author     -- Geoffrey Jarman
// started    -- 30-Oct-2022
// features
//    uses JSON for outcome and feedback to the HTML page
//    dynamically allocates memory for the size of QUERY_STRING paramater
//    supports actions: clear, load, spin, pull, look
//    "Pull" action causes chamber to move one position before hammer hits chamber
// references:
//    https://www.youtube.com/watch?v=dQyXuFWylm4&t=8s -- How to Parse JSON in C
//    https://github.com/json-c/json-c -- json-c on github
//    https://linuxprograms.wordpress.com/2010/08/19/json_object_new_object/
// log:
//    30-Oct-2022 -- started
//    30-Oct-2022 -- use calloc() to assign memory to the action variable instead of a fixed length array
//    31-Oct-2022 -- simplify the apporoach: track only the live chamber and print JSON in a single block
//    01-Nov-2022 -- add SQL to store the state of each game
//    02-Nov-2022 -- test SQL to retrieve settings from mySQL
//    01-Nov-2022 -- SQL to save changed settings to mySQL
//    01-Nov-2022 -- save the caSound to mySQL
//    03-Nov-2022 -- shorten long lines with continuations
//    03-Nov-2022 -- add exceptions for actions on an unloaded gun
//    07-Nov-2022 -- change sprintf() to asprintf()
//    16-Nov-2022 -- change strcpy() to strncpy()
//    22-Jan-2023 -- set freed pointers to NULL

#define _GNU_SOURCE
#define ERR_MSG_LEN 100

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// global declarations -------------------------------------------------------------------------------------------------

char *sgServer = "192.168.0.13";                                                                //mysqlServer IP address
char *sgUsername = "gjarman";                                                               // mysqlSerer logon username
char *sgPassword = "Mpa4egu$";                                                     // password to connect to mysqlserver
char *sgDatabase = "risingfast";                                                 // default database name on mysqlserver

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
MYSQL_FIELD *fields;

int main(void) {

    int  iChamber = 5;                                          // the chamber containing the bullet. Zero it top center
    int  iRand = 0;                                                                   // a random number between 0 and 5
    int  iCompletedGames = 0;                                                                   // completed game number
    int  iCompletedRounds = 0;                                                                 // completed round number
    bool bLive = false;                                                     // booloean to indicate a live or dead round
    char *sQueryString = NULL;                                   // string to hold the QUERY_STRING environment variable
    char *sAction = NULL;                                 // action taken from the web: clear, load, spin, pull, or look
    char caSound[21] = {'N', 'o', 'n', 'e', '\0'};                           // sound of the gun when an action is taken
    char caErrMsg[ERR_MSG_LEN + 1] = "";                                                // error message in JSON package
    time_t t;                                                                             // time seed for random number
    char caResult[8] = "Success\0";                                 // result of processing an action Success or Failure
    char *strSQL = NULL;

// print the html page content type header and CORS <header> block -----------------------------------------------------

    printf("Content-type: text/html\n");
    printf("Access-Control-Allow-Origin: *\n\n");

// fetch the QUERY_STRING environment variable

    sQueryString = getenv("QUERY_STRING");

// test for a null QUERY_STRING (applicable when submitted from the console) -------------------------------------------

    if (sQueryString == NULL) {
        printf("QUERY_STRING is NULL. Expecting QUERY_STRING=\"Action=<action>\". Ending rRoulette.cgi");
        printf("\n\n");
        
        return EXIT_FAILURE;
    }

// test for a empty QUERY_STRING (applicable when submitted from the console) ------------------------------------------

    if (sQueryString[0] == '\0') {
        printf("QUERY_STRING is empty. Expecting QUERY_STRING=\"Action=<action>\". Ending rRoulette.cgi");
        printf("\n\n");
        
        return EXIT_FAILURE;
    }

// allocate memory to sAction to store the parsed QUERY_STRING text ----------------------------------------------------

    sAction = calloc(strlen(sQueryString), sizeof(char));

// Fetch the 'Action' argument in the QUERY_STRING into sAction --------------------------------------------------------

    sscanf(sQueryString, "Action=%s", sAction);

// test for a valid parameter name in QUERY_STRING ---------------------------------------------------------------------

    if (sAction[0] == '\0') {
        printf("QUERY_STRING is \"%s\". Expecting QUERY_STRING=\"Action=<action>\". Ending rRoulette.cgi", sQueryString);
        printf("\n\n");
        free(sAction);
        sAction = NULL;
        
        return EXIT_FAILURE;
    }

// test for a vaiid action parameter argument in QUERY_STRING ----------------------------------------------------------

    if ((strcmp(sAction, "Load") != 0) && (strcmp(sAction, "Spin") != 0) && (strcmp(sAction, "Pull") != 0) \
     && (strcmp(sAction, "Look") != 0) && (strcmp(sAction, "Clear") != 0)) {
        printf("Invalid action: QUERY_STRING=\"%s\" (not \"Action=Load\" or \"Spin\" or \"Pull\" or \"Look\"  \
                or \"Clear\"). Ending rRoulette.cgi", sQueryString);
        printf("\n\n");
        free(sAction);
        sAction = NULL;
        
        return EXIT_FAILURE;
    }

// * initialize the MySQL client library -------------------------------------------------------------------------------

   if (mysql_library_init(0, NULL, NULL)) {
       printf("Cannot initialize MySQL Client library\n");
       
       return EXIT_FAILURE;
   }

// Initialize a connection and connect to the database -----------------------------------------------------------------

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, sgServer, sgUsername, sgPassword, sgDatabase, 0, NULL, 0))
    {
        printf("\n");
        printf("Failed to connect to MySQL Server %s in module %s()", sgServer, __func__);
        printf("\n\n");
        printf("Error: %s\n", mysql_error(conn));
        printf("\n");

// * close the database connection created by mysql_init(NULL) and free memory allecated to the sAction string ---------

        mysql_close(conn);
        free(sAction);
        sAction = NULL;
        mysql_library_end();

        return EXIT_FAILURE;
    }

// set a SQL query to insert the new author ----------------------------------------------------------------------------

    asprintf(&strSQL, "SELECT `Gun ID`, `Game No`, `Round No`, `Chamber No`, `Live Round`, `Sound`"
                   "FROM risingfast.`Game Russian Roulette` "
                   "WHERE `Gun ID` = 1");

// execute the query and check for no result ---------------------------------------------------------------------------

    if(mysql_query(conn, strSQL) != 0)
    {
        printf("\n");
        printf("mysql_query() error in function %s():\n\n%s", __func__, mysql_error(conn));
        printf("\n\n");
        mysql_close(conn);
        free(sAction);
        sAction = NULL;
        mysql_library_end();
        
        return EXIT_FAILURE;
    }

    free(strSQL);

// store the result of the query ---------------------------------------------------------------------------------------

    res = mysql_store_result(conn);
    if(res == NULL)
    {
        printf("%s() -- no results returned", __func__);
        printf("\n");

        mysql_free_result(res);
        mysql_close(conn);
        free(sAction);
        sAction = NULL;
        mysql_library_end();

        return EXIT_FAILURE;
    }

// fetch the fields in the result --------------------------------------------------------------------------------------

    mysql_data_seek(res, 0);

// copy each value from the SQL table into local variables -------------------------------------------------------------

    row = mysql_fetch_row(res);
    iCompletedGames = atoi(row[1]);
    iCompletedRounds = atoi(row[2]);
    iChamber = atoi(row[3]);
    bLive = atoi(row[4]);
    strncpy(caSound, row[5], 20);

    mysql_free_result(res);

// identify the Action parameter name and value and store the value back in the same local variable 'sAction' ----------

    if (strcmp(sAction, "Load") == 0) {
        iChamber = 1;                                                            // live bullet is loaded into chamber 1
        srand((unsigned) time(&t));                                                   // chambers are spun after loading
        iRand = rand() % 6;
        iChamber = iRand;
        iCompletedRounds = 0;
        if (bLive == true) {                                      // Load action is ignored if the gun is already loaded
            strncpy(caErrMsg, "The gun is already loaded. Try spinning start a new game", ERR_MSG_LEN);
            strncpy(caResult, "Exceptn", 7);
        } else if (bLive == false) {
            bLive = true;
            strncpy(caSound, "Loading ", 20);
            strncpy(caErrMsg, "Gun is loaded in a random chamber with one live bullet", ERR_MSG_LEN);
            strncpy(caResult, "Success", 7);
        }
    } else if (strcmp(sAction, "Spin") == 0) {                         // the state of a bullet is unchanged by spinning
        srand((unsigned) time(&t));
        iRand = rand() % 6;
        iChamber = iRand;
        if (bLive == true) {
            iCompletedGames++;
            iCompletedRounds = 0;
            strncpy(caSound, "Spinning", 20);
            strncpy(caErrMsg, "Gun chamber is spun. This action starts a new game only if the ammo is live", ERR_MSG_LEN);
            strncpy(caResult, "Success", 7);
        } else if (bLive == false) {
            strncpy(caSound, "Spinning", 20);
            strncpy(caErrMsg, "You are spinning a gun with no live ammunition. Please reload", ERR_MSG_LEN);
            strncpy(caResult, "Exceptn", 7);
        }
    } else if (strcmp(sAction, "Pull") == 0) {                                                  // the trigger is pulled
        iChamber = (iChamber + 1) % 6;
        if ((iChamber == 0) && (bLive == true)) {
            bLive = false;
            strncpy(caSound, "Banging ", 20);
            iCompletedGames++;
            iCompletedRounds++;
            strncpy(caErrMsg, "Trigger is pulled. The gun fired. A live bullet was staged in Chamber 5", ERR_MSG_LEN);
            strncpy(caResult, "Success", 7);
        } else if (bLive == false) {
            strncpy(caErrMsg, "The gun is not loaded. Reload to start another game", ERR_MSG_LEN);
            strncpy(caResult, "Exceptn", 7);
        } else {
            strncpy(caSound, "Clicking", 20);
            strncpy(caErrMsg, "Trigger is pulled. The gun did not fire. No live bullet was staged in Chamber 5", ERR_MSG_LEN);
            strncpy(caResult, "Success", 7);
            iCompletedRounds++;
        }
    } else if (strcmp(sAction, "Look") == 0) {                                         // no action is taken with a Look
        strncpy(caSound, "Looking ", 20);
        strncpy(caErrMsg, "Game statistics and gun status reprinted. Nothing is changed", ERR_MSG_LEN);
        strncpy(caResult, "Success", 7);
    } else if (strcmp(sAction, "Clear") == 0) {                                     // clear the gun and game statistics
        strncpy(caSound, "Clearing", 20);
        strncpy(caErrMsg, "Chamber is cleared of ammunition and game statistics zeroed-out", ERR_MSG_LEN);
        strncpy(caResult, "Success", 7);
        iChamber = 0;
        iCompletedGames = 0;
        iCompletedRounds = 0;
        bLive = false;
    } else {
        strncpy(caSound, "Unknown ", 20);
        strncpy(sAction, "Unknown", 7);
        strncpy(caErrMsg, "Unknown action. Nothing is changed. Play again", ERR_MSG_LEN);
        strncpy(caResult, "Failure", 7);
    }

// set a SQL query to save the new game settings -----------------------------------------------------------------------

    asprintf(&strSQL, "UPDATE risingfast.`Game Russian Roulette` SET `Game No` = %d "
                    ", `Round No` = %d "
                    ", `Chamber No` = %d "
                    ", `Live Round` = %d "
                    ", `Sound` = '%s' "
                    ", `Result` = '%s'", iCompletedGames, iCompletedRounds, iChamber, bLive, caSound, caResult);

// execute the update query and check for no result --------------------------------------------------------------------

    if(mysql_query(conn, strSQL) != 0)
    {
        printf("\n");
        printf("mysql_query() error in function %s():\n\n%s", __func__, mysql_error(conn));
        printf("\n\n");
        mysql_close(conn);
        free(sAction);
        sAction = NULL;
        mysql_library_end();
        return EXIT_FAILURE;
    }

// create a json object ------------------------------------------------------------------------------------------------

    json_object *jsonCgiResp = json_object_new_object();

// create json strings and build and print the JSON response ----------- -----------------------------------------------

    json_object *jsonCompletedGameNo = json_object_new_int(iCompletedGames);
    json_object *jsonCompletedRoundNo = json_object_new_int(iCompletedRounds);
    json_object *jsonResult = json_object_new_string(caResult);
    json_object *jsonErrMsg = json_object_new_string(caErrMsg);
    json_object *jsonSound = json_object_new_string(caSound);
    json_object *jsonChamber = json_object_new_int(iChamber);
    json_object *jsonLive = json_object_new_boolean(bLive);

    json_object_object_add(jsonCgiResp, "CompletedGameNo", jsonCompletedGameNo);
    json_object_object_add(jsonCgiResp, "CompletedRoundNo", jsonCompletedRoundNo);
    json_object_object_add(jsonCgiResp, "Result", jsonResult);
    json_object_object_add(jsonCgiResp, "ErrMsg", jsonErrMsg);
    json_object_object_add(jsonCgiResp, "Sound", jsonSound);
    json_object_object_add(jsonCgiResp, "Chamber", jsonChamber);
    json_object_object_add(jsonCgiResp, "Live", jsonLive);

    printf("%s\n", json_object_to_json_string(jsonCgiResp));

// free the memory allocated to the json object ------------------------------------------------------------------------

    json_object_put(jsonCgiResp);
    free(sAction);
    sAction = NULL;

// * close the database connection created by mysql_init(NULL) ---------------------------------------------------------

    mysql_close(conn);

// * free resources used by the MySQL library --------------------------------------------------------------------------

    mysql_library_end();
    free(strSQL);
    strSQL = NULL;

    return EXIT_SUCCESS;
}

