#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <fstream>
#include <pwd.h>
#include <unistd.h>
#include <sstream>

namespace fs = std::filesystem;

class Process {
    private:
        std::string pid;
        std::string name;
        std::string cmdline;
        std::string username;
        std::string effectiveUser;

        std::string effectiveUid; // The "launcher" (real UID)
        std::string realUid; // The "Current Permissions" (Effective UID)


        //Helper: Convert UID to Username
        std::string uidToUsername(const std::string& s) {
            try {
                uid_t uid = std::stoul(s);
                if(uid == (uid_t)-1) return "unset";

                struct passwd *pw = getpwuid(uid);
                if(pw)  return std::string(pw->pw_name);

                return std::to_string(uid);
            } catch (...) {
                return "invalid uid";
            }
        }
        //Helper: Read first line of the file or the key line;
        std::string readFile(const std::string& path, const std::string key = "") {
            std::ifstream file("/proc/" + pid + "/" + path);
            std::string line;
            if(file.is_open()) {
                if(key.empty()) {
                std::getline(file, line);
                return line;
                }

                while(std::getline(file, line)) {
                    if (line.find(key) == 0) {
                        return line;
                    }
                }
            }
                return "";
        }




    public:
        //Constructor: Takes PID returns process info
        Process(const std::string& pidStr) : pid(pidStr) {
            refresh();
        }
        //Loads proc data
        void refresh() {
            //name = readFile("comm");
            cmdline = readFile("cmdline");
            std::string statusLine = readFile("status", "Uid:");
            if(!statusLine.empty()) {
                std::stringstream ss(statusLine);
                std::string label;
                ss >> label >> realUid >> effectiveUid;
                username = uidToUsername(realUid);
                effectiveUser = uidToUsername(effectiveUid); 
            }
            statusLine = readFile("status", "Name:");
            if(!statusLine.empty()) {
                std::stringstream ss(statusLine);
                std::string label = "";
                ss >> label >> name;
            }
            std::string uid = readFile("loginuid");
        }

        //Getters. are they even nessecary?
        std::string getName() { return name; }
        std::string getCmdline() { return cmdline; }
        std::string getEffectiveUser() {return effectiveUser; }

        void printInfo() {
            std::cout << "[PID: " << pid <<
                 "] [name: " << name << 
                 "] [cmdline: " << cmdline << 
                 "] [user: " << username << ']' << std::endl;
        }
};

void printHelp() {
    std::cout << "Usage ./main <-r>" << std::endl;
    std::cout << "   -r Enable only root" << std::endl;
    std::cout << "   -h Show this help message" << std::endl;
}

//Utilily: checks if directory is a PID
bool isPIDfolder(const std::string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}



int main(int  argc, char* argv[]) {
    int opt = 0;
    bool rootOnly = false;
    while ((opt = getopt(argc, argv, "rh")) != -1) {
        switch (opt) {
            case 'r':
                rootOnly = true;
                break;
            case 'h':
                printHelp();
                return 0;
                break;
            case '?':
                std::cerr << "[!] Warning: Unknown flag ignored" << std::endl;
                break;
            default:
                break;
        }
    }


    std::string procPath = "/proc/";
    if(fs::exists(procPath)) {
        for (const auto& entry : fs::directory_iterator(procPath)) {
            if(entry.is_directory()) {
                std::string folderName = entry.path().filename().string();

                if(isPIDfolder(folderName)) {
                    Process proc(folderName);
                    
                    if(rootOnly && proc.getEffectiveUser() != "root") { continue; }

                    proc.printInfo(); 
                }
               
            }
        }
    }
    return 0;
}


