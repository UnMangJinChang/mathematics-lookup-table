#ifndef UMJCUTIL_MATH_PRIMES_HPP
#define UMJCUTIL_MATH_PRIMES_HPP

#include <vector>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <thread>
#include "isqrt.hpp"

namespace UMJCUtil {
    namespace Math {
        /**
         * @brief thread-safe prime finding class
         * @tparam t: primitive integer type but 'bool'
        */
        template <typename T>
        class Primes {
        static_assert(std::is_fundamental<T>::value, "this template was targeted for the fundamental types");
        static_assert(std::numeric_limits<T>::is_integer, "this template was targeted for the integral types");
        static_assert(!std::is_same<T, bool>::value, "how do you find primes on the boolean ring?");
        private:
            //to enhance prime finding algorithm, it collects the primes searched before that could possibly divide x.
            static std::vector<T> prefound;
            static T searched_limit;
            static std::mutex mutex;
            static void ensure_primes_up_to(T val) {
                if (searched_limit < val) {
                    std::lock_guard<std::mutex> mutex_lock(mutex);
                    if (searched_limit % T(2) != T(0)) searched_limit++;
                    for (T i = searched_limit + T(1); i <= val; i += 2) {
                        bool i_is_prime = true;
                        for (auto n : prefound) {
                            if (i % n == 0) {
                                i_is_prime = false;
                                break;
                            }
                        }
                        if (i_is_prime) {
                            prefound.push_back(i);
                        }
                    }
                    searched_limit = val;
                }
            }
        public:
            static bool is_prime(T x) {
                if (x <= T(0)) {
                    return false; //negative prime is not a thing.
                }
                if (x == T(1)) {
                    return false;
                }
                if (x == T(2)) {
                    return true;
                }
                if (x == T(3)) {
                    return true;
                }
                if (x == T(5)) {
                    return true;
                }
                if (x == T(7)) {
                    return true;
                }
                if (x % T(2) == 0) {
                    return false;
                }
                if (x % T(5) == 0) {
                    return false;
                }
                //만일 x가 작아 이미 찾은 최대 소수보다도 작다면 이진 탐색 사용
                if (x <= searched_limit) {
                    return std::binary_search(prefound.begin(), prefound.end(), x);
                }
                T root = isqrt(x);
                //x의 제곱근이 현재 기억하고 있는 최대 소수보다 작다면, 바로 나눗셈 가능 여부 확인
                if (root <= searched_limit) {
                    return std::all_of(prefound.begin(), prefound.end(), [x](T found_prime){return x % found_prime != 0;});
                }
                ensure_primes_up_to(root);
                return std::all_of(prefound.begin(), prefound.end(), [x](T found_prime){return x % found_prime != 0;});
            }
        
            static std::vector<std::pair<T, int>> factor(T x) {
                if (x <= T(0)) throw std::domain_error("nonpositive numbers cannot be factored");
                if (x == T(1)) return { 1 };
                if (is_prime(x)) return {1, x};
                ensure_primes_up_to(isqrt(x));
                std::vector<T> rtrn = {};
                for (std::size_t i = 0; i < prefound.size(); ) {
                    if (x % prefound[i] == 0) {
                        if (rtrn.empty() || rtrn.back().first != prefound[i]) rtrn.push_back({prefound[i], 1});
                        else if (rtrn.back().first == prefound[i]) rtrn.back().second++;
                        x /= prefound[i];
                        if (x == T(1)) break;
                    }
                    else {
                        i++;
                    }
                }
                return rtrn;
            }

            static std::size_t pi(T n) {
                if (n <= T(1)) {
                    return 0;
                }
                if (n <= searched_limit) {
                    return static_cast<std::size_t>(std::distance(prefound.begin(), std::upper_bound(prefound.begin(), prefound.end(), n)));
                }
                else {
                    //ensure primes under floor(isqrt(n))
                    T root = isqrt(n);
                    ensure_primes_up_to(root);
                    std::vector<std::size_t> additional_founds = {};
                    //계산해야 하는 양이 클 경우 스레드 사용
                    T thread_count;
                    T loop_count = (n - searched_limit) / 2;
                    //10,000개 아래로는 스레드 1개
                    if (loop_count < T(10'000)) thread_count = 1;
                    //100,000개 아래로는 스레드 4개
                    else if (loop_count < T(100'000)) thread_count = 4;
                    //1,000,000개 아래로는 스레드 16개
                    else if (loop_count < T(1'000'000)) thread_count = 16;
                    //10,000,000개 아래로는 스레드 64개
                    else if (loop_count < T(10'00'000)) thread_count = 64;
                    //그 이상으로는 스레드 256개
                    else thread_count = 256;
                    additional_founds.resize(thread_count, 0);
                    if ((n - searched_limit) % thread_count != 0) additional_founds.push_back(0);
                    auto thread_func = [](T begin, T end, std::size_t* rtrn) {
                        begin = begin % 2 == 0 ? begin + 1 : begin;
                        for (T i = begin; i <= end; i += 2) {
                            T root = isqrt(i);
                            bool i_is_prime = true;
                            for (auto k : prefound) {
                                if (k > root) break;
                                if (i % k == 0) {
                                    i_is_prime = false;
                                    break;
                                }
                            }
                            if (i_is_prime) (*rtrn)++;
                        }
                    };
                    std::vector<std::thread> thread_pool;
                    for (T i = 0; i < thread_count; i++) {
                        thread_pool.push_back(std::move(std::thread(
                            thread_func, searched_limit + i * ((n - searched_limit) / thread_count) + 1, searched_limit + (i + 1) * ((n - searched_limit) / thread_count), &additional_founds[i]
                        )));
                    }
                    if ((n - searched_limit) % thread_count != 0) {
                        thread_pool.push_back(std::move(std::thread(
                            thread_func, searched_limit + thread_count * ((n - searched_limit) / thread_count) + 1, n, &additional_founds[thread_count]
                        )));
                    }
                    for (auto& th : thread_pool) {
                        th.join();
                    }
                    return std::accumulate(additional_founds.begin(), additional_founds.end(), std::size_t(0)) + prefound.size();
                }
            }
            
            static const std::vector<T>& get_prefounds() {
                return prefound;
            }
        };
        template <typename T>
        std::vector<T> Primes<T>::prefound = { T(2), T(3), T(5), T(7) };
        template <typename T>
        T Primes<T>::searched_limit = T(7);
        template <typename T>
        std::mutex Primes<T>::mutex;

        //constexpr version of is_prime so you can ensure prime numbers for template parameters.
        //to do more efficient prime verification on runtime, consider using primes<t>::is_prime().
        template <typename T>
        constexpr bool is_prime(T number) {
            static_assert(std::is_fundamental<T>::value, "this template was targeted for the fundamental types");
            static_assert(std::numeric_limits<T>::is_integer, "this template was targeted for the integral types");
            static_assert(!std::is_same<T, bool>::value, "how do you find primes on the boolean ring?");
            if (number <= T(0)) {
                return false; //negative prime is not a thing.
            }
            if (number == T(1)) {
                return false;
            }
            if (number == T(2)) {
                return true;
            }
            if (number == T(3)) {
                return true;
            }
            if (number == T(5)) {
                return true;
            }
            if (number == T(7)) {
                return true;
            }
            if (number % T(2) == 0) {
                return false;
            }
            if (number % T(5) == 0) {
                return false;
            }
            //constexpr 함수이기 때문에 소수를 따로 저장하지 못하므로 isqrt(n) 이하의 모든 자연수에 대해서 확인한다.
            T loop_length = isqrt(number);
            for (T i = 3; i <= loop_length; i += 2) {
                if (number % i == 0) return false;
            }
            return true;
        }
    }
}

#endif