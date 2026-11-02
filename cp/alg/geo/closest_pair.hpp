#include <algorithm>
#include <set>
#include <vector>
#include "distance.hpp"

#ifndef CP_ALG_GEO_CLOSEST_PAIR
#define CP_ALG_GEO_CLOSEST_PAIR

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Tìm cặp điểm gần nhau nhất trong tập điểm 2D bằng thuật toán Đường quét (Sweep-line).
 * @param pts Danh sách các điểm 2D.
 * @return std::pair<ds::point<Tp>, ds::point<Tp>> Cặp điểm có khoảng cách Euclidean nhỏ nhất.
 * @complexity O(N log N) thời gian, O(N) bộ nhớ.
 */
template <class Tp>
std::pair<ds::point<Tp>, ds::point<Tp>> closest_pair(std::vector<ds::point<Tp>> pts) {
    int n = static_cast<int>(pts.size());
    if (n <= 1) return {};

    std::sort(pts.begin(), pts.end(), [](const ds::point<Tp>& a, const ds::point<Tp>& b) {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    });

    auto cmp_y = [](const ds::point<Tp>& a, const ds::point<Tp>& b) {
        if (a.y != b.y) return a.y < b.y;
        return a.x < b.x;
    };
    std::set<ds::point<Tp>, decltype(cmp_y)> active(cmp_y);

    auto min_d = distance(pts[0], pts[1]);
    std::pair<ds::point<Tp>, ds::point<Tp>> best_pair = { pts[0], pts[1] };

    int left = 0;
    for (int i = 0; i < n; ++i) {
        while (left < i && pts[i].x - pts[left].x >= min_d) {
            active.erase(pts[left]);
            left++;
        }

        ds::point<Tp> lower_bound_pt(pts[i].x, pts[i].y - min_d);
        auto it_start = active.lower_bound(lower_bound_pt);

        for (auto it = it_start; it != active.end() && it->y <= pts[i].y + min_d; ++it) {
            auto d = distance(pts[i], *it);
            if (d < min_d) {
                min_d = d;
                best_pair = { pts[i], *it };
            }
        }

        active.insert(pts[i]);
    }

    return best_pair;
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_CLOSEST_PAIR
