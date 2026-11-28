#include <cctype>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>

#ifndef CP_ALG_EVALUATE
#define CP_ALG_EVALUATE

namespace cp {

namespace internal {

template <typename T>
class expr_parser {
public:
    constexpr explicit expr_parser(std::string_view expr) noexcept
        : expr_(expr), pos_(0) {}

    [[nodiscard]] T parse() {
        T result = parse_expr();
        skip_ws();
        if (!eof()) {
            throw std::invalid_argument("eval: unexpected trailing character");
        }
        return result;
    }

private:
    std::string_view expr_;
    std::size_t pos_;

    [[nodiscard]] constexpr bool eof() const noexcept {
        return pos_ >= expr_.size();
    }

    [[nodiscard]] constexpr char peek() const noexcept {
        return eof() ? '\0' : expr_[pos_];
    }

    void skip_ws() noexcept {
        while (!eof() && std::isspace(static_cast<unsigned char>(expr_[pos_]))) {
            ++pos_;
        }
    }

    // expr := term (('+' | '-') term)*
    T parse_expr() {
        T lhs = parse_term();
        while (true) {
            skip_ws();
            char op = peek();
            if (op == '+' || op == '-') {
                ++pos_;
                T rhs = parse_term();
                lhs = (op == '+') ? (lhs + rhs) : (lhs - rhs);
            } else {
                break;
            }
        }
        return lhs;
    }

    // term := factor (('*' | '/' | '%') factor)*
    T parse_term() {
        T lhs = parse_factor();
        while (true) {
            skip_ws();
            char op = peek();
            if (op == '*' || op == '/' || op == '%') {
                ++pos_;
                T rhs = parse_factor();
                if (op == '*') {
                    lhs *= rhs;
                } else if (op == '/') {
                    if (rhs == 0) throw std::domain_error("eval: division by zero");
                    lhs /= rhs;
                } else {
                    if (rhs == 0) throw std::domain_error("eval: modulo by zero");
                    if constexpr (std::is_integral_v<T>) {
                        lhs %= rhs;
                    } else {
                        lhs = std::fmod(lhs, rhs);
                    }
                }
            } else {
                break;
            }
        }
        return lhs;
    }

    // factor := ('+' | '-') factor | primary
    T parse_factor() {
        skip_ws();
        char ch = peek();
        if (ch == '+') {
            ++pos_;
            return parse_factor();
        }
        if (ch == '-') {
            ++pos_;
            return -parse_factor();
        }
        return parse_primary();
    }

    // primary := '(' expr ')' | number
    T parse_primary() {
        skip_ws();
        if (eof()) {
            throw std::invalid_argument("eval: unexpected end of expression");
        }

        if (peek() == '(') {
            ++pos_;
            T val = parse_expr();
            skip_ws();
            if (peek() != ')') {
                throw std::invalid_argument("eval: missing closing ')'");
            }
            ++pos_;
            return val;
        }

        return parse_number();
    }

    T parse_number() {
        char ch = peek();
        if constexpr (std::is_floating_point_v<T>) {
            if (!std::isdigit(static_cast<unsigned char>(ch)) && ch != '.') {
                throw std::invalid_argument("eval: expected number or '('");
            }
        } else {
            if (!std::isdigit(static_cast<unsigned char>(ch))) {
                throw std::invalid_argument("eval: expected number or '('");
            }
        }

        const char* first = expr_.data() + pos_;
        const char* last  = expr_.data() + expr_.size();
        T value{};
        auto [ptr, ec] = std::from_chars(first, last, value);

        if (ec == std::errc::result_out_of_range) {
            throw std::out_of_range("eval: number out of range");
        }
        if (ec != std::errc{}) {
            throw std::invalid_argument("eval: invalid number");
        }

        pos_ = static_cast<std::size_t>(ptr - expr_.data());
        return value;
    }
};

} // namespace internal

namespace alg {
namespace math {

template <typename T = long long>
[[nodiscard]] T eval(std::string_view expr) {
    static_assert(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>,
                  "cp::eval<T>: T must be a numeric type");
    return internal::expr_parser<T>(expr).parse();
}

} // namespace math
} // namespace alg

} // namespace cp

#endif