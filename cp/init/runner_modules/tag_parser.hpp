#include <string>
#include <string_view>
#include <vector>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "config.hpp"

#ifndef CP_INIT_RUNNER_TAG_PARSER_HPP
#define CP_INIT_RUNNER_TAG_PARSER_HPP

namespace cp {
namespace init {
namespace runner {

namespace fs = std::filesystem;

inline fs::path resolve_relative_path(const fs::path& base_dir, const std::string& path_str) {
    fs::path p(path_str);
    if (p.is_absolute()) return p;
    if (base_dir.empty()) {
        std::error_code ec;
        return fs::absolute(p, ec).lexically_normal();
    }
    return (base_dir / p).lexically_normal();
}

inline std::string read_file_content(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    if (!f) return "";
    return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

struct test_case {
    std::string input;
    std::string expected; // rỗng nếu không có [OUT]
    bool has_expected = false;
    bool skipped = false;
    std::string input_file;    // tên/đường dẫn file nếu đọc từ file (ví dụ "sub1.in")
    std::string expected_file; // tên/đường dẫn file nếu đọc từ file (ví dụ "sub1.out")
};

struct tag_result {
    bool found = false;
    bool skipped = false;  // prefix [-...]
    std::string value;     // phần sau dấu ':'
};

struct parsed_tags {
    tag_result gen, brute, checker, interactive, judge;
    tag_result file_in, file_out;
    tag_result manual, run_only, evaluate;
    std::string mode_override;     // "manual", "run_only", "evaluate"
    std::string mode_override_tag; // "MANUAL", "RUN ONLY", "EVALUATE"
};

struct parse_output {
    parsed_tags            tags;
    std::vector<test_case> tests; // từ [IN]/[OUT] hoặc [TESTS_DIR]
    std::vector<std::string> errors; // lỗi file/thư mục không tìm thấy
};

namespace detail {

inline std::string trim(std::string_view s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string_view::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return std::string(s.substr(start, end - start + 1));
}

inline std::string strip_outer_quotes(std::string_view s) {
    std::string str = trim(s);
    if (str.size() >= 2 && ((str.front() == '"' && str.back() == '"') || (str.front() == '\'' && str.back() == '\''))) {
        str = str.substr(1, str.size() - 2);
    }
    return trim(str);
}

inline std::string normalize_tag_name(std::string_view name) {
    std::string norm;
    for (char c : name) {
        char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (ch == '_' || ch == ' ' || ch == '-') {
            if (!norm.empty() && norm.back() != '-') {
                norm.push_back('-');
            }
        } else {
            norm.push_back(ch);
        }
    }
    while (!norm.empty() && norm.front() == '-') norm.erase(norm.begin());
    while (!norm.empty() && norm.back() == '-') norm.pop_back();
    return norm;
}

inline bool natural_less(std::string_view a, std::string_view b) {
    size_t ia = 0, ib = 0;
    while (ia < a.size() && ib < b.size()) {
        if (std::isdigit(static_cast<unsigned char>(a[ia])) && std::isdigit(static_cast<unsigned char>(b[ib]))) {
            size_t ja = ia;
            while (ja < a.size() && std::isdigit(static_cast<unsigned char>(a[ja]))) ++ja;
            size_t jb = ib;
            while (jb < b.size() && std::isdigit(static_cast<unsigned char>(b[jb]))) ++jb;

            size_t sa = ia;
            while (sa + 1 < ja && a[sa] == '0') ++sa;
            size_t sb = ib;
            while (sb + 1 < jb && b[sb] == '0') ++sb;

            size_t len_a = ja - sa;
            size_t len_b = jb - sb;
            if (len_a != len_b) return len_a < len_b;

            std::string_view va = a.substr(sa, len_a);
            std::string_view vb = b.substr(sb, len_b);
            if (va != vb) return va < vb;

            if ((ja - ia) != (jb - ib)) return (ja - ia) < (jb - ib);

            ia = ja;
            ib = jb;
        } else {
            char ca = static_cast<char>(std::tolower(static_cast<unsigned char>(a[ia])));
            char cb = static_cast<char>(std::tolower(static_cast<unsigned char>(b[ib])));
            if (ca != cb) return ca < cb;
            ++ia;
            ++ib;
        }
    }
    return a.size() < b.size();
}

inline void load_tests_from_dir(
    const std::string& raw_val,
    const fs::path& base_dir,
    parse_output& out
) {
    std::string dir_str = raw_val;
    std::string in_pattern = "";
    std::string out_pattern = "";

    size_t paren_open = dir_str.find('(');
    size_t paren_close = dir_str.rfind(')');
    if (paren_open != std::string::npos && paren_close != std::string::npos && paren_close > paren_open) {
        std::string pat = dir_str.substr(paren_open + 1, paren_close - paren_open - 1);
        dir_str = trim(dir_str.substr(0, paren_open));
        size_t comma = pat.find(',');
        if (comma != std::string::npos) {
            in_pattern = trim(pat.substr(0, comma));
            out_pattern = trim(pat.substr(comma + 1));
        }
    }
    dir_str = strip_outer_quotes(dir_str);
    if (dir_str.empty()) return;

    fs::path dir_path = resolve_relative_path(base_dir, dir_str);
    std::error_code ec;
    if (!fs::exists(dir_path, ec) || !fs::is_directory(dir_path, ec)) {
        out.errors.push_back("tests directory not found: " + dir_str);
        return;
    }

    std::vector<fs::path> subdirs;
    std::vector<fs::path> regular_files;

    for (const auto& entry : fs::directory_iterator(dir_path, ec)) {
        if (entry.is_directory(ec)) {
            subdirs.push_back(entry.path());
        } else if (entry.is_regular_file(ec)) {
            regular_files.push_back(entry.path());
        }
    }

    // 1. Kiểm tra Layout A: Mỗi thư mục con là một test (HSG / Themis)
    if (!subdirs.empty()) {
        std::sort(subdirs.begin(), subdirs.end(), [](const fs::path& a, const fs::path& b) {
            return natural_less(a.filename().string(), b.filename().string());
        });

        bool found_any_sub_test = false;
        for (const auto& subdir : subdirs) {
            fs::path in_file, out_file;
            for (const auto& f : fs::directory_iterator(subdir, ec)) {
                if (!f.is_regular_file(ec)) continue;
                std::string ext = f.path().extension().string();
                std::string name = f.path().filename().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                std::transform(name.begin(), name.end(), name.begin(), ::tolower);

                bool is_in = (ext == ".in" || ext == ".inp" || name.find("input") != std::string::npos);
                bool is_out = (ext == ".out" || ext == ".ans" || ext == ".a" || name.find("output") != std::string::npos || name.find("ans") != std::string::npos);

                if (is_in && !is_out) {
                    if (in_file.empty() || ext == ".in" || ext == ".inp") in_file = f.path();
                } else if (is_out && !is_in) {
                    if (out_file.empty() || ext == ".out" || ext == ".ans" || ext == ".a") out_file = f.path();
                }
            }

            if (!in_file.empty()) {
                found_any_sub_test = true;
                test_case tc;
                tc.input = read_file_content(in_file);
                tc.input_file = (subdir.filename() / in_file.filename()).string();
                if (!out_file.empty()) {
                    tc.expected = read_file_content(out_file);
                    tc.expected_file = (subdir.filename() / out_file.filename()).string();
                    tc.has_expected = true;
                }
                out.tests.push_back(std::move(tc));
            }
        }
        if (found_any_sub_test) return;
    }

    // 2. Layout B: Tất cả test nằm chung trong thư mục (Flat layout: Codeforces / CSES / Hackerrank)
    if (!regular_files.empty()) {
        std::sort(regular_files.begin(), regular_files.end(), [](const fs::path& a, const fs::path& b) {
            return natural_less(a.filename().string(), b.filename().string());
        });

        std::vector<fs::path> in_files;
        std::vector<fs::path> out_files;

        for (const auto& f : regular_files) {
            std::string ext = f.extension().string();
            std::string name = f.filename().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);

            bool is_in = (ext == ".in" || ext == ".inp" || name.find("input") != std::string::npos);
            bool is_out = (ext == ".out" || ext == ".ans" || ext == ".a" || name.find("output") != std::string::npos || name.find("ans") != std::string::npos);

            if (is_in && !is_out) {
                in_files.push_back(f);
            } else if (is_out && !is_in) {
                out_files.push_back(f);
            } else if (is_in && is_out) {
                out_files.push_back(f);
            }
        }

        for (const auto& inf : in_files) {
            test_case tc;
            tc.input = read_file_content(inf);
            tc.input_file = inf.filename().string();

            std::string stem_in = inf.stem().string();
            fs::path matched_out;

            for (const auto& outf : out_files) {
                std::string stem_out = outf.stem().string();
                if (stem_in == stem_out) {
                    matched_out = outf;
                    break;
                }
                std::string lower_in = stem_in;
                std::string lower_out = stem_out;
                std::transform(lower_in.begin(), lower_in.end(), lower_in.begin(), ::tolower);
                std::transform(lower_out.begin(), lower_out.end(), lower_out.begin(), ::tolower);
                if (lower_in.starts_with("input") && lower_out.starts_with("output") && lower_in.substr(5) == lower_out.substr(6)) {
                    matched_out = outf;
                    break;
                }
                if (lower_in.starts_with("in_") && lower_out.starts_with("out_") && lower_in.substr(3) == lower_out.substr(4)) {
                    matched_out = outf;
                    break;
                }
                if (lower_in.starts_with("in") && lower_out.starts_with("out") && lower_in.substr(2) == lower_out.substr(3)) {
                    matched_out = outf;
                    break;
                }
            }

            if (!matched_out.empty()) {
                tc.expected = read_file_content(matched_out);
                tc.expected_file = matched_out.filename().string();
                tc.has_expected = true;
            }
            out.tests.push_back(std::move(tc));
        }
    }
}

enum class tag_kind {
    TAG_IN,
    TAG_OUT,
    TAG_TESTS_DIR,
    TAG_OTHER,
    BLOCK_END
};

struct tag_event {
    tag_kind kind;
    size_t   tag_start; // index of '[' (or '*' for '*/')
    size_t   tag_end;   // index of ']' + 1 (or '/' + 1 for '*/')
    bool     skipped;
    std::string file_path;
    std::string raw_val;
};

} // namespace detail

inline parse_output parse_source(const std::string& src_content, const fs::path& base_dir = "") {
    parse_output out;
    std::vector<detail::tag_event> events;

    const size_t n = src_content.size();
    size_t i = 0;
    bool in_line_comment = false;
    bool in_block_comment = false;
    bool in_string = false;
    bool in_char = false;

    while (i < n) {
        if (in_string) {
            if (src_content[i] == '\\' && i + 1 < n) {
                i += 2;
                continue;
            }
            if (src_content[i] == '"') {
                in_string = false;
            }
            ++i;
            continue;
        }

        if (in_char) {
            if (src_content[i] == '\\' && i + 1 < n) {
                i += 2;
                continue;
            }
            if (src_content[i] == '\'') {
                in_char = false;
            }
            ++i;
            continue;
        }

        if (in_line_comment) {
            if (src_content[i] == '\n') {
                in_line_comment = false;
                ++i;
                continue;
            }
        }

        if (in_block_comment) {
            if (src_content[i] == '*' && i + 1 < n && src_content[i + 1] == '/') {
                events.push_back(detail::tag_event{
                    .kind = detail::tag_kind::BLOCK_END,
                    .tag_start = i,
                    .tag_end = i + 2,
                    .skipped = false
                });
                in_block_comment = false;
                i += 2;
                continue;
            }
        }

        if (!in_line_comment && !in_block_comment) {
            if (src_content[i] == '"') {
                in_string = true;
                ++i;
                continue;
            }
            if (src_content[i] == '\'') {
                in_char = true;
                ++i;
                continue;
            }
            if (src_content[i] == '/' && i + 1 < n) {
                if (src_content[i + 1] == '/') {
                    in_line_comment = true;
                    i += 2;
                    continue;
                }
                if (src_content[i + 1] == '*') {
                    in_block_comment = true;
                    i += 2;
                    continue;
                }
            }
        }

        if (src_content[i] == '[') {
            size_t close = src_content.find(']', i + 1);
            if (close != std::string::npos) {
                bool valid_tag_bounds = true;
                if (in_line_comment) {
                    for (size_t k = i + 1; k < close; ++k) {
                        if (src_content[k] == '\n' || src_content[k] == '\r') {
                            valid_tag_bounds = false;
                            break;
                        }
                    }
                }
                if (in_block_comment) {
                    for (size_t k = i + 1; k + 1 < close; ++k) {
                        if (src_content[k] == '*' && src_content[k + 1] == '/') {
                            valid_tag_bounds = false;
                            break;
                        }
                    }
                }

                if (valid_tag_bounds) {
                    std::string inner = src_content.substr(i + 1, close - (i + 1));
                    std::string trimmed_inner = detail::trim(inner);
                    bool skipped = false;
                    if (!trimmed_inner.empty() && trimmed_inner.front() == '-') {
                        skipped = true;
                        trimmed_inner = detail::trim(trimmed_inner.substr(1));
                    }

                    size_t colon = trimmed_inner.find(':');
                    std::string raw_name = (colon != std::string::npos) ? trimmed_inner.substr(0, colon) : trimmed_inner;
                    std::string raw_val = (colon != std::string::npos) ? trimmed_inner.substr(colon + 1) : "";
                    std::string norm = detail::normalize_tag_name(raw_name);

                    bool is_tag = true;
                    if (norm == "IN" || norm == "FILE-IN") {
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_IN,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped,
                            .file_path = detail::strip_outer_quotes(raw_val),
                            .raw_val = ""
                        });
                    } else if (norm == "OUT" || norm == "FILE-OUT") {
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OUT,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped,
                            .file_path = detail::strip_outer_quotes(raw_val),
                            .raw_val = ""
                        });
                    } else if (norm == "TESTS-DIR" || norm == "TEST-DIR" || norm == "DIR" || norm == "TESTS") {
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_TESTS_DIR,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped,
                            .file_path = "",
                            .raw_val = detail::trim(raw_val)
                        });
                    } else if (norm == "GEN") {
                        out.tags.gen.found = true;
                        out.tags.gen.skipped = skipped;
                        out.tags.gen.value = detail::trim(raw_val);
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "BRUTE") {
                        out.tags.brute.found = true;
                        out.tags.brute.skipped = skipped;
                        out.tags.brute.value = detail::trim(raw_val);
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "CHECKER") {
                        out.tags.checker.found = true;
                        out.tags.checker.skipped = skipped;
                        out.tags.checker.value = detail::trim(raw_val);
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "INTERACTIVE") {
                        out.tags.interactive.found = true;
                        out.tags.interactive.skipped = skipped;
                        out.tags.interactive.value = detail::trim(raw_val);
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "JUDGE") {
                        out.tags.judge.found = true;
                        out.tags.judge.skipped = skipped;
                        out.tags.judge.value = detail::trim(raw_val);
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "MANUAL" || (norm == "INPUT" && detail::normalize_tag_name(raw_val) == "MANUAL") || (norm == "MODE" && detail::normalize_tag_name(raw_val) == "MANUAL")) {
                        out.tags.manual.found = true;
                        out.tags.manual.skipped = skipped;
                        if (!skipped) {
                            out.tags.mode_override = "manual";
                            out.tags.mode_override_tag = "MANUAL";
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "RUN-ONLY" || (norm == "MODE" && detail::normalize_tag_name(raw_val) == "RUN-ONLY")) {
                        out.tags.run_only.found = true;
                        out.tags.run_only.skipped = skipped;
                        if (!skipped) {
                            out.tags.mode_override = "run_only";
                            out.tags.mode_override_tag = "RUN ONLY";
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "EVALUATE" || norm == "EVAL" || (norm == "MODE" && (detail::normalize_tag_name(raw_val) == "EVALUATE" || detail::normalize_tag_name(raw_val) == "EVAL"))) {
                        out.tags.evaluate.found = true;
                        out.tags.evaluate.skipped = skipped;
                        if (!skipped) {
                            out.tags.mode_override = "evaluate";
                            out.tags.mode_override_tag = "EVALUATE";
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "TL") {
                        if (!skipped) {
                            try {
                                config::time_limit = std::stod(detail::strip_outer_quotes(raw_val));
                            } catch (...) {}
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "EPS") {
                        if (!skipped) {
                            try {
                                config::float_eps = std::stold(detail::strip_outer_quotes(raw_val));
                            } catch (...) {}
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "NO-DEBUG") {
                        if (!skipped) {
                            config::show_debug = false;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "NO-LOG") {
                        if (!skipped) {
                            config::show_log = false;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "NO-BUNDLE") {
                        if (!skipped) {
                            config::enable_bundle = false;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "SAVE" || norm == "SAVE-FAILED" || norm == "SAVE-TEST" || norm == "SAVE-TESTS" || norm == "SAVE-FILE") {
                        if (!skipped) {
                            config::save_failed = true;
                            if (!raw_val.empty()) {
                                config::failed_save_dir = detail::strip_outer_quotes(raw_val);
                            }
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "NO-SAVE") {
                        if (!skipped) {
                            config::save_failed = false;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "BUNDLE-ONLY") {
                        if (!skipped) {
                            config::bundle_only = true;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "NO-EMOJI" || norm == "REMOVE-EMOJI") {
                        if (!skipped) {
                            config::remove_emojis = true;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "KEEP-EMOJI") {
                        if (!skipped) {
                            config::remove_emojis = false;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "ASCII-ONLY" || norm == "STRICT-ASCII") {
                        if (!skipped) {
                            config::strict_ascii_only = true;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "NO-INSERT" || norm == "NO-APPEND") {
                        if (!skipped) {
                            config::append_failed_to_src = false;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "INSERT" || norm == "APPEND") {
                        if (!skipped) {
                            config::append_failed_to_src = true;
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "SAVE-DIR") {
                        if (!skipped) {
                            config::save_failed = true;
                            config::failed_save_dir = detail::strip_outer_quotes(raw_val);
                        }
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else {
                        is_tag = false;
                    }

                    if (is_tag) {
                        i = close + 1;
                        continue;
                    }
                }
            }
        }

        ++i;
    }

    auto get_clean_delim_start = [&](size_t pos) -> size_t {
        if (pos == 0) return 0;
        size_t line_start = pos;
        while (line_start > 0 && src_content[line_start - 1] != '\n' && src_content[line_start - 1] != '\r') {
            --line_start;
        }
        std::string_view prefix = std::string_view(src_content).substr(line_start, pos - line_start);
        size_t first_non_space = prefix.find_first_not_of(" \t");
        if (first_non_space != std::string_view::npos) {
            std::string_view trimmed_pre = prefix.substr(first_non_space);
            if (trimmed_pre.starts_with("//") || trimmed_pre.starts_with("*")) {
                return line_start;
            }
        }
        return pos;
    };

    for (size_t k = 0; k < events.size(); ++k) {
        if (events[k].kind == detail::tag_kind::TAG_TESTS_DIR) {
            if (!events[k].skipped) {
                detail::load_tests_from_dir(events[k].raw_val, base_dir, out);
            }
            continue;
        }

        if (events[k].kind == detail::tag_kind::TAG_IN) {
            test_case tc;
            tc.skipped = events[k].skipped;

            // 1. Xử lý INPUT
            if (!events[k].file_path.empty()) {
                tc.input_file = events[k].file_path;
                fs::path in_p = resolve_relative_path(base_dir, tc.input_file);
                std::error_code ec;
                if (!fs::exists(in_p, ec) || fs::is_directory(in_p, ec)) {
                    out.errors.push_back("input file not found: " + tc.input_file);
                } else {
                    tc.input = read_file_content(in_p);
                }
            } else {
                size_t in_content_start = events[k].tag_end;
                size_t next_idx = k + 1;
                size_t in_content_end = (next_idx < events.size())
                    ? get_clean_delim_start(events[next_idx].tag_start)
                    : src_content.size();
                if (in_content_end >= in_content_start) {
                    tc.input = detail::trim(std::string_view(src_content).substr(in_content_start, in_content_end - in_content_start));
                }
            }

            // 2. Xử lý EXPECTED (nếu tag tiếp theo là TAG_OUT)
            size_t next_idx = k + 1;
            if (next_idx < events.size() && events[next_idx].kind == detail::tag_kind::TAG_OUT) {
                if (!events[next_idx].skipped) {
                    tc.has_expected = true;
                    if (!events[next_idx].file_path.empty()) {
                        tc.expected_file = events[next_idx].file_path;
                        fs::path out_p = resolve_relative_path(base_dir, tc.expected_file);
                        std::error_code ec;
                        if (!fs::exists(out_p, ec) || fs::is_directory(out_p, ec)) {
                            out.errors.push_back("output file not found: " + tc.expected_file);
                        } else {
                            tc.expected = read_file_content(out_p);
                        }
                    } else {
                        size_t out_content_start = events[next_idx].tag_end;
                        size_t after_out_idx = next_idx + 1;
                        size_t out_content_end = (after_out_idx < events.size())
                            ? get_clean_delim_start(events[after_out_idx].tag_start)
                            : src_content.size();
                        if (out_content_end >= out_content_start) {
                            tc.expected = detail::trim(std::string_view(src_content).substr(out_content_start, out_content_end - out_content_start));
                        }
                    }
                } else {
                    tc.has_expected = false;
                    tc.expected.clear();
                }

                out.tests.push_back(std::move(tc));
                k = next_idx;
            } else {
                tc.has_expected = false;
                tc.expected.clear();
                out.tests.push_back(std::move(tc));
            }
        }
    }

    return out;
}

inline void parse_tag_file_and_iters(const std::string& val, std::string& file_out, int& iters_out, bool* no_log_out = nullptr) {
    std::istringstream iss(val);
    std::string tok;
    iters_out = 1;
    if (iss >> file_out) {
        file_out = detail::strip_outer_quotes(file_out);
    }
    while (iss >> tok) {
        std::string upper_tok = tok;
        std::transform(upper_tok.begin(), upper_tok.end(), upper_tok.begin(), ::toupper);
        if (no_log_out && (upper_tok == "NO_LOG" || upper_tok == "NOLOG" || upper_tok == "NO-LOG" || upper_tok == "NO")) {
            *no_log_out = true;
        } else {
            try {
                iters_out = std::stoi(tok);
            } catch (...) {}
        }
    }
}

} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_TAG_PARSER_HPP

