// #ifdef ARDUINO_ARCH_ESP8266
// #include <user_interface.h>
// #include <espnow.h>
// #include <ESP8266WiFi.h>
// #include "esphome/core/application.h"
// #endif

// #ifdef ARDUINO_ARCH_ESP32
// #include <esp_wifi.h>
// #include <esp_now.h>
// #include <WiFiType.h>
// #include <WiFi.h>
// #endif
#include "esphome/core/log.h"
#include "esp_now.h"
namespace esphome {
namespace esp_now {
static const char *TAG = "esp_now.component";

// ESPNow::ESPNow() {}

void ESPNow::setup() {
  ESP_LOGI(TAG, "Componente iniciado");
  // Inicialización del hardware
}

void ESPNow::loop() {
  ESP_LOGI(TAG, "Ejecutando loop");
  // Lógica continua
}
}  // namespace esp_now
}  // namespace esphome
