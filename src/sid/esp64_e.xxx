//#include "go.h"

#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <WiFi.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "webpages.h"
#include "Teensy64.h"

extern "C" {
#include "emuapi.h"
#include "iopins.h"
}

#include "esp_event.h"

#include "keyboard_osd.h"

//#ifdef HAS_SND
//#include "AudioPlaySystem.h"
//#endif

#include "c64.h"
//#include "cpu.h"

//#include "ili9341_t3dma.h"
//ILI9341_t3DMA tft = ILI9341_t3DMA(PIN_NUM_CS, PIN_NUM_DC, -1, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_MISO, TPIN_NUM_CS, TPIN_NUM_IRQ);

//#include "display.h"
//Display
Display display = Display();
//#include "cpu.h"
//#define BUFFER_SIZE 256
//unsigned char samples[BUFFER_SIZE];
volatile unsigned long vicSpeed = 0;
volatile unsigned long sidSpeed = 0;


#include <soc/rtc_wdt.h>
#include <soc/rtc.h>


hw_timer_t * timer = NULL;
#define AUDIO_PIN 18

volatile int sampleN = 0;
#include "AudioPlaySID.h"
#include <esp_task_wdt.h>


const String default_httpuser = "admin";
const String default_httppassword = "admin";
const int default_webserverporthttp = 80;

// configuration structure
struct Config {
  String httpuser;           // username to access web admin
  String httppassword;       // password to access web admin
  int webserverporthttp;     // http port number for web admin
};

#define FIRMWARE_VERSION "v0.0.1"
Config config;                        // configuration
bool shouldReboot = false;            // schedule a reboot
AsyncWebServer *server;               // initialise webserver

void startWebServer() {
  // configure web server
  Serial.println("Loading Configuration ...");
  config.httpuser = default_httpuser;
  config.httppassword = default_httppassword;
  config.webserverporthttp = default_webserverporthttp;

  Serial.println("Configuring Webserver ...");
  server = new AsyncWebServer(config.webserverporthttp);
  configureWebServer();

  // startup web server
  Serial.println("Starting Webserver ...");
  server->begin();
  Serial.println("Webserver started!");
}

// function defaults
String listFiles(bool ishtml = false);

void rebootESP(String message) {
  Serial.print("Rebooting ESP32: "); Serial.println(message);
  ESP.restart();
}

// list all of the files, if ishtml=true, return html rather than simple text
String listFiles(bool ishtml) {
  String returnText = "";
  Serial.println("Listing files stored on SPIFFS");
  File root = SPIFFS.open("/");
  File foundfile = root.openNextFile();
  if (ishtml) {
    returnText += "<table><tr><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>";
  }
  while (foundfile) {
    if (ishtml) {
      returnText += "<tr align='left'><td>" + String(foundfile.name()) + "</td><td>" + humanReadableSize(foundfile.size()) + "</td>";
      returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String(foundfile.name()) + "\', \'download\')\">Download</button>";
      returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String(foundfile.name()) + "\', \'delete\')\">Delete</button></tr>";
    } else {
      returnText += "File: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
    }
    foundfile = root.openNextFile();
  }
  if (ishtml) {
    returnText += "</table>";
  }
  root.close();
  foundfile.close();
  return returnText;
}

// Make size of files human readable
// source: https://github.com/CelliesProjects/minimalUploadAuthESP32
String humanReadableSize(const size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}


void setupOTA() {
  ArduinoOTA
  .onStart([]() {
    Serial.println("OTA START");
    timerStop(timer);
    timerAlarmDisable(timer);
    display.end();
    Serial.println("DISPLAY END");
    //    free(cpu.RAM);
    //    Serial.println("RAM FREE");
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setHostname("ESP32-C64_SIMULATOR");
  ArduinoOTA.begin();
}

void setupWiFi_by_manager() {
  WiFiManager manager;
  bool success = manager.autoConnect("ESP32_AP", "password");
  if (!success) {
    Serial.println("Failed to connect");
  }
  else {
    Serial.println("Connected");
    Serial.println("\n\nNetwork Configuration:");
    Serial.println("----------------------");
    Serial.print("         SSID: "); Serial.println(WiFi.SSID());
    Serial.print("  Wifi Status: "); Serial.println(WiFi.status());
    Serial.print("Wifi Strength: "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
    Serial.print("          MAC: "); Serial.println(WiFi.macAddress());
    Serial.print("           IP: "); Serial.println(WiFi.localIP());
    Serial.print("       Subnet: "); Serial.println(WiFi.subnetMask());
    Serial.print("      Gateway: "); Serial.println(WiFi.gatewayIP());
    Serial.print("        DNS 1: "); Serial.println(WiFi.dnsIP(0));
    Serial.print("        DNS 2: "); Serial.println(WiFi.dnsIP(1));
    Serial.print("        DNS 3: "); Serial.println(WiFi.dnsIP(2));
    Serial.println();
  }
}


//#include <esp32-hal.h>
//#include <Arduino.h>
//
//void coreSound(void * pvParameters) {
//  esp_task_wdt_init(30, false);
//  while (true) {
//
//    rtc_wdt_feed();
//    long start = micros();
//
//    AudioPlaySID playSid = c64_PlaySid();
//
//    cycle_count delta_t = playSid.csdelta;
//    //   if (playSid.sidptr) {
//    playSid.sidptr->clock(delta_t);
//    //    playSid.sidptr->clock();
//
//    //   ledcWrite(0, (unsigned char)();
//
//    ledcWrite(0, (unsigned char)((playSid.sidptr->output() >> 8) + 127));
//    sampleN++;
//    //       if (sampleN%1000==0) {
//    yield();
//    vTaskDelay(0);
//    esp_task_wdt_reset();
//    //       }
//    long end = micros();
//
//    delayMicroseconds(max((int)((1000000 / SAMPLERATE) - (end - start)), 0));
//  }
//}



long sids = 0;
void IRAM_ATTR isrSound() {
  AudioPlaySID playSid = c64_PlaySid();
  cycle_count delta_t = playSid.csdelta;
  playSid.sidptr->clock(delta_t);
  ledcWrite(0, (unsigned char)((playSid.sidptr->output() >> 8) + 127));
  sidSpeed++;
  if (sidSpeed % 10000 == 0) {
    int freq = (1000.0 / (millis() - sids)) * 10000;
    sids = millis();
    Serial.printf("sidSpeed %d freq %d\n", vicSpeed, freq);
  }
}

void setupSoundTimer(void * pvParameters) {
  timer = timerBegin(0, 80, true);                //Begin timer with 1 MHz frequency (80MHz/80)
  timerAttachInterrupt(timer, &isrSound, true);   //Attach the interrupt to Timer1
  unsigned int timerFactor = 1000000 / SAMPLERATE; //Calculate the time interval between two readings, or more accurately, the number of cycles between two readings
  timerAlarmWrite(timer, timerFactor, true);      //Initialize the timer
  timerAlarmEnable(timer);
  vTaskDelete(NULL); //  at the end of the function to gracefully end the task
}

void setupSound() {
  ledcSetup(0, 2000000, 8);  // 625000 khz is as fast as we go w 7 bits
  ledcAttachPin(AUDIO_PIN, 0);
  ledcWrite(0, 0);
  TaskHandle_t th;
  xTaskCreatePinnedToCore(&setupSoundTimer, "soundThread", 4096, NULL, 0, &th, 0);
}



void setup(void)
{
  Serial.begin(115200);
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);
  setupWiFi_by_manager();
  setupOTA();
  display.begin();
  c64_Init();
  setupSound();
  startWebServer();
  logFreeHeap();
  emu_InitJoysticks(); // this is for enabling analog joy // CIA 1 addr 53260 53261
}
long start = 0;

void loop(void)
{
  //  while(true) {
  //    rtc_wdt_feed();
  ArduinoOTA.handle();


  for (int n = 0; n < 100; n++) {
    c64_Step();
    vicSpeed++;

    if (vicSpeed % 10000 == 0) {
      int freq = (1000.0 / (millis() - start)) * 10000;
      start = millis();
      Serial.printf("vicSpeed %d freq %d\n", vicSpeed, freq);
    }
  }
}

void logFreeHeap() {
#ifdef use_lib_log_serial
  Serial.printf("FREE HEAP: %d BYTES\n", ESP.getFreeHeap());
#endif
}




// parses and processes webpages
// if the webpage has %SOMETHING% or %SOMETHINGELSE% it will replace those strings with the ones defined
String processor(const String& var) {
  if (var == "FIRMWARE") {
    return FIRMWARE_VERSION;
  }

  if (var == "FREESPIFFS") {
    return humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes()));
  }

  if (var == "USEDSPIFFS") {
    return humanReadableSize(SPIFFS.usedBytes());
  }

  if (var == "TOTALSPIFFS") {
    return humanReadableSize(SPIFFS.totalBytes());
  }
}

void configureWebServer() {
  // configure web server

  // if url isn't found
  server->onNotFound(notFound);

  // run handleUpload function when any file is uploaded
  server->onFileUpload(handleUpload);

  // visiting this page will cause you to be logged out
  server->on("/logout", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->requestAuthentication();
    request->send(401);
  });

  // presents a "you are now logged out webpage
  server->on("/logged-out", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);
    request->send_P(401, "text/html", logout_html, processor);
  });

  server->on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("incoming request");
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();

    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      request->send_P(200, "text/html", index_html, processor);
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }

  });

  server->on("/type", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("incoming request");
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();
    //    if (checkUserWebAuth(request)) {
    logmessage += " Auth: Success";
    Serial.println(logmessage);
    //      int paramsNr = request->params();
    //      Serial.println(paramsNr);

    if (request->hasParam("text"))
    {
      String message = request->getParam("text")->value();
      message.toUpperCase();
      //          String stringOne = "A string";
      //          Serial.println(message);
      char Buf[message.length()];
      message.toCharArray(Buf, message.length() + 1);
      //          cmd = message;
      //          cr=0;
      c64_Type(&Buf[0]);
    }



    request->send_P(200, "text/html", "");


  });

  server->on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();

    if (checkUserWebAuth(request)) {
      request->send(200, "text/html");
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      //      shouldReboot = true;
      c64_Init();
      //      ESP.restart();

    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });

  server->on("/listfiles", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      request->send(200, "text/plain", listFiles(true));
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });

  server->on("/file", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);

      if (request->hasParam("name") && request->hasParam("action")) {
        const char *fileName = request->getParam("name")->value().c_str();
        const char *fileAction = request->getParam("action")->value().c_str();

        logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url() + "?name=" + String(fileName) + "&action=" + String(fileAction);

        if (!SPIFFS.exists(fileName)) {
          Serial.println(logmessage + " ERROR: file does not exist");
          request->send(400, "text/plain", "ERROR: file does not exist");
        } else {
          Serial.println(logmessage + " file exists");
          if (strcmp(fileAction, "download") == 0) {
            logmessage += " downloaded";
            request->send(SPIFFS, fileName, "application/octet-stream");
          } else if (strcmp(fileAction, "delete") == 0) {
            logmessage += " deleted";
            SPIFFS.remove(fileName);
            request->send(200, "text/plain", "Deleted File: " + String(fileName));
          } else {
            logmessage += " ERROR: invalid action param supplied";
            request->send(400, "text/plain", "ERROR: invalid action param supplied");
          }
          Serial.println(logmessage);
        }
      } else {
        request->send(400, "text/plain", "ERROR: name and action params required");
      }
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });
}

void notFound(AsyncWebServerRequest * request) {
  String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
  Serial.println(logmessage);
  request->send(404, "text/plain", "Not found");
}

// used by server.on functions to discern whether a user has the correct httpapitoken OR is authenticated by username and password
bool checkUserWebAuth(AsyncWebServerRequest * request) {
  bool isAuthenticated = false;
  Serial.println("authenticating");
  if (request->authenticate(config.httpuser.c_str(), config.httppassword.c_str())) {
    Serial.println("is authenticated via username and password");
    isAuthenticated = true;
  }
  return isAuthenticated;
}

// handles uploads to the filserver
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  // make sure authenticated before allowing upload
  if (checkUserWebAuth(request)) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);

    if (!index) {
      logmessage = "Upload Start: " + String(filename);
      // open the file on first call and store the file handle in the request object
      //      request->_tempFile = SPIFFS.open("/" + filename, "w");
      Serial.println(logmessage);
    }

    if (len) {
      // stream the incoming chunk to the opened file
      //      request->_tempFile.write(data, len);
      logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
      Serial.println(logmessage);
      for (int n = 0; n < len; n++) {


        // srart is 2049, first 2 in prg are address 16 bit to which upload/ usually 2049
        if (n + index < 2) continue;


        cpu.RAM[2047 + n + index] = data[n];
        rtc_wdt_feed();
      }

    }

    if (final) {
      logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
      // close the file handle as the upload is now done
      //      request->_tempFile.close();
      Serial.println(logmessage);
      //      cmd=String("run\r");
      //      cr=0;
      //      request->redirect("/");
    }
  } else {
    Serial.println("Auth: Failed");
    return request->requestAuthentication();
  }
}
