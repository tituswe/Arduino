#include "Waveshare_SIM7600.h"

int POWERKEY = 2;
char response[1024];
const char* testURL = "http://httpbin.org/get";

void setup() {
	Serial.begin(115200);
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);

  Serial.println("test_http_get");
}

void loop() {
  Serial.println("Performing HTTP GET request...");

  if (sim7600.HTTPGet(testURL, response, sizeof(response))) {
    Serial.println("HTTP GET successful. Response:");
    Serial.println(response);
  } else {
    Serial.println("HTTP GET failed.");
  }

  delay(5000);
}
