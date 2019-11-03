#include <ESP8266WiFi.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include <ArduinoOTA.h>
#include <WifiUdp.h>
#include <NTPClient.h>


/* OTA LED Info Sketch
 *  This code Displays information about the ESP8266 when connected to WiFi
 *  Displayed Information:
 *    Network Name (SSID)
 *    IP Address
 *    MAC Address
 *    Device Name
 *    
 *  It also flashes the internal LED
 *  
 *  Connect to NTP server to get current time
 *  
 *  Wire Connections
 *    ESP8266   SSD1306
 *    GND       GND
 *    3.3v      VIN
 *    D3        SDA
 *    D5        SCL
 */


#define WIFI_SSID "enter your wifi ssid"
#define WIFI_PASSWORD "enter your wifi password"


SSD1306Wire display(0x3C, D3, D5);
const unsigned long interval = 100; // Time between screen updates & LED flashes

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  Serial.begin(115200);     // Start the serial communication for messages
  delay(10);
  Serial.println('\n');

  Serial.println("Initializing LCD");
  display.init();
  Serial.println("LCD Initialized");
  
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  
  Serial.println("Connecting to WiFi");
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD); // Single Connection WiFi network
  Serial.println("Connecting...");

  int i=0;
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(++i);
    Serial.print('.');
  }


  Serial.println('\n');
  Serial.print("Connected to:");
  Serial.println(WiFi.SSID());      // Network Name
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());   // IP Address

  // After WiFi is connected, set up OTA
  Serial.println("Call Setup OTA");
  setupOTA();  // This function call (and the associated function) can be added to any sketch to enable OTA.
  Serial.println("OTA Setup Call returned");
  Serial.print("Device: ");         // Hostname
  Serial.println(ArduinoOTA.getHostname());

  display.clear();
  display.drawString(64,0,ArduinoOTA.getHostname());
  display.drawString(64,16,WiFi.localIP().toString());
  display.display();

  timeClient.setTimeOffset(-5*60*60); // Time offset is specified in seconds (e.g. -4*60*60 is -4 hours)
  timeClient.setUpdateInterval(12*60*60*1000); // Update every 12 hours (in milliseconds)
  timeClient.update();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
}

unsigned long previousTime = millis();
int progress = 0;
int displayMode = 0;

void loop() {
  ArduinoOTA.handle(); // Check for OTA updates

  // timeClient.update(); Should update every twelve hours based on setup()
  
  unsigned long diff = millis() - previousTime;
  if (diff > interval) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); //Invert state of the LED);

    previousTime += diff;
    progress++;
  }
  if(progress>100) {      // When the progress bar reaches 100%
    progress = 0;         //  reset the progress bar to zero
    displayMode++;        //  switch to next display mode
    if (displayMode > 3)  // Check to make sure we haven't exceeded the available display modes
      displayMode = 0;    //  reset if we have
    display.clear();      // Clear the display (probably unnecessary since we do it each loop anyway)
  }

  display.clear();
  display.drawProgressBar(0,54,127,8, progress); // left_pos, top_pos, width, height, value[0-100]
 
  if (displayMode == 0) {
    display.drawString(64,0,ArduinoOTA.getHostname());
  } else if (displayMode == 1) {
    display.drawString(64,0,WiFi.SSID());
  } else if (displayMode ==2) {
    display.drawString(64,0,WiFi.macAddress());  
  } else if (displayMode == 3) {
    int displayHeight = display.getHeight();
    int displayWidth = display.getWidth();
    String displayDimensions = String(displayWidth) + " x " + String(displayHeight);
    display.drawString(64,0,displayDimensions);
  }
  display.drawString(64,16,WiFi.localIP().toString());
  // display.drawString(64,28,String(millis()));
  display.drawString(64,28,timeClient.getFormattedTime());

  // ToDo - implement hh:mm:ss uptime instead of millis()
  //  unsigned long now = millis()/1000;
  //  int Seconds = now%60;
  //  int Minutes = (now/60)%60;
  //  int Hours = (now/3600)%24;
  //  String hms = "Uptime: " + Hours + ":" + Minutes + ":" + Seconds;
  //  display.drawString(64,32,*hms);
  display.display();
}

// The function below can be called from setup() to enable OTA on the ESP8266 device.
void setupOTA() {
  // The name and password should probably be changed for each device.
  ArduinoOTA.setHostname("ESP8266OTAB");
  ArduinoOTA.setPassword("esp8266");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
    display.clear();
    display.drawString(64,24,"Incoming OTA Update");
    display.display();
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd OTA Update");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}
