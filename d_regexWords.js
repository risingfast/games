// d_regexwords.js -- script to fetch initial display attribute values in d_regexWords.html........
// for the help division from the stylesheet to inline
// Author: Geoff Jarman
// Started: 17-Oct-2020
// Log: 
//    17-Oct-2020 start and test
//    25-Sep-2021 add fFetchWords() and  fClearHTML()
//    25-Sep-2021 add a step to clear results in fClearHTML()
//    26-Sep-2021 remove fClear() call from fFetchWords()
//    11-May-2022 capitalize helpDiv
//    12-May-2022 change "none" to x.style.display
//    19-May-2022 change idResult to results-area

  
const uri1 = "http://www.risingfast.com/cgi-bin/d_regexWords.cgi";
const uri2 = "http://www.risingfast.com/cgi-bin/setCornerImage.cgi";

// functions for action buttons to display and hide help ..........................................
  
function fShowHelp() {
    var x = document.getElementById("HELPDIV");
    if (x.style.display === "") {
        x.style.display = "block";
    } else {
        x.style.display = "";
    }
}

function fClearHTML() {
    document.getElementById("rl").value = "";
    document.getElementById("wordpattern-input").value = "";
    document.getElementById("idResult").value = "";
    document.getElementById("sa").value = "Yes";
    document.getElementById("HELPDIV").style.display = "none";
}

async function fFetchWords() {

    var sPattern = document.getElementById("wordpattern-input").value;
    var sLimit = document.getElementById("rl").value;
    var sHideApos = document.getElementById("sa").value;

    const uri2 = uri1 + '?wp=' + sPattern + '&rl=' + sLimit + '&sa=' + sHideApos;
    let response = await fetch(uri2);

    if (response.ok) {
        let text = await response.text();
        document.getElementById("results-area").value= text;
    } else {
        document.getElementById("results-area").value= "CGI Call Failed";
    }
}

// function to ajax fetch the current corner image and captiona

async function fSetCornerImage() {
    let response = await fetch(uri2);
    if (response.ok) {
        let text = await response.text();
        let array = text.split("\n");
        array.pop();                      // remove the last element (empty element) created by the split("\n")
        let intRecords = array.length/3;
        let intRecordSelected = Math.trunc(Math.random() * intRecords);
        document.getElementById("ASIDE2IMG").src=array[intRecordSelected * 3]
        document.getElementById("ASIDE3-PARA").innerHTML=array[(intRecordSelected * 3) + 1];
    } else {
        alert("HttpError: " + response.status);
    }
}

