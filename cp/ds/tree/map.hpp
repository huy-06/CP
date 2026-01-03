#include <ext/pb_ds/assoc_container.hpp>

#ifndef CP_DS_TREE_MAP
#define CP_DS_TREE_MAP
namespace cp {
namespace ds {

template <typename K, typename V>
using map = __gnu_pbds::tree<
    K, 
    V, 
    std::less<K>,
    __gnu_pbds::rb_tree_tag,
    __gnu_pbds::tree_order_statistics_node_update
>;

} // namespace ds
} // namespace cp
#endif
