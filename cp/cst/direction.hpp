#include <array>

#ifndef CP_CST_DIRECTION
#define CP_CST_DIRECTION
namespace cp {
namespace cst {

constexpr std::array<std::pair<int, int>, 4> dxy4 = {
    std::make_pair(-1, 0),
    {1, 0}, 
    {0, -1}, 
    {0, 1}
};

constexpr std::array<std::pair<int, int>, 8> dxy8 = {
    std::make_pair(-1, 0), 
    {1, 0}, 
    {0, -1}, 
    {0, 1}, 
    {-1, -1}, 
    {-1, 1}, 
    {1, -1}, 
    {1, 1}
};

constexpr std::array<std::pair<int, int>, 8> dxy_knight = {
    std::make_pair(-2, -1), 
    {-2, 1}, 
    {-1, -2}, 
    {-1, 2}, 
    {1, -2}, 
    {1, 2}, 
    {2, -1}, 
    {2, 1}
};

    
} // namespace cst
} // namespace cp
#endif