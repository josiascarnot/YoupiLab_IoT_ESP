#include "YoupiLab_IoT_ESP.h"


YoupiLab_IoT_ESP::YoupiLab_IoT_ESP(String APP_ID, String APP_KEY): _APP_ID(APP_ID), _APP_KEY(APP_KEY), _BASE_URL("https://iot.youpilab.com/api"),_BASE_OTA_API_URL("https://iot.youpilab.com/api"),_BASE_WEB_LOG_API_URL("https://yl-logs-node-server-jzre.onrender.com"){}

/********************************************** Les SETTER ET GETTER */

/**
   Returns your chip ID

   @return string of your unique chipID
*/
String YoupiLab_IoT_ESP::getHardwareId()
{
#ifdef ESP8266
  return String(ESP.getChipId());
#elif defined(ESP32)
  String ChipIdHex = String((uint32_t)(ESP.getEfuseMac() >> 32), HEX);
  ChipIdHex += String((uint32_t)ESP.getEfuseMac(), HEX);
  return ChipIdHex;
#endif
}


/**
    handle http error
*/
bool YoupiLab_IoT_ESP::requestError(String payload,int httpResponseCode,String httplastError)
{
    bool hasError = true;

    ///Serial.println(payload);
    //Serial.println(httpResponseCode);
    //Serial.println(httplastError);

    if (httpResponseCode > 0) {
      JsonDocument data;
      deserializeJson(data, payload);

      if(httpResponseCode == 200 || httpResponseCode == 201){
        // _lastError = "";
        hasError = false;
      }else{
       if (payload != "{}" && payload.indexOf("<html") == -1 && !data.isNull()) {
        if(!data["message"].isNull()){
          _lastError = data["message"].as<String>();
        }else{
          _lastError = "Something went wrong when trying to contact server";
        }
       }else{
        _lastError = "Server did not send parsable response";
       }
      }
   
     } else {
       _lastError = httplastError;
     }

     return hasError;
}


/**
   Returns last error encountered

   @return string of last error
*/
String YoupiLab_IoT_ESP::getLastError()
{
  String error = _lastError;
  _lastError = "";
  return error;
}

/*
  get app key
*/
String YoupiLab_IoT_ESP::getAppKey(){
  return _APP_KEY;
  }
  /*
    get app id
  */
String YoupiLab_IoT_ESP::getAppId(){
  return _APP_ID;
  }
  /*
    set app key
  */
void YoupiLab_IoT_ESP::setAppKey(String appkey){
       _APP_KEY = appkey;
  }
  /*
    set app id
  */
void YoupiLab_IoT_ESP::setAppID(String appid){
      _APP_ID = appid;
  }
  /*
    get base key
  */
String YoupiLab_IoT_ESP::getBaseUrl(){
     return _BASE_URL;
  }

/*
  set reboot after update
*/

void YoupiLab_IoT_ESP::rebootAfterUpdate(bool reboot){ 
  _REBOOT_AFTER_UPDATE = reboot; 
    
}


/*
  set reboot after update
*/

void YoupiLab_IoT_ESP::printToWebSerialUpdateProgess(bool log){ 
  _WEB_SERIAL_LOG_UPDATE_PROGRESS = log;
    
}

/*
  set file system
*/

void YoupiLab_IoT_ESP::setFileSystem(FS* fileSystem){ 
  fileSystemObj = fileSystem; 
    
}


/*
  set command action
*/
void YoupiLab_IoT_ESP::onNewWebSerialCommand(std::function<void(String)> action){ 
  _onNewWebSerialCommand = action; 
  checkForNewWebSerialCommand();
  
}

/*
  set http update start call back
*/
void YoupiLab_IoT_ESP::onUpdateStarted(std::function<void()> fn){
  _httpUpdateOnStart = fn;
}

/*
  http update end call back
*/
void YoupiLab_IoT_ESP::onUpdateFinished(std::function<void()> fn){
  _httpUpdateOnEnd = fn;

}

/*
  set http update progress call back
*/
void YoupiLab_IoT_ESP::onUpdateProgress(std::function<void(int, int)> fn){
  _httpUpdateOnProgress = fn;
}


/*
  set http files system update progress call back
*/
void YoupiLab_IoT_ESP::onResourcesUpdateProgress(std::function<void(int, String)> fn){
  _httpResourcesUpdateOnProgress = fn;
}


/*
  set http files system update error call back
*/
void YoupiLab_IoT_ESP::onResourcesUpdateError(std::function<void(String)> fn){
  _httpResourcesUpdateOnError = fn;
}


/*
  set http update error call back
*/
void YoupiLab_IoT_ESP::onUpdateError(std::function<void(int)> fn){
  _httpUpdateOnError = fn;
}



/*
  default http update start call back
*/
void YoupiLab_IoT_ESP::defaultHttUpdateOnStartCallback(){ 
  printToWebSerial("CALLBACK:  HTTP update process started");
}


/*
  default http update progress call back
*/
void YoupiLab_IoT_ESP::defaultHttUpdateOnProgressCallback(int cur, int total){ 
  
  if(_WEB_SERIAL_LOG_UPDATE_PROGRESS){
    printToWebSerial("CALLBACK:  HTTP update process at " + String(cur) + " of " + String(total) + " bytes...\n");
  }
}


/*
  default http update end call back
*/
void YoupiLab_IoT_ESP::defaultHttUpdateOnEndCallback(){ 
  printToWebSerial("CALLBACK:  HTTP update process finished");
}


/*
 default http update error call back
*/
void YoupiLab_IoT_ESP::defaultHttUpdateOnErrorCallback(int err){
  printToWebSerial("CALLBACK:  HTTP update fatal error code : " + String(err) + "\n");
}

/*
  api request helper 
*/
JsonDocument YoupiLab_IoT_ESP::apiRequest(String method, String url, JsonDocument body, String baseUrl){ 
  
  
  JsonDocument result = JsonDocument();
  result["success"] = false;

  String complet_url = baseUrl + "" + url;

#ifdef ESP8266
  WiFiClient client;
  requestHttpClient.begin(client,complet_url); 
#elif defined(ESP32)
  requestHttpClient.begin(complet_url);  
#endif

//Serial.println("url api " + complet_url);
  int httpResponseCode = 0;

  if (method == "POST") {
    requestHttpClient.addHeader("Content-Type", "application/json");

    String body_json_str = "";
    serializeJson(body, body_json_str);
 //Serial.println("Api data : " + body_json_str);
    httpResponseCode = requestHttpClient.POST(body_json_str);
  } else if (method == "GET") {
    httpResponseCode = requestHttpClient.GET();
  }


  String payload = "{}";

  if (httpResponseCode > 0) {
 //Serial.println("HTTP " + method + " Response code: ");
 //Serial.println(httpResponseCode);

    payload = requestHttpClient.getString();

    JsonDocument data;
    deserializeJson(data, payload);


   payload = requestHttpClient.getString(); 
 //Serial.println("payload : " + payload);

    if (payload != "{}" && payload.indexOf("<html") == -1 && !data.isNull()) {
    //Serial.println("payload : " + payload);
    //Serial.println("payload  not null ");
      result["data"] = data;

      if(httpResponseCode == 200 || httpResponseCode == 201){
        result["success"] = true;
      }else{
        if(!data["message"].isNull()){
          _lastError = data["message"].as<String>();
        }else{
          _lastError = "Something went wrong when trying to contact server";
        }
      }
    }else{
      _lastError = "Server did not send parsable response";
    }

  } else {
 //Serial.println("Error HTTP " + method + " Response code: ");
 //Serial.println(httpResponseCode);

    _lastError = http.errorToString(httpResponseCode);
  }
  // Free resources
  requestHttpClient.end();


 //Serial.println("Returning long list resut: ");
  return result;
  
}


  /*
    allows to connect to a wifi
  */
int YoupiLab_IoT_ESP::connectToWifi(const char* ssid, const char* password){
  delay(200); 
  WiFi.begin(ssid, password);
  int startMillis = millis();
  while(WiFi.status() != WL_CONNECTED && millis() - startMillis <= 60000){
     delay(500);
  }

  if(WiFi.status() == WL_CONNECTED){
    return 1;
  }
   
  return 0;

  }


/*
  Web serial check command
*/

void YoupiLab_IoT_ESP::checkForNewWebSerialCommand() {

  
//Serial.println("Contacting server...");

  String chipId = getHardwareId();
  String url = "/web_serial/pending_command?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;

  JsonDocument body;
  body["hardware_id"] = chipId;

  JsonDocument data = apiRequest("POST", url, body, _BASE_OTA_API_URL);


  if (data["success"] == true) {
  //Serial.println("check command call Succes");

    JsonDocument content = data["data"];
  //Serial.println("command data is true");

    JsonArray commands = content.as<JsonArray>();
    String ids = "";
    for (JsonObject one : commands) {

      String id = one["unique_id"].as<String>();
      String task = one["task"].as<String>();

      if( _onNewWebSerialCommand != nullptr){
        _onNewWebSerialCommand(task);
      }

    //Serial.println("One command " + id + " : " + task);

     //Serial.println(task + "\n\r");

      ids += id;
      ids += ",";
    }

    //ids = ids.substring(0, ids.length() - 1);
  //Serial.println("Command executed to send " + ids);


   if(ids!=""){
     //Mark tasks as done


     url = "/web_serial/mark_command_done?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;
    
   //Serial.println("Api url " + url);
 
     body["hardware_id"] = chipId;
     body["commands"] = ids;
 
     JsonDocument data = apiRequest("POST", url, body, _BASE_OTA_API_URL);
 
 
     if (data["success"] == true) {
    //Serial.println("mark command done Success");
     } else {
    //Serial.println("mark command done Error");
     }
   }

  } else {
  //Serial.println(" check command call Error");
  }

}

/*
  Web serial logging
*/

void YoupiLab_IoT_ESP::printToWebSerial(String text) {

 //Serial.println(text);

 //Serial.println("Online logging...");
  String chipId = getHardwareId();
  String url = "/web_serial/log?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;

  //String url = "/api/logs/create";

  JsonDocument body;
  body["hardware_id"] = chipId;
  body["text"] = text;

  JsonDocument data = apiRequest("POST", url, body, _BASE_OTA_API_URL);
  //JsonDocument data = apiRequest("POST", url, body, _BASE_WEB_LOG_API_URL);

  if (data["success"] == true) {

  } else {

  }

}



  /*
    Send Ota Errors online
  */

void YoupiLab_IoT_ESP::saveOtaError(String title,String description) {



   //Serial.println("Online ota error logging...");
    String chipId = getHardwareId();
    String url = "/save_error?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;
    JsonDocument body;
    body["hardware_id"] = chipId;
    body["title"] = title;
    body["description"] = description;
  
    JsonDocument data = apiRequest("POST", url, body, _BASE_OTA_API_URL);
  
    if (data["success"] == true) {
  
    } else {
  
    }
  }


  /*
    Proceed to firmware update
  */

bool YoupiLab_IoT_ESP::updateFirmware(String url){
    bool updated = false;


    int sketchFreeSpace = ESP.getFreeSketchSpace();
    if (!sketchFreeSpace)
    {
        saveOtaError("OTA Update failed","Partition Could Not be Found");
        return false;
    }

  #ifdef ESP8266
    WiFiClient client;
    http.begin(client,url); 
  #elif defined(ESP32)
    http.begin(url);  
  #endif
    int fileClienthttpResponseCode = http.GET();
    bool startUpdate = true;

    switch (fileClienthttpResponseCode)
    {
    case 0:
        
        saveOtaError("OTA Update failed","Can not contact server");
        startUpdate = false;
        break;

    case -1:
    
      saveOtaError("OTA Update failed","Can not contact server");
      startUpdate = false;
      break;

    case 404:
        // HTTP_CODE_NOT_FOUND
        saveOtaError("OTA Update failed","File not found at "+url);
        startUpdate = false;
        break;

    default:
        break;
    }

    if (http.getSize() > sketchFreeSpace)
    {
        saveOtaError("OTA Update failed","FreeSketchSpace too low ("+String(sketchFreeSpace)+") needed: "+String(http.getSize()));
        startUpdate = false;
    }

    http.end();

    if(!startUpdate){
      return false;
    }

#ifdef ESP8266
  ESPhttpUpdate.onStart(_httpUpdateOnStart);
  ESPhttpUpdate.onEnd(_httpUpdateOnEnd);
  ESPhttpUpdate.onProgress(_httpUpdateOnProgress);
  ESPhttpUpdate.onError(_httpUpdateOnError);
  ESPhttpUpdate.rebootOnUpdate(false);

  t_httpUpdate_return ret = ESPhttpUpdate.update(updaterWifiClient, url);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      //Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    //Serial.println("HTTP_UPDATE_FAILED");
      saveOtaError("OTA Update failed "+String(ESPhttpUpdate.getLastError()),String(ESPhttpUpdate.getLastErrorString().c_str()));
  
      break;
  
    case HTTP_UPDATE_NO_UPDATES: 
    //Serial.println("HTTP_UPDATE_NO_UPDATES"); 
      break;
  
    case HTTP_UPDATE_OK: 
    //Serial.println("HTTP_UPDATE_OK");
    //Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
    //Serial.println("Update Success , Restaring...");
      updated = true;
      break;
  }
#elif defined(ESP32)
   //Serial.println("Starting OTA at : " + url);

    httpUpdate.onStart(_httpUpdateOnStart);
    httpUpdate.onEnd(_httpUpdateOnEnd);
    httpUpdate.onProgress(_httpUpdateOnProgress);
    httpUpdate.onError(_httpUpdateOnError);
    httpUpdate.rebootOnUpdate(false);


  //Serial.println("Httpupdate call backs sets");

    t_httpUpdate_return ret = httpUpdate.update(updaterWifiClient, url);


    switch (ret) {
      case HTTP_UPDATE_FAILED:
        //Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      //Serial.println("HTTP_UPDATE_FAILED");
        saveOtaError("OTA Update failed "+String(httpUpdate.getLastError()),String(httpUpdate.getLastErrorString().c_str()));

        break;

      case HTTP_UPDATE_NO_UPDATES: 
      //Serial.println("HTTP_UPDATE_NO_UPDATES"); 
        break;

      case HTTP_UPDATE_OK: 
      //Serial.println("HTTP_UPDATE_OK");
      //Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
      //Serial.println("Update Success , Restaring...");
        updated = true;
        break;
    }
#endif

    return updated;
  
}



  /*
    Proceed to resources update by image
  */

  bool YoupiLab_IoT_ESP::updateResources(String url){
    bool updated = false;


  //Serial.println("Starting OTA at : " + url);

   /*  int sketchFreeSpace = ESP.getFreeSketchSpace();
    if (!sketchFreeSpace)
    {
        saveOtaError("OTA Update failed","Partition Could Not be Found");
        return false;
    } */

  #ifdef ESP8266
    WiFiClient client;
    http.begin(client,url); 
  #elif defined(ESP32)
    http.begin(url);  
  #endif
    int fileClienthttpResponseCode = http.GET();
    bool startUpdate = true;

    switch (fileClienthttpResponseCode)
    {
    case 0:
        
        saveOtaError("OTA Update failed","Can not contact server");
        startUpdate = false;
        break;

    case -1:
    
      saveOtaError("OTA Update failed","Can not contact server");
      startUpdate = false;
      break;

    case 404:
        // HTTP_CODE_NOT_FOUND
        saveOtaError("OTA Update failed","File not found at "+url);
        startUpdate = false;
        break;

    default:
        break;
    }

   /*  if (http.getSize() > sketchFreeSpace)
    {
        saveOtaError("OTA Update failed","FreeSketchSpace too low ("+String(sketchFreeSpace)+") needed: "+String(http.getSize()));
        startUpdate = false;
    }
 */
    http.end();

    if(!startUpdate){
      return false;
    }



#ifdef ESP8266

      ESPhttpUpdate.onStart(_httpUpdateOnStart);
      ESPhttpUpdate.onEnd(_httpUpdateOnEnd);
      ESPhttpUpdate.onProgress(_httpUpdateOnProgress);
      ESPhttpUpdate.onError(_httpUpdateOnError);
      ESPhttpUpdate.rebootOnUpdate(false);

      t_httpUpdate_return ret = ESPhttpUpdate.updateFS(updaterWifiClient, url);

      switch (ret) {
      case HTTP_UPDATE_FAILED:
        //Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
       //Serial.println("HTTP_UPDATE_FAILED");
        saveOtaError("OTA Update failed "+String(ESPhttpUpdate.getLastError()),String(ESPhttpUpdate.getLastErrorString().c_str()));

        break;

      case HTTP_UPDATE_NO_UPDATES: 
       //Serial.println("HTTP_UPDATE_NO_UPDATES"); 
        break;

      case HTTP_UPDATE_OK: 
       //Serial.println("HTTP_UPDATE_OK");
       //Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
       //Serial.println("Update Success , Restaring...");
        updated = true;
        break;
      }
#elif defined(ESP32)

      httpUpdate.onStart(_httpUpdateOnStart);
      httpUpdate.onEnd(_httpUpdateOnEnd);
      httpUpdate.onProgress(_httpUpdateOnProgress);
      httpUpdate.onError(_httpUpdateOnError);
      httpUpdate.rebootOnUpdate(false);

      t_httpUpdate_return ret = httpUpdate.updateSpiffs(updaterWifiClient, url);


      switch (ret) {
        case HTTP_UPDATE_FAILED:
          //Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
       //Serial.println("HTTP_UPDATE_FAILED");
          saveOtaError("OTA Update failed "+String(httpUpdate.getLastError()),String(httpUpdate.getLastErrorString().c_str()));

          break;

        case HTTP_UPDATE_NO_UPDATES: 
       //Serial.println("HTTP_UPDATE_NO_UPDATES"); 
          break;

        case HTTP_UPDATE_OK: 
       //Serial.println("HTTP_UPDATE_OK");
       //Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
       //Serial.println("Update Success , Restaring...");
          updated = true;
          break;
      }

#endif


    return updated;
  
}



  /*
    Proceed to resources update by files
  */

bool YoupiLab_IoT_ESP::updateResources(String url, JsonObject files){
    bool updated = false;

    if(fileSystemObj == nullptr){
      saveOtaError("OTA Resources Update failed","File system is not defined. Please call the setFileSystem method to define a file system");
      return false;
    }
    
    int index = 1;
    for (JsonPair kv : files) {
    //Serial.println(kv.key().c_str());
      
      String path = String(kv.key().c_str());
      JsonArray filenames = kv.value().as<JsonArray>();

      for (JsonVariant value : filenames) {
        //Serial.println(value.as<const char*>());
          String one_file_name = value.as<const char*>();
          String file_url = url+"/"+one_file_name;
         //Serial.println("file_url "+file_url);
#ifdef ESP8266
          WiFiClient client;
          http.begin(client,file_url); 
#elif defined(ESP32)
          http.begin(file_url);  
#endif
          int fileClienthttpResponseCode = http.GET();

          if(fileClienthttpResponseCode > 0){
            String filepath = "/"+path+""+one_file_name;
            filepath.replace("//","/");
            //Serial.println("filepath "+filepath);

            if(_httpResourcesUpdateOnProgress != nullptr){
              _httpResourcesUpdateOnProgress(index,String(filepath.c_str()));
            }

            /*try
            {
              int remaining = fileSystemObj->totalBytes() - fileSystemObj->usedBytes();
              if(fileSystemObj->exists(filepath)){
                File file = fileSystemObj->open(filepath,"r");
                remaining = remaining - file.size();
                file.close();
              }
            
              if (http.getSize() > remaining)
              {
                  saveOtaError("OTA Update failed for "+filepath,"Free file system space too low ("+String(remaining)+") needed: "+String(http.getSize()));
                  updated = false;
                  break;
              }
            }
            catch(const std::exception& e)
            {
              //std::cerr << e.what() << '\n';
              saveOtaError("OTA Update error","Can not check file system space : "+String(e.what()));
            }*/
            
           //Serial.println("filepath "+filepath);
            File file = fileSystemObj->open(filepath,"w");
            if(file){
             //Serial.println("file opened "+filepath);
              file.print("");
              int written = http.writeToStream(&file);
              file.close();
             //Serial.println("file close "+filepath);

              if(written == http.getSize()){
                updated = true;
              }else{
                if(written < 0){
                  written = written * -1;
                }
                saveOtaError("OTA Resources Update failed","Can not write "+String(http.getSize())+" bytes to file  "+String(filepath)+". Only "+String(written)+" written");
                if(_httpResourcesUpdateOnError != nullptr){
                  _httpResourcesUpdateOnError("OTA Resources Update failed -> Can not write "+String(http.getSize())+" bytes to file  "+String(filepath)+". Only "+String(written)+" written");
                }
                
                updated = false;
                break;
              }

            }else{
              saveOtaError("OTA Resources Update failed","Filesystem error  : Can not open file  "+String(filepath) + " to write");
              if(_httpResourcesUpdateOnError != nullptr){
                _httpResourcesUpdateOnError("OTA Resources Update failed -> Filesystem error  : Can not open file  "+String(filepath.c_str()) + " to write");
              }
              updated = false;
              break;
            }

          }else{
            saveOtaError("Can not fetch file "+one_file_name,"HTTP GET "+file_url+" error "+String(fileClienthttpResponseCode) + " "+String(HTTPClient::errorToString(fileClienthttpResponseCode)));
            if(_httpResourcesUpdateOnError != nullptr){
              _httpResourcesUpdateOnError("Can not fetch file "+one_file_name+" -> HTTP GET "+file_url+" error "+String(fileClienthttpResponseCode) + " "+String(HTTPClient::errorToString(fileClienthttpResponseCode)));
            }
            updated = false;
            break;
          }

          http.end();
          index++;
      }
      if(!updated){
        break;
      }
  }
    
    return updated;
  
}



/*
  Get versions of resources and firmware
*/

String YoupiLab_IoT_ESP::getVersions(String uri){


  String chipId = getHardwareId();
  String url = uri + "?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;
  
  JsonDocument body;
  JsonDocument data = apiRequest("POST", url, body, _BASE_OTA_API_URL);

  if (data["success"] == true) {
    JsonDocument content = data["data"];
    
    if (!content["version"].isNull()) {
      String version = content["version"].as<String>();
      return version;
    }
  }
  
  return "null";
  
}

/*
  Get resources current version of device
*/

String YoupiLab_IoT_ESP::getResourcesCurrentVersion(){

  return getVersions("/resources/current");

}


/*
  Get resources latest version of device
*/

String YoupiLab_IoT_ESP::getResourcesLatestVersion(){

  return getVersions("/resources/latest");

}

/*
  Get firmwares current version of device
*/

String YoupiLab_IoT_ESP::getFirmwareCurrentVersion(){

  return getVersions("/firmwares/current");

}

/*
  Get firmwares latest version
*/

String YoupiLab_IoT_ESP::getFirmwareLatestVersion(){

  return getVersions("/firmwares/latest");
  
}


  /*
    Check for new updates
  */

  bool YoupiLab_IoT_ESP::deviceIsUpToDate(){


    String chipId = getHardwareId();
    String url = "/my_update?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;
    
    JsonDocument body;
    JsonDocument data = apiRequest("POST", url, body, _BASE_OTA_API_URL);

    bool isUpToDate = true;
  
    if (data["success"] == true) {
    //Serial.println("Succes");
  
      JsonDocument content = data["data"];
    //Serial.println("Data is true");
  
  
      if (!content["firmware"].isNull()) {
        isUpToDate = false;
      }
  
      if(!content["filesystem"].isNull()){
  
        isUpToDate = false;
  
      }
    } else { 
      
      saveOtaError("OTA Update error"," Can not contact server. Please ensure the device is connected to internet");
  
    }
    
    return isUpToDate;
    
  }

  /*
    Proceed to Ota update
  */

void YoupiLab_IoT_ESP::OTAUpdate(){


  String chipId = getHardwareId();
  String url = "/my_update?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;
  
  JsonDocument body;
  JsonDocument data = apiRequest("POST", url, body, _BASE_OTA_API_URL);

  if (data["success"] == true) {
  //Serial.println("Succes");

    JsonDocument content = data["data"];
  //Serial.println("Data is true");

    bool ota_firmware_success = false;
    bool ota_filesystem_success = false;

    String firmware_version = "";
    String filesystem_version = "";


    if (!content["firmware"].isNull()) {
    //Serial.println("Data has update and device");

      JsonObject firmware_update = content["firmware"].as<JsonObject>();
      String version = firmware_update["version"].as<String>();
      String source_path = firmware_update["source_path"].as<String>();


    //Serial.println("Updating firmware to " + version);

      firmware_version = version;

      ota_firmware_success =  updateFirmware(source_path);

    }




    if(!content["filesystem"].isNull()){

      JsonObject fileystem_update = content["filesystem"].as<JsonObject>();
      String version = fileystem_update["version"].as<String>();
      String source_path = fileystem_update["source_path"].as<String>();

      //Serial.println("Updating filesystem to " + version);
      filesystem_version = version;
      

      if(!fileystem_update["files_json"].isNull()){
        //("Spiffs direct upload " + version);

        JsonObject files = fileystem_update["files_json"].as<JsonObject>();

        if(files.size() != 0){
          ota_filesystem_success =  updateResources(source_path,files);
        }else{
          ota_filesystem_success =  updateResources(source_path);
        }

      }else{

        ota_filesystem_success =  updateResources(source_path);

      }

    }



    if(ota_firmware_success || ota_filesystem_success){

      String url = "/mark_as_up_to_date?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;
      url += "&current_firmware_version=" + firmware_version;
      url += "&current_filesystem_version=" + filesystem_version;
      JsonDocument body;
      body["current_firmware_version"] = firmware_version;
      body["current_filesystem_version"] = filesystem_version;

      JsonDocument data = apiRequest("POST", url, body, _BASE_OTA_API_URL);

      if(_REBOOT_AFTER_UPDATE){
        ESP.restart();
      }
    }
  } else { 
    
    saveOtaError("OTA Update error"," Can not contact server. Please ensure the device is connected to internet");

  }
  
}


/*
  execute an action
*/

int YoupiLab_IoT_ESP::executeAction(int nbr_args, ...){
  /***********retrieving instructions to be executed from the IoT platform ******/
    String chipId = getHardwareId();
    String url = _BASE_OTA_API_URL + "/controls/get?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;
    
#ifdef ESP8266
  WiFiClient client;
  //http.setTimeout(60000);
  http.begin(client,url); 
#elif defined(ESP32)
  //http.setTimeout(60000);
  http.begin(url); 
#endif
    
    String etatb; 
    int httpResponseCode = http.GET();
    etatb=http.getString();
    if (!requestError(etatb,httpResponseCode,http.errorToString(httpResponseCode))) {
      //Serial.println("-"+etatb+"-");
      etatb.replace("\n", "");//processing the recovered string
      etatb.replace("[", "");
      etatb.replace("]", "");
      etatb.replace(",", "");
      etatb.replace(" ", "");
      etatb.trim();
      if (etatb !=""){
      va_list liste_arguments;
      va_start(liste_arguments, nbr_args);
      int a=0;
       for (int i = 0; i < nbr_args; i++) {
       int* val_ptr = va_arg(liste_arguments, int*);
       char etat= etatb.charAt(a);
       *val_ptr = etat - '0';  // store the character value in the pointer to the corresponding variablee
       a++;
       }
        va_end(liste_arguments);
     }

     return 1;
   }
   //Serial.println("-"+etatb+"-");
   http.end();
   return 0;
  }
  /*
    send data on our iot platform (https://iot.youpilab.com); return 1 in case of success and 0 in case of failure
  */
 
int YoupiLab_IoT_ESP::sendData(){

    String chipId = getHardwareId();
    
  String post_url = _BASE_URL + "/data/send?" + "APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY+ "&HARDWARE_ID=" + chipId;//Builds GET request URL with identification parameters

  JsonObject parametersValueObj = parametersValue.as<JsonObject>();
  for (JsonPair kv : parametersValueObj) {
      
      String paramName = String(kv.key().c_str());
      String paramValue = kv.value().as<String>();

     //Serial.println(paramName);
     //Serial.println(paramValue);

      post_url += "&" +paramName+ "=" + paramValue;
    
  }

  parametersValue.clear();

 //Serial.println(post_url);
  

#ifdef ESP8266
  WiFiClient client;
  http.begin(client,post_url); 
#elif defined(ESP32)
  http.begin(post_url);  //Starts an HTTP GET request with the constructed URL   
#endif
    
   
      int httpResponseCode = http.GET();//Sends request and retrieves HTTP response code
     
      if (!requestError(http.getString(),httpResponseCode,http.errorToString(httpResponseCode))) {
        http.end();
        return 1;
      }
      else {
        http.end();
        return 0;
      }

  }

  /*
    count the number of data received by our iot platform
  */
String YoupiLab_IoT_ESP::dataCount(){

    String post_url = _BASE_OTA_API_URL+"/data/count?APP_ID=";
    //ok good
    post_url+=_APP_ID;
    post_url +="&APP_KEY=";
    post_url +=_APP_KEY;
#ifdef ESP8266
  WiFiClient client;
  http.begin(client,post_url); 
#elif defined(ESP32)
  http.begin(post_url);  
#endif
      int httpResponseCode = http.GET();
      String payload = http.getString();
    if (!requestError(payload,httpResponseCode,http.errorToString(httpResponseCode))) {
      http.end();
      return payload;
    }
    
    return "null";
 }

 /*
  send feed back
 */
int YoupiLab_IoT_ESP::sendFeedback(){

    String chipId = getHardwareId();
    String post_url = _BASE_OTA_API_URL+"/controls/executed?APP_ID=" + _APP_ID + "&APP_KEY=" + _APP_KEY + "&HARDWARE_ID=" + chipId;
    
  #ifdef ESP8266
    WiFiClient client;
    http.begin(client,post_url); 
  #elif defined(ESP32)
    http.begin(post_url);  
  #endif
      int httpResponseCode = http.GET();
    if (!requestError(http.getString(),httpResponseCode,http.errorToString(httpResponseCode))) {
      //String payload = http.getString();
      /*if (payload != "") {
            http.end();
            payload = "";
            return 1;
        } else {
            http.end();
            return 0;
        }*/

        http.end();
        return 1;
    }
    http.end();
    return -1; 
  }



