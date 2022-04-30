#include <iostream>
#include "checksum.hpp"

int main(int ac, char* av[]) {
    if (ac < 2) {
        std::cout << "Usage: \"checksum <file>\"\n";
        return 1;
    }
    std::cout << "Checksum: " << calculateChecksum(av[1]) << "\n";
    return 0;
}
