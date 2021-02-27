#include <bluefruit.h>
#include "nrfx_rtc.h"

#define NRFX_RTC_DEFAULT_CONFIG_FREQUENCY 16
#define NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY 2
#define NRFX_RTC1_ENABLED 1

nrfx_rtc_t myRTC = NRFX_RTC_INSTANCE(1);
nrfx_rtc_config_t myRTConfig = NRFX_RTC_DEFAULT_CONFIG;

nrfx_rtc_handler_t myRTChandler(nrfx_rtc_int_type_t NRFX_RTC_INT_COMPARE0){
  Serial.print("hello");
}

void setup() {
  // put your setup code here, to run once:
  nrfx_rtc_init(myRTC*, myRTConfig*, myRTChandler);

}

void loop() {
  // put your main code here, to run repeatedly:
  while(1);
}
