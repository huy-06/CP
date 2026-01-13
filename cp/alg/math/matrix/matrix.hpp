#include <vector>
#include <cassert>
#include "../arithmetic/arithmetic.hpp"

#ifndef CP_ALG_MATH_MATRIX
#define CP_ALG_MATH_MATRIX
namespace cp {

namespace internal {

template <typename T, typename = void>
struct needs_pivoting : std::is_floating_point<T> {};

template <typename T>
struct needs_pivoting<T, std::void_t<decltype(T::eps())>> : std::true_type {};

} // namespace internal

namespace alg {

template <typename Tp>
std::vector<std::vector<Tp>> identity(int n) {
    std::vector<std::vector<Tp>> res(n, std::vector<Tp>(n, Tp(0)));
    for (int i = 0; i < n; ++i) {
        res[i][i] = Tp(1);
    }
    return res;
}

template <typename Tp>
std::vector<std::vector<Tp>> multiply(const std::vector<std::vector<Tp>>& A, 
                                      const std::vector<std::vector<Tp>>& B) {
    if (A.empty() || B.empty()) {
        return {};
    }

    int na = int(A.size());
    int ma = int(A[0].size());
    int nb = int(B.size());
    int mb = int(B[0].size());
    assert(ma == nb);

    std::vector<std::vector<Tp>> C(na, std::vector<Tp>(mb, Tp(0)));
    for (int i = 0; i < na; ++i) {
        for (int k = 0; k < ma; ++k) {
            if (A[i][k] == Tp(0)) {
                continue;
            }
            for (int j = 0; j < mb; ++j) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return C;
}

template <typename Tp, typename Int>
std::vector<std::vector<Tp>> pow(std::vector<std::vector<Tp>> A, Int k) {
    assert(!A.empty() && A.size() == A[0].size());

    int n = int(A.size());
    auto res = identity<Tp>(n);

    while (k > 0) {
        if (k & 1) {
            res = multiply(res, A);
        }
        A = multiply(A, A);
        k >>= 1;
    }

    return res;
}

template <typename Tp>
std::vector<std::vector<Tp>> transpose(const std::vector<std::vector<Tp>>& A) {
    if (A.empty()) {
        return {};
    }

    int n = int(A.size());
    int m = int(A[0].size());

    std::vector<std::vector<Tp>> res(m, std::vector<Tp>(n));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            res[i][j] = A[j][i];
        }
    }

    return res;
}

template <typename Tp>
Tp determinant(std::vector<std::vector<Tp>> A) {
    if (A.empty()) {
        return Tp(1);
    }
    assert(A.size() == A[0].size());

    int n = int(A.size());
    Tp res = Tp(1);

    for (int i = 0; i < n; ++i) {
        int pivot = i;

        if constexpr (internal::needs_pivoting<Tp>::value) {
            for (int j = i + 1; j < n; ++j) {
                if (abs(A[j][i]) > abs(A[pivot][i])) {
                    pivot = j;
                }
            }
        } else {
            while (pivot < n && A[pivot][i] == Tp(0)) {
                ++pivot;
            }
        }

        if (pivot == n || A[pivot][i] == Tp(0)) {
            return Tp(0);
        }

        if (i != pivot) {
            std::swap(A[i], A[pivot]);
            res = -res;
        }

        res *= A[i][i];
        Tp inv = Tp(1) / A[i][i];

        for (int j = i + 1; j < n; ++j) {
            if (A[j][i] == Tp(0)) {
                continue;
            }

            Tp factor = A[j][i] * inv;
            for (int k = i; k < n; ++k) {
                A[j][k] -= factor * A[i][k];
            }
        }
    }

    return res;
}

template <typename Tp>
int rank(std::vector<std::vector<Tp>> A) {
    if (A.empty()) {
        return 0;
    }

    int n = int(A.size());
    int m = int(A[0].size());
    int r = 0;

    for (int j = 0; j < m && r < n; ++j) {
        int pivot = r;

        if constexpr (internal::needs_pivoting<Tp>::value) {
            for (int i = r + 1; i < n; ++i) {
                if (abs(A[i][j]) > abs(A[pivot][j])) {
                    pivot = i;
                }
            }
        } else {
            while (pivot < n && A[pivot][j] == Tp(0)) {
                ++pivot;
            }
        }

        if (pivot == n || A[pivot][j] == Tp(0)) {
            continue;
        }

        std::swap(A[r], A[pivot]);

        Tp inv = Tp(1) / A[r][j];
        for (int k = j; k < m; ++k) {
            A[r][k] *= inv;
        }

        for (int i = 0; i < n; ++i) {
            if (i != r && A[i][j] != Tp(0)) {
                Tp factor = A[i][j];
                for (int k = j; k < m; ++k) {
                    A[i][k] -= factor * A[r][k];
                }
            }
        }

        ++r;
    }

    return r;
}

template <typename Tp>
std::vector<std::vector<Tp>> inverse(std::vector<std::vector<Tp>> A) {
    assert(!A.empty() && A.size() == A[0].size());

    int n = int(A.size());
    auto B = identity<Tp>(n);

    for (int i = 0; i < n; ++i) {
        int pivot = i;

        if constexpr (internal::needs_pivoting<Tp>::value) {
            for (int j = i + 1; j < n; ++j) {
                if (abs(A[j][i]) > abs(A[pivot][i])) {
                    pivot = j;
                }
            }
        } else {
            while (pivot < n && A[pivot][i] == Tp(0)) {
                ++pivot;
            }
        }

        if (pivot == n || A[pivot][i] == Tp(0)) {
            return {};
        }

        std::swap(A[i], A[pivot]);
        std::swap(B[i], B[pivot]);

        Tp inv = Tp(1) / A[i][i];
        for (int k = i; k < n; ++k) {
            A[i][k] *= inv;
        }
        for (int k = 0; k < n; ++k) {
            B[i][k] *= inv;
        }

        for (int row = 0; row < n; ++row) {
            if (row != i && A[row][i] != Tp(0)) {
                Tp factor = A[row][i];
                for (int k = i; k < n; ++k) {
                    A[row][k] -= factor * A[i][k];
                }
                for (int k = 0; k < n; ++k) {
                    B[row][k] -= factor * B[i][k];
                }
            }
        }
    }

    return B;
}

template <typename Tp>
auto solve_linear_system(std::vector<std::vector<Tp>> A, const std::vector<Tp>& b) -> std::pair<std::vector<Tp>, std::vector<std::vector<Tp>>> {
    assert(A.size() == b.size());

    int n = int(A.size());
    int m = int(A[0].size());

    for (int i = 0; i < n; ++i) {
        A[i].push_back(b[i]);
    }

    int rank = 0;
    std::vector<int> col_pivot(m, -1);

    for (int j = 0; j < m && rank < n; ++j) {
        int pivot = rank;

        if constexpr (internal::needs_pivoting<Tp>::value) {
            for (int i = rank + 1; i < n; ++i) {
                if (abs(A[i][j]) > abs(A[pivot][j])) {
                    pivot = i;
                }
            }
        } else {
            while (pivot < n && A[pivot][j] == Tp(0)) {
                ++pivot;
            }
        }

        if (pivot == n || A[pivot][j] == Tp(0)) {
            continue;
        }

        std::swap(A[rank], A[pivot]);
        col_pivot[j] = rank;

        Tp inv = Tp(1) / A[rank][j];
        for (int k = j; k <= m; ++k) {
            A[rank][k] *= inv;
        }

        for (int i = 0; i < n; ++i) {
            if (i != rank && A[i][j] != Tp(0)) {
                Tp factor = A[i][j];
                for (int k = j; k <= m; ++k) {
                    A[i][k] -= factor * A[rank][k];
                }
            }
        }

        ++rank;
    }

    for (int i = rank; i < n; ++i) {
        if (A[i][m] != Tp(0)) {
            return {};
        }
    }

    std::vector<Tp> x(m, Tp(0));
    for (int j = 0; j < m; ++j) {
        if (col_pivot[j] != -1) {
            x[j] = A[col_pivot[j]][m];
        }
    }

    std::vector<std::vector<Tp>> kernel;
    for (int j = 0; j < m; ++j) {
        if (col_pivot[j] == -1) {
            std::vector<Tp> v(m, Tp(0));
            v[j] = Tp(1);
            for (int k = 0; k < j; ++k) {
                if (col_pivot[k] != -1) {
                    v[k] = -A[col_pivot[k]][j];
                }
            }
            kernel.push_back(v);
        }
    }

    return {x, kernel};
}

} // namesapce alg

} // namespace cp
#endif