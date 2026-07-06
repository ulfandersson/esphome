#include "gtest/gtest.h"

#include "common.h"
#include "transmit_test_data.h"

namespace esphome {
namespace gree {

// Test data for each model, covering a range of mode/temperature/fan combinations.
// Tests are ordered by increasing complexity to make debugging easier.
struct TransmitTestData {
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
    {GREE_YAN, climate::CLIMATE_MODE_OFF, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_OFF},
    {GREE_YAN, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_COOL_24_AUTO_OFF},
    {GREE_YAN, climate::CLIMATE_MODE_HEAT, 20.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_HEAT_20_HIGH_VERTICAL},
    {GREE_YAN, climate::CLIMATE_MODE_AUTO, 22.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_AUTO_22_MEDIUM_OFF},
    {GREE_YAN, climate::CLIMATE_MODE_DRY, 26.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_DRY_26_AUTO_OFF},
    {GREE_YAN, climate::CLIMATE_MODE_FAN_ONLY, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_FAN_ONLY_24_HIGH_OFF},
    {GREE_YAN, climate::CLIMATE_MODE_COOL, 16.0f, climate::CLIMATE_FAN_LOW, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_COOL_16_LOW_OFF},
    {GREE_YAN, climate::CLIMATE_MODE_HEAT, 30.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAN_HEAT_30_MEDIUM_VERT},

    // GREE_YAC1FB9 model tests
    {GREE_YAC1FB9, climate::CLIMATE_MODE_OFF, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_OFF},
    {GREE_YAC1FB9, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_COOL_24_AUTO_OFF},
    {GREE_YAC1FB9, climate::CLIMATE_MODE_HEAT, 20.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_HEAT_20_HIGH_VERTICAL},
    {GREE_YAC1FB9, climate::CLIMATE_MODE_AUTO, 22.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_AUTO_22_MEDIUM_OFF},
    {GREE_YAC1FB9, climate::CLIMATE_MODE_DRY, 26.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_DRY_26_AUTO_OFF},
    {GREE_YAC1FB9, climate::CLIMATE_MODE_FAN_ONLY, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_FAN_ONLY_24_HIGH_OFF},
    {GREE_YAC1FB9, climate::CLIMATE_MODE_COOL, 16.0f, climate::CLIMATE_FAN_LOW, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_COOL_16_LOW_OFF},
    {GREE_YAC1FB9, climate::CLIMATE_MODE_HEAT, 30.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YAC1FB9_HEAT_30_MEDIUM_VERT},

    // GREE_YX1FF model tests (with TURBO and SLEEP presets)
    {GREE_YX1FF, climate::CLIMATE_MODE_OFF, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_OFF},
    {GREE_YX1FF, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_COOL_24_AUTO_OFF},
    {GREE_YX1FF, climate::CLIMATE_MODE_HEAT, 20.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_HEAT_20_HIGH_VERTICAL},
    {GREE_YX1FF, climate::CLIMATE_MODE_AUTO, 22.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_AUTO_22_MEDIUM_OFF},
    {GREE_YX1FF, climate::CLIMATE_MODE_DRY, 26.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_DRY_26_AUTO_OFF},
    {GREE_YX1FF, climate::CLIMATE_MODE_FAN_ONLY, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_FAN_ONLY_24_HIGH_OFF},
    {GREE_YX1FF, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_SLEEP, &testing::EXPECTED_YX1FF_COOL_24_AUTO_SLEEP},
    {GREE_YX1FF, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_YX1FF_COOL_24_TURBO_OFF},

    // GREE_GENERIC model tests
    {GREE_GENERIC, climate::CLIMATE_MODE_OFF, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_OFF},
    {GREE_GENERIC, climate::CLIMATE_MODE_COOL, 24.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_COOL_24_AUTO_OFF},
    {GREE_GENERIC, climate::CLIMATE_MODE_HEAT, 20.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_HEAT_20_HIGH_VERTICAL},
    {GREE_GENERIC, climate::CLIMATE_MODE_AUTO, 22.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_AUTO_22_MEDIUM_OFF},
    {GREE_GENERIC, climate::CLIMATE_MODE_DRY, 26.0f, climate::CLIMATE_FAN_AUTO, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_DRY_26_AUTO_OFF},
    {GREE_GENERIC, climate::CLIMATE_MODE_FAN_ONLY, 24.0f, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_FAN_ONLY_24_HIGH_OFF},
    {GREE_GENERIC, climate::CLIMATE_MODE_COOL, 16.0f, climate::CLIMATE_FAN_LOW, climate::CLIMATE_SWING_OFF, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_COOL_16_LOW_OFF},
    {GREE_GENERIC, climate::CLIMATE_MODE_HEAT, 30.0f, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_PRESET_NONE, &testing::EXPECTED_GENERIC_HEAT_30_MEDIUM_VERT},
};
// clang-format on

TEST(GreeClimateTest, Transmit) {
  for (const auto &test : test_data) {
    // Create a new test rig for each iteration
    testing::GreeTestRig rig(test.model);

    // Set the climate state to match test parameters
    rig.climate.mode = test.mode;
    rig.climate.target_temperature = test.target_temp;
    rig.climate.fan_mode = test.fan_mode;
    rig.climate.swing_mode = test.swing_mode;
    rig.climate.preset = test.preset;

    // Transmit the state
    rig.transmit();

    ASSERT_EQ(rig.mock.transmit_count, 1);

    EXPECT_EQ(rig.mock.captured_frequency, 38000);
    /*
        auto dump = [](const std::vector<int32_t>& t, const char* name) {
          std::printf("inline const std::vector<int32_t> %s = {", name);
          for (size_t i = 0; i < t.size(); i++) {
            if (i % 8 == 0) std::printf("\n    ");
            std::printf("%d, ", t[i]);
          }
          std::printf("\n};\n");
        };
        dump(rig.mock.captured_timings, "EXPECTED_GENERIC");
    */
    // Verify timings match expectation
    ASSERT_EQ(rig.mock.captured_timings.size(), test.expectation->size());
    for (size_t i = 0; i < test.expectation->size(); ++i) {
      EXPECT_EQ(rig.mock.captured_timings[i], (*test.expectation)[i])
          << "Mismatch at index " << i << " for model " << static_cast<int>(test.model) << ", mode "
          << static_cast<int>(test.mode) << ", temp " << test.target_temp << ", fan " << static_cast<int>(test.fan_mode)
          << ", swing " << static_cast<int>(test.swing_mode) << ", preset " << static_cast<int>(test.preset);
    }
  }
}

}  // namespace gree
}  // namespace esphome
