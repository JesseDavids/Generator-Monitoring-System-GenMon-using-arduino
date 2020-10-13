/*
Author: Jesse Davids
Date: 08/10/2020
License: GNU GPLv2
Coding platform: PlatformIO
*/

#include <Arduino.h>
#include <SPI.h>
//Ethernet2 library used for the w5500 ethernet chip
#include <Ethernet2.h>
//ArduinoJson library needed to display object in website
#include <ArduinoJson.h>
//Define a mac address, each MCU module that you use must have a unique MAC address assigned to them
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
//Initialise the global pin variables
int pin7 = 7;
int pin4 = 4;
int pin2 = 2;
//Initialise the server
EthernetServer server(80);
//To read the hyperlink
String readString;

char jsonOutput[128];

void setup() {
  Serial.begin(9600); 

  w5500.init(11);
  //Begin Ethernet service.
  Ethernet.begin(mac);
  //Start server.
  server.begin();
  // Print IP to paste into browser. This is a DHCP IP
  Serial.println(Ethernet.localIP()); 
  // SHOULD BE INPUT_PULLUP & CONNECT RESISTOR TO WIRE THAT IS CONNECTED TO THE PIN TO OBSORB VOLTAGE FLUCTUATIONS
  pinMode(pin7, INPUT_PULLUP); 
  pinMode(pin4, INPUT_PULLUP);
  pinMode(pin2, INPUT_PULLUP);
}


void loop() {
  //Ethernet.maintain() function maintains the connection to the router which it is connected to.
  Ethernet.maintain();
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
        if (readString.length()<100){
          readString += c;
        }
        
        if (c == '\n') {          
          // send a standard http response header, the connection-type can be changed to
          // text/html if you'd like to just use html. In this case we are using JSON to display
          // data on the website.
          client.println("HTTP/1.1 200 OK");
          client.println("Connection-Type: application/json");  
          //the connection will be closed after completion of the response.       
          client.println("Connection: close");  
          //uncomment below code to refresh the page automatically every 60 sec down to 1 second.
          //client.println("Refresh: 60");  
          client.println();
          //Initialise JSON procedure. JSON_OBJECT_SIZE should be changed according to how many
          //sensor or pin data you want to display, it should be the exact amount. Here i'm using 3.             
          const size_t cap = JSON_OBJECT_SIZE(3);
          StaticJsonDocument<cap> doc;
          JsonObject object = doc.to<JsonObject>();
          //Reading pin values, remove the exclamation mark to NOT invert the value.
          int valuepin4;
          valuepin4 = !digitalRead(pin4);
          int valuepin2;
          valuepin2 = !digitalRead(pin2);     
          int valuepin7;
          valuepin7 = !digitalRead(pin7);
          //Create the JSON objects to display.
          object["sp"] = valuepin7;
          object["gp"] = valuepin4;
          object["gf"] = valuepin2;
          serializeJson(doc, jsonOutput); 
          //Display to website.
          client.println(jsonOutput);
          break;
        }        
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();    
  delay(5000);   
}
}
