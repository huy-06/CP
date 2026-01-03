#include <tuple>
#include "lines.hpp"

#ifndef CP_ALG_GEOMETRY_INTERSECTION
#define CP_ALG_GEOMETRY_INTERSECTION

namespace cp {
namespace alg {

template <class Tp>
ds::point<Tp> line_intersect(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    return l1.a + (l1.b - l1.a) * (cross(l2.b - l2.a, l1.a - l2.a) / cross(l2.b - l2.a, l1.a - l1.b));
}

template <class Tp>
std::tuple<int, ds::point<Tp>, ds::point<Tp>> segment_intersect(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    if (std::max(l1.a.x, l1.b.x) < std::min(l2.a.x, l2.b.x)) return {0, {}, {}};
    if (std::min(l1.a.x, l1.b.x) > std::max(l2.a.x, l2.b.x)) return {0, {}, {}};
    if (std::max(l1.a.y, l1.b.y) < std::min(l2.a.y, l2.b.y)) return {0, {}, {}};
    if (std::min(l1.a.y, l1.b.y) > std::max(l2.a.y, l2.b.y)) return {0, {}, {}};

    if (cross(l1.b - l1.a, l2.b - l2.a) == 0) {
        if (cross(l1.b - l1.a, l2.a - l1.a) != 0)
            return {0, {}, {}};
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
                std::swap(p1.y, p2.y); // Logic swap này cần check kỹ lại logic gốc
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
        return {0, {}, {}};
    ds::point<Tp> p = line_intersect(l1, l2);
    if (cp1 != 0 && cp2 != 0 && cp3 != 0 && cp4 != 0)
        return {1, p, p};
    else
        return {3, p, p};
}

} // namespace alg
} // namespace cp
#endif