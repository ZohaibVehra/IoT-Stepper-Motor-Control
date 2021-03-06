#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define A 16
#define B 17
#define C 18
#define D 19
#define DelayN 2    //delay in ms for motor steps
#define NUMBER_OF_STEPS_PER_REV 512

const char* ssid = "_"; //your ssid here
const char* password = "_"; //your ssid password here


bool forwardRotate = 0;
bool backwardsRotate = 0;
const int GPIOMotorPin1 = 16;
const int GPIOMotorPin2 = 17;
const int GPIOMotorPin3 = 18;
const int GPIOMotorPin4 = 19;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <style>

h1{ 
    color: black;
    text-align: center;
}

h2{ 
    color: black;
    text-align: center;
}

.buttonContainer{
    padding-top: 50px;
    background-color: white;
    display: flex;
    flex-direction: column;
}

.forwardButton{
    height: 200px;
    width: 300px;
    margin: auto;
    margin-bottom: 30px;
}
.backwardsButton{
    height: 200px;
    width: 300px;
    margin: auto;
}
    </style>


    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Forward Backwards First Attempt</title>
    <!---- <link rel="stylesheet" href="./styles.css"> -->
</head>
<body>
    <div>
        <h1> Forward Backwards for Motor Connected to ESP32 </h1><br>
        <h2 class="state"> <span id="state">%STATE%</span> </h2>
    </div>


    <div  class="buttonContainer">
        <button  class="forwardButton"> Forward </button>
        <button class="backwardsButton"> Backwards </button>
    </div>

    
    

    <script>

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


    </script>
   
</body>
</html>
)rawliteral";


void notifyClients() {
    int x = 5;
    if(forwardRotate == 0 && backwardsRotate == 0){
        x = 0;
    }
    else if(forwardRotate == 1 && backwardsRotate == 1){
        x = 0;
    }
    else if(forwardRotate == 1 && backwardsRotate == 0){
        x = 1;
    }
    else if(forwardRotate == 0 && backwardsRotate == 1){
        x = 2;
    }
  ws.textAll(String(x));
}

  void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {   //takes in message
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;

    if (strcmp((char*)data, "forwarddown") == 0 && forwardRotate != 1) {     //if recieve forwarddown, and are not already moving forward, do so
      forwardRotate = 1;
    }
    if (strcmp((char*)data, "forwardup") == 0 && forwardRotate != 0) {     //if recieve forwardup, and forwardRotate isnt already false set it to false
      forwardRotate = 0;
    }
    if (strcmp((char*)data, "backwardsdown") == 0 && backwardsRotate != 1) {     
      backwardsRotate = 1;
    }
    if (strcmp((char*)data, "backwardsup") == 0 && backwardsRotate != 0) {     
      backwardsRotate = 0;
    }
  }
  notifyClients();
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {  //handles event occurances
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {  //initializes websocket protocol
  ws.onEvent(onEvent);  //if theres an event we call onEvent function
  server.addHandler(&ws);
}

String processor(const String& var){    //double check this shit
  Serial.println(var);

  if(var == "STATE"){
    if(forwardRotate == 0 && backwardsRotate == 0){
        return "Stationary1";
    }
    else if(forwardRotate == 1 && backwardsRotate == 1){
        return "Stationary1";
    }
    else if(forwardRotate == 1 && backwardsRotate == 0){
        return "Forwards1";
    }
    else if(forwardRotate == 0 && backwardsRotate == 1){
        return "Backwards1";
    }
  }
}

void write(int a,int b,int c,int d){    //for conviniently moving motor
digitalWrite(A,a);
digitalWrite(B,b);
digitalWrite(C,c);
digitalWrite(D,d);
}
void onestep(){ //for conviniently moving motor
write(1,0,0,0);
delay(DelayN);
write(1,1,0,0);
delay(DelayN);
write(0,1,0,0);
delay(DelayN);
write(0,1,1,0);
delay(DelayN);
write(0,0,1,0);
delay(DelayN);
write(0,0,1,1);
delay(DelayN);
write(0,0,0,1);
delay(DelayN);
write(1,0,0,1);
delay(DelayN);
}
void bonestep(){    //other direction
write(1,0,0,1);
delay(DelayN);
write(0,0,0,1);
delay(DelayN);
write(0,0,1,1);
delay(DelayN);
write(0,0,1,0);
delay(DelayN);
write(0,1,1,0);
delay(DelayN);
write(0,1,0,0);
delay(DelayN);
write(1,1,0,0);
delay(DelayN);
write(1,0,0,0);
delay(DelayN);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(GPIOMotorPin1, OUTPUT);
  pinMode(GPIOMotorPin2, OUTPUT);
  pinMode(GPIOMotorPin3, OUTPUT);
  pinMode(GPIOMotorPin4, OUTPUT);
  digitalWrite(GPIOMotorPin1, LOW);
  digitalWrite(GPIOMotorPin2, LOW);
  digitalWrite(GPIOMotorPin3, LOW);
  digitalWrite(GPIOMotorPin4, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

 initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}


void loop() {
  ws.cleanupClients();
  
   
    if(forwardRotate == 1 && backwardsRotate == 0){
        onestep();
    }
    else if(forwardRotate == 0 && backwardsRotate == 1){
        bonestep();

}


}
