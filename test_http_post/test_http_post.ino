#include "Waveshare_SIM7600.h"

int POWERKEY = 2;
char response[1024];
const char* testURL = "https://sheepdog-intent-kindly.ngrok-free.app/http/ping";
const char* jsonBody = "{\"latitude\":123.456,\"longitude\":78.910,\"contact_number\":\"+6594305337\"}";

void setup() {
	Serial.begin(115200);
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);

  Serial.println("test_http_post");
}

void loop() {
  Serial.println("Performing HTTP POST request...");

  if (sim7600.HTTPPost(testURL, jsonBody, response, sizeof(response))) {
    Serial.println("HTTP POST successful. Response:");
    Serial.println(response);
  } else {
    Serial.println("HTTP POST failed.");
  }

  delay(5000);
}