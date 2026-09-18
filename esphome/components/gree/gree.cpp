#include "gree.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome::gree {

static const char *const TAG = "gree.climate";

climate::ClimateTraits GreeClimate::traits() {
  auto t = climate_ir::ClimateIR::traits();
  // ClimateIR unconditionally includes HEAT_COOL in the base mode set; remove it when heat is not supported.
  if (!this->supports_heat_) {
    auto modes = t.get_supported_modes();
    modes.erase(climate::CLIMATE_MODE_HEAT_COOL);
    t.set_supported_modes(modes);
  }
  if (this->model_ == GREE_YAG1FB3) {
    t.set_supported_presets({climate::CLIMATE_PRESET_NONE, climate::CLIMATE_PRESET_ECO});

    set_supported_custom_fan_modes({"Medium Low", "Medium High"});
  }
  return t;
}

void GreeClimate::setup() {
  climate_ir::ClimateIR::setup();

  if (this->sensor_ != nullptr) {
    this->set_interval("gree_ifeel_timer", 300000, [this]() { this->send_ifeel_ping_(true); });

    this->sensor_->add_on_state_callback([this](float value) { this->send_ifeel_ping_(false); });
  }
}

void GreeClimate::set_model(Model model) {
  if (model == GREE_YAN) {
    // YAN only has a vertical vane; the horizontal swing IR bytes are not defined for this model.
    this->swing_modes_.erase(climate::CLIMATE_SWING_HORIZONTAL);
    this->swing_modes_.erase(climate::CLIMATE_SWING_BOTH);
  }
  if (model == GREE_YX1FF) {
    this->fan_modes_.insert(climate::CLIMATE_FAN_QUIET);   // YX1FF 4 speed
    this->presets_.insert(climate::CLIMATE_PRESET_NONE);   // YX1FF sleep mode
    this->presets_.insert(climate::CLIMATE_PRESET_SLEEP);  // YX1FF sleep mode
  }
  if (model == GREE_YAG1FB3) {
    this->minimum_temperature_ = 8;
  }

  this->model_ = model;
}

void GreeClimate::set_mode_bit(uint8_t bit_mask, bool enabled) {
  if (enabled) {
    this->mode_bits_ |= bit_mask;
  } else {
    this->mode_bits_ &= ~bit_mask;
  }
  this->transmit_state();
}

uint8_t GreeClimate::get_mode_bits() { return this->mode_bits_; }

void GreeClimate::transmit_state() {
  this->set_timeout("transmit_debounce", 100, [this]() {
    ESP_LOGI(TAG, "Transmitting state");
    this->do_transmit_state();
  });
}

void GreeClimate::do_transmit_state() {
  uint8_t remote_state[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00};

  remote_state[0] = this->fan_speed_() | this->operation_mode_();
  remote_state[1] = this->temperature_();

  if (this->model_ == GREE_YAG1FB3) {
    remote_state[1] -= GREE_TEMP_MIN;

    // Replicate original remote behaviour - fan mode stuck at LOW when dehumidifying
    if (this->mode == climate::CLIMATE_MODE_DRY) {
      remote_state[0] = GREE_FAN_1 | this->operation_mode_();
    }
  }

  if (this->model_ == GREE_YAN || this->model_ == GREE_YAA || this->model_ == GREE_YAC ||
      this->model_ == GREE_YAC1FB9) {
    remote_state[2] = 0x20;  // bits 0..3 always 0000, bits 4..7 TURBO, LIGHT, HEALTH, X-FAN
  } else if (this->model_ == GREE_YX1FF || this->model_ == GREE_YAG) {
    remote_state[2] = 0x60;
  }

  if (this->model_ != GREE_GENERIC) {
    remote_state[3] = 0x50;  // bits 4..7 always 0101
  }

  if (this->model_ == GREE_YAN || this->model_ == GREE_YX1FF || this->model_ == GREE_YAG ||
      this->model_ == GREE_YAG1FB3) {
    remote_state[4] = this->get_vertical_swing();
  }

  if (this->model_ == GREE_YAG || this->model_ == GREE_YAG1FB3) {
    if (this->get_vertical_swing() == GREE_VDIR_SWING || this->get_horizontal_swing() == GREE_HDIR_SWING) {
      remote_state[0] |= (1 << 6);
    }
  }

  if (this->model_ == GREE_YAC || this->model_ == GREE_YAG || this->model_ == GREE_YAG1FB3) {
    remote_state[4] |= (this->get_horizontal_swing() << 4);
  }

  if (this->model_ == GREE_YAG) {
    remote_state[5] = 0x40;
  } else if (this->model_ == GREE_YAG1FB3) {
    // Unknown
    remote_state[5] = 0x80;
    // WiFi on
    remote_state[5] |= 0x40;
    if (this->sensor_ != nullptr) {
      // iFeel on
      remote_state[5] |= 0x04;
    }
  }

  if (this->model_ == GREE_YAA || this->model_ == GREE_YAC || this->model_ == GREE_YAC1FB9) {
    remote_state[6] = 0x20;  // YAA1FB, FAA1FB1, YB1F2 bits 4..7 always 0010

    if (this->get_vertical_swing() == GREE_VDIR_SWING) {
      remote_state[0] |= (1 << 6);  // Enable swing by setting bit 6
    } else if (this->get_vertical_swing() != GREE_VDIR_AUTO) {
      remote_state[5] = this->get_vertical_swing();
    }
  }

  if (this->model_ == GREE_YAN || this->model_ == GREE_YAA || this->model_ == GREE_YAC ||
      this->model_ == GREE_YAC1FB9 || this->model_ == GREE_YAG1FB3) {
    // Merge the mode bits into remote_state[2]
    // Clear the mode bits (bits 4-7) and OR in the current mode_bits_
    remote_state[2] = (remote_state[2] & 0x0F) | (this->mode_bits_ & 0xF0);
  }

  if (this->model_ == GREE_YX1FF) {
    if (this->fan_speed_() == GREE_FAN_TURBO) {
      remote_state[2] |= GREE_FAN_TURBO_BIT;
    }

    if (this->preset_() == GREE_PRESET_SLEEP) {
      remote_state[0] |= GREE_PRESET_SLEEP_BIT;
    }
  }

  // Calculate the checksum
  if (this->model_ == GREE_YAN || this->model_ == GREE_YX1FF) {
    remote_state[7] = ((remote_state[0] << 4) + (remote_state[1] << 4) + 0xC0);
  } else {
    remote_state[7] =
        ((((remote_state[0] & 0x0F) + (remote_state[1] & 0x0F) + (remote_state[2] & 0x0F) + (remote_state[3] & 0x0F) +
           ((remote_state[4] & 0xF0) >> 4) + ((remote_state[5] & 0xF0) >> 4) + ((remote_state[6] & 0xF0) >> 4) + 0x0A) &
          0x0F)
         << 4);
  }
  if (this->model_ == GREE_YAG1FB3) {
    if (this->preset.value() == climate::CLIMATE_PRESET_ECO && this->mode == climate::CLIMATE_MODE_COOL) {
      remote_state[7] |= 0x04;
    }
  }

  auto transmit = this->transmitter_->transmit();
  auto *data = transmit.get_data();
  data->set_carrier_frequency(GREE_IR_FREQUENCY);

  this->encodeData_(data, remote_state);

  if (this->model_ == GREE_YAG1FB3) {
    data->space(30000);

    remote_state[3] = 0x70 & (this->mode_bits_ & 0x01);
    remote_state[4] = this->quiet_bits_;
    remote_state[5] = 0x00;

    uint8_t fan_value_b2 = 0x00;
    if (this->get_custom_fan_mode() == "Medium Low") {
      fan_value_b2 = 2;
    } else if (this->get_custom_fan_mode() == "Medium High") {
      fan_value_b2 = 4;
    } else {
      switch (this->fan_mode.value()) {
        case climate::CLIMATE_FAN_LOW:
          fan_value_b2 = 1;
          break;
        case climate::CLIMATE_FAN_MEDIUM:
          fan_value_b2 = 3;
          break;
        case climate::CLIMATE_FAN_HIGH:
          fan_value_b2 = 5;
          break;
        default:
          fan_value_b2 = 0;
          break;
      }
    }

    remote_state[6] = (fan_value_b2 << 4);
    remote_state[7] =
        ((((remote_state[0] & 0x0F) + (remote_state[1] & 0x0F) + (remote_state[2] & 0x0F) + (remote_state[3] & 0x0F) +
           ((remote_state[4] & 0xF0) >> 4) + ((remote_state[5] & 0xF0) >> 4) + ((remote_state[6] & 0xF0) >> 4) + 0x0A) &
          0x0F)
         << 4);
    this->encodeData_(data, remote_state);

    data->space(30000);

    //--- SEKVENSSIGNAL 3 (Totalt 139 tidsvärden) ---
    // Header-pulser: Mark = 9048 µs, Space = -4434 µs
    // Frame A (35 bitar): ['0x00', '0x00', '0x00', '0xA0', '0x02']
    // Frame B (32 bitar): ['0x00', '0x00', '0x00', '0xA0']
    remote_state[0] = 0x00;
    remote_state[1] = 0x00;
    if (this->target_temperature < 16.0f) {
      int temp_int = static_cast<int>(this->target_temperature);

      if (temp_int >= 8 && temp_int <= 15) {
        uint8_t shift_value = (15 - temp_int) << 4;
        remote_state[2] = 0x80 - shift_value;
      }
    } else {
      remote_state[2] = 0x00;
    }
    remote_state[3] = 0xA0;
    remote_state[4] = 0x00;
    remote_state[5] = 0x00;
    remote_state[6] = 0x00;
    remote_state[7] = 0xA0;
    this->encodeData_(data, remote_state);

    uint8_t temp_val = this->get_sensor_temp_();
    if (temp_val > 0) {
      // TODO actual delay before ifeel paket
      data->space(30000);

      this->encodeIFeel_(data, temp_val);
    }
  }

  data->space(0);

  transmit.perform();
  this->last_transmission_ = millis();
}

void GreeClimate::encodeData_(remote_base::RemoteTransmitData *data, uint8_t *remote_state) {
  data->mark(GREE_HEADER_MARK);
  if (this->model_ == GREE_YAC1FB9 || this->model_ == GREE_YAG1FB3) {
    data->space(GREE_YAC1FB9_HEADER_SPACE);
  } else {
    data->space(GREE_HEADER_SPACE);
  }

  for (int i = 0; i < 4; i++) {
    for (uint8_t mask = 1; mask > 0; mask <<= 1) {  // iterate through bit mask
      data->mark(GREE_BIT_MARK);
      bool bit = remote_state[i] & mask;
      data->space(bit ? GREE_ONE_SPACE : GREE_ZERO_SPACE);
    }
  }

  data->mark(GREE_BIT_MARK);
  data->space(GREE_ZERO_SPACE);
  data->mark(GREE_BIT_MARK);
  data->space(GREE_ONE_SPACE);
  data->mark(GREE_BIT_MARK);
  data->space(GREE_ZERO_SPACE);

  data->mark(GREE_BIT_MARK);
  if (this->model_ == GREE_YAC1FB9 || this->model_ == GREE_YAG1FB3) {
    data->space(GREE_YAC1FB9_MESSAGE_SPACE);
  } else {
    data->space(GREE_MESSAGE_SPACE);
  }

  for (int i = 4; i < 8; i++) {
    for (uint8_t mask = 1; mask > 0; mask <<= 1) {  // iterate through bit mask
      data->mark(GREE_BIT_MARK);
      bool bit = remote_state[i] & mask;
      data->space(bit ? GREE_ONE_SPACE : GREE_ZERO_SPACE);
    }
  }

  data->mark(GREE_BIT_MARK);
}

uint8_t GreeClimate::operation_mode_() {
  uint8_t operating_mode = GREE_MODE_ON;

  switch (this->mode) {
    case climate::CLIMATE_MODE_COOL:
      operating_mode |= GREE_MODE_COOL;
      break;
    case climate::CLIMATE_MODE_DRY:
      operating_mode |= GREE_MODE_DRY;
      break;
    case climate::CLIMATE_MODE_HEAT:
      operating_mode |= GREE_MODE_HEAT;
      break;
    case climate::CLIMATE_MODE_HEAT_COOL:
      operating_mode |= GREE_MODE_AUTO;
      break;
    case climate::CLIMATE_MODE_FAN_ONLY:
      operating_mode |= GREE_MODE_FAN;
      break;
    case climate::CLIMATE_MODE_OFF:
    default:
      operating_mode = GREE_MODE_OFF;
      break;
  }

  return operating_mode;
}

uint8_t GreeClimate::fan_speed_() {
  if (this->model_ == GREE_YAG1FB3) {
    switch (this->fan_mode.value_or(climate::CLIMATE_FAN_ON)) {
      case climate::CLIMATE_FAN_LOW:
        return GREE_FAN_1;
      case climate::CLIMATE_FAN_MEDIUM:
        return GREE_FAN_3;
      default:
        if (this->get_custom_fan_mode() == "Medium Low") {
          return GREE_FAN_2;
        }
        if (this->get_custom_fan_mode() == "Medium High") {
          return GREE_FAN_3;
        }
        return GREE_FAN_AUTO;
    }
  }

  // YX1FF has 4 fan speeds -- we treat low as quiet and turbo as high
  if (this->model_ == GREE_YX1FF) {
    switch (this->fan_mode.value_or(climate::CLIMATE_FAN_ON)) {
      case climate::CLIMATE_FAN_QUIET:
        return GREE_FAN_1;
      case climate::CLIMATE_FAN_LOW:
        return GREE_FAN_2;
      case climate::CLIMATE_FAN_MEDIUM:
        return GREE_FAN_3;
      case climate::CLIMATE_FAN_HIGH:
        return GREE_FAN_TURBO;
      case climate::CLIMATE_FAN_AUTO:
      default:
        return GREE_FAN_AUTO;
    }
  }

  switch (this->fan_mode.value_or(climate::CLIMATE_FAN_ON)) {
    case climate::CLIMATE_FAN_LOW:
      return GREE_FAN_1;
    case climate::CLIMATE_FAN_MEDIUM:
      return GREE_FAN_2;
    case climate::CLIMATE_FAN_HIGH:
      return GREE_FAN_3;
    case climate::CLIMATE_FAN_AUTO:
    default:
      return GREE_FAN_AUTO;
  }
}

void GreeClimate::set_horizontal_swing(uint8_t horizontal_swing) {
  this->gree_horizontal_swing_ = horizontal_swing;
  this->sync_swing_mode();
  this->transmit_state();
}

void GreeClimate::set_vertical_swing(uint8_t vertical_swing) {
  this->gree_vertical_swing_ = vertical_swing;
  this->sync_swing_mode();
  this->transmit_state();
}

void GreeClimate::sync_swing_mode() {
  bool horizontal_auto = this->gree_horizontal_swing_ == 1;
  bool vertical_auto = this->gree_vertical_swing_ == 1;

  if (horizontal_auto && vertical_auto) {
    this->swing_mode = climate::CLIMATE_SWING_BOTH;
  } else if (horizontal_auto) {
    this->swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
  } else if (vertical_auto) {
    this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
  } else {
    this->swing_mode = climate::CLIMATE_SWING_OFF;
  }
  this->publish_state();
}

uint8_t GreeClimate::get_horizontal_swing() {
  switch (this->swing_mode) {
    case climate::CLIMATE_SWING_HORIZONTAL:
    case climate::CLIMATE_SWING_BOTH:
      return GREE_HDIR_SWING;
    default:
      return this->gree_horizontal_swing_;
  }
}

uint8_t GreeClimate::get_vertical_swing() {
  switch (this->swing_mode) {
    case climate::CLIMATE_SWING_VERTICAL:
    case climate::CLIMATE_SWING_BOTH:
      return GREE_VDIR_SWING;
    default:
      return this->gree_vertical_swing_;
  }
}

uint8_t GreeClimate::temperature_() {
  return (uint8_t) roundf(clamp<float>(this->target_temperature, GREE_TEMP_MIN, GREE_TEMP_MAX));
}

uint8_t GreeClimate::preset_() {
  // YX1FF has sleep preset
  if (this->model_ == GREE_YX1FF) {
    switch (this->preset.value_or(climate::CLIMATE_PRESET_NONE)) {
      case climate::CLIMATE_PRESET_NONE:
        return GREE_PRESET_NONE;
      case climate::CLIMATE_PRESET_SLEEP:
        return GREE_PRESET_SLEEP;
      default:
        return GREE_PRESET_NONE;
    }
  }

  return GREE_PRESET_NONE;
}

uint8_t GreeClimate::get_sensor_temp_() {
  if (this->sensor_ == nullptr || !this->sensor_->has_state()) {
    return 0;
  }

  float current_temp = this->sensor_->get_state();
  uint8_t temp_val = (uint8_t) roundf(current_temp);

  temp_val = clamp(temp_val, (uint8_t) GREE_TEMP_MIN, (uint8_t) GREE_TEMP_MAX);

  return temp_val;
}

void GreeClimate::send_ifeel_ping_(bool force) {
  uint8_t temp_val = this->get_sensor_temp_();

  if (temp_val == 0 || (!force && temp_val == this->last_sent_temp_)) {
    return;
  }

  ESP_LOGI("gree.climate", "Sending iFeel temperature %i", temp_val);

  this->last_sent_temp_ = temp_val;

  auto transmit = this->transmitter_->transmit();
  auto *data = transmit.get_data();
  data->set_carrier_frequency(GREE_IR_FREQUENCY);

  this->encodeIFeel_(data, temp_val);

  transmit.perform();
}

void GreeClimate::encodeIFeel_(remote_base::RemoteTransmitData *data, uint8_t temperature) {
  data->reserve(35);

  data->mark(GREE_YAC_HEADER_MARK);  // or 6100?
  data->space(GREE_YAC_HEADER_SPACE);

  for (int i = 0; i < 8; i++) {
    data->mark(GREE_YAC_BIT_MARK);  // or 680?
    if ((temperature >> i) & 1) {
      data->space(GREE_ONE_SPACE);  // or 1630?
    } else {
      data->space(GREE_ZERO_SPACE);  // or 530?
    }
  }

  uint8_t sig = 0xA5;
  for (int i = 0; i < 8; i++) {
    data->mark(GREE_YAC_BIT_MARK);
    if ((sig >> i) & 1) {
      data->space(GREE_ONE_SPACE);
    } else {
      data->space(GREE_ZERO_SPACE);
    }
  }

  data->mark(GREE_YAC_BIT_MARK);
}

bool GreeClimate::on_receive(remote_base::RemoteReceiveData data) {
  uint32_t now = millis();
  if (now - this->last_transmission_ < 700) {
    return false;
  }
  this->last_receive_ = now;

  // ==========================================================================
  // PATH 1: SEARCH FOR STANDARD GREE COMMAND (67 bits divided across two blocks)
  // ==========================================================================
  if (data.expect_item(GREE_HEADER_MARK, GREE_HEADER_SPACE) ||
      data.expect_item(GREE_HEADER_MARK, GREE_YAC1FB9_HEADER_SPACE)) {
    ESP_LOGD(TAG, "Gree Header detected! Starting climate command decoding...");

    uint8_t remote_state[8] = {0};

    // 1. Read the first 4 bytes (remote_state[0] to [3])
    for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
      for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
        if (!data.expect_mark(GREE_BIT_MARK)) {
          ESP_LOGW(TAG, "Decoding error: Missed expected data mark at byte %d, bit %d", byte_idx, bit_idx);
          return false;
        }
        if (data.expect_space(GREE_ONE_SPACE)) {
          remote_state[byte_idx] |= (1 << bit_idx);  // Set the bit to 1 (LSB-first)
        } else if (!data.expect_space(GREE_ZERO_SPACE)) {
          ESP_LOGW(TAG, "Decoding error: Invalid space length at byte %d, bit %d", byte_idx, bit_idx);
          return false;
        }
      }
    }

    // 2. Read the 3 static bits (should logically be 0, 1, 0)
    if (!data.expect_item(GREE_BIT_MARK, GREE_ZERO_SPACE) ||  // Bit 1 (0)
        !data.expect_item(GREE_BIT_MARK, GREE_ONE_SPACE) ||   // Bit 2 (1)
        !data.expect_item(GREE_BIT_MARK, GREE_ZERO_SPACE)) {  // Bit 3 (0)
      ESP_LOGW(TAG, "Decoding error: Static bits (0, 1, 0) did not match");
      return false;
    }

    // 3. Expect the long pause in the middle of the signal (~20ms)
    if (!data.expect_mark(GREE_BIT_MARK)) {
      ESP_LOGW(TAG, "Decoding error: Missed trailing mark before the message space");
      return false;
    }
    if (!data.expect_space(GREE_MESSAGE_SPACE) && !data.expect_space(GREE_YAC1FB9_MESSAGE_SPACE)) {
      ESP_LOGW(TAG, "Decoding error: Did not find the expected 20ms space between data blocks");
      return false;
    }

    // 4. Read the last 4 bytes (remote_state[4] to [7])
    for (int byte_idx = 4; byte_idx < 8; byte_idx++) {
      for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
        if (!data.expect_mark(GREE_BIT_MARK)) {
          ESP_LOGW(TAG, "Decoding error: Missed data mark in second block, byte %d, bit %d", byte_idx, bit_idx);
          return false;
        }
        if (data.expect_space(GREE_ONE_SPACE)) {
          remote_state[byte_idx] |= (1 << bit_idx);  // Set the bit to 1
        } else if (!data.expect_space(GREE_ZERO_SPACE)) {
          ESP_LOGW(TAG, "Decoding error: Invalid space length in second block, byte %d, bit %d", byte_idx, bit_idx);
          return false;
        }
      }
    }

    // 5. Verify the trailing stop pulse
    if (!data.expect_mark(GREE_BIT_MARK)) {
      ESP_LOGW(TAG, "Decoding error: Missing trailing stop pulse for the command");
      return false;
    }

    // 6. Verify the checksum (Last 4 bits in byte 7)
    // Formula: (Byte 0 + Byte 1 + Byte 2 + Byte 3 + Byte 4 + Byte 5 + Byte 6 + 0x0A) & 0x0F
    uint8_t calculated_checksum =
        ((((remote_state[0] & 0x0F) + (remote_state[1] & 0x0F) + (remote_state[2] & 0x0F) + (remote_state[3] & 0x0F) +
           ((remote_state[4] & 0xF0) >> 4) + ((remote_state[5] & 0xF0) >> 4) + ((remote_state[6] & 0xF0) >> 4) + 0x0A) &
          0x0F)
         << 4);
    uint8_t received_checksum = remote_state[7] & 0xF0;

    ESP_LOGI(TAG, "remote state 0: 0x%01X, 1: 0x%01X, 2: 0x%01X, 3: 0x%01X, 4: 0x%01X, 5: 0x%01X, 6: 0x%01X, 7: 0x%01X",
             remote_state[0], remote_state[1], remote_state[2], remote_state[3], remote_state[4], remote_state[5],
             remote_state[6], remote_state[7]);

    if (calculated_checksum != received_checksum) {
      ESP_LOGW(TAG, "Checksum error! Calculated: 0x%01X, Received: 0x%01X. Rejecting command.", calculated_checksum,
               received_checksum);
      return false;
    }

    if ((remote_state[3] & 0xF0) == 0x70) {
      ESP_LOGI(TAG, "Burst 2 - scavenge");
      this->mode_bits_ = (this->mode_bits_ & 0xF0) | (remote_state[3] & 0x0F);
      this->quiet_bits_ = remote_state[4];

      uint8_t fan_step = (remote_state[6] & 0xF0) >> 4;

      switch (fan_step) {
        case 1:
          this->set_fan_mode_(climate::CLIMATE_FAN_LOW);
          break;
        case 2:
          this->set_custom_fan_mode_("Medium Low");
          break;
        case 3:
          this->set_fan_mode_(climate::CLIMATE_FAN_MEDIUM);
          break;
        case 4:
          this->set_custom_fan_mode_("Medium High");
          break;
        case 5:
          this->set_fan_mode_(climate::CLIMATE_FAN_HIGH);
          break;
        default:
          break;
      }

    } else if (remote_state[3] == 0xA0) {
      ESP_LOGI(TAG, "Burst 3 - handle low temp");
      if (remote_state[7] == 0xA0) {
        uint8_t low_temp = (remote_state[2] & 0xF0) >> 4;
        if (low_temp >= 1 && low_temp <= 8) {
          this->target_temperature = 7 + low_temp;
        }
      }

      this->publish_state();

      return true;

    } else {
      ESP_LOGI(TAG, "Burst 1 - import state");
      this->mode_bits_ = remote_state[2] & 0xF0;
      this->import_state_(remote_state);
    }

    return true;
  }

  // ==========================================================================
  // PATH 2: SEARCH FOR ONLY IFEEL PING (16 bits: Temp + Signature 0xA5)
  // ==========================================================================
  if (data.expect_item(GREE_YAC_HEADER_MARK, GREE_YAC_HEADER_SPACE)) {
    ESP_LOGD(TAG, "iFeel Header detected! Starting ping decoding...");

    uint8_t temp_val = 0;
    uint8_t signature = 0;

    // 1. Read Byte 1 (Temperature, LSB-first)
    for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
      if (!data.expect_mark(GREE_YAC_BIT_MARK))
        return false;
      if (data.expect_space(GREE_ONE_SPACE)) {
        temp_val |= (1 << bit_idx);
      } else if (!data.expect_space(GREE_ZERO_SPACE)) {
        return false;
      }
    }

    // 2. Read Byte 2 (Signature, expected to be 0xA5)
    for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
      if (!data.expect_mark(GREE_YAC_BIT_MARK))
        return false;
      if (data.expect_space(GREE_ONE_SPACE)) {
        signature |= (1 << bit_idx);
      } else if (!data.expect_space(GREE_ZERO_SPACE)) {
        return false;
      }
    }

    // 3. Verify trailing pulse
    if (!data.expect_mark(GREE_YAC_BIT_MARK)) {
      return false;
    }

    // 4. Verify that the signature is exactly 0xA5
    if (signature != 0xA5) {
      ESP_LOGW(TAG, "iFeel decoding failed: Incorrect signature (0x%02X instead of 0xA5)", signature);
      return false;
    }

    ESP_LOGI(TAG, "Received valid iFeel ping! Room temperature reported from remote: %d°C", temp_val);

    // Optional: If you want to write the temperature to a sensor or log in Home Assistant
    return false;
  }

  // If the signal did not match any of our templates
  return false;
}

void GreeClimate::import_state_(uint8_t *remote_state) {
  bool is_on = (remote_state[0] & GREE_MODE_ON) != 0;
  uint8_t mode_bits = remote_state[0] & 0x07;

  if (!is_on) {
    this->mode = climate::CLIMATE_MODE_OFF;
  } else {
    switch (mode_bits) {
      case GREE_MODE_AUTO:
        this->mode = climate::CLIMATE_MODE_HEAT_COOL;
        break;
      case GREE_MODE_COOL:
        this->mode = climate::CLIMATE_MODE_COOL;
        break;
      case GREE_MODE_DRY:
        this->mode = climate::CLIMATE_MODE_DRY;
        break;
      case GREE_MODE_FAN:
        this->mode = climate::CLIMATE_MODE_FAN_ONLY;
        break;
      case GREE_MODE_HEAT:
        this->mode = climate::CLIMATE_MODE_HEAT;
        break;
      default:
        this->mode = climate::CLIMATE_MODE_HEAT_COOL;
        break;
    }
  }

  if (this->mode == climate::CLIMATE_MODE_COOL) {
    bool se_mode = (remote_state[7] & 0x04) != 0;
    if (se_mode) {
      this->preset = climate::CLIMATE_PRESET_ECO;
    } else {
      this->preset = climate::CLIMATE_PRESET_NONE;
    }
  }

  uint8_t temp_bits = remote_state[1] & 0x0F;
  this->target_temperature = temp_bits + GREE_TEMP_MIN;

  uint8_t fan_bits = remote_state[0] & 0x30;
  switch (fan_bits) {
    case GREE_FAN_1:
      this->set_fan_mode_(climate::CLIMATE_FAN_LOW);
      break;
    case GREE_FAN_2:
      this->set_custom_fan_mode_("Medium Low");
      break;
    case GREE_FAN_3:
      this->set_fan_mode_(climate::CLIMATE_FAN_MEDIUM);
      break;
    default:
      this->set_fan_mode_(climate::CLIMATE_FAN_AUTO);
      break;
  }

  this->gree_vertical_swing_ = (remote_state[4] >> 0) & 0x0F;
  this->gree_horizontal_swing_ = (remote_state[4] >> 4) & 0x0F;
  this->sync_swing_mode();

  ESP_LOGI(TAG, "Imported state - Mode: %d, Temp: %.1fC, Fan: %d, V Swing: %d, H Swing: %d",
           static_cast<int>(this->mode), this->target_temperature,
           static_cast<int>(this->fan_mode.value_or(climate::CLIMATE_FAN_AUTO)),
           static_cast<int>(this->gree_vertical_swing_), static_cast<int>(this->gree_horizontal_swing_));
}

}  // namespace esphome::gree
