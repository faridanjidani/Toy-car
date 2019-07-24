#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define Addr 0x53

//#define BATTERY_LEVEL_PIN ADC

const char *ssid = "Mcar";
const char *password = "123456789";
const int httpPort = 37842;
const char *host = "192.168.4.2";
WiFiClient client;
String endConnection = "Connection: close\r\n";
String contentType = "Content-Type: application/json";



ADC_MODE(ADC_VCC);




String test = " this is test code=178" ;  // test

int16_t xAccl, yAccl, zAccl;
const int encoderIn1 = 0;
const int encoderIn2 = 4;
int countEncoder1=0;
int countEncoder2=0;
int i=0;
int ii=0;
int detectState=0;
uint32_t timer;//unsigned int
String jsonEncoder(uint32_t arg0, int16_t arg1, int16_t arg2, int16_t arg3, uint32_t arg4, uint32_t arg5) {
      return "{\"time\":\"" + String(arg0) + "\", \"AcX\":\"" + String(arg1) + "\", \"AcY\":\"" + String(arg2) +
              "\", \"AcZ\":\"" + String(arg3) + "\", \"Encode1\":\"" + String(arg4) + "\", \"Encode2\":\"" + String(arg5) + "\"}";
}

void handleroot(int16_t &xAccl , int16_t &yAccl , int16_t &zAccl )
{
      unsigned int data[6];
      Wire.beginTransmission(Addr);
      Wire.write(0x2C);
      Wire.write(0x0A); 
      Wire.endTransmission();
      Wire.beginTransmission(Addr);
      Wire.write(0x2D);
      Wire.write(0x08);
      Wire.endTransmission();
      Wire.beginTransmission(Addr);
      Wire.write(0x31);
      Wire.write(0x08);
      Wire.endTransmission();
      delay(300);
    
      for (int i = 0; i < 6; i++)
      {
        Wire.beginTransmission(Addr);
        Wire.write((50 + i));
        Wire.endTransmission();
        Wire.requestFrom(Addr, 1);
        if (Wire.available() == 1)
        {
          data[i] = Wire.read();
        }
      }
      xAccl = (((data[1] & 0x03) * 256) + data[0]);
      if (xAccl > 511)
      {
        xAccl -= 1024;
      }
      yAccl = (((data[3] & 0x03) * 256) + data[2]);
      if (yAccl > 511)
      {
        yAccl -= 1024;
      }
      zAccl = (((data[5] & 0x03) * 256) + data[4]);
      if (zAccl > 511)
      {
        zAccl -= 1024;
      }

      delay(200);
}


void setup()
{
  ESP.eraseConfig();
  Wire.begin(12, 13);
  pinMode(encoderIn1, INPUT_PULLUP);
  pinMode(encoderIn2, INPUT_PULLUP);
  //pinMode(BATTERY_LEVEL_PIN, INPUT);
  delay(2000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println(myIP);
  
  attachInterrupt(digitalPinToInterrupt(encoderIn1), enc1, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderIn2), enc2, RISING);
  
}


void enc1(){

     countEncoder1++;

}

void enc2(){

      countEncoder2++;

}

void loop()
{
  //delay(200);
  send_data();
}

void send_data(){
  timer = millis();

  if (!client.connect(host, httpPort)) {
        delay(1500);
        Serial.println("connection failed");
        return;
  }
  handleroot(xAccl ,yAccl ,zAccl );

      Serial.print("Acceleration in X-Axis : ");
      Serial.println(xAccl);
      Serial.print("Acceleration in Y-Axis : ");
      Serial.println(yAccl);
      Serial.print("Acceleration in Z-Axis : ");
      Serial.println(zAccl);
      Serial.print("encoder1:");
      Serial.println(countEncoder1);
      Serial.print("encoder2:");
      Serial.println(countEncoder2);
      Serial.print("ADC");
      Serial.println(analogRead(A0));
  client.print(String("POST ") + "/" + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" +
             endConnection +
             contentType + "\r\n\r\n" +
             jsonEncoder(timer,xAccl, yAccl, zAccl, countEncoder1, countEncoder2)
             + "\r\n");
  Serial.print("sending data to cellphone");
}

