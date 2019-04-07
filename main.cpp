#include <iostream>
#include "mainform.hpp"
#include <QFileDialog>
#include <QApplication>

#include "shadertoy_yaml.hpp"

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
          "Project (*.yaml *.yml)").toStdString();
  }
  else
  {
    yaml_config = argv[1];
  }
  auto&& config { YAML::LoadFile(yaml_config).as<shadertoy::config_t>() };
  MainForm w { std::move(config) };
  w.show();
  return a.exec();
}
