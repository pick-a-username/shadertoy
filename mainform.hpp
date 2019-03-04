#ifndef MAINFORM_HPP
#define MAINFORM_HPP

#include <QWidget>
#include <QScopedPointer>

namespace Ui {
  class MainForm;
}

class MainForm : public QWidget
{
  Q_OBJECT

public:
  explicit MainForm(QWidget *parent = nullptr);
  ~MainForm();

private:
  QScopedPointer<Ui::MainForm> ui;
};

#endif // MAINFORM_HPP
