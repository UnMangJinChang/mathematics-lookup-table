#ifndef UMJCUTIL_MATH_PRIMES_HPP
#define UMJCUTIL_MATH_PRIMES_HPP

#include <vector>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <thread>
#include "isqrt.hpp"

namespace umjc_util {
    namespace math {
        /**
         * @brief thread-safe prime finding class
         * @tparam t: primitive integer type but 'bool'
        */
        template <typename t>
        class Primes {
        static_assert(std::is_fundamental<t>::value, "this template was targeted for the fundamental types");
        static_assert(std::numeric_limits<t>::is_integer, "this template was targeted for the integral types");
        static_assert(!std::is_same<t, bool>::value, "how do you find primes on the boolean ring?");
        private:
            //to enhance prime finding algorithm, it collects the primes searched before that could possibly divide x.
            static std::vector<t> prefound;
            static t searched_limit;
            static std::mutex mutex;
            static void ensure_primes_up_to(t val) {
                if (searched_limit < val) {
                    std::lock_guard<std::mutex> mutex_lock(mutex);
                    if (searched_limit % t(2) != t(0)) searched_limit++;
                    for (t i = searched_limit + t(1); i <= val; i += 2) {
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
            static bool is_prime(t x) {
                if (x <= t(0)) {
                    return false; //negative prime is not a thing.
                }
                if (x == t(1)) {
                    return false;
                }
                if (x == t(2)) {
                    return true;
                }
                if (x == t(3)) {
                    return true;
                }
                if (x == t(5)) {
                    return true;
                }
                if (x == t(7)) {
                    return true;
                }
                if (x % t(2) == 0) {
                    return false;
                }
                if (x % t(5) == 0) {
                    return false;
                }
                //만일 x가 작아 이미 찾은 최대 소수보다도 작다면 이진 탐색 사용
                if (x <= searched_limit) {
                    return std::binary_search(prefound.begin(), prefound.end(), x);
                }
                t root = isqrt(x);
                //x의 제곱근이 현재 기억하고 있는 최대 소수보다 작다면, 바로 나눗셈 가능 여부 확인
                if (root <= searched_limit) {
                    return std::all_of(prefound.begin(), prefound.end(), [x](t found_prime){return x % found_prime != 0;});
                }
                ensure_primes_up_to(root);
                return std::all_of(prefound.begin(), prefound.end(), [x](t found_prime){return x % found_prime != 0;});
            }
        
            static std::vector<std::pair<t, int>> factor(t x) {
                if (x <= t(0)) throw std::domain_error("nonpositive numbers cannot be factored");
                if (x == t(1)) return { 1 };
                if (is_prime(x)) return {1, x};
                ensure_primes_up_to(isqrt(x));
                std::vector<t> rtrn = {};
                for (std::size_t i = 0; i < prefound.size(); ) {
                    if (x % prefound[i] == 0) {
                        if (rtrn.empty() || rtrn.back().first != prefound[i]) rtrn.push_back({prefound[i], 1});
                        else if (rtrn.back().first == prefound[i]) rtrn.back().second++;
                        x /= prefound[i];
                        if (x == t(1)) break;
                    }
                    else {
                        i++;
                    }
                }
                return rtrn;
            }

            static std::size_t pi(t n) {
                if (n <= t(1)) {
                    return 0;
                }
                if (n <= searched_limit) {
                    return static_cast<std::size_t>(std::distance(prefound.begin(), std::upper_bound(prefound.begin(), prefound.end(), n)));
                }
                else {
                    //ensure primes under floor(isqrt(n))
                    t root = isqrt(n);
                    ensure_primes_up_to(root);
                    std::vector<std::size_t> additional_founds = {};
                    //계산해야 하는 양이 클 경우 스레드 사용
                    t thread_count;
                    t loop_count = (n - searched_limit) / 2;
                    //10,000개 아래로는 스레드 1개
                    if (loop_count < t(10'000)) thread_count = 1;
                    //100,000개 아래로는 스레드 4개
                    else if (loop_count < t(100'000)) thread_count = 4;
                    //1,000,000개 아래로는 스레드 16개
                    else if (loop_count < t(1'000'000)) thread_count = 16;
                    //10,000,000개 아래로는 스레드 64개
                    else if (loop_count < t(10'00'000)) thread_count = 64;
                    //그 이상으로는 스레드 256개
                    else thread_count = 256;
                    additional_founds.resize(thread_count, 0);
                    if ((n - searched_limit) % thread_count != 0) additional_founds.push_back(0);
                    auto thread_func = [](t begin, t end, std::size_t* rtrn) {
                        begin = begin % 2 == 0 ? begin + 1 : begin;
                        for (t i = begin; i <= end; i += 2) {
                            t root = isqrt(i);
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
                    for (t i = 0; i < thread_count; i++) {
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
            
            static const std::vector<t>& get_prefounds() {
                return prefound;
            }
        };
        template <typename t>
        std::vector<t> Primes<t>::prefound = { t(2), t(3), t(5), t(7) };
        template <typename t>
        t Primes<t>::searched_limit = t(7);
        template <typename t>
        std::mutex Primes<t>::mutex;

        //constexpr version of is_prime so you can ensure prime numbers for template parameters.
        //to do more efficient prime verification on runtime, consider using primes<t>::is_prime().
        template <typename t>
        constexpr bool is_prime(t number) {
            static_assert(std::is_fundamental<t>::value, "this template was targeted for the fundamental types");
            static_assert(std::numeric_limits<t>::is_integer, "this template was targeted for the integral types");
            static_assert(!std::is_same<t, bool>::value, "how do you find primes on the boolean ring?");
            if (number <= t(0)) {
                return false; //negative prime is not a thing.
            }
            if (number == t(1)) {
                return false;
            }
            if (number == t(2)) {
                return true;
            }
            if (number == t(3)) {
                return true;
            }
            if (number == t(5)) {
                return true;
            }
            if (number == t(7)) {
                return true;
            }
            if (number % t(2) == 0) {
                return false;
            }
            if (number % t(5) == 0) {
                return false;
            }
            //constexpr 함수이기 때문에 소수를 따로 저장하지 못하므로 isqrt(n) 이하의 모든 자연수에 대해서 확인한다.
            t loop_length = isqrt(number);
            for (t i = 3; i <= loop_length; i += 2) {
                if (number % i == 0) return false;
            }
            return true;
        }
    }
}

#endif