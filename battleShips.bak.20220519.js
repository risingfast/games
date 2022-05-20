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

// Enhancements:

// listener to set latitude and longitude fields required & visible ....................................................

document.getElementById("action").onchange = fChangeListener;

const uri1 = "http://www.risingfast.com/cgi-bin/battleShips.cgi";


function fChangeListener() {
    var LatChoice = document.getElementById("Lat").required;
    var LngChoice = document.getElementById("Lng").required;

    if (document.getElementById("action").value == "Shoot") {
        document.getElementById("Lat").required = true;
        document.getElementById("Lng").required = true;
        document.getElementById("Lat").style.display  = "inline-block";
        document.getElementById("Lng").style.display  = "inline-block";
        document.getElementById("LatLbl").style.display  = "inline-block";
        document.getElementById("LngLbl").style.display  = "inline-block";
        document.getElementById("Lat").value = "";
        document.getElementById("Lng").value = "";
        document.getElementById("idResult").value = "";
        document.getElementById("idSubmit").focus();
    }
    else {
        document.getElementById("Lat").required = false;
        document.getElementById("Lng").required = false;
        document.getElementById("Lat").style.display  = "none";
        document.getElementById("Lng").style.display  = "none";
        document.getElementById("LatLbl").style.display  = "none";
        document.getElementById("LngLbl").style.display  = "none";
        document.getElementById("Lat").value = "";
        document.getElementById("Lng").value = "";
        document.getElementById("idResult").value = "";
        document.getElementById("idSubmit").focus();
    }
}

// functions for action buttons to display and hide help ...............................................................
  
function fShowHelp() {
    var x = document.getElementById("HELPDIV");
    if (x.style.display === "") {
        x.style.display = "block";
    } else {
        x.style.display = "";
    }
}

// function to clear page if 'Clear' button is pressed ..................................................................

function fClearHTML() {
    document.getElementById("action").value = "Shoot";
    document.getElementById("Lat").value = "";
    document.getElementById("Lng").value = "";
    document.getElementById("idResult").value = "";
    document.getElementById("HELPDIV").style.display = "none";
    document.getElementById("Lat").required = true;
    document.getElementById("Lng").required = true;
    document.getElementById("Lat").style.display  = "inline-block";
    document.getElementById("Lng").style.display  = "inline-block";
    document.getElementById("LatLbl").style.display  = "inline-block";
    document.getElementById("LngLbl").style.display  = "inline-block";
}

// function to fetch results from a shoot or other action ...............................................................

async function fFetchResults() {

    var sAction = document.getElementById("action").value;
    var iLat = document.getElementById("Lat").value;
    var iLng = document.getElementById("Lng").value;

    const uri2 = uri1 + '?action=' + sAction + '&Latitude=' + iLat + '&Longitude=' + iLng;
    let response = await fetch(uri2);

    if (response.ok) {
        let text = await response.text();
        document.getElementById("idResult").value= text;
    } else {
        document.getElementById("idResult").value= "CGI Call Failed";
    }
}
