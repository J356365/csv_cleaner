#include <iostream>
#include <fstream>
#include <string>

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

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
    return 0;
}
