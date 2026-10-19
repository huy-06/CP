#include <iostream>
#include <vector>
#include <type_traits>

namespace cp::ds {

struct flush_t {} flush;

struct endl_t {} endl;

class scanner {
private:
    static constexpr int BUF_SIZE = 1 << 16;
    char buf[BUF_SIZE];
    int pos = 0, len = 0;

    inline char next_char() {
        if (pos == len) {
            pos = 0;
            len = (int)std::fread(buf, 1, BUF_SIZE, stdin);
            if (len == 0) return EOF;
        }
        return buf[pos++];
    }

public:
    template <typename Tp>
    requires std::is_integral_v<Tp>
    scanner& operator>>(Tp& n) {
        char c;
        while ((c = next_char()) <= ' ' && c != EOF);
        if (c == EOF) return *this;
        
        bool sign = false;
        if constexpr (std::is_signed_v<Tp>) {
            if (c == '-') {
                sign = true;
                c = next_char();
            }
        }
        
        using uint = std::make_unsigned_t<Tp>;
        uint res = 0;
        while (c >= '0' && c <= '9') {
            res = res * 10 + (c - '0');
            c = next_char();
        }
        n = sign ? -static_cast<Tp>(res) : static_cast<Tp>(res);
        return *this;
    }

    scanner& operator>>(char& c) {
        while ((c = next_char()) <= ' ' && c != EOF);
        return *this;
    }

    scanner& operator>>(std::string& s) {
        s.clear();
        char c;
        while ((c = next_char()) <= ' ' && c != EOF);
        while (c > ' ' && c != EOF) {
            s += c;
            c = next_char();
        }
        return *this;
    }
};

class printer {
private:
    static constexpr int BUF_SIZE = 1 << 16;
    char buf[BUF_SIZE];
    int pos = 0;

    inline void write_char(char c) {
        if (pos == BUF_SIZE) do_flush();
        buf[pos++] = c;
    }

public:
    ~printer() { 
        do_flush();
    }

    inline void do_flush() {
        if (pos > 0) {
            std::fwrite(buf, 1, pos, stdout);
            pos = 0;
        }
        std::fflush(stdout); 
    }

    template <typename Tp>
    requires std::is_integral_v<Tp>
    printer& operator<<(Tp n) {
        if (n == 0) {
            write_char('0');
            return *this;
        }
        
        using uint = std::make_unsigned_t<Tp>;
        uint res = n;
        if constexpr (std::is_signed_v<Tp>) {
            if (n < 0) {
                write_char('-');
                res = -n;
            }
        }
        
        char stk[20];
        int top = 0;

        while (res > 0) {
            stk[top++] = (res % 10) + '0';
            res /= 10;
        }
        while (top > 0) write_char(stk[--top]);

        return *this;
    }

    printer& operator<<(char c) {
        write_char(c);
        return *this;
    }

    printer& operator<<(const char* s) {
        while (*s) write_char(*s++);
        return *this;
    }

    printer& operator<<(const std::string& s) {
        for (char c : s) write_char(c);
        return *this;
    }

    printer& operator<<(flush_t) {
        do_flush();
        return *this;
    }

    printer& operator<<(endl_t) {
        write_char('\n');
        do_flush();
        return *this;
    }
};

inline scanner input;
inline printer output;

} // namespace cp::ds