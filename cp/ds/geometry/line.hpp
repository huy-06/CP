#include "point.hpp"

#ifndef CP_DS_GEO_LINE
#define CP_DS_GEO_LINE
namespace cp {
namespace ds {

template<class Tp>
struct line {
    point<Tp> a;
    point<Tp> b;

    line(const point<Tp>& a = point<Tp>(), const point<Tp>& b = point<Tp>()) : a(a), b(b) {}

    friend std::istream& operator>>(std::istream& is, line& l) {
        return is >> l.a >> l.b;
    }

    friend std::ostream& operator<<(std::ostream& os, const line& l) {
        return os << "[" << l.a << ", " << l.b << "]";
    }
};

} // namespace ds
} // namespace cp
#endif