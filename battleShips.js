// battleShips.js.. javascript for battleShips.html
// Author: Geoff Jarman
// Started 10/13/2020
// Description: set the lat and long text boxes required if action of 'Shoot' is chosen.
//     Otherwise hide the lat and long text boxes because the other action choices don't use them.
// Log:
//     13-Oct-2020 start
//     19-Sep-2021 add help functions for help button
//     26-Sep-2021 add fFetchResults() for grid layout
//     27-Sep-2021 add clearing steps to the action changeListener()
//     27-Sep-2021 show lat and long entry in clearHTML()
//     12-Mar-2022 change "none" to "" for x.style.display
//     19-May-2022 add ClearExtras()
//     01-Jun-2022 add cornerimage rotation()
//     18-Jun-2022 move fSetCornerImage() to common.js

// Enhancements:

// listener to set latitude and longitude fields required & visible ....................................................

document.getElementById("action-select").onchange = fChangeListener;

const uri1 = "http://www.risingfast.com/cgi-bin/battleShips.cgi";

function fChangeListener() {
    var LatChoice = document.getElementById("latitude-input").required;
    var LngChoice = document.getElementById("longitude-input").required;

    if (document.getElementById("action-select").value == "Shoot") {
        document.getElementById("latitude-input").required = true;
        document.getElementById("longitude-input").required = true;
        document.getElementById("latitude-input").style.display  = "inline-block";
        document.getElementById("longitude-input").style.display  = "inline-block";
        document.getElementById("latitude-label").style.display  = "inline-block";
        document.getElementById("longitude-label").style.display  = "inline-block";
        document.getElementById("latitude-inpu").value = "";
        document.getElementById("longitude-input").value = "";
        document.getElementById("results-area").value = "";
        document.getElementById("idSubmit").focus();
    }
    else {
        document.getElementById("latitude-input").required = false;
        document.getElementById("longitude-input").required = false;
        document.getElementById("latitude-input").style.display  = "none";
        document.getElementById("longitude-input").style.display  = "none";
        document.getElementById("latitude-label").style.display  = "none";
        document.getElementById("longitude-label").style.display  = "none";
        document.getElementById("latitude-input").value = "";
        document.getElementById("longitude-input").value = "";
        document.getElementById("results-area").value = "";
        document.getElementById("idSubmit").focus();
    }
}

// function to clear page if 'Clear' button is pressed ..................................................................

function fClearHTML() {
    document.getElementById("action-select").value = "Shoot";
    document.getElementById("latitude-input").value = "";
    document.getElementById("longitude-input").value = "";
    document.getElementById("results-area").value = "";
    document.getElementById("HELPDIV").style.display = "none";
    document.getElementById("latitude-input").required = true;
    document.getElementById("longitude-input").required = true;
    document.getElementById("latitude-input").style.display  = "inline-block";
    document.getElementById("longitude-input").style.display  = "inline-block";
    document.getElementById("latitude-label").style.display  = "inline-block";
    document.getElementById("longitude-label").style.display  = "inline-block";

    fcClearExtras();
}

// function to fetch results from a shoot or other action ...............................................................

async function fFetchResults() {

    var sAction = document.getElementById("action-select").value;
    var iLat = document.getElementById("latitude-input").value;
    var iLng = document.getElementById("longitude-input").value;

    const uri2 = uri1 + '?action=' + sAction + '&Latitude=' + iLat + '&Longitude=' + iLng;
    let response = await fetch(uri2);

    if (response.ok) {
        let text = await response.text();
        document.getElementById("results-area").value= text;
    } else {
        document.getElementById("idResult").value= "CGI Call Failed";
    }
}
