#include <yaml-cpp/node/node.h>
#include "mainform.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainForm w;
  w.show();
  return a.exec();
}
