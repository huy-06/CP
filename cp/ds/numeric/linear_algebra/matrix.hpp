
#include <iostream> 
#include "../../../alg/math/matrix/matrix.hpp"

#ifndef CP_DS_MATRIX
#define CP_DS_MATRIX

namespace cp {
namespace ds {

template <typename Tp>
class matrix {
public:
    using value_type = Tp;

    matrix(int n = 0, int m = 0, const value_type& val = value_type())
        : n(n), m(m), a(n * m, val) {}
    
    matrix(const std::vector<std::vector<value_type>>& v)
        : n(int(v.size())), m(n ? int(v[0].size()) : 0) {
        a.reserve(n * m);
        for (const auto& i : v) {
            assert(int(i.size()) == m);
            for (const auto& j : i) {
                a.push_back(j);
            }
        }
    }

    matrix(std::initializer_list<std::initializer_list<value_type>> v)
        : n(int(v.size())), m(int(v.begin()->size())) {
        a.reserve(n * m);
        for (const auto& i : v) {
            assert(int(i.size()) == m);
            for (const auto& j : i) {
                a.push_back(j);
            }
        }
    }

    int num_rows() const {
        return n;
    }

    int num_cols() const {
        return m;
    }

    std::vector<std::vector<value_type>> data() const {
        std::vector<std::vector<value_type>> res(n, std::vector<value_type>(m));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                res[i][j] = a[i * m + j];
            }
        }
        return res;
    }

    void fill(const value_type& val) {
        std::fill(a.begin(), a.end(), val);
    }

    value_type& operator()(const int& i, const int& j) {
        return a[i * m + j];
    }

    const value_type& operator()(const int& i, const int& j) const {
        return a[i * m + j];
    }

    static matrix identity(int n) {
        return matrix(alg::identity<value_type>(n));
    }

    template <typename Int>
    matrix pow(Int k) const {
        return matrix(alg::pow(data(), k));
    }

    matrix transpose() const {
        return matrix(alg::transpose(data()));
    }

    value_type determinant() const {
        return alg::determinant(data());
    }

    int rank() const {
        return alg::rank(data());
    }

    matrix inv() const {
        return matrix(alg::inverse(data()));
    }

    std::pair<
        std::vector<value_type>, 
        std::vector<std::vector<value_type>>
    > solve_linear_system(const std::vector<value_type>& b) const  {
        return alg::solve_linear_system(data(), b);
    }

    matrix operator+() const {
        return *this;
    }

    matrix operator-() const {
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] = -a[i];
        }
        return *this;
    }

    matrix& operator+=(const matrix& rhs) {
        assert(n == rhs.n && m == rhs.m);
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] += rhs.a[i];
        }
        return *this;
    }

    matrix& operator-=(const matrix& rhs) {
        assert(n == rhs.n && m == rhs.m);
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] -= rhs.a[i];
        }
        return *this;
    }

    matrix& operator*=(const value_type& rhs) {
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] *= rhs;
        }
        return *this;
    }

    matrix& operator*=(const matrix& rhs) {
        return *this = matrix(alg::multiply(data(), rhs.data()));
    }

    matrix& operator/=(const value_type& rhs) {
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] /= rhs;
        }
        return *this;
    }

    friend matrix operator+(matrix lhs, const matrix& rhs) {
        return lhs += rhs;
    }

    friend matrix operator-(matrix lhs, const matrix& rhs) {
        return lhs -= rhs;
    }

    friend matrix operator*(matrix lhs, const value_type& rhs) {
        return lhs *= rhs;
    }

    friend matrix operator*(matrix lhs, const matrix& rhs) {
        return lhs *= rhs;
    }

    friend matrix operator/(matrix lhs, const value_type& rhs) {
        return lhs /= rhs;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const matrix& x) {
        for (int i = 0; i < x.n; ++i) {
            for (int j = 0; j < x.m; ++j) {
                os << x(i, j) << " \n"[j + 1 == x.m];
            }
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, matrix& x) {
        for (auto& i : x.a) {
            is >> i;
        }
        return is;
    }

private:
    int n, m;
    std::vector<value_type> a;
};

} // namespace ds
} // namespace cp

#endif