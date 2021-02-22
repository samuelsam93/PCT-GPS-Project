#include "Adafruit_LC709203F.h"

Adafruit_LC709203F lc;

int startTime;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\nAdafruit LC709203F demo");

  if (!lc.begin()) {
    Serial.println(F("Couldnt find Adafruit LC709203F?\nMake sure a battery is plugged in!"));
    while (1) delay(10);
  }
  Serial.println(F("Found LC709203F"));
  Serial.print("Version: 0x"); Serial.println(lc.getICversion(), HEX);

//  lc.setThermistorB(3950);
//  Serial.print("Thermistor B = "); Serial.println(lc.getThermistorB());

  lc.setPackSize(LC709203F_APA_100MAH);

  startTime  = millis();
  Serial.print("Start TIME:"); Serial.println(startTime);

//  lc.setAlarmVoltage(3.8);
}

void loop() {
  Serial.print("Batt Voltage: "); Serial.println(lc.cellVoltage(), 3);
  Serial.print("Batt Percent: "); Serial.println(lc.cellPercent(), 1);
  long sinceStart = (millis() - startTime)/1000;
  float hoursSinceStart = (float) sinceStart / (float) 3600;
  Serial.print("Time since 100%: "); Serial.print(sinceStart); Serial.print(" seconds"); 
  Serial.print(" or "); Serial.print(hoursSinceStart); Serial.println(" hours");
//  Serial.print("Batt Temp: "); Serial.println(lc.getCellTemperature(), 1);
  if (lc.cellPercent() <= 1) {
    Serial.print("battery depleted at "); Serial.print(sinceStart); Serial.println(" seconds");
    Serial.print(" or "); Serial.print(hoursSinceStart); Serial.println(" hours");
    while(1) {}
  }

  delay(2000);  // dont query too often!
}
