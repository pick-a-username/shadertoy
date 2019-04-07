#pragma once
#define SHADERTOY_YAML_HPP

#include <fstream>
#include "shadertoy_config.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML
{
  template<>
  struct convert<shadertoy::common_t>
  {
    static bool decode(const Node& node, shadertoy::common_t& config)
    {
      if(!node.IsMap())
        return false;
      try { config.src = node["src"].as<std::string>(); } catch (...) { }
      try { config.includes = node["includes"].as<std::list<std::string>>(); } catch (...) { }
      return true;
    }
  };
  template<>
  struct convert<shadertoy::shader_t>
  {
    static bool decode(const Node& node, shadertoy::shader_t& config)
    {
      if(!node.IsMap())
        return false;
      try { config.width = node["width"].as<uint32_t>(); } catch (...) { }
      try { config.height = node["height"].as<uint32_t>(); } catch (...) { }
      try { config.channel0 = node["channel0"].as<std::string>(); } catch (...) { }
      try { config.channel1 = node["channel1"].as<std::string>(); } catch (...) { }
      try { config.channel2 = node["channel2"].as<std::string>(); } catch (...) { }
      try { config.channel3 = node["channel3"].as<std::string>(); } catch (...) { }
      try { config.src = node["src"].as<std::string>(); } catch (...) { }
      if (config.src.rfind('\n') == std::string::npos)
      {
        std::ifstream src(config.src);
        if (src)
        {
          config.file = config.src;
          src.seekg(0, std::ios_base::end);
          config.src.resize(src.tellg());
          src.seekg(0);
          src.read(&config.src[0], config.src.size());
        }
      }
      return true;
    }
  };
  template<>
  struct convert<shadertoy::config_t>
  {
    static bool decode(const Node& node, shadertoy::config_t& config)
    {
      if(!node.IsMap())
        return false;
      try { config.common = node["common"].as<shadertoy::common_t>(); } catch (...) { }
      try
      {
        auto& shaders { node["shaders"] };
        for (auto& shader : shaders)
        {
          auto&& name { shader.first.as<std::string>() };
          if (name == "main")
            config.main = shader.second.as<shadertoy::shader_t>();
          else
            config.channels.emplace(name, shader.second.as<shadertoy::shader_t>());
        }

        auto& shader { config.main };
        if (!shader.channel0.empty())
          config.channels.at(shader.channel0);
        if (!shader.channel1.empty())
          config.channels.at(shader.channel1);
        if (!shader.channel2.empty())
          config.channels.at(shader.channel2);
        if (!shader.channel3.empty())
          config.channels.at(shader.channel3);
        for (auto& channel : config.channels)
        {
          auto& shader { channel.second };
          if (!shader.channel0.empty())
            config.channels.at(shader.channel0);
          if (!shader.channel1.empty())
            config.channels.at(shader.channel1);
          if (!shader.channel2.empty())
            config.channels.at(shader.channel2);
          if (!shader.channel3.empty())
            config.channels.at(shader.channel3);
        }
      }
      catch (...)
      {
        return false;
      }
      return true;
    }
  };
}
