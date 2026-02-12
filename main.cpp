#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <fstream>
#include <pwd.h>
#include <unistd.h>

namespace fs = std::filesystem;

bool isNumber(const std::string& s);
std::string uidToUsername(const std::string& s);


int main() {
    std::string directoryPath = "/proc/";
    if(fs::exists(directoryPath)) {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if(entry.is_directory()) {
                std::string folderName = entry.path().filename().string();
                                               
                if(isNumber(folderName)) {
                    std::string commPath = (entry.path() / "comm").string();
                    std::string cmdlinePath = (entry.path() / "cmdline").string();
                    std::string loginuidPath = (entry.path() / "loginuid").string();
                    std::string comm, cmdline, loginuid;

                    std::ifstream commFile(commPath);
                    std::getline(commFile, comm);
                    
                    std::ifstream cmdlineFile(cmdlinePath);
                    std::getline(cmdlineFile, cmdline);

                    std::ifstream loginuidFile(loginuidPath);
                    std::getline(loginuidFile, loginuid);
                    
//need to implement loginuid to username here...
                    std::string username = uidToUsername(loginuid);

                    std::cout << "[PID: " << folderName << "] [" << 
                    comm << "] " << username << std::endl;  
               }
            }
        }
    }
    return 0;
}

bool isNumber(const std::string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}
std::string uidToUsername(const std::string& s) {
    try {
        uid_t uid = std::stoul(s);
        if(uid == (uid_t)-1) {
            return "unset";
        }
        struct passwd *pw = getpwuid(uid);
        if(pw) {
            return std::string(pw->pw_name);
        }
    } catch (...) {
        return "invalid uid";
    }
}
