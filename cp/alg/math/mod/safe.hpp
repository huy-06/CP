#ifndef CP_ALG_MOD_SAFE
#define CP_ALG_MOD_SAFE
namespace cp {
namespace alg {
namespace mod {

template <typename Tp1, typename Tp2>
constexpr Tp1 safe(Tp1 a, Tp2 mod) {
    a %= mod;
    if (a < 0) a += mod;
    return a;
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif