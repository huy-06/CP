#include <array>

#ifndef CP_ALG_SP_GRID_DIR
#define CP_ALG_SP_GRID_DIR
namespace cp {
namespace alg {

constexpr std::array<std::pair<int, int>, 4> dxy4 = {std::make_pair(-1, 0), {1, 0}, {0, -1}, {0, 1}};
constexpr std::array<std::pair<int, int>, 8> dxy8 = {std::make_pair(-1, 0), {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
constexpr std::array<std::pair<int, int>, 8> dxyk = {std::make_pair(-2, -1), {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};

    
} // namespace alg
} // namespace cp
#endif