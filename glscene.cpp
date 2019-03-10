#include <QThread>
#include <QDebug>
#include "glscene.hpp"
#include "ui_glscene.h"

GLScene::GLScene(QWidget *parent)
  : QOpenGLWidget(parent)
  , ui(new Ui::GLScene)
  , m_startTime(QTime::currentTime())
{
  ui->setupUi(this);
}

GLScene::~GLScene()
{
  makeCurrent();
  m_vbo.destroy();
  m_program.reset();
  doneCurrent();
}

void GLScene::setShader(const QString& source)
{
  const char* vsrc =
      "attribute highp vec2 vertex;\n"
      "void main(void){gl_Position = vec4(vertex, 0.0, 1.0);\n}\n";
  QString fsrc =
      "uniform vec3      iResolution;\n"           // viewport resolution (in pixels)
      "uniform float     iTime;\n"                 // shader playback time (in seconds)
      //"uniform float     iTimeDelta;\n"            // render time (in seconds)
      //"uniform int       iFrame;\n"                // shader playback frame
      //"uniform float     iChannelTime[4];\n"       // channel playback time (in seconds)
      //"uniform vec3      iChannelResolution[4];\n" // channel resolution (in pixels)
      //"uniform vec4      iMouse;\n"                // mouse pixel coords. xy: current (if MLB down), zw: click
      //"uniform samplerXX iChannel0..3;\n"          // input channel. XX = 2D/Cube
      //"uniform vec4      iDate;\n"                 // (year, month, day, time in seconds)
      //"uniform float     iSampleRate;\n"           // sound sample rate (i.e., 44100)
      "\n"
      + source +
      "void main(void){mainImage(gl_FragColor, gl_FragCoord.xy);}\n"
      ;

  m_program.reset(new QOpenGLShaderProgram);
  m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
  m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
  m_program->bindAttributeLocation("vertex", 1);
  m_program->link();
  qDebug() << __LINE__ << m_program->log();
}

void GLScene::channel_A(GLuint texId, const QSize& size)
{
  makeCurrent();
}

void GLScene::channel_B(GLuint texId, const QSize& size)
{
  makeCurrent();
}

void GLScene::channel_C(GLuint texId, const QSize& size)
{
  makeCurrent();
}

void GLScene::channel_D(GLuint texId, const QSize& size)
{
  makeCurrent();
}

void GLScene::initializeGL()
{
  initializeOpenGLFunctions();
  emit initializedGL(context());
  static const GLfloat quad_coords[4][2] = { { -1.0f, -1.0f }, { -1.0f, +1.0f }, { +1.0f, +1.0f }, { +1.0f, -1.0f } };
  m_vbo.create();
  m_vbo.bind();
  m_vbo.allocate(&quad_coords[0][0], sizeof quad_coords);
  setShader(
        "void mainImage(out vec4 fragColor, in vec2 fragCoord)\n"
        "{\n"
        "  fragColor = vec4(mod(fragCoord, 20.0) / 20.0, 1.0, 1.0);"
        "}\n"
        );
}

void GLScene::paintGL()
{
  m_vbo.bind();
  m_program->bind();
  m_program->enableAttributeArray(1);
  m_program->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);
  m_program->setUniformValue("iResolution", size());
  m_program->setUniformValue("iTime", m_startTime.msecsTo(QTime::currentTime()) * 1e-3f);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
