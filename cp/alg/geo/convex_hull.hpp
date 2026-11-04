#include <algorithm>
#include <vector>
#include "vectors.hpp"
#include "../../ds/geo/polygon.hpp"

#ifndef CP_ALG_GEO_CONVEX_HULL
#define CP_ALG_GEO_CONVEX_HULL

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Tìm bao lồi (Convex Hull) 2D bằng thuật toán Chuỗi đơn điệu của Andrew (Monotone Chain).
 * @tparam Tp Kiểu tọa độ số học.
 * @param pts Tập điểm đầu vào.
 * @param include_collinear Nếu true, giữ lại các điểm thẳng hàng nằm trên cạnh của bao lồi.
 * @return ds::polygon<Tp> Danh sách các đỉnh của bao lồi theo thứ tự ngược chiều kim đồng hồ (CCW).
 * @complexity O(N log N) thời gian, O(N) bộ nhớ.
 */
template <class Tp>
ds::polygon<Tp> convex_hull(std::vector<ds::point<Tp>> pts, bool include_collinear = false) {
    int n = static_cast<int>(pts.size());
    if (n <= 1) return pts;

    std::sort(pts.begin(), pts.end());
    pts.erase(std::unique(pts.begin(), pts.end()), pts.end());
    n = static_cast<int>(pts.size());
    if (n <= 2) return pts;

    std::vector<ds::point<Tp>> hull;
    hull.reserve(2 * n);

    auto is_clockwise = [include_collinear](const ds::point<Tp>& o, const ds::point<Tp>& a, const ds::point<Tp>& b) {
        auto cp = cross(a - o, b - o);
        return include_collinear ? (cp < Tp(0)) : (cp <= Tp(0));
    };

    // Nửa bao lồi dưới (Lower hull)
    for (int i = 0; i < n; ++i) {
        while (hull.size() >= 2 && is_clockwise(hull[hull.size() - 2], hull.back(), pts[i])) {
            hull.pop_back();
        }
        hull.push_back(pts[i]);
    }

    // Nửa bao lồi trên (Upper hull)
    int lower_size = static_cast<int>(hull.size());
    for (int i = n - 2; i >= 0; --i) {
        while (hull.size() > lower_size && is_clockwise(hull[hull.size() - 2], hull.back(), pts[i])) {
            hull.pop_back();
        }
        hull.push_back(pts[i]);
    }

    hull.pop_back();
    return hull;
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_CONVEX_HULL
