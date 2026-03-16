#include <string>
#include <algorithm>

#ifndef CP_ALG_INT128
#define CP_ALG_INT128
namespace cp {
namespace alg {

__float128 stof128(const std::string& str) {
    size_t pos = 0;
    while (pos < str.size() && std::isspace(str[pos])) 
        ++pos;

    bool negative = false;
    if (pos < str.size() && (str[pos] == '-' || str[pos] == '+')) {
        if (str[pos] == '-') 
            negative = true;
        ++pos;
    }

    __float128 value = 0.0;
    while (pos < str.size() && std::isdigit(str[pos])) {
        value = value * 10.0 + (str[pos] - '0');
        ++pos;
    }

    if (pos < str.size() && str[pos] == '.') {
        ++pos;
        __float128 divisor = 10.0;
        while (pos < str.size() && std::isdigit(str[pos])) {
            value += static_cast<__float128>(str[pos] - '0') / divisor;
            divisor *= 10.0;
            ++pos;
        }
    }

    if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
        ++pos;
        bool exp_negative = false;
        if (pos < str.size() && (str[pos] == '-' || str[pos] == '+')) {
            if (str[pos] == '-') 
                exp_negative = true;
            ++pos;
        }
        int exp = 0;
        while (pos < str.size() && std::isdigit(str[pos])) {
            exp = exp * 10 + (str[pos] - '0');
            ++pos;
        }
        if (exp_negative) exp = -exp;
        value *= [&](int exp) -> __float128 {
                    __float128 result = 1.0;
                    __float128 base = 10.0;
                    if (exp < 0) {
                        base = 0.1;
                        exp = -exp;
                    }
                    for (int i = 0; i < exp; ++i) {
                        result *= base;
                    }
                    return result;
                }(exp);
    }

    if (negative) 
        value = -value;
    return value;
}

std::string to_string(__float128 value, int precision = 5) {
    bool is_negative = false;
    if (value < 0) {
        is_negative = true;
        value = -value;
    }

    unsigned long long int_part = (unsigned long long)value;
    __float128 frac_part = value - int_part;

    std::string int_str;
    if (int_part == 0) {
        int_str = "0";
    } else {
        while (int_part > 0) {
            char digit = '0' + (int_part % 10);
            int_str.push_back(digit);
            int_part /= 10;
        }
        std::reverse(int_str.begin(), int_str.end());
    }

    std::string frac_str;
    if (precision > 0) {
        frac_str.push_back('.');
        for (int i = 0; i < precision; ++i) {
            frac_part *= 10;
            int digit = static_cast<int>(frac_part);
            frac_str.push_back('0' + digit);
            frac_part -= digit;
            if (frac_part == 0) break;
        }
        while (frac_str.size() - 1 < static_cast<size_t>(precision)) {
            frac_str.push_back('0');
        }
    }

    std::string result = int_str + frac_str;
    if (is_negative) {
        result = "-" + result;
    }
    return result;
}

} // namespace alg
} // namespace cp
#endif