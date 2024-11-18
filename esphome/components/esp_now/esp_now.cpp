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

#include <utility>
#include "esphome/core/log.h"
#include "esphome/core/defines.h"
#include "esphome/core/automation.h"
#include "esphome/core/base_automation.h"
#include "esphome/core/component.h"

#include "esp_now.h"
namespace esphome {
namespace esp_now {
static const char *TAG = "esp_now.component";
bool processingData = false;
#ifdef ARDUINO_ARCH_ESP8266
static void ICACHE_FLASH_ATTR receivecallback(uint8_t *bssid, uint8_t *data, uint8_t len);
// static void ICACHE_FLASH_ATTR sendcallback(uint8_t *bssid, uint8_t status);
#endif
#ifdef ARDUINO_ARCH_ESP32
static void ICACHE_FLASH_ATTR receivecallback(const uint8_t *bssid, const uint8_t *data, int len);
// static void ICACHE_FLASH_ATTR sendcallback(const uint8_t *bssid, esp_now_send_status_t status);
#endif

ESPNow::ESPNow() {}

void ESPNow::setup() {
  ESP_LOGV(TAG, "enter setup()");

#ifdef ARDUINO_ARCH_ESP8266

  uint8_t mode = wifi_get_opmode();
  if ((mode & WIFI_STA) != WIFI_STA) {
    ESP_LOGI(TAG, "Initializing Wifi STA...");

    ETS_UART_INTR_DISABLE();
    bool ret = wifi_set_opmode_current(static_cast<WiFiMode_t>(mode | WIFI_STA));
    ETS_UART_INTR_ENABLE();

    if (!ret) {
      ESP_LOGE(TAG, "wifi_set_opmode_current(...) failed!");
      this->mark_failed();
      return;
    }

    delay(10);

    if (channel_ > 0) {
      wifi_promiscuous_enable(true);
      if (!wifi_set_channel(channel_)) {
        ESP_LOGE(TAG, "wifi_set_channel(...) failed!");
        this->mark_failed();
        return;
      }
      wifi_promiscuous_enable(false);
    }
  }

  if (esp_now_init()) {
    ESP_LOGE(TAG, "esp_now_init() failed!");
    this->mark_failed();
    return;
  }
  ESP_LOGI(TAG, "esp_now initialized");
  if (esp_now_register_recv_cb(receivecallback)) {
    ESP_LOGE(TAG, "esp_now_register_recv_cb(...) failed!");
    this->mark_failed();
    return;
  }
  // if (esp_now_register_send_cb(sendcallback)) {
  //   ESP_LOGE(TAG, "esp_now_register_send_cb(...) failed!");
  //   this->mark_failed();
  //   return;
  // }
  if (esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER)) {
    ESP_LOGE(TAG, "esp_now_set_self_role(...) failed()!");
    this->mark_failed();
    return;
  }
  // if (aeskey_) {
  //   auto aeskey = aeskey_->data();
  //   if (esp_now_set_kok(aeskey, 16)) {
  //     ESP_LOGE(TAG, "esp_now_set_kok(...) failed!");
  //     this->mark_failed();
  //     return;
  //   }
  // }
  // for (auto peer : this->peers_) {
  //   auto bssid = (uint8_t *) peer->get_bssid().data();
  //   if (peer->get_aeskey()) {
  //     auto aeskey = (uint8_t *) peer->get_aeskey()->data();
  //     if (esp_now_add_peer(bssid, ESP_NOW_ROLE_CONTROLLER, 0, aeskey, 16)) {
  //       ESP_LOGE(TAG, "esp_now_add_peer(...) failed!");
  //       this->mark_failed();
  //       return;
  //     }
  //   } else {
  //     if (esp_now_add_peer(bssid, ESP_NOW_ROLE_CONTROLLER, 0, nullptr, 0)) {
  //       ESP_LOGE(TAG, "esp_now_add_peer(...) failed!");
  //       this->mark_failed();
  //       return;
  //     }
  //   }
  // }

#endif
#ifdef ARDUINO_ARCH_ESP32
  esp_err_t error;

  wifi_mode_t mode = WiFi.getMode();
  if ((mode & WIFI_MODE_STA) != WIFI_MODE_STA) {
    ESP_LOGI(TAG, "Initializing Wifi STA...");

    if (!WiFi.mode(WIFI_MODE_STA)) {
      ESP_LOGE(TAG, "WiFi.mode(...) failed!");
      this->mark_failed();
      return;
    }

    if (channel_ > 0) {
      error = esp_wifi_set_promiscuous(true);
      if (error != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_promiscuous(...) failed with %s!", esp_err_to_name(error));
        this->mark_failed();
        return;
      }

      error = esp_wifi_set_channel(channel_, WIFI_SECOND_CHAN_NONE);
      if (error != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_channel(...) failed with %s!", esp_err_to_name(error));
        this->mark_failed();
        return;
      }

      error = esp_wifi_set_promiscuous(false);
      if (error != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_promiscuous(...) failed with %s!", esp_err_to_name(error));
        this->mark_failed();
        return;
      }
    }
  }

  error = esp_now_init();
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "esp_now_init() failed with %s!", esp_err_to_name(error));
    this->mark_failed();
    return;
  }

  error = esp_now_register_recv_cb(receivecallback);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "esp_now_register_recv_cb(...) failed with %s!", esp_err_to_name(error));
    this->mark_failed();
    return;
  }

  error = esp_now_register_send_cb(sendcallback);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "esp_now_register_send_cb(...) failed with %s!", esp_err_to_name(error));
    this->mark_failed();
    return;
  }

  if (aeskey_) {
    auto aeskey = aeskey_->data();
    error = esp_now_set_pmk(aeskey);
    if (error != ESP_OK) {
      ESP_LOGE(TAG, "esp_now_set_pmk(...) failed with %s!", esp_err_to_name(error));
      this->mark_failed();
      return;
    }
  }

  for (auto peer : this->peers_) {
    esp_now_peer_info_t info;
    std::fill((uint8_t *) &info, ((uint8_t *) &info) + sizeof(info), 0);
    std::copy(peer->get_bssid().cbegin(), peer->get_bssid().cend(), info.peer_addr);
    info.channel = 2;
    info.ifidx = WIFI_IF_STA;
    if (peer->get_aeskey()) {
      std::copy(peer->get_aeskey()->cbegin(), peer->get_aeskey()->cend(), info.lmk);
      info.encrypt = true;
    }
    error = esp_now_add_peer(&info);
    if (error != ESP_OK) {
      ESP_LOGE(TAG, "esp_now_add_peer(...) failed with %s!", esp_err_to_name(error));
      this->mark_failed();
      return;
    }
  }
#endif

  ESP_LOGV(TAG, "exit setup()");
}

void ESPNow::loop() {
  const uint32_t now = millis();
  if (now - this->last_update_ < 5e3)
    return;
  // ESP_LOGW(TAG, "Ejecutando loop %d", this->last_update_);
  ESPNow::ping();
  this->last_update_ = millis();
  // Lógica continua
}

void ESPNow::dump_config() { ESP_LOGCONFIG(TAG, "ESP Now:"); }
void ESPNow::set_channel(uint8_t channel) { this->channel_ = channel; }

void ESPNow::ping() {
  ESP_LOGI(TAG, "Enviando PING");
  command_message commandData;
  esp_now_peer_info_t broadcastPeer;
  memset(&broadcastPeer, 0, sizeof(broadcastPeer));
  for (int ii = 0; ii < 6; ++ii)
    broadcastPeer.peer_addr[ii] = (uint8_t) 0xff;
  broadcastPeer.channel = this->channel_;
  broadcastPeer.encrypt = 0;  // no encryption
  const esp_now_peer_info_t *peer = &broadcastPeer;
#ifdef ARDUINO_ARCH_ESP8266
  u8 *peer_addr = (u8 *) broadcastPeer.peer_addr;
#endif
#ifdef ARDUINO_ARCH_ESP32
  const uint8_t *peer_addr = broadcastPeer.peer_addr;
#endif
  commandData.msgType = ServerCommands::PING;

  if (!esp_now_is_peer_exist(peer_addr)) {
    ESP_LOGI(TAG, "Adding broadcast address as a peer");
    esp_now_add_peer(peer_addr, ESP_NOW_ROLE_SLAVE, this->channel_, NULL, 0);
  }

  commandData.msgType = ServerCommands::PING;
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *) &commandData, sizeof(commandData));
  printBroadcastResponse(result, peer);
}

void ESPNow::printBroadcastResponse(esp_err_t result, const esp_now_peer_info_t *peer) {
#ifdef ARDUINO_ARCH_ESP8266
  u8 *peer_addr = (u8 *) peer->peer_addr;
#endif
#ifdef ARDUINO_ARCH_ESP32
  const uint8_t *peer_addr = peer->peer_addr;
#endif

  ESP_LOGW(TAG, "Broadcast Status: ");
  if (result == ESP_OK) {
    ESP_LOGW(TAG, "Success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    ESP_LOGW(TAG, "ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    ESP_LOGW(TAG, "Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    ESP_LOGW(TAG, "Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    ESP_LOGW(TAG, "ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    ESP_LOGW(TAG, "Peer not found.");
    if (!esp_now_is_peer_exist(peer_addr)) {
      ESP_LOGW(TAG, "Adding peer");
#ifdef ARDUINO_ARCH_ESP8266
      esp_now_add_peer(peer_addr, ESP_NOW_ROLE_CONTROLLER, this->channel_, NULL, 0);
#endif
#ifdef ARDUINO_ARCH_ESP32
      esp_now_add_peer(peer);
#endif
    }
  } else {
    ESP_LOGW(TAG, "Not sure what happened %d", result);
  }
}

#ifdef ARDUINO_ARCH_ESP8266
void ICACHE_FLASH_ATTR receivecallback(uint8_t *bssid, uint8_t *data, uint8_t len)
#endif
#ifdef ARDUINO_ARCH_ESP32
    void ICACHE_FLASH_ATTR receivecallback(const uint8_t *bssid, const uint8_t *data, int len)
#endif
{
  // receivequeue_.emplace(bssid, data, len);
  // ESP_LOGI(TAG, "receiveCallback %s %d", data, len);

  char macStr[18];
  command_message serverCommand;
  processingData = true;
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4],
           bssid[5]);
  memcpy(&serverCommand, data, sizeof(serverCommand));
  switch (serverCommand.msgType) {
    case ServerCommands::RESTART:
      ESP_LOGI(TAG, "Restarting");
      ESP.restart();
      break;
    case ServerCommands::PING:
      ESP_LOGI(TAG, "Received PING from %s", macStr);
      // missedPongCounter = 0;
      break;
    case ServerCommands::PONG:
      ESP_LOGI(TAG, "Received PONG from %s", macStr);
      // missedPongCounter = 0;

      break;
    case ServerCommands::PREVENT_DISCOVERY:
      ESP_LOGI(TAG, "Hidding from Discovery mode");
      // configDeviceAP(1);
      // WiFi.softAPdisconnect();
      break;
    case ServerCommands::READY_FOR_DISCOVERY:
      ESP_LOGI(TAG, "Enabling Discovery mode");
      // configDeviceAP(0);
      break;
    case ServerCommands::ENABLE_OTA:
      ESP_LOGI(TAG, "OTA command received *******");
      // if (taskHandlerOTA == NULL) {
      //   struct_taskOTAParams taskOTAParams;
      //   taskOTAParams.hostName = (char *) hostName.c_str();
      //   taskOTAParams.ssid = (char *) malloc(prefs.getBytesLength("wifi_ssid") + 1);
      //   prefs.getBytes("wifi_ssid", taskOTAParams.ssid, prefs.getBytesLength("wifi_ssid") + 1);
      //   taskOTAParams.password = (char *) malloc(prefs.getBytesLength("wifi_password") + 1);
      //   prefs.getBytes("wifi_password", taskOTAParams.password, prefs.getBytesLength("wifi_password") + 1);
      //   esp_now_unregister_recv_cb();
      //   // esp_now_deinit();
      //   xTaskCreate(taskOTA, "taskOTA", 8192, (void *) &taskOTAParams, 0, &taskHandlerOTA);
      // }
      break;
    case ServerCommands::DISABLE_OTA:
      // if (taskHandlerOTA != NULL) {
      //   vTaskDelete(taskHandlerOTA);
      //   taskHandlerOTA = NULL;
      // }
      break;
    case ServerCommands::START_SENDING_DEFAULT_DATA:
      ESP_LOGI(TAG, "Someone sent its data from %s", macStr);
      break;
    default:
      ESP_LOGI(TAG, "Receiving from %s Server Command: %d", macStr, serverCommand.msgType);
  }
  processingData = false;
}
}  // namespace esp_now
}  // namespace esphome
