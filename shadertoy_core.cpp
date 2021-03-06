#include <QDebug>
#include <cstdlib>
#include <cstring>
#include "shadertoy_core.hpp"

inline const char* from_iChannel0(GLenum target)
{
  switch (target)
  {
  case GL_TEXTURE_1D: return "uniform sampler1D iChannel0;\n";
  case GL_TEXTURE_2D: return "uniform sampler2D iChannel0;\n";
  case GL_TEXTURE_3D: return "uniform sampler3D iChannel0;\n";
  case GL_TEXTURE_CUBE_MAP: return "uniform samplerCube iChannel0;\n";
  case GL_TEXTURE_RECTANGLE: return "uniform sampler2DRect iChannel0;\n";
  case GL_TEXTURE_1D_ARRAY: return "uniform sampler1DArray iChannel0;\n";
  case GL_TEXTURE_2D_ARRAY: return "uniform sampler2DArray iChannel0;\n";
  case GL_TEXTURE_CUBE_MAP_ARRAY: return "uniform samplerCubeArray iChannel0;\n";
  case GL_TEXTURE_BUFFER: return "uniform samplerBuffer iChannel0;\n";
  case GL_TEXTURE_2D_MULTISAMPLE: return "uniform sampler2DMS iChannel0;\n";
  case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return "uniform sampler2DMSArray iChannel0;\n";
  default: return "\n";
  }
}
inline const char* from_iChannel1(GLenum target)
{
  switch (target)
  {
  case GL_TEXTURE_1D: return "uniform sampler1D iChannel1;\n";
  case GL_TEXTURE_2D: return "uniform sampler2D iChannel1;\n";
  case GL_TEXTURE_3D: return "uniform sampler3D iChannel1;\n";
  case GL_TEXTURE_CUBE_MAP: return "uniform samplerCube iChannel1;\n";
  case GL_TEXTURE_RECTANGLE: return "uniform sampler2DRect iChannel1;\n";
  case GL_TEXTURE_1D_ARRAY: return "uniform sampler1DArray iChannel1;\n";
  case GL_TEXTURE_2D_ARRAY: return "uniform sampler2DArray iChannel1;\n";
  case GL_TEXTURE_CUBE_MAP_ARRAY: return "uniform samplerCubeArray iChannel1;\n";
  case GL_TEXTURE_BUFFER: return "uniform samplerBuffer iChannel1;\n";
  case GL_TEXTURE_2D_MULTISAMPLE: return "uniform sampler2DMS iChannel1;\n";
  case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return "uniform sampler2DMSArray iChannel1;\n";
  default: return "\n";
  }
}
inline const char* from_iChannel2(GLenum target)
{
  switch (target)
  {
  case GL_TEXTURE_1D: return "uniform sampler1D iChannel2;\n";
  case GL_TEXTURE_2D: return "uniform sampler2D iChannel2;\n";
  case GL_TEXTURE_3D: return "uniform sampler3D iChannel2;\n";
  case GL_TEXTURE_CUBE_MAP: return "uniform samplerCube iChannel2;\n";
  case GL_TEXTURE_RECTANGLE: return "uniform sampler2DRect iChannel2;\n";
  case GL_TEXTURE_1D_ARRAY: return "uniform sampler1DArray iChannel2;\n";
  case GL_TEXTURE_2D_ARRAY: return "uniform sampler2DArray iChannel2;\n";
  case GL_TEXTURE_CUBE_MAP_ARRAY: return "uniform samplerCubeArray iChannel2;\n";
  case GL_TEXTURE_BUFFER: return "uniform samplerBuffer iChannel2;\n";
  case GL_TEXTURE_2D_MULTISAMPLE: return "uniform sampler2DMS iChannel2;\n";
  case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return "uniform sampler2DMSArray iChannel2;\n";
  default: return "\n";
  }
}
inline const char* from_iChannel3(GLenum target)
{
  switch (target)
  {
  case GL_TEXTURE_1D: return "uniform sampler1D iChannel3;\n";
  case GL_TEXTURE_2D: return "uniform sampler2D iChannel3;\n";
  case GL_TEXTURE_3D: return "uniform sampler3D iChannel3;\n";
  case GL_TEXTURE_CUBE_MAP: return "uniform samplerCube iChannel3;\n";
  case GL_TEXTURE_RECTANGLE: return "uniform sampler2DRect iChannel3;\n";
  case GL_TEXTURE_1D_ARRAY: return "uniform sampler1DArray iChannel3;\n";
  case GL_TEXTURE_2D_ARRAY: return "uniform sampler2DArray iChannel3;\n";
  case GL_TEXTURE_CUBE_MAP_ARRAY: return "uniform samplerCubeArray iChannel3;\n";
  case GL_TEXTURE_BUFFER: return "uniform samplerBuffer iChannel3;\n";
  case GL_TEXTURE_2D_MULTISAMPLE: return "uniform sampler2DMS iChannel3;\n";
  case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return "uniform sampler2DMSArray iChannel3;\n";
  default: return "\n";
  }
}

namespace shadertoy
{
  log_type::log_type(const char* log)
  {
    auto ptr { log };
    while (ptr != nullptr)
    {
      char* ptr_end { nullptr };
      auto source_id { std::strtoul(ptr, &ptr_end, 0) };
      ptr = std::strchr(ptr_end, ':');
      if (ptr == nullptr)
        return;
      ++ ptr;
      line_description description;
      description.line = std::strtoul(ptr, &ptr_end, 0);
      ptr = std::strchr(ptr_end, '(');
      if (ptr == nullptr)
        return;
      ++ ptr;
      description.column = std::strtoul(ptr, &ptr_end, 0);
      ptr = std::strchr(ptr_end, ':');
      if (ptr == nullptr)
        return;
      ++ ptr;
      auto source_type_end { std::strchr(ptr, ':') };
      auto source_type_size { source_type_end - ptr };
      if (true
          && source_type_size == sizeof("error")
          && std::string(ptr, source_type_size) == " error")
        description.type = line_description::ERROR;
      else if (true
               && source_type_size == sizeof("warning")
               && std::string(ptr, source_type_size) == " warning")
        description.type = line_description::WARNING;
      else
        return;
      ptr = std::strchr(source_type_end, ' ');
      if (ptr == nullptr)
        return;
      ++ ptr;
      auto source_text_end { std::strchr(ptr, '\n') };
      if (source_text_end == nullptr)
      {
        description.text = std::string(ptr);
        ptr = source_text_end;
      }
      else
      {
        description.text = std::string(ptr, source_text_end);
        ptr = source_text_end + 1;
      }
      operator[](source_id).operator[](description.line).emplace_back(description);
    }
  }
  core::core(QOpenGLContext* context) : QOpenGLExtraFunctions(context)
  {
    static const GLfloat quad_coords[4][2] = { { -1.0f, -1.0f }, { -1.0f, +1.0f }, { +1.0f, +1.0f }, { +1.0f, -1.0f } };
    initializeOpenGLFunctions();
    GLuint bufId;
    glGenBuffers(1, &bufId);
    m_buffer = gl_buffer_ptr(bufId, gl_buffer_ptr::deleter_type(this));
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer.get());
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_coords), &quad_coords[0][0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  core::~core()
  {
  }
  bool core::compile()
  {
    //static char common_version[] { "#version 100\n" };
    static char common_version[] { "#version 120\n" };
    static char common_precision[] {
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
        "#endif\n" };
    static const char* vertex_src[] {
      common_version,
      common_precision,
      "attribute highp vec2 vertex;\n"
      "void main(void){gl_Position = vec4(vertex, 0.0, 1.0);}\n"
    };

    gl_shader_ptr vertex_shader { glCreateShader(GL_VERTEX_SHADER), gl_shader_ptr::deleter_type(this) };
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
        std::vector<GLchar> log(logSize);
        glGetShaderInfoLog(vertex_shader.get(), logSize, &logSize, &log[0]);
        m_vertex_log = std::make_unique<log_type>(log.data());
        qDebug() << "Vertex shader compile error:\n" << log.data();
        return false;
      }
    }

    std::vector<const char*> fragment_src;
    fragment_src.reserve(2 + 1 + 4 + m_sources.size() + 1);
    fragment_src.emplace_back(common_version);
    fragment_src.emplace_back(common_precision);
    fragment_src.emplace_back(
          "uniform vec3  iResolution;\n"           // viewport resolution (in pixels)
          "uniform float iTime;\n"                 // shader playback time (in seconds)
          "uniform float iTimeDelta;\n"            // render time (in seconds)
          "uniform int   iFrame;\n"                // shader playback frame
          "uniform float iChannelTime[4];\n"       // channel playback time (in seconds)
          "uniform vec3  iChannelResolution[4];\n" // channel resolution (in pixels)
          "uniform vec4  iMouse;\n"                // mouse pixel coords. xy: current (if MLB down), zw: click
          "uniform vec4  iDate;\n"                 // (year, month, day, time in seconds)
          "uniform float iSampleRate;\n"           // sound sample rate (i.e., 44100)
          "\n"
          );
    fragment_src.emplace_back(from_iChannel0(m_iChannel_target[0])); // input channel. XX = 2D/Cube
    fragment_src.emplace_back(from_iChannel1(m_iChannel_target[1]));
    fragment_src.emplace_back(from_iChannel2(m_iChannel_target[2]));
    fragment_src.emplace_back(from_iChannel3(m_iChannel_target[3]));
    for (auto& item : m_sources)
      fragment_src.emplace_back(item.second.c_str());

    fragment_src.emplace_back("void main(){mainImage(gl_FragColor,gl_FragCoord.xy);}");
    gl_shader_ptr fragment_shader { glCreateShader(GL_FRAGMENT_SHADER), gl_shader_ptr::deleter_type(this) };
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
        std::vector<GLchar> log(logSize);
        glGetShaderInfoLog(fragment_shader.get(), logSize, &logSize, &log[0]);
        m_fragment_log = std::make_unique<log_type>(log.data());
        qDebug() << "Fragment shader compile error:\n" << log.data();
        return false;
      }
    }

    m_program = gl_program_ptr { glCreateProgram(), gl_program_ptr::deleter_type(this) };
    glAttachShader(m_program.get(), vertex_shader.get());
    glAttachShader(m_program.get(), fragment_shader.get());
    glLinkProgram(m_program.get());
    // check link
    {
      GLint success { 0 };
      glGetProgramiv(m_program.get(), GL_LINK_STATUS, &success);
      if (success == GL_FALSE)
      {
        GLint logSize { 0 };
        glGetProgramiv(m_program.get(), GL_INFO_LOG_LENGTH, &logSize);
        std::vector<GLchar> log(logSize);
        glGetProgramInfoLog(m_program.get(), logSize, &logSize, &log[0]);
        m_program_log = std::make_unique<log_type>(log.data());
        qDebug() << "Program link error:\n" << log.data();
        glDetachShader(m_program.get(), vertex_shader.get());
        glDetachShader(m_program.get(), fragment_shader.get());
        m_program = nullptr;
        return false;
      }
    }
    m_vertex_location = glGetAttribLocation(m_program.get(), "vertex");
    m_iResolution_location = glGetUniformLocation(m_program.get(), "iResolution");
    m_iTime_location = glGetUniformLocation(m_program.get(), "iTime");
    m_iTimeDelta_location = glGetUniformLocation(m_program.get(), "iTimeDelta");
    m_iFrame_location = glGetUniformLocation(m_program.get(), "iFrame");
    m_iChannelTime_location = glGetUniformLocation(m_program.get(), "iChannelTime");
    m_iChannelResolution_location = glGetUniformLocation(m_program.get(), "iChannelResolution");
    m_iMouse_location = glGetUniformLocation(m_program.get(), "iMouse");
    m_iChannel0_location = glGetUniformLocation(m_program.get(), "iChannel0");
    m_iChannel1_location = glGetUniformLocation(m_program.get(), "iChannel1");
    m_iChannel2_location = glGetUniformLocation(m_program.get(), "iChannel2");
    m_iChannel3_location = glGetUniformLocation(m_program.get(), "iChannel3");
    m_iDate_location = glGetUniformLocation(m_program.get(), "iDate");
    m_iSampleRate_location = glGetUniformLocation(m_program.get(), "iSampleRate");
    return true;
  }
  id_map_type core::uniforms()
  {
    id_map_type r;
    GLuint count { 0 };
    glGetProgramInterfaceiv(m_program.get(), GL_UNIFORM, GL_ACTIVE_RESOURCES, reinterpret_cast<GLint*>(&count));
    GLenum properties[] { GL_NAME_LENGTH, GL_LOCATION };
    for (GLuint i { 0 }; i < count; ++i)
    {
     GLint results[2];
     glGetProgramResourceiv(m_program.get(), GL_UNIFORM, i, 2, properties, 2, nullptr, results);
     std::string name;
     name.resize(results[0] + 1, ' ');
     glGetProgramResourceName(m_program.get(), GL_UNIFORM, i, name.size(), nullptr, &name[0]);
     r.emplace(std::move(name), results[1]);
    }
    return r;
  }
  id_map_type core::attributes()
  {
    id_map_type r;
    GLuint count { 0 };
    glGetProgramInterfaceiv(m_program.get(), GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, reinterpret_cast<GLint*>(&count));
    GLenum properties[] { GL_NAME_LENGTH, GL_LOCATION};
    for (GLuint i { 0 }; i < count; ++i)
    {
     GLint results[2];
     glGetProgramResourceiv(m_program.get(), GL_PROGRAM_INPUT, i, 2, properties, 2, nullptr, results);
     std::string name;
     name.resize(results[0] + 1, ' ');
     glGetProgramResourceName(m_program.get(), GL_PROGRAM_INPUT, i, name.size(), nullptr, &name[0]);
     r.emplace(std::move(name), results[1]);
    }
    return r;
  }
}
