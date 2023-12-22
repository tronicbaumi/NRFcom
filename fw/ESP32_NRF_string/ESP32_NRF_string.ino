#include <SPI.h>
#include "RF24.h"

RF24 radio(4,5); // (CE, CSN)

const byte address[6] = "1RF24"; // address / identifier

//CSV decode
  int Lsub[20];
  int Lind=1;
  long lastMsg = 0;
  char dummy;
  String data0,data1,data2,data3,data4,data5;
  String string_text;



void setup() {
  Serial.begin(115200);

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
}

void loop() {
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
  }
}