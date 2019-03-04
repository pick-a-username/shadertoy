#include <QDebug>
#include <QMutexLocker>
#include "renderworker.hpp"

RenderWorker::RenderWorker(QObject* parent)
  : QThread(parent)
  , m_context { new QOpenGLContext }
  , m_surface { new QOffscreenSurface }
{

}

RenderWorker::~RenderWorker()
{
  exit();
  wait();
}

void RenderWorker::initializeGL(QOpenGLContext* context)
{
  m_context->setFormat(context->format());
  m_context->setShareContext(context);
  m_context->create();
  m_context->moveToThread(this);
  m_surface->setFormat(m_context->format());
  m_surface->create();
  connect(this, SIGNAL(started()), SLOT(initializeGL()));
  connect(this, SIGNAL(finished()), SLOT(deinitializeGL()));
  moveToThread(this);
}

void RenderWorker::setShader(const QString&)
{

}

void RenderWorker::resizeGL(int width, int height)
{
  m_fboSize.setWidth(width);
  m_fboSize.setHeight(height);
  qDebug() << QThread::currentThread() << __LINE__ << __PRETTY_FUNCTION__ << m_fboSize;
}

void RenderWorker::paintGL()
{
  makeCurrent();
  if (!m_render_fbo || m_render_fbo->size() != m_fboSize)
  {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setMipmap(true);
    format.setTextureTarget(GL_TEXTURE_2D);
    m_render_fbo.reset(new QOpenGLFramebufferObject(m_fboSize, format));
    m_display_fbo.reset(new QOpenGLFramebufferObject(m_fboSize, format));
  }
  m_render_fbo->bind();

  m_render_fbo->release();
  m_render_fbo.swap(m_display_fbo);
  emit ready(m_display_fbo->texture(), m_fboSize);
}

void RenderWorker::channel_A(GLuint texId, const QSize& size)
{
  m_channel_A.texId = texId;
  m_channel_A.size = size;
}

void RenderWorker::channel_B(GLuint texId, const QSize& size)
{
  m_channel_B.texId = texId;
  m_channel_B.size = size;
}

void RenderWorker::channel_C(GLuint texId, const QSize& size)
{
  m_channel_C.texId = texId;
  m_channel_C.size = size;
}

void RenderWorker::channel_D(GLuint texId, const QSize& size)
{
  m_channel_D.texId = texId;
  m_channel_D.size = size;
}

void RenderWorker::initializeGL()
{
  qDebug() << QThread::currentThread() << __LINE__ << __PRETTY_FUNCTION__;
  makeCurrent();
  initializeOpenGLFunctions();
  m_timerId = startTimer(20);
}

void RenderWorker::deinitializeGL()
{
  qDebug() << QThread::currentThread() << __LINE__ << __PRETTY_FUNCTION__;
  makeCurrent();
  m_render_fbo.reset();
  m_display_fbo.reset();
  doneCurrent();
  m_context.reset();
  m_surface.reset();
  killTimer(m_timerId);
}

void RenderWorker::timerEvent(QTimerEvent*)
{
  paintGL();
}
