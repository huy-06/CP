#include <random>
#include "distance.hpp"
#include "../../ds/geo/circle.hpp"
#include "../../ds/geo/polygon.hpp"

#ifndef CP_ALG_GEO_CIRCLES
#define CP_ALG_GEO_CIRCLES

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Tìm giao điểm giữa đường tròn `cir` và đường thẳng `l`.
 * @return std::vector<ds::point<Tp>> Danh sách chứa 0, 1 hoặc 2 tọa độ giao điểm.
 */
template <class Tp>
std::vector<ds::point<Tp>> intersect(const ds::circle<Tp>& cir, const ds::line<Tp>& l) {
    auto proj = project(cir.c, l);
    auto d2 = (proj - cir.c).square();
    auto r2 = cir.r * cir.r;

    if (d2 > r2) return {};
    if (d2 == r2) return { proj };

    auto h = math::sqrt(r2 - d2);
    auto u = l.v.unit();
    return {
        proj + u * h,
        proj - u * h
    };
}

/**
 * @brief Tìm giao điểm giữa hai đường tròn c1 và c2.
 * @return std::vector<ds::point<Tp>> Danh sách chứa 0, 1 hoặc 2 tọa độ giao điểm.
 * @note Trả về rỗng nếu hai đường tròn đồng tâm hoặc rời nhau / chứa trong nhau không chạm.
 */
template <class Tp>
std::vector<ds::point<Tp>> intersect(const ds::circle<Tp>& c1, const ds::circle<Tp>& c2) {
    auto d_vec = c2.c - c1.c;
    auto d2 = d_vec.square();
    auto d = math::sqrt(d2);

    auto r1 = c1.r;
    auto r2 = c2.r;

    if (d == Tp(0)) return {}; // Đồng tâm
    if (d > r1 + r2 || d < math::abs(r1 - r2)) return {};

    auto a = (r1 * r1 - r2 * r2 + d2) / (Tp(2) * d);
    auto h2 = r1 * r1 - a * a;
    auto h = math::sqrt(h2 > Tp(0) ? h2 : Tp(0));

    auto u = d_vec / d;
    auto p0 = c1.c + u * a;
    auto n = u.rotate90();

    if (h == Tp(0)) return { p0 };

    return {
        p0 + n * h,
        p0 - n * h
    };
}

/**
 * @brief Tìm các tiếp điểm từ một điểm ngoài `p` đến đường tròn `cir`.
 * @return std::vector<ds::point<Tp>> Danh sách chứa 0 (nằm trong), 1 (nằm trên đường tròn), hoặc 2 tiếp điểm.
 */
template <class Tp>
std::vector<ds::point<Tp>> tangents(const ds::point<Tp>& p, const ds::circle<Tp>& cir) {
    auto v = p - cir.c;
    auto d2 = v.square();
    auto r2 = cir.r * cir.r;

    if (d2 < r2) return {};
    if (d2 == r2) return { p };

    auto d = math::sqrt(d2);
    auto base_ang = math::atan2(v.y, v.x);
    auto delta_ang = math::asin(cir.r / d);
    auto half_pi = static_cast<Tp>(cst::pi) / Tp(2);

    return {
        cir.eval(base_ang + delta_ang - half_pi),
        cir.eval(base_ang - delta_ang + half_pi)
    };
}

/**
 * @brief Tìm các đường tiếp tuyến chung của hai đường tròn c1 và c2.
 * @return std::vector<ds::line<Tp>> Danh sách chứa tối đa 4 đường tiếp tuyến chung (ngoài và trong).
 */
template <class Tp>
std::vector<ds::line<Tp>> common_tangents(const ds::circle<Tp>& c1, const ds::circle<Tp>& c2) {
    std::vector<ds::line<Tp>> res;
    auto r1 = c1.r;
    auto r2 = c2.r;
    auto d_vec = c2.c - c1.c;
    auto d2 = d_vec.square();
    auto d = math::sqrt(d2);

    if (d == Tp(0)) return {};

    for (int sign : {1, -1}) {
        auto dr = r1 - Tp(sign) * r2;
        if (d2 < dr * dr) continue;
        auto base_ang = math::atan2(d_vec.y, d_vec.x);
        auto cos_val = dr / d;
        if (cos_val > Tp(1)) cos_val = Tp(1);
        if (cos_val < -Tp(1)) cos_val = -Tp(1);
        auto delta = (d2 >= dr * dr) ? math::acos(cos_val) : Tp(0);

        for (auto ang : {base_ang + delta, base_ang - delta}) {
            ds::vector<Tp> n(math::cos(ang), math::sin(ang));
            auto p1 = c1.c + n * r1;
            auto p2 = c2.c + n * (Tp(sign) * r2);
            res.emplace_back(p1, p2 - p1);
            if (delta == Tp(0)) break;
        }
    }
    return res;
}

/**
 * @brief Tính diện tích phần giao nhau giữa hai hình tròn c1 và c2.
 */
template <class Tp>
auto intersection_area(const ds::circle<Tp>& c1, const ds::circle<Tp>& c2) {
    auto r1 = c1.r;
    auto r2 = c2.r;
    auto d = (c1.c - c2.c).length();

    if (d >= r1 + r2) return Tp(0);
    if (d <= math::abs(r1 - r2)) {
        auto r_min = std::min(r1, r2);
        return static_cast<Tp>(cst::pi) * r_min * r_min;
    }

    auto cos_alpha = (r1 * r1 + d * d - r2 * r2) / (Tp(2) * r1 * d);
    auto cos_beta = (r2 * r2 + d * d - r1 * r1) / (Tp(2) * r2 * d);
    if (cos_alpha > Tp(1)) cos_alpha = Tp(1); if (cos_alpha < -Tp(1)) cos_alpha = -Tp(1);
    if (cos_beta > Tp(1)) cos_beta = Tp(1); if (cos_beta < -Tp(1)) cos_beta = -Tp(1);

    auto alpha = Tp(2) * math::acos(cos_alpha);
    auto beta = Tp(2) * math::acos(cos_beta);

    auto a1 = (r1 * r1 * (alpha - math::sin(alpha))) / Tp(2);
    auto a2 = (r2 * r2 * (beta - math::sin(beta))) / Tp(2);
    return a1 + a2;
}

/**
 * @brief Tính diện tích có dấu của phần giao giữa hình tròn tâm O bán kính r và tam giác (O, A, B).
 */
namespace detail {
template <class Tp>
Tp circle_triangle_area(const Tp& r, ds::point<Tp> a, ds::point<Tp> b) {
    auto da = a.to_vector().length();
    auto db = b.to_vector().length();
    if (da <= r && db <= r) {
        return cross(a.to_vector(), b.to_vector()) / Tp(2);
    }
    ds::line<Tp> l(a, b - a);
    ds::circle<Tp> cir(Tp(0), Tp(0), r);
    auto pts = intersect(cir, l);

    if (pts.size() <= 1) {
        auto ang = math::atan2(cross(a.to_vector(), b.to_vector()), dot(a.to_vector(), b.to_vector()));
        return r * r * ang / Tp(2);
    }

    auto p1 = pts[0], p2 = pts[1];
    if (dot(p2 - p1, b - a) < Tp(0)) std::swap(p1, p2);

    bool in1 = (dot(p1 - a, b - a) > Tp(0) && dot(p1 - b, a - b) > Tp(0));
    bool in2 = (dot(p2 - a, b - a) > Tp(0) && dot(p2 - b, a - b) > Tp(0));

    if (in1 && in2) {
        return circle_triangle_area(r, a, p1) + circle_triangle_area(r, p1, p2) + circle_triangle_area(r, p2, b);
    } else if (in1) {
        return circle_triangle_area(r, a, p1) + circle_triangle_area(r, p1, b);
    } else if (in2) {
        return circle_triangle_area(r, a, p2) + circle_triangle_area(r, p2, b);
    } else {
        auto ang = math::atan2(cross(a.to_vector(), b.to_vector()), dot(a.to_vector(), b.to_vector()));
        return r * r * ang / Tp(2);
    }
}
} // namespace detail

/**
 * @brief Tính diện tích phần giao nhau giữa hình tròn và một đa giác bất kỳ.
 * @complexity O(N) với N là số đỉnh của đa giác.
 */
template <class Tp>
auto intersection_area(const ds::circle<Tp>& cir, const ds::polygon<Tp>& poly) {
    Tp res(0);
    int n = static_cast<int>(poly.size());
    if (n < 3) return res;

    for (int i = 0; i < n; ++i) {
        ds::point<Tp> a = poly[i] - cir.c.to_vector();
        ds::point<Tp> b = poly[(i + 1) % n] - cir.c.to_vector();
        res += detail::circle_triangle_area(cir.r, a, b);
    }
    return math::abs(res);
}

/**
 * @brief Đường tròn ngoại tiếp tam giác đi qua 3 điểm a, b, c không thẳng hàng.
 */
template <class Tp>
ds::circle<Tp> circumcircle(const ds::point<Tp>& a, const ds::point<Tp>& b, const ds::point<Tp>& c) {
    auto b1 = perpendicular_bisector(ds::segment<Tp>(a, b));
    auto b2 = perpendicular_bisector(ds::segment<Tp>(b, c));
    auto inter = intersect(b1, b2);
    auto center = inter.get_point();
    auto r = (center - a).length();
    return ds::circle<Tp>(center, r);
}

/**
 * @brief Đường tròn nội tiếp tam giác tiếp xúc với 3 cạnh tam giác (a, b, c).
 */
template <class Tp>
ds::circle<Tp> incircle(const ds::point<Tp>& a, const ds::point<Tp>& b, const ds::point<Tp>& c) {
    auto side_a = (b - c).length();
    auto side_b = (a - c).length();
    auto side_c = (a - b).length();
    auto p = side_a + side_b + side_c;

    ds::point<Tp> center(
        (side_a * a.x + side_b * b.x + side_c * c.x) / p,
        (side_a * a.y + side_b * b.y + side_c * c.y) / p
    );
    auto area = math::abs(cross(b - a, c - a)) / Tp(2);
    auto r = Tp(2) * area / p;
    return ds::circle<Tp>(center, r);
}

/**
 * @brief Tìm Đường tròn bao nhỏ nhất (Minimum Enclosing Circle - MEC) chứa tất cả các điểm bằng thuật toán Welzl.
 * @complexity Thời gian kỳ vọng O(N).
 */
template <class Tp>
ds::circle<Tp> min_enclosing_circle(std::vector<ds::point<Tp>> pts) {
    std::mt19937 rng(1337);
    std::shuffle(pts.begin(), pts.end(), rng);

    auto make_circle_2pts = [](const ds::point<Tp>& a, const ds::point<Tp>& b) {
        ds::point<Tp> center((a.x + b.x) / Tp(2), (a.y + b.y) / Tp(2));
        auto r = (center - a).length();
        return ds::circle<Tp>(center, r);
    };

    int n = static_cast<int>(pts.size());
    if (n == 0) return ds::circle<Tp>();
    if (n == 1) return ds::circle<Tp>(pts[0], Tp(0));

    ds::circle<Tp> c = make_circle_2pts(pts[0], pts[1]);

    for (int i = 2; i < n; ++i) {
        if (!c.contains(pts[i])) {
            c = ds::circle<Tp>(pts[i], Tp(0));
            for (int j = 0; j < i; ++j) {
                if (!c.contains(pts[j])) {
                    c = make_circle_2pts(pts[i], pts[j]);
                    for (int k = 0; k < j; ++k) {
                        if (!c.contains(pts[k])) {
                            c = circumcircle(pts[i], pts[j], pts[k]);
                        }
                    }
                }
            }
        }
    }
    return c;
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_CIRCLES
