#include <iostream>
#include "../../alg/math/polynomial/algebra.hpp"

#ifndef CP_DS_POLYNOMIAL
#define CP_DS_POLYNOMIAL
namespace cp {
namespace ds {

template <typename Tp>
class polynomial {
public:
    using value_type = Tp;

    polynomial(int n = 1, const value_type& v = value_type(0))
        : f(n, v) {}

    template <typename U>
    polynomial(const std::vector<U>& v)
        : f(v.begin(), v.end()) { trim(); }

    polynomial(std::vector<value_type>&& v)
        : f(std::move(v)) { trim(); }

    polynomial(const std::initializer_list<value_type>& v)
        : f(v) { trim(); }

    int size() const {
        return static_cast<int>(f.size());
    }

    bool empty() const {
        return f.empty();
    }

    int deg() const {
        return size() - 1;
    }

    std::vector<value_type> data() const {
        return f;
    }

    value_type& operator[](int i) {
        return f[i];
    }

    const value_type& operator[](int i) const {
        return f[i];
    }

    polynomial derivative() const {
        return polynomial(alg::derivative(f));
    }

    polynomial integral() const {
        return polynomial(alg::integral(f));
    }

    polynomial inv(int n) const {
        return polynomial(alg::inverse(f, n));
    } 

    polynomial log(int n) const {
        return polynomial(alg::log(f, n));
    }

    polynomial exp(int n) const {
        return polynomial(alg::exp(f, n));
    }

    polynomial pow(long long k, int n) const {
        return polynomial(alg::pow(f, k, n));
    }

    polynomial sqrt(int n) const {
        return polynomial(alg::sqrt(f, n));
    }

    polynomial shift(value_type c) const {
        return polynomial(alg::taylor_shift(f, c));
    }

    value_type eval(const value_type& x) {
        return alg::evaluate(f, x);
    }

    std::vector<value_type> eval(const std::vector<value_type>& pts) const {
        return alg::multipoint_evaluation(f, pts);
    }

    static polynomial interpolate(const std::vector<value_type>& x, const std::vector<value_type>& y) {
        return polynomial(alg::interpolation(x, y));
    }

    polynomial operator+() const {
        return *this;
    }

    polynomial operator-() const {
        polynomial res = *this;
        for (int i = 0; i < res.size(); ++i) {
            res[i] = -res[i];
        }
        return res;
    }

    polynomial& operator+=(const polynomial& rhs) {
        if (size() < rhs.size()) {
            f.resize(rhs.size());
        }
        for (int i = 0; i < rhs.size(); ++i) {
            f[i] += rhs.f[i];
        }
        trim();
        return *this;
    }

    polynomial& operator-=(const polynomial& rhs) {
        if (size() < rhs.size()) {
            f.resize(rhs.size());
        }
        for (int i = 0; i < rhs.size(); ++i) {
            f[i] -= rhs.f[i];
        }
        trim();
        return *this;
    }

    polynomial& operator*=(const polynomial& rhs) {
        if (empty() || rhs.empty()) {
            f.clear();
        } else {
            f = alg::convolution(f, rhs.f);
            trim();
        }
        return *this;
    }

    polynomial& operator/=(const polynomial& rhs) {
        f = std::move(internal::div_mod(f, rhs.f).first);
        trim();
        return *this;
    }

    polynomial& operator%=(const polynomial& rhs) {
        f = std::move(internal::div_mod(f, rhs.f).second);
        trim();
        return *this;
    }
    
    friend polynomial operator+(polynomial lhs, const polynomial& rhs) {
        return lhs += rhs;
    }

    friend polynomial operator-(polynomial lhs, const polynomial& rhs) {
        return lhs -= rhs;
    }

    friend polynomial operator*(polynomial lhs, const polynomial& rhs) {
        return lhs *= rhs;
    }

    friend polynomial operator/(polynomial lhs, const polynomial& rhs) {
        return lhs /= rhs;
    }

    friend polynomial operator%(polynomial lhs, const polynomial& rhs) {
        return lhs %= rhs;
    }

    friend bool operator==(const polynomial& lhs, const polynomial& rhs) {
        return lhs.f == rhs.f;
    }

    friend bool operator!=(const polynomial& lhs, const polynomial& rhs) {
        return lhs.f != rhs.f;
    }

    friend bool operator<(const polynomial& lhs, const polynomial& rhs) {
        return lhs.f < rhs.f;
    }

    friend bool operator>(const polynomial& lhs, const polynomial& rhs) {
        return lhs.f > rhs.f;
    }

    friend bool operator<=(const polynomial& lhs, const polynomial& rhs) {
        return lhs.f <= rhs.f;
    }

    friend bool operator>=(const polynomial& lhs, const polynomial& rhs) {
        return lhs.f >= rhs.f;
    }

    // Toán tử dịch bit << (Nhân với x^k)
    // P(x) << k  =>  P(x) * x^k
    polynomial operator<<(int k) const {
        if (k < 0) {
            return *this (-k);
        }
        if (empty()) {
            return *this;
        }
        std::vector<value_type> res(size() + k);
        for (int i = 0; i < size(); ++i) {
            res[i + k] = f[i];
        }
        return polynomial(res);
    }

    // Toán tử dịch bit >> (Chia nguyên cho x^k - Bỏ k hệ số đầu)
    // P(x) >> k  =>  P(x) / x^k
    polynomial operator>>(int k) const {
        if (k < 0) {
            return *this << (-k);
        }
        if (k >= size()) {
            return polynomial();
        }
        std::vector<value_type> res(f.begin() + k, f.end());
        return polynomial(res);
    }

    friend std::istream& operator>>(std::istream& is, polynomial& p) {
        for (int i = 0; i < size(); ++i) {
            is >> p.f[i];
        }
        p.trim();
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const polynomial& p) {
        if (p.empty()) {
            return os << '0';
        }
        
        bool first = true;
        for (int i = p.size() - 1; i >= 0; --i) {
            if (p.f[i] == value_type(0)) {
                continue;
            }
            if (!first) {
                os << " + ";
            }
            os << p.f[i];
            if (i > 0) {
                os << 'x';
                if (i > 1) {
                    os << '^' << i;
                }
            }
            first = false;
        }
        if (first) {
            os << '0';
        }
        return os;
    }

private:
    std::vector<value_type> f;

    void trim() {
        while (f.size() > 1 && f.back() == value_type(0)) {
            f.pop_back();
        }
    }
};


} // namespace ds
} // namespace cp
#endif