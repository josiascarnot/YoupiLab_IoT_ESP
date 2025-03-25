#include <Arduino.h>
#include <YoupiLab_IoT_ESP.h>

#include <LittleFS.h>


const char* ssid = "wifi_name";
const char* password = "wifi_password";



// YoupiLab IoT Keys for this product
String APP_KEY = "app key";
String APP_ID = "app id";

// Init the YoupiLab_IoT_ESP
YoupiLab_IoT_ESP  YlIoTDriver(APP_ID, APP_KEY);


//OTA update callbacks
//Feel free to customize

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}


void update_resources_progress(int fileid, String path) {
  Serial.println("CALLBACK:  HTTP resources update process at file "+String(fileid)+", path is "+ String(path));
}

void update_resources_error(String err) {
  Serial.println(" CALLBACK:  HTTP update resources fatal error " + err);
}


void update_error(int err) {
  Serial.println("CALLBACK:  HTTP update fatal error code : " + String(err) + "\n");
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


void loop() {
//
// You can run the OTA process from your loop as well
/*  if(!YlIoTDriver.deviceIsUpToDate()){
  YlIoTDriver.OTAUpdate();
}
*/

}
