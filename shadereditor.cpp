#include "shadereditor.hpp"
#include "ui_shadereditor.h"

ShaderEditor::ShaderEditor(QWidget* parent)
  : QWidget(parent)
  , m_renderWorker { new RenderWorker }
  , ui { new Ui::ShaderEditor }
{
  ui->setupUi(this);
}

ShaderEditor::~ShaderEditor()
{
}

void ShaderEditor::initializeGL(QOpenGLContext* context)
{
  m_renderWorker->initializeGL(context);
  m_renderWorker->start();
}
