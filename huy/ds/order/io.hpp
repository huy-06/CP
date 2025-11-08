// #include "../../python/utils.hpp"

// #ifndef HUY_IO
// #define HUY_IO
// namespace huy {
// namespace internal {
// struct IO {
//     template <typename Tp>
//     void read(Tp& v) {
//         if (!(std::cin >> v)) std::exit(0);
//     }

//     template <typename T1, typename T2>
//     void read(std::pair<T1, T2>& v) {
//         read(v.first);
//         read(v.second);
//     }

//     template <typename Tp>
//     void read(std::vector<Tp>& v) {
//         for (auto& i : v) {
//             read(i);
//         }
//     }

//     template <typename Tp, size_t N>
//     void read(std::array<Tp, N>& v) {
//         for (auto& i : v) {
//             read(i);
//         }
//     }

//     template <typename... Args>
//     void input(Args&... args) {
//         (read(args), ...);
//     }

//     template <typename Tp>
//     void map(std::vector<Tp>& v) {
//         std::string line;
//         do {
//             if (!(std::getline(std::cin, line))) {
//                 std::exit(0);
//             }
//         } while (line.empty());
//         std::stringstream ss(line);
//         v.clear();
//         for (Tp x; ss >> x; v.push_back(x));
//     }

//     template <typename... Args>
//     void maps(Args&... args) {
//         (map(args), ...);
//     }

//     template <typename... Args>
//     void maps(size_t n, Args&... args) {
//         (([&] {
//              args.assign(n, {});
//              for (auto& i : args) {
//                  maps(i);
//              }
//          })(),
//          ...);
//     }

//     template <typename Tp>
//     std::string stringify(const Tp& v) {
//         std::ostringstream oss;
//         oss.copyfmt(std::cout);
//         oss << v;
//         return oss.str();
//     }

//     template <typename T1, typename T2>
//     std::string stringify(const std::pair<T1, T2>& v) {
//         return stringify(v.first) + ' ' + stringify(v.second);
//     }

//     template <typename Tp>
//     std::string stringify(const std::vector<Tp>& v) {
//         std::string s;
//         bool ok = false;
//         for (const auto& i : v) {
//             if (ok) s += ' ';
//             s += stringify(i);
//             ok = true;
//         }
//         return s;
//     }

//     template <typename Tp, size_t N>
//     std::string stringify(const std::array<Tp, N>& v) {
//         std::string s;
//         bool ok = false;
//         for (const auto& i : v) {
//             if (ok) s += ' ';
//             s += stringify(i);
//             ok = true;
//         }
//         return s;
//     }

//     template <typename Tp>
//     std::string stringify(const std::vector<std::vector<Tp>>& v) {
//         std::string s;
//         bool ok = false;
//         for (const auto& i : v) {
//             if (ok) s += '\n';
//             s += stringify(i);
//             ok = true;
//         }
//         return s;
//     }

//     template <typename Tp, size_t N>
//     std::string stringify(const std::vector<std::array<Tp, N>>& v) {
//         std::string s;
//         bool ok = false;
//         for (const auto& i : v) {
//             if (ok) s += '\n';
//             s += stringify(i);
//             ok = true;
//         }
//         return s;
//     }

//     template <typename Tp>
//     std::string stringify(const std::set<Tp>& v) {
//         std::string s;
//         bool ok = false;
//         for (const auto& i : v) {
//             if (ok) s += ' ';
//             s += stringify(i);
//             ok = true;
//         }
//         return s;
//     }

//     template <typename T1, typename T2>
//     std::string stringify(const std::map<T1, T2>& v) {
//         std::string s;
//         bool ok = false;
//         for (const auto& i : v) {
//             if (ok) s += '\n';
//             s += stringify(i);
//             ok = true;
//         }
//         return s;
//     }

//     template <typename Tp>
//     void output(bool& ok, const Tp& v) {
//         if (ok) std::cout << ' ';
//         std::cout << stringify(v);
//         ok = true;
//     }
// } io;
// } // namespace internal

// #define I32(...) i32 __VA_ARGS__; internal::io.input(__VA_ARGS__)
// #define I64(...) i64 __VA_ARGS__; internal::io.input(__VA_ARGS__)
// #define F64(...) f64 __VA_ARGS__; internal::io.input(__VA_ARGS__)
// #define STR(...) str __VA_ARGS__; internal::io.input(__VA_ARGS__)
// #define P32(...) p32 __VA_ARGS__; internal::io.input(__VA_ARGS__)
// #define P64(...) p64 __VA_ARGS__; internal::io.input(__VA_ARGS__)
// #define VI32(...) vi32 __VA_ARGS__; internal::io.maps(__VA_ARGS__)
// #define VI64(...) vi64 __VA_ARGS__; internal::io.maps(__VA_ARGS__)
// #define VF64(...) vf64 __VA_ARGS__; internal::io.maps(__VA_ARGS__)
// #define VSTR(...) vstr __VA_ARGS__; internal::io.maps(__VA_ARGS__)
// #define VVI32(n, ...) vvi32 __VA_ARGS__; internal::io.maps(n, __VA_ARGS__)
// #define VVI64(n, ...) vvi64 __VA_ARGS__; internal::io.maps(n, __VA_ARGS__)

// i32 NI32() { I32(x); return x; }
// i32 NI64() { I64(x); return x; }
// str NSTR() { STR(x); return x; }

// template<typename... Args>
// void out(const Args&... args) {
//     bool ok = false;
//     using expander = i32[];
//     (void)expander{0, ((void)internal::io.output(ok, args), 0)...};
//     io.output('\n');
// }

// }
// #endif