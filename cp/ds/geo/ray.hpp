#include "vector.hpp"

#ifndef CP_DS_GEO_RAY
#define CP_DS_GEO_RAY
namespace cp {
namespace ds {

template<class Tp>
class ray {
public:
    using value_type = Tp;

    point<value_type> p;
    vector<value_type> v;

    ray(const point<value_type>& p = point<value_type>(), const vector<value_type>& v = vector<value_type>())
        : p(p), v(v) {}

    ray(const point<value_type>& a, const point<value_type>& b)
        : p(a), v(vector<value_type>(b - a)) {}

    friend std::ostream& operator<<(std::ostream& os, const ray& r) {
        return os << "ray(" << r.p << " -> vector " << r.v << ")";
    }
};

} // namespace ds
} // namespace cp
#endif