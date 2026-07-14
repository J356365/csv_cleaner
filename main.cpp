#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <vector>

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::string trimFields(const std::string& line)
{
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) {
        fields.push_back(trim(field));
    }
    std::string result;
    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) result += ",";
        result += fields[i];
    }
    return result;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "[错误] 用法: csv-cleaner <CSV文件路径>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "[错误] 无法打开文件 " << filePath << std::endl;
        return 1;
    }

    std::string outPath = filePath.substr(0, filePath.find_last_of('.')) + "_cleaned.csv";
    std::ofstream outFile(outPath);

    if (!outFile.is_open()) {
        std::cerr << "[错误] 无法写入输出文件 " << outPath << std::endl;
        file.close();
        return 1;
    }

    std::unordered_set<std::string> seen;
    bool isHeader = true;
    bool hasData = false;
    int totalLines = 0;
    int blankLines = 0;
    int duplicateLines = 0;
    int keptLines = 0;

    std::string line;
    while (std::getline(file, line)) {
        ++totalLines;
        std::string cleaned = trimFields(line);
        std::string trimmed = trim(cleaned);
        if (trimmed.empty()) {
            ++blankLines;
            continue;
        }
        hasData = true;
        if (isHeader) {
            outFile << cleaned << std::endl;
            isHeader = false;
            ++keptLines;
            continue;
        }
        if (seen.find(trimmed) == seen.end()) {
            seen.insert(trimmed);
            outFile << cleaned << std::endl;
            ++keptLines;
        } else {
            ++duplicateLines;
        }
    }

    file.close();
    outFile.close();

    if (!hasData) {
        std::cout << "[提示] 输入文件为空，未执行清洗操作" << std::endl;
        return 0;
    }

    std::cout << "已清洗完成，输出文件: " << outPath << std::endl;
    std::cout << "--- 清洗统计 ---" << std::endl;
    std::cout << "原文件总行数: " << totalLines << std::endl;
    std::cout << "去除空行数  : " << blankLines << std::endl;
    std::cout << "去除重复行数: " << duplicateLines << std::endl;
    std::cout << "最终保留行数: " << keptLines << std::endl;
    return 0;
}
