#include "Waveshare_SIM7600.h"

int POWERKEY = 2;
char smsc_number[] = "+6596845997";
char phone_number[] = "+6594305337"; 
char message[] = "Hello, world!";

void setup() {
	Serial.begin(115200);
  sim7600.begin(115200, 16, 17);
  sim7600.PowerOn(POWERKEY);
  sim7600.ConfigureSMS(smsc_number);
  sim7600.ClearSMS();

  Serial.println("test_sms_recv_send");
}

void loop() {
  char buffer[200] = {0};
  if (sim7600.ReceiveSMS(buffer)) {
    sim7600.SendSMS(phone_number, buffer);
  }
  delay(500);
}
