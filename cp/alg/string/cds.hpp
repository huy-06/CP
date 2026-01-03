#include "../../ds/string/suffix_automaton.hpp"

#ifndef CP_ALG_STRING_CDS
#define CP_ALG_STRING_CDS
namespace cp {
namespace alg {

// count distinct subsegments
template <typename Array>
long long cds(const Array& a) {
    using type = typename Array::value_type;

    ds::suffix_automaton<type> sam(a);

    long long ans = 0;
    for (int i = 1; i < int(sam.states.size()); ++i) {
        ans += sam.states[i].len - sam.states[sam.states[i].link].len;
    }

    return ans;
}

} // namespace alg
} // namespace cp
#endif