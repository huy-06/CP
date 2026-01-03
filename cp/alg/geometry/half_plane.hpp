#include <vector>
#include "lines.hpp"

#ifndef CP_ALG_GEO_HALF_PLANE_HPP
#define CP_ALG_GEO_HALF_PLANE_HPP

namespace cp {
namespace alg {

template <class Tp>
std::vector<ds::point<Tp>> half_plane(std::vector<ds::line<Tp>> lines) {
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