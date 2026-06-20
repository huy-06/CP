#include "../../../alg/math/poly/convolution_ntt.hpp"

#ifndef CP_DS_BIG_INT
#define CP_DS_BIG_INT
namespace cp {
namespace ds {

class big_int {
private:
    enum class raw_tag { 
        construct 
    };

    inline static constexpr int base = 1000000000;
    inline static constexpr int base_digit = 9;

    int sign;
    std::vector<int> digit;

    constexpr void trim() {
        while (!digit.empty() && digit.back() == 0) {
            digit.pop_back();
        }
        if (digit.empty()) sign = 1;
    }

    constexpr int cmp_abs(const big_int& rhs) const {
        if (digit.size() != rhs.digit.size()) {
            return digit.size() < rhs.digit.size() ? -1 : 1;
        }
        for (int i = int(digit.size()) - 1; i >= 0; --i) {
            if (digit[i] != rhs.digit[i]) {
                return digit[i] < rhs.digit[i] ? -1 : 1;
            }
        }
        return 0;
    }

    constexpr void read(std::string_view s) {
        sign = 1;
        digit.clear();
        int pos = 0;

        while (pos < int(s.size()) && (s[pos] == '-' || s[pos] == '+')) {
            if (s[pos] == '-') sign = -sign;
            ++pos;
        }

        for (int i = int(s.size()) - 1; i >= pos; i -= base_digit) {
            int x = 0;
            for (int j = std::max(pos, i - base_digit + 1); j <= i; ++j) {
                x = x * 10 + (s[j] - '0');
            }
            digit.push_back(x);
        }
        trim();
    }

    
public:
    constexpr void shift_left(int shift) {
        if (shift <= 0 || is_zero()) return;
        digit.insert(digit.begin(), shift, 0);
    }

    constexpr void shift_right(int shift) {
        if (shift <= 0 || is_zero()) return;
        if (shift >= int(digit.size())) {
            digit.clear();
            sign = 1;
            return;
        }
        digit.erase(digit.begin(), digit.begin() + shift);
        trim();
    }

private:
    constexpr big_int invert(int k) const {
        if (k <= 64) {
            big_int P;
            P.digit.assign(2 * k + 1, 0);
            P.digit.back() = 1;
            return slow_div_mod(P, *this).first;
        }
        
        int m = (k + 1) / 2;
        big_int B_top = *this;
        B_top.shift_right(k - m);
        
        big_int X = B_top.invert(m);
        
        big_int term1 = X;
        long long carry = 0;
        for (int& d : term1.digit) {
            long long cur = d * 2LL + carry;
            d = int(cur % base);
            carry = cur / base;
        }
        if (carry) term1.digit.push_back(int(carry));
        term1.shift_left(k + m);
        
        big_int term2 = (X * (*this)) * X;
        
        big_int X_new = term1 - term2;
        X_new.shift_right(2 * m);
        
        big_int P;
        P.digit.assign(2 * k + 1, 0);
        P.digit.back() = 1;
        
        big_int prod = X_new * (*this);
        if (prod > P) {
            big_int diff = prod - P;
            while (!diff.is_zero() && diff >= *this) {
                diff -= *this;
                --X_new;
            }
            if (!diff.is_zero()) {
                --X_new;
            }
        } else {
            big_int diff = P - prod;
            while (diff >= *this) {
                diff -= *this;
                ++X_new;
            }
        }
        return X_new;
    }

    friend constexpr std::pair<big_int, big_int> slow_div_mod(const big_int& a, const big_int& b) {
        big_int abs_a = a.abs();
        big_int abs_b = b.abs();
        
        if (abs_a < abs_b) return { big_int(0), a };
        
        long long norm = base / (abs_b.digit.back() + 1);
        abs_a *= norm;
        abs_b *= norm;
        
        int n = abs_a.digit.size();
        int m = abs_b.digit.size();
        big_int q(0);
        q.digit.assign(n - m + 1, 0);
        
        abs_a.digit.push_back(0); 
        
        for (int i = n - m; i >= 0; --i) {
            long long temp = static_cast<long long>(abs_a.digit[i + m]) * base + abs_a.digit[i + m - 1];
            long long q_guess = temp / abs_b.digit.back();
            long long r_guess = temp % abs_b.digit.back();
            
            while (q_guess == base || (m > 1 && q_guess * abs_b.digit[m - 2] > r_guess * base + abs_a.digit[i + m - 2])) {
                q_guess--;
                r_guess += abs_b.digit.back();
                if (r_guess >= base) break;
            }
            
            long long carry = 0;
            for (int j = 0; j < m; ++j) {
                long long prod = q_guess * abs_b.digit[j] + carry;
                long long diff = abs_a.digit[i + j] - (prod % base);
                carry = prod / base;
                if (diff < 0) {
                    diff += base;
                    carry++;
                }
                abs_a.digit[i + j] = int(diff);
            }
            abs_a.digit[i + m] -= int(carry);
            
            if (abs_a.digit[i + m] < 0) {
                q_guess--;
                carry = 0;
                for (int j = 0; j < m; ++j) {
                    long long sum = abs_a.digit[i + j] + abs_b.digit[j] + carry;
                    abs_a.digit[i + j] = int(sum % base);
                    carry = sum / base;
                }
                abs_a.digit[i + m] += int(carry);
            }
            q.digit[i] = int(q_guess);
        }
        
        q.sign = a.sign * b.sign;
        q.trim();
        
        abs_a.trim();
        big_int r = abs_a / norm;
        r.sign = a.sign;
        if (r.is_zero()) r.sign = 1;
        
        return { q, r };
    }

    friend constexpr std::pair<big_int, big_int> div_mod(const big_int& a, const big_int& b) {
        if (b.is_zero()) throw std::runtime_error("Division by zero");
        if (a.is_zero()) return { big_int(0), big_int(0) };
        
        big_int abs_a = a.abs();
        big_int abs_b = b.abs();
        
        if (abs_a < abs_b) return { big_int(0), a };
        
        if (abs_b.digit.size() <= 64) {
            return slow_div_mod(a, b);
        }
        
        long long norm = base / (abs_b.digit.back() + 1);
        abs_a *= norm;
        abs_b *= norm;
        
        int K = abs_b.digit.size();
        big_int I = abs_b.invert(K);
        big_int Q(0);
        
        while (abs_a >= abs_b) {
            int n = abs_a.digit.size();
            if (n <= 2 * K) {
                big_int Q_add = (abs_a * I);
                Q_add.shift_right(2 * K);
                big_int prod = Q_add * abs_b;
                while (abs_a >= prod + abs_b) {
                    Q_add++;
                    prod += abs_b;
                }
                abs_a -= prod;
                Q += Q_add;
                break;
            } else {
                int shift = n - 2 * K;
                big_int A_top;
                A_top.digit = std::vector<int>(abs_a.digit.begin() + shift, abs_a.digit.end());
                
                big_int Q_add = (A_top * I);
                Q_add.shift_right(2 * K);
                if (Q_add.is_zero()) Q_add = 1;
                
                big_int prod = Q_add * abs_b;
                
                big_int B_shifted = abs_b;
                B_shifted.shift_left(shift);
                
                Q_add.shift_left(shift);
                prod.shift_left(shift);
                
                while (abs_a < prod) {
                    Q_add--;
                    prod -= B_shifted;
                }
                
                abs_a -= prod;
                Q += Q_add;
            }
        }
        
        Q.sign = a.sign * b.sign;
        Q.trim();
        
        big_int R = abs_a / norm;
        R.sign = a.sign;
        if (R.is_zero()) R.sign = 1;
        
        return { Q, R };
    }

public:
    constexpr big_int() noexcept : sign(1) {}

    constexpr big_int(int s, std::vector<int> d, raw_tag) noexcept : sign(s), digit(std::move(d)) {}

    template <std::integral Tp>
    constexpr big_int(Tp x) {
        *this = x;
    }

    constexpr big_int(std::string_view s) {
        read(s);
    }

    constexpr big_int(const std::string& s) {
        read(s);
    }

    constexpr big_int& operator=(const big_int& rhs) = default;

    constexpr big_int(const big_int& rhs) = default;

    constexpr big_int(big_int&& rhs) noexcept = default;

    constexpr big_int& operator=(big_int&& rhs) noexcept = default;

    template <std::integral Tp>
    constexpr big_int& operator=(Tp x) {
        digit.clear();
        sign = 1;
        if (x == 0) return *this;

        std::make_unsigned_t<Tp> ux;
        if (x < 0) {
            sign = -1;
            ux = -static_cast<std::make_unsigned_t<Tp>>(x);
        } else {
            ux = x;
        }

        while (ux > 0) {
            digit.push_back(ux % base);
            ux /= base;
        }
        return *this;
    }

    constexpr big_int operator+() const noexcept {
        return *this;
    }

    constexpr big_int operator-() const noexcept {
        if (is_zero()) return *this;
        return big_int(-sign, digit, raw_tag::construct);
    }

    constexpr big_int& operator+=(const big_int& rhs) {
        if (sign == rhs.sign) {
            int carry = 0;
            for (size_t i = 0; i < std::max(digit.size(), rhs.digit.size()) || carry; ++i) {
                if (i == digit.size()) digit.push_back(0);
                digit[i] += carry + (i < rhs.digit.size() ? rhs.digit[i] : 0);
                carry = digit[i] >= base;
                if (carry) digit[i] -= base;
            }
        } else {
            if (cmp_abs(rhs) >= 0) {
                int carry = 0;
                for (size_t i = 0; i < rhs.digit.size() || carry; ++i) {
                    digit[i] -= carry + (i < rhs.digit.size() ? rhs.digit[i] : 0);
                    carry = digit[i] < 0;
                    if (carry) digit[i] += base;
                }
                trim();
            } else {
                big_int tmp = rhs;
                int carry = 0;
                for (size_t i = 0; i < digit.size() || carry; ++i) {
                    tmp.digit[i] -= carry + (i < digit.size() ? digit[i] : 0);
                    carry = tmp.digit[i] < 0;
                    if (carry) tmp.digit[i] += base;
                }
                tmp.trim();
                *this = std::move(tmp);
            }
        }
        return *this;
    }

    constexpr big_int& operator-=(const big_int& rhs) {
        sign = -sign;
        *this += rhs;
        sign = -sign;
        if (is_zero()) sign = 1;
        return *this;
    }

    constexpr big_int& operator*=(const big_int& rhs) {
        if (is_zero() || rhs.is_zero()) return *this = 0;

        std::vector<long long> a, b;
        a.reserve(digit.size() * 3);
        for (int x : digit) {
            a.push_back(x % 1000);
            a.push_back((x / 1000) % 1000);
            a.push_back(x / 1000000);
        }
        while (!a.empty() && a.back() == 0) a.pop_back();

        b.reserve(rhs.digit.size() * 3);
        for (int x : rhs.digit) {
            b.push_back(x % 1000);
            b.push_back((x / 1000) % 1000);
            b.push_back(x / 1000000);
        }
        while (!b.empty() && b.back() == 0) b.pop_back();

        std::vector<long long> c = alg::convolution(a, b);

        std::vector<long long> res_1000;
        long long carry = 0;
        for (long long x : c) {
            long long cur = x + carry;
            res_1000.push_back(cur % 1000);
            carry = cur / 1000;
        }
        while (carry > 0) {
            res_1000.push_back(carry % 1000);
            carry /= 1000;
        }

        digit.clear();
        digit.reserve((res_1000.size() + 2) / 3);
        for (size_t i = 0; i < res_1000.size(); i += 3) {
            long long d0 = res_1000[i];
            long long d1 = (i + 1 < res_1000.size()) ? res_1000[i + 1] : 0;
            long long d2 = (i + 2 < res_1000.size()) ? res_1000[i + 2] : 0;
            digit.push_back(d0 + d1 * 1000 + d2 * 1000000);
        }
        
        sign *= rhs.sign;
        trim();
        return *this;
    }

    template <std::integral Tp>
    constexpr big_int& operator*=(Tp rhs) {
        if (rhs == 0 || is_zero()) return *this = 0;
        
        if (rhs < 0) {
            sign = -sign;
            rhs = -rhs;
        }

        long long carry = 0;
        for (size_t i = 0; i < digit.size() || carry; ++i) {
            if (i == digit.size()) digit.push_back(0);
            long long cur = static_cast<long long>(digit[i]) * rhs + carry;
            digit[i] = int(cur % base);
            carry = cur / base;
        }
        trim();
        return *this;
    }

    constexpr big_int& operator/=(const big_int& rhs) {
        return *this = div_mod(*this, rhs).first;
    }

    template <std::integral Tp>
    constexpr big_int& operator/=(Tp rhs) {
        if (rhs == 0) throw std::runtime_error("Division by zero");
        if (is_zero()) return *this;

        if (rhs < 0) {
            sign = -sign;
            rhs = -rhs;
        }

        long long rem = 0;
        for (int i = int(digit.size()) - 1; i >= 0; --i) {
            long long cur = rem * base + digit[i];
            digit[i] = int(cur / rhs);
            rem = cur % rhs;
        }
        trim();
        return *this;
    }

    constexpr big_int& operator%=(const big_int& rhs) {
        return *this = div_mod(*this, rhs).second;
    }

    constexpr big_int& operator++() {
        return *this += 1;
    }

    constexpr big_int& operator--() {
        return *this -= 1;
    }

    constexpr big_int operator++(int) {
        big_int res = *this;
        ++*this;
        return res;
    }

    constexpr big_int operator--(int) {
        big_int res = *this;
        --*this;
        return res;
    }

    friend constexpr big_int operator+(big_int lhs, const big_int& rhs) {
        return lhs += rhs;
    }

    friend constexpr big_int operator-(big_int lhs, const big_int& rhs) {
        return lhs -= rhs;
    }

    friend constexpr big_int operator*(big_int lhs, const big_int& rhs) {
        return lhs *= rhs;
    }

    friend constexpr big_int operator/(big_int lhs, const big_int& rhs) {
        return lhs /= rhs;
    }

    friend constexpr big_int operator%(big_int lhs, const big_int& rhs) {
        return lhs %= rhs;
    }

    template <std::integral Tp>
    friend constexpr big_int operator*(big_int lhs, Tp rhs) {
        return lhs *= rhs;
    }

    template <std::integral Tp> friend constexpr big_int operator/(big_int lhs, Tp rhs) {
        return lhs /= rhs;
    }
    
    template <std::integral Tp>
    friend constexpr Tp operator%(const big_int& lhs, Tp rhs) {
        if (rhs == 0) throw std::runtime_error("Modulo by zero");
        if (rhs < 0) rhs = -rhs;
        
        long long m = 0;
        for (int i = int(lhs.digit.size()) - 1; i >= 0; --i) {
            m = (m * base + lhs.digit[i]) % rhs;
        }
        return static_cast<Tp>(m * lhs.sign);
    }

    friend constexpr bool operator==(const big_int& lhs, const big_int& rhs) {
        if (lhs.is_zero() && rhs.is_zero()) return true;
        return lhs.sign == rhs.sign && lhs.digit == rhs.digit;
    }

    friend constexpr bool operator!=(const big_int& lhs, const big_int& rhs) {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const big_int& lhs, const big_int& rhs) {
        if (lhs.sign != rhs.sign) return lhs.sign < rhs.sign;
        if (lhs.is_zero() && rhs.is_zero()) return false;
        int cmp = lhs.cmp_abs(rhs);
        return lhs.sign > 0 ? cmp < 0 : cmp > 0;
    }

    friend constexpr bool operator>(const big_int& lhs, const big_int& rhs) {
        return rhs < lhs;
    }

    friend constexpr bool operator<=(const big_int& lhs, const big_int& rhs) {
        return !(rhs < lhs);
    }

    friend constexpr bool operator>=(const big_int& lhs, const big_int& rhs) {
        return !(lhs < rhs);
    }

    friend std::istream& operator>>(std::istream& is, big_int& x) {
        std::string s;
        is >> s;
        x.read(s);
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const big_int& x) {
        if (x.is_zero()) return os << 0;
        if (x.sign == -1) os << '-';
        os << x.digit.back();
        for (int i = int(x.digit.size()) - 2; i >= 0; --i) {
            os << std::setw(big_int::base_digit) << std::setfill('0') << x.digit[i];
        }
        return os;
    }

    constexpr bool is_zero() const noexcept {
        return digit.empty();
    }

    constexpr big_int abs() const noexcept {
        return big_int(1, digit, raw_tag::construct);
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    constexpr long long long_value() const noexcept {
        long long res = 0;
        for (int i = int(digit.size()) - 1; i >= 0; --i) {
            res = res * base + digit[i];
        }
        return res * sign;
    }

    explicit constexpr operator long long() const noexcept {
        return long_value();
    }

    template <typename Int>
    constexpr big_int pow(Int k) const {
        big_int res = 1, a = *this;
        for (; k; k /= 2, a *= a) {
            if (k % 2 == 1) res *= a;
        }
        return res;
    }

    constexpr big_int sqrt() const {
        if (is_zero()) return big_int(0);
        if (sign == -1) throw std::runtime_error("Square root of negative number");

        unsigned long long bitlen = 0;
        big_int n = *this, t = n, zero(0), two(2);
        while (t > zero) {
            t /= two;
            bitlen++;
        }

        unsigned long long e = (bitlen + 1) / 2;
        big_int x(1), bs = two;
        while (e > 0) {
            if (e % 2 == 1) x *= bs;
            bs *= bs;
            e /= 2;
        }

        big_int y = (x + n / x) / two;
        while (y < x) {
            x = y;
            y = (x + n / x) / two;
        }
        return x;
    }

    friend constexpr big_int gcd(big_int a, big_int b) {
        a = a.abs();
        b = b.abs();
        while (!b.is_zero()) {
            big_int c = a % b;
            a = b;
            b = c;
        }
        return a;
    }

    friend constexpr big_int lcm(big_int a, big_int b) {
        if (a.is_zero() || b.is_zero()) return big_int(0);
        return (a * b).abs() / gcd(a, b);
    }
};

} // namespace ds
} // namespace cp

namespace std {

template <>
class numeric_limits<cp::ds::big_int> {
public:
    static cp::ds::big_int max() {
        return cp::ds::big_int(std::string(1000000, '9'));
    }
};

} // namespace std

#endif