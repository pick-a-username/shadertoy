#ifndef CHANNELSELECTOR_HPP
#define CHANNELSELECTOR_HPP

#include <QWidget>
#include <QScopedPointer>

namespace Ui {
  class ChannelSelector;
}

class ChannelSelector : public QWidget
{
  Q_OBJECT

public:
  explicit ChannelSelector(QWidget *parent = nullptr);
  ~ChannelSelector();

private:
  QScopedPointer<Ui::ChannelSelector> ui;
};

#endif // CHANNELSELECTOR_HPP
