#include <bluefruit.h>
#include <Adafruit_GPS.h>

#define GPSerial Serial1
Adafruit_GPS GPS(&GPSerial);

bool GPS_enabled = 1;

void setup() {
  pinMode(9, OUTPUT);

  GPS.begin(9600);
  
  //initialize GPS settings
  //RMC sentences only
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  //updating at 1hz possible efficiency here
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  //don't send updates on antenna status
  GPS.sendCommand(PGCMD_NOANTENNA);
}

void loop() {
  if (GPS_enabled) {
    digitalWrite(9, HIGH);
    int enableTime = millis();

    while (!GPS.LOCUS_ReadStatus()) {}
    if (!GPS.LOCUS_status) {
      while (!GPS.LOCUS_StartLogger()) {}      
    }
    int enableCounter = 0;
    while (millis() < enableTime + 120000) {}
//      Serial.print("enabled "); Serial.print(enableCounter); Serial.println("s");
//      delay(5000);
//      enableCounter += 5;
//    }
    GPS_enabled = 0;
  } else {
    digitalWrite(9, LOW);
    int disabledTime = millis();
    int disableCounter = 0;
    while(millis() < disabledTime + 480000) {}
//      Serial.print("disabled"); Serial.print(disableCounter); Serial.println("s");
//      delay(5000);
//      disableCounter += 5;
//    }
    GPS_enabled = 1;
  }
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
}
