// Copyright 2023 The Autoware Contributors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CORE__CONFIG_HPP_
#define CORE__CONFIG_HPP_

#include <yaml-cpp/yaml.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace system_diagnostic_graph
{

struct ConfigData
{
  explicit ConfigData(const std::string & file);
  ConfigData load(YAML::Node yaml);
  ConfigData type(const std::string & name) const;
  ConfigData node(const size_t index) const;

  std::string take_text(const std::string & name);
  std::string take_text(const std::string & name, const std::string & fail);
  std::vector<YAML::Node> take_list(const std::string & name);

  std::string file;
  std::string mark;
  std::unordered_map<std::string, YAML::Node> object;
};

struct BaseConfig
{
  explicit BaseConfig(const ConfigData & data) : data(data) {}
  ConfigData data;
};

struct PathConfig : public BaseConfig
{
  using SharedPtr = std::shared_ptr<PathConfig>;
  using BaseConfig::BaseConfig;
  std::string original;
  std::string resolved;
};

struct UnitConfig : public BaseConfig
{
  using SharedPtr = std::shared_ptr<UnitConfig>;
  using BaseConfig::BaseConfig;
  std::string type;
  std::string path;
  std::vector<UnitConfig::SharedPtr> children;
};

struct FileConfig : public BaseConfig
{
  using SharedPtr = std::shared_ptr<FileConfig>;
  using BaseConfig::BaseConfig;
  std::vector<PathConfig::SharedPtr> paths;
  std::vector<UnitConfig::SharedPtr> nodes;
};

struct RootConfig
{
  std::vector<FileConfig::SharedPtr> files;
  std::vector<UnitConfig::SharedPtr> nodes;
};

template <class T>
T error(const std::string & text, const ConfigData & data)
{
  const auto hint = data.mark.empty() ? data.file : data.mark + ":" + data.file;
  return T(text + " (" + hint + ")");
}

template <class T>
T error(const std::string & text)
{
  return T(text);
}

template <class T>
T error(const std::string & text, const std::string & value, const ConfigData & data)
{
  return error<T>(text + ": " + value, data);
}

template <class T>
T error(const std::string & text, const std::string & value)
{
  return error<T>(text + ": " + value);
}

RootConfig load_root_config(const std::string & path);

}  // namespace system_diagnostic_graph

#endif  // CORE__CONFIG_HPP_
