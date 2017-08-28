#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "./DNSServer.h"  
#include "Hash.h"
#include "FS.h"
#include <WebSocketsServer.h>

ESP8266WebServer webServer(80);         //Serve up the webpage for controlling the eyeball
const char *ssid = "TheEyeBall";        //This is the name of the WiFi we are going to broadcast
const char *password = "ZoologyByJemi"; //This is the password for our WiFi
String webPage;

DNSServer dnsServer;           // Create the DNS object to forward all web address sent us to our main webpage on our webserver
const byte DNS_PORT = 53;      // Capture DNS requests on port 53
IPAddress apIP(192, 168, 4, 1); // Private network for server

WebSocketsServer webSocket = WebSocketsServer(81); //websocket allows us to send messages back and forth without having to reload the whole page.

const int EyeBall = 0; //this is the pin the eyeball servo is connected to
const int EyeLid = 2;  //this is the pin the eyelid servo is connected to

Servo EyeBallServo; //this is an object we will use to control our servo moving the eyeball
Servo EyeLidServo;  //this is an object we will use to control our servo moving the eyelid

int EyeBallVal;
int EyeLidVal = 120;

unsigned long BlinkTime;
unsigned long currentTime;

void setup()
{
    Serial.begin(115200); //start up the serial port to send debugging messages over the USB while we are programing this.

    EyeBallServo.attach(EyeBall);
    EyeBallServo.write(45);
    EyeLidServo.attach(EyeLid);
    EyeLidServo.write(50);

    BlinkTime = millis();
  
    WiFi.softAP(ssid, password); //start up the WiFi signal
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    webSocket.begin(); //start up and initialize the websocket
    webSocket.onEvent(webSocketEvent);

    //First Check to see if we can access the webpage stored onboard in the built in file system (SPIFFS)
    bool ok = SPIFFS.begin();
    if (ok)
    {
        Serial.println("File System online");
        bool exist = SPIFFS.exists("/index.html");

        if (exist)
        {
            Serial.println("The main webpage exists!");
            File f = SPIFFS.open("/index.html", "r");
            if (!f)
            {
                Serial.println("Some thing went wrong trying to open the file...");
            }
            else
            {
                //So our main webpage index.html exists and we can read it so lets serve it up!
                webPage = f.readString();
                //first we need to get the DNS server in place to force any connections to our webpage
                dnsServer.start(DNS_PORT, "*", apIP);
                webServer.onNotFound([]() {
                    webServer.send(200, "text/html", webPage);
                });
                webServer.begin();
            }
            f.close();
        }
    }
    else
    {
        Serial.println("No such file found.");
    }
}

void loop()
{
    // put your main code here, to run repeatedly:
    dnsServer.processNextRequest();
    webServer.handleClient();
    webSocket.loop();

    //make it blink!
    currentTime = millis();
    if (currentTime - BlinkTime > 5000)
    {
      EyeLidServo.write(50);
      delay(200);
      EyeLidServo.write(EyeLidVal);
      
      BlinkTime = currentTime;
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t lenght)
{

    switch (type)
    {
    case WStype_DISCONNECTED:

        break;
    case WStype_CONNECTED:
    {
        IPAddress ip = webSocket.remoteIP(num);
    }
    break;
    case WStype_TEXT:
    {

        String text = String((char *)&payload[0]);
        Serial.println(text);
        if (text.startsWith("x"))
        {
            String xVal = (text.substring(text.indexOf("x") + 1, text.length()));
            EyeBallVal = xVal.toInt();
            EyeBallServo.write(EyeBallVal);
            Serial.print("Setting eyeball to:");
            Serial.println(xVal);
            webSocket.sendTXT(num, "eyeball changed", lenght);
        }

        if (text.startsWith("y"))
        {
            String yVal = (text.substring(text.indexOf("y") + 1, text.length()));
            EyeLidVal = yVal.toInt();
            EyeLidServo.write(EyeLidVal);
            Serial.print("Setting eyelid to:");
            Serial.println(yVal);
            webSocket.sendTXT(num, "eyelid changed", lenght);
        }
    }

        webSocket.sendTXT(num, payload, lenght);
        webSocket.broadcastTXT(payload, lenght);
        break;
        

    case WStype_BIN:
        hexdump(payload, lenght);

        // echo data back to browser
        webSocket.sendBIN(num, payload, lenght);
        break;
    }
}
