#include "../../cst/real.hpp"
#include "../math/arith/arithmetic.hpp"

#ifndef CP_ALG_GEO_COMMON
#define CP_ALG_GEO_COMMON

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Hàm dấu (signum): trả về 1 nếu dương, -1 nếu âm, 0 nếu bằng 0.
 * Đối với các kiểu như ds::real, các toán tử so sánh đã tự động xử lý sai số eps.
 */
template <typename Tp>
constexpr int sgn(const Tp& val) {
    if (val > Tp(0)) return 1;
    if (val < Tp(0)) return -1;
    return 0;
}

/**
 * @brief Chuyển đổi góc từ độ (degrees) sang radian dùng hằng số cst::pi.
 */
template <typename Tp>
constexpr Tp deg_to_rad(const Tp& deg) {
    return deg * static_cast<Tp>(cst::pi) / static_cast<Tp>(180);
}

/**
 * @brief Chuyển đổi góc từ radian sang độ (degrees) dùng hằng số cst::pi.
 */
template <typename Tp>
constexpr Tp rad_to_deg(const Tp& rad) {
    return rad * static_cast<Tp>(180) / static_cast<Tp>(cst::pi);
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_COMMON
