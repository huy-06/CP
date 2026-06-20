#include <vector>
#include "point.hpp"

#ifndef CP_DS_GEO_POLYGON
#define CP_DS_GEO_POLYGON
namespace cp {
namespace ds {

template <typename Tp>
using polygon = std::vector<point<Tp>>;

} // namespace ds
} // namespace cp
#endif