#include "map.hpp"

#ifndef CP_DS_TREE_SET
#define CP_DS_TREE_SET
namespace cp {
namespace ds {

template <typename Tp>
using set = map<Tp, __gnu_pbds::null_type>;

} // namespace ds
} // namespace cp
#endif
