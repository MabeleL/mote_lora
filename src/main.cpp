
#include <DHT.h>
#include <DHT_U.h>
#include "Arduino.h"
#include "ACS712.h"
#include <TheThingsNetwork.h>
#include <SoftwareSerial.h>

//initialize the LoRa modeule - RN2483 on SoftwareSerial
SoftwareSerial loraSerial = SoftwareSerial(8,7);
#define debugSerial Serial

const int time = 1000;
#define DHTPIN  2       // Pin which is connected to the DHT sensor.

//Choose your DHT sensor moddel
#define DHTTYPE DHT11
//#define DHTTYPE DHT21
//#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

ACS712 sensor(ACS712_30A, A5);

// Set your AppEUI and AppKey
const char *appEui = "70B3D57ED0012B5C";
const char *appKey = "DF778831E7BA95B2BF967619B365AE61";

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868


TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup() {
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  dht.begin();
  sensor.calibrate();
  
}

void loop() {
   
  debugSerial.println("-- LOOP");

  //float I = sensor.getCurrentDC();
  uint16_t I = sensor.getCurrentDC()  * 100;
 
  // Read sensor values and multiply by 100 to effictively have 2 decimals
  uint16_t humidity = dht.readHumidity(false) * 100;

  // false: Celsius (default)
  // true: Farenheit
  uint16_t temperature = dht.readTemperature(false) * 100;
  // false: Celsius (default)
// Split both words (16 bits) into 2 bytes of 8
  byte payload[6];
  payload[0] = highByte(humidity);
  payload[1] = lowByte(humidity);
  payload[2] = highByte(temperature);
  payload[3] = lowByte(temperature);
  payload[4] = highByte(I);
  payload[5] = lowByte(I);

   debugSerial.print("Humidity: ");
  debugSerial.println(humidity);
  debugSerial.print("Temperature: ");
  debugSerial.println(temperature);
  debugSerial.print("current: ");
  debugSerial.println(I);

  ttn.sendBytes(payload, sizeof(payload));

  delay(time);
}