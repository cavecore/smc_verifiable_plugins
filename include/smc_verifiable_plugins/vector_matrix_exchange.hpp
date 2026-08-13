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
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "smc_plugin_base.hpp"

namespace smc_verifiable_plugins
{

/*!
 * @brief Helpers for exchanging vector/matrix-shaped data through DataExchange:
 * a length-n vector "x" becomes "x_0".."x_{n-1}"
 * an m x n matrix "a" becomes "a_0_0".."a_{m-1}_{n-1}" (row-major)
 *
 * unpack..() functions write into properly sized `out`
 * unpack..() can take a `permissive` flag to skip missing keys
 */
namespace vector_matrix_exchange
{

namespace detail
{
template <typename T>
constexpr bool is_supported_element_v = std::is_same_v<T, int64_t> || std::is_same_v<T, double>;

inline std::string indexedKey(const std::string& base_name, const size_t i)
{
  return base_name + "_" + std::to_string(i);
}

inline std::string indexedKey(const std::string& base_name, const size_t row, const size_t col)
{
  return base_name + "_" + std::to_string(row) + "_" + std::to_string(col);
}

template <typename T>
std::optional<T> tryExtract(const DataExchange& in, const std::string& key)
{
  const auto it = in.find(key);
  if (it == in.end())
  {
    return std::nullopt;
  }
  if (!std::holds_alternative<T>(it->second))
  {
    throw std::invalid_argument("vector_matrix_exchange: key '" + key + "' has an unexpected type.");
  }
  return std::get<T>(it->second);
}

template <typename T>
T extract(const DataExchange& in, const std::string& key)
{
  const auto value = tryExtract<T>(in, key);
  if (!value.has_value())
  {
    throw std::invalid_argument("vector_matrix_exchange: missing expected key '" + key + "'.");
  }
  return *value;
}

template <typename T>
void assignIfPresent(T& target, const DataExchange& in, const std::string& key, const bool permissive)
{
  if (permissive)
  {
    const auto value = tryExtract<T>(in, key);
    if (value.has_value())
    {
      target = *value;
    }
  }
  else
  {
    target = extract<T>(in, key);
  }
}
}  // namespace detail

/*!
 * @brief Write vector elements into `out` as "<bn>_0".."<bn>_{n-1}".
 */
template <typename T>
void packVector(DataExchange& out, const std::string& base_name, const std::vector<T>& values)
{
  static_assert(detail::is_supported_element_v<T>, "packVector only supports int64_t or double elements.");
  for (size_t i = 0; i < values.size(); ++i)
  {
    out[detail::indexedKey(base_name, i)] = values[i];
  }
}

/*!
 * @brief Read "<bn>_0".."<bn>_{n-1}" from `in` into `out` that is
 * `std::vector<T>(n)`
 * @throw std::invalid_argument if a key holds the wrong type, or missing (and not `permissive`).
 */
template <typename T>
void unpackVector(const DataExchange& in, const std::string& base_name, std::vector<T>& out,
                  const bool permissive = false)
{
  static_assert(detail::is_supported_element_v<T>, "unpackVector only supports int64_t or double elements.");
  for (size_t i = 0; i < out.size(); ++i)
  {
    detail::assignIfPresent(out[i], in, detail::indexedKey(base_name, i), permissive);
  }
}

/*!
 * @brief Write matrix elements into `out` as "<bn>_<row>_<col>".
 * @throw std::invalid_argument if `values` is not rectangular.
 */
template <typename T>
void packMatrix(DataExchange& out, const std::string& base_name, const std::vector<std::vector<T>>& values)
{
  static_assert(detail::is_supported_element_v<T>, "packMatrix only supports int64_t or double elements.");
  const size_t n_cols = values.empty() ? 0U : values.front().size();
  for (size_t r = 0; r < values.size(); ++r)
  {
    if (values[r].size() != n_cols)
    {
      throw std::invalid_argument("packMatrix: '" + base_name + "' is not rectangular.");
    }
    for (size_t c = 0; c < n_cols; ++c)
    {
      out[detail::indexedKey(base_name, r, c)] = values[r][c];
    }
  }
}

/*!
 * @brief Read a matrix from `in` into `out` that is:
 * `std::vector<std::vector<T>>(rows, std::vector<T>(cols))`
 * @throw std::invalid_argument if a key holds the wrong type, or missing (and not `permissive`).
 */
template <typename T>
void unpackMatrix(const DataExchange& in, const std::string& base_name, std::vector<std::vector<T>>& out,
                  const bool permissive = false)
{
  static_assert(detail::is_supported_element_v<T>, "unpackMatrix only supports int64_t or double elements.");
  for (size_t r = 0; r < out.size(); ++r)
  {
    for (size_t c = 0; c < out[r].size(); ++c)
    {
      detail::assignIfPresent(out[r][c], in, detail::indexedKey(base_name, r, c), permissive);
    }
  }
}

/*!
 * @brief Write only a square matrix upper triangle and diagonal into `out`
 * @throw std::invalid_argument if `values` is not square.
 */
template <typename T>
void packUpperTriangular(DataExchange& out, const std::string& base_name, const std::vector<std::vector<T>>& values)
{
  static_assert(detail::is_supported_element_v<T>, "packUpperTriangular only supports int64_t or double elements.");
  const size_t n = values.size();
  for (size_t r = 0; r < n; ++r)
  {
    if (values[r].size() != n)
    {
      throw std::invalid_argument("packUpperTriangular: '" + base_name + "' is not square.");
    }
    for (size_t c = r; c < n; ++c)
    {
      out[detail::indexedKey(base_name, r, c)] = values[r][c];
    }
  }
}

/*!
 * @brief Read a matrix upper triangle from `in` into `out`
 * @throw std::invalid_argument if a key holds the wrong type, or missing (and not `permissive`).
 */
template <typename T>
void unpackUpperTriangular(const DataExchange& in, const std::string& base_name, std::vector<std::vector<T>>& out,
                           const bool permissive = false)
{
  static_assert(detail::is_supported_element_v<T>, "unpackUpperTriangular only supports int64_t or double elements.");
  const size_t n = out.size();
  for (size_t r = 0; r < n; ++r)
  {
    for (size_t c = r; c < n; ++c)
    {
      detail::assignIfPresent(out[r][c], in, detail::indexedKey(base_name, r, c), permissive);
    }
  }
}

/*!
 * @brief Write only a square matrix lower triangle and diagonal into `out`
 * @throw std::invalid_argument if `values` is not square.
 */
template <typename T>
void packLowerTriangular(DataExchange& out, const std::string& base_name, const std::vector<std::vector<T>>& values)
{
  static_assert(detail::is_supported_element_v<T>, "packLowerTriangular only supports int64_t or double elements.");
  const size_t n = values.size();
  for (size_t r = 0; r < n; ++r)
  {
    if (values[r].size() != n)
    {
      throw std::invalid_argument("packLowerTriangular: '" + base_name + "' is not square.");
    }
    for (size_t c = 0; c <= r; ++c)
    {
      out[detail::indexedKey(base_name, r, c)] = values[r][c];
    }
  }
}

/*!
 * @brief Read a matrix lower triangle from `in` into `out`
 * @throw std::invalid_argument if a key holds the wrong type, or missing (and not `permissive`).
 */
template <typename T>
void unpackLowerTriangular(const DataExchange& in, const std::string& base_name, std::vector<std::vector<T>>& out,
                           const bool permissive = false)
{
  static_assert(detail::is_supported_element_v<T>, "unpackLowerTriangular only supports int64_t or double elements.");
  const size_t n = out.size();
  for (size_t r = 0; r < n; ++r)
  {
    for (size_t c = 0; c <= r; ++c)
    {
      detail::assignIfPresent(out[r][c], in, detail::indexedKey(base_name, r, c), permissive);
    }
  }
}

}  // namespace vector_matrix_exchange
}  // namespace smc_verifiable_plugins
