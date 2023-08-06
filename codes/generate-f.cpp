#include <iostream>
#include <iostream>
#include <cmath>
#include <boost/math/distributions.hpp>

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

int main() {
    constexpr double SIGNIFICANCE_LEVEL[] = {0.001, 0.005, 0.010, 0.025, 0.05};
    std::ios_base::fmtflags old_flags;
    for (int k = 0; k < 5; k++) {
        std::cout << 
            "\\(\\alpha\\)=" << SIGNIFICANCE_LEVEL[k] << "\n\n";
        for (int p = 0; p < 3; p++) {
            std::cout << 
                "\\begin{tabular}{c || c c c c c | c c c c c}\n"
                "    \\multirow{2}{*}{\\(r_2\\)} & \\multicolumn{10}{c}{\\(r_1\\)} \\\\\n"
                "    & ";
            for (int j = 0; j < 10; j++) {
                std::cout << (j + 10 * p + 1) << (j + 1 < 10 ? " & " : "\\\\\n");
            }
            std::cout << 
                "    \\hline\\hline\n";
            for (int i = 1; i <= 30; i++) {
                std::cout << "    " << i << " & ";
                for (int j = 1; j <= 10; j++) {
                    double y = boost::math::quantile(boost::math::fisher_f(j + 10 * p, i), 1.0 - SIGNIFICANCE_LEVEL[k]);
                    std::pair<int, int> sci = get_sci_notation(y, 5);
                    old_flags = std::cout.flags();
                    std::cout << "\\({" << sci.first << "}_{" << std::showpos << sci.second << std::noshowpos << "}\\)" << (j + 1 <= 10 ? " & " : "\\\\\n");
                    std::cout.flags(old_flags);
                }
                if (i % 5 == 0) std::cout << "    \\hline\n";
            }
            std::cout << 
                "\\end{tabular}\n"
                "\\pagebreak\n";
        }
    }
    return 0;
}