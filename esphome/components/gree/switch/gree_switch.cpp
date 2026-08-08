#include "gree_switch.h"
#include "esphome/core/log.h"

namespace esphome::gree {

ESPHOME_LOG_TAG(TAG, "gree.switch");

void GreeModeBitSwitch::setup() {
  auto initial = this->get_initial_state_with_restore_mode();
  if (initial.has_value()) {
    this->write_state(*initial);
  }
  this->parent_->add_on_state_callback([this](climate::Climate &device) {
    bool new_state = this->parent_->get_mode_bits() & this->bit_mask_;

    ESP_LOGI(TAG, "New state %s: %i", this->name_, new_state);

    if (new_state != this->state) {
      this->state = new_state;
      this->publish_state(new_state);
    }
  });
}

void GreeModeBitSwitch::dump_config() { log_switch(TAG, "  ", this->name_, this); }

void GreeModeBitSwitch::write_state(bool state) {
  this->parent_->set_mode_bit(this->bit_mask_, state);
  this->publish_state(state);
}

}  // namespace esphome::gree
