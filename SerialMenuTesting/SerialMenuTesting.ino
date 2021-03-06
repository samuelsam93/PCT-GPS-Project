#include <bluefruit.h>
#include <Adafruit_GPS.h>

BLEUart bleuart; // uart over ble
#define GPSerial Serial1
Adafruit_GPS GPS(&GPSerial);
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>

int menu = 0;
bool isDumping = 0;
const int buttonPin = 7;
int buttonState = 0;
File dataFile;

#if defined(EXTERNAL_FLASH_USE_QSPI)
  Adafruit_FlashTransport_QSPI flashTransport;
#else
  #error No QSPI/SPI flash are defined on your board variant.h !
#endif

Adafruit_SPIFlash flash(&flashTransport);

FatFileSystem fatfs;

#define FILE_NAME      "GPSdata.txt"


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(115200);

  while(!Serial) {}  //wait until serial is ready

  if (!flash.begin()) {
    Serial.println("Error, failed to initialize flash chip!");
    while(1);
  }

  if (!fatfs.begin(&flash)) {
    Serial.println("Error, failed to mount newly formatted filesystem!");
    Serial.println("Was the flash chip formatted with the fatfs_format example?");
    while(1);
  }

  Serial.println("Mounted filesystem!");
  
  GPS.begin(9600);

  //initialize GPS settings
  //RMC sentences only
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  //logging at 1hz
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  //don't send updates on antenna status
  GPS.sendCommand(PGCMD_NOANTENNA);

  Serial.println("Bluefruit52 GPS BLEUART Streaming");
  Serial.println("---------------------------\n");

  //initialize bluefruit and BLE UART
  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setName("PCT-GPS");
  bleuart.begin();
}

//startAdv is stolen from bleuart Examples/Adafruit bluefruit nRF52 Libraries/Peripheral/bleuart
//it initializes advertising and available connection to the nRF52 board 
void startAdv(void) {
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

void streamData5Seconds(void) {
  int startTime = millis();
  while (millis() < (startTime+5000)) {
    if (GPSerial.available()) {
      char c = GPSerial.read();
      Serial.write(c);  
    }
  }
}


void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    menu = 0;
    Serial.println("BUTTON PRESS");

    if (dataFile) {
      dataFile.close();
    }

    //RMC sentences only
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  }
  if (menu == 0) {
    Serial.println("Select an option:");
    Serial.println("-----------------");
    Serial.println("1) Start logging");
    Serial.println("2) Stream data for 5 seconds");
    Serial.println("3) Stop logging");
    Serial.println("4) Print Logger Status");
    Serial.println("5) Dump Logger Contents to Serial Monitor");
    Serial.println("6) Dump Logger Contents to SPI Flash");
    

//    upcoming menu items
//    Serial.println("a) Start BLE Advertising");
//    Serial.println("b) Start BLE UART Dump");
//    Serial.println("c) Stop BLE Advertising");
    Serial.flush();

    while (!Serial.available()) {}
    menu = Serial.parseInt();
    Serial.flush();
  }
  
  if (menu == 1) {
    while (!GPS.LOCUS_StartLogger()) {}
    Serial.println("\nLOGGER STARTED\n");
    Serial.flush();
    menu = 0;
  }

  if (menu == 2) {
    Serial.println("in menu 2");
    streamData5Seconds();
    Serial.flush();
    menu = 0;
  }

  if (menu == 3) {
    while (!GPS.LOCUS_StopLogger()) {}
    Serial.println("\nLOGGER STOPPED\n");
    Serial.flush();
    menu = 0;
  }

  if (menu == 4) {
    Serial.println("in menu 4");
    while(!GPS.LOCUS_ReadStatus()) {}
    if (GPS.LOCUS_status)
      Serial.print("LOGGING, ");
    else
      Serial.print("Not logging, ");
    Serial.print("Records: "); Serial.print((int)GPS.LOCUS_records);
    Serial.print(", Storage %: "); Serial.print((int)GPS.LOCUS_percent);
    Serial.print("\n");
    Serial.flush();
    menu = 0;
  }
 
  if (menu == 5) {
    if (!isDumping) {
      Serial.println("in menu 5");
      GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);
      while(GPS.available())
        GPSerial.read();
      GPS.sendCommand("$PMTK622,1*29");
      Serial.println("------------------------------");
      isDumping = 1;
    } else {
      if (GPSerial.available()) {
        char c = GPSerial.read();
        Serial.write(c);
      }
    }
  }

  if (menu == 6) {
    if (!isDumping) {
      Serial.println("in menu 6");
      GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);
      while(GPS.available())
        GPSerial.read();
      dataFile = fatfs.open(FILE_NAME, FILE_WRITE);
      GPS.sendCommand("$PMTK622,1*29");
      Serial.println("------------------------------");
      isDumping = 1;
    } else {
      if (GPSerial.available()) {
        char c = GPSerial.read();
        Serial.write(c);
        dataFile.print(c);
      }
    }
  }
}
