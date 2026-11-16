#ifndef CP_CST_STYLE
#define CP_CST_STYLE

namespace cp {
namespace cst {
namespace style {

constexpr const char* reset = "\x1b[0m";

constexpr const char* color_black   = "\x1b[30m";
constexpr const char* color_red     = "\x1b[31m";
constexpr const char* color_green   = "\x1b[32m";
constexpr const char* color_yellow  = "\x1b[33m";
constexpr const char* color_blue    = "\x1b[34m";
constexpr const char* color_magenta = "\x1b[35m";
constexpr const char* color_cyan    = "\x1b[36m";
constexpr const char* color_white   = "\x1b[37m";

constexpr const char* color_bright_black   = "\x1b[90m";
constexpr const char* color_bright_red     = "\x1b[91m";
constexpr const char* color_bright_green   = "\x1b[92m";
constexpr const char* color_bright_yellow  = "\x1b[93m";
constexpr const char* color_bright_blue    = "\x1b[94m";
constexpr const char* color_bright_magenta = "\x1b[95m";
constexpr const char* color_bright_cyan    = "\x1b[96m";
constexpr const char* color_bright_white   = "\x1b[97m";

constexpr const char* backgroud_black   = "\x1b[40m";
constexpr const char* backgroud_red     = "\x1b[41m";
constexpr const char* backgroud_green   = "\x1b[42m";
constexpr const char* backgroud_yellow  = "\x1b[43m";
constexpr const char* backgroud_blue    = "\x1b[44m";
constexpr const char* backgroud_magenta = "\x1b[45m";
constexpr const char* backgroud_cyan    = "\x1b[46m";
constexpr const char* backgroud_white   = "\x1b[47m";

constexpr const char* backgroud_bright_black   = "\x1b[100m";
constexpr const char* backgroud_bright_red     = "\x1b[101m";
constexpr const char* backgroud_bright_green   = "\x1b[102m";
constexpr const char* backgroud_bright_yellow  = "\x1b[103m";
constexpr const char* backgroud_bright_blue    = "\x1b[104m";
constexpr const char* backgroud_bright_magenta = "\x1b[105m";
constexpr const char* backgroud_bright_cyan    = "\x1b[106m";
constexpr const char* backgroud_bright_white   = "\x1b[107m";

constexpr const char* style_bold      = "\x1b[1m"; // In đậm
constexpr const char* style_dim       = "\x1b[2m"; // Làm mờ chữ
constexpr const char* style_italic    = "\x1b[3m"; // In nghiêng (tùy terminal hỗ trợ)
constexpr const char* style_underline = "\x1b[4m"; // Gạch chân
constexpr const char* style_blink     = "\x1b[5m"; // Nhấp nháy (tùy terminal hỗ trợ)
constexpr const char* style_reverse   = "\x1b[7m"; // Đảo ngược màu chữ và nền cho nhau
constexpr const char* style_hidden    = "\x1b[8m"; // Ẩn chữ (thường dùng khi nhập password)
constexpr const char* style_strike    = "\x1b[9m"; // Gạch ngang chữ (Strikethrough)

} // namespace style
} // namespace cst
} // namespace cp

#endif
