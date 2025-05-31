#include "utils.hpp"
#include <fstream>
#include <sstream>

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

void dump_string_to_file(const std::string &filename,
                         const std::string &content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    file << content;
    file.close();
    if (file.fail()) {
        throw std::runtime_error("Failed to write to file: " + filename);
    }
}

void dump_json_to_file(const std::string &filename, const nlohmann::json &j) {
    std::stringstream ss;
    ss << j.dump(4); // Pretty print with 4 spaces
    dump_string_to_file(filename, ss.str());
}

// Stream insertion operator
std::ostream &operator<<(std::ostream &os, const Indent &indent) {
    // Output the appropriate number of spaces or tabs based on the width
    for (uint32_t i = 0; i < indent.width(); ++i) {
        for (uint8_t j = 0; j < indent.getTabSize(); ++j)
            os << " ";
    }
    return os;
}