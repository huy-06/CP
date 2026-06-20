#include "point.hpp"

#ifndef CP_DS_GEO_SEGMENT
#define CP_DS_GEO_SEGMENT
namespace cp {
namespace ds {

template<class Tp>
class segment {
public:
    using value_type = Tp;

    point<value_type> a;
    point<value_type> b;

    segment(const point<value_type>& a = point<value_type>(), const point<value_type>& b = point<value_type>()) 
        : a(a), b(b) {}

    friend std::istream& operator>>(std::istream& is, segment& l) {
        return is >> l.a >> l.b;
    }

    friend std::ostream& operator<<(std::ostream& os, const segment& l) {
        return os << "[" << l.a << ", " << l.b << "]";
    }
};

} // namespace ds
} // namespace cp
#endif