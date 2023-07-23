#pragma once

#include <fstream>
#include <iostream>

void write_to_file(std::string content, std::string filename, bool append = true) {
    std::ofstream file;
    if (append) {
        file.open(filename, std::ios_base::out | std::ios_base::app);
    } else {
        file.open(filename, std::ios_base::out | std::ios_base::trunc);
    }

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    file << content;
    file.close();
}