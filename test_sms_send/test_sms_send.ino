#include "Waveshare_SIM7600.h"

int POWERKEY = 2;
char phone_number[] = "+6594305337"; 
char message[] = "Hello, world!";

void setup() {
	Serial.begin(115200);
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);
  sim7600.ClearSMS();

  Serial.println("test_sms_send");
}

void loop() {
  sim7600.SendSMS(phone_number, message);
  delay(3000);
}
