#ifndef BASE_HPP
#define BASE_HPP

// Define NDEBUG here when release

//

#ifndef NDEBUG
constexpr const bool debug = true;
#else
constexpr const bool debug = false;
#endif

#endif //BASE_HPP
