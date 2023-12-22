#include <SPI.h>
#include "RF24.h"
#include "ThingSpeak.h"
#include <WiFi.h>
WiFiClient  client;
#include <WiFiManager.h>  
#include "secrets.h"

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key index number (needed only for WEP)
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;


RF24 radio(4,5); // (CE, CSN)

const byte address[6] = "1RF24"; // address / identifier

//CSV decode
  int Lsub[20];
  int Lind=1;
  long lastMsg = 0;
  char dummy;
  String data0,data1,data2,data3,data4,data5;
  float data0f,data1f,data2f,data3f,data4f,data5f;
  String string_text;



void setup() {
  Serial.begin(115200);
  Serial.println("Application started.....");
  Serial.println("========================");
  if (!radio.begin()) 
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  Serial.println(F("\n\n\n\n\n"));
  Serial.println(F("NRF24L01 started...."));
  Serial.println(F("=========================================="));
  radio.openReadingPipe(0,address); // set the address for pipe 0
  radio.startListening(); // set as receiver

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("AutoConnectAP","password"); // password protected ap
  if(!res)
  {
      //logging(4,"Failed to connect");
      //while(1);
      delay(10000);
      ESP.restart();
  } 

  ThingSpeak.begin(client);

}

void loop() 
{
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    //logging(1,"Connecting to SSID");
    //logging(1,SECRET_SSID);

    while (WiFi.status() != WL_CONNECTED) 
    {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
    //logging(1,"Connected.");
  }


  if(radio.available())
  {
    char text[33] = {0}; 
    radio.read(&text, sizeof(text)-1);
    //Serial.print(text);Serial.println(sizeof(text));

    string_text = String(text);
    //Serial.print(text);Serial.println(string_text.length());



    Serial.println("======Data received======");
    for(int i=0;i<string_text.length(); i++)
    {
      if(string_text.charAt(i)==';')
      {
        Lsub[Lind++]=i;
      }
    }
    data0 = string_text.substring(Lsub[0],Lsub[1]);
    data1 = string_text.substring(Lsub[1]+1,Lsub[2]);
    data2 = string_text.substring(Lsub[2]+1,Lsub[3]);
    data3 = string_text.substring(Lsub[3]+1,Lsub[4]);
    //data4 = string_text.substring(Lsub[4]+1,Lsub[5]);
    //data5 = string_text.substring(Lsub[5]+1,sizeof(text)-2);
    Lind=1;
    string_text="";
    Serial.println (data0);
    Serial.println (data1);
    Serial.println (data2);
    Serial.println (data3);
    //Serial.println (data4);
    //Serial.println (data5);
    Serial.println("==============================");




    ThingSpeak.setField(1, data0.toFloat());
    ThingSpeak.setField(2, data1.toFloat());
    ThingSpeak.setField(3, data2.toFloat());
    ThingSpeak.setField(4, data3.toFloat());
    ThingSpeak.setField(5, WiFi.RSSI());

  
    int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    // Write value to Field 1 of a ThingSpeak Channel
    //int httpCode = ThingSpeak.writeField(myChannelNumber, 1, rssi, myWriteAPIKey);
    if (httpCode == 200) 
    {
      Serial.println("Channel write successful.");
    }
    else {
      Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
    }




  }
}