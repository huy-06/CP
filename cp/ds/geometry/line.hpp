#include "vector.hpp"
#include "segment.hpp"
#include "../../alg/math/arithmetic/arithmetic.hpp"

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
        : p(a), v(vector<value_type>(b - a)) {}

    line(const segment<value_type>& s) {
        p = s.a;
        v = vector<value_type>(s.b - s.a);
    }

    // ax + by + c = 0
    line (const value_type& a, const value_type& b, const value_type& c) {
        v = vector<value_type>(b, -a);
        if (alg::abs(a) > alg::abs(b)) {
            p = point<value_type>(-c / a, value_type(0));
        } else {
            p = point<value_type>(value_type(0), -c / b);
        }
    }

    std::tuple<value_type, value_type, value_type> get_equation() const {
        value_type a = -v.y;
        value_type b = v.x;
        value_type c = -(a * p.x + b * p.y);
        return { a, b, c };
    }

    friend std::ostream& operator<<(std::ostream& os, const line& l) {
        return os << "line(" << l.p << " -> vector " << l.v << ")";
    }
};

} // namespace ds
} // namespace cp
#endif