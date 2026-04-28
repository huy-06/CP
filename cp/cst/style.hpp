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

constexpr std::string_view color_bright_black   = "\x1b[90m";
constexpr std::string_view color_bright_red     = "\x1b[91m";
constexpr std::string_view color_bright_green   = "\x1b[92m";
constexpr std::string_view color_bright_yellow  = "\x1b[93m";
constexpr std::string_view color_bright_blue    = "\x1b[94m";
constexpr std::string_view color_bright_magenta = "\x1b[95m";
constexpr std::string_view color_bright_cyan    = "\x1b[96m";
constexpr std::string_view color_bright_white   = "\x1b[97m";

constexpr std::string_view backgroud_black   = "\x1b[40m";
constexpr std::string_view backgroud_red     = "\x1b[41m";
constexpr std::string_view backgroud_green   = "\x1b[42m";
constexpr std::string_view backgroud_yellow  = "\x1b[43m";
constexpr std::string_view backgroud_blue    = "\x1b[44m";
constexpr std::string_view backgroud_magenta = "\x1b[45m";
constexpr std::string_view backgroud_cyan    = "\x1b[46m";
constexpr std::string_view backgroud_white   = "\x1b[47m";

constexpr std::string_view backgroud_bright_black   = "\x1b[100m";
constexpr std::string_view backgroud_bright_red     = "\x1b[101m";
constexpr std::string_view backgroud_bright_green   = "\x1b[102m";
constexpr std::string_view backgroud_bright_yellow  = "\x1b[103m";
constexpr std::string_view backgroud_bright_blue    = "\x1b[104m";
constexpr std::string_view backgroud_bright_magenta = "\x1b[105m";
constexpr std::string_view backgroud_bright_cyan    = "\x1b[106m";
constexpr std::string_view backgroud_bright_white   = "\x1b[107m";

constexpr std::string_view style_bold      = "\x1b[1m"; // In đậm
constexpr std::string_view style_dim       = "\x1b[2m"; // Làm mờ chữ
constexpr std::string_view style_italic    = "\x1b[3m"; // In nghiêng (tùy terminal hỗ trợ)
constexpr std::string_view style_underline = "\x1b[4m"; // Gạch chân
constexpr std::string_view style_blink     = "\x1b[5m"; // Nhấp nháy (tùy terminal hỗ trợ)
constexpr std::string_view style_reverse   = "\x1b[7m"; // Đảo ngược màu chữ và nền cho nhau
constexpr std::string_view style_hidden    = "\x1b[8m"; // Ẩn chữ (thường dùng khi nhập password)
constexpr std::string_view style_strike    = "\x1b[9m"; // Gạch ngang chữ (Strikethrough)s

} // namespace style
} // namespace cst
} // namespace cp
#endif