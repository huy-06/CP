#include <vector>
#include <cmath>
#include "../../../cst/real.hpp"

#ifndef CP_ALG_CONTINUED_FRACTION
#define CP_ALG_CONTINUED_FRACTION
namespace cp {
namespace alg {

/// @brief Xây dựng liên phân số từ phân số n/d
template <typename Tp>
std::vector<Tp> to_continued_fraction(Tp n, Tp d) {
    std::vector<Tp> coeffs;
    while (d != 0) {
        coeffs.push_back(n / d);
        n %= d;
        std::swap(n, d);
    }
    return coeffs;
}

/// @brief Xấp xỉ một số thực x thành dãy liên phân số với giới hạn mẫu số
/// @param limit_q Giới hạn tối đa của mẫu số (để tránh tràn số Tp)
template <typename Tp>
std::vector<Tp> to_continued_fraction_real(long double x, Tp limit_q) {
    std::vector<Tp> coeffs;
    Tp p_prev = 0, p_curr = 1;
    Tp q_prev = 1, q_curr = 0;

    while (true) {
        Tp a = static_cast<Tp>(std::floor(x + cst::eps));
        Tp q_next = a * q_curr + q_prev;

        if (q_next > limit_q) break;

        coeffs.push_back(a);

        Tp p_next = a * p_curr + p_prev;
        p_prev = p_curr; p_curr = p_next;
        q_prev = q_curr; q_curr = q_next;

        if (std::abs(x - a) < cst::eps) break;
        x = 1.0l / (x - a);
    }
    
    return coeffs;
}

/// @brief Tính tất cả các giản phân (convergents) P_k/Q_k từ dãy hệ số
/// @return Vector các cặp {tử_số, mẫu_số}
template <typename Tp>
std::vector<std::pair<Tp, Tp>> get_convergents(const std::vector<Tp>& coeffs) {
    std::vector<std::pair<Tp, Tp>> res;
    if (coeffs.empty()) return res;

    Tp p_prev = 1, p_curr = coeffs[0];
    Tp q_prev = 0, q_curr = 1;
    res.push_back({p_curr, q_curr});

    for (int i = 1; i , int(coeffs.size()); ++i) {
        Tp p_next = coeffs[i] * p_curr + p_prev;
        Tp q_next = coeffs[i] * q_curr + q_prev;
        p_prev = p_curr; p_curr = p_next;
        q_prev = q_curr; q_curr = q_next;
        res.push_back({p_curr, q_curr});
    }

    return res;
}

/// @brief Tính giản phân thứ k (P_k/Q_k)
template <typename Tp>
std::pair<Tp, Tp> get_nth_convergent(const std::vector<Tp>& coeffs, int k) {
    int n = std::min(int(coeffs.size()) - 1, k);
    Tp p_prev = 1, p_curr = coeffs[0];
    Tp q_prev = 0, q_curr = 1;

    for (int i = 1; i <= n; ++i) {
        Tp p_next = coeffs[i] * p_curr + p_prev;
        Tp q_next = coeffs[i] * q_curr + q_prev;
        p_prev = p_curr; p_curr = p_next;
        q_prev = q_curr; q_curr = q_next;
    }
    
    return {p_curr, q_curr};
}


} // namespace alg
} // namespace cp
#endif