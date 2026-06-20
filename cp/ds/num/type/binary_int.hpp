#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <stdexcept>

#ifndef CP_DS_BINARY_INT
#define CP_DS_BINARY_INT
namespace cp {
namespace ds {

template <size_t N>
class binary_int {
public:
    constexpr binary_int() : bits(0) {}

    constexpr binary_int(const std::bitset<N>& b) : bits(b) {}

    constexpr binary_int(long long val) {
        if (val >= 0) {
            bits = std::bitset<N>(val);
        } else {
            bits = std::bitset<N>(-val);
            bits.flip();
            bool carry = true;
            for (size_t i = 0; i < N && carry; ++i) {
                bool val_bit = bits[i];
                bits[i] = val_bit ^ carry;
                carry = val_bit && carry;
            }
        }
    }

    constexpr binary_int(const std::string& s) {
        if (s.empty()) return;
        
        bool is_neg = false;
        size_t start_idx = 0;
        
        if (s[0] == '-') {
            is_neg = true;
            start_idx = 1;
        } else if (s[0] == '+') {
            start_idx = 1;
        }

        binary_int ten(10);
        
        for (size_t i = start_idx; i < s.length(); ++i) {
            if (s[i] < '0' || s[i] > '9') {
                continue;
            }
            *this = (*this * ten) + binary_int(s[i] - '0');
        }

        if (is_neg) {
            *this = -(*this);
        }
    }

    constexpr bool is_negative() const {
        return bits.test(N - 1);
    }

    constexpr binary_int abs() const {
        return is_negative() ? -(*this) : *this;
    }

    constexpr binary_int operator~() const {
        return binary_int(~bits);
    }

    friend constexpr binary_int operator&(const binary_int& lhs, const binary_int& rhs) {
        return binary_int(lhs.bits & rhs.bits);
    }

    friend constexpr binary_int operator|(const binary_int& lhs, const binary_int& rhs) {
        return binary_int(lhs.bits | rhs.bits);
    }

    friend constexpr binary_int operator^(const binary_int& lhs, const binary_int& rhs) {
        return binary_int(lhs.bits ^ rhs.bits);
    }


    friend constexpr binary_int operator<<(const binary_int& lhs, const size_t& shift) {
        return binary_int(lhs.bits << shift);
    }

    friend constexpr binary_int operator>>(const binary_int& lhs, const size_t& shift) {
        if (shift >= N) {
            return is_negative() ? binary_int(~std::bitset<N>(0)) : binary_int(0);
        }
        std::bitset<N> res = lhs.bits >> shift;
        if (is_negative()) {
            std::bitset<N> mask;
            mask.set();
            mask <<= (N - shift);
            res |= mask;
        }
        return binary_int(res);
    }

    friend constexpr binary_int operator&=(binary_int& lhs, const binary_int& rhs) {
        return lhs = lhs & rhs;
    }

    friend constexpr binary_int operator|=(binary_int& lhs, const binary_int& rhs) {
        return lhs = lhs | rhs;
    }
    
    friend constexpr binary_int operator^=(binary_int& lhs, const binary_int& rhs) {
        return lhs = lhs ^ rhs;
    }
    
    friend constexpr binary_int operator<<=(binary_int& lhs, const size_t& shift) {
        return lhs = lhs << shift;
    }

    friend constexpr binary_int operator>>=(binary_int& lhs, const size_t& shift) {
        return lhs = lhs >> shift;
    }

    constexpr binary_int operator-() const {
        return (~(*this)) + binary_int(1);
    }

    friend constexpr binary_int operator+(const binary_int& lhs, const binary_int& rhs) {
        std::bitset<N> res;
        bool carry = false;
        for (size_t i = 0; i < N; ++i) {
            bool a = lhs.bits[i];
            bool b = rhs.bits[i];
            res[i] = a ^ b ^ carry;
            carry = (a && b) || (carry && (a ^ b));
        }
        return binary_int(res);
    }

    friend constexpr binary_int operator-(const binary_int& lhs, const binary_int& rhs) {
        return lhs + (-rhs);
    }

    friend constexpr binary_int operator*(const binary_int& lhs, const binary_int& rhs) {
        bool result_negative = lhs.is_negative() ^ rhs.is_negative();

        binary_int abs_a = lhs.abs();
        binary_int abs_b = rhs.abs();

        binary_int res(0);
        for (size_t i = 0; i < N; ++i) {
            if (abs_b.bits[i]) {
                res = res + (abs_a << i);
            }
        }

        return result_negative ? -res : res;
    }

    friend constexpr binary_int operator/(const binary_int& lhs, const binary_int& rhs) {
        return div_mod(lhs, rhs).first;
    }

    friend constexpr binary_int operator%(const binary_int& lhs, const binary_int& rhs) {
        return div_mod(lhs, rhs).second;
    }

    friend constexpr binary_int operator+=(binary_int& lhs, const binary_int& rhs) {
        return lhs = lhs + rhs;
    }

    friend constexpr binary_int operator-=(binary_int& lhs, const binary_int& rhs) {
        return lhs = lhs - rhs;
    }
    
    friend constexpr binary_int operator*=(binary_int& lhs, const binary_int& rhs) {
        return lhs = lhs * rhs;
    }
    
    friend constexpr binary_int operator/=(binary_int& lhs, const binary_int& rhs) {
        return lhs = lhs / rhs;
    }

    friend constexpr binary_int operator%=(binary_int& lhs, const binary_int& rhs) {
        return lhs = lhs % rhs;
    }

    friend constexpr bool operator==(const binary_int& lhs, const binary_int& rhs) { 
        return lhs.bits == rhs.bits;
    }

    friend constexpr bool operator!=(const binary_int& lhs, const binary_int& rhs) { 
        return lhs.bits != rhs.bits;
    }

    friend constexpr bool operator<(const binary_int& lhs, const binary_int& rhs) {
        bool sign_a = lhs.is_negative();
        bool sign_b = rhs.is_negative();
        if (sign_a != sign_b) return sign_a;

        for (int i = N - 1; i >= 0; --i) {
            if (lhs.bits[i] != rhs.bits[i]) {
                return rhs.bits[i];
            }
        }
        return false;
    }

    friend constexpr bool operator>(const binary_int& lhs, const binary_int& rhs) { 
        return rhs < lhs;
    }

    friend constexpr bool operator<=(const binary_int& lhs, const binary_int& rhs) { 
        return !(lhs > rhs);
    }
    
    friend constexpr bool operator>=(const binary_int& lhs, const binary_int& rhs) { 
        return !(lhs < rhs);
    }

    constexpr std::string to_binary_string() const {
        std::string s = bits.to_string();
        size_t first_one = s.find('1');
        if (first_one == std::string::npos) return "0";
        return s.substr(first_one);
    }

    constexpr std::string to_string() const {
        if (*this == binary_int(0)) return "0";

        binary_int temp = *this;
        bool neg = false;
        if (is_negative()) {
            neg = true;
            temp = temp.abs();
        }

        std::bitset<N> b = temp.bits;

        int msb = N - 1;
        while (msb >= 0 && !b[msb]) {
            msb--;
        }

        if (msb < 0) return "0";

        std::vector<int> decimals;
        decimals.push_back(0);

        for (int i = msb; i >= 0; --i) {
            int bit = b[i];
            
            int carry = bit;
            for (size_t d = 0; d < decimals.size(); ++d) {
                int val = decimals[d] * 2 + carry;
                decimals[d] = val % 10;
                carry = val / 10;
            }
            while (carry > 0) {
                decimals.push_back(carry % 10);
                carry /= 10;
            }
        }

        std::string s;
        if (neg) s += '-';
        for (auto it = decimals.rbegin(); it != decimals.rend(); ++it) {
            s += std::to_string(*it);
        }
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const binary_int& v) {
        return os << v.to_string();
    }

    friend std::istream& operator>>(std::istream& is, binary_int& v) {
        std::string s;
        is >> s;
        v = binary_int(s);
        return is;
    }

private:
    std::bitset<N> bits;

    friend constexpr std::pair<binary_int, binary_int> div_mod(const binary_int& lhs, const binary_int& rhs) {
        if (rhs == binary_int(0)) throw std::runtime_error("Division by zero");

        binary_int dividend = lhs.abs();
        binary_int divisor  = rhs.abs();

        binary_int quotient(0);
        binary_int remainder(0);

        for (int i = N - 1; i >= 0; --i) {
            remainder = remainder << 1;
            remainder.bits[0] = dividend.bits[i];

            if (remainder >= divisor) {
                remainder = remainder - divisor;
                quotient.bits[i] = 1;
            }
        }

        bool q_neg = lhs.is_negative() ^ rhs.is_negative();
        bool r_neg = lhs.is_negative();

        if (q_neg) quotient = -quotient;
        if (r_neg) remainder = -remainder;

        return {quotient, remainder};
    }
};

} // namespace ds
} // namespace cp

namespace std {

template <size_t N>
class numeric_limits<cp::ds::binary_int<N>> {
public:
    static constexpr cp::ds::binary_int<N> max() noexcept {
        std::bitset<N> b;
        b.set();
        b.reset(N - 1);
        return cp::ds::binary_int<N>(b);
    }
};

} // namespace std

#endif