//<
#include <iostream>
#include <streambuf>
#include <cstdio>
#include <cstring> // Thêm thư viện này để dùng std::strlen

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

const char* color_start = "\x1b[36m";
const char* color_reset = "\033[0m";

class blue_buf : public std::streambuf {
    std::streambuf* base_buf;
    size_t start_len;
    size_t reset_len;
public:
    explicit blue_buf(std::streambuf* s) : base_buf(s) {
        // Tự động đếm số ký tự của mã màu để không bao giờ bị tràn bộ nhớ
        start_len = std::strlen(color_start);
        reset_len = std::strlen(color_reset);
    }

protected:
    int_type overflow(int_type c) override {
        if (c == traits_type::eof()) return traits_type::eof();
        base_buf->sputn(color_start, start_len); // Sửa ở đây
        int_type ret = base_buf->sputc(c);
        base_buf->sputn(color_reset, reset_len); // Sửa ở đây
        return ret;
    }

    std::streamsize xsputn(const char* s, std::streamsize n) override {
        base_buf->sputn(color_start, start_len); // Sửa ở đây
        std::streamsize ret = base_buf->sputn(s, n);
        base_buf->sputn(color_reset, reset_len); // Sửa ở đây
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

} // namespace std_color

#endif
//>