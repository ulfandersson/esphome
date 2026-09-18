#include "vertical_swing_select.h"

namespace esphome::gree {

static const char *const TAG = "gree.vertical_swing";

void GreeVerticalSwingSelect::setup() {
  this->parent_->add_on_state_callback([this](climate::Climate &device) {
    uint8_t new_state = this->parent_->get_vertical_swing();

    if (new_state != this->active_index().value_or(-1)) {
      ESP_LOGI(TAG, "New state: %i", new_state);
      this->publish_state(new_state);
    }
  });
}

void GreeVerticalSwingSelect::control(size_t index) {
  this->publish_state(index);
  this->parent_->set_vertical_swing(index);
}

}  // namespace esphome::gree
