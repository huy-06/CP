//<
#include <iostream>
#include <streambuf>
#include <cstdio>

#ifndef OUTPUT_COLOR
#define OUTPUT_COLOR

#ifdef _WIN32
    #include <io.h>
    #define check_tty() _isatty(_fileno(stdout))
#else
    #include <unistd.h>
    #define check_tty() isatty(fileno(stdout))
#endif

namespace std_color {

/*
#define ANSI_RESET_ALL          "\x1b[0m"

#define ANSI_COLOR_BLACK        "\x1b[30m"
#define ANSI_COLOR_RED          "\x1b[31m"
#define ANSI_COLOR_GREEN        "\x1b[32m"
#define ANSI_COLOR_YELLOW       "\x1b[33m"
#define ANSI_COLOR_BLUE         "\x1b[34m"
#define ANSI_COLOR_MAGENTA      "\x1b[35m"
#define ANSI_COLOR_CYAN         "\x1b[36m"
#define ANSI_COLOR_WHITE        "\x1b[37m"

#define ANSI_BACKGROUND_BLACK   "\x1b[40m"
#define ANSI_BACKGROUND_RED     "\x1b[41m"
#define ANSI_BACKGROUND_GREEN   "\x1b[42m"
#define ANSI_BACKGROUND_YELLOW  "\x1b[43m"
#define ANSI_BACKGROUND_BLUE    "\x1b[44m"
#define ANSI_BACKGROUND_MAGENTA "\x1b[45m"
#define ANSI_BACKGROUND_CYAN    "\x1b[46m"
#define ANSI_BACKGROUND_WHITE   "\x1b[47m"

#define ANSI_STYLE_BOLD         "\x1b[1m"
#define ANSI_STYLE_ITALIC       "\x1b[3m"
#define ANSI_STYLE_UNDERLINE    "\x1b[4m"
*/

const char* color_start = "\x1b[36m";
const char* color_reset = "\033[0m";

class blue_buf : public std::streambuf {
    std::streambuf* base_buf;
public:
    explicit blue_buf(std::streambuf* s) : base_buf(s) {}

protected:
    int_type overflow(int_type c) override {
        if (c == traits_type::eof()) return traits_type::eof();
        base_buf->sputn(color_start, 7);
        int_type ret = base_buf->sputc(c);
        base_buf->sputn(color_reset, 4);
        return ret;
    }

    std::streamsize xsputn(const char* s, std::streamsize n) override {
        base_buf->sputn(color_start, 7);
        std::streamsize ret = base_buf->sputn(s, n);
        base_buf->sputn(color_reset, 4);
        return ret;
    }

    int sync() override { return base_buf->pubsync(); }
};

struct color_guard {
    std::streambuf* old_buf;
    blue_buf custom_buf;
    bool is_active;

    color_guard() : old_buf(std::cout.rdbuf()), custom_buf(old_buf), is_active(false) {
        if (check_tty()) {
            std::cout.rdbuf(&custom_buf);
            is_active = true;
        }
    }

    ~color_guard() {
        if (is_active) {
            std::cout.rdbuf(old_buf);
        }
    }
};

static color_guard monitor;

} // namespace color

#endif
//>