#include <QDebug>
#include "shader.hpp"

shader::shader(QOpenGLContext* context)
  : QOpenGLFunctions(context)
{
  initializeOpenGLFunctions();
}

shader::~shader()
{
}

void shader::compile()
{
  //static char common_version[] = "#version 100\n";
  static char common_version[] = "#version 120\n";
  static char common_precision[] =
      "#ifdef GL_ES\n"
      "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
      "precision highp float;\n"
      "#else\n"
      "precision mediump float;\n"
      "#endif\n"
      "#else\n"
      "#define lowp\n"
      "#define mediump\n"
      "#define highp\n"
      "#endif\n";
  static const char* vertex_src[] = {
    common_version,
    common_precision,
    "attribute highp vec2 vertex;\n"
    "void main(void){gl_Position = vec4(vertex, 0.0, 1.0);}\n"
  };

  unique_gl_shader vertex_shader(glCreateShader(GL_VERTEX_SHADER), unique_gl_shader_deleter(this));
  glShaderSource(vertex_shader.get(), sizeof(vertex_src) / sizeof(vertex_src[0]), vertex_src, nullptr);
  glCompileShader(vertex_shader.get());
  // check compile
  {
    GLint success { 0 };
    glGetShaderiv(vertex_shader.get(), GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
      GLint logSize { 0 };
      glGetShaderiv(vertex_shader.get(), GL_INFO_LOG_LENGTH, &logSize);
      m_fragment_error_log.resize(logSize);
      glGetShaderInfoLog(vertex_shader.get(), logSize, &logSize, &m_fragment_error_log[0]);
      qDebug() << __LINE__ << m_fragment_error_log.data();
      return;
    }
  }

  std::vector<const char*> fragment_src;
  fragment_src.emplace_back(common_version);
  fragment_src.emplace_back(common_precision);
  fragment_src.emplace_back(
        "uniform vec3      iResolution;\n"           // viewport resolution (in pixels)
        "uniform float     iTime;\n"                 // shader playback time (in seconds)
        //"uniform float     iTimeDelta;\n"            // render time (in seconds)
        //"uniform int       iFrame;\n"                // shader playback frame
        //"uniform float     iChannelTime[4];\n"       // channel playback time (in seconds)
        //"uniform vec3      iChannelResolution[4];\n" // channel resolution (in pixels)
        "uniform vec4      iMouse;\n"                // mouse pixel coords. xy: current (if MLB down), zw: click
        //"uniform samplerXX iChannel0..3;\n"          // input channel. XX = 2D/Cube
        //"uniform vec4      iDate;\n"                 // (year, month, day, time in seconds)
        //"uniform float     iSampleRate;\n"           // sound sample rate (i.e., 44100)
        "\n"
        );
  for (auto& item : m_sources)
    fragment_src.emplace_back(item.second.c_str());

  fragment_src.emplace_back("\nvoid main(void){mainImage(gl_FragColor, gl_FragCoord.xy);}\n");
  unique_gl_shader fragment_shader(glCreateShader(GL_FRAGMENT_SHADER), unique_gl_shader_deleter(this));
  glShaderSource(fragment_shader.get(), fragment_src.size(), fragment_src.data(), nullptr);
  glCompileShader(fragment_shader.get());
  // check compile
  {
    GLint success { 0 };
    glGetShaderiv(fragment_shader.get(), GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
      GLint logSize { 0 };
      glGetShaderiv(fragment_shader.get(), GL_INFO_LOG_LENGTH, &logSize);
      m_fragment_error_log.resize(logSize);
      glGetShaderInfoLog(fragment_shader.get(), logSize, &logSize, &m_fragment_error_log[0]);
      qDebug() << __LINE__ << m_fragment_error_log.data();
      return;
    }
  }

  m_program = unique_gl_program(glCreateProgram(), unique_gl_program_deleter(this));
  glAttachShader(m_program.get(), vertex_shader.get());
  glAttachShader(m_program.get(), fragment_shader.get());
  glLinkProgram(m_program.get());
  // check link
  {
    GLint success { 0 };
    glGetProgramiv(m_program.get(), GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetProgramiv(m_program.get(), GL_INFO_LOG_LENGTH, &maxLength);
      m_program_error_log.resize(maxLength);
      glGetProgramInfoLog(m_program.get(), maxLength, &maxLength, &m_program_error_log[0]);
      qDebug() << __LINE__ << m_program_error_log.data();
      m_program = nullptr;
    }
  }
  glDetachShader(m_program.get(), vertex_shader.get());
  glDetachShader(m_program.get(), fragment_shader.get());
  m_vertex_location = glGetAttribLocation(m_program.get(), "vertex");
  m_iResolution_location = glGetUniformLocation(m_program.get(), "iResolution");
  m_iTime_location = glGetUniformLocation(m_program.get(), "iTime");
  m_iMouse_location = glGetUniformLocation(m_program.get(), "iMouse");
  qDebug() << m_vertex_location << m_iResolution_location << m_iTime_location << m_iMouse_location;
}
