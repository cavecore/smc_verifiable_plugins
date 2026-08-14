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
#include <dlfcn.h>

#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "smc_plugin_base.hpp"

namespace smc_verifiable_plugins
{
// A macro that can be used to generate the function used by dlopen to generate the plugin instance.
#define GENERATE_PLUGIN_LOADER(plugin_class)                    \
  extern "C" smc_verifiable_plugins::SmcPluginBase* getPlugin() \
  {                                                             \
    return new plugin_class;                                    \
  }

/*!
 * @brief Helper function to generate the full path to the plugin library, given the folder path and the plugin.
 * @param plugin_folder The folder containing the plugin.
 * @param plugin_name The name of the plugin to load.
 * @return The full path to the plugin library file.
 */
inline std::filesystem::path generatePathToPlugin(const std::filesystem::path& plugin_folder,
                                                  const std::string& plugin_name)
{
  return plugin_folder / ("lib" + plugin_name + ".so");
}

/*!
 * @brief Function to load an external SMC plugin based on its parent folder path and its name.
 * @param plugin_folder The path to the folder containing the plugin library.
 * @param plugin_name The name of the plugin instance to load. The library name and the plugin name are expected to
 * match.
 * @return A unique_ptr containing an instance of the loaded plugin, or a nullptr if loading was not possible.
 */
std::unique_ptr<SmcPluginBase> loadPlugin(const std::filesystem::path& plugin_folder, const std::string& plugin_name)
{
  const std::filesystem::path full_path = generatePathToPlugin(plugin_folder, plugin_name);
  if (std::filesystem::exists(full_path))
  {
    void* handle = dlopen(full_path.c_str(), RTLD_LAZY);
    if (!handle)
    {
      std::stringstream dl_error_stream;
      dl_error_stream << "Cannot open library: " << dlerror();
      throw std::invalid_argument(dl_error_stream.str());
    }
    auto create_plugin_fun = (SmcPluginBase * (*)()) dlsym(handle, "getPlugin");
    return std::unique_ptr<SmcPluginBase>(create_plugin_fun());
  }
  return nullptr;
}
}  // namespace smc_verifiable_plugins
