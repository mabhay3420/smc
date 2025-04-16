#include "utils.hpp"
#include <fstream>

// Read entire file into a string
std::string read_file_to_string(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Read entire file content into a string
    return std::string(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
}

// Stream insertion operator
std::ostream &operator<<(std::ostream &os, const Indent &indent) {
    // Output the appropriate number of spaces or tabs based on the width
    for (uint32_t i = 0; i < indent.width(); ++i) {
        os << "    "; // 4 spaces per indentation level
        // Alternative: os << "\t"; // tab per indentation level
    }
    return os;
}