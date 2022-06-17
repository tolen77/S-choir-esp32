#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#define LED 2

const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

//Your IP address or domain name with URL path
const char* serverNameTemp   = "http://192.168.4.1/temperature";
const char* serverNameHumi   = "http://192.168.4.1/humidity";
const char* serverNamePres   = "http://192.168.4.1/pressure";
const char* serverNameProfil = "http://192.168.4.1/profiltype";
unsigned int temperature;
unsigned int humidity;
unsigned int pressure;
unsigned int  profil=0;
unsigned long previousMillis = 0;
const long interval = 3000;
unsigned int timeout=0; 

// the number of the LED pin
const int ventpin = 16;  // 16 corresponds to GPIO16
const int ledpin = 17; // 17 corresponds to GPIO17

// setting PWM properties
const int freq = 25000;
const int ledChannel = 0;
const int resolution = 8;

String httpGETRequest(const char* serverName)
{
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}


void setup()
{
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  Serial.begin(115200);
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution); 
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ventpin, ledChannel);
  ledcAttachPin(ledpin, ledChannel);
  ledcWrite(ledChannel, 0);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) 
  { 
    delay(500);
    Serial.print(".");
    digitalWrite(LED, !digitalRead(LED));
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED,HIGH);
}

void loop()
{
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval)
  {
     // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED )
    {
      digitalWrite(LED,HIGH); 
      temperature = httpGETRequest(serverNameTemp).toInt();
      humidity = httpGETRequest(serverNameHumi).toInt();
      pressure = httpGETRequest(serverNamePres).toInt();
      profil = httpGETRequest(serverNameProfil).toInt();
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" *C - Humidity: ");
      Serial.print(humidity);
      Serial.print(" % - Pressyre: ");;
      Serial.print(pressure);
      Serial.println(" hPa");
        if(profil==0)
        {
          if(temperature<25) ledcWrite(ledChannel,20);
          else ledcWrite(ledChannel,200);
        }
        else
        {
          if(temperature>25) ledcWrite(ledChannel,20);
          else ledcWrite(ledChannel,200);
        }

      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else
    {
      Serial.println("WiFi Disconnected");
      digitalWrite(LED,0);
      ledcWrite(ledChannel,0);
      WiFi.begin(ssid, password);
      Serial.println("re-connecting");
      while(WiFi.status() != WL_CONNECTED) 
      { 
        Serial.print(".");
        digitalWrite(LED, !digitalRead(LED));
        delay(250);
      }

    } 
  }
}