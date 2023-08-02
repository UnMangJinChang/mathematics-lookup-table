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
    const double SQRT_2 = std::sqrt(2);
    std::cout   << "\\begin{longtable}{c || c c c c c | c c c c c}\n"
                << "    \\(z\\) & 0 & 1 & 2 & 3 & 4 & 5 & 6 & 7 & 8 & 9\\\\\n"
                << "    \\hline\\hline\n";
    for (int i = 0; i < 40; i++) {
        std::cout << "    " << (i / 10) << "." << (i % 10) << " & ";
        for (int j = 0; j < 10; j++) {
            double x = static_cast<double>(i * 10 + j);
            x /= 100.0;
            double y = 0.5 * std::erf(x / SQRT_2);
            std::pair<int, int> sci = get_sci_notation(y, 5);
            std::cout << "\\({" << sci.first << "}_{" << sci.second << "}\\)" << (j + 1 < 10 ? " & " : "\\\\\n");
        }
    }
    std::cout   << "\\end{longtable}\n";
    return 0;
}