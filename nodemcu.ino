#include <ESP8266WiFi.h>;

#include <WiFiClient.h>;

#include <ThingSpeak.h>;

const char* ssid = "Surya"; //Your Network SSID

const char* password = "6282309241"; //Your Network Password

String data;
String receivedVal="";


WiFiClient client;

unsigned long myChannelNumber = 1723434; //Your Channel Number (Without Brackets)

const char * myWriteAPIKey = "3RR0CNABFBBQI8SU"; //Your Write API Key

void setup()

{

Serial.begin(115200);


delay(10);

// Connect to WiFi network

WiFi.begin(ssid, password);



ThingSpeak.begin(client);

}


String getValue(String receivedVal, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = receivedVal.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(receivedVal.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
    
  }

  return found>index ? receivedVal.substring(strIndex[0], strIndex[1]) : " ";
}


void loop()

{
//Serial.println("test");
//String data = Serial.readString();
//Serial.println(data);
receivedVal = Serial.readString();
Serial.println(receivedVal);
    
     
      String  fan      = getValue(receivedVal, ',', 0);
      String  bulb       = getValue(receivedVal, ',', 1);
      
      


ThingSpeak.writeField(myChannelNumber, 1,fan, myWriteAPIKey); //Update in ThingSpeak
ThingSpeak.writeField(myChannelNumber, 2,bulb, myWriteAPIKey); //Update in ThingSpeak






delay(10);

}
