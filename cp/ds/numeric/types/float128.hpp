#include <string>
#include <iostream>
#include "../../../alg/math/arithmetic/float128.hpp"

#ifndef CP_DS_FLOAT128
#define CP_DS_FLOAT128
namespace cp {
namespace ds {

class float128 {
public:
    constexpr float128() : value(0.0) {}
    
    template <typename Tp>
    constexpr float128(Tp v) : value(v) {}

    constexpr float128(const char* s) : value(0.0) {
        if (!s || *s == '\0') return;
        bool is_neg = false;
        if (*s == '-') { is_neg = true; ++s; }
        else if (*s == '+') { ++s; }

        __float128 int_part = 0;
        while (*s >= '0' && *s <= '9') {
            int_part = int_part * 10 + (*s - '0');
            ++s;
        }

        __float128 frac_part = 0;
        __float128 div = 1.0;
        if (*s == '.') {
            ++s;
            while (*s >= '0' && *s <= '9') {
                frac_part = frac_part * 10 + (*s - '0');
                div *= 10.0;
                ++s;
            }
        }

        value = int_part + frac_part / div;

        if (*s == 'e' || *s == 'E') {
            ++s;
            bool exp_neg = false;
            if (*s == '-') { exp_neg = true; ++s; }
            else if (*s == '+') { ++s; }
            int exp = 0;
            while (*s >= '0' && *s <= '9') {
                exp = exp * 10 + (*s - '0');
                ++s;
            }
            __float128 mult = 1.0;
            __float128 base = 10.0;
            while (exp > 0) {
                if (exp % 2 == 1) mult *= base;
                base *= base;
                exp /= 2;
            }
            if (exp_neg) value /= mult;
            else value *= mult;
        }

        if (is_neg) value = -value;
    }

    constexpr explicit operator double() const { 
        return static_cast<double>(value);
    }

    constexpr explicit operator long double() const {
        return static_cast<long double>(value);
    }

    constexpr explicit operator long long() const {
        return static_cast<long long>(value);
    }

    constexpr explicit operator bool() const {
        return value != 0.0;
    }

    constexpr __float128 val() const {
        return value;
    }

    constexpr float128& operator+=(const float128& rhs) {
        value += rhs.value;
        return *this;
    }

    constexpr float128& operator-=(const float128& rhs) {
        value -= rhs.value;
        return *this;
    }

    constexpr float128& operator*=(const float128& rhs) {
        value *= rhs.value;
        return *this;
    }

    constexpr float128& operator/=(const float128& rhs) {
        value /= rhs.value;
        return *this;
    }

    friend constexpr float128 operator+(float128 lhs, const float128& rhs) { 
        lhs += rhs;
        return lhs;
    }

    friend constexpr float128 operator-(float128 lhs, const float128& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr float128 operator*(float128 lhs, const float128& rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend constexpr float128 operator/(float128 lhs, const float128& rhs) {
        lhs /= rhs;
        return lhs;
    }

    constexpr float128 operator-() const {
        return float128(-value);
    }

    constexpr float128 operator+() const {
        return *this;
    }

    friend constexpr bool operator==(const float128& lhs, const float128& rhs) {
        return lhs.value == rhs.value;
    }

    friend constexpr bool operator!=(const float128& lhs, const float128& rhs) {
        return lhs.value != rhs.value;
    }

    friend constexpr bool operator<(const float128& lhs, const float128& rhs) {
        return lhs.value < rhs.value;
    }

    friend constexpr bool operator>(const float128& lhs, const float128& rhs) {
        return lhs.value > rhs.value;
    }

    friend constexpr bool operator<=(const float128& lhs, const float128& rhs) {
        return lhs.value <= rhs.value;
    }

    friend constexpr bool operator>=(const float128& lhs, const float128& rhs) {
        return lhs.value >= rhs.value;
    }

    friend std::ostream& operator<<(std::ostream& os, const float128& num) {
        __float128 v = num.value;
        
        if (v >= 3.4e38 || v <= -3.4e38) {
            return os << static_cast<double>(v);
        }

        if (v < 0) {
            os << '-';
            v = -v;
        }
        
        int prec = os.precision();
        
        __float128 round_adder = 0.5;
        for (int i = 0; i < prec; ++i) round_adder /= 10.0;
        v += round_adder;
        
        unsigned __int128 int_part = (unsigned __int128)v;
        __float128 frac_part = v - (__float128)int_part;
        
        if (int_part == 0) {
            os << '0';
        } else {
            std::string s_int = "";
            while (int_part > 0) {
                s_int += (char)('0' + (int_part % 10));
                int_part /= 10;
            }
            std::reverse(s_int.begin(), s_int.end());
            os << s_int;
        }
        
        if (prec > 0 || (os.flags() & std::ios::showpoint)) {
            os << '.';
            for (int i = 0; i < prec; ++i) {
                frac_part *= 10;
                int digit = (int)frac_part;
                if (digit < 0) digit = 0;
                if (digit > 9) digit = 9;
                os << digit;
                frac_part -= digit;
            }
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, float128& num) {
        std::string s;
        if (is >> s) num = float128(s.c_str());
        return is;
    }


    friend constexpr float128 abs(const float128& num) {
        return num.value < 0 ? -num : num;
    }

    friend constexpr float128 floor(const float128& num) {
        long long int_part = (long long)num.value;
        if (num.value < int_part) return float128(int_part - 1);
        return float128(int_part);
    }
    
    friend constexpr float128 ceil(const float128& num) {
        long long int_part = (long long)num.value;
        if (num.value > int_part) return float128(int_part + 1);
        return float128(int_part);
    }

    friend constexpr float128 sqrt(const float128& num) {
        if (num.value < 0.0) return float128(-1.0);
        if (num.value == 0.0) return float128(0.0);
        
        __float128 x = num.value;
        __float128 guess = (double)num.value > 0 ? __builtin_sqrt((double)num.value) : 1.0;
        
        for (int i = 0; i < 10; ++i) {
            guess = 0.5 * (guess + x / guess);
        }
        return float128(guess);
    }

private:
    __float128 value;    
};

} // namespace ds
} // namespace cp
#endif