#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <thread>
#include <future>
#include <sstream>
#include <format>
#include <chrono>

namespace fs = std::filesystem;
typedef const std::vector<std::string> strvec;


enum class Tokentype {
    _command,
    _colon,
    _identifier,
    _nextline,
    _shell
};

struct Token {
    std::string name;
    Tokentype type;
};

typedef std::unordered_map<std::string, Tokentype> tmap;

inline tmap tokens{
    {":", Tokentype::_colon},
    {"shell", Tokentype::_shell}
};


std::vector<std::string> out;
std::unordered_map<std::string, std::string> usrvar;
std::vector<std::future<int>> global_hintergrund_tasks;

std::string global_clean = "clean:";
std::string projekt_word = "projekt:";
std::string file_word = "files:";
std::string global_suchwort = "shell:";
std::string build_command = "build:";
std::string global_bdcommand;

// Target-Struktur
struct BuildTarget {
    std::string name = "default";
    std::vector<std::string> zeilen;
    std::string lokales_projekt = ".";
    std::string lokales_executable = ""; 
    std::string lokale_files;
};


std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

int getfiledata() {
    std::string dateiname = "fymfile";
    std::ifstream datei(dateiname);
    out.clear();

    if (!datei.is_open()) {
        std::cerr << "error: no such file found" << std::endl;
        return 1;
    }

    std::string zeile;
    while (std::getline(datei, zeile)) {
        out.push_back(zeile);
    }
    datei.close();
    return 0;
}

void cd(const std::string& path) {
    try {
        fs::current_path(path);
        std::cout << "\033[32m" << std::format("changed directory to {}, successful", path) << "\033[0m" << std::endl;
    } 
    catch (const fs::filesystem_error& e) {
        std::cerr << "\033[31m" << std::format("error while trying to open the directory {}: {}", path, e.code().message()) << "\033[0m" << std::endl;
    }
}


void getprojekt(BuildTarget& target) {
    for (const std::string &roh_zeile : target.zeilen) {
        std::string zeile = trim(roh_zeile);
        if (zeile.empty() || zeile.rfind("#", 0) == 0 || zeile.rfind("//", 0) == 0) continue;
        size_t propos = zeile.find(projekt_word);
        if (propos != std::string::npos) {
            target.lokales_projekt = trim(zeile.substr(propos + projekt_word.length()));
            break;
        }
    }
}

void executable(BuildTarget& target) {
    bool gefunden = false;
    
    std::vector<std::string> keywords = {"executeable:", "executable:"};
    
    for (const std::string &roh_zeile : target.zeilen) {
        std::string zeile = trim(roh_zeile);
        if (zeile.empty() || zeile.rfind("#", 0) == 0 || zeile.rfind("//", 0) == 0) continue;
        
        for (const auto& key : keywords) {
            size_t executepos = zeile.find(key);
            if (executepos != std::string::npos) {
                target.lokales_executable = trim(zeile.substr(executepos + key.length()));
                gefunden = true;
                break;
            }
        }
        if (gefunden) break;
    }
    
    
    if (!gefunden || target.lokales_executable.empty()) {
        target.lokales_executable = target.name;
    }
}

void if_clean(BuildTarget& target) {
    for (const std::string &roh_zeile : target.zeilen) {
        std::string zeile = trim(roh_zeile);
        if (zeile.empty() || zeile.rfind("#", 0) == 0 || zeile.rfind("//", 0) == 0) continue;
        size_t cleanpos = zeile.find(global_clean);
        if (cleanpos != std::string::npos) {
            std::string clname = trim(zeile.substr(cleanpos + global_clean.length()));
            if (clname == "files") target.lokale_files = "";
            else if (clname == "projekt") target.lokales_projekt = "";
            else if (clname == "executeable" || clname == "executable") target.lokales_executable = "";
        }
    }
}

void vardec(strvec &exblv) {
    for (const std::string& roh_zeile : exblv) {
        std::string zeile = trim(roh_zeile);
        if (zeile.empty() || zeile.rfind("#", 0) == 0 || zeile.rfind("//", 0) == 0 ||
            zeile.find(global_suchwort) != std::string::npos || zeile.find(global_clean) != std::string::npos || 
            zeile.find(projekt_word) != std::string::npos || zeile.find(file_word) != std::string::npos ||
            zeile.find("executeable:") != std::string::npos || zeile.find("executable:") != std::string::npos) {
            continue;
        }
        size_t eq_pos = zeile.find("=");
        if (eq_pos != std::string::npos) {
            std::string name = trim(zeile.substr(0, eq_pos));
            std::string val = trim(zeile.substr(eq_pos + 1));
            if (!name.empty()) usrvar[name] = val;
        }
    }
}

void filecommand(BuildTarget& target) {
    for (const std::string &roh_zeile : target.zeilen) {
        std::string zeile = trim(roh_zeile);
        if (zeile.empty() || zeile.rfind("#", 0) == 0 || zeile.rfind("//", 0) == 0) continue;
        size_t filepos = zeile.find(file_word);
        if (filepos != std::string::npos) {
            std::string inhalt = trim(zeile.substr(filepos + file_word.length()));
            
            size_t startBrace = 0;
            while ((startBrace = inhalt.find('{', startBrace)) != std::string::npos) {
                size_t endBrace = inhalt.find('}', startBrace + 1);
                if (endBrace != std::string::npos) {
                    size_t totalLen = endBrace - startBrace + 1;
                    std::string varName = inhalt.substr(startBrace + 1, endBrace - startBrace - 1);
                    std::string rep = "";
                    if (varName == "projekt") rep = target.lokales_projekt;
                    else if (usrvar.count(varName)) rep = usrvar[varName];
                    
                    inhalt.replace(startBrace, totalLen, rep);
                    startBrace += rep.length();
                } else break;
            }

            size_t star_pos = inhalt.find('*');
            if (star_pos != std::string::npos && inhalt.find('.') != std::string::npos) {
                std::string gesuchte_endung = inhalt.substr(inhalt.find('.', star_pos));
                std::string such_ordner = target.lokales_projekt.empty() ? "." : target.lokales_projekt;
                bool pfad_im_dateinamen = false;

                if (star_pos > 0) {
                    std::string pfad_teil = inhalt.substr(0, star_pos);
                    size_t slash_pos = pfad_teil.find_last_of("/\\");
                    if (slash_pos != std::string::npos) {
                        such_ordner = pfad_teil.substr(0, slash_pos);
                        if (such_ordner.empty()) such_ordner = ".";
                        pfad_im_dateinamen = true;
                    }
                }
                
                std::string gesammelte_dateien = "";
                if (fs::exists(such_ordner) && fs::is_directory(such_ordner)) {
                    for (const auto &eintrag : fs::directory_iterator(such_ordner)) {
                        if (eintrag.is_regular_file() && eintrag.path().extension() == gesuchte_endung) {
                            gesammelte_dateien += (pfad_im_dateinamen ? such_ordner + "/" : "") + eintrag.path().filename().string() + " ";
                        }
                    }
                }
                if (!gesammelte_dateien.empty()) gesammelte_dateien.pop_back();
                target.lokale_files = std::move(gesammelte_dateien);
            } else {
                target.lokale_files = std::move(inhalt);
            }
            break;
        }
    }
}

bool rebuild_required(const std::string &files, const std::string &target) {
    if (!fs::exists(target)) return true;
    auto target_time = fs::last_write_time(target);
    std::string datei;
    std::stringstream ss(files);
    while (ss >> datei) {
        if (fs::exists(datei) && fs::last_write_time(datei) > target_time) return true;
    }
    return false;
}

std::future<int> compilecommand(const BuildTarget& target) {
    std::string bdcommand;
    for (const std::string &zeile : target.zeilen) {
        size_t buildpos = zeile.find(build_command);
        if (zeile.rfind("#", 0) == 0 || buildpos == std::string::npos) continue;
        bdcommand = trim(zeile.substr(buildpos + build_command.length()));
        break;
    }

    if (!rebuild_required(target.lokale_files, target.lokales_executable)) {
        return std::async(std::launch::deferred, []() { return 0; });
    }

    size_t startBrace = 0;
    while ((startBrace = bdcommand.find('{', startBrace)) != std::string::npos) {
        size_t endBrace = bdcommand.find('}', startBrace + 1);
        if (endBrace != std::string::npos) {
            size_t totalLen = endBrace - startBrace + 1;
            std::string varName = bdcommand.substr(startBrace + 1, endBrace - startBrace - 1);
            std::string rep = "";
            if (varName == "files") rep = target.lokale_files;
            else if (varName == "projekt") rep = target.lokales_projekt;
            else if (varName == "executable" || varName == "executeable") rep = target.lokales_executable;
            else if (usrvar.count(varName)) rep = usrvar[varName];

            bdcommand.replace(startBrace, totalLen, rep);
            startBrace += rep.length();
        } else break;
    }

    global_bdcommand = bdcommand;
    return std::async(std::launch::async, [bdcommand]() {
        std::cout << "[DEBUG] Executing: " << bdcommand << std::endl;
        return std::system(bdcommand.c_str());
    });
}

void executeBuildLine(const BuildTarget& target) {
    for (auto it = global_hintergrund_tasks.begin(); it != global_hintergrund_tasks.end();) {
        if (it->valid() && it->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            it->get();
            it = global_hintergrund_tasks.erase(it);
        } else ++it;
    }

    for (const std::string &zeile : target.zeilen) {
        if (zeile.rfind("#", 0) == 0) continue;
        size_t pos = zeile.find(global_suchwort);
        if (pos == std::string::npos) continue;

        std::string befehl = trim(zeile.substr(pos + global_suchwort.length()));
        size_t startBrace = 0;
        while ((startBrace = befehl.find('{', startBrace)) != std::string::npos) {
            size_t endBrace = befehl.find('}', startBrace + 1);
            if (endBrace != std::string::npos) {
                size_t totalLen = endBrace - startBrace + 1;
                std::string varName = befehl.substr(startBrace + 1, endBrace - startBrace - 1);
                std::string rep = (varName == "files") ? target.lokale_files : 
                                  (varName == "projekt") ? target.lokales_projekt : 
                                  (varName == "executable" || varName == "executeable") ? target.lokales_executable : 
                                  (usrvar.count(varName) ? usrvar[varName] : "");
                befehl.replace(startBrace, totalLen, rep);
                startBrace += rep.length();
            } else break;
        }

        if (befehl.rfind("cd ", 0) == 0) {
            for (auto &task : global_hintergrund_tasks) if (task.valid()) task.get();
            global_hintergrund_tasks.clear();
            cd(trim(befehl.substr(3)));
        } else {
            global_hintergrund_tasks.push_back(std::async(std::launch::async, [befehl]() {
                return std::system(befehl.c_str());
            }));
        }
    }
}


int main() {
    if (getfiledata() != 0) return 1;

    std::vector<BuildTarget> targets;
    BuildTarget aktuelles_target;
    bool erstes_target_gefunden = false;

    for (const std::string& roh_zeile : out) {
        std::string zeile = trim(roh_zeile);
        if (zeile.empty() || zeile.rfind("#", 0) == 0 || zeile.rfind("//", 0) == 0) continue;

        if (zeile.rfind("[target:", 0) == 0) {
            if (erstes_target_gefunden) targets.push_back(aktuelles_target);
            aktuelles_target = BuildTarget();
            size_t end_bracket = zeile.find(']');
            aktuelles_target.name = trim(zeile.substr(8, end_bracket - 8));
            erstes_target_gefunden = true;
            continue;
        }

        if (!erstes_target_gefunden && zeile.find("=") != std::string::npos) {
            size_t eq = zeile.find("=");
            usrvar[trim(zeile.substr(0, eq))] = trim(zeile.substr(eq + 1));
        } else if (erstes_target_gefunden) {
            aktuelles_target.zeilen.push_back(roh_zeile);
        }
    }
    if (erstes_target_gefunden) targets.push_back(aktuelles_target);

    
    for (auto& target : targets) {
        std::cout << "\033[36m--- Processing Target: " << target.name << " ---\033[0m" << std::endl;

        getprojekt(target);    
        executable(target);    
        if_clean(target);      
        vardec(target.zeilen);        
        filecommand(target);   

        auto mein_task = compilecommand(target);
        if (mein_task.valid()) {
            if (mein_task.get() != 0){
                std::cerr << "\033[31m[error] Target" << target.name << "faildes!\033[0m" << std::endl;
                return 1;
            }
        }
        executeBuildLine(target);
    }
    for (auto& task : global_hintergrund_tasks) if (task.valid()) task.get();
    
    std::cout << "\033[32m[success] All targets build succesfully!\033[0m" << std::endl;
    return 0;
}