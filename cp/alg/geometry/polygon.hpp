#include <vector>
#include "lines.hpp"

#ifndef CP_ALG_GEOMETRY_POLYGON
#define CP_ALG_GEOMETRY_POLYGON

namespace cp {
namespace alg {

template <class Tp>
int in_polygon(const ds::point<Tp>& a, const std::vector<ds::point<Tp>>& p) {
    int n = p.size();
    for (int i = 0; i < n; ++i) {
        if (on_segment(a, ds::line<Tp>(p[i], p[(i + 1) % n])))
            return 2;
    }
    int t = 0;
    for (int i = 0; i < n; ++i) {
        auto u = p[i], v = p[(i + 1) % n];
        if (u.x < a.x && v.x >= a.x && is_left(a, ds::line<Tp>(v, u)))
            t ^= 1;
        if (u.x >= a.x && v.x < a.x && is_left(a, ds::line<Tp>(u, v)))
            t ^= 1;
    }
    return t == 1 ? 1 : 0;
}

template <class Tp>
long double polygon_area(const std::vector<ds::point<Tp>>& p) {
    long long ans = 0;
    int n = p.size();
    for (int i = 0; i < n; ++i) {
        auto u = p[i];
        auto v = p[(i + 1) % n];
        ans += cross(u, v);
    }
    return static_cast<long double>(std::abs(ans)) / 2.0l;
}

template <class Tp>
bool in_polygon(const ds::line<Tp>& l, const std::vector<ds::point<Tp>>& p) {
    int n = p.size();
    if (!in_polygon(l.a, p)) return false;
    if (!in_polygon(l.b, p)) return false;
    for (int i = 0; i < n; ++i) {
        auto u = p[i];
        auto v = p[(i + 1) % n];
        auto w = p[(i + 2) % n];
        int t;
        ds::point<Tp> p1, p2;
        std::tie(t, p1, p2) = segment_intersect(l, ds::line<Tp>(u, v));
        if (t == 1) return false;
        if (t == 0) continue;
        if (t == 2) {
            if (on_segment(v, l) && v != l.a && v != l.b) {
                if (cross(v - u, w - v) > 0)
                    return false;
            }
        } else {
            if (p1 != u && p1 != v) {
                if (is_left(l.a, ds::line<Tp>(v, u)) ||
                    is_left(l.b, ds::line<Tp>(v, u)))
                    return false;
            } else if (p1 == v) {
                if (l.a == v) {
                    if (is_left(u, l)) {
                        if (is_left(w, l) && is_left(w, ds::line<Tp>(u, v)))
                            return false;
                    } else {
                        if (is_left(w, l) || is_left(w, ds::line<Tp>(u, v)))
                            return false;
                    }
                } else if (l.b == v) {
                    if (is_left(u, ds::line<Tp>(l.b, l.a))) {
                        if (is_left(w, ds::line<Tp>(l.b, l.a)) && is_left(w, ds::line<Tp>(u, v)))
                            return false;
                    } else {
                        if (is_left(w, ds::line<Tp>(l.b, l.a)) || is_left(w, ds::line<Tp>(u, v)))
                            return false;
                    }
                } else {
                    if (is_left(u, l)) {
                        if (is_left(w, ds::line<Tp>(l.b, l.a)) || is_left(w, ds::line<Tp>(u, v)))
                            return false;
                    } else {
                        if (is_left(w, l) || is_left(w, ds::line<Tp>(u, v)))
                            return false;
                    }
                }
            }
        }
    }
    return true;
}

} // namespace alg
} // namespace cp
#endif