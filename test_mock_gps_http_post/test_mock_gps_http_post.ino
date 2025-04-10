#include "Waveshare_SIM7600.h"

int POWERKEY = 2;
char response[1024];
char phone_number[] = "+6594305337";
const char* testURL = "https://sheepdog-intent-kindly.ngrok-free.app/http/ping";

void setup() {
	Serial.begin(115200);
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);

  Serial.println("test_mock_gps_http_post");
}

void loop() {
  char message[100] = {0};
  float lat, log;
  if (sim7600.GetGPSStub(&lat, &log)) {
    sprintf(message, "{\"latitude\":%.6f,\"longitude\":%.6f,\"contact_number\":\"%s\"}", lat, log, phone_number);
    if (sim7600.HTTPPost(testURL, message, response, sizeof(response))) {
      Serial.println(response);
    } else {
      Serial.println("Failed to send HTTP POST");
    }
  }
  delay(500);
}