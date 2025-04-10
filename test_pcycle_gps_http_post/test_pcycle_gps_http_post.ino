#include "Waveshare_SIM7600.h"
#include <esp_sleep.h> 

#define uS_TO_S_FACTOR 1000000 
#define TIME_TO_SLEEP  10 // in seconds

int POWERKEY = 2;
char phone_number[] = "+6594305337";
unsigned long startTime;

void setup() {
  Serial.begin(115200);
  
  startTime = millis();
  
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);
  sim7600.ClearSMS();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  Serial.println("test_pcycle_gps_http_post");

  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
}

void loop() {
  char message[50] = {0};
  float lat, log;

  if (sim7600.GetGPS(POWERKEY, &lat, &log)) {
    sprintf(message, "{\"latitude\":%.6f,\"longitude\":%.6f,\"contact_number\":\"%s\"}", lat, log, phone_number);
    if (sim7600.HTTPPost(testURL, message, response, sizeof(response))) {
      Serial.println(response);
    } else {
      Serial.println("Failed to send HTTP POST");
    }
  }

  unsigned long dutyCycleDuration = millis() - startTime;
  Serial.print("Duty cycle duration: ");
  Serial.print(dutyCycleDuration);
  Serial.println(" ms");
  
  Serial.println("Going to sleep now");
  delay(1000);
  esp_deep_sleep_start(); 
}