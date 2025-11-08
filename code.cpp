#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <system_error>
#include <fstream>

namespace fs = std::filesystem;

class FileExplorer {
public:
    FileExplorer() {
        try {
            cwd = fs::current_path();
        } catch (std::exception &e) {
            cwd = fs::path("/");
        }
    }

    void repl() {
        std::cout << "Simple File Explorer — type 'help' for commands\n";
        while (true) {
            std::cout << cwd << " $ ";
            std::string line;
            if (!std::getline(std::cin, line)) break;
            if (line.empty()) continue;

            auto args = split_args(line);
            auto cmd = args[0];

            if (cmd == "exit" || cmd == "quit") break;
            if (cmd == "help") {
                print_help();
                continue;
            }

            try {
                if (cmd == "ls") cmd_ls(args);
                else if (cmd == "cd") cmd_cd(args);
                else if (cmd == "pwd") cmd_pwd(args);
                else if (cmd == "stat") cmd_stat(args);
                else if (cmd == "cp") cmd_copy(args);
                else if (cmd == "mv") cmd_move(args);
                else if (cmd == "rm") cmd_remove(args);
                else if (cmd == "mkdir") cmd_mkdir(args);
                else if (cmd == "touch") cmd_touch(args);
                else if (cmd == "search") cmd_search(args);
                else if (cmd == "chmod") cmd_chmod(args);
                else std::cout << "Unknown command: " << cmd << " (type 'help')\n";
            } catch (const std::exception &e) {
                std::cout << "Error: " << e.what() << "\n";
            } catch (...) {
                std::cout << "Unknown error occurred.\n";
            }
        }
    }

private:
    fs::path cwd;

    static std::vector<std::string> split_args(const std::string &line) {
        std::istringstream iss(line);
        std::vector<std::string> args;
        std::string token;

        while (iss >> token) args.push_back(token);
        return args;
    }

    void print_help() {
        std::cout << "Commands:\n"
                  << " ls [-l] [path] : list directory (use -l for details)\n"
                  << " cd <path> : change directory\n"
                  << " pwd : print current directory\n"
                  << " stat <path> : show file/directory info\n"
                  << " cp <src> <dst> : copy file/directory\n"
                  << " mv <src> <dst> : move/rename file or directory\n"
                  << " rm <path> : remove file or directory (recursive for dirs)\n"
                  << " mkdir <dir> : create directory\n"
                  << " touch <file> : create empty file (or update timestamp)\n"
                  << " search <name> [path] : recursive search for name (partial match)\n"
                  << " chmod <octal> <path> : change permissions (e.g. chmod 755 file)\n"
                  << " help : show this help\n"
                  << " exit : quit\n";
    }

    fs::path resolve(const std::string &p) {
        fs::path path(p);
        if (path.is_relative()) path = cwd / path;
        return fs::weakly_canonical(path);
    }

    void cmd_ls(const std::vector<std::string> &args) {
        bool longfmt = false;
        fs::path target = cwd;
        size_t i = 1;

        if (i < args.size() && args[i] == "-l") {
            longfmt = true;
            ++i;
        }
        if (i < args.size()) target = resolve(args[i]);
        if (!fs::exists(target)) {
            std::cout << "ls: path does not exist: " << target << "\n";
            return;
        }

        if (fs::is_regular_file(target)) {
            if (longfmt) print_detailed(target);
            else std::cout << target.filename().string() << "\n";
            return;
        }

        std::error_code ec;
        for (auto &entry : fs::directory_iterator(target, ec)) {
            if (ec) {
                std::cout << "ls: error reading directory\n";
                break;
            }
            if (longfmt) print_detailed(entry.path());
            else std::cout << entry.path().filename().string() << "\n";
        }
    }

    void print_detailed(const fs::path &p) {
        std::error_code ec;
        char typechar = '?';
        if (fs::is_directory(p, ec)) typechar = 'd';
        else if (fs::is_regular_file(p, ec)) typechar = '-';
        else if (fs::is_symlink(p, ec)) typechar = 'l';

        auto size = fs::is_regular_file(p, ec) ? fs::file_size(p, ec) : 0ULL;
        auto perms = fs::status(p, ec).permissions();

        std::cout << typechar
                  << perms_to_string(perms)
                  << ' ' << std::setw(10) << size
                  << ' ' << time_to_string(fs::last_write_time(p, ec))
                  << ' ' << p.filename().string() << "\n";
    }

    static std::string perms_to_string(fs::perms p) {
        std::string s = "---------";
        if ((p & fs::perms::owner_read) != fs::perms::none) s[0] = 'r';
        if ((p & fs::perms::owner_write) != fs::perms::none) s[1] = 'w';
        if ((p & fs::perms::owner_exec) != fs::perms::none) s[2] = 'x';
        if ((p & fs::perms::group_read) != fs::perms::none) s[3] = 'r';
        if ((p & fs::perms::group_write) != fs::perms::none) s[4] = 'w';
        if ((p & fs::perms::group_exec) != fs::perms::none) s[5] = 'x';
        if ((p & fs::perms::others_read) != fs::perms::none) s[6] = 'r';
        if ((p & fs::perms::others_write) != fs::perms::none) s[7] = 'w';
        if ((p & fs::perms::others_exec) != fs::perms::none) s[8] = 'x';
        return s;
    }

    static std::string time_to_string(const fs::file_time_type &ft) {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(
            ft - fs::file_time_type::clock::now() + system_clock::now());
        std::time_t cftime = system_clock::to_time_t(sctp);
        std::tm tm = *std::localtime(&cftime);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    void cmd_cd(const std::vector<std::string> &args) {
        if (args.size() < 2) {
            std::cout << "cd: requires argument\n";
            return;
        }
        fs::path dest = resolve(args[1]);
        if (!fs::exists(dest) || !fs::is_directory(dest)) {
            std::cout << "cd: not a directory: " << dest << "\n";
            return;
        }
        cwd = fs::weakly_canonical(dest);
    }

    void cmd_pwd(const std::vector<std::string> &) {
        std::cout << cwd << "\n";
    }

    void cmd_stat(const std::vector<std::string> &args) {
        if (args.size() < 2) {
            std::cout << "stat: requires path\n";
            return;
        }
        fs::path p = resolve(args[1]);
        if (!fs::exists(p)) {
            std::cout << "stat: path does not exist\n";
            return;
        }
        print_detailed(p);
        std::cout << "Absolute: " << fs::absolute(p).string() << "\n";
        std::cout << "Type: " << (fs::is_directory(p) ? "directory" : (fs::is_regular_file(p) ? "file" : "other")) << "\n";
    }

    void cmd_copy(const std::vector<std::string> &args) {
        if (args.size() < 3) {
            std::cout << "cp: requires source and destination\n";
            return;
        }

        fs::path src = resolve(args[1]);
        fs::path dst = resolve(args[2]);
        if (!fs::exists(src)) {
            std::cout << "cp: source does not exist\n";
            return;
        }

        std::error_code ec;
        if (fs::is_directory(src)) {
            fs::create_directories(dst, ec);
            for (auto &entry : fs::recursive_directory_iterator(src, ec)) {
                if (ec) break;
                auto rel = fs::relative(entry.path(), src, ec);
                fs::path target = dst / rel;
                if (fs::is_directory(entry.path()))
                    fs::create_directories(target, ec);
                else
                    fs::copy_file(entry.path(), target, fs::copy_options::overwrite_existing, ec);
            }
        } else {
            if (fs::is_directory(dst)) dst /= src.filename();
            fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec);
        }

        if (ec) std::cout << "cp: error: " << ec.message() << "\n";
    }

    void cmd_move(const std::vector<std::string> &args) {
        if (args.size() < 3) {
            std::cout << "mv: requires source and destination\n";
            return;
        }

        fs::path src = resolve(args[1]);
        fs::path dst = resolve(args[2]);
        if (!fs::exists(src)) {
            std::cout << "mv: source does not exist\n";
            return;
        }

        std::error_code ec;
        if (fs::is_directory(dst)) dst /= src.filename();
        fs::rename(src, dst, ec);
        if (ec) {
            cmd_copy(args);
            std::vector<std::string> rmargs = {"rm", args[1]};
            cmd_remove(rmargs);
        }
    }

    void cmd_remove(const std::vector<std::string> &args) {
        if (args.size() < 2) {
            std::cout << "rm: requires path\n";
            return;
        }

        fs::path p = resolve(args[1]);
        if (!fs::exists(p)) {
            std::cout << "rm: path does not exist\n";
            return;
        }

        std::error_code ec;
        if (fs::is_directory(p))
            fs::remove_all(p, ec);
        else
            fs::remove(p, ec);

        if (ec) std::cout << "rm: error: " << ec.message() << "\n";
    }

    void cmd_mkdir(const std::vector<std::string> &args) {
        if (args.size() < 2) {
            std::cout << "mkdir: requires directory name\n";
            return;
        }
        fs::path dir = resolve(args[1]);
        std::error_code ec;
        fs::create_directories(dir, ec);
        if (ec) std::cout << "mkdir: error: " << ec.message() << "\n";
    }

    void cmd_touch(const std::vector<std::string> &args) {
        if (args.size() < 2) {
            std::cout << "touch: requires filename\n";
            return;
        }

        fs::path file = resolve(args[1]);
        std::error_code ec;
        if (!fs::exists(file)) {
            std::ofstream ofs(file.string());
            if (!ofs) std::cout << "touch: could not create file\n";
            ofs.close();
        } else {
            auto now = fs::file_time_type::clock::now();
            fs::last_write_time(file, now, ec);
            if (ec) std::cout << "touch: failed to update timestamp: " << ec.message() << "\n";
        }
    }

    void cmd_search(const std::vector<std::string> &args) {
        if (args.size() < 2) {
            std::cout << "search: requires name\n";
            return;
        }

        std::string name = args[1];
        fs::path start = cwd;
        if (args.size() >= 3) start = resolve(args[2]);
        if (!fs::exists(start) || !fs::is_directory(start)) {
            std::cout << "search: start path not a directory\n";
            return;
        }

        std::error_code ec;
        for (auto &entry : fs::recursive_directory_iterator(start, ec)) {
            if (ec) {
                std::cout << "search: error reading directory\n";
                break;
            }
            std::string fname = entry.path().filename().string();
            if (fname.find(name) != std::string::npos)
                std::cout << entry.path().string() << "\n";
        }
    }

    void cmd_chmod(const std::vector<std::string> &args) {
        if (args.size() < 3) {
            std::cout << "chmod: requires mode and path\n";
            return;
        }

        std::string mode_str = args[1];
        fs::path target = resolve(args[2]);
        if (!fs::exists(target)) {
            std::cout << "chmod: path does not exist\n";
            return;
        }

        int mode = 0;
        try {
            mode = std::stoi(mode_str, nullptr, 8);
        } catch (...) {
            std::cout << "chmod: invalid mode\n";
            return;
        }

        fs::perms p = octal_to_perms(mode);
        std::error_code ec;
        fs::permissions(target, p, ec);
        if (ec) std::cout << "chmod: error: " << ec.message() << "\n";
    }

    static fs::perms octal_to_perms(int oct) {
        fs::perms p = fs::perms::none;
        int u = (oct >> 6) & 7;
        int g = (oct >> 3) & 7;
        int o = oct & 7;

        if (u & 4) p |= fs::perms::owner_read;
        if (u & 2) p |= fs::perms::owner_write;
        if (u & 1) p |= fs::perms::owner_exec;
        if (g & 4) p |= fs::perms::group_read;
        if (g & 2) p |= fs::perms::group_write;
        if (g & 1) p |= fs::perms::group_exec;
        if (o & 4) p |= fs::perms::others_read;
        if (o & 2) p |= fs::perms::others_write;
        if (o & 1) p |= fs::perms::others_exec;

        return p;
    }
};

int main() {
    FileExplorer fe;
    fe.repl();
    std::cout << "Bye!\n";
    return 0;
}
