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

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

namespace smc_verifiable_plugins
{
/*!
 * @brief The data structure containing all the variables serving as interface between this plugin and the SMC engine
 */
using DataExchange = std::unordered_map<std::string, std::variant<int64_t, double, bool>>;

/*!
 * @brief Plugin configuration: key=value pairs provided to the SMC engine.
 * The same configuration is passed to every plugin, not used keys should be ignored.
 */
using PluginConfig = std::unordered_map<std::string, std::string>;

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
   * @param config The configuration parameters from the model
   * @param plugin_config The plugin configuration, separate from the model
   */
  inline void loadParameters(const DataExchange& config, const PluginConfig& plugin_config = {})
  {
    _plugin_config = plugin_config;
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

  /*!
   * @brief The plugin configuration.
   */
  inline const PluginConfig& getPluginConfig() const
  {
    return _plugin_config;
  }

  /*!
   * @brief Get a plugin configuration value.
   */
  inline std::optional<std::string> getConfigValue(const std::string& key) const
  {
    const auto it = _plugin_config.find(key);
    if (it == _plugin_config.end())
    {
      return std::nullopt;
    }
    return it->second;
  }

  /*!
   * @brief Treat a key as a path and get its absolute value.
   * @return The absolute path; relative paths resolved from the current working directory.
   * @throw std::invalid_argument if the key is not set or the path does not exist.
   */
  inline std::filesystem::path getConfigPath(const std::string& key) const
  {
    const std::optional<std::string> value = getConfigValue(key);
    if (!value.has_value())
    {
      throw std::invalid_argument("Missing required plugin configuration '" + key + "'.");
    }
    const std::filesystem::path path(*value);
    if (!std::filesystem::exists(path))
    {
      throw std::invalid_argument("Plugin configuration '" + key + "': path '" + *value + "' does not exist.");
    }
    return std::filesystem::absolute(path).lexically_normal();
  }

  inline void setParamsLoaded()
  {
    _params_loaded = true;
  }

  inline void checkParamsLoaded()
  {
    if (!_params_loaded)
    {
      throw std::runtime_error("Trying to execute the model before the parameters are loaded.");
    }
  }

 private:
  bool _params_loaded = false;
  PluginConfig _plugin_config;
};
}  // namespace smc_verifiable_plugins
