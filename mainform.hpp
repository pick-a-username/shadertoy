#ifndef MAINFORM_HPP
#define MAINFORM_HPP

#include <QWidget>
#include <QScopedPointer>
#include "shadertoy_config.hpp"

namespace Ui {
  class MainForm;
}

class MainForm : public QWidget
{
  Q_OBJECT

public:
  explicit MainForm(shadertoy::config_t&& condig, QWidget *parent = nullptr);
  ~MainForm();

private:
  QScopedPointer<Ui::MainForm> ui;
};

#endif // MAINFORM_HPP
