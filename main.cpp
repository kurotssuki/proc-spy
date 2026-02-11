#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

bool isNumber(const std::string& s);

int main() {
    std::string directoryPath = "/proc/";
    if(fs::exists(directoryPath)) {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if(entry.is_directory()) {
                std::string folderName = entry.path().filename().string();
            
                if(isNumber(folderName)) {
                    std::cout << folderName << std::endl;
                }
            }
        }
    }    
}

bool isNumber(const std::string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}
