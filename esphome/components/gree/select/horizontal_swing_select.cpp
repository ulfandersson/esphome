#include "horizontal_swing_select.h"

namespace esphome::gree {

void GreeHorizontalSwingSelect::setup() {
  this->parent_->add_on_state_callback([this](climate::Climate &device) {
    uint8_t new_state = this->parent_->get_horizontal_swing();

    if (new_state != this->active_index().value_or(0)) {
      this->publish_state(new_state);
    }
  });
}
void GreeHorizontalSwingSelect::control(size_t index) {
  this->publish_state(index);
  this->parent_->set_horizontal_swing(index);
}

}  // namespace esphome::gree
