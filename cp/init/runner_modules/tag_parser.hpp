#include <string>
#include <string_view>
#include <vector>
#include <cctype>
#include <algorithm>
#include <sstream>

#include "config.hpp"

#ifndef CP_INIT_RUNNER_TAG_PARSER_HPP
#define CP_INIT_RUNNER_TAG_PARSER_HPP

namespace cp {
namespace init {
namespace runner {

struct test_case {
    std::string input;
    std::string expected; // rỗng nếu không có [OUT]
    bool has_expected = false;
    bool skipped = false;
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
    std::vector<test_case> tests; // từ [IN]/[OUT]
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

enum class tag_kind {
    TAG_IN,
    TAG_OUT,
    TAG_OTHER,
    BLOCK_END
};

struct tag_event {
    tag_kind kind;
    size_t   tag_start; // index of '[' (or '*' for '*/')
    size_t   tag_end;   // index of ']' + 1 (or '/' + 1 for '*/')
    bool     skipped;
};

} // namespace detail

inline parse_output parse_source(const std::string& src_content) {
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
                    if (norm == "IN") {
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_IN,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "OUT") {
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OUT,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "FILE-IN") {
                        out.tags.file_in.found = true;
                        out.tags.file_in.skipped = skipped;
                        out.tags.file_in.value = detail::strip_outer_quotes(raw_val);
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
                        });
                    } else if (norm == "FILE-OUT") {
                        out.tags.file_out.found = true;
                        out.tags.file_out.skipped = skipped;
                        out.tags.file_out.value = detail::strip_outer_quotes(raw_val);
                        events.push_back(detail::tag_event{
                            .kind = detail::tag_kind::TAG_OTHER,
                            .tag_start = i,
                            .tag_end = close + 1,
                            .skipped = skipped
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
        if (events[k].kind == detail::tag_kind::TAG_IN) {
            test_case tc;
            tc.skipped = events[k].skipped;
            size_t in_content_start = events[k].tag_end;

            size_t next_idx = k + 1;
            if (next_idx < events.size() && events[next_idx].kind == detail::tag_kind::TAG_OUT) {
                size_t in_content_end = get_clean_delim_start(events[next_idx].tag_start);
                if (in_content_end >= in_content_start) {
                    tc.input = detail::trim(std::string_view(src_content).substr(in_content_start, in_content_end - in_content_start));
                }

                size_t out_content_start = events[next_idx].tag_end;
                size_t after_out_idx = next_idx + 1;
                size_t out_content_end = (after_out_idx < events.size())
                    ? get_clean_delim_start(events[after_out_idx].tag_start)
                    : src_content.size();

                if (!events[next_idx].skipped) {
                    tc.has_expected = true;
                    if (out_content_end >= out_content_start) {
                        tc.expected = detail::trim(std::string_view(src_content).substr(out_content_start, out_content_end - out_content_start));
                    }
                } else {
                    tc.has_expected = false;
                    tc.expected.clear();
                }

                out.tests.push_back(std::move(tc));
                k = next_idx;
            } else {
                size_t in_content_end = (next_idx < events.size())
                    ? get_clean_delim_start(events[next_idx].tag_start)
                    : src_content.size();

                if (in_content_end >= in_content_start) {
                    tc.input = detail::trim(std::string_view(src_content).substr(in_content_start, in_content_end - in_content_start));
                }
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

