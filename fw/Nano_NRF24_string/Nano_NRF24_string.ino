#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8); // (CE, CSN)

const byte address[6] = "1RF24"; // address / identifier

void setup() 
{
  Serial.begin(115200);
  while (!Serial) 
  {
    // some boards need to wait to ensure access to serial over USB
  }

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) 
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  radio.openWritingPipe(address); // set the address
  radio.stopListening(); // set as transmitter
}


void loop() {
  const char text[] = "Hi Receiver"; // max. 32 bytes
  radio.write(&text, sizeof(text));
  
  delay(2000);
}