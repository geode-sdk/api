#pragma once

#include <string>
#include <fstream>

static uint32_t calculateChecksum(std::string const& path) {
    // https://codereview.stackexchange.com/questions/104948/32-bit-checksum-of-a-file
    std::ifstream file(path);
    if (file.is_open()) {
        uint32_t sum = 0;
        uint32_t word = 0;
        while (file.read(reinterpret_cast<char*>(&word), sizeof(word))) {
            sum += word;
        }
        if (file.gcount()) {
            word &= (~0U >> ((sizeof(uint32_t) - file.gcount()) * 8));
            sum += word;
        }
        return sum;
    }
    return 0;
}
