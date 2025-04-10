/**
*  @filename   :   Waveshare_SIM7600.cpp
*  @brief      :   Implements for sim7600 library (MODIFIED FOR ESP32)
*  @author     :   Kaloha from Waveshare
*
*  Copyright (C) Waveshare     April 27 2018
*  http://www.waveshare.com  http://www.waveshare.net
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documnetation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to  whom the Software is
* furished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "Waveshare_SIM7600.h"

Sim7x00::Sim7x00(){
}

Sim7x00::~Sim7x00(){
}

// ADDED: New initialization method with pin configuration
void Sim7x00::begin(int baudRate, int rxPin, int txPin) {
  // Configure Serial2 with specified pins
  Serial2.begin(baudRate, SERIAL_8N1, rxPin, txPin);
  delay(100);
}

/**************************Power on Sim7x00**************************/
void Sim7x00::PowerOn(int PowerKey){
   uint8_t answer = 0;

  // checks if the module is started
  answer = sendATcommand("AT", "OK", 2000);
  if (answer == 0)
  {
    Serial.print("Starting up...\n");  // Keep Serial for debug output

    pinMode(PowerKey, OUTPUT);
    // power on pulse
    digitalWrite(PowerKey, HIGH);
    delay(500);
    digitalWrite(PowerKey, LOW);

    // waits for an answer from the module
    while (answer == 0) {     // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT", "OK", 2000);
      delay(1000);
    }
  }

  delay(5000);

  while ((sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0)
    delay(500);

  if (!sendATcommand("AT+CSCLK=1", "OK", 1000)) {
    Serial.println("Failed to set clock");
  }
}

/**************************Phone Calls**************************/
void Sim7x00::PhoneCall(const char* PhoneNumber) {
  char aux_str[30];

  sprintf(aux_str, "ATD%s;", PhoneNumber);
  sendATcommand(aux_str, "OK", 10000);

  delay(20000);

  // CHANGED: Serial to Serial2
  Serial2.println("AT+CHUP");            // disconnects the existing call
  Serial.print("Call disconnected\n");   // Keep Serial for debug output
}

/**************************Configure SMSC number**************************/
bool Sim7x00::ConfigureSMS(const char* SMSCNumber) {
  uint8_t answer = 0;
  char aux_string[30];

  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  sendATcommand("AT+CPMS=\"SM\",\"SM\",\"SM\"", "OK", 1000);    // selects the memory
  
  sprintf(aux_string,"AT+CSCA=\"%s\"", SMSCNumber);

  answer = sendATcommand(aux_string, "OK", 3000);    // send the SMS number
  
  if (answer == 1) {
      Serial.print("SMSC number set successfully\n");  // Keep Serial for debug output
      return true;
  } else {
      Serial.print("Error setting SMSC number\n");  // Keep Serial for debug output
      return false;
  }
}

/**************************SMS sending and receiving message **************************/
//SMS sending short message
bool Sim7x00::SendSMS(const char* PhoneNumber, const char* Message){
  uint8_t answer = 0;
  char aux_string[30];

  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  Serial.print("Sending Short Message\n");  // Keep Serial for debug output

  sprintf(aux_string,"AT+CMGS=\"%s\"", PhoneNumber);

  answer = sendATcommand(aux_string, ">", 3000);    // send the SMS number
  if (answer == 1)
  {
      // CHANGED: Serial to Serial2
      Serial.println(Message);
      Serial.write((char)26);
      Serial2.println(Message);
      Serial2.write((char)26);
      answer = sendATcommand("", "OK", 20000);
      if (answer == 1)
      {
          Serial.print("Sent successfully \n");  // Keep Serial for debug output
          return true;
      }
      else
      {
          Serial.print("error \n");  // Keep Serial for debug output
          return false;
      }
  }
  else
  {
      Serial.print(" error.\n");  // Keep Serial for debug output
      return false;
  }
}

/**************************SMS receiving short message**************************/
bool Sim7x00::ReceiveSMS(){
  uint8_t answer = 0;
  int i = 0;
  char RecMessage[200];

  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  sendATcommand("AT+CPMS=\"SM\",\"SM\",\"SM\"", "OK", 1000);    // selects the memory

  answer = sendATcommand("AT+CMGRD=1", "+CMGRD:", 2000);    // reads the first SMS

  if (answer == 1)
    {
        answer = 0;
        // CHANGED: Serial to Serial2
        while(Serial2.available() == 0);
        // this loop reads the data of the SMS
        do{
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            if(Serial2.available() > 0){
                RecMessage[i] = Serial2.read();
                i++;
                // check if the desired answer (OK) is in the response of the module
                if (strstr(RecMessage, "OK") != NULL)
                {
                    answer = 1;
                }
            }
        }while(answer == 0);    // Waits for the asnwer with time out

        Serial.println(RecMessage);  // Keep Serial for debug output
    }
    else
    {
        // Serial.print(answer);  
        // Serial.print(" error.\n");
        return false;
    }

  return true;
}

bool Sim7x00::ReceiveSMS(char* RecMessage){
  uint8_t answer = 0;
  int i = 0;

  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  sendATcommand("AT+CPMS=\"SM\",\"SM\",\"SM\"", "OK", 1000);    // selects the memory

  answer = sendATcommand("AT+CMGRD=1", "+CMGRD:", 2000);    // reads the first SMS

  if (answer == 1)
    {
        answer = 0;
        // CHANGED: Serial to Serial2
        while(Serial2.available() == 0);
        // this loop reads the data of the SMS
        do{
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            if(Serial2.available() > 0){
                RecMessage[i] = Serial2.read();
                i++;
                // check if the desired answer (OK) is in the response of the module
                if (strstr(RecMessage, "OK") != NULL)
                {
                    answer = 1;
                }
            }
        }while(answer == 0);    // Waits for the asnwer with time out

        Serial.println(RecMessage);  // Keep Serial for debug output
    }
    else
    {
        // Serial.print(answer);  
        // Serial.print(" error.\n");
        return false;
    }

  return true;
}

/**************************Delete all SMS messages**************************/
bool Sim7x00::ClearSMS() {
    uint8_t answer = 0;
    
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    sendATcommand("AT+CPMS=\"SM\",\"SM\",\"SM\"", "OK", 1000);    // selects the memory
    
    // Delete all messages
    answer = sendATcommand("AT+CMGD=1,4", "OK", 2000);    // delete all messages
    
    if (answer == 1) {
        Serial.print("All messages deleted successfully\n");  // Keep Serial for debug output
        return true;
    } else {
        Serial.print("Error deleting messages\n");  // Keep Serial for debug output
        return false;
    }
}

/**************************FTP download file to Module EFS , uploading EFS file to FTP**************************/
void Sim7x00::ConfigureFTP(const char* FTPServer,const char* FTPUserName,const char* FTPPassWord){
    char aux_str[50];

    // sets the paremeters for the FTP server
  sendATcommand("AT+CFTPPORT=21", "OK", 2000);
  sendATcommand("AT+CFTPMODE=1", "OK", 2000);
  sendATcommand("AT+CFTPTYPE=A", "OK", 2000);

    sprintf(aux_str,"AT+CFTPSERV=\"%s\"", FTPServer);
    sendATcommand(aux_str, "OK", 2000);

    sprintf(aux_str, "AT+CFTPUN=\"%s\"", FTPUserName);
    sendATcommand(aux_str, "OK", 2000);
    sprintf(aux_str, "AT+CFTPPW=\"%s\"", FTPPassWord);
    sendATcommand(aux_str, "OK", 2000);
}

void Sim7x00::UploadToFTP(const char* FileName){
  char aux_str[50];

  Serial.print("Upload file to FTP...\n");  // Keep Serial for debug output
  sprintf(aux_str, "AT+CFTPPUTFILE=\"%s\",0", FileName);
  sendATcommand(aux_str, "OK", 2000);
}

void Sim7x00::DownloadFromFTP(const char* FileName){
  char aux_str[50];

  Serial.print("Download file from FTP...\n");  // Keep Serial for debug output
  sprintf(aux_str, "AT+CFTPGETFILE=\"%s\",0", FileName);
  sendATcommand(aux_str, "OK", 2000);
}

/**************************HTTP GET request**************************/
bool Sim7x00::HTTPGet(const char* url, char* response, int responseSize) {
    if (!sendATcommand("AT+HTTPINIT", "OK", 1000)) {
        Serial.println("Failed to initialize HTTP session");
        return false;
    }

    char commandBuffer[100];
    sprintf(commandBuffer, "AT+HTTPPARA=\"URL\",\"%s\"", url);
    if (!sendATcommand(commandBuffer, "OK", 2000)) {
        Serial.println("Failed to set HTTP parameters");
        sendATcommand("AT+HTTPTERM", "OK", 2000);
        return false;
    }

    if (!sendATcommand("AT+HTTPACTION=0", "+HTTPACTION: 0,", 5000)) {
        Serial.println("Failed to send HTTP GET request");
        sendATcommand("AT+HTTPTERM", "OK", 2000);
        return false;
    }

    if (sendATcommand("AT+HTTPREAD=0,10000", "+HTTPREAD: ", 10000)) {
        int i = 0;
        unsigned long timeout = millis() + 5000;
        while (millis() < timeout && i < responseSize - 1) {
            if (Serial2.available()) {
                char c = Serial2.read();
                response[i++] = c;
            }
        }
        response[i] = '\0';
    }

    if (!sendATcommand("AT+HTTPTERM", "OK", 2000)) {
        Serial.println("Failed to terminate HTTP session");
        return false;
    }

    return true;
}

/**************************HTTP POST request**************************/
bool Sim7x00::HTTPPost(const char* url, const char* jsonBody, char* response, int responseSize) {
    if (!sendATcommand("AT+HTTPINIT", "OK", 1000)) {
        Serial.println("Failed to initialize HTTP session");
        return false;
    }

    char commandBuffer[100];
    sprintf(commandBuffer, "AT+HTTPPARA=\"URL\",\"%s\"", url);
    if (!sendATcommand(commandBuffer, "OK", 2000)) {
        Serial.println("Failed to set HTTP parameters");
        sendATcommand("AT+HTTPTERM", "OK", 2000);
        return false;
    }

    if (!sendATcommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", "OK", 2000)) {
        Serial.println("Failed to set content type");
        sendATcommand("AT+HTTPTERM", "OK", 2000);
        return false;
    }

    int bodyLength = strlen(jsonBody);
    sprintf(commandBuffer, "AT+HTTPDATA=%d,%d", bodyLength, 10000);
    if (!sendATcommand(commandBuffer, "DOWNLOAD", 2000)) {
        Serial.println("Failed to set HTTP data length");
        sendATcommand("AT+HTTPTERM", "OK", 2000);
        return false;
    }

    Serial2.print(jsonBody);
    Serial2.println();

    if (!sendATcommand("AT+HTTPACTION=1", "+HTTPACTION: 1,", 5000)) {
        Serial.println("Failed to send HTTP POST request");
        sendATcommand("AT+HTTPTERM", "OK", 2000);
        return false;
    }

    if (sendATcommand("AT+HTTPREAD=0,10000", "+HTTPREAD: ", 10000)) {
        int i = 0;
        unsigned long timeout = millis() + 5000;
        while (millis() < timeout && i < responseSize - 1) {
            if (Serial2.available()) {
                char c = Serial2.read();
                response[i++] = c;
            }
        }
        response[i] = '\0';
    }

    if (!sendATcommand("AT+HTTPTERM", "OK", 2000)) {
        Serial.println("Failed to terminate HTTP session");
        return false;
    }

    return true;
}

/**************************GPS positoning**************************/
bool Sim7x00::GetGPS(int PowerKey){

    uint8_t answer = 0;
    bool RecNull = true;
    int i = 0;
    char RecMessage[200] = {0};
    char LatDD[3] = {0};
    char LatMM[10] = {0};
    char LogDD[4] = {0};
    char LogMM[10] = {0};
    char DdMmYy[7] = {0};
    char UTCTime[7] = {0};
    int DayMonthYear;
    float Lat = 0;
    float Log = 0;

    Serial.print("Start GPS session...\n");  // Keep Serial for debug output
    sendATcommand("AT+CGPS=1,2", "OK", 1000);    // start GPS session, standalone mode

    delay(2000);

    while(RecNull)
    {
        pinMode(PowerKey, OUTPUT);
        digitalWrite(PowerKey, HIGH);
        answer = sendATcommand("AT+CGPSINFO", "+CGPSINFO: ", 1000);    // start GPS session, standalone mode

        if (answer == 1) {
            answer = 0;
            // CHANGED: Serial to Serial2
            while(Serial2.available() == 0);
            // this loop reads the data of the GPS
            do{
                // if there are data in the UART input buffer, reads it and checks for the asnwer
                if(Serial2.available() > 0){
                    RecMessage[i] = Serial2.read();
                    i++;
                    // check if the desired answer (OK) is in the response of the module
                    if (strstr(RecMessage, "OK") != NULL)
                    {
                        answer = 1;
                    }
                }
            }while(answer == 0);    // Waits for the asnwer with time out

            RecMessage[i] = '\0';
            Serial.print(RecMessage);  // Keep Serial for debug output
            Serial.print("\n");

            if (strstr(RecMessage, ",,,,,,,,") != NULL)
            {
                memset(RecMessage, '\0', 200);    // Initialize the string
                i = 0;
                answer = 0;
                delay(1000);
            }
            else
            {
                RecNull = false;
                sendATcommand("AT+CGPS=0", "OK:", 1000);
            }
        } else {
            Serial.print("error \n");  // Keep Serial for debug output
            digitalWrite(PowerKey, LOW);
            return false;
        }
        digitalWrite(PowerKey, LOW);
        delay(2000);
    }

    // Rest of the GPS parsing code remains the same
    strncpy(LatDD,RecMessage,2);
    LatDD[2] = '\0';

    strncpy(LatMM,RecMessage+2,9);
    LatMM[9] = '\0';

    Lat = atoi(LatDD) + (atof(LatMM)/60);
    if(RecMessage[12] == 'N')
    {
        Serial.print("Latitude is ");  // Keep Serial for debug output
        Serial.print(Lat);
        Serial.print(" N\n");
      }
    else if(RecMessage[12] == 'S')
    {
        Serial.print("Latitude is ");  // Keep Serial for debug output
        Serial.print(Lat);
        Serial.print(" S\n");
      }
    else
        return false;

    strncpy(LogDD,RecMessage+14,3);
    LogDD[3] = '\0';

    strncpy(LogMM,RecMessage+17,9);
    LogMM[9] = '\0';

    Log = atoi(LogDD) + (atof(LogMM)/60);
    if(RecMessage[27] == 'E')
    {
        Serial.print("Longitude is ");  // Keep Serial for debug output
        Serial.print(Log);
        Serial.print(" E\n");
      }
    else if(RecMessage[27] == 'W')
    {
        Serial.print("Latitude is ");  // Keep Serial for debug output
        Serial.print(Lat);
        Serial.print(" W\n");
      }
    else
        return false;

    strncpy(DdMmYy,RecMessage+29,6);
    DdMmYy[6] = '\0';
    Serial.print("Day Month Year is ");  // Keep Serial for debug output
    Serial.print(DdMmYy);
    Serial.print("\n");

    strncpy(UTCTime,RecMessage+36,6);
    UTCTime[6] = '\0';
    Serial.print("UTC time is ");  // Keep Serial for debug output
    Serial.print(UTCTime);
    Serial.print("\n");

    return true;
}

bool Sim7x00::GetGPS(int PowerKey, float* Lat, float* Log){

    uint8_t answer = 0;
    bool RecNull = true;
    int i = 0;
    char RecMessage[200] = {0};
    char LatDD[3] = {0};
    char LatMM[10] = {0};
    char LogDD[4] = {0};
    char LogMM[10] = {0};
    char DdMmYy[7] = {0};
    char UTCTime[7] = {0};
    int DayMonthYear;

    Serial.print("Start GPS session...\n");  // Keep Serial for debug output
    sendATcommand("AT+CGPS=1", "OK", 1000);    // start GPS session, standalone mode

    delay(2000);

    while(RecNull)
    {
        pinMode(PowerKey, OUTPUT);
        digitalWrite(PowerKey, HIGH);
        answer = sendATcommand("AT+CGPSINFO", "+CGPSINFO: ", 1000);    // start GPS session, standalone mode

        if (answer == 1)
        {
            answer = 0;
            // CHANGED: Serial to Serial2
            while(Serial2.available() == 0);
            // this loop reads the data of the GPS
            do{
                // if there are data in the UART input buffer, reads it and checks for the asnwer
                if(Serial2.available() > 0){
                    RecMessage[i] = Serial2.read();
                    i++;
                    // check if the desired answer (OK) is in the response of the module
                    if (strstr(RecMessage, "OK") != NULL)
                    {
                        answer = 1;
                    }
                }
            }while(answer == 0);    // Waits for the asnwer with time out

            RecMessage[i] = '\0';
            Serial.print(RecMessage);  // Keep Serial for debug output
            Serial.print("\n");

            if (strstr(RecMessage, ",,,,,,,,") != NULL)
            {
                memset(RecMessage, '\0', 200);    // Initialize the string
                i = 0;
                answer = 0;
                delay(1000);
            }
            else
            {
                RecNull = false;
                sendATcommand("AT+CGPS=0", "OK:", 1000);
            }
        }
        else
        {
            Serial.print("error \n");  // Keep Serial for debug output
            digitalWrite(PowerKey, LOW);
            return false;
        }
        digitalWrite(PowerKey, LOW);
        delay(2000);

    }

    // Rest of the GPS parsing code remains the same
    strncpy(LatDD,RecMessage,2);
    LatDD[2] = '\0';

    strncpy(LatMM,RecMessage+2,9);
    LatMM[9] = '\0';

    *Lat = atoi(LatDD) + (atof(LatMM)/60);
    if(RecMessage[12] == 'N')
    {
        Serial.print("Latitude is ");  // Keep Serial for debug output
        Serial.print(*Lat);
        Serial.print(" N\n");
      }
    else if(RecMessage[12] == 'S')
    {
        Serial.print("Latitude is ");  // Keep Serial for debug output
        Serial.print(*Lat);
        Serial.print(" S\n");
      }
    else
        return false;

    strncpy(LogDD,RecMessage+14,3);
    LogDD[3] = '\0';

    strncpy(LogMM,RecMessage+17,9);
    LogMM[9] = '\0';

    *Log = atoi(LogDD) + (atof(LogMM)/60);
    if(RecMessage[27] == 'E')
    {
        Serial.print("Longitude is ");  // Keep Serial for debug output
        Serial.print(*Log);
        Serial.print(" E\n");
      }
    else if(RecMessage[27] == 'W')
    {
        Serial.print("Longitude is ");  // Keep Serial for debug output
        Serial.print(*Log);
        Serial.print(" W\n");
      }
    else
        return false;

    strncpy(DdMmYy,RecMessage+29,6);
    DdMmYy[6] = '\0';
    Serial.print("Day Month Year is ");  // Keep Serial for debug output
    Serial.print(DdMmYy);
    Serial.print("\n");

    strncpy(UTCTime,RecMessage+36,6);
    UTCTime[6] = '\0';
    Serial.print("UTC time is ");  // Keep Serial for debug output
    Serial.print(UTCTime);
    Serial.print("\n");

    return true;
}

bool Sim7x00::GetGPSStub(float* Lat, float* Log) {
  // Generate random latitude between -90 and 90
  *Lat = random(-90000000, 90000000) / 1000000.0;
  
  // Generate random longitude between -180 and 180
  *Log = random(-180000000, 180000000) / 1000000.0;
  
  Serial.print("Generated coordinates: ");
  Serial.print(*Lat, 6);
  Serial.print(", ");
  Serial.println(*Log, 6);

  delay(2000);

  return true;
}

/**************************Other functions**************************/
uint8_t Sim7x00::sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout) {

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string

    delay(100);

    // CHANGED: Serial to Serial2
    while(Serial2.available() > 0) Serial2.read();    // Clean the input buffer

    Serial2.println(ATcommand);    // Send the AT command

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // CHANGED: Serial to Serial2
        if(Serial2.available() != 0){
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = Serial2.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer) != NULL)
            {
                answer = 1;
            }
        }
         // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));

    return answer;
}

char Sim7x00::sendATcommand2(const char* ATcommand, const char* expected_answer1, const char* expected_answer2, unsigned int timeout){
  uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string

    delay(100);

    // CHANGED: Serial to Serial2
    while(Serial2.available() > 0) Serial2.read();    // Clean the input buffer

    Serial2.println(ATcommand);    // Send the AT command

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        // CHANGED: Serial to Serial2
        if(Serial2.available() != 0){
            response[x] = Serial2.read();
            Serial.print(response[x]);  // Keep Serial for debug output
            x++;
            // check if the desired answer 1 is in the response of the module
            if (strstr(response, expected_answer1) != NULL)
            {
                Serial.print("\n");  // Keep Serial for debug output
                answer = 1;
            }
            // check if the desired answer 2 is in the response of the module
            else if (strstr(response, expected_answer2) != NULL)
            {
                Serial.print("\n");  // Keep Serial for debug output
                answer = 2;
            }
        }
    }
    // Waits for the asnwer with time out
    while((answer == 0) && ((millis() - previous) < timeout));

    return answer;
}

Sim7x00 sim7600 = Sim7x00();
