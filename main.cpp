#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "用法: csv-cleaner <CSV文件路径>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "错误：无法打开文件 " << filePath << std::endl;
        return 1;
    }

    std::string outPath = filePath.substr(0, filePath.find_last_of('.')) + "_cleaned.csv";
    std::ofstream outFile(outPath);

    std::unordered_set<std::string> seen;
    bool isHeader = true;

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) {
            continue;
        }
        if (isHeader) {
            outFile << line << std::endl;
            isHeader = false;
            continue;
        }
        if (seen.find(trimmed) == seen.end()) {
            seen.insert(trimmed);
            outFile << line << std::endl;
        }
    }

    file.close();
    outFile.close();

    std::cout << "已清洗完成，输出文件: " << outPath << std::endl;
    return 0;
}
