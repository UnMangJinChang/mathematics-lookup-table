#ifndef UMJCUTIL_MATH_ISQRT_HPP
#define UMJCUTIL_MATH_ISQRT_HPP

#include <type_traits>
#include <stdexcept>
#include <limits>

namespace UMJCUtil {
    namespace Math {
        template <typename T>
        constexpr T isqrt(T x) {
            static_assert(std::is_fundamental<T>::value, "this template was targeted for the fundamental types");
            static_assert(std::numeric_limits<T>::is_integer, "this template was targeted for the integral types");
            if (x < T(0)) {
                throw std::domain_error("you cannot calculate square root of a negative number");
            }
            std::size_t bit_size = std::numeric_limits<T>::digits;
            if (bit_size % 2 != 0) bit_size++;
            T rtrn(0);
            T multiplier(T(1) << (bit_size - 2));
            T subtracted(x);
            for (T i = T(0); i < T(bit_size / 2); i++) {
                T j = ((rtrn << 2) + T(1)) * multiplier > subtracted ? T(0) : T(1);
                if (j) subtracted -= ((rtrn << 2) + T(1)) * multiplier;
                rtrn <<= 1;
                rtrn |= j;
                multiplier >>= 2;
            }
            return rtrn;
        }
    }
}

#endif