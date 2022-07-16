// guessAnIdiom.js -- script match letters to an idiom .................................................................
// for the help division from the stylesheet to inline
// Author: Geoff Jarman
// Started: 17-Oct-2020
// Log:
//    17-Oct-2020 start and test
//    25-Sep-2021 add fFetchWords() and  fClearHTML()
//    25-Sep-2021 add a step to clear results in fClearHTML()
//    26-Sep-2021 remove fClear() call from fFetchWords()
//    03-Oct-2021 rename functions and add a Clear() function
//    12-May-2022 change "none" to "" for x.style.display
//    19-May-2022 change ResultArea to results-area
//    19-May-2022 add fSetCornerImage()
//    18-Jun-2022 move fSetCornerImage() go common.js
//

const uri1 = "http://www.risingfast.com/cgi-bin/guessAnIdiom.cgi";

// function to set document model values to match action chosen by the user .............................................

function fChangeListener() {
    if (document.getElementById("Action").value == "Letter") {
        document.getElementById("Letter").required = true;
        document.getElementById("Idiom").required = false;
        document.getElementById("Letter").style.display = "inline-block";
        document.getElementById("Idiom").style.display = "none";
        document.getElementById("LetterLbl").style.display = "inline-block";
        document.getElementById("IdiomLbl").style.display = "none";
        document.getElementById("Idiom").value = "";
        document.getElementById("Letter").value = "";
        document.getElementById("results-area").value = ""
    }
    else if (document.getElementById("Action").value == "Idiom") {
        document.getElementById("Letter").required = false;
        document.getElementById("Letter").value = false;
        document.getElementById("Letter").style.display = "none";
        document.getElementById("Letter").value = "";
        document.getElementById("Idiom").required = true;
        document.getElementById("Idiom").style.display = "inline";
        document.getElementById("IdiomLbl").style.display = "inline";
        document.getElementById("Idiom").value = "";
        document.getElementById("LetterLbl").style.display = "none";
        document.getElementById("results-area").value = ""
    }
    else if (document.getElementById("Action").value == "Reset") {
        document.getElementById("Idiom").required = false;
        document.getElementById("Letter").required = false;
        document.getElementById("Idiom").style.display = "none";
        document.getElementById("Letter").style.display = "none";
        document.getElementById("LetterLbl").style.display = "none";
        document.getElementById("IdiomLbl").style.display = "none";
        document.getElementById("Letter").value = "";
        document.getElementById("Idiom").value = "";
        document.getElementById("results-area").value = ""
    }
    else {
        document.getElementById("Idiom").required = false;
        document.getElementById("Letter").required = false;
        document.getElementById("Idiom").style.display = "none";
        document.getElementById("Letter").style.display = "none";
        document.getElementById("LetterLbl").style.display = "none";
        document.getElementById("IdiomLbl").style.display = "none";
        document.getElementById("Letter").value = "";
        document.getElementById("Idiom").value = "";
        document.getElementById("results-area").value = ""
    }
}

// function to ajax fetch text2 from the server

async function fGetResults() {
    var sAction = document.getElementById("action").value;
    var sLetter = document.getElementById("Letter").value;
    var sIdiom = document.getElementById("Idiom").value;
    var sGetValue = uri1 + '?' + 'Action=' + sAction + '&Letter=' + sLetter + '&Idiom=' + sIdiom;
    let response = await fetch(sGetValue);
    if (response.ok) {
        let text = await response.text();
        console.log(text.slice(0, 80));
        document.getElementById("results-area").value=text;
    } else {
        alert("HttpError: " + response.status);
    }
}

function fClearPage() {
    document.getElementById("Idiom").required = false;
    document.getElementById("Letter").required = true;
    document.getElementById("Idiom").style.display = "none";
    document.getElementById("LetterLbl").style.display = "inline-block";
    document.getElementById("Letter").style.display = "inline";
    document.getElementById("IdiomLbl").style.display = "none";
    document.getElementById("Letter").value = "";
    document.getElementById("Idiom").value = "";
    document.getElementById("results-area").value = ""
    document.getElementById("HELPDIV").style.display = "none"
}

