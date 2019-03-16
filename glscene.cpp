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
      "\nvoid main(void){mainImage(gl_FragColor, gl_FragCoord.xy);}\n"
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
#if 0
        "void mainImage(out vec4 fragColor, in vec2 fragCoord)\n"
        "{\n"
        "  fragColor = vec4(mod(fragCoord, 20.0) / 20.0, 1.0, 1.0);"
        "}\n"
#else
        "struct shape_sdf { float dist; vec3 color; };\n"
        "\n"
        "\n"
        "float circle_sdf(vec3 p, vec3 pos, float size)\n"
        "{\n"
        "    return length(p - pos) - size;\n"
        "}\n"
        "\n"
        "float box_sdf(vec3 p, vec3 pos, float size)\n"
        "{\n"
        "    p = abs(p - pos);\n"
        "    return max(p.x, max(p.y, p.z)) - size * .5;\n"
        "}\n"
        "\n"
        "float surface_sdf(vec3 p, vec3 orientation, float dist)\n"
        "{\n"
        "    return dot(p, orientation) - dist;\n"
        "}\n"
        "\n"
        "shape_sdf intersect_sdf(shape_sdf A, shape_sdf B) { if (A.dist > B.dist) return A; return B; }\n"
        "shape_sdf union_sdf(shape_sdf A, shape_sdf B) { if (A.dist < B.dist) return A; return B; }\n"
        "shape_sdf difference_sdf(shape_sdf A, shape_sdf B) { B.dist = - B.dist; return intersect_sdf(A, B); }\n"
        "\n"
        "vec3 Stereo(vec2 res, vec2 p)\n"
        "{\n"
        "    vec3 uv;\n"
        "    uv.xy = 2. * p / res - vec2(1.);\n"
        "    res.x *= .5;\n"
        "    res /= res.x;\n"
        "    if (uv.x < 0.)\n"
        "    {\n"
        "        uv.x += 1.;\n"
        "        uv.z = 1.;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        uv.z = -1.;\n"
        "    }\n"
        "    uv.x = uv.x * 2. - 1.;\n"
        "    uv.xy *= res;\n"
        "    return uv;\n"
        "}\n"
        "\n"
        "vec2 fisheye(vec2 uv)\n"
        "{\n"
        "    float d = length(uv);\n"
        "    float r = atan(d, sqrt(1. - d * d)) / 3.14159;\n"
        "    float phi = atan(uv.y, uv.x);\n"
        "    return 2. * r * vec2(cos(phi), sin(phi));\n"
        "}\n"
        "\n"
        "\n"
        "#define MAX_STEPS 100\n"
        "#define MAX_DIST 100.\n"
        "#define SURF_DIST .001\n"
        "\n"
        "shape_sdf get_dist(vec3 p)\n"
        "{\n"
        "    shape_sdf cur;\n"
        "\n"
        "    cur = difference_sdf(\n"
        "        shape_sdf(box_sdf(p, vec3(0., 1., 6.), 2.), vec3(1., 0., 0.))\n"
        "        ,\n"
        "        shape_sdf(circle_sdf(p, vec3(0., 1., 6.),  1.3), vec3(0., 1., 0.))\n"
        "    );\n"
        "\n"
        "    cur = union_sdf(\n"
        "        cur,\n"
        "        shape_sdf(surface_sdf(p, vec3(0., 1., 0.), - 1.), vec3(0., 0., 1.))\n"
        "    );\n"
        "\n"
        "    return cur;\n"
        "}\n"
        "\n"
        "shape_sdf ray_march(vec3 ro, vec3 rd)\n"
        "{\n"
        "    float dist = 0.;\n"
        "    shape_sdf dO;\n"
        "\n"
        "    for (int i = 0; i < MAX_STEPS; ++i)\n"
        "    {\n"
        "        dO = get_dist(ro + rd * dist);\n"
        "        dist += dO.dist;\n"
        "        if (dO.dist < SURF_DIST)\n"
        "            break;\n"
        "        if (dist > MAX_DIST)\n"
        "        {\n"
        "            dO.color = vec3(1.);\n"
        "            break;\n"
        "        }\n"
        "    }\n"
        "    dO.dist = dist;\n"
        "    return dO;\n"
        "}\n"
        "\n"
        "vec3 get_normal(vec3 p)\n"
        "{\n"
        "    vec2 e = vec2(SURF_DIST, 0.);\n"
        "    return normalize(get_dist(p).dist - vec3(get_dist(p-e.xyy).dist, get_dist(p-e.yxy).dist, get_dist(p-e.yyx).dist));\n"
        "}\n"
        "\n"
        "float get_light(vec3 p, vec3 lightPos)\n"
        "{\n"
        "    vec3 l = normalize(lightPos - p);\n"
        "    vec3 n = get_normal(p);\n"
        "    float d = ray_march(p + n * SURF_DIST * 2., l).dist;\n"
        "    float dif = clamp(dot(n, l), 0., 1.);\n"
        "    if (d < length(lightPos - p)) dif *= .1;\n"
        "    return dif;\n"
        "}\n"
        "\n"
        "vec4 Scene(vec3 ro, vec3 rd)\n"
        "{\n"
        "    shape_sdf d = ray_march(ro, rd);\n"
        "    vec3 p = ro + rd * d.dist;\n"
        "#if 1\n"
        "    float dif = get_light(p, vec3(0., 5., 6.) + 5. * vec3(sin(iTime), 0., cos(iTime)));\n"
        "    return vec4(d.color * dif, 1.);\n"
        "#else\n"
        "    return vec4(d.color, 1.);\n"
        "#endif\n"
        "}\n"
        "\n"
        "#define EYE .4\n"
        "void mainImage(out vec4 fragColor, in vec2 fragCoord)\n"
        "{\n"
        "    vec3 ro = vec3(0., 1., 0.);\n"
        "    vec3 uv = Stereo(iResolution.xy, fragCoord.xy);\n"
        "\n"
        "    uv.xy = fisheye(uv.xy);\n"
        "\n"
        "    ro.x = uv.z * EYE;\n"
        "    ro.y += .5 * sin(.25 * iTime);\n"
        "\n"
        "    uv.xy *= 1. + .5 * sin(.125 * iTime);\n"
        "\n"
        "    fragColor = Scene(ro, normalize(vec3(uv.xy, 1)));\n"
        "}"
#endif
        );
}

void GLScene::paintGL()
{
  m_vbo.bind();
  m_program->bind();
  m_program->enableAttributeArray(1);
  m_program->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);
  m_program->setUniformValue("iResolution", size().width() * 1.f, size().height() * 1.f, 0.f);
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
