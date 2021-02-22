#include <bluefruit.h>
#include <Adafruit_GPS.h>

#define GPSerial Serial1
Adafruit_GPS GPS(&GPSerial);

bool GPS_enabled = 1;
bool fixFound = 0;
int totalFixesSinceStart = 0;
int initTime;

#define ONE_MINUTE          60000
#define TWO_MINUTES         120000
#define FIVE_MINUTES        300000
#define TEN_MINUTES         600000
#define TWENTY_MINUTES      1200000
#define THIRTY_MINUTES      1800000
#define FIFTY_EIGHT_MINUTES 3480000 
#define ONE_HOUR            3600000
#define TWO_HOURS           7200000


void setup() {
  initTime = millis();
  pinMode(9, OUTPUT);

  GPS.begin(9600);
  
  //initialize GPS settings
  //RMC sentences only
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  //updating at 1hz possible efficiency here
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  //don't send updates on antenna status
  GPS.sendCommand(PGCMD_NOANTENNA);
}

void enableGPS(void){
  digitalWrite(9, HIGH);
  delay(20);

  //initialize GPS settings
  //RMC sentences only
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  //updating at 1hz possible efficiency here
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  //don't send updates on antenna status
  GPS.sendCommand(PGCMD_NOANTENNA);
}

void disableGPS(void){
  digitalWrite(9, LOW);
}

void startLogger(void){
  while (!GPS.LOCUS_ReadStatus()) {}
  if (!GPS.LOCUS_status) {
    while (!GPS.LOCUS_StartLogger()) {}      
  }  
}

bool waitForFix(int timeoutInMinutes, bool logToSerial){
  long startTime = millis();
  long timeoutInMillis = timeoutInMinutes * 60000;
  int prevTime = millis();
  while (millis() < startTime + timeoutInMillis) {
    char c = GPS.read();
    if (GPS.newNMEAreceived()) {
      if (logToSerial){
        Serial.print("New NMEA sentence recieved!");
      }
      GPS.parse(GPS.lastNMEA());
      if (logToSerial){
        Serial.print("FIX? "); Serial.println(GPS.fix);
      }
      if(GPS.fix) {
        totalFixesSinceStart += 1;
        if(logToSerial){
          Serial.print("Fix Found, leaving loop. Total Fixes Since Start: "); Serial.println(totalFixesSinceStart);
          Serial.print("Total Time Since Start: "); Serial.println(millis()-initTime);
        }
        return 1;
      }
    }
    if (logToSerial && millis() > prevTime + 2000){
      Serial.print("enabled "); Serial.print((millis()-startTime)/1000); Serial.println("s");
      prevTime = millis();
    }
  }
  Serial.print("NO FIX, leaving loop. Total Fixes Since Start: "); Serial.println(totalFixesSinceStart);
  Serial.print("Total Time Since Start: "); Serial.println(millis()-initTime);
  return 0;
}

void lowPowerWait(bool foundFix, bool logToSerial) {
  disableGPS();
  long startTime = millis();
  if (!foundFix) {
    while(millis() < startTime + ONE_HOUR) {
//      if (logToSerial) {
//        Serial.print("NO FIX; disabled "); Serial.print((millis()-startTime)/1000); Serial.println("s");
//        delay(5000);
//      }
    }
  } else {
    while(millis() < startTime + ONE_HOUR) {
//      if (logToSerial) {
//        Serial.print("FIX FOUND; disabled "); Serial.print((millis()-startTime)/1000); Serial.println("s");
//        delay(5000);
//      }
    }
  }
}

void loop() {
  if (GPS_enabled) {
    enableGPS();
    startLogger();
    fixFound = waitForFix(2, false);

    GPS_enabled = 0;
  } else {
    lowPowerWait(fixFound, false);
    GPS_enabled = 1;
  }
}








// old code dump
// --------------------------------------------------------------------------------------------------
//    int enableCounter = 0;
//    while (millis() < enableTime + FIVE_MINUTES && GPS_enabled) {
//      char c = GPS.read();
//      if (GPS.newNMEAreceived()) {
//        GPS.parse(GPS.lastNMEA());
////        Serial.print("FIX? "); Serial.println(GPS.fix);
//        if (GPS.fix) {
//          fixFound = 1;
//          delay(5000);
//          GPS_enabled = 0;
//        }
//      }
////      Serial.print("enabled "); Serial.print((millis()-enableTime)/1000); Serial.println("s");
//      delay(5000);
//    }
////      Serial.print("enabled "); Serial.print(enableCounter); Serial.println("s");
////      delay(5000);
////      enableCounter += 5;
////    }

//------------------------------------------------------------------------------------------------------

//    disableGPS();
//    int disabledTime = millis();
//    int disableCounter = 0;
//    if (fixFound) {
//      while(millis() < disabledTime + ONE_HOUR) {
//  //      Serial.print("disabled "); Serial.print(disableCounter); Serial.println("s");
//  //      delay(5000);
//  //      disableCounter += 5;
//      }
//    } else {
//      while(millis() < disabledTime + TWENTY_MINUTES) {}
//    }

//-----------------------------------------------------------------------------------------------------

//  if (millis() > (toggleTime+30000)) {
//    digitalWrite(9, !digitalRead(9));
//    toggleTime = millis();
//    Serial.println("Toggled GPS Enable");
////    while(!GPS.LOCUS_ReadStatus()) {}
////    Serial.println(GPS.LOCUS_percent);
////    if (GPS.LOCUS_percent > 90) {
////      //here will be dumping to QSPI flash
////      //for now:
////    }
//  }
