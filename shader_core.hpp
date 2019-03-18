#pragma once
#define SHADER_CORE_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include <QOpenGLExtraFunctions>

class shader_core : public QOpenGLExtraFunctions
{
public:
  shader_core(QOpenGLContext *context = nullptr);
  virtual ~shader_core();
  inline void set_source(GLuint id, const std::string& src) { m_sources.emplace(id, src); }
  inline void set_source(GLuint id, const QString& src) { m_sources.emplace(id, src.toStdString()); }
  inline void set_iChannel(GLuint id, GLenum target) { m_iChannel_target.at(id) = target; }
  inline void set_iChannel0(GLenum target) { m_iChannel_target[0] = target; }
  inline void set_iChannel1(GLenum target) { m_iChannel_target[1] = target; }
  inline void set_iChannel2(GLenum target) { m_iChannel_target[2] = target; }
  inline void set_iChannel3(GLenum target) { m_iChannel_target[3] = target; }
  void compile();
  inline GLuint get() const { return m_program.get(); }
  inline GLuint vertex() const { return static_cast<GLuint>(m_vertex_location); }
  inline GLint iResolution() const { return m_iResolution_location; }
  inline GLint iTime() const { return m_iTime_location; }
  inline GLint iTimeDelta() const { return m_iTimeDelta_location; }
  inline GLint iFrame() const { return m_iFrame_location; }
  inline GLint iChannelTime() const { return m_iChannelTime_location; }
  inline GLint iChannelResolution() const { return m_iChannelResolution_location; }
  inline GLint iMouse() const { return m_iMouse_location; }
  inline GLint iChannel0() const { return m_iChannel0_location; }
  inline GLint iChannel1() const { return m_iChannel1_location; }
  inline GLint iChannel2() const { return m_iChannel2_location; }
  inline GLint iChannel3() const { return m_iChannel3_location; }
  inline GLint iDate() const { return m_iDate_location; }
  inline GLint iSampleRate() const { return m_iSampleRate_location; }
  std::unordered_map<std::string, GLint> uniforms();
  std::unordered_map<std::string, GLint> attributes();

private:
  template<class QOpenGLFunctions>
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
  template<class QOpenGLFunctions>
  using unique_gl_program = std::unique_ptr<GLuint, unique_gl_program_deleter<QOpenGLFunctions>>;

  template<class QOpenGLFunctions>
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
  template<class QOpenGLFunctions>
  using unique_gl_shader = std::unique_ptr<GLuint, unique_gl_shader_deleter<QOpenGLFunctions>>;

private:
  using gl_program_ptr = unique_gl_program<QOpenGLExtraFunctions>;
  using gl_shader_ptr = unique_gl_shader<QOpenGLExtraFunctions>;
  std::vector<GLchar> m_fragment_error_log;
  std::vector<GLchar> m_vertex_error_log;
  std::vector<GLchar> m_program_error_log;
  std::map<GLuint, std::string> m_sources;
  std::array<GLenum, 4> m_iChannel_target { GL_NONE, GL_NONE, GL_NONE, GL_NONE };
  gl_program_ptr m_program;
  GLint m_vertex_location;
  GLint m_iResolution_location;
  GLint m_iTime_location;
  GLint m_iTimeDelta_location;
  GLint m_iFrame_location;
  GLint m_iChannelTime_location;
  GLint m_iChannelResolution_location;
  GLint m_iMouse_location;
  GLint m_iChannel0_location;
  GLint m_iChannel1_location;
  GLint m_iChannel2_location;
  GLint m_iChannel3_location;
  GLint m_iDate_location;
  GLint m_iSampleRate_location;
};
