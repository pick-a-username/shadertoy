#include <QThread>
#include <QDebug>
#include <QMouseEvent>
#include "glscene.hpp"
#include "ui_glscene.h"

inline QDebug operator << (QDebug dbg, const shadertoy::id_map_type& v)
{
  dbg.nospace() << "shader_core::id_map(";
  auto delimer { false };
  for (auto& item : v)
  {
    if (delimer) dbg << ", ";
    dbg << item.first.c_str() << " = " << item.second;
    delimer = true;
  }
  dbg.nospace() << ")";
  return dbg.maybeSpace();
}

inline QDebug operator << (QDebug dbg, const shadertoy::log_type* log)
{
  if (log == nullptr)
  {
    dbg << "no log";
  }
  else
  {
    for (auto& source : *log)
    {
      dbg.nospace() << "Souuce " << source.first << '\n';
      for (auto& line : source.second)
      {
        for (auto& column: line.second)
        {
          dbg << column.line << ':' << column.column << "\t\"" << column.text.c_str() << "\"\n";
        }
        dbg << '\n';
      }
    }
  }
  return dbg.maybeSpace();
}

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
  killTimer(m_timer_handle);
  m_program.reset();
  doneCurrent();
}

void GLScene::setShader(const QString& source)
{
  m_program.reset(new shadertoy::core(context()));
  m_program->set_source(0,
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
                        );
  m_program->set_source(1, source);
  m_program->compile();
  qDebug() << m_program->uniforms();
  qDebug() << m_program->attributes();
  qDebug() << m_program->fragment_log();
  qDebug() << m_program->vertex_log();
  qDebug() << m_program->program_log();
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
  setShader(
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
        );
  m_timer_handle = startTimer(16);
}

void GLScene::paintGL()
{
  m_program->begin();
  m_program->iResolution(size().width(), size().height(), 0.f);
  m_program->iTime(m_startTime.msecsTo(QTime::currentTime()) * 1e-3f);
  m_program->iMouse(m_mousePosition.x(), m_mousePosition.y(), m_mouseClick.x(), m_mouseClick.y());
  m_program->end();
}

void GLScene::resizeGL(int width, int height)
{
  emit resizedGL(width, height);
}

void GLScene::mousePressEvent(QMouseEvent* event)
{
  m_mouseClick = event->pos();
}

void GLScene::mouseMoveEvent(QMouseEvent* event)
{
  m_mousePosition = event->pos();
}

void GLScene::mouseReleaseEvent(QMouseEvent* event)
{
}

void GLScene::timerEvent(QTimerEvent *event)
{
  repaint();
}

void GLScene::dump_glerror(int line)
{
  for (;;)
  {
    auto err { glGetError() };
    if (err == GL_NO_ERROR)
      return;
    switch (err)
    {
    case GL_INVALID_ENUM: qDebug() << "GL_INVALID_ENUM"; break;
    case GL_INVALID_VALUE: qDebug() << "GL_INVALID_VALUE"; break;
    case GL_INVALID_OPERATION: qDebug() << "GL_INVALID_OPERATION"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION: qDebug() << "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
    case GL_OUT_OF_MEMORY: qDebug() << "GL_OUT_OF_MEMORY"; break;
    case GL_STACK_UNDERFLOW: qDebug() << "GL_STACK_UNDERFLOW"; break;
    case GL_STACK_OVERFLOW: qDebug() << "GL_STACK_OVERFLOW"; break;
    default: qDebug() << line << err;
    }
  }
}
