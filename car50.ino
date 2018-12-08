    /*
     *  Simple HTTP get webclient test
     */
    #include <ESP8266WiFi.h>
    #include "Wire.h" 
    #include <string.h>
   
    const char* ssid     = "mCar";
    const char* password = "il0v3g00gl3";
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 37842;
     
    const char* host = "192.168.43.1";
    String endConnection = "Connection: close\r\n";
    String contentType = "Content-Type: application/json";
    
    const int MPU=0x53;  // I2C address was found by I2c scanner
    int16_t AcX,AcY,AcZ;
    //int16_t Tmp,GyX,GyY,GyZ;
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

    void wifi_init() {
      Serial.print("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
     
      Serial.println("");
      Serial.println("WiFi connected");  
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
    
    void setup() {
      pinMode(encoderIn1, INPUT_PULLUP);
      pinMode(encoderIn2, INPUT_PULLUP);
      Wire.begin(12, 13); // sda, scl
      Wire.beginTransmission(MPU);
        Wire.write(0x2D);
        Wire.write(0);
      Wire.endTransmission(true);

      Wire.beginTransmission(MPU);
        Wire.write(0x2D);
        Wire.write(16);
      Wire.endTransmission(true);

      Wire.beginTransmission(MPU);
          Wire.write(0x2D);
          Wire.write(8);
      Wire.endTransmission(true);


      
  
      Serial.begin(115200);
      delay(100);
     
      // We start by connecting to a WiFi network
     
      Serial.println();
      Serial.println();
      
      wifi_init();
      
      
    }
     
    int value = 0;
    String response;
    uint32_t encoder1 = 0;
    uint32_t encoder2 = 0;
    
    void loop() {
      delay(50); 
      //Serial.println("newloop");
      ++value;
      response = "";
      timer = millis();
      Wire.beginTransmission(MPU);
      Wire.write(0x32);  // starting with register 0x3B (ACCEL_XOUT_H)//  changed
      Wire.endTransmission(false);
      Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
      AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
      AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
      AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
      //Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
      detectState=digitalRead(encoderIn1);
      if (detectState == HIGH) { //If encoder output is high
       i=1;
      }
      else {
       if (i==1){
         countEncoder1++;
          i=0;
        }
      }

      detectState=digitalRead(encoderIn2);
      if (detectState == HIGH) {
        ii=1;
      }
      else {
       if (ii==1){
        countEncoder2++;
        ii=0;
        }
      }

      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      /*
      //set value to Json object :: put your data here!
      root["time"] = timer;
      root["AcX"] = 100 + random(40);// acceleration
      root["AcY"] = -140 - random(40);
      root["AcZ"] = 2050 + random(100);
      encoder1 = encoder1 + random(3);
      encoder2 = encoder2 + random(3);
      root["Encode1"] = 0 + encoder1;
      root["Encode2"] = 28 + encoder2;
      root.printTo(buffer, sizeof(buffer));
      */
      
      client.print(String("POST ") + "/" + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" +
             endConnection +
             contentType + "\r\n\r\n" +
             jsonEncoder(timer, AcX, AcY, AcZ, countEncoder1, countEncoder2)
             + "\r\n");

      String line = client.readStringUntil('\r');

      if(line == "") {
          WiFi.disconnect();
          while (WiFi.status() == WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        wifi_init();
      } else {
      
      //Serial.print(" " + line);
     
      //Serial.println('.');
      //Serial.println("closing connection");
      }
      
    }

