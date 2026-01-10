#include "intersect.hpp"
#include "../../ds/geometry/polygon.hpp"

#ifndef CP_ALG_GEOMETRY_POLYGON
#define CP_ALG_GEOMETRY_POLYGON
namespace cp {
namespace alg {

template <class Tp>
Tp area(const ds::polygon<Tp>& poly) {
    Tp res = Tp(0);
    int n = int(poly.size());
    if (n < 3) return res;
    for (int i = 0; i < n; ++i) {
        res += cross(poly[i], poly[(i + 1) % n]);
    }
    return res / Tp(2);
}

/// @return 0: nằm ngoài, 1: nằm trên cạnh, 2: nằm trong
template <class Tp>
int is_inside(const ds::point<Tp>& p, const ds::polygon<Tp>& poly) {
    int n = int(poly.size());
    if (n == 0) return 0;
    
    bool inside = false;
    for (int i = 0; i < n; ++i) {
        auto p1 = poly[i];
        auto p2 = poly[(i + 1) % n];

        if (is_on(p, ds::segment<Tp>(p1, p2))) {
            return 1; 
        }

        if ((p1.y > p.y) != (p2.y > p.y)) {
            auto cross_val = (p2.x - p1.x) * (p.y - p1.y) - (p.x - p1.x) * (p2.y - p1.y);
            if (p2.y > p1.y) {
                if (cross_val > 0) inside = !inside;
            } else {
                if (cross_val < 0) inside = !inside;
            }
        }
    }
    return inside ? 2 : 0;
}

template <class Tp>
bool is_inside(const ds::segment<Tp>& s, const ds::polygon<Tp>& poly) {
    int pos_a = is_inside(s.a, poly);
    int pos_b = is_inside(s.b, poly);

    if (pos_a == 0 || pos_b == 0) return false; 

    int n = int(poly.size());
    for (int i = 0; i < n; ++i) {
        ds::segment<Tp> edge(poly[i], poly[(i + 1) % n]);
        auto inter = intersect(s, edge);
        
        if (inter.is_point()) {
            ds::point<Tp> p = inter.get_point();
            if (p != edge.a && p != edge.b && p != s.a && p != s.b) 
                return false;
        }
    }

    ds::point<Tp> mid((s.a.x + s.b.x) / Tp(2), (s.a.y + s.b.y) / Tp(2));
    if (is_inside(mid, poly) == 0) return false;

    return true;
}


} // namespace alg
} // namespace cp
#endif