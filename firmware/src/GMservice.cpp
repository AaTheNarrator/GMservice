#include "GMservice.h"
#include <stdlib.h>
#include "FS.h"
#include "leds.h"
#include <ESPAsyncWebServer.h>

GMservice::GMservice() {
  this->state = _states::_pause;
  // this->data.reserve(9000);
  // this->res.reserve(6000);
}

bool is_digit(String s) {
  if (s.length() == 0) return false;
  for (int i = 0; i < s.length(); i++) {
    if (!isDigit(s.charAt(i))) {
      return false;
    }
  }
  return true;
}

void GMservice::modmodzap(String cadr) {
}

String two_char_digit(int d) {
  return d < 10 ? "0" + String(d) : String(d);
}


// void GMservice::process_cadr(String cadr) {
//   Serial.println("Write on cart");
//   if (!cadr.endsWith("!!!")) return;
//   if (!cadr.startsWith("SPCSPC")) return;
//   // File service_f = this->service_sd.open("/data", FILE_APPEND);
//   // File user_f = this->user_sd.open("/data", FILE_APPEND);
//   this->user_f = this->user_sd.open("/data", FILE_APPEND);

//   this->data = String(this->gps->date.year()) + "-" + two_char_digit(this->gps->date.month()) + "-" + two_char_digit(this->gps->date.day());
//   data += " " + two_char_digit(this->gps->time.hour()) + ":" + two_char_digit(this->gps->time.minute()) + ":" + two_char_digit(this->gps->time.second());
//   data += " " + (this->gps->satellites.value() != 0? String(this->gps->location.lat(),7) : "NULL");
//   data += " " + (this->gps->satellites.value() != 0? String(this->gps->location.lng(),7) : "NULL");
//   data += " " + String(this->gps->altitude.meters());
//   data += " " + String(min((int)map(analogRead(39), 400, 1024, 0, 100), 100));
//   if (this->baro->getPressure() > 300) {
//     this->last_baro = this->baro->getPressure();
//   }
//   float new_temp = this->temp_sensor->getTempCByIndex(0);
//   if (new_temp > -50.0 && new_temp != DEVICE_DISCONNECTED) {
//     this->last_temp = new_temp;
//   }
//   data += " " + String(this->last_baro);
//   data += " " + String(this->last_temp);


//   // service_f.write((uint8_t *)data.c_str(), data.length());
//   user_f.write((uint8_t *)data.c_str(), data.length());
//   int i = 0;
//   char* frame = strtok((char* )cadr.c_str(), " \t");
//   Serial.println("cadr\n-------------");
//   while (frame != NULL) {
//     frame = strtok(NULL, " \t");
//     if (String(frame) == "!!!") break;
//     String f = " " + String(frame);
//     data += f;
//     // service_f.write((uint8_t *)f.c_str(), f.length());
//     user_f.write((uint8_t *)f.c_str(), f.length());
//     if (is_digit(frame)) {
//       if (i == 0) {
// 	      this->stabilization = (bool)(atoi(frame));
//       }
//       else if (i == 1) {
// 	      this->N = atoi(frame);
// 	      Serial.print("N = ");
// 	      Serial.println(frame);
//       }
//       else if (i == 2) {
// 	      this->M = atoi(frame);
// 	      Serial.print("M = ");
// 	      Serial.println(frame);
//       }
//       else if (i == 3) {
// 	      this->L = atoi(frame);
// 	      Serial.print("L = ");
// 	      Serial.println(frame);
//       }
//       else if (i == 4) {
//         this->cadr_index = atoi(frame);
// 	      Serial.print("cadr index = ");
// 	      Serial.println(frame);
//       }
//       else if (i > 5) {
// 	      this->last_data[i - 6] = atoi(frame);
// 	      this->acc_data[i - 6] += atoi(frame);
//       }
//       i++;
//     }
//   }
//   for (int ch = 0; ch < 4096 - i; ch++) {
//     // service_f.write((uint8_t*)" 0", 2);
//     data += " 0";
//     user_f.write((uint8_t*)" 0", 2);
//   }
//   // service_f.write((uint8_t*)"\n", 1);
//   user_f.write((uint8_t*)"\n", 1);
//   data += "\n";
//   // service_f.close();
//   user_f.close();
  
//   // this->user_f = this->user_sd.open("/data", FILE_APPEND);
//   // user_f.write((uint8_t *)data.c_str(), data.length());
//   // user_f.close();

//   this->service_f = this->service_sd.open("/data", FILE_APPEND);
//   service_f.write((uint8_t *)data.c_str(), data.length());
//   service_f.close();
// }

void GMservice::read_write_cadr(){
  if(this->state != _states::_work) return;
  Serial.println("\n-----------------------------");
  Serial.println("Read and Write File");

  this->user_f = this->user_sd.open("/data", FILE_APPEND);
  this->service_f = this->service_sd.open("/data", FILE_APPEND);

  String data = String(this->gps->date.year()) + "-" + two_char_digit(this->gps->date.month()) + "-" + two_char_digit(this->gps->date.day());
  data += " " + two_char_digit(this->gps->time.hour()) + ":" + two_char_digit(this->gps->time.minute()) + ":" + two_char_digit(this->gps->time.second());
  data += " " + (this->gps->satellites.value() != 0? String(this->gps->location.lat(),7) : "NULL");
  data += " " + (this->gps->satellites.value() != 0? String(this->gps->location.lng(),7) : "NULL");
  data += " " + String(this->gps->altitude.meters());
  data += " " + String(min((int)map(analogRead(39), 400, 1024, 0, 100), 100));
  if (this->baro->getPressure() > 300) {
    this->last_baro = this->baro->getPressure();
  }
  float new_temp = this->temp_sensor->getTempCByIndex(0);
  if (new_temp > -50.0 && new_temp != DEVICE_DISCONNECTED) {
    this->last_temp = new_temp;
  }
  data += " " + String(this->last_baro);
  data += " " + String(this->last_temp);

  user_f.write((uint8_t *)data.c_str(), data.length());
  service_f.write((uint8_t *)data.c_str(), data.length());
  int is_cadr_end = 0;
  int i = 0;
  String buf = "";
  while (is_cadr_end < 3){
    if (this->state != _states::_work) return; // смена режима происходит асинхронно
    blink();
    char c = '\0';
    if (gm_serial.available() > 0){
      c = gm_serial.read();
      buf += String(c);
    } 
    if (c == ' '){
      if (i == 2) {
	      this->stabilization = (bool)(atoi(buf.c_str()));
      }
      else if (i == 4) {
	      this->N = atoi(buf.c_str());
	      Serial.print("N = ");
	      Serial.println(buf);
      }
      else if (i == 6) {
	      this->M = atoi(buf.c_str());
	      Serial.print("M = ");
	      Serial.println(buf);
      }
      else if (i == 8) {
	      this->L = atoi(buf.c_str());
	      Serial.print("L = ");
	      Serial.println(buf);
      }
      else if (i == 10) {
        buf = " " + buf;
        // this->data += buf;
        user_f.write((uint8_t*) buf.c_str(), buf.length());
        service_f.write((uint8_t*) buf.c_str(), buf.length());
        this->cadr_index = atoi(buf.c_str());
	      Serial.print("cadr index = ");
	      Serial.println(buf);
      }
      else if (i > 12) {
        // this->data += buf;
        user_f.write((uint8_t*) buf.c_str(), buf.length());
        service_f.write((uint8_t*) buf.c_str(), buf.length());
	      this->last_data[i - 12] = atoi(buf.c_str());
	      this->acc_data[i - 12] += atoi(buf.c_str());
      }
      i++;
      buf = "";
    }
    if (c == '!') {
      is_cadr_end++;
    }
  }
  for (int ch = 0; ch < 4096 - i; ch++) {
    user_f.write((uint8_t*)" 0", 2);
    service_f.write((uint8_t*)" 0", 2);
  }
  service_f.write((uint8_t*) "\n", 1);
  service_f.close();

  user_f.write((uint8_t*) "\n", 1);
  user_f.close();

  // this->data += "\n";
  // this->service_f = this->service_sd.open("/data", FILE_APPEND);
  // service_f.write((uint8_t *) this->data.c_str(), data.length());
  // service_f.close();
}


void GMservice::work() {
  Serial.print("work");
  gm_serial.write("MODMODTEST");
  gm_serial.write(0x00);
  this->state = _states::_work;
  this->download_status = false;
  
}

void GMservice::pause() {
  gm_serial.write("MODMODWAIT");
  gm_serial.write(0x00);
  this->state = _states::_pause;
}

void GMservice::mode_read() {
  gm_serial.write("MODMODWAIT");
  gm_serial.write(0x00);
  delay(1000);
  this->state = _states::_read;
}

// void GMservice::read_cadr_int() {
//   Serial.println("read_cadr_int");
//   if (this->state != _states::_work) return;
//   auto is_cadr_end = 0;
//   String res = "";
//   while (is_cadr_end < 3) {
//     if (this->state != _states::_work) return; // смена режима происходит асинхронно
//     blink();
//     char c = '\0';
//     if (gm_serial.available() > 0) c = gm_serial.read();
//     if (c == '!') {
//       is_cadr_end++;
//     }
//     res += String(c);
//     Serial.print(c);
//   }

//   res.trim();
//   if (res.endsWith("!!!")) {
//     this->process_cadr(res);
//     res = "";
//   }
// }

String GMservice::cadr(int index) {
  if (index > 5) return "error";
  index *= 700;
  String res = "[";
  for (int i = index; i < index + 699; i++) {
    if (i > 4094) break;
    res += String(this->last_data[i]) + ",";
  }
  
  if (index == 5) {
    return res + "0]";    
  }
  else {
    return res + String(this->last_data[index + 699]) + "]";  
  }
}

String GMservice::clear_acc(){
  for (int i = 0; i < 4096; i++){
    this->acc_data[i] = 0;
  }
  this->count_cadr_to_skip = this->cadr_index;
  return "ok";
}

String GMservice::getAccDataByIndex(int index){
  if(index > 6) return "error";
  index *= 700;
  String res = "[";
  
  for (int i = index; i < index + 699; i++) {
    if (i > 4094) break;
    res += String(this->acc_data[i]) + ",";
  }

  if (index == 5) {
    return res + "0]";    
  }
  else {
    return res + String(this->acc_data[index + 699]) + "]";  
  }
}

String GMservice::sys_info() {
  return (
	  "{"	  
	  "\"lat\": \"" + String(this->gps->location.lat(),7) + "\"" 
	  + ",\"long\": \"" + String(this->gps->location.lng(),7) + "\"" 
	  + ",\"alt\": \"" + String(this->gps->altitude.meters()) + "\""

	  + ",\"pressure\": " + String(this->last_baro)
	  + ",\"temp\": \"" + String(this->last_temp)+ "\""

	  + ",\"date\": \"" + String(this->gps->date.year()) + "-" + two_char_digit(this->gps->date.month()) + "-" + two_char_digit(this->gps->date.day()) + "\""
	  + ",\"time\": \"" + two_char_digit(this->gps->time.hour()) + ":" + two_char_digit(this->gps->time.minute()) + ":" + two_char_digit(this->gps->time.second()) + "\""

	  + ",\"sats\": " + String(this->gps->satellites.value())
	  + ",\"batt\": " + String(min((int)map(analogRead(39), 400, 1024, 0, 100), 100))
	  + ",\"n\": " + String(this->N)
	  + ",\"l\": " + String(this->L)
	  + ",\"m\": " + String(this->M)
	  + ",\"stabilization\": " + String(this->stabilization)
	  + ",\"mode\": " + String(this->state)
    + ",\"count_cadr\": " + String(this->cadr_index)
	  + "}"
  );
}

void GMservice::set_N(unsigned int val) {
  Serial.println("PEMPEM "+String(val)+" "+String(val)+" \0");
  gm_serial.write(("PEMPEM "+String(val)+" "+String(val)+" \0").c_str());
  gm_serial.write(0x00);
}

void GMservice::set_M(unsigned int val) {
  Serial.println("SDVSDV "+String(val)+" "+String(val)+" \0");
  gm_serial.write(("SDVSDV "+String(val)+" "+String(val)+" ").c_str());
  gm_serial.write(0x00);
}

void GMservice::set_L(unsigned int val) {
  Serial.println("TVHTVH "+String(val)+" "+String(val)+" \0");
  gm_serial.write(("TVHTVH "+String(val)+" "+String(val)+" ").c_str());
  gm_serial.write(0x00);
}

void GMservice::stabilization_on() {
  gm_serial.write("ONNOSS");
  gm_serial.write(0x00);
}

void GMservice::stabilization_off() {
  gm_serial.write("OFFOSS");
  gm_serial.write(0x00);
}
