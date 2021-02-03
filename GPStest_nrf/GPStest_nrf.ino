#include <SoftwareSerial.h>
#include "config.h"
#include <bluefruit.h>
#include <TinyGPS.h>

BLEUart bleuart; // uart over ble

SoftwareSerial adafruitGPSerial(11, 12);
//SoftwareSerial linx_gps(9, 7);
TinyGPS GPS;

char reading;
bool isAdvertising = 0;
//uint16_t connected_handle;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

// stuff copied from bleuart is below until next line -------------------------------------------------------------------------

#if CFG_DEBUG
  // Blocking wait for connection when debug mode is enabled via IDE
  while ( !Serial ) yield();
#endif

  Serial.println("Bluefruit52 GPS BLEUART Streaming");
  Serial.println("---------------------------\n");

  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setName("SLAMTOOTH");

  bleuart.begin();

//  startAdv();

  Serial.println("Can we track down the issue");
  Serial.println("gps info should start streaming");

// stuff I wrote is below ------------------------------------------------------------------------------------------------
  
  adafruitGPSerial.begin(9600);
  delay(2000);
  adafruitGPSerial.println(PMTK_SET_NMEA_OUTPUT_RMCONLY);

//  linx_gps.begin(9600);

  Serial.println("Testing Serial Here!");
  
}

//advertising for ble should not affect uart-----------------------------------------------------------------------------------------------------

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

//-----------------------------------------------------------------------------------------
void loop() {
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  for (unsigned long start = millis(); millis() - start < 1000;) {
    while (adafruitGPSerial.available()) {
      char c = adafruitGPSerial.read();
      if (GPS.encode(c)) newData = true;
    }
  }
  if (newData) {
    long lat, lon;
    unsigned long age;
    GPS.get_position(&lat, &lon, &age);
    Serial.println(lat);
    Serial.println(lon);
    startAdv();
    delay(10000);
    bleuart.write("lat: ");
    bleuart.write(lat);
    bleuart.write("lon: ");
    bleuart.write(lon);
  }
//  if (adafruitGPSerial.available() && !isAdvertising) {
//    reading = adafruitGPSerial.read();
//    Serial.write(reading);
//  }
//  if (Serial.available()) {
//   char c = Serial.read();
////   Serial.write(c);
////   adafruitGPSerial.write(c);
//   startAdv();
//   isAdvertising = 1;
//  }
//  if (isAdvertising) {
//    Serial.println("Advertisinggggggggg");
//    Serial.println(reading);
//    bleuart.write(reading);
//    delay(5000);
////    Bluefruit.Connection(connected_handle).disconnect();
//    Bluefruit.Advertising.stop();
//    isAdvertising = 0;
//    delay(5000);
//  }
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);
//  connected_handle = connection;

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
//  bleuart.write(reading);
//  delay(10000);
//  isAdvertising = 0;
//  Bluefruit.Advertising.stop();
}
