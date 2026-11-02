#include "lines.hpp"
#include "../../ds/geo/ray.hpp"

#ifndef CP_ALG_GEO_RAYS
#define CP_ALG_GEO_RAYS

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Kiểm tra điểm `p` có nằm trên tia `r` không.
 */
template <class Tp>
constexpr bool is_on(const ds::point<Tp>& p, const ds::ray<Tp>& r) {
    auto u = p - r.p;
    if (cross(r.v, u) != 0) return false;
    return dot(r.v, u) >= Tp(0);
}

/**
 * @brief Kiểm tra hai tia r1 và r2 có song song không.
 */
template <class Tp>
constexpr bool is_parallel(const ds::ray<Tp>& r1, const ds::ray<Tp>& r2) {
    return cross(r1.v, r2.v) == 0;
}

/**
 * @brief Hình chiếu vuông góc của điểm `p` lên tia `r` (giới hạn tham số t >= 0).
 */
template <class Tp>
auto project(const ds::point<Tp>& p, const ds::ray<Tp>& r) {
    auto len_sq = r.v.square();
    if (len_sq == 0) return r.p;

    auto t = dot(p - r.p, r.v) / len_sq;
    if (t < Tp(0)) t = Tp(0);
    return r.p + r.v * t;
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_RAYS
