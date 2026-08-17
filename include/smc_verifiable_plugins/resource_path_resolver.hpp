/*
 * Copyright (c) 2026 - for information on the respective copyright owner
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

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace smc_verifiable_plugins
{

inline constexpr const char* RESOURCE_DATA_FOLDER = "data";

/*!
 * @brief Resolves the path to an external file for a plugin, in order:
 *  a) from env_var, absolute or relative to ../data/
 *  b) fallback to ../data/<default_filename>
 *
 * @param env_var environment variable name
 * @param default_filename default filename
 * @return absolute path to the file
 * @throws std::runtime_error if the file cannot be found
 */
inline std::filesystem::path resolveResourcePath(const std::string& env_var, const std::string& default_filename)
{
  Dl_info info;
  if (dladdr(reinterpret_cast<void*>(&resolveResourcePath), &info) == 0 || info.dli_fname == nullptr)
  {
    throw std::runtime_error("resolveResourcePath: dladdr() failed");
  }
  const std::filesystem::path dl_dir = std::filesystem::path(info.dli_fname).parent_path();
  const std::filesystem::path data_dir = dl_dir / ".." / RESOURCE_DATA_FOLDER;

  const char* env_value = std::getenv(env_var.c_str());
  const bool from_env = env_value != nullptr;
  std::filesystem::path resolved;
  if (from_env)
  {
    const std::filesystem::path env_path(env_value);
    resolved = env_path.is_absolute() ? env_path : (data_dir / env_path);
  }
  else
  {
    resolved = data_dir / default_filename;
  }

  if (!std::filesystem::exists(resolved))
  {
    throw std::runtime_error("resolveResourcePath: resolved path '" + resolved.string() + "' does not exist" +
                             (from_env ? " (from environment variable '" + env_var + "')" : " (default)") + ".");
  }
  return std::filesystem::weakly_canonical(resolved);
}

}  // namespace smc_verifiable_plugins
