#include <string>
#include <iostream>

#ifndef CP_DS_INT128
#define CP_DS_INT128
namespace cp {
namespace internal {

template <typename Tp>
class base128 {
public:
    using value_type = Tp;

    constexpr base128() : value(0) {}

    template <typename U>
    requires std::is_integral_v<U> || std::is_same_v<U, __int128> || std::is_same_v<U, unsigned __int128>
    constexpr base128(U v) : value(static_cast<value_type>(v)) {}

    template <typename U>
    constexpr base128(const base128<U>& rhs) : value(static_cast<value_type>(rhs.val())) {}

    constexpr base128(const char* s) : value(0) {
        if (!s || s[0] == '\0') return;
        bool is_negative = (s[0] == '-');
        int i = (is_negative || s[0] == '+') ? 1 : 0;

        value_type temp = 0;
        for (; s[i] != '\0'; ++i) {
            if (s[i] >= '0' && s[i] <= '9') {
                temp = temp * 10 + (s[i] - '0');
            }
        }
        value = is_negative ? -temp : temp;
    }

    constexpr explicit operator value_type() const {
        return value;
    }

    constexpr explicit operator long long() const {
        return static_cast<long long>(value);
    }

    constexpr explicit operator int() const {
        return static_cast<int>(value);
    }

    constexpr explicit operator bool() const {
        return value != 0;
    }

    constexpr value_type val() const {
        return value;
    }

    constexpr base128& operator+=(const base128& rhs) {
        value += rhs.value;
        return *this;
    }

    constexpr base128& operator-=(const base128& rhs) {
        value -= rhs.value;
        return *this;
    }

    constexpr base128& operator*=(const base128& rhs) {
        value *= rhs.value;
        return *this;
    }

    constexpr base128& operator/=(const base128& rhs) {
        value /= rhs.value;
        return *this;
    }

    constexpr base128& operator%=(const base128& rhs) {
        value %= rhs.value;
        return *this;
    }

    constexpr base128& operator&=(const base128& rhs) {
        value &= rhs.value;
        return *this;
    }

    constexpr base128& operator|=(const base128& rhs) {
        value |= rhs.value;
        return *this;
    }

    constexpr base128& operator^=(const base128& rhs) {
        value ^= rhs.value;
        return *this;
    }

    constexpr base128& operator<<=(int shift) {
        value <<= shift;
        return *this;
    }

    constexpr base128& operator>>=(int shift) {
        value >>= shift;
        return *this;
    }

    friend constexpr base128 operator+(base128 lhs, const base128& rhs) {
        lhs += rhs;
        return lhs;
    }

    friend constexpr base128 operator-(base128 lhs, const base128& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr base128 operator*(base128 lhs, const base128& rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend constexpr base128 operator/(base128 lhs, const base128& rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend constexpr base128 operator%(base128 lhs, const base128& rhs) {
        lhs %= rhs;
        return lhs;
    }

    friend constexpr base128 operator&(base128 lhs, const base128& rhs) {
        lhs &= rhs;
        return lhs;
    }

    friend constexpr base128 operator|(base128 lhs, const base128& rhs) {
        lhs |= rhs;
        return lhs;
    }

    friend constexpr base128 operator^(base128 lhs, const base128& rhs) {
        lhs ^= rhs;
        return lhs;
    }

    friend constexpr base128 operator<<(base128 lhs, int shift) {
        lhs <<= shift;
        return lhs;
    }

    friend constexpr base128 operator>>(base128 lhs, int shift) {
        lhs >>= shift;
        return lhs;
    }

    constexpr base128 operator~() const {
        return base128(~value);
    }

    constexpr base128 operator-() const { 
        return base128(-value);
    }

    constexpr base128 operator+() const {
        return *this;
    }

    constexpr base128& operator++() {
        ++value;
        return *this;
    }

    constexpr base128 operator++(int) {
        base128 tmp(*this);
        ++value;
        return tmp;
    }

    constexpr base128& operator--() {
        --value;
        return *this;
    }

    constexpr base128 operator--(int) {
        base128 tmp(*this);
        --value;
        return tmp;
    }

    friend constexpr bool operator==(const base128& lhs, const base128& rhs) {
        return lhs.value == rhs.value;
    }

    friend constexpr bool operator!=(const base128& lhs, const base128& rhs) {
        return lhs.value != rhs.value;
    }

    friend constexpr bool operator<(const base128& lhs, const base128& rhs) {
        return lhs.value < rhs.value;
    }

    friend constexpr bool operator>(const base128& lhs, const base128& rhs) {
        return lhs.value > rhs.value;
    }

    friend constexpr bool operator<=(const base128& lhs, const base128& rhs) {
        return lhs.value <= rhs.value;
    }

    friend constexpr bool operator>=(const base128& lhs, const base128& rhs) {
        return lhs.value >= rhs.value;
    }

    friend constexpr base128 abs(const base128& num) {
        return num.value < 0 ? -num : num;
    }

    friend std::ostream& operator<<(std::ostream& os, const base128& num) {
        if (num.value == 0) return os << "0";
        unsigned __int128 un = num.value < 0 ? -static_cast<unsigned __int128>(num.value) : static_cast<unsigned __int128>(num.value);
        std::string s = "";
        while (un > 0) {
            s += static_cast<char>('0' + (un % 10));
            un /= 10;
        }
        if (num.value < 0) s += '-';
        std::reverse(s.begin(), s.end());
        return os << s;
    }

    friend std::istream& operator>>(std::istream& is, base128& num) {
        std::string s;
        if (is >> s) num = base128(s.c_str());
        return is;
    }

    std::string to_string() const {
        if (value == 0) return "0";
        unsigned __int128 un = (value < 0) ? -static_cast<unsigned __int128>(value) : static_cast<unsigned __int128>(value);
        std::string s = "";
        while (un > 0) {
            s += static_cast<char>('0' + (un % 10));
            un /= 10;
        }
        if (value < 0) s += '-';
        std::reverse(s.begin(), s.end());
        return s;
    }

private:
    value_type value;
};

} // namespace internal

namespace ds {

using int128 = internal::base128<__int128>;
using uint128 = internal::base128<unsigned __int128>;

} // namespace ds

} // namespace cp
#endif