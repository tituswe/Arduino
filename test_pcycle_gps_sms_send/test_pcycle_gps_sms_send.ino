#include "Waveshare_SIM7600.h"
#include <esp_sleep.h> 

#define uS_TO_S_FACTOR 1000000 
#define TIME_TO_SLEEP   10

int POWERKEY = 2;
char smsc_number[] = "+6596845997";
char phone_number[] = "+6594305337";
unsigned long startTime;

void setup() {
  Serial.begin(115200);
  
  startTime = millis();
  
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);
  sim7600.ConfigureSMS(smsc_number);
  sim7600.ClearSMS();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  Serial.println("test_pcycle_gps_sms_send");

  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
}

void loop() {
  char message[50] = {0};
  float lat, log;

  if (sim7600.GetGPSStub(&lat, &log)) {
    sprintf(message, "%.6f,%.6f", lat, log);
    sim7600.SendSMS(phone_number, message);
    Serial.print("Sent SMS with coordinates: ");
    Serial.println(message);
  } else {
    Serial.println("Failed to get GPS coordinates");
  }

  unsigned long dutyCycleDuration = millis() - startTime;
  Serial.print("Duty cycle duration: ");
  Serial.print(dutyCycleDuration);
  Serial.println(" ms");
  
  Serial.println("Going to sleep now");
  delay(1000);
  esp_deep_sleep_start(); 
}