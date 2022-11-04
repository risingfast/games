// rRoulette.js -- script to fetch JSON Roulette game results from the server and show on a web page
// Author: Geoff Jarman
// Started: 02-Nov-2022
// Log: 
//    02-Nov-2022 -- started

function fClearTemplateFields() {
    fcClearExtras();
}

// functions fetch a simple JSON file from the server ------------------------------------------------------------------

const uri01 = "http://gjarman2020.com/cgi-bin/rRoulette.cgi";

async function fFetchJSON() {
    
    let uri02 = uri01 + "?Action=" + document.getElementById("instruction-select").value
    let response = await fetch(uri02);

    if (response.ok) {
        let jsonText = await response.json();
        let sMessage = document.getElementById("message-input");
        if (jsonText.Status === 'Failure') {
            sMessage.value = jsonText.Status + ': ' + jsonText["Error Message"];
        } else if (jsonText.Result === 'Success') {
            sMessage.value = jsonText.Status;
            document.getElementById("results-area").value = 
                jsonText.CompletedGameNo + '\n' +
                jsonText.CompletedRoundNo + '\n\ +
                jsonText.Result + '\n' +
                jsonText.Sound + '\n' +
                jsonText.Chamber | '\n' + 
                jsonText.Live;
        } else if (jsonText.Status === 'Unknown') {
            sMessage.value = jsonText.Result + ': ' + jsonText["Error Message"];
            document.getElementById("results-area").value = '';
        }
    } else {
        alert("HttpError: " + response.status);
    }
}

function fEnableDisablePlayButton() {
    let sAction = document.getElementById("action-select").value
    let playbutton = document.getElementById("play-button");

// clear any text from the message and results areas

    document.getElementById("results-area").value = '';
    document.getElementById("message-input").value = '';

// enable or disable the fetch button based on a valid or invalid choice of instruction

    if ((sAction === 'Success') || (sAction === 'Failure')) {
        playbutton.removeAttribute('disabled');
    } else {
        playbutton.setAttribute('disabled', 'disabled');
    }
}
