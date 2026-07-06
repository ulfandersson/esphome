#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>

#include "esphome/components/gree/gree.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome::gree::testing {

// Mock RemoteTransmitterBase that records every completed transmit.
class MockRemoteTransmitterBase : public remote_base::RemoteTransmitterBase {
 public:
  MockRemoteTransmitterBase() : remote_base::RemoteTransmitterBase(nullptr) {}

  std::vector<int32_t> captured_timings;
  uint32_t captured_frequency{0};
  int transmit_count{0};

  void reset() {
    this->captured_timings.clear();
    this->captured_frequency = 0;
    this->transmit_count = 0;
  }

 protected:
  void send_internal(uint32_t /*send_times*/, uint32_t /*send_wait*/) override {
    // temp_ is the protected RemoteTransmitData buffer owned by the base class.
    // Copy its raw timings and carrier frequency out for the test to inspect.
    const auto &raw = this->temp_.get_data();
    this->captured_timings.assign(raw.begin(), raw.end());
    this->captured_frequency = this->temp_.get_carrier_frequency();
    this->transmit_count++;
  }
};

struct GreeTestRig {
  MockRemoteTransmitterBase mock;
  GreeClimate climate;

  explicit GreeTestRig(Model model) {
    this->climate.set_transmitter(&this->mock);
    this->climate.set_model(model);
    // Set safe defaults so uninitialised optionals never reach transmit_state().
    this->climate.mode = climate::CLIMATE_MODE_OFF;
    this->climate.target_temperature = 24.0f;
    this->climate.fan_mode = climate::CLIMATE_FAN_AUTO;
    this->climate.swing_mode = climate::CLIMATE_SWING_OFF;
    this->climate.preset = climate::CLIMATE_PRESET_NONE;
  }

  void transmit() {
    this->mock.reset();
    this->climate.make_call().perform();
  }
};

}  // namespace esphome::gree::testing
