#include "point.hpp"

#ifndef CP_DS_GEO_VECTOR
#define CP_DS_GEO_VECTOR
namespace cp {
namespace ds {

template <typename Tp>
class vector : public point<Tp> {
public:
    using value_type = Tp;

    using point<Tp>::point;

    vector(const point<Tp>& p) 
        : point<Tp>(p.x, p.y) {}
};

} // namespace ds
} // namespace cp
#endif