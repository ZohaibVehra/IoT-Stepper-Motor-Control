var gateway = `ws://${window.location.hostname}/ws`;  //entry point to websocket interface. window.location.home finds current address, which will be web server ip address
var websocket;    
window.addEventListener('load', onLoad);  //onLoad called when web page loads
function initWebSocket() {    //function to initialize websocket connection to server
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway); //uses the gateway address to initialize
    websocket.onopen    = onOpen;       //self explanatory, onOpen called when websocket connection opened, onClose on connection closed and onMessage when recieving message from server
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; 
}
function onOpen(event) {
    console.log('Connection opened');   //lets us know the connection worked
}
function onClose(event) {
    console.log('Connection closed');       //lets us know connection failed
    setTimeout(initWebSocket, 2000);        //tries to connect again every 2000ms (2sec)
}
function onMessage(event) { //reacts to message sent from server which tells us what motor is doing. Sets text on site accordingly
    var state;
    if(event.data == "0"){
        state = "Stationary";
    }
    else if(event.data == "1"){
        state = "Forwards";
    }
    else{
        state = "Backwards";
    }
    document.getElementById('state').innerHTML = state;
}
function onLoad(event) {  //called when we load in. sets up connection and then sets the functions for our buttons with initButton()
    initWebSocket();
    initButton();
}
function initButton() {   //sets buttons up

    document.getElementsByClassName('forwardButton')[0].addEventListener('mousedown', forwarddown);    //when presses button
    document.getElementsByClassName('forwardButton')[0].addEventListener('pointerdown', forwarddown);    //when presses button
    document.getElementsByClassName('forwardButton')[0].addEventListener('mouseleave', forwardup);   //when mouse dragged out of button
    document.getElementsByClassName('forwardButton')[0].addEventListener('pointerleave', forwardup);   //when mouse dragged out of button
    document.getElementsByClassName('forwardButton')[0].addEventListener('mouseup', forwardup);       //when mouse up over button
    document.getElementsByClassName('forwardButton')[0].addEventListener('pointerup', forwardup);       //when mouse up over button


    document.getElementsByClassName('backwardsButton')[0].addEventListener('mousedown', backwardsdown);    //when presses button
    document.getElementsByClassName('backwardsButton')[0].addEventListener('mouseleave', backwardsup);   //when mouse dragged out of button
    document.getElementsByClassName('backwardsButton')[0].addEventListener('mouseup', backwardsup);       //when mouse up over button
    document.getElementsByClassName('backwardsButton')[0].addEventListener('pointerdown', backwardsdown);    //when presses button
    document.getElementsByClassName('backwardsButton')[0].addEventListener('pointerleave', backwardsup);   //when mouse dragged out of button
    document.getElementsByClassName('backwardsButton')[0].addEventListener('pointerup', backwardsup);       //when mouse up over button

    //document.getElementById('button').addEventListener('click', toggle);
}
function forwarddown(){ 
websocket.send('forwarddown');
}        //functions send message through websocket connection to server

function forwardup(){websocket.send('forwardup');}
function backwardsdown(){websocket.send('backwardsdown');}
function backwardsup(){websocket.send('backwardsup');}
