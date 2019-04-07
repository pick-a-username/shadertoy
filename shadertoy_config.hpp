#pragma once
#define SHADERTOY_CONFIG_HPP

#include <string>
#include <list>
#include <map>

namespace shadertoy
{
  struct common_t
  {
    std::string src;
    std::list<std::string> includes;
  };
  struct shader_t
  {
    uint32_t width { 0 };
    uint32_t height { 0 };
    std::string channel0;
    std::string channel1;
    std::string channel2;
    std::string channel3;
    std::string file;
    std::string src;
  };
  struct config_t
  {
    common_t common;
    shader_t main;
    std::map<std::string, shader_t> channels;
  };
}
