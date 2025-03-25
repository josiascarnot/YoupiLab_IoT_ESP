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
    
    //Get the version of the current firmware running on the device
    String current_firmware_version = YlIoTDriver.getFirmwareCurrentVersion();
    Serial.print("Device's current firmware version ");
    Serial.println(current_firmware_version);


    //Get the version of the current resources on the device
    String current_resources_version = YlIoTDriver.getResourcesCurrentVersion();
    Serial.print("Device's current resources version ");
    Serial.println(current_resources_version);


    //Get the version of the latest firmware of the device's group 
    // and which is released
    // Return null if none
    String latest_firmware_version = YlIoTDriver.getFirmwareLatestVersion();
    Serial.print("Device's group latest firmware version ");
    Serial.println(latest_firmware_version);


    //Get the version of the latest resources of the device's group 
    // and which is released
    // Return null if none
    String latest_resources_version = YlIoTDriver.getResourcesLatestVersion();
    Serial.print("Device's group latest resources version ");
    Serial.println(latest_resources_version);
    

}


void loop() {

//

}