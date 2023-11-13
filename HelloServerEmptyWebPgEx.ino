// This code is derived from the HelloServer Example 
// in the (ESP32) WebServer library .
//
// It hosts a webpage which has one temperature reading to display.
// The webpage is always the same apart from the reading which would change.
// The getTemp() function simulates getting a temperature reading.
// homePage.h contains 2 constant string literals which is the two parts of the
// webpage that never change.
// handleRoot() builds up the webpage by adding as a C++ String:
// homePagePart1 + getTemp() +homePagePart2 
// It then serves the webpage with the command:  
// server.send(200, "text/html", message);
// Note the text is served as html.
//
// Replace the code in the homepage.h file with your own website HTML code.
// 
// This example requires only an ESP32 and download cable. No other hardware is reuired.
// A wifi SSID and password is required.
// Written by: Natasha Rohan  12/3/23
//
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "homepage.h"
#include <DFRobot_DHT11.h>
#include <Adafruit_PN532.h>
#include <Servo.h>

DFRobot_DHT11 DHT;
#define DHT11_PIN 14

#define PN532_SCK   (16)
#define PN532_MOSI  (15)
#define PN532_SS    (4)
#define PN532_MISO  (5)
#define PN532_IRQ   (16)
#define PN532_RESET (15)
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

Servo servo1;
static const int servoPin = 21;

const char* ssid = "Humm";
const char* password = "g00416547";

WebServer server(80);

//temp function to simulate temp sensor
String getTemp() 
{
  DHT.read(DHT11_PIN);
  String temp = String(DHT.temperature);
  return temp;
}

String getHumid()
{
  DHT.read(DHT11_PIN);
  String humid = String(DHT.humidity);
  return humid;
}

void motorLock()
{
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
  uint8_t uidLength;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type

  int lock = 0;
  if(lock == 0)
  {
    for(int posDegrees = 0; posDegrees <= 90; posDegrees += 2)
    {
      servo1.write(posDegrees);
      delay(20);
    }

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
    if(success)
    {
      for(int posDegrees = 90; posDegrees >= 0; posDegrees -= 2)
      {
        servo1.write(posDegrees);
        delay(20);
      }
    }
  }

  if(lock == 1)
  {
    
  }
}

void handleRoot() {
  String message1 = homePagePart1 + getTemp() + homePagePart2 + getHumid() + homePagePart3;
  server.send(200, "text/html", message1);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  nfc.begin();

  //Error catching
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print(versiondata);
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  //State whether it's successful
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  //Prevents an infinite loop while waiting for a card
  nfc.setPassiveActivationRetries(0xFF);

  servo1.attach(servoPin);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
  
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
  uint8_t uidLength;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  if(success)
  {
    motorLock();
  }
}
