#include <algorithm>
#include <utility>
#include "distance.hpp"
#include "../../ds/geo/polygon.hpp"

#ifndef CP_ALG_GEO_ROTATING_CALIPERS
#define CP_ALG_GEO_ROTATING_CALIPERS

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Đường kính đa giác (Polygon diameter): tìm cặp điểm đối cực xa nhau nhất trên bao lồi.
 * @param hull Đa giác lồi theo thứ tự CCW.
 * @return std::pair<ds::point<Tp>, ds::point<Tp>> Cặp hai điểm xa nhất.
 * @complexity O(N) thời gian bằng kỹ thuật Thước cặp quay (Rotating Calipers).
 */
template <class Tp>
std::pair<ds::point<Tp>, ds::point<Tp>> polygon_diameter(const ds::polygon<Tp>& hull) {
    int n = static_cast<int>(hull.size());
    if (n <= 1) return { hull.front(), hull.front() };
    if (n == 2) return { hull[0], hull[1] };

    int k = 1;
    while (math::abs(cross(hull[1] - hull[0], hull[(k + 1) % n] - hull[0])) >
           math::abs(cross(hull[1] - hull[0], hull[k] - hull[0]))) {
        k = (k + 1) % n;
    }

    auto max_dist_sq = (hull[0] - hull[k]).square();
    std::pair<ds::point<Tp>, ds::point<Tp>> best = { hull[0], hull[k] };

    for (int i = 0, j = k; i < n; ++i) {
        while (math::abs(cross(hull[(i + 1) % n] - hull[i], hull[(j + 1) % n] - hull[i])) >
               math::abs(cross(hull[(i + 1) % n] - hull[i], hull[j] - hull[i]))) {
            j = (j + 1) % n;
        }
        auto d1 = (hull[i] - hull[j]).square();
        if (d1 > max_dist_sq) {
            max_dist_sq = d1;
            best = { hull[i], hull[j] };
        }
        auto d2 = (hull[(i + 1) % n] - hull[j]).square();
        if (d2 > max_dist_sq) {
            max_dist_sq = d2;
            best = { hull[(i + 1) % n], hull[j] };
        }
    }
    return best;
}

/**
 * @brief Khoảng cách ngắn nhất giữa hai đa giác lồi không giao nhau.
 * @param P Đa giác lồi thứ nhất (CCW).
 * @param Q Đa giác lồi thứ hai (CCW).
 * @complexity O(|P| + |Q|) thời gian bằng Thước cặp quay.
 */
template <class Tp>
auto min_distance_convex(const ds::polygon<Tp>& P, const ds::polygon<Tp>& Q) {
    int n = static_cast<int>(P.size());
    int m = static_cast<int>(Q.size());
    if (n == 0 || m == 0) return decltype(distance(P[0], Q[0]))(0);

    int l = 0, r = 0;
    for (int i = 1; i < n; ++i) if (P[i].y < P[l].y) l = i;
    for (int i = 1; i < m; ++i) if (Q[i].y > Q[r].y) r = i;

    auto ans = distance(P[l], Q[r]);

    for (int i = 0; i < n; ++i) {
        while (cross(P[(l + 1) % n] - P[l], Q[(r + 1) % m] - Q[r]) < Tp(0)) {
            r = (r + 1) % m;
        }
        ans = std::min({
            ans,
            distance(P[l], ds::segment<Tp>(Q[r], Q[(r + 1) % m])),
            distance(Q[r], ds::segment<Tp>(P[l], P[(l + 1) % n]))
        });
        l = (l + 1) % n;
    }
    return ans;
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_ROTATING_CALIPERS
