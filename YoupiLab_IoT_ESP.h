#ifndef YOUPILAB_LIBRARY_ESP_32_H
#define YOUPILAB_LIBRARY_ESP_32_H

#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>



#if defined(ESP8266)
   #include <Wire.h> 
   #include <ESP8266HTTPClient.h> 
   #include <ESP8266WiFi.h>
   #include <ESPAsyncTCP.h>
   #include <WiFiClient.h>
   #include <ESP8266httpUpdate.h>
#elif defined(ESP32)
   #include<WiFi.h>
   #include <WiFiAP.h>
   #include <HTTPUpdate.h>
   #include <HTTPClient.h>
#endif


struct YoupiLab_IoT_ESP{
  
   public:
      YoupiLab_IoT_ESP(String APP_KEY, String APP_ID);

      int connectToWifi(const char* ssid, const char* password); //allows to connect to a wifi

      int sendData(); //send a data on our iot platform (https://iot.youpilab.com); return 1 in case of success and 0 in case of failure

      int executeAction(int nbr_args, ...); //execute an action return 1 in case of success and 0 in case of failure

      String dataCount();//count the number of data items sent to the object

      int sendFeedback();//Send feedback after executing a task
      

      String getResourcesCurrentVersion(); // get current version of resources of the device Ex: 1.0.1
      String getResourcesLatestVersion(); // get latest version of resources of the thing Ex: 1.0.1
      String getFirmwareCurrentVersion(); // get current version of firmwares of the device
      String getFirmwareLatestVersion(); // get latest version of firmwares of the thing
      
      bool deviceIsUpToDate(); // check for new updates
      void OTAUpdate(); // proceed to ota update
      void printToWebSerial(String text); // log to web serial
      void onNewWebSerialCommand(std::function<void(String)> action); // check if there is a new web serial command and execute the callback
      void saveOtaError(String title, String description); // to save errors encountered during ota process

      //Ota update callbacks
      void onUpdateStarted(std::function<void()> fn);
      void onUpdateFinished(std::function<void()> fn);
      void onUpdateProgress(std::function<void(int, int)> fn);
      void onUpdateError(std::function<void(int)> fn);
      void onResourcesUpdateProgress(std::function<void(int, String)> fn);
      void onResourcesUpdateError(std::function<void(String)> fn);

      void setFileSystem(FS* fileSystem); // to set file system for ota updates
      void rebootAfterUpdate(bool reboot); //set if the device must reboot after an update
      String getLastError(); //get last error encountered while making http calls or others process


      JsonDocument parametersValue; //hold thing's parameters value
   
   private:
      HTTPClient http;
      HTTPClient requestHttpClient;
      WiFiClient updaterWifiClient;
      String _APP_ID;
      String _APP_KEY;
      String _BASE_URL;
      String _BASE_OTA_API_URL;
      String _BASE_WEB_LOG_API_URL;
      String _lastError = "";
      bool _REBOOT_AFTER_UPDATE = true;
      bool _WEB_SERIAL_LOG_UPDATE_PROGRESS = false;
      std::function<void(String)> _onNewWebSerialCommand = nullptr;
      std::function<void()> _httpUpdateOnStart = nullptr;
      std::function<void()> _httpUpdateOnEnd = nullptr;
      std::function<void(int)> _httpUpdateOnError = nullptr;
      std::function<void(int, int)> _httpUpdateOnProgress = nullptr;
      std::function<void(int,String)> _httpResourcesUpdateOnProgress = nullptr;
      std::function<void(String)> _httpResourcesUpdateOnError = nullptr;

      String getAppKey();
      String getBaseUrl();
      String getAppId();
      String getVersions(String uri);
      String getHardwareId();
      void setAppKey(String key);
      void setAppID(String id);
      bool requestError(String payload,int httpResponseCode,String httplastError); // check if an http call results in error and save error



      FS *fileSystemObj;
      JsonDocument apiRequest(String method, String uri, JsonDocument body = JsonDocument(), String baseUrl = ""); // api request helper
      bool updateFirmware(String url); //update firmware from image bin
      bool updateResources(String url); // update fs from image bin
      bool updateResources(String url, JsonObject files); //update fs by writing files in memory

      void checkForNewWebSerialCommand(); //check for web serial command

      //unused
      void defaultHttUpdateOnStartCallback();
      void defaultHttUpdateOnEndCallback();
      void defaultHttUpdateOnErrorCallback(int err);
      void defaultHttUpdateOnProgressCallback(int cur, int total);
      void printToWebSerialUpdateProgess(bool log);

};

#endif