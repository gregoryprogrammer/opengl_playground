#ifndef OGP_UTILS_H
#define OGP_UTILS_H

#include "ogp_defines.h"

#include <cstdio>
#include <string.h>

#include <iostream>

enum LogLevel {
    SYSTEM,   // for versions
    INFO,     // for spreading good news about resources
    DEBUG,    // for me
    WARNING,  // for leaks
    ERROR,    // for disasters
    ME,       // for user
    TODO      // for developer
};

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOGFORMAT "%-16s  %-26s : %-24s  >  "

// extract relative path filename
#define __LOCALFILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define ogp_log_system(...)  do { ogp::ogplog(LogLevel::SYSTEM,  __LOCALFILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); } while (0)
#define ogp_log_info(...)    do { ogp::ogplog(LogLevel::INFO,    __LOCALFILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); } while (0)
#define ogp_log_debug(...)   do { ogp::ogplog(LogLevel::DEBUG,   __LOCALFILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); } while (0)
#define ogp_log_warning(...) do { ogp::ogplog(LogLevel::WARNING, __LOCALFILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); } while (0)
#define ogp_log_error(...)   do { ogp::ogplog(LogLevel::ERROR,   __LOCALFILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); } while (0)
#define ogp_log_me(...)      do { ogp::ogplog(LogLevel::ME,      __LOCALFILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); } while (0)
#define ogp_log_todo(...)    do { ogp::ogplog(LogLevel::TODO,    __LOCALFILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); } while (0)

#define BUFFER_OFFSET(i) ((void*)(i))
#define NOT_USED(x) ((void)(x))

#define TODO(x) do { \
    static bool seen_this = false; \
    if (!seen_this) { \
        seen_this = true; \
        ogp_log_todo("TODO: %s at %s (%s:%d)", \
        x, __FUNCTION__, __LOCALFILENAME__, __LINE__); \
    } \
} while(0)

#define PRINT_IVEC2(v) do { \
    ogp_log_debug("%s = (%d, %d)", #v, v.x, v.y); \
} while (0)

#define PRINT_VEC2(v) do { \
    ogp_log_debug("%s = (%f, %f)", #v, v.x, v.y); \
} while (0)

#define PRINT_VEC3(v) do { \
    ogp_log_debug("%s = (%f, %f, %f)", #v, v.x, v.y, v.z); \
} while (0)

#define PRINT_FLOAT(v) do { \
    ogp_log_debug("%s = %f", #v, v); \
} while (0)

#define PRINT_INT(v) do { \
    ogp_log_debug("%s = %d", #v, v); \
} while (0)

#define PRINT_UINT(v) do { \
    ogp_log_debug("%s = %u", #v, v); \
} while (0)

#define PRINT_INDEX(v) do { \
    ogp_log_debug("%s: value = %d, version = %d", #v, v.value, v.version); \
} while (0)

#define NULL_WARNING(x) do { \
    if (x == nullptr) ogp_log_warning("NULL: %s", #x); \
} while (0)

namespace ogp
{

void ogplog(LogLevel level, const char *filename, const char *function, i32 line, const char *fmt, ...);

void Mr_Terminator();

void terminate(char const *msg);

void print_working_dir();

void print_size_info();

char *file_read(char const *filename);

struct cmp_str
{
   bool operator() (char const *a, char const *b) const;
};

constexpr f32 angle_unwind(f32 value)
{
    // TODO("check this and fixit");
    return (value - static_cast<i32>(value / 180) * 360.0f);
}

template <typename T>
T clamp(T a, T min_val, T max_val)
{
    if (a < min_val) a = min_val;
    else if (a > max_val) a = max_val;
    return a;
}

template <typename T>
i32 between(T x, T a, T b)
{
    if (x >= a && x <= b) return 1;
    return 0;
}

template <typename T> i32 sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

template <typename T>
class smooth_t
{
    T m_now;
    T m_next;

public:

    smooth_t() = delete;

    smooth_t(T x)
    {
        m_now = x;
        m_next = x;
    }

    void set(T const &x)
    {
        m_now = x;
        m_next = x;
    }

    void step(T x)
    {
        std::swap(m_now, m_next);
        std::swap(m_next, x);
    }

    T const get(f32 dt = 0.0f) const
    {
        T result = (m_next * dt) + (m_now * (1.0f - dt));
        return result;
    }

    T const get_now() const { return m_now; }

    T const get_next() const { return m_next; }
};

}  // namespace ogp

#endif  // OGP_UTILS_H
