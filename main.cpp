#include <iostream>
#include <yaml-cpp/yaml.h>
#include "mainform.hpp"
#include <QFileDialog>
#include <QApplication>

struct shadertoy_config
{
  std::string common;
};

namespace YAML
{
  template<>
  struct convert<shadertoy_config>
  {
    static bool decode(const Node& node, shadertoy_config& config)
    {
      if(!node.IsMap())
        return false;
      config.common = node["common"].as<std::string>();
      return true;
    }
  };
}

int main(int argc, char *argv[])
{
  std::string yaml_config;
  QApplication a(argc, argv);
  if (argc != 2)
  {
    yaml_config = QFileDialog::getOpenFileName(
          nullptr,
          "Select yaml file",
          QDir::current().path(),
          ("Project (*.yaml *.yml)")
          ).toStdString();
  }
  else
  {
    yaml_config = argv[1];
  }
  auto config { YAML::LoadFile(yaml_config).as<shadertoy_config>() };
  std::cout << "===\n" << config.common << "\n=====\n";
  //MainForm w;
  //w.show();
  //return a.exec();
  return 0;
}
