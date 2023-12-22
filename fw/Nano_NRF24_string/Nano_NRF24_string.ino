#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include "HX711.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_SleepyDog.h>

#define UART

#define SLEEP_DELAY 1     // debug
//#define SLEEP_DELAY 3     // debug
//#define SLEEP_DELAY 75    // 10min
//#define SLEEP_DELAY 150   // 20min
//#define SLEEP_DELAY 225   // 30min
//#define SLEEP_DELAY 450   // 60min


// Data wire is connected to GPIO15
#define ONE_WIRE_BUS 5
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
HX711 scale;

RF24 radio(7, 8); // (CE, CSN)
const byte address[6] = "1RF24"; // address / identifier

char value0char[5] = "";
char value1char[3] = "";
char value2char[8] = "";
char value3char[8] = "";
char value4char[8] = "";
char value5char[8] = "";

String value0;
String value1;
String value2;
String value3;
String value4;
String value5;

float tempDS18B20;
char tempDS18B20String[5];

float weight = 0;
float weightfactor = 1;
char weightString[12];

int cnt=0;

void setup() 
{
  delay(5000);
  Serial.begin(115200);
  Serial.println("Beehive 'Scale started....");
  Serial.println("==========================");
  Init_Radio();
  Init_LoadCell();

  Serial.print("Initializing DS18B20...");
  sensors.begin();
  Serial.println(" Done!");

}


void loop() 
{
 
  value0 = String(cnt);
  value0.toCharArray(value1char,4);


  value1 = String(float(1690/float(analogRead(1))),2);
  value1.toCharArray(value1char,8);
  #ifdef UART
  Serial.print("Vbat: ");
  Serial.println(value1char);
  #endif

  weight = weightfactor*scale.get_units(10);
  value2 = String(weight);
  value2.toCharArray(value2char,5);
  #ifdef UART
    Serial.print("Stockgewicht: ");Serial.println(value2char);
  #endif

  sensors.requestTemperatures(); // Send the command to get temperatures
  tempDS18B20 = sensors.getTempCByIndex(0);
  if (tempDS18B20 != DEVICE_DISCONNECTED_C)
  {
    value3 = String(tempDS18B20);
    value3.toCharArray(value3char,8);
  #ifdef UART
    Serial.print("StockTemperature: ");Serial.println(value3char);
  #endif

  }
  else
  {
    #ifdef UART    
    Serial.println("Error: Could not read temperature data");
    #endif
  }

  char csvString[64] = "";
  strcat(csvString,value0char);
  strcat(csvString,value1char);
  strcat(csvString,";");
  strcat(csvString,value2char);
  strcat(csvString,";");
  strcat(csvString,value3char);
  strcat(csvString,";");

  #ifdef UART 
  Serial.println(csvString);
  #endif
  //Serial.println(sizeof(csvString));
  radio.write(&csvString, sizeof(csvString));
  #ifdef UART 
  Serial.println("Message sent.");
  #endif

  cnt++;

  //delay(3000);
  radio.powerDown();
  scale.power_down();
  for(int i=0;i<SLEEP_DELAY;i++)
  {
    #ifdef UART 
    Serial.println("Going to sleep ...");

    #endif
    
    delay(10);
    int sleepMS = Watchdog.sleep(8000);
    #ifdef UART 
    Serial.print("I'm awake now! I slept for ");
    Serial.print(sleepMS, DEC);
    Serial.println(" milliseconds.");
    #endif
    
  }
  #ifdef UART 
  Serial.println();
  #endif

  radio.powerUp();
  scale.power_up();
}




void Init_LoadCell(void)
{
  Serial.println("Initializing HX711");

  // Initialize library with data output pin, clock input pin and gain factor.
  // Channel selection is made by passing the appropriate gain:
  // - With a gain factor of 64 or 128, channel A is selected
  // - With a gain factor of 32, channel B is selected
  // By omitting the gain factor parameter, the library
  // default "128" (Channel A) is used here.
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());			// print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  	// print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);	// print the average of 5 readings from the ADC minus tare weight (not set) divided
						// by the SCALE parameter (not set yet)

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
						// by the SCALE parameter set with set_scale

  Serial.println("...Done!");
  
}

void Init_Radio(void)
{
    // initialize the transceiver on the SPI bus
  if (!radio.begin()) 
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  Serial.println("\n\n\n\n\n");
  Serial.println(F("NRF24L01 started..."));
  Serial.println(F("======================================="));

  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.

  radio.openWritingPipe(address); // set the address
  radio.stopListening(); // set as transmitter

}