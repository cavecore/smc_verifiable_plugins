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
#include <stdint.h>

#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <variant>

namespace smc_verifiable_plugins
{
/*!
 * @brief The data structure containing all the variables serving as interface between this plugin and the SMC engine
 */
using DataExchange = std::unordered_map<std::string, std::variant<int64_t, double, bool>>;

/*!
 * @brief Base class providing the minimum interface required by a plugin to work with SMC
 */
class SmcPluginBase
{
 public:
  virtual ~SmcPluginBase() = default;

  /*!
   * @brief Get a string containing the plugin name
   * @return The string ID of the loaded plugin instance (e.g. "SmcPluginBase")
   */
  virtual std::string getPluginName() const = 0;

  /*!
   * @brief Set the random seed for the plugin
   * @param seed The seed value to set
   */
  virtual void setRandomSeed([[maybe_unused]] const uint32_t seed)
  {
  }

  /*!
   * @brief Configure the plugin before resetting and using it.
   * @param config The configuration parameters
   */
  inline void loadParameters(const DataExchange& config)
  {
    processInitParameters(config);
    setParamsLoaded();
  }

  /*!
   * @brief Reset the plugin to the initial state
   * @return The initial state of the output variables, or nullopt in case of errors.
   */
  std::optional<DataExchange> reset()
  {
    checkParamsLoaded();
    return processReset();
  }

  /*!
   * @brief Advances the plugin to the next state
   * @param input_data The data used to control the evolution of the plugin
   * @return The output values associated to the new reached state, or nullopt in case of errors.
   */
  inline std::optional<DataExchange> nextStep(const DataExchange& input_data)
  {
    checkParamsLoaded();
    return processInputParameters(input_data);
  }

 protected:
  /*!
   * @brief Process the provided plugin's configuration.
   * @param config The structure containing the configuration to load.
   */
  virtual void processInitParameters(const DataExchange& config) = 0;

  /*!
   * @brief Reset the plugin to the initial state.
   * @return The initial state of the output variables, or nullopt in case of errors..
   */
  virtual std::optional<DataExchange> processReset() = 0;

  /*!
   * @brief Process the input parameters provided to advance the plugin to its next state.
   * @param config The structure containing the configuration to load.
   * @return The new plugin state, after getting to its next state, or nullopt in case of errors.
   */
  virtual std::optional<DataExchange> processInputParameters(const DataExchange& config) = 0;

  inline void setParamsLoaded()
  {
    _params_loaded = true;
  }

  inline void checkParamsLoaded()
  {
    if (!_params_loaded)
    {
      std::runtime_error("Trying to execute the model before the parameters are loaded.");
    }
  }

 private:
  bool _params_loaded = false;
};
}  // namespace smc_verifiable_plugins
