#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <fstream>
#include <pwd.h>
#include <unistd.h>

namespace fs = std::filesystem;

class Process {
    private:
        std::string pid;
        std::string name;
        std::string username;
        std::string cmdline;

        //Helper: Convert UID to Username
        std::string uidToUsername(const std::string& s) {
            try {
                uid_t uid = std::stoul(s);
                if(uid == (uid_t)-1) return "unset";

                struct passwd *pw = getpwuid(uid);
                if(pw)  return std::string(pw->pw_name);                      
            } catch (...) {
                return "invalid uid";
            }
        }
        //Helper: Read first line of a file;
        std::string readFile(const std::string& path) {
            std::ifstream file("/proc/" + pid + "/" + path);
            std::string line;
            if(file.is_open()) {
                std::getline(file, line);
            }
            return line;
        }




    public:
        //Constructor: Takes PID returns process info
        Process(const std::string& pidStr) : pid(pidStr) {
            refresh();
        }
        //Loads proc data
        void refresh() {
            name = readFile("comm");
            cmdline = readFile("cmdline");

            std::string uid = readFile("loginuid");
            username = uidToUsername(uid);
        }
        void printInfo() const {
            std::cout << "[PID: " << pid <<
                 "] [name: " << name << 
                 "] [cmdline: " << cmdline << 
                 "] [user: " << username << ']' << std::endl;
        }
};


//Utilily: checks if directory is a PID
bool isPIDfolder(const std::string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}



int main() {
    std::string procPath = "/proc/";
    if(fs::exists(procPath)) {
        for (const auto& entry : fs::directory_iterator(procPath)) {
            if(entry.is_directory()) {
                std::string folderName = entry.path().filename().string();

                if(isPIDfolder(folderName)) {
                    Process proc(folderName);
                    proc.printInfo();
                }
               
            }
        }
    }
    return 0;
}


