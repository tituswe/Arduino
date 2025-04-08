#include "Waveshare_SIM7600.h"

int POWERKEY = 2;
char smsc_number[] = "+6596845997";
char phone_number[] = "+6586508466";

void setup() {
	Serial.begin(115200);
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);
  sim7600.ConfigureSMS(smsc_number);
  sim7600.ClearSMS();

  Serial.println("test_mock_gps_sms_send");
}

void loop() {
  char message[50] = {0};
  float lat, log;
  if (sim7600.GetGPSStub(&lat, &log)) {
    sprintf(message, "%.6f,%.6f", lat, log);
    sim7600.SendSMS(phone_number, message);
    Serial.print("Sent SMS with coordinates: ");
    Serial.println(message);
  }
  delay(500);
}