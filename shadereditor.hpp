#ifndef SHADEREDITOR_HPP
#define SHADEREDITOR_HPP

#include <QWidget>
#include <QScopedPointer>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions_4_5_Core>
#include "renderworker.hpp"

namespace Ui {
  class ShaderEditor;
}

class ShaderEditor
    : public QWidget
    , protected QOpenGLFunctions_4_5_Core
{
  Q_OBJECT

  QScopedPointer<RenderWorker> m_renderWorker;

public:
  explicit ShaderEditor(QWidget *parent = nullptr);
  ~ShaderEditor();

public:
  inline RenderWorker* renderWorker() const { return m_renderWorker.get(); }

public slots:
  void initializeGL(QOpenGLContext* context);

private:
  QScopedPointer<Ui::ShaderEditor> ui;
};

#endif // SHADEREDITOR_HPP
