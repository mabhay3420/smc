#ifndef UTILS_HPP
#define UTILS_HPP
#include <cstdint>
#include <ostream>
#include <string>

std::string read_file_to_string(const std::string &filename);
class Indent {
  private:
    uint32_t s_width;

  public:
    Indent(uint32_t s_width) : s_width(s_width){};

    // Addition operator to increase indentation
    Indent operator+(uint32_t additional) const {
        return Indent(s_width + additional);
    }

    // Subtraction operator to decrease indentation
    Indent operator-(uint32_t reduction) const {
        // Prevent underflow
        if (s_width <= reduction) {
            return Indent(0);
        }
        return Indent(s_width - reduction);
    }

    // Addition assignment operator
    Indent &operator+=(uint32_t additional) {
        s_width += additional;
        return *this;
    }

    // Subtraction assignment operator
    Indent &operator-=(uint32_t reduction) {
        // Prevent underflow
        if (s_width <= reduction) {
            s_width = 0;
        } else {
            s_width -= reduction;
        }
        return *this;
    }

    // Getter for indentation width
    uint32_t width() const { return s_width; }
};

// Stream insertion operator
std::ostream &operator<<(std::ostream &os, const Indent &indent);

class Line {
  public:
    Indent indent;
    std::string text;
    Line(Indent indent, std::string text) : indent(indent), text(text) {}
    Line(Indent indent, std::ostream &os) : indent(indent) {
    }
    Line(std::string text) : indent(0), text(text) {}
    Line() : indent(0), text("") {}

    // Stream insertion operator
    friend std::ostream &operator<<(std::ostream &os, const Line &line) {
        os << line.indent << line.text << "\n";
        return os;
    }
};
#endif