#pragma once

#include "esphome/core/component.h"
#include "esphome/components/select/select.h"
#include "esphome/components/gree/gree.h"

namespace esphome::gree {

class GreeVerticalSwingSelect final : public select::Select, public Component, public Parented<GreeClimate> {
 public:
  GreeVerticalSwingSelect() = default;

  void setup() override;

 protected:
  void control(size_t index) override;
};

}  // namespace esphome::gree
