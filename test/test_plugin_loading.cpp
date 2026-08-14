/*
 * Copyright (c) 2024 - for information on the respective copyright owner
 * see the NOTICE file
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include <filesystem>

#include "smc_verifiable_plugins/utils.hpp"

/*
Test the custom, cache-less jani states generator
*/
TEST(LoadIntAccumulator, TestPluginLoader)
{
  const std::string plugin_name = "int_accumulation_smc_plugin";
  const auto plugin_ptr = smc_verifiable_plugins::loadPlugin(std::filesystem::current_path(), plugin_name);
  EXPECT_EQ(plugin_name, plugin_ptr->getPluginName());
  plugin_ptr->loadParameters({});
  auto plugin_out = plugin_ptr->reset();
  ASSERT_NE(plugin_out, std::nullopt);
  int64_t expected_val = 0;
  for (size_t n = 0; n < 10; n++)
  {
    plugin_out = plugin_ptr->nextStep({{"input_value", static_cast<int64_t>(n)}});
    const int64_t acc_val = std::get<int64_t>(plugin_out->at("accumulated_value"));
    expected_val += static_cast<int64_t>(n);
    ASSERT_EQ(acc_val, expected_val);
  }
  const int64_t step_back_val = -10;
  expected_val += step_back_val;
  plugin_out = plugin_ptr->nextStep({{"input_value", static_cast<int64_t>(step_back_val)}});
  const int64_t acc_val = std::get<int64_t>(plugin_out->at("accumulated_value"));
  ASSERT_EQ(acc_val, expected_val);
}

TEST(LoadWrongPath, TestPluginLoader)
{
  const std::string plugin_name = "a_fake_plugin";
  const auto plugin_ptr = smc_verifiable_plugins::loadPlugin(std::filesystem::current_path(), plugin_name);
  EXPECT_EQ(plugin_ptr, nullptr);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  // Initialize the default STORM settings (required for comparator)
  return RUN_ALL_TESTS();
}
