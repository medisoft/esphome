#pragma once

#include "esphome/core/defines.h"
#include "esphome/core/component.h"

namespace esphome {
namespace esp_now {
enum ServerCommands {
  PREVENT_DISCOVERY = 0,
  READY_FOR_DISCOVERY = 1,
  RESTART = 3,
  PING = 5,
  PONG = 6,
  ENABLE_OTA = 10,
  DISABLE_OTA = 11,
  SENDME_YOUR_KEY = 20,
  RECEIVE_MY_KEY = 21,
  START_SENDING_DEFAULT_DATA = 100,
  STOP_SENDING_DEFAULT_DATA = 100,
};
typedef struct command_message {
  ServerCommands msgType;
} command_message;

#ifdef ARDUINO_ARCH_ESP8266
#define ESP_NOW_ETH_ALEN 6 /*!< Length of ESPNOW peer MAC address */
#define ESP_NOW_KEY_LEN 16 /*!< Length of ESPNOW peer local master key */
typedef int esp_err_t;
#define ESP_OK 0                                           /*!< esp_err_t value indicating success (no error) */
#define ESP_FAIL -1                                        /*!< Generic esp_err_t code indicating failure */
#define ESP_ERR_WIFI_BASE 0x3000                           /*!< Starting number of WiFi error codes */
#define ESP_ERR_ESPNOW_BASE (ESP_ERR_WIFI_BASE + 100)      /*!< ESPNOW error number base. */
#define ESP_ERR_ESPNOW_NOT_INIT (ESP_ERR_ESPNOW_BASE + 1)  /*!< ESPNOW is not initialized. */
#define ESP_ERR_ESPNOW_ARG (ESP_ERR_ESPNOW_BASE + 2)       /*!< Invalid argument */
#define ESP_ERR_ESPNOW_NO_MEM (ESP_ERR_ESPNOW_BASE + 3)    /*!< Out of memory */
#define ESP_ERR_ESPNOW_FULL (ESP_ERR_ESPNOW_BASE + 4)      /*!< ESPNOW peer list is full */
#define ESP_ERR_ESPNOW_NOT_FOUND (ESP_ERR_ESPNOW_BASE + 5) /*!< ESPNOW peer is not found */
#define ESP_ERR_ESPNOW_INTERNAL (ESP_ERR_ESPNOW_BASE + 6)  /*!< Internal error */
#define ESP_ERR_ESPNOW_EXIST (ESP_ERR_ESPNOW_BASE + 7)     /*!< ESPNOW peer has existed */
#define ESP_ERR_ESPNOW_IF (ESP_ERR_ESPNOW_BASE + 8)        /*!< Interface error */

typedef enum {
  ESP_IF_WIFI_STA = 0, /**< ESP32 station interface */
  ESP_IF_WIFI_AP,      /**< ESP32 soft-AP interface */
  ESP_IF_ETH,          /**< ESP32 ethernet interface */
  ESP_IF_MAX
} esp_interface_t;

typedef enum {
  WIFI_IF_STA = ESP_IF_WIFI_STA,
  WIFI_IF_AP = ESP_IF_WIFI_AP,
} wifi_interface_t;
typedef struct esp_now_peer_info {
  uint8_t peer_addr[ESP_NOW_ETH_ALEN]; /**< ESPNOW peer MAC address that is also the MAC address of station or softap */
  uint8_t lmk[ESP_NOW_KEY_LEN];        /**< ESPNOW peer local master key that is used to encrypt data */
  uint8_t channel;                     /**< Wi-Fi channel that peer uses to send/receive ESPNOW data. If the value is 0,
                                            use the current channel which station or softap is on. Otherwise, it must be
                                            set as the channel that station or softap is on. */
  wifi_interface_t ifidx;              /**< Wi-Fi interface that peer uses to send/receive ESPNOW data */
  bool encrypt;                        /**< ESPNOW data that this peer sends/receives is encrypted or not */
  void *priv;                          /**< ESPNOW peer private data */
} esp_now_peer_info_t;
#endif
class ESPNow : public Component {
 public:
  ESPNow();
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_channel(uint8_t channel);
  uint8_t get_channel() const;
  void ping();
  void printBroadcastResponse(esp_err_t result, const esp_now_peer_info_t *peer);

 protected:
  uint32_t last_update_{0};
  uint8_t channel_{1};  // Valor predeterminado, canal 1
  void scan_network();
};
}  // namespace esp_now
}  // namespace esphome
