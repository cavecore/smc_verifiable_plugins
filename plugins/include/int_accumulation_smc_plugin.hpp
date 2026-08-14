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
#pragma once
#include <vector>

#include "smc_verifiable_plugins/smc_plugin_base.hpp"

namespace smc_verifiable_plugins
{

/*!
 * @brief Base class providing the minimum interface required by a plugin to work with SMC
 */
class IntAccumulationSmcPlugin : public SmcPluginBase
{
 public:
  IntAccumulationSmcPlugin();

  ~IntAccumulationSmcPlugin();

  /*!
   * @copydoc SmcPluginBase::getPluginName()
   */
  std::string getPluginName() const override
  {
    return "int_accumulation_smc_plugin";
  }

 private:
  /*!
   * @copydoc SmcPluginBase::processInitParameters(const DataExchange& config)
   */
  void processInitParameters(const DataExchange& config) override;

  /*!
   * @copydoc SmcPluginBase::processReset()
   */
  std::optional<DataExchange> processReset() override;

  /*!
   * @copydoc SmcPluginBase::processInputParameters(const DataExchange& input_data)
   */
  std::optional<DataExchange> processInputParameters(const DataExchange& input_data) override;

  int64_t _current_state = 0U;
  const std::string _input_label;
  const std::string _output_label;
};
}  // namespace smc_verifiable_plugins
