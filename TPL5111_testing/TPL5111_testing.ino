#include <bluefruit.h>
#include <Adafruit_GPS.h>

#define GPSerial Serial1
Adafruit_GPS GPS(&GPSerial);

const int donePin = A5;
const int gpsEnablePin = 9;
bool fixFound = 0;
int startTime;

void enableGPS(void){
  digitalWrite(gpsEnablePin, HIGH);
  delay(20);

  //initialize GPS settings
  //RMC sentences only
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  //updating at 1hz possible efficiency here
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  //don't send updates on antenna status
  GPS.sendCommand(PGCMD_NOANTENNA);
}

void startLogger(void){
  while (!GPS.LOCUS_ReadStatus()) {}
  if (!GPS.LOCUS_status) {
    while (!GPS.LOCUS_StartLogger()) {}      
  }  
}
//
//bool waitForFix(int timeoutInMinutes){
//  long timeoutInMillis = timeoutInMinutes * 60000;
//  long startTime = millis();
//  while (millis() < startTime + timeoutInMillis) {
//    char c = GPS.read();
//    if (GPS.newNMEAreceived()) {
//      GPS.parse(GPS.lastNMEA());
//      if(GPS.fix) {
//        return 1;
//      }
//    }
//  }
//  return 1;
//}

void sendDone(void) {
  pinMode(A5, OUTPUT);
  digitalWrite(A5, LOW);
}

void setup() {
  pinMode(gpsEnablePin, OUTPUT);
  
  GPS.begin(9600);
  
  enableGPS();
  startLogger();
  startTime = millis();
}

void loop() {
  if (millis() < startTime + 120000 && !fixFound) {
    char c = GPS.read();
    if (GPS.newNMEAreceived()) {
      GPS.parse(GPS.lastNMEA());
      if (GPS.fix) {
        fixFound = 1;
        delay(1000);
      }
    }
  } else {
    digitalWrite(A5, HIGH);
  }
}
