#include "Waveshare_SIM7600.h"
#include <chrono>

using namespace std::chrono;

int POWERKEY = 2;
char response[1024];
char phone_number[] = "+6594305337";
const char* testURL = "https://sheepdog-intent-kindly.ngrok-free.app/http/ping";
duration<double> time_span = duration<double>::zero();

void setup() {
	Serial.begin(115200);
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);

  Serial.println("test_gps_http_post_timed");
}

void loop() {
  high_resolution_clock::time_point start = high_resolution_clock::now();


  char message[200] = {0};
  float lat, log;
  if (sim7600.GetGPS(POWERKEY, &lat, &log)) {
  snprintf(message, sizeof(message), "{\"latitude\":%.6f,\"longitude\":%.6f,\"contact_number\":\"%s\",\"prev_duration\":%f}", lat, log, phone_number, time_span.count());
    if (sim7600.HTTPPost(testURL, message, response, sizeof(response))) {
      Serial.println(response);
    } else {
      Serial.println("Failed to send HTTP POST");
    }
  }
  delay(500);


  high_resolution_clock::time_point end = high_resolution_clock::now();
  time_span = end - start;
}