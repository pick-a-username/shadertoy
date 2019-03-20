#include "mainform.hpp"
#include "ui_mainform.h"

MainForm::MainForm(QWidget *parent)
  : QWidget(parent)
  , ui { new Ui::MainForm }
{
  ui->setupUi(this);
#if 0
  auto f = [&](GLScene* scene, ShaderEditor* shaderEditor) {
    connect(scene, &GLScene::initializedGL, shaderEditor, &ShaderEditor::initializeGL, Qt::QueuedConnection);
    connect(scene, &GLScene::resizedGL, shaderEditor->renderWorker(), &RenderWorker::resizeGL, Qt::QueuedConnection);
  };
  f(ui->scene, ui->image);
  f(ui->scene, ui->buffer_A);
  f(ui->scene, ui->buffer_B);
  f(ui->scene, ui->buffer_C);
  f(ui->scene, ui->buffer_D);
  connect(ui->image->renderWorker(), &RenderWorker::ready, ui->scene, &GLScene::channel_A, Qt::QueuedConnection);
#endif
}

MainForm::~MainForm()
{
}
