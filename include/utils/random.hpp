#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>
#include <ctime>

namespace utils
{
    /**
     * Substitute specific uniform distribution whether for real
     * or for integral type
     */
    template<class T>
    using uniform_distribution = typename std::conditional_t<
            std::is_floating_point_v<T>,
            std::uniform_real_distribution<T>,
            typename std::conditional_t<std::is_integral_v<T>,
                    std::uniform_int_distribution<T>,
                    void>>;

    class Random
    {
    public:
        explicit Random() : m_generator(std::random_device()()){}

        /**
         * Fill sequence with unique elements
         * fix_near parameter controls whether near elements
         * can differ by 1 from each other.
         * @tparam T
         * @tparam ForwardIterator
         * @param begin
         * @param end
         * @param left
         * @param right
         * @param fix_near
         */
        template<typename T, class ForwardIterator,
                typename = std::enable_if_t<std::is_arithmetic_v<T>>>
        void fill_unique(ForwardIterator begin, ForwardIterator end, T left,
                         T right, bool fix_near = false)
        {
            std::generate(begin, end,
                          [begin, end, left, right, fix_near, this]() {
                              T val = generateu<T>(left, right);
                              if (fix_near) {
                                  while (std::count(begin, end, val) != 0
                                         || std::count(begin, end, val - 1) != 0
                                         || std::count(begin, end, val + 1) != 0)
                                      val = generateu<T>(left, right);
                              } else {
                                  while (std::count(begin, end, val) != 0)
                                      val = generateu<T>(left, right);
                              }

                              return val;
                          });
        }

        /**
         * Fill sequence with gaussian distributed numbers
         * @tparam T
         * @tparam ForwardIterator
         * @param begin
         * @param end
         * @param mean
         * @param deviation
         */
        template<typename T, class ForwardIterator,
                typename = std::enable_if_t<std::is_floating_point_v<T>>>
        void fill_gauss(ForwardIterator begin, ForwardIterator end, T mean,
                        T deviation)
        {
            std::generate(begin, end, [mean, deviation, this]() {
                return generaten<T>(mean, deviation);
            });
        }

        /**
         * Fill sequence with uniform distributed numbers
         * @tparam T
         * @tparam ForwardIterator
         * @param begin
         * @param end
         * @param left
         * @param right
         */
        template<typename T, class ForwardIterator,
                typename = std::enable_if_t<std::is_arithmetic_v<T>>>
        void fill(ForwardIterator begin, ForwardIterator end, T left, T right)
        {
            std::generate(begin, end, [left, right, this]() {
                return generateu<T>(left, right);
            });
        }

        /**
         * Generate number of type T with uniform distribution
         * in range [a, b]
         * @tparam T
         * @param a
         * @param b
         * @return T
         */
        template<typename T>
        typename std::enable_if<std::is_arithmetic_v<T>, T>::type
        generateu(T a, T b)
        {
            uniform_distribution<T> dist(a, b);
            return dist(m_generator);
        }

        /**
         * Generate number of type T with gaussian distribution
         * @tparam T
         * @param mean - mean value
         * @param std - standard deviation
         * @return
         */
        template<typename T>
        typename std::enable_if<std::is_floating_point_v<T>, T>::type
        generaten(T mean, T std)
        {
            std::normal_distribution<T> dist(mean, std);
            return dist(m_generator);
        }

    private:
        std::mt19937 m_generator;
    };
}

#endif //RANDOM_HPP
