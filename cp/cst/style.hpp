#include <string>

#ifndef CP_CST_STYLE
#define CP_CST_STYLE
namespace cp {
namespace cst {
namespace style {

constexpr std::string_view reset = "\x1b[0m";

constexpr std::string_view color_black   = "\x1b[30m";
constexpr std::string_view color_red     = "\x1b[31m";
constexpr std::string_view color_green   = "\x1b[32m";
constexpr std::string_view color_yellow  = "\x1b[33m";
constexpr std::string_view color_blue    = "\x1b[34m";
constexpr std::string_view color_magenta = "\x1b[35m";
constexpr std::string_view color_cyan    = "\x1b[36m";
constexpr std::string_view color_white   = "\x1b[37m";

constexpr std::string_view backgroud_black   = "\x1b[40m";
constexpr std::string_view backgroud_red     = "\x1b[41m";
constexpr std::string_view backgroud_green   = "\x1b[42m";
constexpr std::string_view backgroud_yellow  = "\x1b[43m";
constexpr std::string_view backgroud_blue    = "\x1b[44m";
constexpr std::string_view backgroud_magenta = "\x1b[45m";
constexpr std::string_view backgroud_cyan    = "\x1b[46m";
constexpr std::string_view backgroud_white   = "\x1b[47m";

constexpr std::string_view bold      = "\x1b[1m";
constexpr std::string_view italic    = "\x1b[3m";
constexpr std::string_view underline = "\x1b[4m";

} // namespace style
} // namespace cst
} // namespace cp
#endif