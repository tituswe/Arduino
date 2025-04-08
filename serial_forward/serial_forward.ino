#define SIM7600_RX 16  // ESP32 pin connected to SIM7600 TX
#define SIM7600_TX 17  // ESP32 pin connected to SIM7600 RX
#define BAUD_RATE 115200

void setup() {
  // Initialize Serial communication with computer
  Serial.begin(BAUD_RATE);
  
  // Initialize Serial2 communication with SIM7600
  Serial2.begin(BAUD_RATE, SERIAL_8N1, SIM7600_RX, SIM7600_TX);
  
  // Wait for serial ports to initialize
  delay(1000);
  
  Serial.println("ESP32 <-> SIM7600 Command Relay");
  Serial.println("Type AT commands in the Serial Monitor");
  Serial.println("-------------------------------------");
}

void loop() {
  // Forward data: Serial Monitor -> SIM7600
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    if (command == "XXX") {
      Serial2.write(0x1A);
      Serial2.println();
    } else {
      Serial2.println(command);
    }
    Serial.print(">> ");
    Serial.println(command);
  }
  
  // Forward data: SIM7600 -> Serial Monitor
  if (Serial2.available()) {
    String response = "";
    // Allow some time for the complete response
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) {
      if (Serial2.available()) {
        char c = Serial2.read();
        response += c;
        startTime = millis(); // Reset timeout on new data
      }
    }
    
    if (response.length() > 0) {
      Serial.print("<< ");
      Serial.println(response);
    }
  }
}

