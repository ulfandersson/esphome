#include "gtest/gtest.h"
#include <fstream>
#include <cstdlib>

#include "common.h"
#include "transmit_test_data.h"

namespace esphome::gree::testing {

struct TransmitTestData {
  const char *snapshot_name;
  Model model;
  climate::ClimateMode mode;
  float target_temp;
  climate::ClimateFanMode fan_mode;
  climate::ClimateSwingMode swing_mode;
  climate::ClimatePreset preset;
  const std::vector<int32_t> *expectation;
};

// clang-format off
const TransmitTestData test_data[] = {
    // GREE_YAN model tests
    {"EXPECTED_YAN_OFF", GREE_YAN, climate::CLIMATE_MODE_OFF, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_OFF},
    {"EXPECTED_YAN_COOL_24_AUTO_OFF", GREE_YAN, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_COOL_24_AUTO_OFF},
    {"EXPECTED_YAN_HEAT_20_HIGH_VERTICAL", GREE_YAN, climate::CLIMATE_MODE_HEAT, 20.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_HEAT_20_HIGH_VERTICAL},
    {"EXPECTED_YAN_AUTO_22_MEDIUM_OFF", GREE_YAN, climate::CLIMATE_MODE_AUTO, 22.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_AUTO_22_MEDIUM_OFF},
    {"EXPECTED_YAN_DRY_26_AUTO_OFF", GREE_YAN, climate::CLIMATE_MODE_DRY, 26.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_DRY_26_AUTO_OFF},
    {"EXPECTED_YAN_FAN_ONLY_24_HIGH_OFF", GREE_YAN, climate::CLIMATE_MODE_FAN_ONLY, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_FAN_ONLY_24_HIGH_OFF},
    {"EXPECTED_YAN_COOL_16_LOW_OFF", GREE_YAN, climate::CLIMATE_MODE_COOL, 16.0f, climate::CLIMATE_FAN_LOW, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_COOL_16_LOW_OFF},
    {"EXPECTED_YAN_HEAT_30_MEDIUM_VERT", GREE_YAN, climate::CLIMATE_MODE_HEAT, 30.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_HEAT_30_MEDIUM_VERT},

    // GREE_YAC1FB9 model tests
    {"EXPECTED_YAC1FB9_OFF", GREE_YAC1FB9, climate::CLIMATE_MODE_OFF, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_OFF},
    {"EXPECTED_YAC1FB9_COOL_24_AUTO_OFF", GREE_YAC1FB9, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_COOL_24_AUTO_OFF},
    {"EXPECTED_YAC1FB9_HEAT_20_HIGH_VERTICAL", GREE_YAC1FB9, climate::CLIMATE_MODE_HEAT, 20.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_HEAT_20_HIGH_VERTICAL},
    {"EXPECTED_YAC1FB9_AUTO_22_MEDIUM_OFF", GREE_YAC1FB9, climate::CLIMATE_MODE_AUTO, 22.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_AUTO_22_MEDIUM_OFF},
    {"EXPECTED_YAC1FB9_DRY_26_AUTO_OFF", GREE_YAC1FB9, climate::CLIMATE_MODE_DRY, 26.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_DRY_26_AUTO_OFF},
    {"EXPECTED_YAC1FB9_FAN_ONLY_24_HIGH_OFF", GREE_YAC1FB9, climate::CLIMATE_MODE_FAN_ONLY, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_FAN_ONLY_24_HIGH_OFF},
    {"EXPECTED_YAC1FB9_COOL_16_LOW_OFF", GREE_YAC1FB9, climate::CLIMATE_MODE_COOL, 16.0f, climate::CLIMATE_FAN_LOW, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_COOL_16_LOW_OFF},
    {"EXPECTED_YAC1FB9_HEAT_30_MEDIUM_VERT", GREE_YAC1FB9, climate::CLIMATE_MODE_HEAT, 30.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_HEAT_30_MEDIUM_VERT},

    // GREE_YX1FF model tests
    {"EXPECTED_YX1FF_OFF", GREE_YX1FF, climate::CLIMATE_MODE_OFF, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_OFF},
    {"EXPECTED_YX1FF_COOL_24_AUTO_OFF", GREE_YX1FF, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_COOL_24_AUTO_OFF},
    {"EXPECTED_YX1FF_HEAT_20_HIGH_VERTICAL", GREE_YX1FF, climate::CLIMATE_MODE_HEAT, 20.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_HEAT_20_HIGH_VERTICAL},
    {"EXPECTED_YX1FF_AUTO_22_MEDIUM_OFF", GREE_YX1FF, climate::CLIMATE_MODE_AUTO, 22.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_AUTO_22_MEDIUM_OFF},
    {"EXPECTED_YX1FF_DRY_26_AUTO_OFF", GREE_YX1FF, climate::CLIMATE_MODE_DRY, 26.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_DRY_26_AUTO_OFF},
    {"EXPECTED_YX1FF_FAN_ONLY_24_HIGH_OFF", GREE_YX1FF, climate::CLIMATE_MODE_FAN_ONLY, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_FAN_ONLY_24_HIGH_OFF},
    {"EXPECTED_YX1FF_COOL_24_AUTO_SLEEP", GREE_YX1FF, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_SLEEP, &testing::EXPECTED_YX1FF_COOL_24_AUTO_SLEEP},
    {"EXPECTED_YX1FF_COOL_24_TURBO_OFF", GREE_YX1FF, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_COOL_24_TURBO_OFF},

    // GREE_GENERIC model tests
    {"EXPECTED_GENERIC_OFF", GREE_GENERIC, climate::CLIMATE_MODE_OFF, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_OFF},
    {"EXPECTED_GENERIC_COOL_24_AUTO_OFF", GREE_GENERIC, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_COOL_24_AUTO_OFF},
    {"EXPECTED_GENERIC_HEAT_20_HIGH_VERTICAL", GREE_GENERIC, climate::CLIMATE_MODE_HEAT, 20.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_HEAT_20_HIGH_VERTICAL},
    {"EXPECTED_GENERIC_AUTO_22_MEDIUM_OFF", GREE_GENERIC, climate::CLIMATE_MODE_AUTO, 22.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_AUTO_22_MEDIUM_OFF},
    {"EXPECTED_GENERIC_DRY_26_AUTO_OFF", GREE_GENERIC, climate::CLIMATE_MODE_DRY, 26.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_DRY_26_AUTO_OFF},
    {"EXPECTED_GENERIC_FAN_ONLY_24_HIGH_OFF", GREE_GENERIC, climate::CLIMATE_MODE_FAN_ONLY, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_FAN_ONLY_24_HIGH_OFF},
    {"EXPECTED_GENERIC_COOL_16_LOW_OFF", GREE_GENERIC, climate::CLIMATE_MODE_COOL, 16.0f, climate::CLIMATE_FAN_LOW, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_COOL_16_LOW_OFF},
    {"EXPECTED_GENERIC_HEAT_30_MEDIUM_VERT", GREE_GENERIC, climate::CLIMATE_MODE_HEAT, 30.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_HEAT_30_MEDIUM_VERT},
};
// clang-format on

TEST(GreeClimateTest, Transmit) {
  const bool update_snapshots = std::getenv("UPDATE_SNAPSHOTS") != nullptr;
  std::ofstream snapshot_file;

  if (update_snapshots) {
    snapshot_file.open("tests/components/gree/transmit_test_data.h", std::ios::trunc);
    snapshot_file << "#pragma once\n\n#include <cstdint>\n#include <vector>\n\nnamespace esphome::gree::testing {\n\n";
  }

  for (const auto &test : test_data) {
    testing::GreeTestRig rig(test.model);

    rig.climate.mode = test.mode;
    rig.climate.target_temperature = test.target_temp;
    rig.climate.fan_mode = test.fan_mode;
    rig.climate.swing_mode = test.swing_mode;
    rig.climate.preset = test.preset;

    rig.transmit();

    ASSERT_EQ(rig.mock.transmit_count, 1);
    EXPECT_EQ(rig.mock.captured_frequency, 38000);

    if (update_snapshots) {
      snapshot_file << "inline const std::vector<int32_t> " << test.snapshot_name << " = {";
      for (size_t i = 0; i < rig.mock.captured_timings.size(); i++) {
        if (i % 18 == 0)
          snapshot_file << "\n   ";
        snapshot_file << " " << rig.mock.captured_timings[i] << ",";
      }
      snapshot_file << "\n};\n\n";
    } else {
      ASSERT_EQ(rig.mock.captured_timings.size(), test.expectation->size());
      for (size_t i = 0; i < test.expectation->size(); ++i) {
        EXPECT_EQ(rig.mock.captured_timings[i], (*test.expectation)[i])
            << "Mismatch at index " << i << " for model " << static_cast<int>(test.model) << ", mode "
            << static_cast<int>(test.mode) << ", temp " << test.target_temp << ", fan "
            << static_cast<int>(test.fan_mode) << ", swing " << static_cast<int>(test.swing_mode) << ", preset "
            << static_cast<int>(test.preset);
      }
    }
  }

  if (update_snapshots) {
    snapshot_file << "}  // namespace esphome::gree::testing\n";
    snapshot_file.close();
    FAIL() << "Snapshots updated. Re-run tests without UPDATE_SNAPSHOTS to verify.";
  }
}

}  // namespace esphome::gree::testing
