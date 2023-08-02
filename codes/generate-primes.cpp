#include <iostream>
#include "primes.hpp"

int main(int argc, char* argv[]) {
    int x = 2;
    int primes = 0;
    std::cout   << "\\begin{longtable}{c c c c c c c c c c c c c c c}\n"
                << "    \\hline\n";
    while (primes < 10 * 300) {
        if (umjc_util::math::Primes<int>::is_prime(x)) {
            primes++;
            if (primes % 12 == 1) std::cout << "    ";
            std::cout << x << " ";
            if (primes % 12 == 0) std::cout << "\\\\\n";
            else std::cout << "& ";
        }
        x++;
    }
    std::cout   << "    \\hline\n"
                << "\\end{longtable}\n";
    return 0;
}