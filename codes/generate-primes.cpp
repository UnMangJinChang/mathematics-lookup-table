#include <iostream>
#include "primes.hpp"

int main(int argc, char* argv[]) {
    int x = 2;
    int primes = 0;
    std::cout << 
        "\\begin{longtable}{c c c c c c c c c c c c c c}\n"
        "    \\hline\n";
    while (primes < 14 * 90) {
        if (UMJCUtil::Math::Primes<int>::is_prime(x)) {
            primes++;
            if (primes % 14 == 1) std::cout << "    ";
            std::cout << x << " ";
            if (primes % 14 == 0) {
                std::cout << "\\\\\n";
                if (primes % 140 == 0) std::cout << "    \\hline\n";
            }
            else std::cout << "& ";
        }
        x++;
    }
    std::cout << 
        "\\end{longtable}\n";
    return 0;
}