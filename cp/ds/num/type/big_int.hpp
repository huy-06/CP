#include "../alge/polynomial.hpp" 

#ifndef CP_DS_BIG_INT
#define CP_DS_BIG_INT
namespace cp {
namespace ds {

class big_int {
public:
    using value_type = int;

    constexpr big_int() noexcept : m_sign(1), m_digit() {}

    template <std::integral Tp>
    constexpr big_int(Tp x) {
        *this = x;
    }

    big_int(std::string_view s) {
        read(s);
    }

    big_int(const std::string& s) : big_int(std::string_view(s)) {}

    big_int(const big_int& rhs) = default;

    big_int(big_int&& rhs) noexcept = default;

    big_int& operator=(const big_int& rhs) = default;

    big_int& operator=(big_int&& rhs) noexcept = default;

    template <std::integral Tp>
    big_int& operator=(Tp x) {
        m_digit.clear();
        m_sign = 1;
        if (x == 0) return *this;

        std::make_unsigned_t<Tp> ux;
        if (x < 0) {
            m_sign = -1;
            ux = -static_cast<std::make_unsigned_t<Tp>>(x);
        } else {
            ux = static_cast<std::make_unsigned_t<Tp>>(x);
        }

        while (ux > 0) {
            m_digit.push_back(static_cast<int>(ux % base));
            ux /= base;
        }
        return *this;
    }

    static big_int wrap(int sign, std::vector<int> digits) noexcept {
        big_int res;
        res.m_sign = sign;
        res.m_digit = std::move(digits);
        return res;
    }

    static big_int raw(int sign, std::vector<int> digits) {
        big_int res = wrap(sign, std::move(digits));
        res.trim();
        return res;
    }

    constexpr int sign() const noexcept { 
        return is_zero() ? 0 : m_sign; 
    }

    constexpr bool is_zero() const noexcept { 
        return m_digit.empty(); 
    }

    explicit constexpr operator bool() const noexcept { 
        return !is_zero(); 
    }

    const std::vector<int>& data() const noexcept { 
        return m_digit; 
    }

    big_int abs() const noexcept {
        return wrap(1, m_digit);
    }

    long long long_value() const noexcept {
        long long res = 0;
        for (int i = static_cast<int>(m_digit.size()) - 1; i >= 0; --i) {
            res = res * base + m_digit[i];
        }
        return res * m_sign;
    }

    explicit operator long long() const noexcept {
        return long_value();
    }

    std::string to_string() const {
        if (is_zero()) {
            return "0";
        }

        std::string s;
        if (m_sign == -1) {
            s += '-';
        }
        s += std::to_string(m_digit.back());

        for (int i = static_cast<int>(m_digit.size()) - 2; i >= 0; --i) {
            std::string sub = std::to_string(m_digit[i]);
            s.append(base_digits - sub.size(), '0');
            s += sub;
        }

        return s;
    }

    big_int operator+() const noexcept {
        return *this;
    }

    big_int operator-() const noexcept {
        if (is_zero()) {
            return *this;
        }
        return wrap(-m_sign, m_digit);
    }

    big_int& operator+=(const big_int& rhs) {
        if (m_sign == rhs.m_sign) {
            int carry = 0;
            for (size_t i = 0; i < std::max(m_digit.size(), rhs.m_digit.size()) || carry; ++i) {
                if (i == m_digit.size()) {
                    m_digit.push_back(0);
                }
                m_digit[i] += carry + (i < rhs.m_digit.size() ? rhs.m_digit[i] : 0);

                carry = m_digit[i] >= base;
                if (carry) {
                    m_digit[i] -= base;
                }
            }
        } else {
            if (cmp_abs(rhs) >= 0) {
                int carry = 0;
                for (size_t i = 0; i < rhs.m_digit.size() || carry; ++i) {
                    m_digit[i] -= carry + (i < rhs.m_digit.size() ? rhs.m_digit[i] : 0);

                    carry = m_digit[i] < 0;
                    if (carry) {
                        m_digit[i] += base;
                    }
                }
                trim();
            } else {
                big_int tmp = rhs;
                int carry = 0;

                for (size_t i = 0; i < m_digit.size() || carry; ++i) {
                    tmp.m_digit[i] -= carry + (i < m_digit.size() ? m_digit[i] : 0);

                    carry = tmp.m_digit[i] < 0;
                    if (carry) {
                        tmp.m_digit[i] += base;
                    }
                }

                tmp.trim();
                *this = std::move(tmp);
            }
        }
        return *this;
    }

    big_int& operator-=(const big_int& rhs) {
        m_sign = -m_sign;
        *this += rhs;

        m_sign = -m_sign;
        if (is_zero()) {
            m_sign = 1;
        }

        return *this;
    }

    big_int& operator*=(const big_int& rhs) {
        if (is_zero() || rhs.is_zero()) {
            return *this = 0;
        }

        std::vector<long long> a, b;
        a.reserve(m_digit.size() * 3);

        for (int x : m_digit) {
            a.push_back(x % 1000);
            a.push_back((x / 1000) % 1000);
            a.push_back(x / 1000000);
        }
        while (!a.empty() && a.back() == 0) {
            a.pop_back();
        }

        b.reserve(rhs.m_digit.size() * 3);
        for (int x : rhs.m_digit) {
            b.push_back(x % 1000);
            b.push_back((x / 1000) % 1000);
            b.push_back(x / 1000000);
        }
        while (!b.empty() && b.back() == 0) {
            b.pop_back();
        }

        polynomial<long long> poly_a(std::move(a));
        polynomial<long long> poly_b(std::move(b));
        polynomial<long long> poly_c = poly_a * poly_b;

        std::vector<long long> res_1000;
        res_1000.reserve(poly_c.size() + 5);
        long long carry = 0;

        for (int i = 0; i < poly_c.size(); ++i) {
            long long cur = poly_c[i] + carry;
            res_1000.push_back(cur % 1000);
            carry = cur / 1000;
        }
        while (carry > 0) {
            res_1000.push_back(carry % 1000);
            carry /= 1000;
        }

        m_digit.clear();
        m_digit.reserve((res_1000.size() + 2) / 3);
        
        for (size_t i = 0; i < res_1000.size(); i += 3) {
            long long d0 = res_1000[i];
            long long d1 = (i + 1 < res_1000.size()) ? res_1000[i + 1] : 0;
            long long d2 = (i + 2 < res_1000.size()) ? res_1000[i + 2] : 0;
            m_digit.push_back(static_cast<int>(d0 + d1 * 1000 + d2 * 1000000));
        }
        
        m_sign *= rhs.m_sign;
        trim();

        return *this;
    }

    template <std::integral Tp>
    big_int& operator*=(Tp rhs) {
        if (rhs == 0 || is_zero()) {
            return *this = 0;
        }
        
        if (rhs < 0) {
            m_sign = -m_sign;
            rhs = -rhs;
        }

        long long carry = 0;
        for (size_t i = 0; i < m_digit.size() || carry; ++i) {
            if (i == m_digit.size()) m_digit.push_back(0);
            long long cur = static_cast<long long>(m_digit[i]) * rhs + carry;
            m_digit[i] = static_cast<int>(cur % base);
            carry = cur / base;
        }
        trim();

        return *this;
    }

    big_int& operator/=(const big_int& rhs) {
        return *this = div_mod(*this, rhs).first;
    }

    template <std::integral Tp>
    big_int& operator/=(Tp rhs) {
        if (rhs == 0) {
            throw std::runtime_error("Division by zero");
        }

        if (is_zero()) {
            return *this;
        }

        if (rhs < 0) {
            m_sign = -m_sign;
            rhs = -rhs;
        }

        long long rem = 0;
        for (int i = static_cast<int>(m_digit.size()) - 1; i >= 0; --i) {
            long long cur = rem * base + m_digit[i];
            m_digit[i] = static_cast<int>(cur / rhs);
            rem = cur % rhs;
        }
        trim();

        return *this;
    }

    big_int& operator%=(const big_int& rhs) {
        return *this = div_mod(*this, rhs).second;
    }

    big_int& operator++() {
        return *this += 1;
    }

    big_int& operator--() {
        return *this -= 1;
    }

    big_int operator++(int) {
        big_int res = *this;
        ++*this;
        return res;
    }

    big_int operator--(int) {
        big_int res = *this;
        --*this;
        return res;
    }

    template <typename Int>
    big_int pow(Int k) const {
        big_int res = 1, a = *this;
        for (; k; k /= 2, a *= a) {
            if (k % 2 == 1) res *= a;
        }
        return res;
    }

    big_int sqrt() const {
        if (is_zero()) {
            return big_int(0);
        }

        if (m_sign == -1) {
            throw std::runtime_error("Square root of negative number");
        }

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

    static std::pair<big_int, big_int> div_mod(const big_int& a, const big_int& b) {
        if (b.is_zero()) {
            throw std::runtime_error("Division by zero");
        }

        if (a.is_zero()) {
            return { big_int(0), big_int(0) };
        }
        
        big_int abs_a = a.abs();
        big_int abs_b = b.abs();
        
        if (abs_a < abs_b) {
            return { big_int(0), a };
        }
        
        if (abs_b.m_digit.size() <= 64) {
            return slow_div_mod(a, b);
        }
        
        long long norm = base / (abs_b.m_digit.back() + 1);
        abs_a *= norm;
        abs_b *= norm;
        
        int k = abs_b.m_digit.size();
        big_int inv = abs_b.invert(k);
        big_int q(0);
        
        while (abs_a >= abs_b) {
            int n = abs_a.m_digit.size();

            if (n <= 2 * k) {
                big_int q_add = (abs_a * inv);
                q_add.shift_right(2 * k);

                big_int prod = q_add * abs_b;
                while (abs_a >= prod + abs_b) {
                    q_add++;
                    prod += abs_b;
                }

                abs_a -= prod;
                q += q_add;

                break;
            } else {
                int shift = n - 2 * k;

                big_int a_top;
                a_top.m_digit = std::vector<int>(abs_a.m_digit.begin() + shift, abs_a.m_digit.end());
                
                big_int q_add = (a_top * inv);
                q_add.shift_right(2 * k);

                if (q_add.is_zero()) {
                    q_add = 1;
                }
                
                big_int prod = q_add * abs_b;
                
                big_int b_shifted = abs_b;
                b_shifted.shift_left(shift);
                
                q_add.shift_left(shift);
                prod.shift_left(shift);
                
                while (abs_a < prod) {
                    q_add--;
                    prod -= b_shifted;
                }
                
                abs_a -= prod;
                q += q_add;
            }
        }
        
        q.m_sign = a.m_sign * b.m_sign;
        q.trim();
        
        big_int r = abs_a / norm;
        r.m_sign = a.m_sign;

        if (r.is_zero()) {
            r.m_sign = 1;
        }
        
        return { q, r };
    }

    friend big_int gcd(big_int a, big_int b) {
        a = a.abs();
        b = b.abs();

        while (!b.is_zero()) {
            big_int c = a % b;
            a = b;
            b = c;
        }

        return a;
    }

    friend big_int lcm(big_int a, big_int b) {
        if (a.is_zero() || b.is_zero()) {
            return big_int(0);
        }
        return (a * b).abs() / gcd(a, b);
    }

    friend big_int operator+(big_int lhs, const big_int& rhs) { 
        return lhs += rhs;
    }

    friend big_int operator-(big_int lhs, const big_int& rhs) {
        return lhs -= rhs;
    }

    friend big_int operator*(big_int lhs, const big_int& rhs) {
        return lhs *= rhs;
    }

    friend big_int operator/(big_int lhs, const big_int& rhs) {
        return lhs /= rhs;
    }

    friend big_int operator%(big_int lhs, const big_int& rhs) { 
        return lhs %= rhs; 
    }

    template <std::integral Tp>
    friend big_int operator*(big_int lhs, Tp rhs) {
        return lhs *= rhs;
    }

    template <std::integral Tp>
    friend big_int operator*(Tp lhs, const big_int& rhs) {
        return rhs * lhs;
    }

    template <std::integral Tp>
    friend big_int operator/(big_int lhs, Tp rhs) {
        return lhs /= rhs;
    }

    template <std::integral Tp>
    friend Tp operator%(const big_int& lhs, Tp rhs) {
        if (rhs == 0) {
            throw std::runtime_error("Modulo by zero");
        }

        if (rhs < 0) {
            rhs = -rhs;
        }
        
        long long m = 0;
        for (int i = static_cast<int>(lhs.m_digit.size()) - 1; i >= 0; --i) {
            m = (m * base + lhs.m_digit[i]) % rhs;
        }
        return static_cast<Tp>(m * lhs.m_sign);
    }

    friend bool operator==(const big_int& lhs, const big_int& rhs) {
        if (lhs.is_zero() && rhs.is_zero()) {
            return true;
        }
        
        return lhs.m_sign == rhs.m_sign && lhs.m_digit == rhs.m_digit;
    }

    friend bool operator!=(const big_int& lhs, const big_int& rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<(const big_int& lhs, const big_int& rhs) {
        if (lhs.m_sign != rhs.m_sign) {
            return lhs.m_sign < rhs.m_sign;
        }

        if (lhs.is_zero() && rhs.is_zero()) {
            return false;
        }

        int cmp = lhs.cmp_abs(rhs);
        return lhs.m_sign > 0 ? cmp < 0 : cmp > 0;
    }

    friend bool operator>(const big_int& lhs, const big_int& rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(const big_int& lhs, const big_int& rhs) {
        return !(rhs < lhs);
    }

    friend bool operator>=(const big_int& lhs, const big_int& rhs) {
        return !(lhs < rhs);
    }

    friend std::istream& operator>>(std::istream& is, big_int& x) {
        std::string s;
        if (is >> s) {
            x.read(s);
        }

        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const big_int& x) {
        return os << x.to_string();
    }

private:
    static constexpr int base = 1000000000;
    static constexpr int base_digits = 9;

    int m_sign;
    std::vector<int> m_digit;

    void trim() {
        while (!m_digit.empty() && m_digit.back() == 0) {
            m_digit.pop_back();
        }
        if (m_digit.empty()) {
            m_sign = 1;
        }
    }

    int cmp_abs(const big_int& rhs) const {
        if (m_digit.size() != rhs.m_digit.size()) {
            return m_digit.size() < rhs.m_digit.size() ? -1 : 1;
        }
        for (int i = static_cast<int>(m_digit.size()) - 1; i >= 0; --i) {
            if (m_digit[i] != rhs.m_digit[i]) {
                return m_digit[i] < rhs.m_digit[i] ? -1 : 1;
            }
        }
        return 0;
    }

    void read(std::string_view s) {
        m_sign = 1;
        m_digit.clear();
        int pos = 0;

        while (pos < static_cast<int>(s.size()) && (s[pos] == '-' || s[pos] == '+')) {
            if (s[pos] == '-') m_sign = -m_sign;
            ++pos;
        }

        for (int i = static_cast<int>(s.size()) - 1; i >= pos; i -= base_digits) {
            int x = 0;
            for (int j = std::max(pos, i - base_digits + 1); j <= i; ++j) {
                x = x * 10 + (s[j] - '0');
            }
            m_digit.push_back(x);
        }
        trim();
    }

    void shift_left(int shift) {
        if (shift <= 0 || is_zero()) {
            return;
        }
        m_digit.insert(m_digit.begin(), shift, 0);
    }

    void shift_right(int shift) {
        if (shift <= 0 || is_zero()) {
            return;
        }

        if (shift >= static_cast<int>(m_digit.size())) {
            m_digit.clear();
            m_sign = 1;
            return;
        }

        m_digit.erase(m_digit.begin(), m_digit.begin() + shift);
        trim();
    }

    big_int invert(int k) const {
        if (k <= 64) {
            big_int p;
            p.m_digit.assign(2 * k + 1, 0);
            p.m_digit.back() = 1;
            return slow_div_mod(p, *this).first;
        }
        
        int m = (k + 1) / 2;
        big_int b_top = *this;
        b_top.shift_right(k - m);
        
        big_int x = b_top.invert(m);
        
        big_int term1 = x;
        long long carry = 0;
        
        for (int& d : term1.m_digit) {
            long long cur = d * 2LL + carry;
            d = static_cast<int>(cur % base);
            carry = cur / base;
        }
        if (carry) {
            term1.m_digit.push_back(static_cast<int>(carry));
        }
        term1.shift_left(k + m);
        
        big_int term2 = (x * (*this)) * x;
        big_int x_new = term1 - term2;
        x_new.shift_right(2 * m);
        
        big_int p;
        p.m_digit.assign(2 * k + 1, 0);
        p.m_digit.back() = 1;
        
        big_int prod = x_new * (*this);
        if (prod > p) {
            big_int diff = prod - p;
            while (!diff.is_zero() && diff >= *this) {
                diff -= *this;
                --x_new;
            }
            if (!diff.is_zero()) {
                --x_new;
            }
        } else {
            big_int diff = p - prod;
            while (diff >= *this) {
                diff -= *this;
                ++x_new;
            }
        }
        return x_new;
    }

    static std::pair<big_int, big_int> slow_div_mod(const big_int& a, const big_int& b) {
        big_int abs_a = a.abs();
        big_int abs_b = b.abs();
        
        if (abs_a < abs_b) {
            return { big_int(0), a };
        }
        
        long long norm = base / (abs_b.m_digit.back() + 1);
        abs_a *= norm;
        abs_b *= norm;
        
        int n = abs_a.m_digit.size();
        int m = abs_b.m_digit.size();
        big_int q(0);
        q.m_digit.assign(n - m + 1, 0);
        
        abs_a.m_digit.push_back(0); 
        
        for (int i = n - m; i >= 0; --i) {
            long long temp = static_cast<long long>(abs_a.m_digit[i + m]) * base + abs_a.m_digit[i + m - 1];
            long long q_guess = temp / abs_b.m_digit.back();
            long long r_guess = temp % abs_b.m_digit.back();
            
            while (q_guess == base || (m > 1 && q_guess * abs_b.m_digit[m - 2] > r_guess * base + abs_a.m_digit[i + m - 2])) {
                q_guess--;
                r_guess += abs_b.m_digit.back();
                if (r_guess >= base) break;
            }
            
            long long carry = 0;
            for (int j = 0; j < m; ++j) {
                long long prod = q_guess * abs_b.m_digit[j] + carry;
                long long diff = abs_a.m_digit[i + j] - (prod % base);
                carry = prod / base;
                if (diff < 0) {
                    diff += base;
                    carry++;
                }
                abs_a.m_digit[i + j] = static_cast<int>(diff);
            }
            abs_a.m_digit[i + m] -= static_cast<int>(carry);
            
            if (abs_a.m_digit[i + m] < 0) {
                q_guess--;
                carry = 0;
                for (int j = 0; j < m; ++j) {
                    long long sum = abs_a.m_digit[i + j] + abs_b.m_digit[j] + carry;
                    abs_a.m_digit[i + j] = static_cast<int>(sum % base);
                    carry = sum / base;
                }
                abs_a.m_digit[i + m] += static_cast<int>(carry);
            }
            q.m_digit[i] = static_cast<int>(q_guess);
        }
        
        q.m_sign = a.m_sign * b.m_sign;
        q.trim();
        
        abs_a.trim();
        big_int r = abs_a / norm;
        r.m_sign = a.m_sign;
        if (r.is_zero()) r.m_sign = 1;
        
        return { q, r };
    }
};

} // namespace ds
} // namespace cp

namespace std {

template <>
class numeric_limits<cp::ds::big_int> {
public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr float_round_style round_style = round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_modulo = false;
    static constexpr int digits = 0;
    static constexpr int digits10 = 0;
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 10;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;

    static cp::ds::big_int min() noexcept { 
        return cp::ds::big_int(0);
    }

    static cp::ds::big_int lowest() noexcept {
        return cp::ds::big_int(0);
    }

    static cp::ds::big_int max() noexcept {
        return cp::ds::big_int(0);
    }

    static cp::ds::big_int epsilon() noexcept {
        return cp::ds::big_int(0);
    }

    static cp::ds::big_int round_error() noexcept {
        return cp::ds::big_int(0);
    }

    static cp::ds::big_int infinity() noexcept {
        return cp::ds::big_int(0);
    }

    static cp::ds::big_int quiet_NaN() noexcept {
        return cp::ds::big_int(0);
    }

    static cp::ds::big_int signaling_NaN() noexcept {
        return cp::ds::big_int(0);
    }

    static cp::ds::big_int denorm_min() noexcept {
        return cp::ds::big_int(0);
    }
};

} // namespace std

#endif