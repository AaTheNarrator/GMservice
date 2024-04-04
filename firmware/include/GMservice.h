#ifndef gm_h
#define gm_h
#include "Arduino.h"
#include "vfs_api.h"
#include "sd_diskio.h"
#include "ff.h"
#include "FS.h"
#include "SD.h"
#include "TinyGPS++.h"
#include "MS5611_SPI.h"
#include <DallasTemperature.h>
#include <ESPAsyncWebServer.h>
#define gm_serial Serial2


enum _states {
  _pause = 0,
  _work = 1,
  _read = 2
};

class GMservice {
 public:
  GMservice();
  SDFS service_sd = fs::SDFS(FSImplPtr(new VFSImpl()));
  SDFS user_sd = fs::SDFS(FSImplPtr(new VFSImpl()));

  TinyGPSPlus* gps = nullptr;
  DallasTemperature* temp_sensor = nullptr;
  MS5611_SPI* baro = nullptr;

  unsigned int N = 0, M = 0, L = 0;
  unsigned int cadr_index = 0;
  unsigned int count_cadr_to_skip = 0;
  String last_cadr;
  unsigned int last_data[4096] = {};
  unsigned int acc_data[4096] = {};
  bool stabilization = false;
  float internal_temp = 0;
  _states state;
  float last_baro = 760;
  float last_temp = 0;
  
  // String data;
  // String res;
  void read_cadr_int();
  void read_cadr_encode(char);
  void process_cadr(String cadr);
  void modmodzap(String cadr);
  void pause();
  void work();
  void mode_read();
  void stabilization_on();
  void stabilization_off();
  void set_N(unsigned int val);
  void set_M(unsigned int val);
  void set_L(unsigned int val);
  void read_write_cadr();
  String cadr(int index);
  String clear_acc();
  String getAccDataByIndex(int index);
  void write_on_sd();
  File user_f;
  File service_f;
  String sys_info();

  struct {
    bool service_sd = false;
    bool user_sd = false;
    bool baro = false;
  } periph_state;
  bool download_status = false;
};
#endif
