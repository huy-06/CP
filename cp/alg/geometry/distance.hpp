#include "intersection.hpp" 

#ifndef CP_ALG_GEOMETRY_DISTANCE
#define CP_ALG_GEOMETRY_DISTANCE

namespace cp {
namespace alg {
    
template <class Tp>
long double distance(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return length(a - b);
}

template <class Tp>
long double dist_to_line(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return std::abs(cross(l.a - l.b, l.a - p)) / length(l);
}

template <class Tp>
long double dist_to_segment(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    if (alg::dot(p - l.a, l.b - l.a) < 0)
        return distance(p, l.a);
    if (alg::dot(p - l.b, l.a - l.b) < 0)
        return distance(p, l.b);
    return dist_to_line(p, l);
}

template <class Tp>
long double dist_segments(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    if (std::get<0>(alg::segment_intersection(l1, l2)) != 0)
        return 0.0;
    return std::min({dist_to_segment(l1.a, l2), dist_to_segment(l1.b, l2),
                     dist_to_segment(l2.a, l1), dist_to_segment(l2.b, l1)});
}

} // namespace alg
} // namespace cp
#endif