#ifndef __WAVESHARE_SIM7600_H_
#define __WAVESHARE_SIM7600_H_

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

// ADDED: Default pins for ESP32 Serial2
#define SIM7600_RX_PIN 16  // Default ESP32 Serial2 RX pin
#define SIM7600_TX_PIN 17  // Default ESP32 Serial2 TX pin

class Sim7x00 {
public:
    Sim7x00();
    ~Sim7x00();
    
    // ADDED: New initialization method with pin configuration
    void begin(int baudRate = 115200, int rxPin = SIM7600_RX_PIN, int txPin = SIM7600_TX_PIN);
    
    void PowerOn(int PowerKey);
    void PhoneCall(const char* PhoneNumber);
    bool ConfigureSMS(const char* SMSCNumber);
    bool SendSMS(const char* PhoneNumber, const char* Message);
    bool ReceiveSMS();
    bool ReceiveSMS(char* RecMessage);
    bool ClearSMS();  // New method to delete all SMS messages
    void ConfigureFTP(const char* FTPServer, const char* FTPUserName, const char* FTPPassWord);
    void UploadToFTP(const char* FileName);
    void DownloadFromFTP(const char* FileName);
    bool HTTPGet(const char* url, char* response, int responseSize);
    bool GetGPS();
    bool GetGPS(float* Lat, float* Log);
    bool GetGPSStub(float* Lat, float* Log);
    uint8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout);
    char sendATcommand2(const char* ATcommand, const char* expected_answer1, const char* expected_answer2, unsigned int timeout);
};

extern Sim7x00 sim7600;

#endif
