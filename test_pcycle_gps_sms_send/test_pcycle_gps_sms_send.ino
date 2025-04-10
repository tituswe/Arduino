#include "Waveshare_SIM7600.h"
#include <esp_sleep.h> 
#include <chrono>

#define POWERKEY      2
#define SLEEPKEY      4
#define TIME_TO_SLEEP 10

using namespace std::chrono;

void setup() {
  Serial.begin(115200);
  delay(500);
  
  pinMode(POWERKEY, OUTPUT);
  pinMode(SLEEPKEY, OUTPUT);
  
  high_resolution_clock::time_point start_time = high_resolution_clock::now();
  
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("Wake up");
  } else {
    Serial.println("First boot");
  }

  // Set DTR high to keep UART active
  digitalWrite(SLEEPKEY, HIGH);

  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);
  sim7600.ClearSMS();
  
  const char* url = "https://sheepdog-intent-kindly.ngrok-free.app/http/ping";
  char phone_number[] = "+6594305337";
  char message[200] = {0};
  float lat, log;
  if (sim7600.GetGPS(POWERKEY, &lat, &log)) {
    high_resolution_clock::time_point gps_time = high_resolution_clock::now();
    duration<double> gps_fix_time = gps_time - start_time;
    snprintf(message, sizeof(message), "{\"latitude\":%.6f,\"longitude\":%.6f,\"contact_number\":\"%s\",\"gps_fix_time\":\"%f\"}", lat, log, phone_number, gps_fix_time.count());
    if (sim7600.SendSMS(phone_number, message)) {
      Serial.println(message);
    } else {
      Serial.println("Failed to send send SMS");
    }
  }

  high_resolution_clock::time_point end_time = high_resolution_clock::now();

  duration<double> time_elapsed = end_time - start_time;
  Serial.print("Duty cycle: ");
  Serial.print(time_elapsed.count());
  Serial.println("s");
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * 1000000);
  
  digitalWrite(SLEEPKEY, LOW);
  Serial.println("Sleep");
  Serial.flush(); 
  delay(500); 
  
  esp_deep_sleep_start();
}

void loop() {
  // Empty as we're using deep sleep
}
