#pragma once
#define SHADER_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include <QOpenGLFunctions>

class shader : public QOpenGLFunctions
{
public:
  shader(QOpenGLContext* context = nullptr);
  virtual ~shader();
  inline void set_source(GLuint id, const std::string& src) { m_sources.emplace(id, src); }
  inline void set_source(GLuint id, const QString& src) { m_sources.emplace(id, src.toStdString()); }
  void compile();
  inline GLuint get() const { return m_program.get(); }
  inline GLint vertex() const { return m_vertex_location; }
  inline GLint iResolution() const { return m_iResolution_location; }
  inline GLint iTime() const { return m_iTime_location; }
  inline GLint iMouse() const { return m_iMouse_location; }

private:
  struct unique_gl_program_deleter
  {
    QOpenGLFunctions* gl { nullptr };
    struct pointer
    {
      pointer() : init { false } { }
      pointer(GLuint handle) : handle { handle }, init { true } { }
      pointer(std::nullptr_t) : init { false } { }
      inline operator GLuint () { return handle; }
      inline bool operator == (const pointer &other) const { return handle == other.handle; }
      inline bool operator != (const pointer &other) const { return handle != other.handle; }
      inline bool operator == (std::nullptr_t) const { return init == false; }
      inline bool operator != (std::nullptr_t) const { return init != false; }
      GLuint handle;
      bool init { false };
    };
    unique_gl_program_deleter() = default;
    unique_gl_program_deleter(QOpenGLFunctions* gl) : gl { gl } { }
    inline void operator () (GLuint handle) { if (gl != nullptr) gl->glDeleteProgram(handle); }
  };
  using unique_gl_program = std::unique_ptr<GLuint, unique_gl_program_deleter>;

  struct unique_gl_shader_deleter
  {
    QOpenGLFunctions* gl { nullptr };
    struct pointer
    {
      pointer() = default;
      pointer(GLuint handle) : handle { handle }, init { true } { }
      pointer(std::nullptr_t) : init { false } { }
      inline operator GLuint () { return handle; }
      inline bool operator == (const pointer &other) const { return init == other.init && handle == other.handle; }
      inline bool operator != (const pointer &other) const { return init != other.init || handle != other.handle; }
      inline bool operator == (std::nullptr_t) const { return init == false; }
      inline bool operator != (std::nullptr_t) const { return init != false; }
      GLuint handle;
      bool init { false };
    };
    unique_gl_shader_deleter() = default;
    unique_gl_shader_deleter(QOpenGLFunctions* gl) : gl { gl } { }
    inline void operator () (GLuint handle) { gl->glDeleteShader(handle); }
  };
  using unique_gl_shader = std::unique_ptr<GLuint, unique_gl_shader_deleter>;

private:
  std::vector<GLchar> m_fragment_error_log;
  std::vector<GLchar> m_vertex_error_log;
  std::vector<GLchar> m_program_error_log;
  std::map<GLuint, std::string> m_sources;
  unique_gl_program m_program;
  GLint m_vertex_location;
  GLint m_iResolution_location;
  GLint m_iTime_location;
  GLint m_iMouse_location;
};
