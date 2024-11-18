#pragma once

#include "esphome/core/defines.h"
#include "esphome/core/component.h"

namespace esphome {
namespace esp_now {
class ESPNow : public Component {
 public:
  // ESPNow();
  void setup() override;
  void loop() override;

 protected:
};
}  // namespace esp_now
}  // namespace esphome
