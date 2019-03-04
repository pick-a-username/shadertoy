#include <QThread>
#include <QDebug>
#include "glscene.hpp"
#include "ui_glscene.h"

GLScene::GLScene(QWidget *parent)
  : QOpenGLWidget(parent)
  , ui(new Ui::GLScene)
{
  ui->setupUi(this);
}

GLScene::~GLScene()
{
}

void GLScene::channel_A(GLuint texId, const QSize& size)
{
  makeCurrent();
  qDebug() << QThread::currentThread() << __PRETTY_FUNCTION__ << __LINE__ << texId << size;
}

void GLScene::channel_B(GLuint texId, const QSize& size)
{
  makeCurrent();
  qDebug() << QThread::currentThread() << __PRETTY_FUNCTION__ << __LINE__ << texId << size;
}

void GLScene::channel_C(GLuint texId, const QSize& size)
{
  makeCurrent();
  qDebug() << QThread::currentThread() << __PRETTY_FUNCTION__ << __LINE__ << texId << size;
}

void GLScene::channel_D(GLuint texId, const QSize& size)
{
  makeCurrent();
  qDebug() << QThread::currentThread() << __PRETTY_FUNCTION__ << __LINE__ << texId << size;
}

void GLScene::initializeGL()
{
  initializeOpenGLFunctions();
  emit initializedGL(context());
}

void GLScene::paintGL()
{
}

void GLScene::resizeGL(int width, int height)
{
  emit resizedGL(width, height);
}

void GLScene::mousePressEvent(QMouseEvent *event)
{
}

void GLScene::mouseMoveEvent(QMouseEvent *event)
{
}

void GLScene::mouseReleaseEvent(QMouseEvent *event)
{
}
