#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <vector>

struct CleanStats {
    int totalLines = 0;
    int blankLines = 0;
    int duplicateLines = 0;
    int keptLines = 0;
};

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::string trimCsvFields(const std::string& line)
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

bool readFile(const std::string& path, std::vector<std::string>& lines)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[错误] 无法打开文件 " << path << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    return true;
}

bool writeFile(const std::string& path, const std::vector<std::string>& lines)
{
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "[错误] 无法写入输出文件 " << path << std::endl;
        return false;
    }
    for (size_t i = 0; i < lines.size(); ++i) {
        file << lines[i] << std::endl;
    }
    file.close();
    return true;
}

void processCsv(const std::vector<std::string>& input,
                std::vector<std::string>& output,
                CleanStats& stats)
{
    std::unordered_set<std::string> seen;
    bool isHeader = true;

    for (size_t i = 0; i < input.size(); ++i) {
        ++stats.totalLines;
        std::string cleaned = trimCsvFields(input[i]);
        std::string trimmed = trim(cleaned);
        if (trimmed.empty()) {
            ++stats.blankLines;
            continue;
        }
        if (isHeader) {
            output.push_back(cleaned);
            isHeader = false;
            ++stats.keptLines;
            continue;
        }
        if (seen.find(trimmed) == seen.end()) {
            seen.insert(trimmed);
            output.push_back(cleaned);
            ++stats.keptLines;
        } else {
            ++stats.duplicateLines;
        }
    }
}

void printStats(const CleanStats& stats, const std::string& outPath)
{
    if (stats.keptLines == 0) {
        std::cout << "[提示] 输入文件为空，未执行清洗操作" << std::endl;
        return;
    }
    std::cout << "已清洗完成，输出文件: " << outPath << std::endl;
    std::cout << "--- 清洗统计 ---" << std::endl;
    std::cout << "原文件总行数: " << stats.totalLines << std::endl;
    std::cout << "去除空行数  : " << stats.blankLines << std::endl;
    std::cout << "去除重复行数: " << stats.duplicateLines << std::endl;
    std::cout << "最终保留行数: " << stats.keptLines << std::endl;
}

std::string buildOutputPath(const std::string& inputPath)
{
    size_t dot = inputPath.find_last_of('.');
    return (dot == std::string::npos)
        ? inputPath + "_cleaned.csv"
        : inputPath.substr(0, dot) + "_cleaned.csv";
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "[错误] 用法: csv-cleaner <CSV文件路径>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::vector<std::string> rawLines;
    if (!readFile(inputPath, rawLines)) {
        return 1;
    }

    CleanStats stats;
    std::vector<std::string> cleanedLines;
    processCsv(rawLines, cleanedLines, stats);

    std::string outputPath = buildOutputPath(inputPath);
    if (!writeFile(outputPath, cleanedLines)) {
        return 1;
    }

    printStats(stats, outputPath);
    return 0;
}
