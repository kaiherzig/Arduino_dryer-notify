#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

/*
 *Edit WIFI_SSID to your SSID 
 *Edit WIFI_KEY to your Password
 *Edit NOTIFY_URL_START to your WebApi
 *Edit NOTIFY_URL_STOP to your WebApi
 *Edit countdown - How long will we wait for starting in sec. after reaching temp. and for stopping after dropping below. in Sec!
 *int countdown = 60;
 *Edit TargetTemp - in Celsius!
 *To use CallMeBot referer to https://www.callmebot.com/blog/free-api-whatsapp-messages/
 *URL consists of phone= Number in +xxnumber (here +49 but you can change!)
 *text seperated by +
 *apikey is your api key
 */

#define WIFI_SSID "<<FillMe>>"
#define WIFI_KEY "<<FillMe>>"
#define NOTIFY_URL_START "https://api.callmebot.com/whatsapp.php?phone=+49<<NUMBER>>&text=<<start+text>>&apikey=<<APIKEY>>"
#define NOTIFY_URL_STOP "http://api.callmebot.com/whatsapp.php?phone=+49<<NUMBER>>&text=<<finish+text>>&apikey=<<APIKEY>>"

//How long will we wait for starting in sec. after reaching temp. and for stopping after dropping below. in Sec!
int countdown = 60;
//TargetTemp in Celsius!
int TargetTemp = 30;

unsigned long delayTime;
float t;
char temperatureCString[6];
int tickle = 0;
bool running=false;

int setcountdown = countdown;

Adafruit_BME280 bme;
void setup()
{
  Serial.begin(115200);
  delay(10);
  bool status;
  status = bme.begin(0x76);
}
void loop() 
{
  t = bme.readTemperature(); 

  if(t >TargetTemp && running==false)
  {
    Serial.println("running??");
    Serial.println("Actual Temp:");
    Serial.println(t);
    delay(1000);
    tickle=tickle+1;
    Serial.println(tickle);
      if(tickle==countdown)
      {
        running=true;
        Serial.println("OK running!");
        sendStartNotification();
        tickle=0;
      }
  }
  if(t<TargetTemp && tickle !=0)
  {
    tickle=0;
    Serial.println("Resetting tickle...");
  }
  
  if(t<TargetTemp && running==true)
  {
    Serial.println("Temperatur below running temp.");
    Serial.println(t);
    Serial.println("Starting Countdown");
    Serial.println (countdown);
    Serial.println("Sekunden");
    countdown = countdown-1;
    if(countdown==0)
    {
      running=false;
      Serial.println("Done!");
      sendDoneNotification();
      Serial.println("Resetting Countdown");
      countdown=setcountdown;
    }
    delay(1000);
  }
  
  if(t>TargetTemp && running==true)
  {
    Serial.println("still running...");
    Serial.println("Actual temperature:");
    Serial.println(t);
    
    delay(5000);
  }
}
void sendStartNotification() 
{
  Serial.println("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_KEY);
  
  while((WiFi.status() != WL_CONNECTED))
  {
    delay(100);
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connected!");  
  }
  HTTPClient http;
  Serial.println("Sedning Start Notification");
  http.begin(NOTIFY_URL_START);
  int httpCode = http.GET();
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

  WiFi.disconnect();
}
void sendDoneNotification() 
{
  
  Serial.println("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_KEY);
  
  while((WiFi.status() != WL_CONNECTED))
  {
    delay(100);
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connected!");  
  }
  HTTPClient http;
  Serial.println("Sedning Done Notification");
  http.begin(NOTIFY_URL_STOP);
  int httpCode = http.GET();
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

  WiFi.disconnect();
}
