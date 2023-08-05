#include <iostream>
#include <iomanip>
#include <gmpxx.h>

int main(void) {
    std::cout << 
        "\\begin{longtable}{l | r}\n"
        "    \\(n\\) & \\(n!\\)\\\\\n"
        "    \\hline\\hline\n";
    for (int i = 1; i < 50; i++) {
        mpz_class fact = factorial(static_cast<mpz_class>(i));
        std::string fact_str = fact.get_str();
        std::cout << "    " << i << " & ";
        for (std::size_t j = 0; j < fact_str.length(); j++) {
            std::cout << fact_str[j];
            if ((fact_str.length() - j) % 5 == 1) std::cout << " ";
        }
        std::cout << "\\\\\n";
        if (i % 5 == 0) std::cout << "    \\hline\n";
    }
    std::cout << "\\end{longtable}";
    return 0;
}