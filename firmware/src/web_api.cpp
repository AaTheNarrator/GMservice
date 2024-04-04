#include <web_api.h>
#include "GMservice.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include "SPIFFS.h"
#include <SD.h>

void start_api(AsyncWebServer* server, GMservice* gm, SPIClass * spi) {
  server->on("/ping", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "pong");
  });

  server->on("/api/v1/cadr", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", gm->cadr(std::atoi(request->getParam(0)->value().c_str())));
  });

  server->on("/api/v1/acc", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", gm->getAccDataByIndex(std::atoi(request->getParam(0)->value().c_str())));
  });

  // server->on("/api/v1/avg", HTTP_GET, [gm](AsyncWebServerRequest *request){
  //   request->send(200, "text/plain", gm->avg_in_interval(std::atoi(request->getParam(0)->value().c_str())));
  // });

  server->on("/api/v1/clearGraphics", HTTP_GET, [gm](AsyncWebServerRequest *request){
    request->send(200, "text/plain", gm->clear_acc());
  });

  server->on("/api/v1/N", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    gm->set_N(atoi(request->getParam(0)->value().c_str()));
    request->send(200, "text/plain", "ok");
  });

  server->on("/api/v1/L", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    gm->set_L(atoi(request->getParam(0)->value().c_str()));
    request->send(200, "text/plain", "ok");
  });

  server->on("/api/v1/M", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    gm->set_M(atoi(request->getParam(0)->value().c_str()));
    request->send(200, "text/plain", "ok");
  });

  server->on("/api/v1/work", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    gm->work();
    request->send(200, "text/plain", "ok");
  });

  server->on("/api/v1/pause", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    gm->pause();
    request->send(200, "text/plain", "ok");
  });

  server->on("/api/v1/delete", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    gm->mode_read();
    if (!gm->download_status) {
      request->send(418, "text/plain", "Файл не скачан");
    } else if (gm->user_sd.remove("/data") && gm->service_sd.remove("/data")) {
      request->send(200, "text/plain", "Файл был успешно удален");
    } else {
      request->send(418, "text/plain", "Неудачная попытка удаления");
    }
  });

  server->on("/api/v1/stabilization-on", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    gm->stabilization_on();
    request->send(200, "text/plain", "ok");
  });

  server->on("/api/v1/stabilization-off", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    gm->stabilization_off();
    request->send(200, "text/plain", "ok");
  });

  server->on("/api/v1/sys-info", HTTP_GET, [gm](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", gm->sys_info());
  });

  server->on("/api/v1/countCadr", HTTP_GET, [gm](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(gm->cadr_index - gm->count_cadr_to_skip));
  });

  server->on("/download", HTTP_GET, [gm](AsyncWebServerRequest *request){
    gm->mode_read();
    gm->user_f.close();
    Serial.println("DOWNLOAD");
    request->send(gm->user_sd, "/data", "application/octet-stream");
    gm->download_status = true;
  });

  server->on("/download2", HTTP_GET, [gm, spi](AsyncWebServerRequest *request){
    gm->mode_read();

    int num = atoi(request->getParam(0)->value().c_str());

    if (num == 0) {
      gm->user_f.close();
      gm->service_f.close();
      delay(10);
    }

    Serial.print("DOWNLOAD2 ");
    Serial.println(num);
    File f1 = gm->user_sd.open("/data", FILE_READ); 
    File f2 = gm->user_sd.open("/data_chunk", FILE_WRITE);
    f1.seek(num * 100000);
    for (int i = 0; i < 10 ; i++){
      uint8_t buf[10000];
      int bytesRead = f1.read(buf, sizeof(buf));
      if (bytesRead > 0) {
        f2.write(buf, bytesRead);
      } else {
        request->send(200, "text/plain", "");
        f2.close();
        f1.close();
        gm->download_status = true;
        return;
      }
    }    
    f2.close();
    f1.close();
    delay(50);
    request->send(gm->user_sd, "/data_chunk", "application/octet-stream");

  });

  server->on("/api/v1/file_size", HTTP_GET, [gm](AsyncWebServerRequest *request){
    File f = gm->user_sd.open("/data", FILE_READ);
    size_t size = f.size();
    request->send(200, "text/plain", String(size));
  });

  server->on("/", HTTP_GET, [gm](AsyncWebServerRequest *request){
    AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", conf_html_gz, conf_html_gz_len); //
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    
  });

  server->on("/graph", HTTP_GET, [gm, server](AsyncWebServerRequest *request){
    AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", deleteAfterTest_html_gz, deleteAfterTest_html_gz_len); //
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });
}




