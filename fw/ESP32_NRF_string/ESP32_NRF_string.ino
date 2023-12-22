#include <SPI.h>
#include "RF24.h"

RF24 radio(4,5); // (CE, CSN)

const byte address[6] = "1RF24"; // address / identifier

//CSV decode
  int Lsub[20];
  int Lind=1;
  long lastMsg = 0;
  char dummy;
  String data0,data1,data2,data3,data4;



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
    Serial.print(text);Serial.println(sizeof(text));



      Serial.println("======Data received======");
      // for(int i=0;i<text.length(); i++)
      // {
      //   if(text.charAt(i)==',')
      //   {
      //     Lsub[Lind++]=i;
      //   }
      // }
      // data0 = text.substring(Lsub[0],Lsub[1]);
      // data1 = text.substring(Lsub[1]+1,Lsub[2]);
      // data2 = text.substring(Lsub[2]+1,Lsub[3]);
      // data3 = text.substring(Lsub[3]+1,Lsub[4]);
      // data4 = text.substring(Lsub[4]+1,Lsub[5]);
      // data5 = text.substring(Lsub[5]+1,text.length()-2);
      // Lind=1;
      text="";
      Serial.println (data0);
      Serial.println (data1);
      Serial.println (data2);
      Serial.println (data3);
      // Serial.println (data4);
      // Serial.println (data5);
      Serial.println("==============================");






  }
}