// spaceinvaders.js -- script to implement spaceinvaders.html
// Author: Geoffrey Jarman
// Started: 16-Nov-2023
// Log: 
//    16-Nov-2023 start
//    16-Nov-2023 put global functions in the window scope
// References

const canvas = document.getElementById("game-canvas");
const ctx = canvas.getContext("2d");
import EnemyController from "./EnemyController.js";
import Player from "./Player.js";
import BulletController from "./BulletController.js";

canvas.width = 600;
canvas.height = 600;

const background = new Image();
background.src = "images/space.png";

const playerBulletController = new BulletController(canvas, 10, "red", true);
const enemyController = new EnemyController(canvas);
const player = new Player(canvas, 3, playerBulletController);

function game()
{
    ctx.drawImage(background, 0, 0, canvas.width, canvas.height);
    enemyController.draw(ctx);
    player.draw(ctx);
    playerBulletController.draw(ctx);
}
    
setInterval(game, 1000/60);

window.fClearTemplateFields = function() {
    fcClearExtras();
    let tapAudio = document.querySelector("#tap-audio");
    tapAudio.play();
    document.querySelector("#message-input").value = "";
}

window.fShowMessageReadOnly = function() {
    let buzzerAudio = document.querySelector("#buzzer-audio");
    buzzerAudio.play();
    document.querySelector("#message-input").value = "This field is read only";
}

window.fShowMessageForCanvasClick = function() {
    document.querySelector("#message-input").value = "Clicking here does nothing. Use the spacebar to shoot, arrow keys to move";
    let nononoAudio = document.querySelector("#nonono-audio");
    nononoAudio.play();
}

