#include <fstream>
#include <iostream>

void write_to_file(std::string result, std::string filename) {
    std::ofstream outputFile(filename, std::ios::app);
    if (outputFile.is_open()) {
        outputFile << result << std::endl;
        outputFile.close();
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
}