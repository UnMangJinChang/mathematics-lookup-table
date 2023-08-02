#include <iostream>
#include <iomanip>
#include <cmath>
#include <cfloat>

constexpr std::pair<int, int> get_sci_notation(double x, int digits = 5) {
    if (x == 0.0) return std::make_pair<int, int>(0, 0);
    else return std::make_pair<int, int>(
        std::round(x * std::pow(10.0, -std::floor(std::log10(x)) + static_cast<double>(digits) - 1.0)), 
        std::floor(std::log10(x))
    );
}

int main(int argc, char** argv) {
    std::cout   << "\\begin{longtable}{c || c c c c c | c c c c c}\n"
                << "    \\(x\\) & 0 & 1 & 2 & 3 & 4 & 5 & 6 & 7 & 8 & 9\\\\\n"
                << "    \\hline\\hline\n";
    for (int i = 10; i < 100; i++) {
        std::cout << "    " << (i / 10) << "." << (i % 10) << " & ";
        for (int j = 0; j < 10; j++) {
            double x = static_cast<double>(i * 10 + j);
            x /= 100.0;
            double y = std::log10(x);
            std::pair<int, int> sci = get_sci_notation(y, 5);
            std::cout << "\\({" << sci.first << "}_{" << sci.second << "}\\)" << (j + 1 < 10 ? " & " : "\\\\\n");
        }
    }
    std::cout   << "\\end{longtable}\n";
    return 0;
}