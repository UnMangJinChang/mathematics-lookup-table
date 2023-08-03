#include <iostream>
#include <iomanip>
#include <cmath>
#include <numbers>

constexpr std::pair<int, int> get_sci_notation(double x, int digits = 5) {
    if (x == 0.0) return std::make_pair<int, int>(0, 0);
    else {
        int significands = static_cast<int>(std::round(x * std::pow(10.0, -std::floor(std::log10(x)) + static_cast<double>(digits) - 1.0)));
        int exp = static_cast<int>(std::floor(std::log10(x)));
        int significands_max = 1;
        for (int i = 0; i < digits; i++) significands_max *= 10;
        if (significands == significands_max) {
            significands /= 10;
            ++exp;
        }
        return std::make_pair(significands, exp);
    }
}

int main(int argc, char** argv) {
    std::ios_base::fmtflags old_flags;
    constexpr double PI_2 = std::numbers::pi_v<double> / 2.0;
    std::cout << 
        "\\begin{longtable}{c || c c c c c | c c c c c}\n"
        "    \\(z\\) & 0 & 1 & 2 & 3 & 4 & 5 & 6 & 7 & 8 & 9\\\\\n"
        "    \\hline\\hline\n";
    for (int i = 0; i < 100; i++) {
        old_flags = std::cout.flags();
        std::cout << "    ""0." << std::setw(2) << std::right << std::setfill('0') << i << " & ";
        std::cout.flags(old_flags);
        for (int j = 0; j < 10; j++) {
            double x = static_cast<double>(i * 10 + j);
            x /= 1000.0;
            double y = std::sin(x * PI_2);
            std::pair<int, int> sci = get_sci_notation(y, 5);
            std::cout << "\\({" << sci.first << "}_{" << sci.second << "}\\)" << (j + 1 < 10 ? " & " : "\\\\\n");
        }
        if (i % 10 == 9) std::cout << "    \\hline\n";
    }
    std::cout << 
        "\\end{longtable}\n";
    return 0;
}