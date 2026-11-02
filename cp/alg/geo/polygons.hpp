#include "distance.hpp"
#include "../../ds/geo/polygon.hpp"

#ifndef CP_ALG_GEO_POLYGONS
#define CP_ALG_GEO_POLYGONS

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Tính diện tích có dấu của đa giác bằng công thức Shoelace (Định lý Green).
 * Dương nếu các đỉnh xếp ngược chiều kim đồng hồ (CCW), âm nếu cùng chiều kim đồng hồ (CW).
 */
template <class Tp>
auto signed_area(const ds::polygon<Tp>& poly) {
    int n = static_cast<int>(poly.size());
    if (n < 3) return Tp(0);
    Tp sum(0);
    for (int i = 0; i < n; ++i) {
        int j = (i + 1 == n ? 0 : i + 1);
        sum += cross(poly[i].to_vector(), poly[j].to_vector());
    }
    return sum / Tp(2);
}

/**
 * @brief Diện tích đa giác không dấu: |signed_area(poly)|.
 */
template <class Tp>
auto area(const ds::polygon<Tp>& poly) {
    return math::abs(signed_area(poly));
}

/**
 * @brief Chu vi của đa giác.
 */
template <class Tp>
auto perimeter(const ds::polygon<Tp>& poly) {
    int n = static_cast<int>(poly.size());
    if (n < 2) return decltype((poly[0] - poly[1]).length())(0);
    auto perim = (poly[0] - poly[n - 1]).length();
    for (int i = 0; i < n - 1; ++i) {
        perim += (poly[i + 1] - poly[i]).length();
    }
    return perim;
}

/**
 * @brief Trọng tâm (center of mass) của đa giác phẳng không tự cắt.
 */
template <class Tp>
ds::point<Tp> centroid(const ds::polygon<Tp>& poly) {
    int n = static_cast<int>(poly.size());
    Tp cx(0), cy(0);

    for (int i = 0; i < n; ++i) {
        int j = (i + 1 == n ? 0 : i + 1);
        auto factor = cross(poly[i].to_vector(), poly[j].to_vector());
        cx += (poly[i].x + poly[j].x) * factor;
        cy += (poly[i].y + poly[j].y) * factor;
    }
    auto total = Tp(6) * signed_area(poly);
    return ds::point<Tp>(cx / total, cy / total);
}

/**
 * @brief Kiểm tra đa giác có phải là đa giác lồi hay không.
 * @param strictly Nếu true, yêu cầu lồi nghiêm ngặt (không có 3 đỉnh liên tiếp thẳng hàng).
 */
template <class Tp>
bool is_convex(const ds::polygon<Tp>& poly, bool strictly = true) {
    int n = static_cast<int>(poly.size());
    if (n < 3) return false;

    int pos = 0, neg = 0;
    for (int i = 0; i < n; ++i) {
        int o = orient(poly[i], poly[(i + 1) % n], poly[(i + 2) % n]);
        if (o > 0) pos++;
        if (o < 0) neg++;
        if (strictly && o == 0) return false;
    }
    return (pos == 0 || neg == 0);
}

/**
 * @brief Kiểm tra điểm p có nằm trong đa giác bất kỳ hay không bằng thuật toán Winding Number.
 * @param p Điểm cần kiểm tra.
 * @param poly Đa giác bất kỳ.
 * @return 0 nếu nằm ngoài, 1 nếu nằm trên biên/cạnh/đỉnh, 2 nếu nằm hẳn bên trong.
 * @complexity O(N) thời gian.
 */
template <class Tp>
int is_inside(const ds::point<Tp>& p, const ds::polygon<Tp>& poly) {
    int n = static_cast<int>(poly.size());
    if (n == 0) return 0;

    int wn = 0;
    for (int i = 0; i < n; ++i) {
        auto p1 = poly[i];
        auto p2 = poly[(i + 1) % n];

        if (is_on(p, ds::segment<Tp>(p1, p2))) {
            return 1;
        }

        int k = orient(p1, p2, p);
        int d1 = sgn(p1.y - p.y);
        int d2 = sgn(p2.y - p.y);

        if (k > 0 && d1 <= 0 && d2 > 0) wn++;
        if (k < 0 && d2 <= 0 && d1 > 0) wn--;
    }
    return wn != 0 ? 2 : 0;
}

/**
 * @brief Kiểm tra điểm p có nằm trong đa giác lồi trong thời gian O(log N) bằng tìm kiếm nhị phân trên góc.
 * @param p Điểm cần kiểm tra.
 * @param poly Đa giác lồi với các đỉnh xếp ngược chiều kim đồng hồ (CCW).
 * @return 0 nếu nằm ngoài, 1 nếu nằm trên biên, 2 nếu nằm hẳn bên trong.
 * @complexity O(log N) thời gian.
 */
template <class Tp>
int is_inside_convex(const ds::point<Tp>& p, const ds::polygon<Tp>& poly) {
    int n = static_cast<int>(poly.size());
    if (n < 3) return 0;

    int o1 = orient(poly[0], poly[1], p);
    int on = orient(poly[0], poly[n - 1], p);

    if (o1 < 0 || on > 0) return 0;
    if (o1 == 0) return is_on(p, ds::segment<Tp>(poly[0], poly[1])) ? 1 : 0;
    if (on == 0) return is_on(p, ds::segment<Tp>(poly[0], poly[n - 1])) ? 1 : 0;

    // Tìm kiếm nhị phân góc hình quạt chứa điểm p
    int l = 1, r = n - 1;
    while (r - l > 1) {
        int mid = (l + r) / 2;
        if (orient(poly[0], poly[mid], p) >= 0) {
            l = mid;
        } else {
            r = mid;
        }
    }

    // Điểm p nằm trong góc giữa cạnh poly[l] và poly[l + 1]
    int o_edge = orient(poly[l], poly[l + 1], p);
    if (o_edge < 0) return 0;
    if (o_edge == 0) return 1;
    return 2;
}

/**
 * @brief Cắt đa giác lồi bằng một đường thẳng có hướng, giữ lại phần nằm ở nửa mặt phẳng bên trái.
 * @param poly Đa giác lồi ban đầu (CCW).
 * @param l Đường thẳng định hướng.
 * @return ds::polygon<Tp> Đa giác lồi thu được sau khi cắt.
 * @complexity O(N) thời gian.
 */
template <class Tp>
ds::polygon<Tp> cut_polygon(const ds::polygon<Tp>& poly, const ds::line<Tp>& l) {
    ds::polygon<Tp> res;
    int n = static_cast<int>(poly.size());

    for (int i = 0; i < n; ++i) {
        auto cur = poly[i];
        auto next = poly[(i + 1) % n];

        int s_cur = side(cur, l);
        int s_next = side(next, l);

        if (s_cur >= 0) {
            res.push_back(cur);
        }
        if ((s_cur > 0 && s_next < 0) || (s_cur < 0 && s_next > 0)) {
            auto inter = intersect(l, ds::line<Tp>(cur, next));
            if (inter.is_point()) {
                res.push_back(inter.get_point());
            }
        }
    }
    return res;
}

/**
 * @brief Tìm đỉnh cực trị của đa giác lồi theo hướng vector `dir` (cực đại hóa tích vô hướng p . dir).
 * @param poly Đa giác lồi (CCW).
 * @param dir Vector chỉ hướng.
 * @return int Chỉ số (index) của đỉnh xa nhất theo hướng dir.
 * @complexity O(log N) thời gian.
 */
template <class Tp>
int extreme_vertex(const ds::polygon<Tp>& poly, const ds::vector<Tp>& dir) {
    int n = static_cast<int>(poly.size());
    auto dot_val = [&](int idx) {
        return dot(poly[idx].to_vector(), dir);
    };

    auto is_up = [&](int idx) {
        return dot_val((idx + 1) % n) >= dot_val(idx);
    };

    if (is_up(0) && !is_up(n - 1)) return 0;

    int l = 0, r = n;
    while (r - l > 1) {
        int mid = (l + r) / 2;
        if (is_up(mid)) {
            if (dot_val(mid) >= dot_val(0)) l = mid;
            else r = mid;
        } else {
            if (dot_val(mid) >= dot_val(0)) r = mid;
            else l = mid;
        }
    }
    return (l + 1) % n;
}

/**
 * @brief Tổng Minkowski của hai đa giác lồi P và Q.
 * @param P Đa giác lồi (CCW).
 * @param Q Đa giác lồi (CCW).
 * @return ds::polygon<Tp> Đa giác lồi tổng P + Q theo thứ tự CCW.
 * @complexity O(|P| + |Q|) thời gian.
 */
template <class Tp>
ds::polygon<Tp> minkowski_sum(ds::polygon<Tp> P, ds::polygon<Tp> Q) {
    if (P.empty()) return Q;
    if (Q.empty()) return P;

    auto reorder = [](ds::polygon<Tp>& poly) {
        auto it = std::min_element(poly.begin(), poly.end());
        std::rotate(poly.begin(), it, poly.end());
    };
    reorder(P);
    reorder(Q);

    int n = static_cast<int>(P.size());
    int m = static_cast<int>(Q.size());

    ds::polygon<Tp> res;
    res.reserve(n + m);

    int i = 0, j = 0;
    while (i < n || j < m) {
        res.push_back(P[i % n] + Q[j % m].to_vector());
        auto cross_p = cross(P[(i + 1) % n] - P[i % n], Q[(j + 1) % m] - Q[j % m]);
        if (cross_p >= 0 && i < n) ++i;
        if (cross_p <= 0 && j < m) ++j;
    }
    return res;
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_POLYGONS
