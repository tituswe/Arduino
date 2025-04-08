#include "Waveshare_SIM7600.h"

int POWERKEY = 2;

void setup() {
	Serial.begin(115200);
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);

  Serial.println("test_gps");
}

void loop() {
  sim7600.GetGPS();
  delay(500);
}
