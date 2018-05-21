#include <Ethernet.h>
#include "DHT.h"

int dhtPin = 2;
const int sensorPin = A0;
DHT dht(dhtPin, DHT11);

int SensorValue = 0;

long duration, distance;
const int echoPin = 7;
const int trigPin = 6;


// Set to any mac address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress webServerIP(192,168,0,100);    // Put the IP address for your server

// Set the static IP address to use for Arduino client
IPAddress clientIP(192,168,0,101);

// Used for connecting to web server
EthernetClient client;

void setup() {
  Serial.begin(9600);
  dht.begin();
  Ethernet.begin(mac, clientIP);
  Serial.println("10 seconds delay...");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // give the Ethernet shield some time to initialize:
  delay(10000);
  
}

void loop() {
 checkDHT();
 checkLDR();
 checkUltrasonic();
 
 delay(5000);
}

void checkDHT(){
    // You should replace these with actual sensor values: 
    //float t = 1.22;
    //float h = 2.33;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    
    String sensorsData="sensor1="+String(t)+"&sensor2="+String(h);
    httpRequest("GET /mydatalogger/add_dht11.php?"+sensorsData+" HTTP/1.0");
}

void checkLDR(){
  SensorValue = analogRead(sensorPin);
  
  String sensorsData="sensor3="+String(SensorValue);
  httpRequest("GET /mydatalogger/add_ldr.php?"+sensorsData+" HTTP/1.0");
}

void checkUltrasonic(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration/57.8;

  String sensorsData="sensor4="+String(distance);
  httpRequest("GET /mydatalogger/add_ultrasonic.php?"+sensorsData+" HTTP/1.0");
  
  delay(50);
}

void httpRequest(String request) 
{
/*---connect the "Arduino" as client to the web server---*/ 
   if (client.connect(webServerIP,80)) {  //connect the "Arduino" as client to the web server using socket   
      Serial.println("connected.");
      Serial.println("sending data to web server...");      
      Serial.println(request);
      Serial.println("Connection: close"); // telling the server that we are over transmitting the message
      Serial.println(); // empty line
/*----send sensors data to the web server using GET request---*/ 
      client.println(request);
      client.println("Connection: close"); // telling the server that we are over transmitting the message
      client.println();                    // empty line
/*----display the response message from the server------------*/
      httpResponse();
    }
    else {
/*---if Arduino can't connect to the server----------*/
      Serial.println("--> connection failed\n");
      while (true);                          // do nothing forevermore
    }
    if (client.connected()) { 
      client.stop();                         // close communication socket 
    }
}
/******************************************/
void httpResponse() 
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();    // closing connection to server
  } 
}


