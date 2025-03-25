/*
    Process ota update and log the evolution of 
    the update process (percent or bytes number)
    on your web serial
*/

#include <Arduino.h>
#include <YoupiLab_IoT_ESP.h>


#include <LittleFS.h>


const char* ssid = "wifi_name";
const char* password = "wifi_password";



// YoupiLab IoT Keys for this product
String APP_KEY = "app key";
String APP_ID = "app id";

// Init the YoupiLab_IoT_ESP
//One for the update
YoupiLab_IoT_ESP  YlIoTDriver(APP_ID, APP_KEY);
//The second to Log update progress on your web serial
//If you want to Log the update progress on your web serial
//you must use another instance of YoupiLab_IoT_ESP than the one 
//who is making the update. Otherwhise, your code will crash.
YoupiLab_IoT_ESP  YlIoTLogDriver(APP_ID, APP_KEY);


void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
  //Use the second instance of YoupiLab_IoT_ESP to log update process start
  YlIoTLogDriver.printToWebSerial("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
  //Use the second instance of YoupiLab_IoT_ESP to log update process end
  YlIoTLogDriver.printToWebSerial("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
  //Use the second instance of YoupiLab_IoT_ESP to log update process progress
  YlIoTLogDriver.printToWebSerial("CALLBACK:  HTTP update process at "+String(cur)+" of "+String(total)+" bytes...\n");
}


void update_resources_progress(int fileid, String path) {
  Serial.println("CALLBACK:  HTTP resources update process at file "+String(fileid)+", path is "+ String(path));
  //Use the second instance of YoupiLab_IoT_ESP to log update process progress
  YlIoTLogDriver.printToWebSerial("CALLBACK:  HTTP resources update process at file"+String(fileid)+", path is "+String(path)+" \n");
}

void update_resources_error(String err) {
  Serial.println(" CALLBACK:  HTTP update resources fatal error " + err);
  //Use the second instance of YoupiLab_IoT_ESP to log update process progress
  YlIoTLogDriver.printToWebSerial(err);
}


void update_error(int err) {
  Serial.println("CALLBACK:  HTTP update fatal error code : " + String(err) + "\n");
  //Use the second instance of YoupiLab_IoT_ESP to log update process error 
  YlIoTLogDriver.printToWebSerial("CALLBACK:  HTTP update fatal error code : " + String(err) + "\n");
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);


    //Connect to Wifi
    if (YlIoTDriver.connectToWifi(ssid, password) == 1) {
      Serial.println("Successfully connected to Wifi");
    }else{
      Serial.print("Could not connect to Wifi ");
      Serial.println(ssid);
    }
    
  //Check your desired file system for the OTA
  // LittleFS, LITTLEFS are supported for update by filesystem image
  // All other FS are supported for update by writing files
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");

  }


  //Set the OTA callbacks
  YlIoTDriver.onUpdateStarted(update_started);
  YlIoTDriver.onUpdateFinished(update_finished);
  YlIoTDriver.onUpdateProgress(update_progress);
  YlIoTDriver.onResourcesUpdateProgress(update_resources_progress);
  YlIoTDriver.onUpdateError(update_error);
  YlIoTDriver.onResourcesUpdateError(update_resources_error);

  //Set the filesystem to update your files or resources by OTA
  YlIoTDriver.setFileSystem(&LittleFS);

  //Run the OTA Process :
  //This checks for updates and proceed if there is any new updates
  //You can check for time or any other condition before call the 
  // YlIoTDriver.OTAUpdate() to proceed the update
  if(!YlIoTDriver.deviceIsUpToDate()){
    Serial.println("Device is not up to date");
    Serial.println("Waiting to update ...");
    YlIoTDriver.OTAUpdate();
  }else{
    Serial.println("Device is up to date");
  }


}

// the loop function runs over and over again forever
void loop() {
    
    //
    // You can run the OTA process from your loop as well
    /*  if(!YlIoTDriver.deviceIsUpToDate()){
      YlIoTDriver.OTAUpdate();
    }
    */

}