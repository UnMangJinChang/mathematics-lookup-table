#ifndef UMJCUTIL_MATH_ISQRT_HPP
#define UMJCUTIL_MATH_ISQRT_HPP

#include <type_traits>
#include <stdexcept>
#include <limits>

namespace umjc_util {
    namespace math {
        template <typename t>
        constexpr t isqrt(t x) {
            static_assert(std::is_fundamental<t>::value, "this template was targeted for the fundamental types");
            static_assert(std::numeric_limits<t>::is_integer, "this template was targeted for the integral types");
            if (x < t(0)) {
                throw std::domain_error("you cannot calculate square root of a negative number");
            }
            std::size_t bit_size = std::numeric_limits<t>::digits;
            if (bit_size % 2 != 0) bit_size++;
            t rtrn(0);
            t multiplier(t(1) << (bit_size - 2));
            t subtracted(x);
            for (t i = t(0); i < t(bit_size / 2); i++) {
                t j = ((rtrn << 2) + t(1)) * multiplier > subtracted ? t(0) : t(1);
                if (j) subtracted -= ((rtrn << 2) + t(1)) * multiplier;
                rtrn <<= 1;
                rtrn |= j;
                multiplier >>= 2;
            }
            return rtrn;
        }
    }
}

#endif