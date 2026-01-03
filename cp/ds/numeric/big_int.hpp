#include <istream>
#include <iomanip>
#include <vector>
#include <string>
#include <type_traits>
#include <sstream>
#include <cassert>

#ifndef CP_DS_BIG_INT
#define CP_DS_BIG_INT
namespace cp {
namespace ds {

class big_int {
public:
    big_int() : sign(1) {}

    template <std::integral Tp>
    big_int(Tp x) {
        *this = x;
    }

    big_int(const std::string& s) {
        read(s);
    }

    big_int& operator=(const big_int& rhs) {
        if (this != &rhs) {
            sign = rhs.sign;
            digit = rhs.digit;
        }
        return *this;
    }

    template <std::integral Tp>
    big_int& operator=(Tp x) {
        digit.clear();
        sign = 1;
        if (x == 0) {
            return *this;
        }
        if (x < 0) {
            sign = -1;
            x = -x;
        }
        while (x > 0) {
            digit.push_back(x % base);
            x /= base;
        }
        return *this;
    }

    big_int operator-() const {
        big_int res = *this;
        res.sign = -sign;
        return res;
    }

    friend big_int operator+(const big_int& lhs, const big_int& rhs) {
        if (lhs.sign == rhs.sign) {
            big_int res = rhs;
            for (int i = 0, carry = 0; i < int(std::max(lhs.digit.size(), rhs.digit.size())) || carry; ++i) {
                if (i == int(res.digit.size())) {
                    res.digit.push_back(0);
                }
                res.digit[i] += carry + (i < int(lhs.digit.size()) ? lhs.digit[i] : 0);
                carry = res.digit[i] >= base;
                if (carry) {
                    res.digit[i] -= base;
                }
            }
            res.trim();
            return res;
        }
        return lhs - (-rhs);
    }

    friend big_int operator-(const big_int& lhs, const big_int& rhs) {
        if (lhs.sign == rhs.sign) {
            if (lhs.abs() >= rhs.abs()) {
                big_int res = lhs;
                for (int i = 0, carry = 0; i < int(rhs.digit.size()) || carry; ++i) {
                    res.digit[i] -= carry + (i < int(rhs.digit.size()) ? rhs.digit[i] : 0);
                    carry = res.digit[i] < 0;
                    if (carry) {
                        res.digit[i] += base;
                    }
                }
                res.trim();
                return res;
            }
            return -(rhs - lhs);
        }
        return lhs + (-rhs);
    }

    template<std::integral Tp>
    friend big_int operator*(big_int lhs, const Tp& rhs) {
        return lhs *= rhs;
    }

    friend big_int operator*(const big_int& lhs, const big_int& rhs) {
        std::vector<int> a6 = convert_base(lhs.digit, lhs.base_digit, 6);
        std::vector<int> b6 = convert_base(rhs.digit, rhs.base_digit, 6);
        std::vector<long long> a(a6.begin(), a6.end());
        std::vector<long long> b(b6.begin(), b6.end());
        while (a.size() < b.size()) {
            a.push_back(0);
        }
        while (b.size() < a.size()) {
            b.push_back(0);
        }
        while (a.size() & (a.size() - 1)) {
            a.push_back(0);
            b.push_back(0);
        }
        std::vector<long long> c = karatsuba(a, b);
        big_int res;
        res.sign = lhs.sign * rhs.sign;
        for (int i = 0, carry = 0; i < int(c.size()); ++i) {
            long long cur = c[i] + carry;
            res.digit.push_back(int(cur % 1000000));
            carry = int(cur / 1000000);
        }
        res.digit = convert_base(res.digit, 6, res.base_digit);
        res.trim();
        return res;
    }

    template<std::integral Tp>
    friend big_int operator/(big_int lhs, const Tp& rhs) {
        return lhs /= rhs;
    }

    friend big_int operator/(const big_int& lhs, const big_int& rhs) {
        return div_mod(lhs, rhs).first;
    }

    friend big_int operator%(const big_int& lhs, const big_int& rhs) {
        return div_mod(lhs, rhs).second;
    }

    template<std::integral Tp>
    friend Tp operator%(const big_int& lhs, Tp rhs) {
        assert(rhs != 0);
        if (rhs < 0) {
            rhs = -rhs;
        }
        Tp m = 0;
        for (int i = int(lhs.digit.size()) - 1; i >= 0; --i) {
            m = static_cast<Tp>((static_cast<long long>(m) * base + lhs.digit[i]) % rhs);
        }
        return m * lhs.sign;
    }

    big_int& operator+=(const big_int& rhs) {
        return *this = *this + rhs;
    }

    big_int& operator-=(const big_int& rhs) {
        return *this = *this - rhs;
    }

    template<std::integral Tp>
    big_int& operator*=(Tp rhs) {
        if (rhs == 0 || is_zero()) {
            digit.clear();
            sign = 1;
            return *this;
        }
        if (rhs < 0) {
            sign = -sign;
            rhs = -rhs;
        }
        for (int i = 0, carry = 0; i < int(digit.size()) || carry; ++i) {
            if (i == int(digit.size())) {
                digit.push_back(0);
            }
            long long cur = static_cast<long long>(digit[i]) * rhs + carry;
            digit[i] = int(cur % base);
            carry = int(cur / base);
        }
        trim();
        return *this;
    }

    big_int& operator*=(const big_int& rhs) {
        return *this = *this * rhs;
    }

    template<std::integral Tp>
    big_int& operator/=(Tp rhs) {
        assert(rhs != 0);
        if (rhs < 0) {
            sign = -sign;
            rhs = -rhs;
        }
        Tp rem = 0;
        for (int i = int(digit.size()) - 1; i >= 0; --i) {
            long long cur = static_cast<long long>(rem) * base + digit[i];
            digit[i] = static_cast<int>(cur / rhs);
            rem = static_cast<Tp>(cur % rhs);
        }
        trim();
        return *this;
    }

    big_int& operator/=(const big_int& rhs) {
        return *this = *this / rhs;
    }

    big_int& operator%=(const big_int& rhs) {
        return *this = *this %  rhs;
    }

    friend bool operator<(const big_int& lhs, const big_int& rhs) {
        if (lhs.sign != rhs.sign) {
            return lhs.sign < rhs.sign;
        }
        if (lhs.digit.size() != rhs.digit.size()) {
            return int(lhs.digit.size()) * lhs.sign < int(rhs.digit.size()) * rhs.sign;
        }
        for (int i = int(lhs.digit.size()) - 1; i >= 0; --i) {
            if (lhs.digit[i] != rhs.digit[i]) {
                return lhs.digit[i] * lhs.sign < rhs.digit[i] * rhs.sign;
            }
        }
        return false;
    }

    friend bool operator>(const big_int& lhs, const big_int& rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(const big_int& lhs, const big_int& rhs) {
        return !(lhs > rhs);
    }

    friend bool operator>=(const big_int& lhs, const big_int& rhs) {
        return !(lhs < rhs);
    }

    friend bool operator==(const big_int& lhs, const big_int& rhs) {
        return !(lhs < rhs) && !(rhs < lhs);
    }

    friend bool operator!=(const big_int& lhs, const big_int& rhs) {
        return !(lhs == rhs);
    }

    friend std::istream& operator>>(std::istream& is, big_int& x) {
        std::string s;
        is >> s;
        x.read(s);
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const big_int& x) {
        if (x.sign == -1) {
            os << '-';
        }
        os << (x.digit.empty() ? 0 : x.digit.back());
        for (int i = int(x.digit.size()) - 2; i >= 0; --i) {
            os << std::setw(big_int::base_digit) << std::setfill('0') << x.digit[i];
        }
        return os;
    }

    bool is_zero() const  {
        return digit.empty();
    }

    big_int abs() const {
        big_int res = *this;
        res.sign = 1;
        return res;
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    long long long_value() const {
        long long res = 0;
        for (int i = int(digit.size()) - 1; i >= 0; --i) {
            res = res * base + digit[i];
        }
        return res * sign;
    }

    template <typename Int>
    big_int pow(Int k) const {
        big_int res = 1, a = *this;
        for (; k; k /= 2, a *= a) {
            if (k % 2 == 1)
                res *= a;
        }
        return res;
    }

    big_int sqrt() const {
        if (is_zero()) return big_int(0);
        unsigned long long bitlen = 0;
        big_int n = *this, t = n, zero(0), two(2);
        while (t > zero) {
            t = t / two;
            bitlen++;
        }
        unsigned long long e = (bitlen + 1) / 2;
        big_int x(1), base = two;
        while (e > 0) {
            if (e % 2 == 1) {
                x = x * base;
            }
            base = base * base;
            e = e / 2;
        }
        big_int y = (x + n / x) / two;
        while (y < x) {
            x = y;
            y = (x + n / x) / two;
        }
        return x;
    }

    friend big_int gcd(big_int a, big_int b) {
        while (b > 0) {
            big_int c = a % b;
            a = b;
            b = c;
        }
        return a;
    }

    friend big_int lcm(big_int a, big_int b) {
        return a * b / gcd(a, b);
    }

private:
    inline static constexpr int base = 1000000000;
    inline static constexpr int base_digit = 9;

    int sign;
    std::vector<int> digit;

    void read(const std::string& s) {
        sign = 1;
        digit.clear();
        int pos = 0;
        while (pos < int(s.size()) && (s[pos] == '-' || s[pos] == '+')) {
            if (s[pos] == '-') {
                sign = -sign;
            }
            ++pos;
        }
        for (int i = s.size() - 1; i >= pos; i -= base_digit) {
            int x = 0;
            for (int j = std::max(pos, i - base_digit + 1); j <= i; ++j) {
                x = x * 10 + s[j] - '0';
            }
            digit.push_back(x);
        }
        trim();
    }

    void trim() {
        while (!digit.empty() && !digit.back()) {
            digit.pop_back();
        }
        if (digit.empty()) {
            sign = 1;
        }
    }

    friend std::pair<big_int, big_int> div_mod(const big_int& lhs, const big_int& rhs) {
        assert(!rhs.is_zero());
        if (lhs.abs() < rhs.abs()) {
            return {big_int(0), lhs};
        }
        int norm = base / (rhs.digit.back() + 1);
        big_int a = lhs.abs() * norm;
        big_int b = rhs.abs() * norm;
        big_int q, r;
        q.digit.assign(a.digit.size(), 0);
        for (int i = int(a.digit.size()) - 1; i >= 0; --i) {
            r = r * base + a.digit[i];
            int s1 = r.digit.size() <= b.digit.size() ? 0 : r.digit[b.digit.size()];
            int s2 = r.digit.size() <= b.digit.size() - 1 ? 0 : r.digit[b.digit.size() - 1];
            int d = (static_cast<long long>(base) * s1 + s2) / b.digit.back();
            r -= b * d;
            while (r < 0) {
                r += b;
                --d;
            }
            q.digit[i] = d;
        }
        q.sign = lhs.sign * rhs.sign;
        r.sign = lhs.sign;
        q.trim();
        r.trim();
        return {q, r / norm};
    }

    static std::vector<int> convert_base(const std::vector<int>& a, int old_digits, int new_digits) {
        std::vector<long long> p(std::max(old_digits, new_digits) + 1);
        p[0] = 1;
        for (int i = 1; i < int(p.size()); ++i) {
            p[i] = p[i - 1] * 10;
        }
        std::vector<int> res;
        long long cur = 0;
        int cur_digits = 0;
        for (int i = 0; i < int(a.size()); ++i) {
            cur += a[i] * p[cur_digits];
            cur_digits += old_digits;
            while (cur_digits >= new_digits) {
                res.push_back(int(cur % p[new_digits]));
                cur /= p[new_digits];
                cur_digits -= new_digits;
            }
        }
        res.push_back(int(cur));
        while (!res.empty() && !res.back()) {
            res.pop_back();
        }
        return res;
    }

    static std::vector<long long> karatsuba(const std::vector<long long>& a, const std::vector<long long>& b) {
        int n = int(a.size());
        std::vector<long long> res(n + n);
        if (n <= 32) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    res[i + j] += a[i] * b[j];
                }
            }
            return res;
        }
        int k = n >> 1;
        std::vector<long long> a1(a.begin(), a.begin() + k);
        std::vector<long long> a2(a.begin() + k, a.end());
        std::vector<long long> b1(b.begin(), b.begin() + k);
        std::vector<long long> b2(b.begin() + k, b.end());
        std::vector<long long> a1b1 = karatsuba(a1, b1);
        std::vector<long long> a2b2 = karatsuba(a2, b2);
        for (int i = 0; i < k; ++i) {
            a2[i] += a1[i];
            b2[i] += b1[i];
        }
        std::vector<long long> r = karatsuba(a2, b2);
        for (int i = 0; i < int(a1b1.size()); ++i) {
            r[i] -= a1b1[i];
        }
        for (int i = 0; i < int(a2b2.size()); ++i) {
            r[i] -= a2b2[i];
        }
        for (int i = 0; i < int(r.size()); ++i) {
            res[i + k] += r[i];
        }
        for (int i = 0; i < int(a1b1.size()); ++i) {
            res[i] += a1b1[i];
        }
        for (int i = 0; i < int(a2b2.size()); ++i) {
            res[i + n] += a2b2[i];
        }
        return res;
    }
};

} // namespace ds
} // namespace cp

namespace std {

template <>
class numeric_limits<cp::ds::big_int> {
public:
    static cp::ds::big_int max() noexcept {
        return cp::ds::big_int(std::string(1000000, '9'));
    }
};

} // namespace std

#endif