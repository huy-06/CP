#include <iostream>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>
#include <initializer_list>
#include "../mod/is_modint.hpp"
#include "../../../alg/math/arith/arithmetic.hpp"

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

    value_type& operator()(int i, int j) {
        return a[i * m + j];
    }

    const value_type& operator()(int i, int j) const {
        return a[i * m + j];
    }

    value_type& operator[](int i, int j) {
        return a[i * m + j];
    }

    const value_type& operator[](int i, int j) const {
        return a[i * m + j];
    }

    static matrix identity(int n) {
        assert(n >= 0);
        matrix res(n, n, value_type(0));
        for (int i = 0; i < n; ++i) {
            res[i, i] = value_type(1);
        }
        return res;
    }

    template <typename Int>
    matrix pow(Int k) const {
        assert(n == m && "Matrix must be square to exponentiate");
        assert(k >= 0);
        matrix base = *this;
        matrix res = identity(n);
        while (k > 0) {
            if (k & 1) res *= base;
            if (k > 1) base *= base;
            k >>= 1;
        }
        return res;
    }

    matrix trans() const {
        matrix res(m, n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                res[j, i] = (*this)[i, j];
            }
        }
        return res;
    }

    value_type det() const {
        assert(n == m && "Matrix must be square to compute determinant");
        matrix mat = *this;
        value_type res = value_type(1);

        for (int i = 0; i < n; ++i) {
            int pivot = i;

            if constexpr (modint<value_type>) {
                for (int r = i; r < n; ++r) {
                    if (mat[r, i] != value_type(0)) {
                        pivot = r;
                        break;
                    }
                }
            } else {
                for (int r = i + 1; r < n; ++r) {
                    if (alg::abs(mat[r, i]) > alg::abs(mat[pivot, i])) {
                        pivot = r;
                    }
                }
            }

            if (mat[pivot, i] == value_type(0)) return value_type(0);

            if (pivot != i) {
                mat.swap_rows(i, pivot);
                res = -res;
            }

            res *= mat[i, i];
            value_type inv_val = value_type(1) / mat[i, i];

            int row_i = i * n;
            for (int r = i + 1; r < n; ++r) {
                if (mat[r, i] == value_type(0)) continue;
                value_type factor = mat[r, i] * inv_val;
                int row_r = r * n;
                for (int c = i; c < n; ++c) {
                    mat.a[row_r + c] -= factor * mat.a[row_i + c];
                }
            }
        }
        return res;
    }

    int rank() const {
        matrix mat = *this;
        int r = 0;

        for (int c = 0; c < m && r < n; ++c) {
            int pivot = r;

            if constexpr (modint<value_type>) {
                for (int i = r; i < n; ++i) {
                    if (mat[i, c] != value_type(0)) {
                        pivot = i;
                        break;
                    }
                }
            } else {
                for (int i = r + 1; i < n; ++i) {
                    if (alg::abs(mat[i, c]) > alg::abs(mat[pivot, c])) {
                        pivot = i;
                    }
                }
            }

            if (mat[pivot, c] == value_type(0)) continue;

            mat.swap_rows(r, pivot);
            value_type inv_val = value_type(1) / mat[r, c];

            int row_r = r * m;
            for (int i = r + 1; i < n; ++i) {
                if (mat[i, c] == value_type(0)) continue;
                value_type factor = mat[i, c] * inv_val;
                int row_i = i * m;
                for (int j = c; j < m; ++j) {
                    mat.a[row_i + j] -= factor * mat.a[row_r + j];
                }
            }
            ++r;
        }
        return r;
    }

    std::optional<matrix> inv() const {
        assert(n == m && "Matrix must be square to invert");

        matrix mat = *this;
        matrix res = identity(n);

        for (int i = 0; i < n; ++i) {
            int pivot = i;

            if constexpr (modint<value_type>) {
                for (int r = i; r < n; ++r) {
                    if (mat[r, i] != value_type(0)) {
                        pivot = r;
                        break;
                    }
                }
            } else {
                for (int r = i + 1; r < n; ++r) {
                    if (alg::abs(mat[r, i]) > alg::abs(mat[pivot, i])) {
                        pivot = r;
                    }
                }
            }

            if (mat[pivot, i] == value_type(0)) {
                return std::nullopt;
            }

            if (pivot != i) {
                mat.swap_rows(i, pivot);
                res.swap_rows(i, pivot);
            }

            value_type factor = value_type(1) / mat[i, i];
            int row_i = i * n;
            for (int j = 0; j < n; ++j) {
                mat.a[row_i + j] *= factor;
                res.a[row_i + j] *= factor;
            }

            for (int r = 0; r < n; ++r) {
                if (r == i || mat[r, i] == value_type(0)) continue;
                value_type mul = mat[r, i];
                int row_r = r * n;
                for (int j = 0; j < n; ++j) {
                    mat.a[row_r + j] -= mul * mat.a[row_i + j];
                    res.a[row_r + j] -= mul * res.a[row_i + j];
                }
            }
        }
        return res;
    }

    /**
     * @brief Giải hệ phương trình đại số tuyến tính dạng A * x = b.
     * 
     * @details Hàm sử dụng phương pháp khử Gauss-Jordan trên ma trận mở rộng [A | b]
     * để đưa về dạng bậc thang rút gọn (Reduced Row Echelon Form - RREF).
     * 
     * Nghiệm tổng quát của hệ có dạng:
     *      x = x_particular + c_1 * v_1 + c_2 * v_2 + ... + c_k * v_k
     * trong đó:
     * - x_particular: Là một nghiệm riêng của hệ phương trình.
     * - {v_1, v_2, ..., v_k}: Là hệ cơ sở của không gian nghiệm thuần nhất (Kernel / Null space)
     *   thỏa mãn A * v_i = 0.
     * - k = m - rank(A): Bậc tự do (số biến tự do).
     * 
     * @param b Vector vế phải của phương trình, có kích thước bắt buộc phải bằng số hàng (n).
     * 
     * @return std::pair<std::vector<Tp>, std::vector<std::vector<Tp>>>
     * - `first`: Vector nghiệm riêng x (kích thước m).
     * - `second`: Tập hợp các vector cơ sở của kernel (mỗi vector kích thước m).
     * 
     * @note Cách nhận biết các trường hợp nghiệm:
     * - **Vô nghiệm**: `first` rỗng (nghĩa là `res.first.empty() == true`).
     * - **Nghiệm duy nhất**: `first` không rỗng và `second` rỗng (`res.second.empty() == true`).
     * - **Vô số nghiệm**: `first` không rỗng và `second` chứa k vector cơ sở (`res.second.size() == k > 0`).
     * 
     * @pre Yêu cầu `b.size() == num_rows()`.
     * @complexity Thời gian: O(n * m * min(n, m)), Bộ nhớ phụ trợ: O(n * m).
     * 
     * @code
     * // Ví dụ sử dụng:
     * matrix<double> A = {
     *     {1, 2, 3},
     *     {4, 5, 6}
     * };
     * std::vector<double> b = {1, 2};
     * 
     * auto [particular, ker] = A.solve_linear_system(b);
     * 
     * if (particular.empty()) {
     *     std::cout << "Hệ vô nghiệm\n";
     * } else if (ker.empty()) {
     *     std::cout << "Nghiệm duy nhất: ";
     *     for (auto x : particular) std::cout << x << " ";
     *     std::cout << "\n";
     * } else {
     *     std::cout << "Vô số nghiệm với bậc tự do = " << ker.size() << "\n";
     *     std::cout << "Nghiệm riêng: ";
     *     for (auto x : particular) std::cout << x << " ";
     *     std::cout << "\nCơ sở kernel:\n";
     *     for (const auto& v : ker) {
     *         for (auto x : v) std::cout << x << " ";
     *         std::cout << "\n";
     *     }
     * }
     * @endcode
     */
    std::pair<std::vector<Tp>, std::vector<std::vector<Tp>>> solve_linear_system(const std::vector<value_type>& b) const {
        assert(int(b.size()) == n);

        matrix aug(n, m + 1);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                aug[i, j] = (*this)[i, j];
            }
            aug[i, m] = b[i];
        }

        std::vector<int> pivot_col(n, -1);
        int row = 0;

        for (int col = 0; col < m && row < n; ++col) {
            int pivot = row;

            if constexpr (modint<value_type>) {
                for (int r = row; r < n; ++r) {
                    if (aug[r, col] != value_type(0)) {
                        pivot = r;
                        break;
                    }
                }
            } else {
                for (int r = row + 1; r < n; ++r) {
                    if (alg::abs(aug[r, col]) > alg::abs(aug[pivot, col])) {
                        pivot = r;
                    }
                }
            }

            if (aug[pivot, col] == value_type(0)) continue;

            aug.swap_rows(row, pivot);
            pivot_col[row] = col;

            value_type inv_val = value_type(1) / aug[row, col];
            int row_offset = row * (m + 1);
            for (int j = col; j <= m; ++j) {
                aug.a[row_offset + j] *= inv_val;
            }

            for (int r = 0; r < n; ++r) {
                if (r != row && aug[r, col] != value_type(0)) {
                    value_type mult = aug[r, col];
                    int r_offset = r * (m + 1);
                    for (int j = col; j <= m; ++j) {
                        aug.a[r_offset + j] -= mult * aug.a[row_offset + j];
                    }
                }
            }
            ++row;
        }

        // Kiểm tra vô nghiệm
        for (int r = row; r < n; ++r) {
            if (aug[r, m] != value_type(0)) {
                return {{}, {}};
            }
        }

        // Nghiệm riêng
        std::vector<Tp> x(m, Tp(0));
        std::vector<bool> is_pivot(m, false);
        for (int r = 0; r < row; ++r) {
            x[pivot_col[r]] = aug[r, m];
            is_pivot[pivot_col[r]] = true;
        }

        // Cơ sở nghiệm thuần nhất kernel
        std::vector<std::vector<Tp>> ker;
        for (int c = 0; c < m; ++c) {
            if (!is_pivot[c]) {
                std::vector<Tp> v(m, Tp(0));
                v[c] = Tp(1);
                for (int r = 0; r < row; ++r) {
                    v[pivot_col[r]] -= aug[r, c];
                }
                ker.push_back(std::move(v));
            }
        }

        return {x, ker};
    }

    matrix operator+() const {
        return *this;
    }

    matrix operator-() const {
        matrix res = *this;
        for (int i = n * m - 1; i >= 0; --i) {
            res.a[i] = -res.a[i];
        }
        return res;
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
        assert(m == rhs.n && "Matrix dimension mismatch for multiplication");
        matrix res(n, rhs.m, value_type(0));
        for (int i = 0; i < n; ++i) {
            int lhs_row = i * m;
            int res_row = i * rhs.m;
            for (int k = 0; k < m; ++k) {
                if (a[lhs_row + k] == value_type(0)) continue;
                const value_type& val = a[lhs_row + k];
                int rhs_row = k * rhs.m;
                for (int j = 0; j < rhs.m; ++j) {
                    res.a[res_row + j] += val * rhs.a[rhs_row + j];
                }
            }
        }
        return *this = std::move(res);
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

    friend matrix operator*(const value_type& lhs, matrix rhs) {
        return rhs *= lhs;
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
                os << x[i, j] << " \n"[j + 1 == x.m];
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

    void swap_rows(int r1, int r2) {
        if (r1 == r2) return;
        int idx1 = r1 * m;
        int idx2 = r2 * m;
        for (int j = 0; j < m; ++j) {
            std::swap(a[idx1 + j], a[idx2 + j]);
        }
    }
};

} // namespace ds
} // namespace cp
#endif