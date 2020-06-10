#include <SoftwareSerial.h>       //Software Serial library
SoftwareSerial espSerial(2, 3);   //Pin 2 and 3 act as RX and TX. Connect them to TX and RX of ESP8266      
#define DEBUG true
String mySSID = "wollymc";       // WiFi SSID
String myPWD = "123456789"; // WiFi PasswordString
String myAPI = "FRVWBAVK4RWE71F1";   // API Key
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myFIELD ;
float TempCel;
int BPM;
int TempPin = A0;
int pulse = A1;
int bp = A2;
int BLOOD;
float TempValue;
float Temp;
float TempFarh;
String s;
void setup()
{
  Serial.begin(9600);
  espSerial.begin(115200);
   
  espData("AT+RST", 1000, DEBUG);                      //Reset the ESP8266 module
  espData("AT+CWMODE=1", 1000, DEBUG);                 //Set the ESP mode as station mode
  espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000, DEBUG);   //Connect to WiFi network
  /*while(!esp.find("OK"))
  {          
      //Wait for connection
  }*/
  delay(1000);

}
void heat()
{
  TempValue = analogRead(TempPin); // Getting LM35 value and saving it in variable
   Temp = ( TempValue/1024.0)*500; // Getting the celsius value from 10 bit analog value
   TempFarh = (TempCel*9)/5 + 32; // Converting Celsius into Fahrenhiet
   TempCel = Temp;
  Serial.print("TEMPRATURE in Celsius = "); //Displaying temperature in Celsius
  Serial.print(Temp);
  Serial.print("*C");
  Serial.print("    |    ");

  Serial.print("TEMPRATURE = "); // Displaying Temperature in Fahrenheit
  Serial.print(TempFarh);
  Serial.print("*F");
  Serial.println();
 
  delay(1000);

}
void heart()
{
  BPM = analogRead(pulse);
  BPM = map(BPM,1000,0,0,120);
  Serial.println("BPM:");
  BPM = BPM+10;
  Serial.print(BPM);
  delay(1000);
}
void blood_pressure()
{
 BLOOD = analogRead(bp);
 BLOOD = map(BLOOD,0,1023,0,250);
  Serial.println("BP:");
  Serial.print(BLOOD);
  delay(1000);
}
  void loop()
  {
  heat();
  heart();
  blood_pressure();
  if(BLOOD > 180 || (BPM > 80 || BPM < 40) || TempFarh > 101 )
  {
    s = "ABNORMAL";
  }
  else
  {
    s = "NORMAL";
  }
    /* Here, I'm using the function random(range) to send a random value to the
     ThingSpeak API.
    */
    myFIELD += "&field1=";
myFIELD += String(BLOOD);
myFIELD += "&field2=";
myFIELD += String(BPM);
 myFIELD += "&field3=";
myFIELD += String(TempFarh);
myFIELD += "&field4=";
myFIELD += String(s);
   
    String sendData = "GET /update?api_key="+ myAPI +"&"+ myFIELD ;
    espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(myFIELD);
     
   
    espData("AT+CIPCLOSE=0",1000,DEBUG);
    delay(10000);
  }

  String espData(String command, const int timeout, boolean debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");
 
  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug)
  {
    //Serial.print(response);
  }
  return response;
}
