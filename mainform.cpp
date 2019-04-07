#include "mainform.hpp"
#include "ui_mainform.h"

MainForm::MainForm(shadertoy::config_t&& condig, QWidget *parent)
  : QWidget(parent)
  , ui { new Ui::MainForm }
{
  ui->setupUi(this);
}

MainForm::~MainForm()
{
}
