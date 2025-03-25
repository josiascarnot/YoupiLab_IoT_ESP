/*
 * 1 pour entier
 * 2 pour nombre à virgule
 * 3 pour string
 */
#include <YoupiLab_IoT_ESP.h>


const char* ssid =  "Mon_Wifi";//
const char* password = "1234567890";//
String APP_ID     = "espc8123";  // Equipment APP ID
String APP_KEY    = "6033f31f";  // Equipment Key
YoupiLab_IoT_ESP  YlIoTDriver(APP_ID, APP_KEY);
int n = 2;
int led1 = 4;
int led2 = 5;
int button1, button2;

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
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop() {

  // Get remotes controls value and write it into 
  // button1 and button2 variables
 if(YlIoTDriver.executeAction(n, &button1, &button2) == 1){
    Serial.print("button1=");
    Serial.println(button1);
    Serial.print("button2=");
    Serial.println(button2);
    if (button1 == 1) {
      digitalWrite(led1, HIGH);
    } else if (button1 == 0) {
      digitalWrite(led1, LOW);
    }
    if (button2 == 1) {
      digitalWrite(led2, HIGH);
    } else if (button2 == 0) {
      digitalWrite(led2, LOW);
    }
 }else{
  printLastError();
 }

}
