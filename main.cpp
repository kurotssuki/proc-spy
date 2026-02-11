#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <fstream>

namespace fs = std::filesystem;


bool isNumber(const std::string& s);

int main() {
    std::string directoryPath = "/proc/";
    if(fs::exists(directoryPath)) {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if(entry.is_directory()) {
                std::string folderName = entry.path().filename().string();
                                               
                if(isNumber(folderName)) {
                    std::string processInfo = (entry.path() / "comm").string();
                    std::string str = "";
                    std::ifstream file(processInfo);
                    if(file.is_open()) {
                        if(std::getline(file, str)) {
                            std::cout << "[PID: " << folderName << "] " << str << std::endl;
                        } else {
                            std::cerr << "Error reading process name, empty?" << std::endl;
                        }
                   } 
                }
            }
        }
    }
    return 0;
}

bool isNumber(const std::string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

