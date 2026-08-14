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

#include "int_accumulation_smc_plugin.hpp"

#include <stdexcept>

#include "smc_verifiable_plugins/utils.hpp"

namespace smc_verifiable_plugins
{

IntAccumulationSmcPlugin::IntAccumulationSmcPlugin()
    : _input_label{"input_value"}, _output_label{"accumulated_value"} {};

IntAccumulationSmcPlugin::~IntAccumulationSmcPlugin() {};

void IntAccumulationSmcPlugin::processInitParameters(const DataExchange& config)
{
  if (!config.empty())
  {
    throw std::invalid_argument("Invalid configuration provided. Expected no params.");
  }
};

std::optional<DataExchange> IntAccumulationSmcPlugin::processReset()
{
  _current_state = 0U;
  return std::make_optional<DataExchange>({{_output_label, _current_state}});
}

std::optional<DataExchange> IntAccumulationSmcPlugin::processInputParameters(const DataExchange& input_data)
{
  if (input_data.size() != 1U && !input_data.contains(_input_label))
  {
    throw std::invalid_argument("Invalid input provided. Expected only one integer.");
  }
  _current_state += std::get<int64_t>(input_data.at(_input_label));
  return std::make_optional<DataExchange>({{_output_label, _current_state}});
}

GENERATE_PLUGIN_LOADER(IntAccumulationSmcPlugin);

}  // namespace smc_verifiable_plugins
