#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include "config.hpp"
#include "style.hpp"

#ifndef CP_INIT_RUNNER_BUNDLER_HPP
#define CP_INIT_RUNNER_BUNDLER_HPP

namespace cp {
namespace init {
namespace runner {

namespace fs = std::filesystem;

class bundler {
public:
    static fs::path resolve_submit_path(const fs::path& src_file) {
        if (!config::submit_path.empty()) {
            return fs::path(config::submit_path);
        }
        fs::path cur = src_file.parent_path();
        for (int depth = 0; depth < 5; ++depth) {
            fs::path candidate = cur / "submit";
            std::error_code ec;
            if (fs::is_directory(candidate, ec)) {
                return candidate / "submit.cpp";
            }
            if (!cur.has_parent_path() || cur == cur.parent_path()) break;
            cur = cur.parent_path();
        }
        return src_file.parent_path() / "submit" / "submit.cpp";
    }

    static bool bundle_to(const fs::path& src_file, const fs::path& out_file) {
        std::error_code ec;
        if (out_file.has_parent_path()) {
            fs::create_directories(out_file.parent_path(), ec);
        }
        bundler b(out_file);
        if (!b.is_valid()) return false;
        b.process(src_file);
        return true;
    }

    static bool run(const fs::path& src_file) {
        if (!config::enable_bundle) return true;
        fs::path submit_file = resolve_submit_path(src_file);
        return bundle_to(src_file, submit_file);
    }

private:
    std::ofstream fout;
    bool valid = true;
    bool is_first_line = true;
    bool last_line_was_empty = true;

    std::unordered_set<std::string> seen_files;
    std::unordered_set<std::string> seen_system_includes;

    const std::vector<std::string> keep_tokens = {
        "@author",
        "@since",
        "namespace"
    };

    const std::vector<std::pair<std::string, std::string>> replacements = {
        {"🍀", "tc"},
        {"🌸", "t"},
        {"🌻🌻🌻", "solve"},
        {"🧊", "tc"},
        {"🍉", "t"},
        {"🍧🍧🍧", "solve"},
        {"🎁", "tc"},
        {"🧦", "t"},
        {"🎄🎄🎄", "solve"}
    };

    const std::vector<std::string> remove_call_patterns = {
        "debug(",
    };

    explicit bundler(const fs::path& out_path) : fout(out_path, std::ios::trunc) {
        if (!fout) {
            valid = false;
        }
    }

    bool is_valid() const { return valid; }

    static std::string_view trim_view(std::string_view s) {
        size_t start = 0;
        while (start < s.size() && static_cast<unsigned char>(s[start]) <= ' ') ++start;
        size_t end = s.size();
        while (end > start && static_cast<unsigned char>(s[end - 1]) <= ' ') --end;
        if (start >= end) return {};
        return s.substr(start, end - start);
    }

    static void rtrim(std::string& s) {
        while (!s.empty() && static_cast<unsigned char>(s.back()) <= ' ') {
            s.pop_back();
        }
    }

    void write_line(const std::string& s) {
        if (!is_first_line) {
            fout << '\n';
        }
        fout << s;
        is_first_line = false;
    }

    bool is_in_whitelist(const fs::path& file_path) {
        std::string s = file_path.lexically_normal().string();
        std::replace(s.begin(), s.end(), '\\', '/');
        
        std::string filename = file_path.filename().string();
        for (const auto& skip : config::bundle_skip) {
            if (filename == skip) return false;
        }

        for (const auto& dir : config::bundle_dirs) {
            std::string pat1 = "/" + dir + "/";
            std::string pat2 = dir + "/";
            if (s.find(pat1) != std::string::npos) return true;
            if (s.rfind(pat2, 0) == 0) return true;
        }
        return false;
    }

    void remove_calls_in_line(std::string& line) {
        if (line.empty()) return;

        bool possible = false;
        for (const auto& pat : remove_call_patterns) {
            if (line.find(pat) != std::string::npos) {
                possible = true;
                break;
            }
        }
        if (!possible) return;

        std::string result;
        result.reserve(line.size());

        size_t i = 0;
        bool modified = false;
        while (i < line.size()) {
            bool matched = false;
            for (const auto& pattern : remove_call_patterns) {
                if (line.compare(i, pattern.size(), pattern) == 0) {
                    size_t pos_open = line.find('(', i + pattern.size() - 1);
                    if (pos_open == std::string::npos) continue;

                    int depth = 0;
                    bool started = false;
                    size_t j = pos_open;
                    for (; j < line.size(); ++j) {
                        char c = line[j];
                        if (c == '(') { ++depth; started = true; }
                        else if (c == ')') {
                            if (started) {
                                --depth;
                                if (depth == 0) { ++j; break; }
                            }
                        }
                    }

                    if (j > pos_open) {
                        size_t k = j;
                        while (k < line.size() && (line[k] == ' ' || line[k] == '\t')) ++k;
                        if (k < line.size() && line[k] == ';') {
                            ++k;
                            while (k < line.size() && (line[k] == ' ' || line[k] == '\t')) ++k;
                            i = k;
                        } else {
                            i = j;
                        }
                        matched = true;
                        modified = true;
                        break;
                    }
                }
            }
            if (!matched) {
                result += line[i];
                ++i;
            }
        }
        if (modified) line = std::move(result);
    }

    void replace_all(std::string& line, const std::string& key, const std::string& val) {
        if (key.empty()) return;
        size_t pos = 0;
        while ((pos = line.find(key, pos)) != std::string::npos) {
            line.replace(pos, key.size(), val);
            pos += val.size();
        }
    }

    static bool is_emoji_codepoint(uint32_t cp) {
        return (cp >= 0x1F600 && cp <= 0x1F64F) ||
               (cp >= 0x1F300 && cp <= 0x1F5FF) ||
               (cp >= 0x1F680 && cp <= 0x1F6FF) ||
               (cp >= 0x1F700 && cp <= 0x1F77F) ||
               (cp >= 0x1F780 && cp <= 0x1F7FF) ||
               (cp >= 0x1F800 && cp <= 0x1F8FF) ||
               (cp >= 0x1F900 && cp <= 0x1F9FF) ||
               (cp >= 0x1FA00 && cp <= 0x1FA6F) ||
               (cp >= 0x1FA70 && cp <= 0x1FAFF) ||
               (cp >= 0x2600  && cp <= 0x26FF)  ||
               (cp >= 0x2700  && cp <= 0x27BF)  ||
               (cp >= 0xFE00  && cp <= 0xFE0F)  ||
               (cp == 0x200D);
    }

    static void strip_emojis(std::string& line) {
        std::string result;
        result.reserve(line.size());

        for (size_t i = 0; i < line.size(); ) {
            unsigned char c = static_cast<unsigned char>(line[i]);
            size_t seq_len = 0;
            uint32_t cp = 0;

            if (c <= 0x7F) {
                result += line[i++];
                continue;
            } else if ((c & 0xE0) == 0xC0) {
                seq_len = 2;
                cp = c & 0x1F;
            } else if ((c & 0xF0) == 0xE0) {
                seq_len = 3;
                cp = c & 0x0F;
            } else if ((c & 0xF8) == 0xF0) {
                seq_len = 4;
                cp = c & 0x07;
            } else {
                i++;
                continue;
            }

            if (i + seq_len > line.size()) break;

            bool valid = true;
            for (size_t k = 1; k < seq_len; ++k) {
                unsigned char next_byte = static_cast<unsigned char>(line[i + k]);
                if ((next_byte & 0xC0) != 0x80) { valid = false; break; }
                cp = (cp << 6) | (next_byte & 0x3F);
            }

            if (valid && is_emoji_codepoint(cp)) {
                i += seq_len;
            } else {
                result.append(line, i, seq_len);
                i += seq_len;
            }
        }
        line = std::move(result);
    }

    static void strip_non_ascii(std::string& line) {
        std::string result;
        result.reserve(line.size());
        for (unsigned char c : line) {
            if (c >= 32 && c <= 126) {
                result += c;
            } else if (c == '\t' || c == '\r') {
                result += c;
            }
        }
        line = std::move(result);
    }

    void process(const fs::path& p) {
        std::error_code ec;
        fs::path abs_path = fs::absolute(p, ec).lexically_normal();
        if (ec) return;

        std::string key = abs_path.string();
        if (seen_files.count(key)) return;
        seen_files.insert(key);

        std::ifstream fin(abs_path);
        if (!fin) return;

        std::string line;
        fs::path parent = abs_path.parent_path();

        bool skipping_angle_block = false;
        bool in_block_comment = false;

        while (std::getline(fin, line)) {
            std::string_view trimmed_original = trim_view(line);
            bool originally_blank = trimmed_original.empty();

            if (in_block_comment) {
                size_t pos_end = line.find("*/");
                if (pos_end == std::string::npos) continue;
                line.erase(0, pos_end + 2);
                in_block_comment = false;
            }

            if (skipping_angle_block) {
                size_t pos_close = line.find("//>");
                if (pos_close == std::string::npos) continue;
                line.erase(0, pos_close + 3);
                skipping_angle_block = false;
                if (trim_view(line).empty()) continue;
            }

            // 1. Kiểm tra inline include: #include "relative"
            size_t pos_include = line.find("#include \"");
            size_t pos_slash = line.find("//");
            if (pos_include != std::string::npos && (pos_slash == std::string::npos || pos_include < pos_slash)) {
                size_t first_q = line.find('"', pos_include);
                size_t second_q = (first_q != std::string::npos) ? line.find('"', first_q + 1) : std::string::npos;
                if (first_q != std::string::npos && second_q != std::string::npos) {
                    std::string inc_rel = line.substr(first_q + 1, second_q - first_q - 1);
                    fs::path target_file = (parent / inc_rel).lexically_normal();

                    // Kiểm tra skip
                    std::string inc_filename = fs::path(inc_rel).filename().string();
                    bool should_skip = false;
                    for (const auto& s : config::bundle_skip) {
                        if (inc_filename == s) { should_skip = true; break; }
                    }
                    if (should_skip) {
                        continue;
                    }

                    // Nếu nằm trong whitelist dirs thì inline
                    if (is_in_whitelist(target_file)) {
                        process(target_file);
                        continue;
                    } else {
                        // Giữ nguyên dòng #include "..."
                        write_line(line);
                        last_line_was_empty = false;
                        continue;
                    }
                }
            }

            // 2. Xóa block comments /* ... */
            while (true) {
                size_t pos_block = line.find("/*");
                if (pos_block == std::string::npos) break;
                size_t pos_block_end = line.find("*/", pos_block + 2);
                if (pos_block_end != std::string::npos) {
                    line.erase(pos_block, pos_block_end + 2 - pos_block);
                } else {
                    line.erase(pos_block);
                    in_block_comment = true;
                    break;
                }
            }

            // 3. Xử lý line comments: //, //#, //!, //< ... //>
            pos_slash = line.find("//");
            if (pos_slash != std::string::npos) {
                size_t slash_end = pos_slash;
                while (slash_end < line.size() && line[slash_end] == '/') {
                    slash_end++;
                }

                bool is_hash_comment = false;
                if (slash_end < line.size() && line[slash_end] == '#') {
                    is_hash_comment = true;
                    line.erase(slash_end, 1);
                }

                std::string_view comment_part = std::string_view(line).substr(pos_slash);
                bool keep = is_hash_comment;

                if (!keep) {
                    for (const auto& tok : keep_tokens) {
                        if (comment_part.find(tok) != std::string_view::npos) {
                            keep = true;
                            break;
                        }
                    }
                }

                if (!keep) {
                    size_t pos_bang = line.find("//!", pos_slash);
                    if (pos_bang == pos_slash) {
                        // Bật lại code từ comment //!
                        std::string prefix = line.substr(0, pos_bang);
                        std::string tail = (pos_bang + 3 < line.size()) ? line.substr(pos_bang + 3) : "";
                        size_t j = 0;
                        while (j < tail.size() && (tail[j] == ' ' || tail[j] == '\t' || tail[j] == '!')) ++j;
                        line = prefix + (j < tail.size() ? tail.substr(j) : "");
                    } else {
                        size_t pos_langle = line.find("//<", pos_slash);
                        if (pos_langle == pos_slash) {
                            size_t pos_rangle = line.find("//>");
                            line.erase(pos_langle);
                            if (pos_rangle == std::string::npos) skipping_angle_block = true;
                        } else {
                            line.erase(pos_slash);
                        }
                    }
                }
            }

            // 4. Kiểm tra deduplicate system include: #include <header>
            // (Thực hiện SAU KHI //! đã được mở lại và comment // đã bị xóa)
            std::string_view current_view = trim_view(line);
            if (current_view.starts_with("#include <") && current_view.ends_with(">")) {
                std::string header(current_view);
                if (seen_system_includes.count(header)) {
                    continue; // Bỏ qua include trùng lặp
                }
                seen_system_includes.insert(header);
            }

            // 5. Xóa lời gọi debug(...)
            remove_calls_in_line(line);

            // 6. Thay thế emoji đặt tên biến
            for (const auto& kv : replacements) {
                replace_all(line, kv.first, kv.second);
            }

            // 7. Xóa emoji hoặc giữ strict ASCII
            if (config::strict_ascii_only) {
                strip_non_ascii(line);
            } else if (config::remove_emojis) {
                strip_emojis(line);
            }

            rtrim(line);

            std::string_view current_trim = trim_view(line);
            if (!current_trim.empty()) {
                write_line(line);
                last_line_was_empty = false;
            } else {
                if (originally_blank && !last_line_was_empty) {
                    write_line("");
                    last_line_was_empty = true;
                }
            }
        }
    }
};

} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_BUNDLER_HPP
