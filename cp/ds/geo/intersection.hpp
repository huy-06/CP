#include <cassert>
#include "line.hpp"
#include "ray.hpp"

#ifndef CP_DS_GEO_INTERSECTION
#define CP_DS_GEO_INTERSECTION
namespace cp {
namespace ds {

template<class Tp>
class intersection {
public:
    using value_type = Tp;

    intersection(char type = 0, point<value_type> a = point<value_type>(), point<value_type> b = point<value_type>())
        : type(type), a(a), b(b) {}

    operator bool() const {
        return type;
    }

    bool is_none() const {
        return type == 0;
    }

    bool is_point() const {
        return type == 1;
    }

    bool is_segment() const {
        return type == 2;
    }

    bool is_ray() const {
        return type == 3;
    }

    bool is_line() const {
        return type == 4;
    }

    std::string get_type() const {
        if (type == 0) return "none";
        if (type == 1) return "point";
        if (type == 2) return "segment";
        if (type == 3) return "ray";
        if (type == 4) return "line";
        return "";
    }

    point<value_type> get_point() const {
        assert(is_point());
        return a;
    }

    segment<value_type> get_segment() const {
        assert(is_segment());
        return segment<value_type>(a, b);
    }

    ray<value_type> get_ray() const {
        assert(is_ray());
        return ray<value_type>(a, b);
    }

    line<value_type> get_line() const {
        assert(is_line());
        return line<value_type>(segment<value_type>(a, b));
    }

private:
    char type;
    point<value_type> a, b;
};

} // namespace ds
} // namespace cp
#endif