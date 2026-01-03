#include "unordered_map.hpp"

#ifndef CP_DS_HASH_UNORDERED_SET
#define CP_DS_HASH_UNORDERED_SET
namespace cp {
namespace ds {

template <typename Tp>
using unordered_set = unordered_map<Tp, __gnu_pbds::null_type>;

} // namespace ds
} // namespace cp
#endif
