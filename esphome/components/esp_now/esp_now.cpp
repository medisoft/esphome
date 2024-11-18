#ifdef ARDUINO_ARCH_ESP8266
#include <user_interface.h>
#include <espnow.h>
#include <ESP8266WiFi.h>
#include "esphome/core/application.h"
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFiType.h>
#include <WiFi.h>
#endif
#include "esphome/core/log.h"
#include "esp_now.h"
namespace esphome {
namespace esp_now {
static const char *TAG = "esp_now.component";

ESPNow::ESPNow() {}

void ESPNow::setup() {
  ESP_LOGI(TAG, "Componente iniciado");
  // Inicialización del hardware
}

void ESPNow::loop() {
  const uint32_t now = millis();
  if (now - this->last_update_ < 1000)
    return;
  ESP_LOGW(TAG, "Ejecutando loop %d", this->last_update_);
  this->last_update_ = millis();
  // Lógica continua
}

void ESPNow::dump_config() { ESP_LOGCONFIG(TAG, "ESP Now:"); }
void ESPNow::set_channel(uint8_t channel) { this->channel_ = channel; }
}  // namespace esp_now
}  // namespace esphome
