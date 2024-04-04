#include <Arduino.h>
#include "main.h"

#include "SPI.h"
#include "SPIFFS.h"
#include <TinyGPS++.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <web_api.h>

#include "GMservice.h"
#include "MS5611_SPI.h"

#include <OneWire.h>
#include <DallasTemperature.h>
#include "leds.h"

#include <ctime>

SPIClass spi = SPIClass(HSPI);
GMservice gm;
TinyGPSPlus gps;

AsyncWebServer server(80);

OneWire oneWire(18);
DallasTemperature sensor(&oneWire);

MS5611_SPI baro(25);


void setup() {
  
  pinMode(STATUS_LED, OUTPUT);
  pinMode(GNSS_RST, OUTPUT);
  pinMode(SD2_CS, OUTPUT);

  init_leds(&gm);
  blink();
  digitalWrite(GNSS_RST, HIGH);
  Serial.begin(115200, SERIAL_8N1);
  gps_serial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  gm_serial.setRxBufferSize(4096);
  gm_serial.begin(115200, SERIAL_8N1, GM_RX, GM_TX);
  spi.begin(SPI_CLK, SPI_MISO, SPI_MOSI, SD2_CS);
  
  WiFi.mode(WIFI_AP);
  // WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("GM_test_lab", "aerodyne2018");
  Serial.println(WiFi.softAPIP());


  // WiFi.begin("Aerodyne-RC", "aerodyne2018");
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print('.');
  // }
  // Serial.println(WiFi.localIP());
  
  // if(!SPIFFS.begin(true, "/fs")){
  //   Serial.println("An Error has occurred while mounting SPIFFS");
  //   return;
  // }

  digitalWrite(25, HIGH);
  digitalWrite(SD1_CS, LOW);
  digitalWrite(SD2_CS, HIGH);

  for (int i = 0; i < 10; i++) {
    if (gm.service_sd.begin(SD1_CS, spi, 40000000, "/sd1")) {
      Serial.println("SD1 oppened");
      gm.periph_state.service_sd = true;
      break;
    } else {
      Serial.println("Try to open SD1");
    }
    delay(500);
  }
  
  digitalWrite(SD1_CS, HIGH);
  digitalWrite(SD2_CS, LOW);

  for (int i = 0; i < 10; i++) {
    if (gm.user_sd.begin(SD2_CS, spi, 40000000, "/sd2")) {
      Serial.println("SD2 oppened");
      gm.periph_state.user_sd = true;
      break;
    } else {
      Serial.println("Try to open SD2");
    }
    delay(500);
  }

  baro = MS5611_SPI(25);

  if (baro.begin() == true) {
    Serial.print("MS5611 found: ");
    gm.periph_state.baro = true;
    Serial.println(baro.getDeviceID(), HEX);
  } else {
    Serial.println("MS5611 not found. halt.");
  }

  gm.gps = &gps;
  gm.temp_sensor = &sensor;
  gm.baro = &baro;

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  AsyncElegantOTA.begin(&server, "", "");
  start_api(&server, &gm, &spi);

  server.begin();
  Serial.println("start");
  // Serial.println(CONFIG_ASYNC_TCP_USE_WDT);
  gm.work();
  // disableCore0WDT();
  // disableCore1WDT();
}

uint32_t start = millis();
void loop() {
  blink();
  digitalWrite(STATUS_LED, (millis() / 100) % 2);
  // if (gm_serial.available()) gm.read_cadr_int();
  if (gm_serial.available() && gm.state == _states::_work) gm.read_write_cadr();
  while (gps_serial.available()) {
    gps.encode(gps_serial.read()); // широта/долгота
  }
  /* обновление данных барометра и температуры раз в две секунды */
  if (millis() - start > 2000) {
    baro.read();
    sensor.requestTemperatures();
    start = millis();
  }
}
