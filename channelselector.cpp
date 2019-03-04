#include "channelselector.hpp"
#include "ui_channelselector.h"

ChannelSelector::ChannelSelector(QWidget *parent) :
  QWidget(parent),
  ui { new Ui::ChannelSelector }
{
  ui->setupUi(this);
}

ChannelSelector::~ChannelSelector()
{
}
