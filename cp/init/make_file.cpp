#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <string_view>

namespace fs = std::filesystem;

struct string_hash {
    using is_transparent = void;
    
    size_t operator()(std::string_view sv) const {
        return std::hash<std::string_view>{}(sv);
    }
    
    size_t operator()(const std::string& s) const {
        return std::hash<std::string>{}(s);
    }
};

class cpp_bundler {
public:
    cpp_bundler(const std::string& output_path) : fout(output_path, std::ios::trunc) {
        if (!fout) {
            std::cerr << "Can not open output file: " << output_path << '\n';
            valid = false;
        }
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
    }

    bool is_valid() const { return valid; }

    void process(const fs::path& root_path) {
        process_file(root_path);
    }

private:
    std::ofstream fout;
    
    std::unordered_set<std::string, string_hash, std::equal_to<>> seen;
    
    bool valid = true;
    bool is_first_line = true; 

    const std::vector<std::string> keep_tokens = {
        "@author",
        "@since",
        "namespace"
    };

    const std::vector<std::pair<std::string, std::string>> replacements = {
        {"🍀", "tc"},
        {"🌸", "t"},
        {"🌻🌻🌻", "solve"}
    };

    const std::vector<std::string> remove_call_patterns = {
        "debug(",
    };

    std::string_view trim_view(std::string_view s) {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        
        if (start >= end) return {};
        return s.substr(start, end - start);
    }

    void rtrim(std::string& s) {
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
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

    void remove_calls_in_line(std::string& line) {
        if (line.empty()) return;

        bool possible = false;
        for(const auto& pat : remove_call_patterns) {
            if(line.find(pat) != std::string::npos) {
                possible = true;
                break;
            }
        }
        if(!possible) return;

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
                        while (k < line.size() && std::isspace(static_cast<unsigned char>(line[k]))) ++k;
                        if (k < line.size() && line[k] == ';') {
                            ++k;
                            while (k < line.size() && std::isspace(static_cast<unsigned char>(line[k]))) ++k;
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

    void process_file(const fs::path& p) {
        std::error_code ec;
        fs::path abs_path = fs::absolute(p, ec).lexically_normal();
        if (ec) return;

        std::string key = abs_path.string();
        if (seen.find(key) != seen.end()) return;
        seen.insert(key);

        std::ifstream fin(abs_path);
        if (!fin) {
            std::cerr << "Can not open file: " << abs_path << '\n';
            return;
        }

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

            size_t pos_include = line.find("#include \"");
            size_t pos_slash = line.find("//");
            if (pos_include != std::string::npos && 
               (pos_slash == std::string::npos || pos_include < pos_slash)) {
                
                size_t first_q = line.find('"', pos_include);
                size_t second_q = (first_q != std::string::npos) ? line.find('"', first_q + 1) : std::string::npos;
                
                if (first_q != std::string::npos && second_q != std::string::npos) {
                    std::string inc_rel = line.substr(first_q + 1, second_q - first_q - 1);
                    process_file(parent / inc_rel);
                    continue; 
                }
            }

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

            pos_slash = line.find("//");
            if (pos_slash != std::string::npos) {
                std::string_view comment_part = std::string_view(line).substr(pos_slash);
                bool keep = false;
                for (const auto& tok : keep_tokens) {
                    if (comment_part.find(tok) != std::string_view::npos) {
                        keep = true;
                        break;
                    }
                }

                if (!keep) {
                    size_t pos_bang = line.find("//!", pos_slash);
                    if (pos_bang == pos_slash) {
                        std::string prefix = line.substr(0, pos_bang);
                        std::string tail = (pos_bang + 3 < line.size()) ? line.substr(pos_bang + 3) : "";
                        size_t j = 0;
                        while(j < tail.size() && (tail[j] == ' ' || tail[j] == '\t' || tail[j] == '!')) ++j;
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

            remove_calls_in_line(line);

            for (const auto& kv : replacements) {
                replace_all(line, kv.first, kv.second);
            }

            rtrim(line);

            std::string_view current_trim = trim_view(line);

            if (!current_trim.empty()) {
                write_line(line);
            } else {
                if (originally_blank) {
                    write_line("");
                }
            }
        }
    }
};

int main() {
    std::string root_path_str;
    if (!std::getline(std::cin, root_path_str) || root_path_str.empty()) {
        std::cerr << "No input file provided\n";
        return 1;
    }
    
    if (root_path_str.size() >= 2 && root_path_str.front() == '"' && root_path_str.back() == '"') {
        root_path_str = root_path_str.substr(1, root_path_str.size() - 2);
    }

    std::string submit_path = "E:\\Code\\CP\\Tasks\\CPP\\submit\\submit.cpp";
    
    cpp_bundler bundler(submit_path);
    if (bundler.is_valid()) {
        bundler.process(root_path_str);
        // std::cerr << "Saved!\n";
    }

    return 0;
}