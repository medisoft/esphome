#pragma once

#include "esphome/core/defines.h"
#include "esphome/core/component.h"

namespace esphome {
namespace esp_now {
class ESPNow : public Component {
 public:
  ESPNow();
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_channel(uint8_t channel);
  uint8_t get_channel() const;

 protected:
  uint32_t last_update_{0};
  uint8_t channel_{1};  // Valor predeterminado, canal 1
};
}  // namespace esp_now
}  // namespace esphome
