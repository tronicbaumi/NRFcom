#include <SPI.h>
#include "RF24.h"

RF24 radio(4,5); // (CE, CSN)

const byte address[6] = "1RF24"; // address / identifier

void setup() {
  Serial.begin(115200);

  if (!radio.begin()) 
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  radio.openReadingPipe(0,address); // set the address for pipe 0
  radio.startListening(); // set as receiver
}

void loop() {
  if(radio.available()){
    char text[33] = {0}; 
    radio.read(&text, sizeof(text)-1);
    Serial.println(text);
  }
}