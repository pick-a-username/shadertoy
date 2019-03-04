#ifndef GLSCENE_HPP
#define GLSCENE_HPP

#include <QScopedPointer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>

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
  ~GLScene();

signals:
  void initializedGL(QOpenGLContext*);
  void resizedGL(int width, int height);

public slots:
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

private:
  QScopedPointer<Ui::GLScene> ui;
};

#endif // GLSCENE_HPP
