// d_regexwords.js -- script to fetch initial display attribute values in d_regexWords.html........
// for the help division from the stylesheet to inline
// Author: Geoff Jarman
// Started: 17-Oct-2020
// Log: 
//    17-Oct-2020 start and test
//    25-Sep-2021 add fFetchWords() and  fClearHTML()
//    25-Sep-2021 add a step to clear results in fClearHTML()
//    26-Sep-2021 remove fClear() call from fFetchWords()
  
const uri1 = "http://www.risingfast.com/cgi-bin/d_regexWords.cgi";

// functions for action buttons to display and hide help ..........................................
  
function fShowHelp() {
    var x = document.getElementById("helpDiv");
    const stx = getComputedStyle(x);
    x.style.display = stx.display;
    if (x.style.display === "none") {
        x.style.display = "block";
    } else {
        x.style.display = "none";
    }
}

function fClearHTML() {
    document.getElementById("rl").value = "";
    document.getElementById("wp").value = "";
    document.getElementById("idResult").value = "";
    document.getElementById("sa").value = "Yes";
    document.getElementById("helpDiv").style.display = "none";
}

async function fFetchWords() {

    var sPattern = document.getElementById("wp").value;
    var sLimit = document.getElementById("rl").value;
    var sHideApos = document.getElementById("sa").value;

    const uri2 = uri1 + '?wp=' + sPattern + '&rl=' + sLimit + '&sa=' + sHideApos;
    let response = await fetch(uri2);

    if (response.ok) {
        let text = await response.text();
        document.getElementById("idResult").value= text;
    } else {
        document.getElementById("idResult").value= "CGI Call Failed";
    }
}
