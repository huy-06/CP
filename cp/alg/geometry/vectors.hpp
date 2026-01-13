
#include <algorithm>
#include "../../cst/real.hpp"
#include "../../ds/geometry/vector.hpp"
#include "../../alg/math/arithmetic/arithmetic.hpp"

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
    return sqrt(square(p));
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

template <class Tp>
Tp angle(const ds::vector<Tp>& a, const ds::vector<Tp>& b) {
    return atan2(cross(a, b), dot(a, b));
}

template <class Tp>
Tp to_degree(Tp radian) {
    return radian * Tp(180) / Tp(cst::pi);
}

} // namespace alg
} // namespace cp
#endif