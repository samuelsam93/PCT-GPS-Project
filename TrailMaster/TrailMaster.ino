#include <bluefruit.h>
#include <Adafruit_GPS.h>

#define GPSerial Serial1
#define donePin 12
#define gpsEnablePin 9

//times in millis
#define ONE_MINUTE          60000
#define TWO_MINUTES         120000
#define FIVE_MINUTES        300000
#define TEN_MINUTES         600000
#define TWENTY_MINUTES      1200000
#define THIRTY_MINUTES      1800000
#define FIFTY_EIGHT_MINUTES 3480000 
#define ONE_HOUR            3600000
#define TWO_HOURS           7200000

int timeOut = TWO_MINUTES;

Adafruit_GPS GPS(&GPSerial);

bool fixFound = 0;
int killTime;

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

void disableGPS(void) {
  digitalWrite(gpsEnablePin, LOW);
}

void startLogger(void){
  while (!GPS.LOCUS_ReadStatus()) {}
  if (!GPS.LOCUS_status) {
    while (!GPS.LOCUS_StartLogger()) {}      
  }  
}

void sendDone(void) {
  pinMode(donePin, OUTPUT);
  delay(20);
  digitalWrite(donePin, HIGH);
}

void setup() {
  pinMode(gpsEnablePin, OUTPUT);
  
  GPS.begin(9600);
  
  enableGPS();
  startLogger();
  killTime = millis() + timeOut;
}

void loop() {
  if (millis() < killTime && !fixFound) {
    char c = GPS.read();
    if (GPS.newNMEAreceived()) {
      GPS.parse(GPS.lastNMEA());
      if (GPS.fix) {
        fixFound = 1;
        delay(1000);
      }
    }
  } else {
    disableGPS();
    sendDone();
  }
}
