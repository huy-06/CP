
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include "../../ds/geo/line.hpp"

#ifndef HUY_ALG_GEO
#define HUY_ALG_GEO
namespace huy {
namespace alg {

template<class T>
T dot(const ds::point<T>& a, const ds::point<T>& b) {
    return a.x * b.x + a.y * b.y;
}

template<class T>
T cross(const ds::point<T>& a, const ds::point<T>& b) {
    return a.x * b.y - a.y * b.x;
}

template<class T>
T square(const ds::point<T>& p) {
    return dot(p, p);
}


template<class T>
long double length(const ds::point<T>& p) {
    return std::sqrt(square(p));
}


template<class T>
long double length(const ds::line<T>& l) {
    return length(l.a - l.b);
}


template<class T>
ds::point<T> normalize(const ds::point<T>& p) {
    return p / length(p);
}


template<class T>
bool parallel(const ds::line<T>& l1, const ds::line<T>& l2) {
    return cross(l1.b - l1.a, l2.b - l2.a) == 0;
}

namespace dist {
    
template<class T>
long double point_to_point(const ds::point<T>& a, const ds::point<T>& b) {
    return length(a - b);
}

template<class T>
long double point_to_line(const ds::point<T>& p, const ds::line<T>& l) {
    return std::abs(cross(l.a - l.b, l.a - p)) / length(l);
}


template<class T>
long double point_to_segment(const ds::point<T>& p, const ds::line<T>& l) {
    if (dot(p - l.a, l.b - l.a) < 0)
        return point_to_point(p, l.a);
    if (dot(p - l.b, l.a - l.b) < 0)
        return point_to_point(p, l.b);
    return point_to_line(p, l);
}

} // namespace dist

template<class T>
ds::point<T> rotate(const ds::point<T>& a) {
    return ds::point<T>(-a.y, a.x);
}

template<class T>
int sgn(const ds::point<T>& a) {
    return (a.y > 0 || (a.y == 0 && a.x > 0)) ? 1 : -1;
}

template<class T>
bool point_on_left_line(const ds::point<T>& p, const ds::line<T>& l) {
    return cross(l.b - l.a, p - l.a) > 0;
}


template<class T>
ds::point<T> line_itersection(const ds::line<T>& l1, const ds::line<T>& l2) {
    return l1.a + (l1.b - l1.a) * (cross(l2.b - l2.a, l1.a - l2.a) / cross(l2.b - l2.a, l1.a - l1.b));
}


template<class T>
bool point_on_segment(const ds::point<T>& p, const ds::line<T>& l) {
    return cross(p - l.a, l.b - l.a) == 0 &&
           std::min(l.a.x, l.b.x) <= p.x && p.x <= std::max(l.a.x, l.b.x) &&
           std::min(l.a.y, l.b.y) <= p.y && p.y <= std::max(l.a.y, l.b.y);
}


template<class T>
int point_in_polygon(const ds::point<T>& a, const std::vector<ds::point<T>>& p) {
    int n = p.size();
    for (int i = 0; i < n; ++i) {
        if (point_on_segment(a, ds::line<T>(p[i], p[(i + 1) % n])))
            return 2;
    }
    int t = 0;
    for (int i = 0; i < n; ++i) {
        auto u = p[i], v = p[(i + 1) % n];
        if (u.x < a.x && v.x >= a.x && point_on_left_line(a, ds::line<T>(v, u)))
            t ^= 1;
        if (u.x >= a.x && v.x < a.x && point_on_left_line(a, ds::line<T>(u, v)))
            t ^= 1;
    }
    return t == 1 ? 1 : 0;
}

template<class T>
std::tuple<int, ds::point<T>, ds::point<T>> segment_intersection(const ds::line<T>& l1, const ds::line<T>& l2) {
    if (std::max(l1.a.x, l1.b.x) < std::min(l2.a.x, l2.b.x)) return {0, ds::point<T>(), ds::point<T>()};
    if (std::min(l1.a.x, l1.b.x) > std::max(l2.a.x, l2.b.x)) return {0, ds::point<T>(), ds::point<T>()};
    if (std::max(l1.a.y, l1.b.y) < std::min(l2.a.y, l2.b.y)) return {0, ds::point<T>(), ds::point<T>()};
    if (std::min(l1.a.y, l1.b.y) > std::max(l2.a.y, l2.b.y)) return {0, ds::point<T>(), ds::point<T>()};

    if (cross(l1.b - l1.a, l2.b - l2.a) == 0) {
        if (cross(l1.b - l1.a, l2.a - l1.a) != 0)
            return {0, ds::point<T>(), ds::point<T>()};
        else {
            auto maxx1 = std::max(l1.a.x, l1.b.x);
            auto minx1 = std::min(l1.a.x, l1.b.x);
            auto maxy1 = std::max(l1.a.y, l1.b.y);
            auto miny1 = std::min(l1.a.y, l1.b.y);
            auto maxx2 = std::max(l2.a.x, l2.b.x);
            auto minx2 = std::min(l2.a.x, l2.b.x);
            auto maxy2 = std::max(l2.a.y, l2.b.y);
            auto miny2 = std::min(l2.a.y, l2.b.y);
            ds::point<T> p1(std::max(minx1, minx2), std::max(miny1, miny2));
            ds::point<T> p2(std::min(maxx1, maxx2), std::min(maxy1, maxy2));
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
        return {0, ds::point<T>(), ds::point<T>()};
    ds::point<T> p = line_itersection(l1, l2);
    if (cp1 != 0 && cp2 != 0 && cp3 != 0 && cp4 != 0)
        return {1, p, p};
    else
        return {3, p, p};
}

namespace dist {

template<class T>
long double segment_to_segment(const ds::line<T>& l1, const ds::line<T>& l2) {
    if (std::get<0>(segment_intersection(l1, l2)) != 0)
        return 0.0;
    return std::min({dist::point_to_segment(l1.a, l2), dist::point_to_segment(l1.b, l2),
                     dist::point_to_segment(l2.a, l1), dist::point_to_segment(l2.b, l1)});
}

} // namespace dist

template<class T>
long double area_of_polygon(const std::vector<ds::point<T>>& p) {
    long long ans = 0;
    int n = p.size();
    for (int i = 0; i < n; ++i) {
        auto u = p[i];
        auto v = p[(i + 1) % n];
        ans += cross(u, v);
    }
    return static_cast<long double>(std::abs(ans)) / 2.0;
}

template<class T>
bool segment_in_polygon(const ds::line<T>& l, const std::vector<ds::point<T>>& p) {
    int n = p.size();
    if (!point_in_polygon(l.a, p)) return false;
    if (!point_in_polygon(l.b, p)) return false;
    for (int i = 0; i < n; ++i) {
        auto u = p[i];
        auto v = p[(i + 1) % n];
        auto w = p[(i + 2) % n];
        auto [t, p1, p2] = segment_intersection(l, ds::line<T>(u, v));
        if (t == 1) return false;
        if (t == 0) continue;
        if (t == 2) {
            if (point_on_segment(v, l) && v != l.a && v != l.b) {
                if (cross(v - u, w - v) > 0)
                    return false;
            }
        } else {
            if (p1 != u && p1 != v) {
                if (point_on_left_line(l.a, ds::line<T>(v, u)) ||
                    point_on_left_line(l.b, ds::line<T>(v, u)))
                    return false;
            } else if (p1 == v) {
                if (l.a == v) {
                    if (point_on_left_line(u, l)) {
                        if (point_on_left_line(w, l) && point_on_left_line(w, ds::line<T>(u, v)))
                            return false;
                    } else {
                        if (point_on_left_line(w, l) || point_on_left_line(w, ds::line<T>(u, v)))
                            return false;
                    }
                } else if (l.b == v) {
                    if (point_on_left_line(u, ds::line<T>(l.b, l.a))) {
                        if (point_on_left_line(w, ds::line<T>(l.b, l.a)) && point_on_left_line(w, ds::line<T>(u, v)))
                            return false;
                    } else {
                        if (point_on_left_line(w, ds::line<T>(l.b, l.a)) || point_on_left_line(w, ds::line<T>(u, v)))
                            return false;
                    }
                } else {
                    if (point_on_left_line(u, l)) {
                        if (point_on_left_line(w, ds::line<T>(l.b, l.a)) || point_on_left_line(w, ds::line<T>(u, v)))
                            return false;
                    } else {
                        if (point_on_left_line(w, l) || point_on_left_line(w, ds::line<T>(u, v)))
                            return false;
                    }
                }
            }
        }
    }
    return true;
}

template<class T>
std::vector<ds::point<T>> hp(std::vector<ds::line<T>> lines) {
    std::sort(lines.begin(), lines.end(), [&](const ds::line<T>& l1, const ds::line<T>& l2) {
        auto d1 = l1.b - l1.a;
        auto d2 = l2.b - l2.a;
        if (sgn(d1) != sgn(d2))
            return sgn(d1) == 1;
        return cross(d1, d2) > 0;
    });
    
    std::deque<ds::line<T>> ls;
    std::deque<ds::point<T>> ps;
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
    return std::vector<ds::point<T>>(ps.begin(), ps.end());
}
    
} // namespace alg
} // namespace huy
#endif