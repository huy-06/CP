#include <algorithm>
#include <deque>
#include <vector>
#include "intersect.hpp"
#include "../../ds/geo/half_plane.hpp"
#include "../../ds/geo/polygon.hpp"

#ifndef CP_ALG_GEO_HALF_PLANE_HPP
#define CP_ALG_GEO_HALF_PLANE_HPP

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Giao các nửa mặt phẳng (Half-plane Intersection).
 * Mỗi nửa mặt phẳng lấy miền điểm nằm bên trái đường thẳng định hướng (p -> p + v).
 * @tparam Tp Kiểu tọa độ số học.
 * @param hps Danh sách các nửa mặt phẳng.
 * @return ds::polygon<Tp> Đa giác lồi giao điểm thu được theo thứ tự CCW.
 * Trả về rỗng nếu miền giao nhau rỗng.
 * @complexity O(N log N) thời gian với N là số nửa mặt phẳng.
 */
template <class Tp>
ds::polygon<Tp> half_plane_intersection(std::vector<ds::half_plane<Tp>> hps) {
    std::sort(hps.begin(), hps.end());

    // Loại bỏ các nửa mặt phẳng song song cùng hướng, chỉ giữ lại nửa mặt phẳng chặt chẽ nhất
    std::vector<ds::half_plane<Tp>> unique_hps;
    for (size_t i = 0; i < hps.size(); ++i) {
        if (!unique_hps.empty() && hps[i].angle == unique_hps.back().angle) {
            continue;
        }
        unique_hps.push_back(hps[i]);
    }

    std::deque<ds::half_plane<Tp>> q;
    std::deque<ds::point<Tp>> pts;

    auto compute_intersection = [&](const ds::half_plane<Tp>& h1, const ds::half_plane<Tp>& h2) -> ds::point<Tp> {
        auto inter = intersect(ds::line<Tp>(h1.p, h1.v), ds::line<Tp>(h2.p, h2.v));
        return inter.get_point();
    };

    for (const auto& hp : unique_hps) {
        while (!pts.empty() && !hp.contains(pts.back())) {
            pts.pop_back();
            q.pop_back();
        }
        while (!pts.empty() && !hp.contains(pts.front())) {
            pts.pop_front();
            q.pop_front();
        }

        if (!q.empty()) {
            pts.push_back(compute_intersection(q.back(), hp));
        }
        q.push_back(hp);
    }

    while (!pts.empty() && !q.front().contains(pts.back())) {
        pts.pop_back();
        q.pop_back();
    }

    if (q.size() <= 2) {
        return {};
    }

    pts.push_back(compute_intersection(q.back(), q.front()));
    return ds::polygon<Tp>(pts.begin(), pts.end());
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_HALF_PLANE_HPP
