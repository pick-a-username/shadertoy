#ifndef RENDERWORKER_HPP
#define RENDERWORKER_HPP

#include <QThread>
#include <QMutex>
#include <QOffscreenSurface>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>

class RenderWorker
    : public QThread
    , protected QOpenGLFunctions
{
  Q_OBJECT

public:
  RenderWorker(QObject* parent = nullptr);
  ~RenderWorker();

/*
Shader Inputs
uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform int       iFrame;                // shader playback frame
uniform float     iChannelTime[4];       // channel playback time (in seconds)
uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
uniform samplerXX iChannel0..3;          // input channel. XX = 2D/Cube
uniform vec4      iDate;                 // (year, month, day, time in seconds)
uniform float     iSampleRate;           // sound sample rate (i.e., 44100)
void mainImage( out vec4 fragColor, in vec2 fragCoord)
{
}
 */
public slots:
  void initializeGL(QOpenGLContext* context);
  void setShader(const QString&);
  void resizeGL(int width, int height);
  void paintGL();
  void channel_A(GLuint, const QSize&);
  void channel_B(GLuint, const QSize&);
  void channel_C(GLuint, const QSize&);
  void channel_D(GLuint, const QSize&);

private slots:
  void initializeGL();
  void deinitializeGL();

protected:
  void timerEvent(QTimerEvent*) override;

signals:
  void ready(GLuint, const QSize&);

private:
  QMutex m_lock;
  QSize m_fboSize;
  int m_timerId { -1 };
  QScopedPointer<QOpenGLContext> m_context;
  QScopedPointer<QOffscreenSurface> m_surface;
  QScopedPointer<QOpenGLFramebufferObject> m_render_fbo;
  QScopedPointer<QOpenGLFramebufferObject> m_display_fbo;
  struct channel_info
  {
    GLuint texId { 0 };
    QSize size;
  };
  channel_info m_channel_A;
  channel_info m_channel_B;
  channel_info m_channel_C;
  channel_info m_channel_D;
private:
  inline void makeCurrent() { m_context->makeCurrent(m_surface.get()); }
  inline void doneCurrent() { m_context->doneCurrent(); }
};

#endif // RENDERWORKER_HPP
