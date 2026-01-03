
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include "../../ds/geo/line.hpp"

#ifndef CP_ALG_GEO
#define CP_ALG_GEO
namespace cp {
namespace alg {

template <class Tp>
Tp dot(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return a.x * b.x + a.y * b.y;
}

template <class Tp>
Tp cross(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return a.x * b.y - a.y * b.x;
}

template <class Tp>
Tp square(const ds::point<Tp>& p) {
    return dot(p, p);
}


template <class Tp>
long double length(const ds::point<Tp>& p) {
    return std::sqrt(square(p));
}


template <class Tp>
long double length(const ds::line<Tp>& l) {
    return length(l.a - l.b);
}


template <class Tp>
ds::point<Tp> normalize(const ds::point<Tp>& p) {
    return p / length(p);
}


template <class Tp>
bool parallel(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    return cross(l1.b - l1.a, l2.b - l2.a) == 0;
}

namespace dist {
    
template <class Tp>
long double point_to_point(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return length(a - b);
}

template <class Tp>
long double point_to_line(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return std::abs(cross(l.a - l.b, l.a - p)) / length(l);
}


template <class Tp>
long double point_to_segment(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    if (dot(p - l.a, l.b - l.a) < 0)
        return point_to_point(p, l.a);
    if (dot(p - l.b, l.a - l.b) < 0)
        return point_to_point(p, l.b);
    return point_to_line(p, l);
}

} // namespace dist

template <class Tp>
ds::point<Tp> rotate(const ds::point<Tp>& a) {
    return ds::point<Tp>(-a.y, a.x);
}

template <class Tp>
int sgn(const ds::point<Tp>& a) {
    return (a.y > 0 || (a.y == 0 && a.x > 0)) ? 1 : -1;
}

template <class Tp>
bool point_on_left_line(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return cross(l.b - l.a, p - l.a) > 0;
}


template <class Tp>
ds::point<Tp> line_itersection(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    return l1.a + (l1.b - l1.a) * (cross(l2.b - l2.a, l1.a - l2.a) / cross(l2.b - l2.a, l1.a - l1.b));
}


template <class Tp>
bool point_on_segment(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return cross(p - l.a, l.b - l.a) == 0 &&
           std::min(l.a.x, l.b.x) <= p.x && p.x <= std::max(l.a.x, l.b.x) &&
           std::min(l.a.y, l.b.y) <= p.y && p.y <= std::max(l.a.y, l.b.y);
}


template <class Tp>
int point_in_polygon(const ds::point<Tp>& a, const std::vector<ds::point<Tp>>& p) {
    int n = p.size();
    for (int i = 0; i < n; ++i) {
        if (point_on_segment(a, ds::line<Tp>(p[i], p[(i + 1) % n])))
            return 2;
    }
    int t = 0;
    for (int i = 0; i < n; ++i) {
        auto u = p[i], v = p[(i + 1) % n];
        if (u.x < a.x && v.x >= a.x && point_on_left_line(a, ds::line<Tp>(v, u)))
            t ^= 1;
        if (u.x >= a.x && v.x < a.x && point_on_left_line(a, ds::line<Tp>(u, v)))
            t ^= 1;
    }
    return t == 1 ? 1 : 0;
}

template <class Tp>
std::tuple<int, ds::point<Tp>, ds::point<Tp>> segment_intersection(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    if (std::max(l1.a.x, l1.b.x) < std::min(l2.a.x, l2.b.x)) return {0, ds::point<Tp>(), ds::point<Tp>()};
    if (std::min(l1.a.x, l1.b.x) > std::max(l2.a.x, l2.b.x)) return {0, ds::point<Tp>(), ds::point<Tp>()};
    if (std::max(l1.a.y, l1.b.y) < std::min(l2.a.y, l2.b.y)) return {0, ds::point<Tp>(), ds::point<Tp>()};
    if (std::min(l1.a.y, l1.b.y) > std::max(l2.a.y, l2.b.y)) return {0, ds::point<Tp>(), ds::point<Tp>()};

    if (cross(l1.b - l1.a, l2.b - l2.a) == 0) {
        if (cross(l1.b - l1.a, l2.a - l1.a) != 0)
            return {0, ds::point<Tp>(), ds::point<Tp>()};
        else {
            auto maxx1 = std::max(l1.a.x, l1.b.x);
            auto minx1 = std::min(l1.a.x, l1.b.x);
            auto maxy1 = std::max(l1.a.y, l1.b.y);
            auto miny1 = std::min(l1.a.y, l1.b.y);
            auto maxx2 = std::max(l2.a.x, l2.b.x);
            auto minx2 = std::min(l2.a.x, l2.b.x);
            auto maxy2 = std::max(l2.a.y, l2.b.y);
            auto miny2 = std::min(l2.a.y, l2.b.y);
            ds::point<Tp> p1(std::max(minx1, minx2), std::max(miny1, miny2));
            ds::point<Tp> p2(std::min(maxx1, maxx2), std::min(maxy1, maxy2));
            if (!point_on_segment(p1, l1))
                std::swap(p1.y, p2.y);
            if (p1 == p2)
                return {3, p1, p2};
            else
                return {2, p1, p2};
        }
    }
    auto cp1 = cross(l2.a - l1.a, l2.b - l1.a);
    auto cp2 = cross(l2.a - l1.b, l2.b - l1.b);
    auto cp3 = cross(l1.a - l2.a, l1.b - l2.a);
    auto cp4 = cross(l1.a - l2.b, l1.b - l2.b);
    if ((cp1 > 0 && cp2 > 0) || (cp1 < 0 && cp2 < 0) ||
        (cp3 > 0 && cp4 > 0) || (cp3 < 0 && cp4 < 0))
        return {0, ds::point<Tp>(), ds::point<Tp>()};
    ds::point<Tp> p = line_itersection(l1, l2);
    if (cp1 != 0 && cp2 != 0 && cp3 != 0 && cp4 != 0)
        return {1, p, p};
    else
        return {3, p, p};
}

namespace dist {

template <class Tp>
long double segment_to_segment(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    if (std::get<0>(segment_intersection(l1, l2)) != 0)
        return 0.0;
    return std::min({dist::point_to_segment(l1.a, l2), dist::point_to_segment(l1.b, l2),
                     dist::point_to_segment(l2.a, l1), dist::point_to_segment(l2.b, l1)});
}

} // namespace dist

template <class Tp>
long double area_of_polygon(const std::vector<ds::point<Tp>>& p) {
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
bool segment_in_polygon(const ds::line<Tp>& l, const std::vector<ds::point<Tp>>& p) {
    int n = p.size();
    if (!point_in_polygon(l.a, p)) return false;
    if (!point_in_polygon(l.b, p)) return false;
    for (int i = 0; i < n; ++i) {
        auto u = p[i];
        auto v = p[(i + 1) % n];
        auto w = p[(i + 2) % n];
        int t;
        ds::point<Tp> p1, p2;
        std::tie(t, p1, p2) = segment_intersection(l, ds::line<Tp>(u, v));
        if (t == 1) return false;
        if (t == 0) continue;
        if (t == 2) {
            if (point_on_segment(v, l) && v != l.a && v != l.b) {
                if (cross(v - u, w - v) > 0)
                    return false;
            }
        } else {
            if (p1 != u && p1 != v) {
                if (point_on_left_line(l.a, ds::line<Tp>(v, u)) ||
                    point_on_left_line(l.b, ds::line<Tp>(v, u)))
                    return false;
            } else if (p1 == v) {
                if (l.a == v) {
                    if (point_on_left_line(u, l)) {
                        if (point_on_left_line(w, l) && point_on_left_line(w, ds::line<Tp>(u, v)))
                            return false;
                    } else {
                        if (point_on_left_line(w, l) || point_on_left_line(w, ds::line<Tp>(u, v)))
                            return false;
                    }
                } else if (l.b == v) {
                    if (point_on_left_line(u, ds::line<Tp>(l.b, l.a))) {
                        if (point_on_left_line(w, ds::line<Tp>(l.b, l.a)) && point_on_left_line(w, ds::line<Tp>(u, v)))
                            return false;
                    } else {
                        if (point_on_left_line(w, ds::line<Tp>(l.b, l.a)) || point_on_left_line(w, ds::line<Tp>(u, v)))
                            return false;
                    }
                } else {
                    if (point_on_left_line(u, l)) {
                        if (point_on_left_line(w, ds::line<Tp>(l.b, l.a)) || point_on_left_line(w, ds::line<Tp>(u, v)))
                            return false;
                    } else {
                        if (point_on_left_line(w, l) || point_on_left_line(w, ds::line<Tp>(u, v)))
                            return false;
                    }
                }
            }
        }
    }
    return true;
}

template <class Tp>
std::vector<ds::point<Tp>> hp(std::vector<ds::line<Tp>> lines) {
    std::sort(lines.begin(), lines.end(), [&](const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
        auto d1 = l1.b - l1.a;
        auto d2 = l2.b - l2.a;
        if (sgn(d1) != sgn(d2))
            return sgn(d1) == 1;
        return cross(d1, d2) > 0;
    });
    
    std::deque<ds::line<Tp>>  ls;
    std::deque<ds::point<Tp>> ps;
    for (auto l : lines) {
        if (ls.empty()) {
            ls.push_back(l);
            continue;
        }
        while (!ps.empty() && !point_on_left_line(ps.back(), l)) {
            ps.pop_back();
            ls.pop_back();
        }
        while (!ps.empty() && !point_on_left_line(ps.front(), l)) {
            ps.pop_front();
            ls.pop_front();
        }
        if (cross(l.b - l.a, ls.back().b - ls.back().a) == 0) {
            if (dot(l.b - l.a, ls.back().b - ls.back().a) > 0) {
                if (!point_on_left_line(ls.back().a, l)) {
                    assert(ls.size() == 1);
                    ls[0] = l;
                }
                continue;
            }
            return {};
        }
        ps.push_back(line_itersection(ls.back(), l));
        ls.push_back(l);
    }
    while (!ps.empty() && !point_on_left_line(ps.back(), ls.front())) {
        ps.pop_back();
        ls.pop_back();
    }
    if (ls.size() <= 2)
        return {};
    ps.push_back(line_itersection(ls.front(), ls.back()));
    return std::vector<ds::point<Tp>>(ps.begin(), ps.end());
}
    
} // namespace alg
} // namespace cp
#endif