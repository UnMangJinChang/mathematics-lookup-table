#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>

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

double lower_gamma(double s, double x) {
    double term = 1.0;
    double rtrnv = 0.0;
    long idx = 0;
    while (term >= std::numeric_limits<double>::epsilon()) {
        term /= (static_cast<double>(idx) + s);
        rtrnv += term;
        term *= x;
        ++idx;
    }
    rtrnv *= std::pow(x, s) * std::exp(-x);
    return rtrnv;
}

double get_chi2_percentile_x(int freedom, double percentile_fraction) {
    double rtrnv = std::max<double>(freedom - 2, std::numeric_limits<double>::epsilon());
    double gamma_freedom_over_2 = std::tgamma(0.5 * static_cast<double>(freedom));
    int cnt = 0;
    while (cnt < 500) {
        double new_rtrnv;
        double delta = lower_gamma(freedom * 0.5, rtrnv * 0.5);
        delta -= gamma_freedom_over_2 * percentile_fraction;
        delta /= std::pow(rtrnv * 0.5, freedom * 0.5 - 1) * std::exp(-rtrnv * 0.5) * 0.5;
        new_rtrnv = rtrnv - delta;
        if (std::abs(new_rtrnv - rtrnv) < std::numeric_limits<double>::epsilon()) break;
        rtrnv = new_rtrnv;
        cnt++;
    }
    return rtrnv;
}

int main() {
    constexpr double SIGNIFICANCE_LEVEL[] = {0.005, 0.01, 0.025, 0.05, 0.1, 0.9, 0.95, 0.975, 0.99, 0.995};
    std::ios_base::fmtflags old_flags;
    std::cout << 
        "\\begin{longtable}{c || c c c c c | c c c c c}\n"
        "    \\(n\\) & .005 & .010 & .025 & .050 & .100 & .900 & .950 & .975 & .990 & .995\\\\\n"
        "    \\hline\\hline\n";
    for (int i = 1; i <= 40; i++) {
        std::cout << "    " << i << " & ";
        for (int j = 0; j < 10; j++) {
            double y = get_chi2_percentile_x(i, 1.0 - SIGNIFICANCE_LEVEL[j]);
            std::pair<int, int> sci = get_sci_notation(y, 5);
            old_flags = std::cout.flags();
            std::cout << "\\({" << sci.first << "}_{" << std::showpos << sci.second << std::noshowpos << "}\\)" << (j + 1 < 10 ? " & " : "\\\\\n");
            std::cout.flags(old_flags);
        }
        if (i % 5 == 0) std::cout << "    \\hline\n";
    }
    std::cout << 
        "\\end{longtable}\n";
    return 0;
    return 0;
}