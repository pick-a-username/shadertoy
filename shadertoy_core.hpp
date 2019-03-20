#pragma once
#define SHADERTOY_CORE_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include <QOpenGLExtraFunctions>

namespace shadertoy
{
  struct id_map : std::unordered_map<std::string, GLint> { };
  struct line_status
  {
    enum { UNKNOWN, ERROR, WARNING } type { UNKNOWN };
    uint32_t line;
    uint32_t column;
    std::string description;
  };
  class core : public QOpenGLExtraFunctions
  {
  public:
    core(QOpenGLContext *context = nullptr);
    virtual ~core();
    inline void set_source(GLuint id, const char* src) { m_sources.emplace(id, "#line 0 " + std::to_string(id) + "\n" + src); }
    inline void set_source(GLuint id, const std::string& src) { m_sources.emplace(id, "#line 0 " + std::to_string(id) + "\n" + src); }
    inline void set_source(GLuint id, const QString& src) { m_sources.emplace(id, "#line 0 " + std::to_string(id) + "\n" + src.toStdString()); }
    inline void set_iChannel0(GLenum target) { m_iChannel_target[0] = target; }
    inline void set_iChannel1(GLenum target) { m_iChannel_target[1] = target; }
    inline void set_iChannel2(GLenum target) { m_iChannel_target[2] = target; }
    inline void set_iChannel3(GLenum target) { m_iChannel_target[3] = target; }
    bool compile();
    inline void begin()
    {
      glUseProgram(m_program.get());
      glBindBuffer(GL_ARRAY_BUFFER, m_buffer.get());
      glEnableVertexAttribArray(static_cast<GLuint>(m_vertex_location));
      glVertexAttribPointer(static_cast<GLuint>(m_vertex_location), 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }
    inline void iResolution(GLfloat width, GLfloat height, GLfloat v) { glUniform3f(m_iResolution_location, width, height, v); }
    inline void iTime(GLfloat time) { glUniform1f(m_iTime_location, time); }
    inline void iTimeDelta(GLfloat time) { glUniform1f(m_iTimeDelta_location, time); }
    inline void iFrame(GLint n) { glUniform1i(m_iFrame_location, n); }
    inline void iChannel0Time(GLfloat time) { m_iChannelTime[0] = time; }
    inline void iChannel1Time(GLfloat time) { m_iChannelTime[1] = time; }
    inline void iChannel2Time(GLfloat time) { m_iChannelTime[2] = time; }
    inline void iChannel3Time(GLfloat time) { m_iChannelTime[3] = time; }
    inline void iChannel0Resolution(GLfloat width, GLfloat height)
    {
      m_iChannelResolution[0][0] = width;
      m_iChannelResolution[0][1] = height;
    }
    inline void iChannel1Resolution(GLfloat width, GLfloat height)
    {
      m_iChannelResolution[1][0] = width;
      m_iChannelResolution[1][1] = height;
    }
    inline void iChannel2Resolution(GLfloat width, GLfloat height)
    {
      m_iChannelResolution[2][0] = width;
      m_iChannelResolution[2][1] = height;
    }
    inline void iChannel3Resolution(GLfloat width, GLfloat height)
    {
      m_iChannelResolution[3][0] = width;
      m_iChannelResolution[3][1] = height;
    }
    inline void iMouse(GLfloat posX, GLfloat posY, GLfloat clickX, GLfloat clickY) { glUniform4f(m_iMouse_location, posX, posY, clickX, clickY); }
    inline void iChannel0(GLint unit) { glUniform1i(m_iChannel0_location, unit); }
    inline void iChannel1(GLint unit) { glUniform1i(m_iChannel1_location, unit); }
    inline void iChannel2(GLint unit) { glUniform1i(m_iChannel2_location, unit); }
    inline void iChannel3(GLint unit) { glUniform1i(m_iChannel3_location, unit); }
    inline void iDate(GLuint year, GLuint month, GLuint day, GLfloat time) { glUniform4f(m_iDate_location, year, month, day, time); }
    inline void iSampleRate(GLfloat rate) { glUniform1f(m_iSampleRate_location, rate); }
    inline void end()
    {
      glUniform1fv(m_iChannelTime_location, m_iChannelTime.size(), m_iChannelTime.data());
      glUniform3fv(m_iChannelResolution_location, m_iChannelResolution.size(), m_iChannelResolution.data()[0]);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      for (auto& i : m_iChannelTime)
        i = .0f;
      for (auto& i : m_iChannelResolution)
        i[0] = i[1] = i[2] = .0f;
      glUseProgram(0);
    }
    id_map uniforms();
    id_map attributes();

  private:
    struct gl_program_deleter
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
      gl_program_deleter() = default;
      gl_program_deleter(QOpenGLFunctions* gl) : gl { gl } { }
      inline void operator () (GLuint handle) { if (gl != nullptr) gl->glDeleteProgram(handle); }
    };
    using gl_program_ptr = std::unique_ptr<GLuint, gl_program_deleter>;

    struct gl_shader_deleter
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
      gl_shader_deleter() = default;
      gl_shader_deleter(QOpenGLFunctions* gl) : gl { gl } { }
      inline void operator () (GLuint handle) { gl->glDeleteShader(handle); }
    };
    using gl_shader_ptr = std::unique_ptr<GLuint, gl_shader_deleter>;

    struct gl_buffer_deleter
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
      gl_buffer_deleter() = default;
      gl_buffer_deleter(QOpenGLFunctions* gl) : gl { gl } { }
      inline void operator () (GLuint handle) { gl->glDeleteBuffers(1, &handle); }
    };
    using gl_buffer_ptr = std::unique_ptr<GLuint, gl_buffer_deleter>;

  private:
    std::vector<GLchar> m_fragment_error_log;
    std::vector<GLchar> m_vertex_error_log;
    std::vector<GLchar> m_program_error_log;
    std::map<GLuint, std::string> m_sources;
    std::array<GLenum, 4> m_iChannel_target { GL_NONE, GL_NONE, GL_NONE, GL_NONE };
    std::array<GLfloat, 4> m_iChannelTime { .0f, .0f, .0f, .0f };
    std::array<GLfloat[3], 4> m_iChannelResolution;
    gl_program_ptr m_program;
    gl_buffer_ptr m_buffer;
    GLint m_vertex_location { -1 };
    GLint m_iResolution_location { -1 };
    GLint m_iTime_location { -1 };
    GLint m_iTimeDelta_location { -1 };
    GLint m_iFrame_location { -1 };
    GLint m_iChannelTime_location { -1 };
    GLint m_iChannelResolution_location { -1 };
    GLint m_iMouse_location { -1 };
    GLint m_iChannel0_location { -1 };
    GLint m_iChannel1_location { -1 };
    GLint m_iChannel2_location { -1 };
    GLint m_iChannel3_location { -1 };
    GLint m_iDate_location { -1 };
    GLint m_iSampleRate_location { -1 };
  };
}
