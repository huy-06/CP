
#include <algorithm>
#include "../../ds/geometry/vector.hpp"

#ifndef CP_ALG_GEOMETRY_VECTORS
#define CP_ALG_GEOMETRY_VECTORS
namespace cp {
namespace alg {

template <class Tp>
Tp dot(const ds::vector<Tp>& a, const ds::vector<Tp>& b) {
    return a.x * b.x + a.y * b.y;
}

template <class Tp>
Tp cross(const ds::vector<Tp>& a, const ds::vector<Tp>& b) {
    return a.x * b.y - a.y * b.x;
}

template <class Tp>
Tp square(const ds::vector<Tp>& p) {
    return dot(p, p);
}

template <class Tp>
Tp length(const ds::vector<Tp>& p) {
    return std::sqrt(square(p));
}

template <class Tp>
ds::vector<Tp> normalize(const ds::vector<Tp>& p) {
    return p / length(p);
}

template <class Tp>
ds::vector<Tp> rotate(const ds::vector<Tp>& a) {
    return ds::vector<Tp>(-a.y, a.x);
}

template <class Tp>
int sign(const ds::vector<Tp>& a) {
    return (a.y > 0 || (a.y == 0 && a.x > 0)) ? 1 : -1;
}

} // namespace alg
} // namespace cp
#endif