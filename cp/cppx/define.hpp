#ifndef CP_CPPX_DEFINE
#define CP_CPPX_DEFINE

#define int int64_t
#define float long double
#define var auto

#define __overload_go(_1, _2, _3, _4, e,...) e
#define __go1(n) for (typename std::decay<decltype(n)>::type _n = (n); _n > 0; --_n)
#define __go2(i, n) for (typename std::decay<decltype(n)>::type i = 0, _n##i = (n); i < _n##i; ++i)
#define __go3(i, a, b) for (typename std::decay<decltype(b)>::type i = (a), _n##i = (b); i < _n##i; ++i)
#define __go4(i, a, b, c) for (typename std::decay<decltype(b)>::type i = (a), _n##i = (b), _c##i = (c); ((_c##i) > 0 ? i < _n##i : i > _n##i); i += _c##i)
#define go(...) __overload_go(__VA_ARGS__, __go4, __go3, __go2, __go1)(__VA_ARGS__)

#define each(...) for (auto& __VA_ARGS__)

#if __cplusplus > 202002L
#define fun(ret, name, ...) auto name = [&](this auto&& name __VA_OPT__(, ) __VA_ARGS__) -> ret
#else
#define fun(ret, name, ...) std::function<ret(__VA_ARGS__)> name = [&](__VA_ARGS__) -> ret
#endif

#endif