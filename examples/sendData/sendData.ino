/*
 * 1 pour entier
 * 2 pour nombre à virgule
 * 3 pour string
 */
#include <YoupiLab_IoT_ESP.h>


const char* ssid =  "Mon_Wifi";//
const char* password = "1234567890";//
int pho_res = A0;
String d;
String APP_ID     = "espc8123";  // Equipment APP ID
String APP_KEY    = "6033f31f";  // Equipment Key
YoupiLab_IoT_ESP  YlIoTDriver(APP_ID, APP_KEY);


//print error if an operation doesn't run well
void printLastError(){
  String error = YlIoTDriver.getLastError();
  Serial.print("An error occurred : ");
  Serial.println(error);
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
 
  pinMode(pho_res, INPUT);
}

void loop() {

  //read light sensor value
  int lummiere=analogRead(pho_res);

  float n1=23.03 ;//decimal number
  float n2=3.14;//decimal number
  d="YoupiLab";//string
 
  //Send thing's parameters values
  YlIoTDriver.parametersValue["P1"] = lummiere;
  YlIoTDriver.parametersValue["P2"] = d;
  YlIoTDriver.parametersValue["P3"] = n1;
  YlIoTDriver.parametersValue["P4"] = n2;
  if (YlIoTDriver.sendData() == 1) {
    Serial.println("Parameters values sent successfully");
  }else{
    printLastError();
  }

  String n_data=YlIoTDriver.dataCount();//Count data number in the thing
  if(n_data == "null"){
    Serial.println("Can not get data count");
    printLastError();
  }else{
    Serial.print("dataCount=");
    Serial.println(n_data);
  }
  delay(5000);


}
