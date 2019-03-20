#ifndef GLSCENE_HPP
#define GLSCENE_HPP

#include <QScopedPointer>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QTime>
#include "shadertoy_core.hpp"

namespace Ui {
  class GLScene;
}

class GLScene
    : public QOpenGLWidget
    , protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit GLScene(QWidget *parent = nullptr);
  virtual ~GLScene();

signals:
  void initializedGL(QOpenGLContext*);
  void resizedGL(int width, int height);

public slots:
  void setShader(const QString&);
  void channel_A(GLuint, const QSize&);
  void channel_B(GLuint, const QSize&);
  void channel_C(GLuint, const QSize&);
  void channel_D(GLuint, const QSize&);

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void timerEvent(QTimerEvent *event) override;

private:
  void dump_glerror(int line);

private:
  QScopedPointer<Ui::GLScene> ui;
  QScopedPointer<shadertoy::core> m_program;
  int m_timer_handle { -1 };
  QTime m_startTime;
  QPointF m_mouseClick;
  QPointF m_mousePosition;
};

#endif // GLSCENE_HPP
