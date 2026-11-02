#include <vector>
#include "point.hpp"

#ifndef CP_DS_GEO_POLYGON
#define CP_DS_GEO_POLYGON

namespace cp {
namespace ds {

/**
 * @brief Đa giác 2D biểu diễn bằng danh sách các đỉnh theo thứ tự (thường là ngược chiều kim đồng hồ - CCW).
 */
template <typename Tp>
using polygon = std::vector<point<Tp>>;

} // namespace ds
} // namespace cp

#endif // CP_DS_GEO_POLYGON
