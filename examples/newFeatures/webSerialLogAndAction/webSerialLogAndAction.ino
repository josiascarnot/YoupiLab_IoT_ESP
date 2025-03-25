#include <Arduino.h>
#include <YoupiLab_IoT_ESP.h>


const char* ssid = "wifi_name";
const char* password = "wifi_password";




// YoupiLab IoT Keys for this product
String APP_KEY = "app key";
String APP_ID = "app id";

// Init the YoupiLab_IoT_ESP
YoupiLab_IoT_ESP  YlIoTDriver(APP_ID, APP_KEY);


//Define the function to execute when there is a new command from your web serial
void execCmd(String cmd) {
  Serial.println(cmd);

}



void setup() {
  Serial.begin(9600);

    //Connect to Wifi
    if (YlIoTDriver.connectToWifi(ssid, password) == 1) {
      Serial.println("Successfully connected to Wifi");
    }else{
      Serial.print("Could not connect to Wifi ");
      Serial.println(ssid);
    }
    
    

}


void loop() {

  //This checks if there is a new command from your web serial
  //call the function you pass in parameter there is
  YlIoTDriver.onNewWebSerialCommand(execCmd);
  delay(500);


  Serial.print("ESP32 IP on the WiFi network: ");
  Serial.println(WiFi.localIP());


  //Log the IP address of the device on your web serial
  YlIoTDriver.printToWebSerial("ESP32 IP on the WiFi network 2 : " + WiFi.localIP().toString());


}