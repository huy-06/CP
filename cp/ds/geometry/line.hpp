#include "vector.hpp"
#include "segment.hpp"

#ifndef CP_DS_GEO_LINE
#define CP_DS_GEO_LINE
namespace cp {
namespace ds {

template<class Tp>
class line {
public:
    using value_type = Tp;

    point<value_type> p;
    vector<value_type> v;

    line(const point<value_type>& p = point<value_type>(), const vector<value_type>& v = vector<value_type>())
        : p(p), v(v) {}

    line(const point<value_type>& a, const point<value_type>& b)
        : p(a), v(b - a) {}

    line(const segment<value_type>& s) {
        p = s.a;
        v = s.b - s.a;
    }
};

} // namespace ds
} // namespace cp
#endif