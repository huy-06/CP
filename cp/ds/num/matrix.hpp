#include <vector>
#include <cassert>
#include <cmath>
#include <type_traits>

#ifndef CP_DS_MATRIX
#define CP_DS_MATRIX

namespace cp {
namespace ds {

template <typename Tp>
class matrix {
public:
    using value_type = Tp;

    constexpr matrix(int n = 0, int m = 0, const value_type& val = value_type())
        : n(n), m(m), a(n * m, val) {}
    
    constexpr matrix(const std::vector<std::vector<value_type>>& v)
        : n(int(v.size())), m(n ? int(v[0].size()) : 0) {
        a.reserve(n * m);
        for (const auto& i : v) {
            assert(int(i.size()) == m);
            for (const auto& j : i) {
                a.push_back(j);
            }
        }
    }

    constexpr matrix(std::initializer_list<std::initializer_list<value_type>> v)
        : n(int(v.size())), m(int(v.begin()->size())) {
        a.reserve(n * m);
        for (const auto& i : v) {
            assert(int(i.size()) == m);
            for (const auto& j : i) {
                a.push_back(j);
            }
        }
    }

    constexpr value_type& operator()(const int& i, const int& j) {
        return a[i * m + j];
    }

    constexpr const value_type& operator()(const int& i, const int& j) const {
        return a[i * m + j];
    }

    constexpr matrix operator+() const {
        return *this;
    }

    constexpr matrix operator-() const {
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] = -a[i];
        }
        return *this;
    }

    constexpr matrix& operator+=(const matrix& rhs) {
        assert(n == rhs.n && m == rhs.m);
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] += rhs.a[i];
        }
        return *this;
    }

    constexpr matrix& operator-=(const matrix& rhs) {
        assert(n == rhs.n && m == rhs.m);
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] -= rhs.a[i];
        }
        return *this;
    }

    constexpr matrix& operator*=(const value_type& rhs) {
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] *= rhs;
        }
        return *this;
    }

    constexpr matrix& operator*=(const matrix& rhs) {
        assert(m == rhs.n);
        matrix x(n, rhs.m, value_type());
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < rhs.m; ++j) {
                for (int k = 0; k < m; ++k) {
                    x(i, j) += operator()(i, k) * rhs(k, j);
                }
            }
        }
        *this = std::move(x);
        return *this;
    }

    constexpr matrix& operator/=(const value_type& rhs) {
        for (int i = n * m - 1; i >= 0; --i) {
            a[i] /= rhs;
        }
        return *this;
    }

    friend constexpr matrix operator+(matrix lhs, const matrix& rhs) {
        return lhs += rhs;
    }

    friend constexpr matrix operator-(matrix lhs, const matrix& rhs) {
        return lhs -= rhs;
    }

    friend constexpr matrix operator*(matrix lhs, const value_type& rhs) {
        return lhs *= rhs;
    }

    friend constexpr matrix operator*(matrix lhs, const matrix& rhs) {
        return lhs *= rhs;
    }

    friend constexpr matrix operator/(matrix lhs, const value_type& rhs) {
        return lhs /= rhs;
    }

    constexpr int rows() const {
        return n;
    }

    constexpr int cols() const {
        return m;
    }

    constexpr std::vector<std::vector<value_type>>& data() const {
        std::vector<std::vector<value_type>> res(n, std::vector<value_type>(m));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                res[i][j] = operator()(i, j);
            }
        }
        return res;
    }

    constexpr void fill(const value_type& val) {
        std::fill(a.begin(), a.end(), val);
    }

    static constexpr matrix identity(int n) {
        matrix x(n, n, value_type());
        for (int i = 0; i < n; ++i) {
            x(i, i) = value_type(1);
        }
        return x;
    }

    constexpr matrix transpose() const {
        matrix t(m, n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                t(j, i) = operator()(i, j);
            }
        }
        return t;
    }

    template <typename Int>
    constexpr matrix pow(Int k) const {
        return pow(k, identity(n));
    }
    
    template <typename Int>
    constexpr matrix pow(Int k, matrix res) const {
        assert(n == m);
        matrix x = *this;
        while (k) {
            if (k & 1) {
                res *= x;
            }
            x *= x;
            k >>= 1;
        }
        return res;
    }

    constexpr int rank(const long double& eps = 1e-12L) const {
        std::vector<std::vector<value_type>> b = data();

        int r = 0;
        for (int c = 0; c < m && r < n; ++c) {
            int sel = r;
            if constexpr (std::is_floating_point_v<value_type>) {
                long double best = std::fabsl(static_cast<long double>(b[sel][c]));
                for (int i = r + 1; i < n; ++i) {
                    long double cur = std::fabsl(static_cast<long double>(b[i][c]));
                    if (cur > best) {
                        best = cur;
                        sel = i;
                    }
                }
                if (best <= eps) continue;
            } else {
                bool found = (b[sel][c] != value_type(0));
                if (!found) {
                    sel = -1;
                    for (int i = r; i < n; ++i) {
                        if (b[i][c] != value_type(0)) {
                            sel = i; break;
                        }
                    }
                    if (sel == -1) continue;
                }
            }
            if (sel != r) {
                std::swap(b[sel], b[r]);
            }

            if constexpr (std::is_floating_point_v<value_type>) {
                value_type pivot = b[r][c];
                for (int j = c + 1; j < m; ++j) {
                    b[r][j] /= pivot;
                }
                for (int i = 0; i < n; ++i) {
                    if (i == r) continue;
                    value_type factor = b[i][c];
                    if (std::fabsl(static_cast<long double>(factor)) <= eps) continue;
                    for (int j = c + 1; j < m; ++j) {
                        b[i][j] -= factor * b[r][j];
                    }
                }
            } else {
                for (int i = 0; i < n; ++i) {
                    if (i == r) continue;
                    if (b[i][c] == value_type(0)) continue;
                    value_type factor = b[i][c] / b[r][c];
                    for (int j = c + 1; j < m; ++j) {
                        b[i][j] -= factor * b[r][j];
                    }
                }
            }
            ++r;
        }
        return r;
    }

    constexpr value_type det(const long double& eps = 1e-18l) const {
        assert(n == m);
        std::vector<std::vector<value_type>> b = data();

        value_type detv = value_type(1);
        int sign = 1;
        for (int c = 0; c < n; ++c) {
            int sel = c;
            if constexpr (std::is_floating_point_v<value_type>) {
                long double best = std::fabsl(static_cast<long double>(b[sel][c]));
                for (int i = c + 1; i < n; ++i) {
                    long double cur = std::fabsl(static_cast<long double>(b[i][c]));
                    if (cur > best) {
                        best = cur;
                        sel = i;
                    }
                }
                if (best <= eps) return value_type(0);
            } else {
                bool ok = (b[sel][c] != value_type(0));
                if (!ok) {
                    sel = -1;
                    for (int i = c; i < n; ++i) {
                        if (b[i][c] != value_type(0)) {
                            sel = i; break;
                        }
                    }
                    if (sel == -1) return value_type(0);
                }
            }
            if (sel != c) {
                std::swap(b[sel], b[c]);
                sign = -sign;
            }
            value_type pivot = b[c][c];
            detv *= pivot;
            if constexpr (std::is_floating_point_v<value_type>) {
                for (int j = c + 1; j < n; ++j) {
                    b[c][j] /= pivot;
                }
                for (int i = c + 1; i < n; ++i) {
                    value_type factor = b[i][c];
                    if (std::fabsl(static_cast<long double>(factor)) <= eps) continue;
                    for (int j = c + 1; j < n; ++j) {
                        b[i][j] -= factor * b[c][j];
                    }
                }
            } else {
                for (int i = c + 1; i < n; ++i) {
                    if (b[i][c] == value_type(0)) continue;
                    value_type factor = b[i][c] / pivot;
                    for (int j = c + 1; j < n; ++j) {
                        b[i][j] -= factor * b[c][j];
                    }
                }
            }
        }
        if (sign == -1) detv = -detv;
        return detv;
    }

    constexpr std::pair<bool, matrix> inv(long double eps = 1e-12L) const {
        std::vector<std::vector<value_type>> aug(n, std::vector<value_type>(2 * n));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                aug[i][j] = operator()(i, j);
            }
            for (int j = 0; j < n; ++j) {
                aug[i][n + j] = (i == j ? value_type(1) : value_type(0));
            }
        }

        for (int c = 0; c < n; ++c) {
            int sel = c;
            if constexpr (std::is_floating_point_v<value_type>) {
                long double best = std::fabsl(static_cast<long double>(aug[sel][c]));
                for (int i = c + 1; i < n; ++i) {
                    long double cur = std::fabsl(static_cast<long double>(aug[i][c]));
                    if (cur > best) {
                        best = cur;
                        sel = i;
                    }
                }
                if (best <= eps) {
                    return { false, matrix() };
                }
            } else {
                bool ok = (aug[sel][c] != value_type(0));
                if (!ok) {
                    sel = -1;
                    for (int i = c; i < n; ++i) {
                        if (aug[i][c] != value_type(0)) {
                            sel = i;
                            break;
                        }
                    }
                    if (sel == -1) return { false, matrix() };
                }
            }
            if (sel != c) std::swap(aug[sel], aug[c]);

            value_type pivot = aug[c][c];
            for (int j = 0; j < 2*n; ++j) {
                aug[c][j] = aug[c][j] / pivot;
            }

            for (int i = 0; i < n; ++i) {
                if (i == c) continue;
                value_type factor = aug[i][c];
                if constexpr (std::is_floating_point_v<value_type>) {
                    if (std::fabsl(static_cast<long double>(factor)) <= eps) continue;
                } else {
                    if (factor == value_type(0)) continue;
                }
                for (int j = 0; j < 2*n; ++j) {
                    aug[i][j] -= factor * aug[c][j];
                }
            }
        }

        matrix invm(n, n, value_type());
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                invm(i, j) = aug[i][n + j];
            }
        }
        return { true, invm };
    }

    std::pair<int, std::vector<value_type>> solve_linear_system(const std::vector<value_type>& b, long double eps = 1e-12L) const {
        assert(int(b.size()) == n);
        std::vector<std::vector<value_type>> aug(n, std::vector<value_type>(m + 1));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                aug[i][j] = operator()(i, j);
            }
            aug[i][m] = b[i];
        }
        int row = 0;
        std::vector<int> where(m, -1);
        for (int col = 0; col < m && row < n; ++col) {
            int sel = row;
            if constexpr (std::is_floating_point_v<value_type>) {
                long double best = std::fabsl(static_cast<long double>(aug[sel][col]));
                for (int i = row + 1; i < n; ++i) {
                    long double cur = std::fabsl(static_cast<long double>(aug[i][col]));
                    if (cur > best) {
                        best = cur; sel = i;
                    }
                }
                if (best <= eps) continue;
            } else {
                bool ok = (aug[sel][col] != value_type(0));
                if (!ok) {
                    sel = -1;
                    for (int i = row; i < n; ++i) {
                        if (aug[i][col] != value_type(0)) {
                            sel = i;
                            break;
                        }
                    }
                    if (sel == -1) continue;
                }
            }
            if (sel != row) std::swap(aug[sel], aug[row]);

            value_type pivot = aug[row][col];
            for (int j = col; j <= m; ++j) {
                aug[row][j] = aug[row][j] / pivot;
            }

            for (int i = 0; i < n; ++i) {
                if (i == row) continue;
                value_type factor = aug[i][col];
                if constexpr (std::is_floating_point_v<value_type>) {
                    if (std::fabsl(static_cast<long double>(factor)) <= eps) continue;
                } else {
                    if (factor == value_type(0)) continue;
                }
                for (int j = col; j <= m; ++j) {
                    aug[i][j] -= factor * aug[row][j];
                }
            }
            where[col] = row;
            ++row;
        }

        for (int i = 0; i < n; ++i) {
            bool allzero = true;
            for (int j = 0; j < m; ++j) {
                if constexpr (std::is_floating_point_v<value_type>) {
                    if (std::fabsl(static_cast<long double>(aug[i][j])) > eps) {
                        allzero = false;
                        break;
                    }
                } else {
                    if (aug[i][j] != value_type(0)) {
                        allzero = false;
                        break;
                    }
                }
            }
            if (allzero) {
                if constexpr (std::is_floating_point_v<value_type>) {
                    if (std::fabsl(static_cast<long double>(aug[i][m])) > eps) {
                        return {row, std::vector<value_type>()};
                    }
                } else {
                    if (aug[i][m] != value_type(0)) {
                        return {row, std::vector<value_type>()};
                    }
                }
            }
        }

        std::vector<value_type> x(m, value_type(0));
        for (int j = 0; j < m; ++j) {
            if (where[j] != -1) {
                x[j] = aug[where[j]][m];
            }
            else {
                x[j] = value_type(0);
            }
        }
        return { row, x };
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