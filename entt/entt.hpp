// #include "config/version.h"
#ifndef ENTT_CONFIG_VERSION_H
#define ENTT_CONFIG_VERSION_H


#define ENTT_VERSION_MAJOR 3
#define ENTT_VERSION_MINOR 8
#define ENTT_VERSION_PATCH 0


#endif

// #include "core/algorithm.hpp"
#ifndef ENTT_CORE_ALGORITHM_HPP
#define ENTT_CORE_ALGORITHM_HPP


#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>
// #include "utility.hpp"
#ifndef ENTT_CORE_UTILITY_HPP
#define ENTT_CORE_UTILITY_HPP


#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/*! @brief Identity function object (waiting for C++20). */
struct identity {
    /**
     * @brief Returns its argument unchanged.
     * @tparam Type Type of the argument.
     * @param value The actual argument.
     * @return The submitted value as-is.
     */
    template<class Type>
    [[nodiscard]] constexpr Type && operator()(Type &&value) const ENTT_NOEXCEPT {
        return std::forward<Type>(value);
    }
};


/**
 * @brief Constant utility to disambiguate overloaded members of a class.
 * @tparam Type Type of the desired overload.
 * @tparam Class Type of class to which the member belongs.
 * @param member A valid pointer to a member.
 * @return Pointer to the member.
 */
template<typename Type, typename Class>
[[nodiscard]] constexpr auto overload(Type Class:: *member) ENTT_NOEXCEPT { return member; }


/**
 * @brief Constant utility to disambiguate overloaded functions.
 * @tparam Func Function type of the desired overload.
 * @param func A valid pointer to a function.
 * @return Pointer to the function.
 */
template<typename Func>
[[nodiscard]] constexpr auto overload(Func *func) ENTT_NOEXCEPT { return func; }


/**
 * @brief Helper type for visitors.
 * @tparam Func Types of function objects.
 */
template<class... Func>
struct overloaded: Func... {
    using Func::operator()...;
};


/**
 * @brief Deduction guide.
 * @tparam Func Types of function objects.
 */
template<class... Func>
overloaded(Func...)
-> overloaded<Func...>;


/**
 * @brief Basic implementation of a y-combinator.
 * @tparam Func Type of a potentially recursive function.
 */
template<class Func>
struct y_combinator {
    /**
     * @brief Constructs a y-combinator from a given function.
     * @param recursive A potentially recursive function.
     */
    y_combinator(Func recursive):
        func{std::move(recursive)}
    {}

    /**
     * @brief Invokes a y-combinator and therefore its underlying function.
     * @tparam Args Types of arguments to use to invoke the underlying function.
     * @param args Parameters to use to invoke the underlying function.
     * @return Return value of the underlying function, if any.
     */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) const {
        return func(*this, std::forward<Args>(args)...);
    }

    /*! @copydoc operator()() */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) {
        return func(*this, std::forward<Args>(args)...);
    }

private:
    Func func;
};


}


#endif



namespace entt {


/**
 * @brief Function object to wrap `std::sort` in a class type.
 *
 * Unfortunately, `std::sort` cannot be passed as template argument to a class
 * template or a function template.<br/>
 * This class fills the gap by wrapping some flavors of `std::sort` in a
 * function object.
 */
struct std_sort {
    /**
     * @brief Sorts the elements in a range.
     *
     * Sorts the elements in a range using the given binary comparison function.
     *
     * @tparam It Type of random access iterator.
     * @tparam Compare Type of comparison function object.
     * @tparam Args Types of arguments to forward to the sort function.
     * @param first An iterator to the first element of the range to sort.
     * @param last An iterator past the last element of the range to sort.
     * @param compare A valid comparison function object.
     * @param args Arguments to forward to the sort function, if any.
     */
    template<typename It, typename Compare = std::less<>, typename... Args>
    void operator()(It first, It last, Compare compare = Compare{}, Args &&... args) const {
        std::sort(std::forward<Args>(args)..., std::move(first), std::move(last), std::move(compare));
    }
};


/*! @brief Function object for performing insertion sort. */
struct insertion_sort {
    /**
     * @brief Sorts the elements in a range.
     *
     * Sorts the elements in a range using the given binary comparison function.
     *
     * @tparam It Type of random access iterator.
     * @tparam Compare Type of comparison function object.
     * @param first An iterator to the first element of the range to sort.
     * @param last An iterator past the last element of the range to sort.
     * @param compare A valid comparison function object.
     */
    template<typename It, typename Compare = std::less<>>
    void operator()(It first, It last, Compare compare = Compare{}) const {
        if(first < last) {
            for(auto it = first + 1; it < last; ++it) {
                auto value = std::move(*it);
                auto pre = it;

                for(; pre > first && compare(value, *(pre-1)); --pre) {
                    *pre = std::move(*(pre-1));
                }

                *pre = std::move(value);
            }
        }
    }
};


/**
 * @brief Function object for performing LSD radix sort.
 * @tparam Bit Number of bits processed per pass.
 * @tparam N Maximum number of bits to sort.
 */
template<std::size_t Bit, std::size_t N>
struct radix_sort {
    static_assert((N % Bit) == 0, "The maximum number of bits to sort must be a multiple of the number of bits processed per pass");

    /**
     * @brief Sorts the elements in a range.
     *
     * Sorts the elements in a range using the given _getter_ to access the
     * actual data to be sorted.
     *
     * This implementation is inspired by the online book
     * [Physically Based Rendering](http://www.pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies.html#RadixSort).
     *
     * @tparam It Type of random access iterator.
     * @tparam Getter Type of _getter_ function object.
     * @param first An iterator to the first element of the range to sort.
     * @param last An iterator past the last element of the range to sort.
     * @param getter A valid _getter_ function object.
     */
    template<typename It, typename Getter = identity>
    void operator()(It first, It last, Getter getter = Getter{}) const {
        if(first < last) {
            static constexpr auto mask = (1 << Bit) - 1;
            static constexpr auto buckets = 1 << Bit;
            static constexpr auto passes = N / Bit;

            using value_type = typename std::iterator_traits<It>::value_type;
            std::vector<value_type> aux(std::distance(first, last));

            auto part = [getter = std::move(getter)](auto from, auto to, auto out, auto start) {
                std::size_t index[buckets]{};
                std::size_t count[buckets]{};

                for(auto it = from; it != to; ++it) {
                    ++count[(getter(*it) >> start) & mask];
                }

                for(std::size_t pos{}, end = buckets - 1u; pos < end; ++pos) {
                    index[pos + 1u] = index[pos] + count[pos];
                }

                for(auto it = from; it != to; ++it) {
                    out[index[(getter(*it) >> start) & mask]++] = std::move(*it);
                }
            };

            for(std::size_t pass = 0; pass < (passes & ~1); pass += 2) {
                part(first, last, aux.begin(), pass * Bit);
                part(aux.begin(), aux.end(), first, (pass + 1) * Bit);
            }

            if constexpr(passes & 1) {
                part(first, last, aux.begin(), (passes - 1) * Bit);
                std::move(aux.begin(), aux.end(), first);
            }
        }
    }
};


}


#endif

// #include "core/any.hpp"
#ifndef ENTT_CORE_ANY_HPP
#define ENTT_CORE_ANY_HPP


#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/utility.hpp"
#ifndef ENTT_CORE_UTILITY_HPP
#define ENTT_CORE_UTILITY_HPP


#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/*! @brief Identity function object (waiting for C++20). */
struct identity {
    /**
     * @brief Returns its argument unchanged.
     * @tparam Type Type of the argument.
     * @param value The actual argument.
     * @return The submitted value as-is.
     */
    template<class Type>
    [[nodiscard]] constexpr Type && operator()(Type &&value) const ENTT_NOEXCEPT {
        return std::forward<Type>(value);
    }
};


/**
 * @brief Constant utility to disambiguate overloaded members of a class.
 * @tparam Type Type of the desired overload.
 * @tparam Class Type of class to which the member belongs.
 * @param member A valid pointer to a member.
 * @return Pointer to the member.
 */
template<typename Type, typename Class>
[[nodiscard]] constexpr auto overload(Type Class:: *member) ENTT_NOEXCEPT { return member; }


/**
 * @brief Constant utility to disambiguate overloaded functions.
 * @tparam Func Function type of the desired overload.
 * @param func A valid pointer to a function.
 * @return Pointer to the function.
 */
template<typename Func>
[[nodiscard]] constexpr auto overload(Func *func) ENTT_NOEXCEPT { return func; }


/**
 * @brief Helper type for visitors.
 * @tparam Func Types of function objects.
 */
template<class... Func>
struct overloaded: Func... {
    using Func::operator()...;
};


/**
 * @brief Deduction guide.
 * @tparam Func Types of function objects.
 */
template<class... Func>
overloaded(Func...)
-> overloaded<Func...>;


/**
 * @brief Basic implementation of a y-combinator.
 * @tparam Func Type of a potentially recursive function.
 */
template<class Func>
struct y_combinator {
    /**
     * @brief Constructs a y-combinator from a given function.
     * @param recursive A potentially recursive function.
     */
    y_combinator(Func recursive):
        func{std::move(recursive)}
    {}

    /**
     * @brief Invokes a y-combinator and therefore its underlying function.
     * @tparam Args Types of arguments to use to invoke the underlying function.
     * @param args Parameters to use to invoke the underlying function.
     * @return Return value of the underlying function, if any.
     */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) const {
        return func(*this, std::forward<Args>(args)...);
    }

    /*! @copydoc operator()() */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) {
        return func(*this, std::forward<Args>(args)...);
    }

private:
    Func func;
};


}


#endif

// #include "fwd.hpp"
#ifndef ENTT_CORE_FWD_HPP
#define ENTT_CORE_FWD_HPP


#include <type_traits>
// #include "../config/config.h"



namespace entt {


template<std::size_t Len = sizeof(double[2]), std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_any;


/*! @brief Alias declaration for type identifiers. */
using id_type = ENTT_ID_TYPE;


/*! @brief Alias declaration for the most common use case. */
using any = basic_any<>;


}


#endif

// #include "type_info.hpp"
#ifndef ENTT_CORE_TYPE_INFO_HPP
#define ENTT_CORE_TYPE_INFO_HPP


#include <string_view>
#include <type_traits>
// #include "../config/config.h"

// #include "../core/attribute.h"
#ifndef ENTT_CORE_ATTRIBUTE_H
#define ENTT_CORE_ATTRIBUTE_H


#ifndef ENTT_EXPORT
#   if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#       define ENTT_EXPORT __declspec(dllexport)
#       define ENTT_IMPORT __declspec(dllimport)
#       define ENTT_HIDDEN
#   elif defined __GNUC__ && __GNUC__ >= 4
#       define ENTT_EXPORT __attribute__((visibility("default")))
#       define ENTT_IMPORT __attribute__((visibility("default")))
#       define ENTT_HIDDEN __attribute__((visibility("hidden")))
#   else /* Unsupported compiler */
#       define ENTT_EXPORT
#       define ENTT_IMPORT
#       define ENTT_HIDDEN
#   endif
#endif


#ifndef ENTT_API
#   if defined ENTT_API_EXPORT
#       define ENTT_API ENTT_EXPORT
#   elif defined ENTT_API_IMPORT
#       define ENTT_API ENTT_IMPORT
#   else /* No API */
#       define ENTT_API
#   endif
#endif


#endif

// #include "hashed_string.hpp"
#ifndef ENTT_CORE_HASHED_STRING_HPP
#define ENTT_CORE_HASHED_STRING_HPP


#include <cstddef>
#include <cstdint>
// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
struct fnv1a_traits;


template<>
struct fnv1a_traits<std::uint32_t> {
    using type = std::uint32_t;
    static constexpr std::uint32_t offset = 2166136261;
    static constexpr std::uint32_t prime = 16777619;
};


template<>
struct fnv1a_traits<std::uint64_t> {
    using type = std::uint64_t;
    static constexpr std::uint64_t offset = 14695981039346656037ull;
    static constexpr std::uint64_t prime = 1099511628211ull;
};


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Zero overhead unique identifier.
 *
 * A hashed string is a compile-time tool that allows users to use
 * human-readable identifers in the codebase while using their numeric
 * counterparts at runtime.<br/>
 * Because of that, a hashed string can also be used in constant expressions if
 * required.
 *
 * @tparam Char Character type.
 */
template<typename Char>
class basic_hashed_string {
    using traits_type = internal::fnv1a_traits<id_type>;

    struct const_wrapper {
        // non-explicit constructor on purpose
        constexpr const_wrapper(const Char *curr) ENTT_NOEXCEPT: str{curr} {}
        const Char *str;
    };

    // Fowler–Noll–Vo hash function v. 1a - the good
    [[nodiscard]] static constexpr id_type helper(const Char *curr) ENTT_NOEXCEPT {
        auto value = traits_type::offset;

        while(*curr != 0) {
            value = (value ^ static_cast<traits_type::type>(*(curr++))) * traits_type::prime;
        }

        return value;
    }

public:
    /*! @brief Character type. */
    using value_type = Char;
    /*! @brief Unsigned integer type. */
    using hash_type = id_type;

    /**
     * @brief Returns directly the numeric representation of a string view.
     * @param str Human-readable identifer.
     * @param size Length of the string to hash.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static constexpr hash_type value(const value_type *str, std::size_t size) ENTT_NOEXCEPT {
        id_type partial{traits_type::offset};
        while(size--) { partial = (partial^(str++)[0])*traits_type::prime; }
        return partial;
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * const auto value = basic_hashed_string<char>::to_value("my.png");
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param str Human-readable identifer.
     * @return The numeric representation of the string.
     */
    template<std::size_t N>
    [[nodiscard]] static constexpr hash_type value(const value_type (&str)[N]) ENTT_NOEXCEPT {
        return helper(str);
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static hash_type value(const_wrapper wrapper) ENTT_NOEXCEPT {
        return helper(wrapper.str);
    }

    /*! @brief Constructs an empty hashed string. */
    constexpr basic_hashed_string() ENTT_NOEXCEPT
        : str{nullptr}, hash{}
    {}

    /**
     * @brief Constructs a hashed string from an array of const characters.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * basic_hashed_string<char> hs{"my.png"};
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param curr Human-readable identifer.
     */
    template<std::size_t N>
    constexpr basic_hashed_string(const value_type (&curr)[N]) ENTT_NOEXCEPT
        : str{curr}, hash{helper(curr)}
    {}

    /**
     * @brief Explicit constructor on purpose to avoid constructing a hashed
     * string directly from a `const value_type *`.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     */
    explicit constexpr basic_hashed_string(const_wrapper wrapper) ENTT_NOEXCEPT
        : str{wrapper.str}, hash{helper(wrapper.str)}
    {}

    /**
     * @brief Returns the human-readable representation of a hashed string.
     * @return The string used to initialize the instance.
     */
    [[nodiscard]] constexpr const value_type * data() const ENTT_NOEXCEPT {
        return str;
    }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr hash_type value() const ENTT_NOEXCEPT {
        return hash;
    }

    /*! @copydoc data */
    [[nodiscard]] constexpr operator const value_type *() const ENTT_NOEXCEPT { return data(); }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr operator hash_type() const ENTT_NOEXCEPT { return value(); }

    /**
     * @brief Compares two hashed strings.
     * @param other Hashed string with which to compare.
     * @return True if the two hashed strings are identical, false otherwise.
     */
    [[nodiscard]] constexpr bool operator==(const basic_hashed_string &other) const ENTT_NOEXCEPT {
        return hash == other.hash;
    }

private:
    const value_type *str;
    hash_type hash;
};


/**
 * @brief Deduction guide.
 *
 * It allows to deduce the character type of the hashed string directly from a
 * human-readable identifer provided to the constructor.
 *
 * @tparam Char Character type.
 * @tparam N Number of characters of the identifier.
 * @param str Human-readable identifer.
 */
template<typename Char, std::size_t N>
basic_hashed_string(const Char (&str)[N])
-> basic_hashed_string<Char>;


/**
 * @brief Compares two hashed strings.
 * @tparam Char Character type.
 * @param lhs A valid hashed string.
 * @param rhs A valid hashed string.
 * @return True if the two hashed strings are identical, false otherwise.
 */
template<typename Char>
[[nodiscard]] constexpr bool operator!=(const basic_hashed_string<Char> &lhs, const basic_hashed_string<Char> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/*! @brief Aliases for common character types. */
using hashed_string = basic_hashed_string<char>;


/*! @brief Aliases for common character types. */
using hashed_wstring = basic_hashed_string<wchar_t>;


inline namespace literals {


/**
 * @brief User defined literal for hashed strings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed string.
 */
[[nodiscard]] constexpr entt::hashed_string operator"" _hs(const char *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_string{str};
}


/**
 * @brief User defined literal for hashed wstrings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed wstring.
 */
[[nodiscard]] constexpr entt::hashed_wstring operator"" _hws(const wchar_t *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_wstring{str};
}


}


}


#endif

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


struct ENTT_API type_seq final {
    [[nodiscard]] static id_type next() ENTT_NOEXCEPT {
        static ENTT_MAYBE_ATOMIC(id_type) value{};
        return value++;
    }
};


template<typename Type>
[[nodiscard]] constexpr auto stripped_type_name() ENTT_NOEXCEPT {
#if defined ENTT_PRETTY_FUNCTION
    std::string_view pretty_function{ENTT_PRETTY_FUNCTION};
    auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(ENTT_PRETTY_FUNCTION_PREFIX)+1);
    auto value = pretty_function.substr(first, pretty_function.find_last_of(ENTT_PRETTY_FUNCTION_SUFFIX) - first);
    return value;
#else
    return std::string_view{""};
#endif
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr std::string_view type_name(int) ENTT_NOEXCEPT {
    constexpr auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type>
[[nodiscard]] static std::string_view type_name(char) ENTT_NOEXCEPT {
    static const auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr id_type type_hash(int) ENTT_NOEXCEPT {
    constexpr auto stripped = stripped_type_name<Type>();
    constexpr auto value = hashed_string::value(stripped.data(), stripped.size());
    return value;
}


template<typename Type>
[[nodiscard]] static id_type type_hash(char) ENTT_NOEXCEPT {
    static const auto value = [](const auto stripped) {
        return hashed_string::value(stripped.data(), stripped.size());
    }(stripped_type_name<Type>());
    return value;
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Type sequential identifier.
 * @tparam Type Type for which to generate a sequential identifier.
 */
template<typename Type, typename = void>
struct ENTT_API type_seq final {
    /**
     * @brief Returns the sequential identifier of a given type.
     * @return The sequential identifier of a given type.
     */
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const id_type value = internal::type_seq::next();
        return value;
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type hash.
 * @tparam Type Type for which to generate a hash value.
 */
template<typename Type, typename = void>
struct type_hash final {
    /**
     * @brief Returns the numeric representation of a given type.
     * @return The numeric representation of the given type.
     */
#if defined ENTT_PRETTY_FUNCTION
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return internal::type_hash<Type>(0);
#else
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return type_seq<Type>::value();
#endif
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type name.
 * @tparam Type Type for which to generate a name.
 */
template<typename Type, typename = void>
struct type_name final {
    /**
     * @brief Returns the name of a given type.
     * @return The name of the given type.
     */
    [[nodiscard]] static constexpr std::string_view value() ENTT_NOEXCEPT {
        return internal::type_name<Type>(0);
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator std::string_view() const ENTT_NOEXCEPT { return value(); }
};


/*! @brief Implementation specific information about a type. */
class type_info final {
    template<typename>
    friend type_info type_id() ENTT_NOEXCEPT;

    type_info(id_type seq_v, id_type hash_v, std::string_view name_v) ENTT_NOEXCEPT
        : seq_value{seq_v},
          hash_value{hash_v},
          name_value{name_v}
    {}

public:
    /*! @brief Default constructor. */
    type_info() ENTT_NOEXCEPT
        : type_info({}, {}, {})
    {}

    /*! @brief Default copy constructor. */
    type_info(const type_info &) ENTT_NOEXCEPT = default;
    /*! @brief Default move constructor. */
    type_info(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Default copy assignment operator.
     * @return This type info object.
     */
    type_info & operator=(const type_info &) ENTT_NOEXCEPT = default;

    /**
     * @brief Default move assignment operator.
     * @return This type info object.
     */
    type_info & operator=(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Checks if a type info object is properly initialized.
     * @return True if the object is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return name_value.data() != nullptr;
    }

    /**
     * @brief Type sequential identifier.
     * @return Type sequential identifier.
     */
    [[nodiscard]] id_type seq() const ENTT_NOEXCEPT {
        return seq_value;
    }

    /**
     * @brief Type hash.
     * @return Type hash.
     */
    [[nodiscard]] id_type hash() const ENTT_NOEXCEPT {
        return hash_value;
    }

    /**
     * @brief Type name.
     * @return Type name.
     */
    [[nodiscard]] std::string_view name() const ENTT_NOEXCEPT {
        return name_value;
    }

    /**
     * @brief Compares the contents of two type info objects.
     * @param other Object with which to compare.
     * @return False if the two contents differ, true otherwise.
     */
    [[nodiscard]] bool operator==(const type_info &other) const ENTT_NOEXCEPT {
        return hash_value == other.hash_value;
    }

private:
    id_type seq_value;
    id_type hash_value;
    std::string_view name_value;
};


/**
 * @brief Compares the contents of two type info objects.
 * @param lhs A type info object.
 * @param rhs A type info object.
 * @return True if the two contents differ, false otherwise.
 */
[[nodiscard]] inline bool operator!=(const type_info &lhs, const type_info &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Returns the type info object for a given type.
 * @tparam Type Type for which to generate a type info object.
 * @return The type info object for the given type.
 */
template<typename Type>
[[nodiscard]] type_info type_id() ENTT_NOEXCEPT {
    return type_info{
        type_seq<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_name<std::remove_cv_t<std::remove_reference_t<Type>>>::value()
    };
}


}


#endif

// #include "type_traits.hpp"
#ifndef ENTT_CORE_TYPE_TRAITS_HPP
#define ENTT_CORE_TYPE_TRAITS_HPP


#include <cstddef>
#include <utility>
#include <type_traits>
// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @brief Utility class to disambiguate overloaded functions.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
struct choice_t
    // Unfortunately, doxygen cannot parse such a construct.
    /*! @cond TURN_OFF_DOXYGEN */
    : choice_t<N-1>
    /*! @endcond */
{};


/*! @copybrief choice_t */
template<>
struct choice_t<0> {};


/**
 * @brief Variable template for the choice trick.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
inline constexpr choice_t<N> choice{};


/**
 * @brief Identity type trait.
 *
 * Useful to establish non-deduced contexts in template argument deduction
 * (waiting for C++20) or to provide types through function arguments.
 *
 * @tparam Type A type.
 */
template<typename Type>
struct type_identity {
    /*! @brief Identity type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Type A type.
 */
template<typename Type>
using type_identity_t = typename type_identity<Type>::type;


/**
 * @brief A type-only `sizeof` wrapper that returns 0 where `sizeof` complains.
 * @tparam Type The type of which to return the size.
 * @tparam The size of the type if `sizeof` accepts it, 0 otherwise.
 */
template<typename Type, typename = void>
struct size_of: std::integral_constant<std::size_t, 0u> {};


/*! @copydoc size_of */
template<typename Type>
struct size_of<Type, std::void_t<decltype(sizeof(Type))>>
    : std::integral_constant<std::size_t, sizeof(Type)>
{};


/**
 * @brief Helper variable template.
 * @tparam Type The type of which to return the size.
 */
template<class Type>
inline constexpr auto size_of_v = size_of<Type>::value;


/**
 * @brief Using declaration to be used to _repeat_ the same type a number of
 * times equal to the size of a given parameter pack.
 * @tparam Type A type to repeat.
 */
template<typename Type, typename>
using unpack_as_t = Type;


/**
 * @brief Helper variable template to be used to _repeat_ the same value a
 * number of times equal to the size of a given parameter pack.
 * @tparam Value A value to repeat.
 */
template<auto Value, typename>
inline constexpr auto unpack_as_v = Value;


/**
 * @brief Wraps a static constant.
 * @tparam Value A static constant.
 */
template<auto Value>
using integral_constant = std::integral_constant<decltype(Value), Value>;


/**
 * @brief Alias template to facilitate the creation of named values.
 * @tparam Value A constant value at least convertible to `id_type`.
 */
template<id_type Value>
using tag = integral_constant<Value>;


/**
 * @brief A class to use to push around lists of types, nothing more.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list {
    /*! @brief Type list type. */
    using type = type_list;
    /*! @brief Compile-time number of elements in the type list. */
    static constexpr auto size = sizeof...(Type);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct type_list_element;


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Index Index of the type to return.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<std::size_t Index, typename Type, typename... Other>
struct type_list_element<Index, type_list<Type, Other...>>
    : type_list_element<Index - 1u, type_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<typename Type, typename... Other>
struct type_list_element<0u, type_list<Type, Other...>> {
    /*! @brief Searched type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the type to return.
 * @tparam List Type list to search into.
 */
template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @return A type list composed by the types of both the type lists.
 */
template<typename... Type, typename... Other>
constexpr type_list<Type..., Other...> operator+(type_list<Type...>, type_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_cat;


/*! @brief Concatenates multiple type lists. */
template<>
struct type_list_cat<> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<>;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @tparam List Other type lists, if any.
 */
template<typename... Type, typename... Other, typename... List>
struct type_list_cat<type_list<Type...>, type_list<Other...>, List...> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = typename type_list_cat<type_list<Type..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list_cat<type_list<Type...>> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<Type...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists to concatenate.
 */
template<typename... List>
using type_list_cat_t = typename type_list_cat<List...>::type;


/*! @brief Primary template isn't defined on purpose. */
template<typename>
struct type_list_unique;


/**
 * @brief Removes duplicates types from a type list.
 * @tparam Type One of the types provided by the given type list.
 * @tparam Other The other types provided by the given type list.
 */
template<typename Type, typename... Other>
struct type_list_unique<type_list<Type, Other...>> {
    /*! @brief A type list without duplicate types. */
    using type = std::conditional_t<
        std::disjunction_v<std::is_same<Type, Other>...>,
        typename type_list_unique<type_list<Other...>>::type,
        type_list_cat_t<type_list<Type>, typename type_list_unique<type_list<Other...>>::type>
    >;
};


/*! @brief Removes duplicates types from a type list. */
template<>
struct type_list_unique<type_list<>> {
    /*! @brief A type list without duplicate types. */
    using type = type_list<>;
};


/**
 * @brief Helper type.
 * @tparam Type A type list.
 */
template<typename Type>
using type_list_unique_t = typename type_list_unique<Type>::type;


/**
 * @brief Provides the member constant `value` to true if a type list contains a
 * given type, false otherwise.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<typename List, typename Type>
struct type_list_contains;


/**
 * @copybrief type_list_contains
 * @tparam Type Types provided by the type list.
 * @tparam Other Type to look for.
 */
template<typename... Type, typename Other>
struct type_list_contains<type_list<Type...>, Other>: std::disjunction<std::is_same<Type, Other>...> {};


/**
 * @brief Helper variable template.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<class List, typename Type>
inline constexpr auto type_list_contains_v = type_list_contains<List, Type>::value;


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_diff;


/**
 * @brief Computes the difference between two type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 */
template<typename... Type, typename... Other>
struct type_list_diff<type_list<Type...>, type_list<Other...>> {
    /*! @brief A type list that is the difference between the two type lists. */
    using type = type_list_cat_t<std::conditional_t<type_list_contains_v<type_list<Other...>, Type>, type_list<>, type_list<Type>>...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists between which to compute the difference.
 */
template<typename... List>
using type_list_diff_t = typename type_list_diff<List...>::type;


/**
 * @brief A class to use to push around lists of constant values, nothing more.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list {
    /*! @brief Value list type. */
    using type = value_list;
    /*! @brief Compile-time number of elements in the value list. */
    static constexpr auto size = sizeof...(Value);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct value_list_element;


/**
 * @brief Provides compile-time indexed access to the values of a value list.
 * @tparam Index Index of the value to return.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<std::size_t Index, auto Value, auto... Other>
struct value_list_element<Index, value_list<Value, Other...>>
    : value_list_element<Index - 1u, value_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<auto Value, auto... Other>
struct value_list_element<0u, value_list<Value, Other...>> {
    /*! @brief Searched value. */
    static constexpr auto value = Value;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the value to return.
 * @tparam List Value list to search into.
 */
template<std::size_t Index, typename List>
inline constexpr auto value_list_element_v = value_list_element<Index, List>::value;


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @return A value list composed by the values of both the value lists.
 */
template<auto... Value, auto... Other>
constexpr value_list<Value..., Other...> operator+(value_list<Value...>, value_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct value_list_cat;


/*! @brief Concatenates multiple value lists. */
template<>
struct value_list_cat<> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<>;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @tparam List Other value lists, if any.
 */
template<auto... Value, auto... Other, typename... List>
struct value_list_cat<value_list<Value...>, value_list<Other...>, List...> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = typename value_list_cat<value_list<Value..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list_cat<value_list<Value...>> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<Value...>;
};


/**
 * @brief Helper type.
 * @tparam List Value lists to concatenate.
 */
template<typename... List>
using value_list_cat_t = typename value_list_cat<List...>::type;


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
[[nodiscard]] constexpr bool is_equality_comparable(...) { return false; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<0>)
-> decltype(std::declval<Type>() == std::declval<Type>()) { return true; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<1>)
-> decltype(std::declval<typename Type::value_type>(), std::declval<Type>() == std::declval<Type>()) {
    if constexpr(std::is_same_v<typename Type::value_type, Type>) {
        return is_equality_comparable<Type>(choice<0>);
    } else {
        return is_equality_comparable<typename Type::value_type>(choice<2>);
    }
}


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<2>)
-> decltype(std::declval<typename Type::mapped_type>(), std::declval<Type>() == std::declval<Type>()) {
    return is_equality_comparable<typename Type::key_type>(choice<2>) && is_equality_comparable<typename Type::mapped_type>(choice<2>);
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Provides the member constant `value` to true if a given type is
 * equality comparable, false otherwise.
 * @tparam Type Potentially equality comparable type.
 */
template<typename Type, typename = void>
struct is_equality_comparable: std::bool_constant<internal::is_equality_comparable<Type>(choice<2>)> {};


/**
 * @brief Helper variable template.
 * @tparam Type Potentially equality comparable type.
 */
template<class Type>
inline constexpr auto is_equality_comparable_v = is_equality_comparable<Type>::value;


/*! @brief Same as std::is_invocable, but with tuples. */
template<typename, typename>
struct is_applicable: std::false_type {};


/**
 * @copybrief is_applicable
 * @tparam Func A valid function type.
 * @tparam Tuple Tuple-like type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
* @copybrief is_applicable
* @tparam Func A valid function type.
* @tparam Tuple Tuple-like type.
* @tparam Args The list of arguments to use to probe the function type.
*/
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, const Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, typename Args>
inline constexpr auto is_applicable_v = is_applicable<Func, Args>::value;


/*! @brief Same as std::is_invocable_r, but with tuples for arguments. */
template<typename, typename, typename>
struct is_applicable_r: std::false_type {};


/**
 * @copybrief is_applicable_r
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename... Args>
struct is_applicable_r<Ret, Func, std::tuple<Args...>>: std::is_invocable_r<Ret, Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename Args>
inline constexpr auto is_applicable_r_v = is_applicable_r<Ret, Func, Args>::value;


/**
* @brief Provides the member constant `value` to true if a given type is
* complete, false otherwise.
* @tparam Type Potential complete type.
*/
template<typename Type, typename = void>
struct is_complete: std::false_type {};


/*! @copydoc is_complete */
template<typename Type>
struct is_complete<Type, std::void_t<decltype(sizeof(Type))>>: std::true_type {};


/**
* @brief Helper variable template.
* @tparam Type Potential complete type.
*/
template<typename Type>
inline constexpr auto is_complete_v = is_complete<Type>::value;


/**
 * @brief Transcribes the constness of a type to another type.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
struct constness_as {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::remove_const_t<To>;
};


/*! @copydoc constness_as */
template<typename To, typename From>
struct constness_as<To, const From> {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::add_const_t<To>;
};


/**
 * @brief Alias template to facilitate the transcription of the constness.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;


/**
 * @brief Extracts the class of a non-static member object or function.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
class member_class {
    static_assert(std::is_member_pointer_v<Member>, "Invalid pointer type to non-static member object or function");

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...));

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...) const);

    template<typename Class, typename Type>
    static Class * clazz(Type Class:: *);

public:
    /*! @brief The class of the given non-static member object or function. */
    using type = std::remove_pointer_t<decltype(clazz(std::declval<Member>()))>;
};


/**
 * @brief Helper type.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
using member_class_t = typename member_class<Member>::type;


}


#endif



namespace entt {


/**
 * @brief A SBO friendly, type-safe container for single values of any type.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 */
template<std::size_t Len, std::size_t Align>
class basic_any {
    enum class operation: std::uint8_t { COPY, MOVE, DTOR, COMP, ADDR, CADDR, TYPE };
    enum class policy: std::uint8_t { OWNER, REF, CREF };

    using storage_type = std::aligned_storage_t<Len + !Len, Align>;
    using vtable_type = const void *(const operation, const basic_any &, void *);

    template<typename Type>
    static constexpr bool in_situ = Len && alignof(Type) <= alignof(storage_type) && sizeof(Type) <= sizeof(storage_type) && std::is_nothrow_move_constructible_v<Type>;

    template<typename Type>
    [[nodiscard]] static constexpr policy type_to_policy() {
        if constexpr(std::is_lvalue_reference_v<Type>) {
            if constexpr(std::is_const_v<std::remove_reference_t<Type>>) {
                return policy::CREF;
            } else {
                return policy::REF;
            }
        } else {
            return policy::OWNER;
        }
    }

    template<typename Type>
    [[nodiscard]] static bool compare(const void *lhs, const void *rhs) {
        if constexpr(!std::is_function_v<Type> && is_equality_comparable_v<Type>) {
            return *static_cast<const Type *>(lhs) == *static_cast<const Type *>(rhs);
        } else {
            return lhs == rhs;
        }
    }

    template<typename Type>
    static const void * basic_vtable([[maybe_unused]] const operation op, [[maybe_unused]] const basic_any &from, [[maybe_unused]] void *to) {
        static_assert(std::is_same_v<std::remove_reference_t<std::remove_const_t<Type>>, Type>, "Invalid type");

        if constexpr(!std::is_void_v<Type>) {
            const Type *instance = (in_situ<Type> && from.mode == policy::OWNER)
                ? ENTT_LAUNDER(reinterpret_cast<const Type *>(&from.storage))
                : static_cast<const Type *>(from.instance);

            switch(op) {
            case operation::COPY:
                if constexpr(std::is_copy_constructible_v<Type>) {
                    static_cast<basic_any *>(to)->emplace<Type>(*instance);
                }
                break;
            case operation::MOVE:
                if constexpr(in_situ<Type>) {
                    if(from.mode == policy::OWNER) {
                        return new (&static_cast<basic_any *>(to)->storage) Type{std::move(*const_cast<Type *>(instance))};
                    }
                }

                return (static_cast<basic_any *>(to)->instance = std::exchange(const_cast<basic_any &>(from).instance, nullptr));
            case operation::DTOR:
                if(from.mode == policy::OWNER) {
                    if constexpr(in_situ<Type>) {
                        instance->~Type();
                    } else if constexpr(std::is_array_v<Type>) {
                        delete[] instance;
                    } else {
                        delete instance;
                    }
                }
                break;
            case operation::COMP:
                return compare<Type>(instance, (*static_cast<const basic_any **>(to))->data()) ? to : nullptr;
            case operation::ADDR:
                if(from.mode == policy::CREF) {
                    return nullptr;
                }
                [[fallthrough]];
            case operation::CADDR:
                return instance;
            case operation::TYPE:
                *static_cast<type_info *>(to) = type_id<Type>();
                break;
            }
        }

        return nullptr;
    }

    template<typename Type, typename... Args>
    void initialize([[maybe_unused]] Args &&... args) {
        if constexpr(!std::is_void_v<Type>) {
            if constexpr(std::is_lvalue_reference_v<Type>) {
                static_assert(sizeof...(Args) == 1u && (std::is_lvalue_reference_v<Args> && ...), "Invalid arguments");
                instance = (std::addressof(args), ...);
            } else if constexpr(in_situ<Type>) {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    new (&storage) Type{std::forward<Args>(args)...};
                } else {
                    new (&storage) Type(std::forward<Args>(args)...);
                }
            } else {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    instance = new Type{std::forward<Args>(args)...};
                } else {
                    instance = new Type(std::forward<Args>(args)...);
                }
            }
        }
    }

    basic_any(const basic_any &other, const policy pol) ENTT_NOEXCEPT
        : instance{other.data()},
          vtable{other.vtable},
          mode{pol}
    {}

public:
    /*! @brief Size of the internal storage. */
    static constexpr auto length = Len;
    /*! @brief Alignment requirement. */
    static constexpr auto alignment = Align;

    /*! @brief Default constructor. */
    basic_any() ENTT_NOEXCEPT
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {}

    /**
     * @brief Constructs a wrapper by directly initializing the new object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    explicit basic_any(std::in_place_type_t<Type>, Args &&... args)
        : instance{},
          vtable{&basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>},
          mode{type_to_policy<Type>()}
    {
        initialize<Type>(std::forward<Args>(args)...);
    }

    /**
     * @brief Constructs a wrapper that holds an unmanaged object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type>
    basic_any(std::reference_wrapper<Type> value) ENTT_NOEXCEPT
        : basic_any{}
    {
        // invokes deprecated assignment operator (and avoids issues with vs2017)
        *this = value;
    }

    /**
     * @brief Constructs a wrapper from a given value.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>>>
    basic_any(Type &&value)
        : instance{},
          vtable{&basic_vtable<std::decay_t<Type>>},
          mode{policy::OWNER}
    {
        initialize<std::decay_t<Type>>(std::forward<Type>(value));
    }

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    basic_any(const basic_any &other)
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {
        other.vtable(operation::COPY, other, this);
    }

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    basic_any(basic_any &&other) ENTT_NOEXCEPT
        : instance{},
          vtable{other.vtable},
          mode{other.mode}
    {
        vtable(operation::MOVE, other, this);
    }

    /*! @brief Frees the internal storage, whatever it means. */
    ~basic_any() {
        vtable(operation::DTOR, *this, nullptr);
    }

    /**
     * @brief Copy assignment operator.
     * @param other The instance to copy from.
     * @return This any object.
     */
    basic_any & operator=(const basic_any &other) {
        reset();
        other.vtable(operation::COPY, other, this);
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This any object.
     */
    basic_any & operator=(basic_any &&other) ENTT_NOEXCEPT {
        std::exchange(vtable, other.vtable)(operation::DTOR, *this, nullptr);
        other.vtable(operation::MOVE, other, this);
        mode = other.mode;
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    [[deprecated("Use std::in_place_type<T &>, entt::make_any<T &>, emplace<Type &> or forward_as_any instead")]]
    basic_any & operator=(std::reference_wrapper<Type> value) ENTT_NOEXCEPT {
        emplace<Type &>(value.get());
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>, basic_any &>
    operator=(Type &&value) {
        emplace<std::decay_t<Type>>(std::forward<Type>(value));
        return *this;
    }

    /**
     * @brief Returns the type of the contained object.
     * @return The type of the contained object, if any.
     */
    [[nodiscard]] type_info type() const ENTT_NOEXCEPT {
        type_info info{};
        vtable(operation::TYPE, *this, &info);
        return info;
    }

    /**
     * @brief Returns an opaque pointer to the contained instance.
     * @return An opaque pointer the contained instance, if any.
     */
    [[nodiscard]] const void * data() const ENTT_NOEXCEPT {
        return vtable(operation::CADDR, *this, nullptr);
    }

    /*! @copydoc data */
    [[nodiscard]] void * data() ENTT_NOEXCEPT {
        return const_cast<void *>(vtable(operation::ADDR, *this, nullptr));
    }

    /**
     * @brief Replaces the contained object by creating a new instance directly.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    void emplace(Args &&... args) {
        std::exchange(vtable, &basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>)(operation::DTOR, *this, nullptr);
        mode = type_to_policy<Type>();
        initialize<Type>(std::forward<Args>(args)...);
    }

    /*! @brief Destroys contained object */
    void reset() {
        std::exchange(vtable, &basic_vtable<void>)(operation::DTOR, *this, nullptr);
        mode = policy::OWNER;
    }

    /**
     * @brief Returns false if a wrapper is empty, true otherwise.
     * @return False if the wrapper is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(vtable(operation::CADDR, *this, nullptr) == nullptr);
    }

    /**
     * @brief Checks if two wrappers differ in their content.
     * @param other Wrapper with which to compare.
     * @return False if the two objects differ in their content, true otherwise.
     */
    bool operator==(const basic_any &other) const ENTT_NOEXCEPT {
        const basic_any *trampoline = &other;
        return type() == other.type() && (vtable(operation::COMP, *this, &trampoline) || !other.data());
    }

    /**
     * @brief Aliasing constructor.
     * @return A wrapper that shares a reference to an unmanaged object.
     */
    [[nodiscard]] basic_any as_ref() ENTT_NOEXCEPT {
        return basic_any{*this, (mode == policy::CREF ? policy::CREF : policy::REF)};
    }

    /*! @copydoc as_ref */
    [[nodiscard]] basic_any as_ref() const ENTT_NOEXCEPT {
        return basic_any{*this, policy::CREF};
    }

    /**
     * @brief Returns true if a wrapper owns its object, false otherwise.
     * @return True if the wrapper owns its object, false otherwise.
     */
    [[nodiscard]] bool owner() const ENTT_NOEXCEPT {
        return (mode == policy::OWNER);
    }

private:
    union { const void *instance; storage_type storage; };
    vtable_type *vtable;
    policy mode;
};


/**
 * @brief Checks if two wrappers differ in their content.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param lhs A wrapper, either empty or not.
 * @param rhs A wrapper, either empty or not.
 * @return True if the two wrappers differ in their content, false otherwise.
 */
template<std::size_t Len, std::size_t Align>
[[nodiscard]] inline bool operator!=(const basic_any<Len, Align> &lhs, const basic_any<Len, Align> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Performs type-safe access to the contained object.
 * @tparam Type Type to which conversion is required.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param data Target any object.
 * @return The element converted to the requested type.
 */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(const basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    const auto * const instance = any_cast<std::remove_reference_t<Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &&data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(std::move(*instance));
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
const Type * any_cast(const basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    return (data->type() == type_id<Type>() ? static_cast<const Type *>(data->data()) : nullptr);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type * any_cast(basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    // last attempt to make wrappers for const references return their values
    return (data->type() == type_id<Type>() ? static_cast<Type *>(static_cast<constness_as_t<basic_any<Len, Align>, Type> *>(data)->data()) : nullptr);
}


/**
 * @brief Constructs a wrapper from a given type, passing it all arguments.
 * @tparam Type Type of object to use to initialize the wrapper.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Args Types of arguments to use to construct the new instance.
 * @param args Parameters to use to construct the instance.
 * @return A properly initialized wrapper for an object of the given type.
 */
template<typename Type, std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename... Args>
basic_any<Len, Align> make_any(Args &&... args) {
    return basic_any<Len, Align>{std::in_place_type<Type>, std::forward<Args>(args)...};
}


/**
 * @brief Forwards its argument and avoids copies for lvalue references.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Type Type of argument to use to construct the new instance.
 * @param value Parameter to use to construct the instance.
 * @return A properly initialized and not necessarily owning wrapper.
 */
template<std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename Type>
basic_any<Len, Align> forward_as_any(Type &&value) {
    return basic_any<Len, Align>{std::in_place_type<std::conditional_t<std::is_rvalue_reference_v<Type>, std::decay_t<Type>, Type>>, std::forward<Type>(value)};
}


}


#endif

// #include "core/attribute.h"
#ifndef ENTT_CORE_ATTRIBUTE_H
#define ENTT_CORE_ATTRIBUTE_H


#ifndef ENTT_EXPORT
#   if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#       define ENTT_EXPORT __declspec(dllexport)
#       define ENTT_IMPORT __declspec(dllimport)
#       define ENTT_HIDDEN
#   elif defined __GNUC__ && __GNUC__ >= 4
#       define ENTT_EXPORT __attribute__((visibility("default")))
#       define ENTT_IMPORT __attribute__((visibility("default")))
#       define ENTT_HIDDEN __attribute__((visibility("hidden")))
#   else /* Unsupported compiler */
#       define ENTT_EXPORT
#       define ENTT_IMPORT
#       define ENTT_HIDDEN
#   endif
#endif


#ifndef ENTT_API
#   if defined ENTT_API_EXPORT
#       define ENTT_API ENTT_EXPORT
#   elif defined ENTT_API_IMPORT
#       define ENTT_API ENTT_IMPORT
#   else /* No API */
#       define ENTT_API
#   endif
#endif


#endif

// #include "core/family.hpp"
#ifndef ENTT_CORE_FAMILY_HPP
#define ENTT_CORE_FAMILY_HPP


// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @brief Dynamic identifier generator.
 *
 * Utility class template that can be used to assign unique identifiers to types
 * at runtime. Use different specializations to create separate sets of
 * identifiers.
 */
template<typename...>
class family {
    inline static ENTT_MAYBE_ATOMIC(id_type) identifier{};

public:
    /*! @brief Unsigned integer type. */
    using family_type = id_type;

    /*! @brief Statically generated unique identifier for the given type. */
    template<typename... Type>
    // at the time I'm writing, clang crashes during compilation if auto is used instead of family_type
    inline static const family_type type = identifier++;
};


}


#endif

// #include "core/hashed_string.hpp"

// #include "core/ident.hpp"
#ifndef ENTT_CORE_IDENT_HPP
#define ENTT_CORE_IDENT_HPP


#include <cstddef>
#include <utility>
#include <type_traits>
// #include "../config/config.h"

// #include "fwd.hpp"

// #include "type_traits.hpp"



namespace entt {


/**
 * @brief Types identifiers.
 *
 * Variable template used to generate identifiers at compile-time for the given
 * types. Use the `get` member function to know what's the identifier associated
 * to the specific type.
 *
 * @note
 * Identifiers are constant expression and can be used in any context where such
 * an expression is required. As an example:
 * @code{.cpp}
 * using id = entt::identifier<a_type, another_type>;
 *
 * switch(a_type_identifier) {
 * case id::type<a_type>:
 *     // ...
 *     break;
 * case id::type<another_type>:
 *     // ...
 *     break;
 * default:
 *     // ...
 * }
 * @endcode
 *
 * @tparam Types List of types for which to generate identifiers.
 */
template<typename... Types>
class identifier {
    template<typename Type, std::size_t... Index>
    [[nodiscard]] static constexpr id_type get(std::index_sequence<Index...>) {
        static_assert(std::disjunction_v<std::is_same<Type, Types>...>, "Invalid type");
        return (0 + ... + (std::is_same_v<Type, type_list_element_t<Index, type_list<std::decay_t<Types>...>>> ? id_type{Index} : id_type{}));
    }

public:
    /*! @brief Unsigned integer type. */
    using identifier_type = id_type;

    /*! @brief Statically generated unique identifier for the given type. */
    template<typename Type>
    static constexpr identifier_type type = get<std::decay_t<Type>>(std::index_sequence_for<Types...>{});
};


}


#endif

// #include "core/monostate.hpp"
#ifndef ENTT_CORE_MONOSTATE_HPP
#define ENTT_CORE_MONOSTATE_HPP


// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @brief Minimal implementation of the monostate pattern.
 *
 * A minimal, yet complete configuration system built on top of the monostate
 * pattern. Thread safe by design, it works only with basic types like `int`s or
 * `bool`s.<br/>
 * Multiple types and therefore more than one value can be associated with a
 * single key. Because of this, users must pay attention to use the same type
 * both during an assignment and when they try to read back their data.
 * Otherwise, they can incur in unexpected results.
 */
template<id_type>
struct monostate {
    /**
     * @brief Assigns a value of a specific type to a given key.
     * @tparam Type Type of the value to assign.
     * @param val User data to assign to the given key.
     */
    template<typename Type>
    void operator=(Type val) const ENTT_NOEXCEPT {
        value<Type> = val;
    }

    /**
     * @brief Gets a value of a specific type for a given key.
     * @tparam Type Type of the value to get.
     * @return Stored value, if any.
     */
    template<typename Type>
    operator Type() const ENTT_NOEXCEPT {
        return value<Type>;
    }

private:
    template<typename Type>
    inline static ENTT_MAYBE_ATOMIC(Type) value{};
};


/**
 * @brief Helper variable template.
 * @tparam Value Value used to differentiate between different variables.
 */
template<id_type Value>
inline monostate<Value> monostate_v = {};


}


#endif

// #include "core/type_info.hpp"

// #include "core/type_traits.hpp"

// #include "core/utility.hpp"

// #include "entity/component.hpp"
#ifndef ENTT_ENTITY_COMPONENT_HPP
#define ENTT_ENTITY_COMPONENT_HPP


#include <type_traits>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/*! @brief Commonly used default traits for all types. */
struct basic_component_traits {
    /*! @brief Pointer stability, default is `std::false_type`. */
    using in_place_delete = std::false_type;
    /*! @brief Empty type optimization, default is `ENTT_IGNORE_IF_EMPTY`. */
    using ignore_if_empty = ENTT_IGNORE_IF_EMPTY;
};


/**
 * @brief Common way to access various properties of components.
 * @tparam Type Type of component.
 */
template<typename Type, typename = void>
struct component_traits: basic_component_traits {
    static_assert(std::is_same_v<std::decay_t<Type>, Type>, "Unsupported type");
};


}


#endif

// #include "entity/entity.hpp"
#ifndef ENTT_ENTITY_ENTITY_HPP
#define ENTT_ENTITY_ENTITY_HPP


#include <cstddef>
#include <cstdint>
#include <type_traits>
// #include "../config/config.h"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename, typename = void>
struct entt_traits;


template<typename Type>
struct entt_traits<Type, std::enable_if_t<std::is_enum_v<Type>>>
    : entt_traits<std::underlying_type_t<Type>>
{};


template<typename Type>
struct entt_traits<Type, std::enable_if_t<std::is_class_v<Type>>>
    : entt_traits<typename Type::entity_type>
{};


template<>
struct entt_traits<std::uint32_t> {
    using entity_type = std::uint32_t;
    using version_type = std::uint16_t;
    using difference_type = std::int64_t;

    static constexpr entity_type entity_mask = 0xFFFFF;
    static constexpr entity_type version_mask = 0xFFF;
    static constexpr std::size_t entity_shift = 20u;
};


template<>
struct entt_traits<std::uint64_t> {
    using entity_type = std::uint64_t;
    using version_type = std::uint32_t;
    using difference_type = std::int64_t;

    static constexpr entity_type entity_mask = 0xFFFFFFFF;
    static constexpr entity_type version_mask = 0xFFFFFFFF;
    static constexpr std::size_t entity_shift = 32u;
};


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Entity traits.
 * @tparam Type Type of identifier.
 */
template<typename Type>
class entt_traits: private internal::entt_traits<Type> {
    using traits_type = internal::entt_traits<Type>;

public:
    /*! @brief Value type. */
    using value_type = Type;
    /*! @brief Underlying entity type. */
    using entity_type = typename traits_type::entity_type;
    /*! @brief Underlying version type. */
    using version_type = typename traits_type::version_type;
    /*! @brief Difference type. */
    using difference_type = typename traits_type::difference_type;

    /**
     * @brief Converts an entity to its underlying type.
     * @param value The value to convert.
     * @return The integral representation of the given value.
     */
    [[nodiscard]] static constexpr entity_type to_integral(const value_type value) ENTT_NOEXCEPT {
        return static_cast<entity_type>(value);
    }

    /**
     * @brief Returns the entity part once converted to the underlying type.
     * @param value The value to convert.
     * @return The integral representation of the entity part.
     */
    [[nodiscard]] static constexpr entity_type to_entity(const value_type value) ENTT_NOEXCEPT {
        return (to_integral(value) & traits_type::entity_mask);
    }

    /**
     * @brief Returns the version part once converted to the underlying type.
     * @param value The value to convert.
     * @return The integral representation of the version part.
     */
    [[nodiscard]] static constexpr version_type to_version(const value_type value) ENTT_NOEXCEPT {
        constexpr auto mask = (traits_type::version_mask << traits_type::entity_shift);
        return ((to_integral(value) & mask) >> traits_type::entity_shift);
    }

    /**
     * @brief Constructs an identifier from its parts.
     *
     * If the version part is not provided, a tombstone is returned.<br/>
     * If the entity part is not provided, a null identifier is returned.
     *
     * @param entity The entity part of the identifier.
     * @param version The version part of the identifier.
     * @return A properly constructed identifier.
     */
    [[nodiscard]] static constexpr value_type construct(const entity_type entity = traits_type::entity_mask, const version_type version = traits_type::version_mask) ENTT_NOEXCEPT {
        return value_type{(entity & traits_type::entity_mask) | (version << traits_type::entity_shift)};
    }
};


/**
 * @brief Converts an entity to its underlying type.
 * @tparam Entity The value type.
 * @param entity The value to convert.
 * @return The integral representation of the given value.
 */
template<typename Entity>
[[nodiscard]] constexpr auto to_integral(const Entity entity) ENTT_NOEXCEPT {
    return entt_traits<Entity>::to_integral(entity);
}


/*! @brief Null object for all entity identifiers.  */
struct null_t {
    /**
     * @brief Converts the null object to identifiers of any type.
     * @tparam Entity Type of entity identifier.
     * @return The null representation for the given type.
     */
    template<typename Entity>
    [[nodiscard]] constexpr operator Entity() const ENTT_NOEXCEPT {
        return entt_traits<Entity>::construct();
    }

    /**
     * @brief Compares two null objects.
     * @param other A null object.
     * @return True in all cases.
     */
    [[nodiscard]] constexpr bool operator==([[maybe_unused]] const null_t other) const ENTT_NOEXCEPT {
        return true;
    }

    /**
     * @brief Compares two null objects.
     * @param other A null object.
     * @return False in all cases.
     */
    [[nodiscard]] constexpr bool operator!=([[maybe_unused]] const null_t other) const ENTT_NOEXCEPT {
        return false;
    }

    /**
     * @brief Compares a null object and an entity identifier of any type.
     * @tparam Entity Type of entity identifier.
     * @param entity Entity identifier with which to compare.
     * @return False if the two elements differ, true otherwise.
     */
    template<typename Entity>
    [[nodiscard]] constexpr bool operator==(const Entity entity) const ENTT_NOEXCEPT {
        return entt_traits<Entity>::to_entity(entity) == entt_traits<Entity>::to_entity(*this);
    }

    /**
     * @brief Compares a null object and an entity identifier of any type.
     * @tparam Entity Type of entity identifier.
     * @param entity Entity identifier with which to compare.
     * @return True if the two elements differ, false otherwise.
     */
    template<typename Entity>
    [[nodiscard]] constexpr bool operator!=(const Entity entity) const ENTT_NOEXCEPT {
        return !(entity == *this);
    }

    /**
     * @brief Creates a null object from an entity identifier of any type.
     * @tparam Entity Type of entity identifier.
     * @param entity Entity identifier to turn into a null object.
     * @return The null representation for the given identifier.
     */
    template<typename Entity>
    [[nodiscard]] constexpr Entity operator|(const Entity entity) const ENTT_NOEXCEPT {
        return entt_traits<Entity>::construct(entt_traits<Entity>::to_entity(*this), entt_traits<Entity>::to_version(entity));
    }
};


/**
 * @brief Compares a null object and an entity identifier of any type.
 * @tparam Entity Type of entity identifier.
 * @param entity Entity identifier with which to compare.
 * @param other A null object yet to be converted.
 * @return False if the two elements differ, true otherwise.
 */
template<typename Entity>
[[nodiscard]] constexpr bool operator==(const Entity entity, const null_t other) ENTT_NOEXCEPT {
    return other.operator==(entity);
}


/**
 * @brief Compares a null object and an entity identifier of any type.
 * @tparam Entity Type of entity identifier.
 * @param entity Entity identifier with which to compare.
 * @param other A null object yet to be converted.
 * @return True if the two elements differ, false otherwise.
 */
template<typename Entity>
[[nodiscard]] constexpr bool operator!=(const Entity entity, const null_t other) ENTT_NOEXCEPT {
    return !(other == entity);
}


/*! @brief Tombstone object for all entity identifiers.  */
struct tombstone_t {
    /**
     * @brief Converts the tombstone object to identifiers of any type.
     * @tparam Entity Type of entity identifier.
     * @return The tombstone representation for the given type.
     */
    template<typename Entity>
    [[nodiscard]] constexpr operator Entity() const ENTT_NOEXCEPT {
        return entt_traits<Entity>::construct();
    }

    /**
     * @brief Compares two tombstone objects.
     * @param other A tombstone object.
     * @return True in all cases.
     */
    [[nodiscard]] constexpr bool operator==([[maybe_unused]] const tombstone_t other) const ENTT_NOEXCEPT {
        return true;
    }

    /**
     * @brief Compares two tombstone objects.
     * @param other A tombstone object.
     * @return False in all cases.
     */
    [[nodiscard]] constexpr bool operator!=([[maybe_unused]] const tombstone_t other) const ENTT_NOEXCEPT {
        return false;
    }

    /**
     * @brief Compares a tombstone object and an entity identifier of any type.
     * @tparam Entity Type of entity identifier.
     * @param entity Entity identifier with which to compare.
     * @return False if the two elements differ, true otherwise.
     */
    template<typename Entity>
    [[nodiscard]] constexpr bool operator==(const Entity entity) const ENTT_NOEXCEPT {
        return entt_traits<Entity>::to_version(entity) == entt_traits<Entity>::to_version(*this);
    }

    /**
     * @brief Compares a tombstone object and an entity identifier of any type.
     * @tparam Entity Type of entity identifier.
     * @param entity Entity identifier with which to compare.
     * @return True if the two elements differ, false otherwise.
     */
    template<typename Entity>
    [[nodiscard]] constexpr bool operator!=(const Entity entity) const ENTT_NOEXCEPT {
        return !(entity == *this);
    }

    /**
     * @brief Creates a tombstone object from an entity identifier of any type.
     * @tparam Entity Type of entity identifier.
     * @param entity Entity identifier to turn into a tombstone object.
     * @return The tombstone representation for the given identifier.
     */
    template<typename Entity>
    [[nodiscard]] constexpr Entity operator|(const Entity entity) const ENTT_NOEXCEPT {
        return entt_traits<Entity>::construct(entt_traits<Entity>::to_entity(entity));
    }
};


/**
 * @brief Compares a tombstone object and an entity identifier of any type.
 * @tparam Entity Type of entity identifier.
 * @param entity Entity identifier with which to compare.
 * @param other A tombstone object yet to be converted.
 * @return False if the two elements differ, true otherwise.
 */
template<typename Entity>
[[nodiscard]] constexpr bool operator==(const Entity entity, const tombstone_t other) ENTT_NOEXCEPT {
    return other.operator==(entity);
}


/**
 * @brief Compares a tombstone object and an entity identifier of any type.
 * @tparam Entity Type of entity identifier.
 * @param entity Entity identifier with which to compare.
 * @param other A tombstone object yet to be converted.
 * @return True if the two elements differ, false otherwise.
 */
template<typename Entity>
[[nodiscard]] constexpr bool operator!=(const Entity entity, const tombstone_t other) ENTT_NOEXCEPT {
    return !(other == entity);
}


/**
 * @brief Compile-time constant for null entities.
 *
 * There exist implicit conversions from this variable to entity identifiers of
 * any allowed type. Similarly, there exist comparision operators between the
 * null entity and any other entity identifier.
 */
inline constexpr null_t null{};


/**
 * @brief Compile-time constant for tombstone entities.
 *
 * There exist implicit conversions from this variable to entity identifiers of
 * any allowed type. Similarly, there exist comparision operators between the
 * tombstone entity and any other entity identifier.
 */
inline constexpr tombstone_t tombstone{};


}


#endif

// #include "entity/group.hpp"
#ifndef ENTT_ENTITY_GROUP_HPP
#define ENTT_ENTITY_GROUP_HPP


#include <tuple>
#include <utility>
#include <type_traits>
// #include "../config/config.h"

// #include "../core/type_traits.hpp"
#ifndef ENTT_CORE_TYPE_TRAITS_HPP
#define ENTT_CORE_TYPE_TRAITS_HPP


#include <cstddef>
#include <utility>
#include <type_traits>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "fwd.hpp"
#ifndef ENTT_CORE_FWD_HPP
#define ENTT_CORE_FWD_HPP


#include <type_traits>
// #include "../config/config.h"



namespace entt {


template<std::size_t Len = sizeof(double[2]), std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_any;


/*! @brief Alias declaration for type identifiers. */
using id_type = ENTT_ID_TYPE;


/*! @brief Alias declaration for the most common use case. */
using any = basic_any<>;


}


#endif



namespace entt {


/**
 * @brief Utility class to disambiguate overloaded functions.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
struct choice_t
    // Unfortunately, doxygen cannot parse such a construct.
    /*! @cond TURN_OFF_DOXYGEN */
    : choice_t<N-1>
    /*! @endcond */
{};


/*! @copybrief choice_t */
template<>
struct choice_t<0> {};


/**
 * @brief Variable template for the choice trick.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
inline constexpr choice_t<N> choice{};


/**
 * @brief Identity type trait.
 *
 * Useful to establish non-deduced contexts in template argument deduction
 * (waiting for C++20) or to provide types through function arguments.
 *
 * @tparam Type A type.
 */
template<typename Type>
struct type_identity {
    /*! @brief Identity type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Type A type.
 */
template<typename Type>
using type_identity_t = typename type_identity<Type>::type;


/**
 * @brief A type-only `sizeof` wrapper that returns 0 where `sizeof` complains.
 * @tparam Type The type of which to return the size.
 * @tparam The size of the type if `sizeof` accepts it, 0 otherwise.
 */
template<typename Type, typename = void>
struct size_of: std::integral_constant<std::size_t, 0u> {};


/*! @copydoc size_of */
template<typename Type>
struct size_of<Type, std::void_t<decltype(sizeof(Type))>>
    : std::integral_constant<std::size_t, sizeof(Type)>
{};


/**
 * @brief Helper variable template.
 * @tparam Type The type of which to return the size.
 */
template<class Type>
inline constexpr auto size_of_v = size_of<Type>::value;


/**
 * @brief Using declaration to be used to _repeat_ the same type a number of
 * times equal to the size of a given parameter pack.
 * @tparam Type A type to repeat.
 */
template<typename Type, typename>
using unpack_as_t = Type;


/**
 * @brief Helper variable template to be used to _repeat_ the same value a
 * number of times equal to the size of a given parameter pack.
 * @tparam Value A value to repeat.
 */
template<auto Value, typename>
inline constexpr auto unpack_as_v = Value;


/**
 * @brief Wraps a static constant.
 * @tparam Value A static constant.
 */
template<auto Value>
using integral_constant = std::integral_constant<decltype(Value), Value>;


/**
 * @brief Alias template to facilitate the creation of named values.
 * @tparam Value A constant value at least convertible to `id_type`.
 */
template<id_type Value>
using tag = integral_constant<Value>;


/**
 * @brief A class to use to push around lists of types, nothing more.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list {
    /*! @brief Type list type. */
    using type = type_list;
    /*! @brief Compile-time number of elements in the type list. */
    static constexpr auto size = sizeof...(Type);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct type_list_element;


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Index Index of the type to return.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<std::size_t Index, typename Type, typename... Other>
struct type_list_element<Index, type_list<Type, Other...>>
    : type_list_element<Index - 1u, type_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<typename Type, typename... Other>
struct type_list_element<0u, type_list<Type, Other...>> {
    /*! @brief Searched type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the type to return.
 * @tparam List Type list to search into.
 */
template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @return A type list composed by the types of both the type lists.
 */
template<typename... Type, typename... Other>
constexpr type_list<Type..., Other...> operator+(type_list<Type...>, type_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_cat;


/*! @brief Concatenates multiple type lists. */
template<>
struct type_list_cat<> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<>;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @tparam List Other type lists, if any.
 */
template<typename... Type, typename... Other, typename... List>
struct type_list_cat<type_list<Type...>, type_list<Other...>, List...> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = typename type_list_cat<type_list<Type..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list_cat<type_list<Type...>> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<Type...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists to concatenate.
 */
template<typename... List>
using type_list_cat_t = typename type_list_cat<List...>::type;


/*! @brief Primary template isn't defined on purpose. */
template<typename>
struct type_list_unique;


/**
 * @brief Removes duplicates types from a type list.
 * @tparam Type One of the types provided by the given type list.
 * @tparam Other The other types provided by the given type list.
 */
template<typename Type, typename... Other>
struct type_list_unique<type_list<Type, Other...>> {
    /*! @brief A type list without duplicate types. */
    using type = std::conditional_t<
        std::disjunction_v<std::is_same<Type, Other>...>,
        typename type_list_unique<type_list<Other...>>::type,
        type_list_cat_t<type_list<Type>, typename type_list_unique<type_list<Other...>>::type>
    >;
};


/*! @brief Removes duplicates types from a type list. */
template<>
struct type_list_unique<type_list<>> {
    /*! @brief A type list without duplicate types. */
    using type = type_list<>;
};


/**
 * @brief Helper type.
 * @tparam Type A type list.
 */
template<typename Type>
using type_list_unique_t = typename type_list_unique<Type>::type;


/**
 * @brief Provides the member constant `value` to true if a type list contains a
 * given type, false otherwise.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<typename List, typename Type>
struct type_list_contains;


/**
 * @copybrief type_list_contains
 * @tparam Type Types provided by the type list.
 * @tparam Other Type to look for.
 */
template<typename... Type, typename Other>
struct type_list_contains<type_list<Type...>, Other>: std::disjunction<std::is_same<Type, Other>...> {};


/**
 * @brief Helper variable template.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<class List, typename Type>
inline constexpr auto type_list_contains_v = type_list_contains<List, Type>::value;


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_diff;


/**
 * @brief Computes the difference between two type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 */
template<typename... Type, typename... Other>
struct type_list_diff<type_list<Type...>, type_list<Other...>> {
    /*! @brief A type list that is the difference between the two type lists. */
    using type = type_list_cat_t<std::conditional_t<type_list_contains_v<type_list<Other...>, Type>, type_list<>, type_list<Type>>...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists between which to compute the difference.
 */
template<typename... List>
using type_list_diff_t = typename type_list_diff<List...>::type;


/**
 * @brief A class to use to push around lists of constant values, nothing more.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list {
    /*! @brief Value list type. */
    using type = value_list;
    /*! @brief Compile-time number of elements in the value list. */
    static constexpr auto size = sizeof...(Value);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct value_list_element;


/**
 * @brief Provides compile-time indexed access to the values of a value list.
 * @tparam Index Index of the value to return.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<std::size_t Index, auto Value, auto... Other>
struct value_list_element<Index, value_list<Value, Other...>>
    : value_list_element<Index - 1u, value_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<auto Value, auto... Other>
struct value_list_element<0u, value_list<Value, Other...>> {
    /*! @brief Searched value. */
    static constexpr auto value = Value;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the value to return.
 * @tparam List Value list to search into.
 */
template<std::size_t Index, typename List>
inline constexpr auto value_list_element_v = value_list_element<Index, List>::value;


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @return A value list composed by the values of both the value lists.
 */
template<auto... Value, auto... Other>
constexpr value_list<Value..., Other...> operator+(value_list<Value...>, value_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct value_list_cat;


/*! @brief Concatenates multiple value lists. */
template<>
struct value_list_cat<> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<>;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @tparam List Other value lists, if any.
 */
template<auto... Value, auto... Other, typename... List>
struct value_list_cat<value_list<Value...>, value_list<Other...>, List...> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = typename value_list_cat<value_list<Value..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list_cat<value_list<Value...>> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<Value...>;
};


/**
 * @brief Helper type.
 * @tparam List Value lists to concatenate.
 */
template<typename... List>
using value_list_cat_t = typename value_list_cat<List...>::type;


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
[[nodiscard]] constexpr bool is_equality_comparable(...) { return false; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<0>)
-> decltype(std::declval<Type>() == std::declval<Type>()) { return true; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<1>)
-> decltype(std::declval<typename Type::value_type>(), std::declval<Type>() == std::declval<Type>()) {
    if constexpr(std::is_same_v<typename Type::value_type, Type>) {
        return is_equality_comparable<Type>(choice<0>);
    } else {
        return is_equality_comparable<typename Type::value_type>(choice<2>);
    }
}


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<2>)
-> decltype(std::declval<typename Type::mapped_type>(), std::declval<Type>() == std::declval<Type>()) {
    return is_equality_comparable<typename Type::key_type>(choice<2>) && is_equality_comparable<typename Type::mapped_type>(choice<2>);
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Provides the member constant `value` to true if a given type is
 * equality comparable, false otherwise.
 * @tparam Type Potentially equality comparable type.
 */
template<typename Type, typename = void>
struct is_equality_comparable: std::bool_constant<internal::is_equality_comparable<Type>(choice<2>)> {};


/**
 * @brief Helper variable template.
 * @tparam Type Potentially equality comparable type.
 */
template<class Type>
inline constexpr auto is_equality_comparable_v = is_equality_comparable<Type>::value;


/*! @brief Same as std::is_invocable, but with tuples. */
template<typename, typename>
struct is_applicable: std::false_type {};


/**
 * @copybrief is_applicable
 * @tparam Func A valid function type.
 * @tparam Tuple Tuple-like type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
* @copybrief is_applicable
* @tparam Func A valid function type.
* @tparam Tuple Tuple-like type.
* @tparam Args The list of arguments to use to probe the function type.
*/
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, const Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, typename Args>
inline constexpr auto is_applicable_v = is_applicable<Func, Args>::value;


/*! @brief Same as std::is_invocable_r, but with tuples for arguments. */
template<typename, typename, typename>
struct is_applicable_r: std::false_type {};


/**
 * @copybrief is_applicable_r
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename... Args>
struct is_applicable_r<Ret, Func, std::tuple<Args...>>: std::is_invocable_r<Ret, Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename Args>
inline constexpr auto is_applicable_r_v = is_applicable_r<Ret, Func, Args>::value;


/**
* @brief Provides the member constant `value` to true if a given type is
* complete, false otherwise.
* @tparam Type Potential complete type.
*/
template<typename Type, typename = void>
struct is_complete: std::false_type {};


/*! @copydoc is_complete */
template<typename Type>
struct is_complete<Type, std::void_t<decltype(sizeof(Type))>>: std::true_type {};


/**
* @brief Helper variable template.
* @tparam Type Potential complete type.
*/
template<typename Type>
inline constexpr auto is_complete_v = is_complete<Type>::value;


/**
 * @brief Transcribes the constness of a type to another type.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
struct constness_as {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::remove_const_t<To>;
};


/*! @copydoc constness_as */
template<typename To, typename From>
struct constness_as<To, const From> {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::add_const_t<To>;
};


/**
 * @brief Alias template to facilitate the transcription of the constness.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;


/**
 * @brief Extracts the class of a non-static member object or function.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
class member_class {
    static_assert(std::is_member_pointer_v<Member>, "Invalid pointer type to non-static member object or function");

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...));

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...) const);

    template<typename Class, typename Type>
    static Class * clazz(Type Class:: *);

public:
    /*! @brief The class of the given non-static member object or function. */
    using type = std::remove_pointer_t<decltype(clazz(std::declval<Member>()))>;
};


/**
 * @brief Helper type.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
using member_class_t = typename member_class<Member>::type;


}


#endif

// #include "entity.hpp"

// #include "fwd.hpp"
#ifndef ENTT_ENTITY_FWD_HPP
#define ENTT_ENTITY_FWD_HPP


#include <memory>
// #include "../core/fwd.hpp"



namespace entt {


template<typename Entity, typename = std::allocator<Entity>>
class basic_sparse_set;


template<typename, typename Type, typename = std::allocator<Type>>
struct basic_storage;


template<typename>
class basic_registry;


template<typename...>
struct basic_view;


template<typename>
class basic_runtime_view;


template<typename...>
class basic_group;


template<typename>
class basic_observer;


template<typename>
class basic_organizer;


template<typename, typename...>
struct basic_handle;


template<typename>
class basic_snapshot;


template<typename>
class basic_snapshot_loader;


template<typename>
class basic_continuous_loader;


/*! @brief Default entity identifier. */
enum class entity: id_type {};


/*! @brief Alias declaration for the most common use case. */
using sparse_set = basic_sparse_set<entity>;


/**
 * @brief Alias declaration for the most common use case.
 * @tparam Args Other template parameters.
 */
template<typename... Args>
using storage = basic_storage<entity, Args...>;


/*! @brief Alias declaration for the most common use case. */
using registry = basic_registry<entity>;


/*! @brief Alias declaration for the most common use case. */
using observer = basic_observer<entity>;


/*! @brief Alias declaration for the most common use case. */
using organizer = basic_organizer<entity>;


/*! @brief Alias declaration for the most common use case. */
using handle = basic_handle<entity>;


/*! @brief Alias declaration for the most common use case. */
using const_handle = basic_handle<const entity>;


/**
 * @brief Alias declaration for the most common use case.
 * @tparam Args Other template parameters.
 */
template<typename... Args>
using handle_view = basic_handle<entity, Args...>;


/**
 * @brief Alias declaration for the most common use case.
 * @tparam Args Other template parameters.
 */
template<typename... Args>
using const_handle_view = basic_handle<const entity, Args...>;


/*! @brief Alias declaration for the most common use case. */
using snapshot = basic_snapshot<entity>;


/*! @brief Alias declaration for the most common use case. */
using snapshot_loader = basic_snapshot_loader<entity>;


/*! @brief Alias declaration for the most common use case. */
using continuous_loader = basic_continuous_loader<entity>;


/**
 * @brief Alias declaration for the most common use case.
 * @tparam Args Other template parameters.
 */
template<typename... Args>
using view = basic_view<entity, Args...>;


/*! @brief Alias declaration for the most common use case. */
using runtime_view = basic_runtime_view<entity>;


/**
 * @brief Alias declaration for the most common use case.
 * @tparam Args Other template parameters.
 */
template<typename... Args>
using group = basic_group<entity, Args...>;


}


#endif

// #include "sparse_set.hpp"
#ifndef ENTT_ENTITY_SPARSE_SET_HPP
#define ENTT_ENTITY_SPARSE_SET_HPP


#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/algorithm.hpp"
#ifndef ENTT_CORE_ALGORITHM_HPP
#define ENTT_CORE_ALGORITHM_HPP


#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>
// #include "utility.hpp"
#ifndef ENTT_CORE_UTILITY_HPP
#define ENTT_CORE_UTILITY_HPP


#include <utility>
// #include "../config/config.h"



namespace entt {


/*! @brief Identity function object (waiting for C++20). */
struct identity {
    /**
     * @brief Returns its argument unchanged.
     * @tparam Type Type of the argument.
     * @param value The actual argument.
     * @return The submitted value as-is.
     */
    template<class Type>
    [[nodiscard]] constexpr Type && operator()(Type &&value) const ENTT_NOEXCEPT {
        return std::forward<Type>(value);
    }
};


/**
 * @brief Constant utility to disambiguate overloaded members of a class.
 * @tparam Type Type of the desired overload.
 * @tparam Class Type of class to which the member belongs.
 * @param member A valid pointer to a member.
 * @return Pointer to the member.
 */
template<typename Type, typename Class>
[[nodiscard]] constexpr auto overload(Type Class:: *member) ENTT_NOEXCEPT { return member; }


/**
 * @brief Constant utility to disambiguate overloaded functions.
 * @tparam Func Function type of the desired overload.
 * @param func A valid pointer to a function.
 * @return Pointer to the function.
 */
template<typename Func>
[[nodiscard]] constexpr auto overload(Func *func) ENTT_NOEXCEPT { return func; }


/**
 * @brief Helper type for visitors.
 * @tparam Func Types of function objects.
 */
template<class... Func>
struct overloaded: Func... {
    using Func::operator()...;
};


/**
 * @brief Deduction guide.
 * @tparam Func Types of function objects.
 */
template<class... Func>
overloaded(Func...)
-> overloaded<Func...>;


/**
 * @brief Basic implementation of a y-combinator.
 * @tparam Func Type of a potentially recursive function.
 */
template<class Func>
struct y_combinator {
    /**
     * @brief Constructs a y-combinator from a given function.
     * @param recursive A potentially recursive function.
     */
    y_combinator(Func recursive):
        func{std::move(recursive)}
    {}

    /**
     * @brief Invokes a y-combinator and therefore its underlying function.
     * @tparam Args Types of arguments to use to invoke the underlying function.
     * @param args Parameters to use to invoke the underlying function.
     * @return Return value of the underlying function, if any.
     */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) const {
        return func(*this, std::forward<Args>(args)...);
    }

    /*! @copydoc operator()() */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) {
        return func(*this, std::forward<Args>(args)...);
    }

private:
    Func func;
};


}


#endif



namespace entt {


/**
 * @brief Function object to wrap `std::sort` in a class type.
 *
 * Unfortunately, `std::sort` cannot be passed as template argument to a class
 * template or a function template.<br/>
 * This class fills the gap by wrapping some flavors of `std::sort` in a
 * function object.
 */
struct std_sort {
    /**
     * @brief Sorts the elements in a range.
     *
     * Sorts the elements in a range using the given binary comparison function.
     *
     * @tparam It Type of random access iterator.
     * @tparam Compare Type of comparison function object.
     * @tparam Args Types of arguments to forward to the sort function.
     * @param first An iterator to the first element of the range to sort.
     * @param last An iterator past the last element of the range to sort.
     * @param compare A valid comparison function object.
     * @param args Arguments to forward to the sort function, if any.
     */
    template<typename It, typename Compare = std::less<>, typename... Args>
    void operator()(It first, It last, Compare compare = Compare{}, Args &&... args) const {
        std::sort(std::forward<Args>(args)..., std::move(first), std::move(last), std::move(compare));
    }
};


/*! @brief Function object for performing insertion sort. */
struct insertion_sort {
    /**
     * @brief Sorts the elements in a range.
     *
     * Sorts the elements in a range using the given binary comparison function.
     *
     * @tparam It Type of random access iterator.
     * @tparam Compare Type of comparison function object.
     * @param first An iterator to the first element of the range to sort.
     * @param last An iterator past the last element of the range to sort.
     * @param compare A valid comparison function object.
     */
    template<typename It, typename Compare = std::less<>>
    void operator()(It first, It last, Compare compare = Compare{}) const {
        if(first < last) {
            for(auto it = first + 1; it < last; ++it) {
                auto value = std::move(*it);
                auto pre = it;

                for(; pre > first && compare(value, *(pre-1)); --pre) {
                    *pre = std::move(*(pre-1));
                }

                *pre = std::move(value);
            }
        }
    }
};


/**
 * @brief Function object for performing LSD radix sort.
 * @tparam Bit Number of bits processed per pass.
 * @tparam N Maximum number of bits to sort.
 */
template<std::size_t Bit, std::size_t N>
struct radix_sort {
    static_assert((N % Bit) == 0, "The maximum number of bits to sort must be a multiple of the number of bits processed per pass");

    /**
     * @brief Sorts the elements in a range.
     *
     * Sorts the elements in a range using the given _getter_ to access the
     * actual data to be sorted.
     *
     * This implementation is inspired by the online book
     * [Physically Based Rendering](http://www.pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies.html#RadixSort).
     *
     * @tparam It Type of random access iterator.
     * @tparam Getter Type of _getter_ function object.
     * @param first An iterator to the first element of the range to sort.
     * @param last An iterator past the last element of the range to sort.
     * @param getter A valid _getter_ function object.
     */
    template<typename It, typename Getter = identity>
    void operator()(It first, It last, Getter getter = Getter{}) const {
        if(first < last) {
            static constexpr auto mask = (1 << Bit) - 1;
            static constexpr auto buckets = 1 << Bit;
            static constexpr auto passes = N / Bit;

            using value_type = typename std::iterator_traits<It>::value_type;
            std::vector<value_type> aux(std::distance(first, last));

            auto part = [getter = std::move(getter)](auto from, auto to, auto out, auto start) {
                std::size_t index[buckets]{};
                std::size_t count[buckets]{};

                for(auto it = from; it != to; ++it) {
                    ++count[(getter(*it) >> start) & mask];
                }

                for(std::size_t pos{}, end = buckets - 1u; pos < end; ++pos) {
                    index[pos + 1u] = index[pos] + count[pos];
                }

                for(auto it = from; it != to; ++it) {
                    out[index[(getter(*it) >> start) & mask]++] = std::move(*it);
                }
            };

            for(std::size_t pass = 0; pass < (passes & ~1); pass += 2) {
                part(first, last, aux.begin(), pass * Bit);
                part(aux.begin(), aux.end(), first, (pass + 1) * Bit);
            }

            if constexpr(passes & 1) {
                part(first, last, aux.begin(), (passes - 1) * Bit);
                std::move(aux.begin(), aux.end(), first);
            }
        }
    }
};


}


#endif

// #include "../core/fwd.hpp"

// #include "entity.hpp"

// #include "fwd.hpp"



namespace entt {


/*! @brief Sparse set deletion policy. */
enum class deletion_policy: std::uint8_t {
    /*! @brief Swap-and-pop deletion policy. */
    swap_and_pop = 0u,
    /*! @brief In-place deletion policy. */
    in_place = 1u
};


/**
 * @brief Basic sparse set implementation.
 *
 * Sparse set or packed array or whatever is the name users give it.<br/>
 * Two arrays: an _external_ one and an _internal_ one; a _sparse_ one and a
 * _packed_ one; one used for direct access through contiguous memory, the other
 * one used to get the data through an extra level of indirection.<br/>
 * This is largely used by the registry to offer users the fastest access ever
 * to the components. Views and groups in general are almost entirely designed
 * around sparse sets.
 *
 * This type of data structure is widely documented in the literature and on the
 * web. This is nothing more than a customized implementation suitable for the
 * purpose of the framework.
 *
 * @note
 * Internal data structures arrange elements to maximize performance. There are
 * no guarantees that entities are returned in the insertion order when iterate
 * a sparse set. Do not make assumption on the order in any case.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Allocator Type of allocator used to manage memory and elements.
 */
template<typename Entity, typename Allocator>
class basic_sparse_set {
    static constexpr auto growth_factor = 1.5;
    static constexpr auto sparse_page = ENTT_SPARSE_PAGE;

    using traits_type = entt_traits<Entity>;

    using alloc_traits = typename std::allocator_traits<Allocator>::template rebind_traits<Entity>;
    using alloc_pointer = typename alloc_traits::pointer;
    using alloc_const_pointer = typename alloc_traits::const_pointer;

    using bucket_alloc_traits = typename std::allocator_traits<Allocator>::template rebind_traits<alloc_pointer>;
    using bucket_alloc_pointer = typename bucket_alloc_traits::pointer;

    static_assert(alloc_traits::propagate_on_container_move_assignment::value);
    static_assert(bucket_alloc_traits::propagate_on_container_move_assignment::value);

    struct sparse_set_iterator final {
        using difference_type = typename traits_type::difference_type;
        using value_type = Entity;
        using pointer = const value_type *;
        using reference = const value_type &;
        using iterator_category = std::random_access_iterator_tag;

        sparse_set_iterator() ENTT_NOEXCEPT = default;

        sparse_set_iterator(const alloc_const_pointer *ref, const difference_type idx) ENTT_NOEXCEPT
            : packed{ref},
              index{idx}
        {}

        sparse_set_iterator & operator++() ENTT_NOEXCEPT {
            return --index, *this;
        }

        sparse_set_iterator operator++(int) ENTT_NOEXCEPT {
            iterator orig = *this;
            return ++(*this), orig;
        }

        sparse_set_iterator & operator--() ENTT_NOEXCEPT {
            return ++index, *this;
        }

        sparse_set_iterator operator--(int) ENTT_NOEXCEPT {
            sparse_set_iterator orig = *this;
            return operator--(), orig;
        }

        sparse_set_iterator & operator+=(const difference_type value) ENTT_NOEXCEPT {
            index -= value;
            return *this;
        }

        sparse_set_iterator operator+(const difference_type value) const ENTT_NOEXCEPT {
            sparse_set_iterator copy = *this;
            return (copy += value);
        }

        sparse_set_iterator & operator-=(const difference_type value) ENTT_NOEXCEPT {
            return (*this += -value);
        }

        sparse_set_iterator operator-(const difference_type value) const ENTT_NOEXCEPT {
            return (*this + -value);
        }

        difference_type operator-(const sparse_set_iterator &other) const ENTT_NOEXCEPT {
            return other.index - index;
        }

        [[nodiscard]] reference operator[](const difference_type value) const {
            const auto pos = size_type(index-value-1u);
            return (*packed)[pos];
        }

        [[nodiscard]] bool operator==(const sparse_set_iterator &other) const ENTT_NOEXCEPT {
            return other.index == index;
        }

        [[nodiscard]] bool operator!=(const sparse_set_iterator &other) const ENTT_NOEXCEPT {
            return !(*this == other);
        }

        [[nodiscard]] bool operator<(const sparse_set_iterator &other) const ENTT_NOEXCEPT {
            return index > other.index;
        }

        [[nodiscard]] bool operator>(const sparse_set_iterator &other) const ENTT_NOEXCEPT {
            return index < other.index;
        }

        [[nodiscard]] bool operator<=(const sparse_set_iterator &other) const ENTT_NOEXCEPT {
            return !(*this > other);
        }

        [[nodiscard]] bool operator>=(const sparse_set_iterator &other) const ENTT_NOEXCEPT {
            return !(*this < other);
        }

        [[nodiscard]] pointer operator->() const {
            const auto pos = size_type(index-1u);
            return std::addressof((*packed)[pos]);
        }

        [[nodiscard]] reference operator*() const {
            return *operator->();
        }

    private:
        const alloc_const_pointer *packed;
        difference_type index;
    };

    [[nodiscard]] static auto page(const Entity entt) ENTT_NOEXCEPT {
        return size_type{traits_type::to_entity(entt) / sparse_page};
    }

    [[nodiscard]] static auto offset(const Entity entt) ENTT_NOEXCEPT {
        return size_type{traits_type::to_entity(entt) & (sparse_page - 1)};
    }

    [[nodiscard]] auto assure_page(const std::size_t idx) {
        if(!(idx < bucket)) {
            const size_type sz = idx + 1u;
            const auto mem = bucket_alloc_traits::allocate(bucket_allocator, sz);

            std::uninitialized_value_construct(mem + bucket, mem + sz);
            std::uninitialized_copy(sparse, sparse + bucket, mem);

            std::destroy(sparse, sparse + bucket);
            bucket_alloc_traits::deallocate(bucket_allocator, sparse, bucket);

            sparse = mem;
            bucket = sz;
        }

        if(!sparse[idx]) {
            sparse[idx] = alloc_traits::allocate(allocator, sparse_page);
            std::uninitialized_fill(sparse[idx], sparse[idx] + sparse_page, null);
        }

        return sparse[idx];
    }

    void resize_packed(const std::size_t req) {
        ENTT_ASSERT((req != reserved) && !(req < count), "Invalid request");
        const auto mem = alloc_traits::allocate(allocator, req);

        std::uninitialized_copy(packed, packed + count, mem);
        std::uninitialized_fill(mem + count, mem + req, tombstone);

        std::destroy(packed, packed + reserved);
        alloc_traits::deallocate(allocator, packed, reserved);

        packed = mem;
        reserved = req;
    }

    void release_memory() {
        if(packed) {
            for(size_type pos{}; pos < bucket; ++pos) {
                if(sparse[pos]) {
                    std::destroy(sparse[pos], sparse[pos] + sparse_page);
                    alloc_traits::deallocate(allocator, sparse[pos], sparse_page);
                }
            }

            std::destroy(packed, packed + reserved);
            std::destroy(sparse, sparse + bucket);
            alloc_traits::deallocate(allocator, packed, reserved);
            bucket_alloc_traits::deallocate(bucket_allocator, sparse, bucket);
        }
    }

    void stable_erase(const Entity entt, void *ud = nullptr) {
        // last chance to use the entity for derived classes and mixins, if any
        about_to_pop(entt, ud);

        auto &ref = sparse[page(entt)][offset(entt)];
        const auto pos = size_type{traits_type::to_entity(ref)};
        ENTT_ASSERT(packed[pos] == entt, "Invalid entity identifier");

        packed[pos] = std::exchange(free_list, traits_type::construct(static_cast<typename traits_type::entity_type>(pos)));
        ref = null;

        // strong exception guarantee
        in_place_pop(pos);
    }

    void unstable_erase(const Entity entt, void *ud = nullptr) {
        // last chance to use the entity for derived classes and mixins, if any
        about_to_pop(entt, ud);

        auto &ref = sparse[page(entt)][offset(entt)];
        const auto pos = size_type{traits_type::to_entity(ref)};
        ENTT_ASSERT(packed[pos] == entt, "Invalid entity identifier");

        auto &last = packed[count - 1u];

        packed[pos] = last;
        sparse[page(last)][offset(last)] = ref;
        // lazy self-assignment guard
        ref = null;
        // unnecessary but it helps to detect nasty bugs
        ENTT_ASSERT((last = tombstone, true), "");

        --count;

        ENTT_TRY {
            // strong exception guarantee
            swap_and_pop(pos);
        } ENTT_CATCH {
            last = std::exchange(packed[pos], entt);
            ref = std::exchange(sparse[page(last)][offset(last)], traits_type::construct(static_cast<typename traits_type::entity_type>(count++)));
            ENTT_THROW;
        }
    }

protected:
    /**
     * @brief Swaps two entities in the internal packed array.
     * @param lhs A valid position of an entity within storage.
     * @param rhs A valid position of an entity within storage.
     */
    virtual void swap_at([[maybe_unused]] const std::size_t lhs, [[maybe_unused]] const std::size_t rhs) {}

    /**
     * @brief Attempts to move an entity in the internal packed array.
     * @param from A valid position of an entity within storage.
     * @param to A valid position of an entity within storage.
     */
    virtual void move_and_pop([[maybe_unused]] const std::size_t from, [[maybe_unused]] const std::size_t to) {}

    /**
     * @brief Attempts to erase an entity from the internal packed array.
     * @param pos A valid position of an entity within storage.
     */
    virtual void swap_and_pop([[maybe_unused]] const std::size_t pos) {}

    /**
     * @brief Attempts to erase an entity from the internal packed array.
     * @param pos A valid position of an entity within storage.
     */
    virtual void in_place_pop([[maybe_unused]] const std::size_t pos) {}

    /**
     * @brief Last chance to use an entity that is about to be erased.
     * @param entity A valid entity identifier.
     * @param ud Optional user data that are forwarded as-is to derived classes.
     */
    virtual void about_to_pop([[maybe_unused]] const Entity entity, [[maybe_unused]] void *ud) {}

public:
    /*! @brief Allocator type. */
    using allocator_type = typename alloc_traits::allocator_type;
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Pointer type to contained entities. */
    using pointer = alloc_const_pointer;
    /*! @brief Random access iterator type. */
    using iterator = sparse_set_iterator;
    /*! @brief Reverse iterator type. */
    using reverse_iterator = std::reverse_iterator<iterator>;

    /**
     * @brief Constructs an empty container with the given policy and allocator.
     * @param pol Type of deletion policy.
     * @param alloc Allocator to use (possibly default-constructed).
     */
    explicit basic_sparse_set(deletion_policy pol, const allocator_type &alloc = {})
        : allocator{alloc},
          bucket_allocator{alloc},
          sparse{bucket_alloc_traits::allocate(bucket_allocator, 0u)},
          packed{alloc_traits::allocate(allocator, 0u)},
          bucket{0u},
          count{0u},
          reserved{0u},
          free_list{tombstone},
          mode{pol}
    {}

    /**
     * @brief Constructs an empty container with the given allocator.
     * @param alloc Allocator to use (possibly default-constructed).
     */
    explicit basic_sparse_set(const allocator_type &alloc = {})
        : basic_sparse_set{deletion_policy::swap_and_pop, alloc}
    {}

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    basic_sparse_set(basic_sparse_set &&other) ENTT_NOEXCEPT
        : allocator{std::move(other.allocator)},
          bucket_allocator{std::move(other.bucket_allocator)},
          sparse{std::exchange(other.sparse, bucket_alloc_pointer{})},
          packed{std::exchange(other.packed, alloc_pointer{})},
          bucket{std::exchange(other.bucket, 0u)},
          count{std::exchange(other.count, 0u)},
          reserved{std::exchange(other.reserved, 0u)},
          free_list{std::exchange(other.free_list, tombstone)},
          mode{other.mode}
    {}

    /*! @brief Default destructor. */
    virtual ~basic_sparse_set() {
        release_memory();
    }

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This sparse set.
     */
    basic_sparse_set & operator=(basic_sparse_set &&other) ENTT_NOEXCEPT {
        release_memory();

        allocator = std::move(other.allocator);
        bucket_allocator = std::move(other.bucket_allocator);
        sparse = std::exchange(other.sparse, bucket_alloc_pointer{});
        packed = std::exchange(other.packed, alloc_pointer{});
        bucket = std::exchange(other.bucket, 0u);
        count = std::exchange(other.count, 0u);
        reserved = std::exchange(other.reserved, 0u);
        free_list = std::exchange(other.free_list, tombstone);
        mode = other.mode;

        return *this;
    }

    /**
     * @brief Returns the deletion policy of a sparse set.
     * @return The deletion policy of the sparse set.
     */
    deletion_policy policy() const ENTT_NOEXCEPT {
        return mode;
    }

    /**
     * @brief Increases the capacity of a sparse set.
     *
     * If the new capacity is greater than the current capacity, new storage is
     * allocated, otherwise the method does nothing.
     *
     * @param cap Desired capacity.
     */
    void reserve(const size_type cap) {
        if(cap > reserved) {
            resize_packed(cap);
        }
    }

    /**
     * @brief Returns the number of elements that a sparse set has currently
     * allocated space for.
     * @return Capacity of the sparse set.
     */
    [[nodiscard]] size_type capacity() const ENTT_NOEXCEPT {
        return reserved;
    }

    /*! @brief Requests the removal of unused capacity. */
    void shrink_to_fit() {
        if(count < reserved) {
            resize_packed(count);
        }
    }

    /**
     * @brief Returns the extent of a sparse set.
     *
     * The extent of a sparse set is also the size of the internal sparse array.
     * There is no guarantee that the internal packed array has the same size.
     * Usually the size of the internal sparse array is equal or greater than
     * the one of the internal packed array.
     *
     * @return Extent of the sparse set.
     */
    [[nodiscard]] size_type extent() const ENTT_NOEXCEPT {
        return bucket * sparse_page;
    }

    /**
     * @brief Returns the number of elements in a sparse set.
     *
     * The number of elements is also the size of the internal packed array.
     * There is no guarantee that the internal sparse array has the same size.
     * Usually the size of the internal sparse array is equal or greater than
     * the one of the internal packed array.
     *
     * @return Number of elements.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return count;
    }

    /**
     * @brief Checks whether a sparse set is empty.
     * @return True if the sparse set is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return (count == size_type{});
    }

    /**
     * @brief Direct access to the internal packed array.
     * @return A pointer to the internal packed array.
     */
    [[nodiscard]] pointer data() const ENTT_NOEXCEPT {
        return packed;
    }

    /**
     * @brief Returns an iterator to the beginning.
     *
     * The returned iterator points to the first entity of the internal packed
     * array. If the sparse set is empty, the returned iterator will be equal to
     * `end()`.
     *
     * @return An iterator to the first entity of the internal packed array.
     */
    [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
        return iterator{std::addressof(packed), static_cast<typename traits_type::difference_type>(count)};
    }

    /**
     * @brief Returns an iterator to the end.
     *
     * The returned iterator points to the element following the last entity in
     * the internal packed array. Attempting to dereference the returned
     * iterator results in undefined behavior.
     *
     * @return An iterator to the element following the last entity of the
     * internal packed array.
     */
    [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
        return iterator{std::addressof(packed), {}};
    }

    /**
     * @brief Returns a reverse iterator to the beginning.
     *
     * The returned iterator points to the first entity of the reversed internal
     * packed array. If the sparse set is empty, the returned iterator will be
     * equal to `rend()`.
     *
     * @return An iterator to the first entity of the reversed internal packed
     * array.
     */
    [[nodiscard]] reverse_iterator rbegin() const ENTT_NOEXCEPT {
        return std::make_reverse_iterator(end());
    }

    /**
     * @brief Returns a reverse iterator to the end.
     *
     * The returned iterator points to the element following the last entity in
     * the reversed internal packed array. Attempting to dereference the
     * returned iterator results in undefined behavior.
     *
     * @return An iterator to the element following the last entity of the
     * reversed internal packed array.
     */
    [[nodiscard]] reverse_iterator rend() const ENTT_NOEXCEPT {
        return std::make_reverse_iterator(begin());
    }

    /**
     * @brief Finds an entity.
     * @param entt A valid entity identifier.
     * @return An iterator to the given entity if it's found, past the end
     * iterator otherwise.
     */
    [[nodiscard]] iterator find(const entity_type entt) const ENTT_NOEXCEPT {
        return contains(entt) ? --(end() - index(entt)) : end();
    }

    /**
     * @brief Checks if a sparse set contains an entity.
     * @param entt A valid entity identifier.
     * @return True if the sparse set contains the entity, false otherwise.
     */
    [[nodiscard]] bool contains(const entity_type entt) const ENTT_NOEXCEPT {
        const auto curr = page(entt);
        // testing versions permits to avoid accessing the packed array
        return (curr < bucket && sparse[curr] && sparse[curr][offset(entt)] != null);
    }

    /**
     * @brief Returns the position of an entity in a sparse set.
     *
     * @warning
     * Attempting to get the position of an entity that doesn't belong to the
     * sparse set results in undefined behavior.
     *
     * @param entt A valid entity identifier.
     * @return The position of the entity in the sparse set.
     */
    [[nodiscard]] size_type index(const entity_type entt) const ENTT_NOEXCEPT {
        ENTT_ASSERT(contains(entt), "Set does not contain entity");
        return size_type{traits_type::to_entity(sparse[page(entt)][offset(entt)])};
    }

    /**
     * @brief Returns the entity at specified location, with bounds checking.
     * @param pos The position for which to return the entity.
     * @return The entity at specified location if any, a null entity otherwise.
     */
    [[nodiscard]] entity_type at(const size_type pos) const ENTT_NOEXCEPT {
        return pos < count ? packed[pos] : null;
    }

    /**
     * @brief Returns the entity at specified location, without bounds checking.
     * @param pos The position for which to return the entity.
     * @return The entity at specified location.
     */
    [[nodiscard]] entity_type operator[](const size_type pos) const ENTT_NOEXCEPT {
        ENTT_ASSERT(pos < count, "Position is out of bounds");
        return packed[pos];
    }

    /**
     * @brief Assigns an entity to a sparse set.
     *
     * @warning
     * Attempting to assign an entity that already belongs to the sparse set
     * results in undefined behavior.
     *
     * @param entt A valid entity identifier.
     */
    void emplace(const entity_type entt) {
        ENTT_ASSERT(!contains(entt), "Set already contains entity");

        if(free_list == null) {
            if(count == reserved) {
                const size_type sz = static_cast<size_type>(reserved * growth_factor);
                resize_packed(sz + !(sz > reserved));
            }

            assure_page(page(entt))[offset(entt)] = traits_type::construct(static_cast<typename traits_type::entity_type>(count));
            packed[count++] = entt;
        } else {
            const auto pos = size_type{traits_type::to_entity(free_list)};
            move_and_pop(count, pos);
            sparse[page(entt)][offset(entt)] = traits_type::construct(static_cast<typename traits_type::entity_type>(pos));
            free_list = std::exchange(packed[pos], entt);
        }
    }

    /**
     * @brief Assigns one or more entities to a sparse set.
     *
     * @warning
     * Attempting to assign an entity that already belongs to the sparse set
     * results in undefined behavior.
     *
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     */
    template<typename It>
    void insert(It first, It last) {
        reserve(count + std::distance(first, last));

        for(; first != last; ++first) {
            const auto entt = *first;
            ENTT_ASSERT(!contains(entt), "Set already contains entity");
            assure_page(page(entt))[offset(entt)] = traits_type::construct(static_cast<typename traits_type::entity_type>(count));
            packed[count++] = entt;
        }
    }

    /**
     * @brief Erases an entity from a sparse set.
     *
     * @warning
     * Attempting to erase an entity that doesn't belong to the sparse set
     * results in undefined behavior.
     *
     * @param entt A valid entity identifier.
     * @param ud Optional user data that are forwarded as-is to derived classes.
     */
    void erase(const entity_type entt, void *ud = nullptr) {
        ENTT_ASSERT(contains(entt), "Set does not contain entity");
        (mode == deletion_policy::in_place) ? stable_erase(entt, ud) : unstable_erase(entt, ud);
    }

    /**
     * @brief Erases entities from a set.
     *
     * @sa erase
     *
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @param ud Optional user data that are forwarded as-is to derived classes.
     */
    template<typename It>
    void erase(It first, It last, void *ud = nullptr) {
        for(; first != last; ++first) {
            erase(*first, ud);
        }
    }

    /**
     * @brief Removes an entity from a sparse set if it exists.
     * @param entt A valid entity identifier.
     * @param ud Optional user data that are forwarded as-is to derived classes.
     * @return True if the entity is actually removed, false otherwise.
     */
    bool remove(const entity_type entt, void *ud = nullptr) {
        if(contains(entt)) {
            erase(entt, ud);
            return true;
        }

        return false;
    }

    /**
     * @brief Removes entities from a sparse set if they exist.
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @param ud Optional user data that are forwarded as-is to derived classes.
     * @return The number of entities actually removed.
     */
    template<typename It>
    size_type remove(It first, It last, void *ud = nullptr) {
        size_type found{};

        for(; first != last; ++first) {
            found += remove(*first, ud);
        }

        return found;
    }

    /*! @brief Removes all tombstones from the packed array of a sparse set. */
    void compact() {
        size_type next = count;
        for(; next && packed[next - 1u] == tombstone; --next);

        for(auto *it = &free_list; *it != null && next; it = std::addressof(packed[traits_type::to_entity(*it)])) {
            if(const size_type pos = traits_type::to_entity(*it); pos < next) {
                --next;
                move_and_pop(next, pos);
                std::swap(packed[next], packed[pos]);
                sparse[page(packed[pos])][offset(packed[pos])] = traits_type::construct(static_cast<const typename traits_type::entity_type>(pos));
                *it = traits_type::construct(static_cast<typename traits_type::entity_type>(next));
                for(; next && packed[next - 1u] == tombstone; --next);
            }
        }

        free_list = tombstone;
        count = next;
    }

    /**
     * @copybrief swap_at
     *
     * For what it's worth, this function affects both the internal sparse array
     * and the internal packed array. Users should not care of that anyway.
     *
     * @warning
     * Attempting to swap entities that don't belong to the sparse set results
     * in undefined behavior.
     *
     * @param lhs A valid entity identifier.
     * @param rhs A valid entity identifier.
     */
    void swap(const entity_type lhs, const entity_type rhs) {
        const auto from = index(lhs);
        const auto to = index(rhs);

        // basic no-leak guarantee (with invalid state) if swapping throws
        swap_at(from, to);
        std::swap(sparse[page(lhs)][offset(lhs)], sparse[page(rhs)][offset(rhs)]);
        std::swap(packed[from], packed[to]);
    }

    /**
     * @brief Sort the first count elements according to the given comparison
     * function.
     *
     * The comparison function object must return `true` if the first element
     * is _less_ than the second one, `false` otherwise. The signature of the
     * comparison function should be equivalent to the following:
     *
     * @code{.cpp}
     * bool(const Entity, const Entity);
     * @endcode
     *
     * Moreover, the comparison function object shall induce a
     * _strict weak ordering_ on the values.
     *
     * The sort function object must offer a member function template
     * `operator()` that accepts three arguments:
     *
     * * An iterator to the first element of the range to sort.
     * * An iterator past the last element of the range to sort.
     * * A comparison function to use to compare the elements.
     *
     * @tparam Compare Type of comparison function object.
     * @tparam Sort Type of sort function object.
     * @tparam Args Types of arguments to forward to the sort function object.
     * @param length Number of elements to sort.
     * @param compare A valid comparison function object.
     * @param algo A valid sort function object.
     * @param args Arguments to forward to the sort function object, if any.
     */
    template<typename Compare, typename Sort = std_sort, typename... Args>
    void sort_n(const size_type length, Compare compare, Sort algo = Sort{}, Args &&... args) {
        // basic no-leak guarantee (with invalid state) if sorting throws
        ENTT_ASSERT(!(length > count), "Length exceeds the number of elements");
        compact();

        algo(std::make_reverse_iterator(packed + length), std::make_reverse_iterator(packed), std::move(compare), std::forward<Args>(args)...);

        for(size_type pos{}; pos < length; ++pos) {
            auto curr = pos;
            auto next = index(packed[curr]);

            while(curr != next) {
                const auto idx = index(packed[next]);
                const auto entt = packed[curr];

                swap_at(next, idx);
                sparse[page(entt)][offset(entt)] = traits_type::construct(static_cast<typename traits_type::entity_type>(curr));
                curr = std::exchange(next, idx);
            }
        }
    }

    /**
     * @brief Sort all elements according to the given comparison function.
     *
     * @sa sort_n
     *
     * @tparam Compare Type of comparison function object.
     * @tparam Sort Type of sort function object.
     * @tparam Args Types of arguments to forward to the sort function object.
     * @param compare A valid comparison function object.
     * @param algo A valid sort function object.
     * @param args Arguments to forward to the sort function object, if any.
     */
    template<typename Compare, typename Sort = std_sort, typename... Args>
    void sort(Compare compare, Sort algo = Sort{}, Args &&... args) {
        sort_n(count, std::move(compare), std::move(algo), std::forward<Args>(args)...);
    }

    /**
     * @brief Sort entities according to their order in another sparse set.
     *
     * Entities that are part of both the sparse sets are ordered internally
     * according to the order they have in `other`. All the other entities goes
     * to the end of the list and there are no guarantees on their order.<br/>
     * In other terms, this function can be used to impose the same order on two
     * sets by using one of them as a master and the other one as a slave.
     *
     * Iterating the sparse set with a couple of iterators returns elements in
     * the expected order after a call to `respect`. See `begin` and `end` for
     * more details.
     *
     * @param other The sparse sets that imposes the order of the entities.
     */
    void respect(const basic_sparse_set &other) {
        compact();

        const auto to = other.end();
        auto from = other.begin();

        for(size_type pos = count - 1; pos && from != to; ++from) {
            if(contains(*from)) {
                if(*from != packed[pos]) {
                    // basic no-leak guarantee (with invalid state) if swapping throws
                    swap(packed[pos], *from);
                }

                --pos;
            }
        }
    }

    /**
     * @brief Clears a sparse set.
     * @param ud Optional user data that are forwarded as-is to derived classes.
     */
    void clear(void *ud = nullptr) {
        for(auto &&entity: *this) {
            if(entity != tombstone) {
                stable_erase(entity, ud);
            }
        }

        free_list = tombstone;
        count = 0u;
    }

private:
    typename alloc_traits::allocator_type allocator;
    typename bucket_alloc_traits::allocator_type bucket_allocator;
    bucket_alloc_pointer sparse;
    alloc_pointer packed;
    std::size_t bucket;
    std::size_t count;
    std::size_t reserved;
    entity_type free_list;
    deletion_policy mode;
};


}


#endif

// #include "storage.hpp"
#ifndef ENTT_ENTITY_STORAGE_HPP
#define ENTT_ENTITY_STORAGE_HPP


#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/algorithm.hpp"

// #include "../core/fwd.hpp"

// #include "../core/type_traits.hpp"

// #include "../signal/sigh.hpp"
#ifndef ENTT_SIGNAL_SIGH_HPP
#define ENTT_SIGNAL_SIGH_HPP


#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>
#include <type_traits>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "delegate.hpp"
#ifndef ENTT_SIGNAL_DELEGATE_HPP
#define ENTT_SIGNAL_DELEGATE_HPP


#include <tuple>
#include <cstddef>
#include <utility>
#include <functional>
#include <type_traits>
// #include "../core/type_traits.hpp"
#ifndef ENTT_CORE_TYPE_TRAITS_HPP
#define ENTT_CORE_TYPE_TRAITS_HPP


#include <cstddef>
#include <utility>
#include <type_traits>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "fwd.hpp"
#ifndef ENTT_CORE_FWD_HPP
#define ENTT_CORE_FWD_HPP


#include <type_traits>
// #include "../config/config.h"



namespace entt {


template<std::size_t Len = sizeof(double[2]), std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_any;


/*! @brief Alias declaration for type identifiers. */
using id_type = ENTT_ID_TYPE;


/*! @brief Alias declaration for the most common use case. */
using any = basic_any<>;


}


#endif



namespace entt {


/**
 * @brief Utility class to disambiguate overloaded functions.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
struct choice_t
    // Unfortunately, doxygen cannot parse such a construct.
    /*! @cond TURN_OFF_DOXYGEN */
    : choice_t<N-1>
    /*! @endcond */
{};


/*! @copybrief choice_t */
template<>
struct choice_t<0> {};


/**
 * @brief Variable template for the choice trick.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
inline constexpr choice_t<N> choice{};


/**
 * @brief Identity type trait.
 *
 * Useful to establish non-deduced contexts in template argument deduction
 * (waiting for C++20) or to provide types through function arguments.
 *
 * @tparam Type A type.
 */
template<typename Type>
struct type_identity {
    /*! @brief Identity type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Type A type.
 */
template<typename Type>
using type_identity_t = typename type_identity<Type>::type;


/**
 * @brief A type-only `sizeof` wrapper that returns 0 where `sizeof` complains.
 * @tparam Type The type of which to return the size.
 * @tparam The size of the type if `sizeof` accepts it, 0 otherwise.
 */
template<typename Type, typename = void>
struct size_of: std::integral_constant<std::size_t, 0u> {};


/*! @copydoc size_of */
template<typename Type>
struct size_of<Type, std::void_t<decltype(sizeof(Type))>>
    : std::integral_constant<std::size_t, sizeof(Type)>
{};


/**
 * @brief Helper variable template.
 * @tparam Type The type of which to return the size.
 */
template<class Type>
inline constexpr auto size_of_v = size_of<Type>::value;


/**
 * @brief Using declaration to be used to _repeat_ the same type a number of
 * times equal to the size of a given parameter pack.
 * @tparam Type A type to repeat.
 */
template<typename Type, typename>
using unpack_as_t = Type;


/**
 * @brief Helper variable template to be used to _repeat_ the same value a
 * number of times equal to the size of a given parameter pack.
 * @tparam Value A value to repeat.
 */
template<auto Value, typename>
inline constexpr auto unpack_as_v = Value;


/**
 * @brief Wraps a static constant.
 * @tparam Value A static constant.
 */
template<auto Value>
using integral_constant = std::integral_constant<decltype(Value), Value>;


/**
 * @brief Alias template to facilitate the creation of named values.
 * @tparam Value A constant value at least convertible to `id_type`.
 */
template<id_type Value>
using tag = integral_constant<Value>;


/**
 * @brief A class to use to push around lists of types, nothing more.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list {
    /*! @brief Type list type. */
    using type = type_list;
    /*! @brief Compile-time number of elements in the type list. */
    static constexpr auto size = sizeof...(Type);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct type_list_element;


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Index Index of the type to return.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<std::size_t Index, typename Type, typename... Other>
struct type_list_element<Index, type_list<Type, Other...>>
    : type_list_element<Index - 1u, type_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<typename Type, typename... Other>
struct type_list_element<0u, type_list<Type, Other...>> {
    /*! @brief Searched type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the type to return.
 * @tparam List Type list to search into.
 */
template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @return A type list composed by the types of both the type lists.
 */
template<typename... Type, typename... Other>
constexpr type_list<Type..., Other...> operator+(type_list<Type...>, type_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_cat;


/*! @brief Concatenates multiple type lists. */
template<>
struct type_list_cat<> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<>;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @tparam List Other type lists, if any.
 */
template<typename... Type, typename... Other, typename... List>
struct type_list_cat<type_list<Type...>, type_list<Other...>, List...> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = typename type_list_cat<type_list<Type..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list_cat<type_list<Type...>> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<Type...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists to concatenate.
 */
template<typename... List>
using type_list_cat_t = typename type_list_cat<List...>::type;


/*! @brief Primary template isn't defined on purpose. */
template<typename>
struct type_list_unique;


/**
 * @brief Removes duplicates types from a type list.
 * @tparam Type One of the types provided by the given type list.
 * @tparam Other The other types provided by the given type list.
 */
template<typename Type, typename... Other>
struct type_list_unique<type_list<Type, Other...>> {
    /*! @brief A type list without duplicate types. */
    using type = std::conditional_t<
        std::disjunction_v<std::is_same<Type, Other>...>,
        typename type_list_unique<type_list<Other...>>::type,
        type_list_cat_t<type_list<Type>, typename type_list_unique<type_list<Other...>>::type>
    >;
};


/*! @brief Removes duplicates types from a type list. */
template<>
struct type_list_unique<type_list<>> {
    /*! @brief A type list without duplicate types. */
    using type = type_list<>;
};


/**
 * @brief Helper type.
 * @tparam Type A type list.
 */
template<typename Type>
using type_list_unique_t = typename type_list_unique<Type>::type;


/**
 * @brief Provides the member constant `value` to true if a type list contains a
 * given type, false otherwise.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<typename List, typename Type>
struct type_list_contains;


/**
 * @copybrief type_list_contains
 * @tparam Type Types provided by the type list.
 * @tparam Other Type to look for.
 */
template<typename... Type, typename Other>
struct type_list_contains<type_list<Type...>, Other>: std::disjunction<std::is_same<Type, Other>...> {};


/**
 * @brief Helper variable template.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<class List, typename Type>
inline constexpr auto type_list_contains_v = type_list_contains<List, Type>::value;


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_diff;


/**
 * @brief Computes the difference between two type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 */
template<typename... Type, typename... Other>
struct type_list_diff<type_list<Type...>, type_list<Other...>> {
    /*! @brief A type list that is the difference between the two type lists. */
    using type = type_list_cat_t<std::conditional_t<type_list_contains_v<type_list<Other...>, Type>, type_list<>, type_list<Type>>...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists between which to compute the difference.
 */
template<typename... List>
using type_list_diff_t = typename type_list_diff<List...>::type;


/**
 * @brief A class to use to push around lists of constant values, nothing more.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list {
    /*! @brief Value list type. */
    using type = value_list;
    /*! @brief Compile-time number of elements in the value list. */
    static constexpr auto size = sizeof...(Value);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct value_list_element;


/**
 * @brief Provides compile-time indexed access to the values of a value list.
 * @tparam Index Index of the value to return.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<std::size_t Index, auto Value, auto... Other>
struct value_list_element<Index, value_list<Value, Other...>>
    : value_list_element<Index - 1u, value_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<auto Value, auto... Other>
struct value_list_element<0u, value_list<Value, Other...>> {
    /*! @brief Searched value. */
    static constexpr auto value = Value;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the value to return.
 * @tparam List Value list to search into.
 */
template<std::size_t Index, typename List>
inline constexpr auto value_list_element_v = value_list_element<Index, List>::value;


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @return A value list composed by the values of both the value lists.
 */
template<auto... Value, auto... Other>
constexpr value_list<Value..., Other...> operator+(value_list<Value...>, value_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct value_list_cat;


/*! @brief Concatenates multiple value lists. */
template<>
struct value_list_cat<> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<>;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @tparam List Other value lists, if any.
 */
template<auto... Value, auto... Other, typename... List>
struct value_list_cat<value_list<Value...>, value_list<Other...>, List...> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = typename value_list_cat<value_list<Value..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list_cat<value_list<Value...>> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<Value...>;
};


/**
 * @brief Helper type.
 * @tparam List Value lists to concatenate.
 */
template<typename... List>
using value_list_cat_t = typename value_list_cat<List...>::type;


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
[[nodiscard]] constexpr bool is_equality_comparable(...) { return false; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<0>)
-> decltype(std::declval<Type>() == std::declval<Type>()) { return true; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<1>)
-> decltype(std::declval<typename Type::value_type>(), std::declval<Type>() == std::declval<Type>()) {
    if constexpr(std::is_same_v<typename Type::value_type, Type>) {
        return is_equality_comparable<Type>(choice<0>);
    } else {
        return is_equality_comparable<typename Type::value_type>(choice<2>);
    }
}


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<2>)
-> decltype(std::declval<typename Type::mapped_type>(), std::declval<Type>() == std::declval<Type>()) {
    return is_equality_comparable<typename Type::key_type>(choice<2>) && is_equality_comparable<typename Type::mapped_type>(choice<2>);
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Provides the member constant `value` to true if a given type is
 * equality comparable, false otherwise.
 * @tparam Type Potentially equality comparable type.
 */
template<typename Type, typename = void>
struct is_equality_comparable: std::bool_constant<internal::is_equality_comparable<Type>(choice<2>)> {};


/**
 * @brief Helper variable template.
 * @tparam Type Potentially equality comparable type.
 */
template<class Type>
inline constexpr auto is_equality_comparable_v = is_equality_comparable<Type>::value;


/*! @brief Same as std::is_invocable, but with tuples. */
template<typename, typename>
struct is_applicable: std::false_type {};


/**
 * @copybrief is_applicable
 * @tparam Func A valid function type.
 * @tparam Tuple Tuple-like type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
* @copybrief is_applicable
* @tparam Func A valid function type.
* @tparam Tuple Tuple-like type.
* @tparam Args The list of arguments to use to probe the function type.
*/
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, const Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, typename Args>
inline constexpr auto is_applicable_v = is_applicable<Func, Args>::value;


/*! @brief Same as std::is_invocable_r, but with tuples for arguments. */
template<typename, typename, typename>
struct is_applicable_r: std::false_type {};


/**
 * @copybrief is_applicable_r
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename... Args>
struct is_applicable_r<Ret, Func, std::tuple<Args...>>: std::is_invocable_r<Ret, Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename Args>
inline constexpr auto is_applicable_r_v = is_applicable_r<Ret, Func, Args>::value;


/**
* @brief Provides the member constant `value` to true if a given type is
* complete, false otherwise.
* @tparam Type Potential complete type.
*/
template<typename Type, typename = void>
struct is_complete: std::false_type {};


/*! @copydoc is_complete */
template<typename Type>
struct is_complete<Type, std::void_t<decltype(sizeof(Type))>>: std::true_type {};


/**
* @brief Helper variable template.
* @tparam Type Potential complete type.
*/
template<typename Type>
inline constexpr auto is_complete_v = is_complete<Type>::value;


/**
 * @brief Transcribes the constness of a type to another type.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
struct constness_as {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::remove_const_t<To>;
};


/*! @copydoc constness_as */
template<typename To, typename From>
struct constness_as<To, const From> {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::add_const_t<To>;
};


/**
 * @brief Alias template to facilitate the transcription of the constness.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;


/**
 * @brief Extracts the class of a non-static member object or function.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
class member_class {
    static_assert(std::is_member_pointer_v<Member>, "Invalid pointer type to non-static member object or function");

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...));

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...) const);

    template<typename Class, typename Type>
    static Class * clazz(Type Class:: *);

public:
    /*! @brief The class of the given non-static member object or function. */
    using type = std::remove_pointer_t<decltype(clazz(std::declval<Member>()))>;
};


/**
 * @brief Helper type.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
using member_class_t = typename member_class<Member>::type;


}


#endif

// #include "../config/config.h"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename Ret, typename... Args>
auto function_pointer(Ret(*)(Args...)) -> Ret(*)(Args...);


template<typename Ret, typename Type, typename... Args, typename Other>
auto function_pointer(Ret(*)(Type, Args...), Other &&) -> Ret(*)(Args...);


template<typename Class, typename Ret, typename... Args, typename... Other>
auto function_pointer(Ret(Class:: *)(Args...), Other &&...) -> Ret(*)(Args...);


template<typename Class, typename Ret, typename... Args, typename... Other>
auto function_pointer(Ret(Class:: *)(Args...) const, Other &&...) -> Ret(*)(Args...);


template<typename Class, typename Type, typename... Other>
auto function_pointer(Type Class:: *, Other &&...) -> Type(*)();


template<typename... Type>
using function_pointer_t = decltype(internal::function_pointer(std::declval<Type>()...));


template<typename... Class, typename Ret, typename... Args>
[[nodiscard]] constexpr auto index_sequence_for(Ret(*)(Args...)) {
    return std::index_sequence_for<Class..., Args...>{};
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/*! @brief Used to wrap a function or a member of a specified type. */
template<auto>
struct connect_arg_t {};


/*! @brief Constant of type connect_arg_t used to disambiguate calls. */
template<auto Func>
inline constexpr connect_arg_t<Func> connect_arg{};


/**
 * @brief Basic delegate implementation.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error unless the template parameter is a function type.
 */
template<typename>
class delegate;


/**
 * @brief Utility class to use to send around functions and members.
 *
 * Unmanaged delegate for function pointers and members. Users of this class are
 * in charge of disconnecting instances before deleting them.
 *
 * A delegate can be used as a general purpose invoker without memory overhead
 * for free functions possibly with payloads and bound or unbound members.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
class delegate<Ret(Args...)> {
    template<auto Candidate, std::size_t... Index>
    [[nodiscard]] auto wrap(std::index_sequence<Index...>) ENTT_NOEXCEPT {
        return [](const void *, Args... args) -> Ret {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);
            return static_cast<Ret>(std::invoke(Candidate, std::forward<type_list_element_t<Index, type_list<Args...>>>(std::get<Index>(arguments))...));
        };
    }

    template<auto Candidate, typename Type, std::size_t... Index>
    [[nodiscard]] auto wrap(Type &, std::index_sequence<Index...>) ENTT_NOEXCEPT {
        return [](const void *payload, Args... args) -> Ret {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);
            Type *curr = static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(payload));
            return static_cast<Ret>(std::invoke(Candidate, *curr, std::forward<type_list_element_t<Index, type_list<Args...>>>(std::get<Index>(arguments))...));
        };
    }

    template<auto Candidate, typename Type, std::size_t... Index>
    [[nodiscard]] auto wrap(Type *, std::index_sequence<Index...>) ENTT_NOEXCEPT {
        return [](const void *payload, Args... args) -> Ret {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);
            Type *curr = static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(payload));
            return static_cast<Ret>(std::invoke(Candidate, curr, std::forward<type_list_element_t<Index, type_list<Args...>>>(std::get<Index>(arguments))...));
        };
    }

public:
    /*! @brief Function type of the contained target. */
    using function_type = Ret(const void *, Args...);
    /*! @brief Function type of the delegate. */
    using type = Ret(Args...);
    /*! @brief Return type of the delegate. */
    using result_type = Ret;

    /*! @brief Default constructor. */
    delegate() ENTT_NOEXCEPT
        : fn{nullptr}, data{nullptr}
    {}

    /**
     * @brief Constructs a delegate and connects a free function or an unbound
     * member.
     * @tparam Candidate Function or member to connect to the delegate.
     */
    template<auto Candidate>
    delegate(connect_arg_t<Candidate>) ENTT_NOEXCEPT {
        connect<Candidate>();
    }

    /**
     * @brief Constructs a delegate and connects a free function with payload or
     * a bound member.
     * @tparam Candidate Function or member to connect to the delegate.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<auto Candidate, typename Type>
    delegate(connect_arg_t<Candidate>, Type &&value_or_instance) ENTT_NOEXCEPT {
        connect<Candidate>(std::forward<Type>(value_or_instance));
    }

    /**
     * @brief Constructs a delegate and connects an user defined function with
     * optional payload.
     * @param function Function to connect to the delegate.
     * @param payload User defined arbitrary data.
     */
    delegate(function_type *function, const void *payload = nullptr) ENTT_NOEXCEPT {
        connect(function, payload);
    }

    /**
     * @brief Connects a free function or an unbound member to a delegate.
     * @tparam Candidate Function or member to connect to the delegate.
     */
    template<auto Candidate>
    void connect() ENTT_NOEXCEPT {
        data = nullptr;

        if constexpr(std::is_invocable_r_v<Ret, decltype(Candidate), Args...>) {
            fn = [](const void *, Args... args) -> Ret {
                return Ret(std::invoke(Candidate, std::forward<Args>(args)...));
            };
        } else if constexpr(std::is_member_pointer_v<decltype(Candidate)>) {
            fn = wrap<Candidate>(internal::index_sequence_for<type_list_element_t<0, type_list<Args...>>>(internal::function_pointer_t<decltype(Candidate)>{}));
        } else {
            fn = wrap<Candidate>(internal::index_sequence_for(internal::function_pointer_t<decltype(Candidate)>{}));
        }
    }

    /**
     * @brief Connects a free function with payload or a bound member to a
     * delegate.
     *
     * The delegate isn't responsible for the connected object or the payload.
     * Users must always guarantee that the lifetime of the instance overcomes
     * the one of the delegate.<br/>
     * When used to connect a free function with payload, its signature must be
     * such that the instance is the first argument before the ones used to
     * define the delegate itself.
     *
     * @tparam Candidate Function or member to connect to the delegate.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid reference that fits the purpose.
     */
    template<auto Candidate, typename Type>
    void connect(Type &value_or_instance) ENTT_NOEXCEPT {
        data = &value_or_instance;

        if constexpr(std::is_invocable_r_v<Ret, decltype(Candidate), Type &, Args...>) {
            fn = [](const void *payload, Args... args) -> Ret {
                Type *curr = static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(payload));
                return Ret(std::invoke(Candidate, *curr, std::forward<Args>(args)...));
            };
        } else {
            fn = wrap<Candidate>(value_or_instance, internal::index_sequence_for(internal::function_pointer_t<decltype(Candidate), Type>{}));
        }
    }

    /**
     * @brief Connects a free function with payload or a bound member to a
     * delegate.
     *
     * @sa connect(Type &)
     *
     * @tparam Candidate Function or member to connect to the delegate.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid pointer that fits the purpose.
     */
    template<auto Candidate, typename Type>
    void connect(Type *value_or_instance) ENTT_NOEXCEPT {
        data = value_or_instance;

        if constexpr(std::is_invocable_r_v<Ret, decltype(Candidate), Type *, Args...>) {
            fn = [](const void *payload, Args... args) -> Ret {
                Type *curr = static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(payload));
                return Ret(std::invoke(Candidate, curr, std::forward<Args>(args)...));
            };
        } else {
            fn = wrap<Candidate>(value_or_instance, internal::index_sequence_for(internal::function_pointer_t<decltype(Candidate), Type>{}));
        }
    }

    /**
     * @brief Connects an user defined function with optional payload to a
     * delegate.
     *
     * The delegate isn't responsible for the connected object or the payload.
     * Users must always guarantee that the lifetime of an instance overcomes
     * the one of the delegate.<br/>
     * The payload is returned as the first argument to the target function in
     * all cases.
     *
     * @param function Function to connect to the delegate.
     * @param payload User defined arbitrary data.
     */
    void connect(function_type *function, const void *payload = nullptr) ENTT_NOEXCEPT {
        fn = function;
        data = payload;
    }

    /**
     * @brief Resets a delegate.
     *
     * After a reset, a delegate cannot be invoked anymore.
     */
    void reset() ENTT_NOEXCEPT {
        fn = nullptr;
        data = nullptr;
    }

    /**
     * @brief Returns the instance or the payload linked to a delegate, if any.
     * @return An opaque pointer to the underlying data.
     */
    [[nodiscard]] const void * instance() const ENTT_NOEXCEPT {
        return data;
    }

    /**
     * @brief Triggers a delegate.
     *
     * The delegate invokes the underlying function and returns the result.
     *
     * @warning
     * Attempting to trigger an invalid delegate results in undefined
     * behavior.
     *
     * @param args Arguments to use to invoke the underlying function.
     * @return The value returned by the underlying function.
     */
    Ret operator()(Args... args) const {
        ENTT_ASSERT(static_cast<bool>(*this), "Uninitialized delegate");
        return fn(data, std::forward<Args>(args)...);
    }

    /**
     * @brief Checks whether a delegate actually stores a listener.
     * @return False if the delegate is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        // no need to test also data
        return !(fn == nullptr);
    }

    /**
     * @brief Compares the contents of two delegates.
     * @param other Delegate with which to compare.
     * @return False if the two contents differ, true otherwise.
     */
    [[nodiscard]] bool operator==(const delegate<Ret(Args...)> &other) const ENTT_NOEXCEPT {
        return fn == other.fn && data == other.data;
    }

private:
    function_type *fn;
    const void *data;
};


/**
 * @brief Compares the contents of two delegates.
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 * @param lhs A valid delegate object.
 * @param rhs A valid delegate object.
 * @return True if the two contents differ, false otherwise.
 */
template<typename Ret, typename... Args>
[[nodiscard]] bool operator!=(const delegate<Ret(Args...)> &lhs, const delegate<Ret(Args...)> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Deduction guide.
 * @tparam Candidate Function or member to connect to the delegate.
 */
template<auto Candidate>
delegate(connect_arg_t<Candidate>)
-> delegate<std::remove_pointer_t<internal::function_pointer_t<decltype(Candidate)>>>;


/**
 * @brief Deduction guide.
 * @tparam Candidate Function or member to connect to the delegate.
 * @tparam Type Type of class or type of payload.
 */
template<auto Candidate, typename Type>
delegate(connect_arg_t<Candidate>, Type &&)
-> delegate<std::remove_pointer_t<internal::function_pointer_t<decltype(Candidate), Type>>>;


/**
 * @brief Deduction guide.
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
delegate(Ret(*)(const void *, Args...), const void * = nullptr)
-> delegate<Ret(Args...)>;


}


#endif

// #include "fwd.hpp"
#ifndef ENTT_SIGNAL_FWD_HPP
#define ENTT_SIGNAL_FWD_HPP


namespace entt {


template<typename>
class delegate;


class dispatcher;


template<typename>
class emitter;


class connection;


struct scoped_connection;


template<typename>
class sink;


template<typename>
class sigh;


}


#endif



namespace entt {


/**
 * @brief Sink class.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error unless the template parameter is a function type.
 *
 * @tparam Function A valid function type.
 */
template<typename Function>
class sink;


/**
 * @brief Unmanaged signal handler.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error unless the template parameter is a function type.
 *
 * @tparam Function A valid function type.
 */
template<typename Function>
class sigh;


/**
 * @brief Unmanaged signal handler.
 *
 * It works directly with references to classes and pointers to member functions
 * as well as pointers to free functions. Users of this class are in charge of
 * disconnecting instances before deleting them.
 *
 * This class serves mainly two purposes:
 *
 * * Creating signals to use later to notify a bunch of listeners.
 * * Collecting results from a set of functions like in a voting system.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
class sigh<Ret(Args...)> {
    /*! @brief A sink is allowed to modify a signal. */
    friend class sink<Ret(Args...)>;

public:
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Sink type. */
    using sink_type = sink<Ret(Args...)>;

    /**
     * @brief Instance type when it comes to connecting member functions.
     * @tparam Class Type of class to which the member function belongs.
     */
    template<typename Class>
    using instance_type = Class *;

    /**
     * @brief Number of listeners connected to the signal.
     * @return Number of listeners currently connected.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return calls.size();
    }

    /**
     * @brief Returns false if at least a listener is connected to the signal.
     * @return True if the signal has no listeners connected, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return calls.empty();
    }

    /**
     * @brief Triggers a signal.
     *
     * All the listeners are notified. Order isn't guaranteed.
     *
     * @param args Arguments to use to invoke listeners.
     */
    void publish(Args... args) const {
        for(auto &&call: std::as_const(calls)) {
            call(args...);
        }
    }

    /**
     * @brief Collects return values from the listeners.
     *
     * The collector must expose a call operator with the following properties:
     *
     * * The return type is either `void` or such that it's convertible to
     *   `bool`. In the second case, a true value will stop the iteration.
     * * The list of parameters is empty if `Ret` is `void`, otherwise it
     *   contains a single element such that `Ret` is convertible to it.
     *
     * @tparam Func Type of collector to use, if any.
     * @param func A valid function object.
     * @param args Arguments to use to invoke listeners.
     */
    template<typename Func>
    void collect(Func func, Args... args) const {
        for(auto &&call: calls) {
            if constexpr(std::is_void_v<Ret>) {
                if constexpr(std::is_invocable_r_v<bool, Func>) {
                    call(args...);
                    if(func()) { break; }
                } else {
                    call(args...);
                    func();
                }
            } else {
                if constexpr(std::is_invocable_r_v<bool, Func, Ret>) {
                    if(func(call(args...))) { break; }
                } else {
                    func(call(args...));
                }
            }
        }
    }

private:
    std::vector<delegate<Ret(Args...)>> calls;
};


/**
 * @brief Connection class.
 *
 * Opaque object the aim of which is to allow users to release an already
 * estabilished connection without having to keep a reference to the signal or
 * the sink that generated it.
 */
class connection {
    /*! @brief A sink is allowed to create connection objects. */
    template<typename>
    friend class sink;

    connection(delegate<void(void *)> fn, void *ref)
        : disconnect{fn}, signal{ref}
    {}

public:
    /*! @brief Default constructor. */
    connection() = default;

    /**
     * @brief Checks whether a connection is properly initialized.
     * @return True if the connection is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return static_cast<bool>(disconnect);
    }

    /*! @brief Breaks the connection. */
    void release() {
        if(disconnect) {
            disconnect(signal);
            disconnect.reset();
        }
    }

private:
    delegate<void(void *)> disconnect;
    void *signal{};
};


/**
 * @brief Scoped connection class.
 *
 * Opaque object the aim of which is to allow users to release an already
 * estabilished connection without having to keep a reference to the signal or
 * the sink that generated it.<br/>
 * A scoped connection automatically breaks the link between the two objects
 * when it goes out of scope.
 */
struct scoped_connection {
    /*! @brief Default constructor. */
    scoped_connection() = default;

    /**
     * @brief Constructs a scoped connection from a basic connection.
     * @param other A valid connection object.
     */
    scoped_connection(const connection &other)
        : conn{other}
    {}

    /*! @brief Default copy constructor, deleted on purpose. */
    scoped_connection(const scoped_connection &) = delete;

    /*! @brief Automatically breaks the link on destruction. */
    ~scoped_connection() {
        conn.release();
    }

    /**
     * @brief Default copy assignment operator, deleted on purpose.
     * @return This scoped connection.
     */
    scoped_connection & operator=(const scoped_connection &) = delete;

    /**
     * @brief Acquires a connection.
     * @param other The connection object to acquire.
     * @return This scoped connection.
     */
    scoped_connection & operator=(connection other) {
        conn = std::move(other);
        return *this;
    }

    /**
     * @brief Checks whether a scoped connection is properly initialized.
     * @return True if the connection is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return static_cast<bool>(conn);
    }

    /*! @brief Breaks the connection. */
    void release() {
        conn.release();
    }

private:
    connection conn;
};


/**
 * @brief Sink class.
 *
 * A sink is used to connect listeners to signals and to disconnect them.<br/>
 * The function type for a listener is the one of the signal to which it
 * belongs.
 *
 * The clear separation between a signal and a sink permits to store the former
 * as private data member without exposing the publish functionality to the
 * users of the class.
 *
 * @warning
 * Lifetime of a sink must not overcome that of the signal to which it refers.
 * In any other case, attempting to use a sink results in undefined behavior.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
class sink<Ret(Args...)> {
    using signal_type = sigh<Ret(Args...)>;
    using difference_type = typename std::iterator_traits<typename decltype(signal_type::calls)::iterator>::difference_type;

    template<auto Candidate, typename Type>
    static void release(Type value_or_instance, void *signal) {
        sink{*static_cast<signal_type *>(signal)}.disconnect<Candidate>(value_or_instance);
    }

    template<auto Candidate>
    static void release(void *signal) {
        sink{*static_cast<signal_type *>(signal)}.disconnect<Candidate>();
    }

public:
    /**
     * @brief Constructs a sink that is allowed to modify a given signal.
     * @param ref A valid reference to a signal object.
     */
    sink(sigh<Ret(Args...)> &ref) ENTT_NOEXCEPT
        : offset{},
          signal{&ref}
    {}

    /**
     * @brief Returns false if at least a listener is connected to the sink.
     * @return True if the sink has no listeners connected, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return signal->calls.empty();
    }

    /**
     * @brief Returns a sink that connects before a given free function or an
     * unbound member.
     * @tparam Function A valid free function pointer.
     * @return A properly initialized sink object.
     */
    template<auto Function>
    [[nodiscard]] sink before() {
        delegate<Ret(Args...)> call{};
        call.template connect<Function>();

        const auto &calls = signal->calls;
        const auto it = std::find(calls.cbegin(), calls.cend(), std::move(call));

        sink other{*this};
        other.offset = std::distance(it, calls.cend());
        return other;
    }

    /**
     * @brief Returns a sink that connects before a free function with payload
     * or a bound member.
     * @tparam Candidate Member or free function to look for.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     * @return A properly initialized sink object.
     */
    template<auto Candidate, typename Type>
    [[nodiscard]] sink before(Type &&value_or_instance) {
        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>(value_or_instance);

        const auto &calls = signal->calls;
        const auto it = std::find(calls.cbegin(), calls.cend(), std::move(call));

        sink other{*this};
        other.offset = std::distance(it, calls.cend());
        return other;
    }

    /**
     * @brief Returns a sink that connects before a given instance or specific
     * payload.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     * @return A properly initialized sink object.
     */
    template<typename Type>
    [[nodiscard]] sink before(Type &value_or_instance) {
        return before(&value_or_instance);
    }

    /**
     * @brief Returns a sink that connects before a given instance or specific
     * payload.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid pointer that fits the purpose.
     * @return A properly initialized sink object.
     */
    template<typename Type>
    [[nodiscard]] sink before(Type *value_or_instance) {
        sink other{*this};

        if(value_or_instance) {
            const auto &calls = signal->calls;
            const auto it = std::find_if(calls.cbegin(), calls.cend(), [value_or_instance](const auto &delegate) {
                return delegate.instance() == value_or_instance;
            });

            other.offset = std::distance(it, calls.cend());
        }

        return other;
    }

    /**
     * @brief Returns a sink that connects before anything else.
     * @return A properly initialized sink object.
     */
    [[nodiscard]] sink before() {
        sink other{*this};
        other.offset = signal->calls.size();
        return other;
    }

    /**
     * @brief Connects a free function or an unbound member to a signal.
     *
     * The signal handler performs checks to avoid multiple connections for the
     * same function.
     *
     * @tparam Candidate Function or member to connect to the signal.
     * @return A properly initialized connection object.
     */
    template<auto Candidate>
    connection connect() {
        disconnect<Candidate>();

        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>();
        signal->calls.insert(signal->calls.end() - offset, std::move(call));

        delegate<void(void *)> conn{};
        conn.template connect<&release<Candidate>>();
        return { std::move(conn), signal };
    }

    /**
     * @brief Connects a free function with payload or a bound member to a
     * signal.
     *
     * The signal isn't responsible for the connected object or the payload.
     * Users must always guarantee that the lifetime of the instance overcomes
     * the one of the signal. On the other side, the signal handler performs
     * checks to avoid multiple connections for the same function.<br/>
     * When used to connect a free function with payload, its signature must be
     * such that the instance is the first argument before the ones used to
     * define the signal itself.
     *
     * @tparam Candidate Function or member to connect to the signal.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     * @return A properly initialized connection object.
     */
    template<auto Candidate, typename Type>
    connection connect(Type &&value_or_instance) {
        disconnect<Candidate>(value_or_instance);

        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>(value_or_instance);
        signal->calls.insert(signal->calls.end() - offset, std::move(call));

        delegate<void(void *)> conn{};
        conn.template connect<&release<Candidate, Type>>(value_or_instance);
        return { std::move(conn), signal };
    }

    /**
     * @brief Disconnects a free function or an unbound member from a signal.
     * @tparam Candidate Function or member to disconnect from the signal.
     */
    template<auto Candidate>
    void disconnect() {
        auto &calls = signal->calls;
        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>();
        calls.erase(std::remove(calls.begin(), calls.end(), std::move(call)), calls.end());
    }

    /**
     * @brief Disconnects a free function with payload or a bound member from a
     * signal.
     * @tparam Candidate Function or member to disconnect from the signal.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<auto Candidate, typename Type>
    void disconnect(Type &&value_or_instance) {
        auto &calls = signal->calls;
        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>(value_or_instance);
        calls.erase(std::remove(calls.begin(), calls.end(), std::move(call)), calls.end());
    }

    /**
     * @brief Disconnects free functions with payload or bound members from a
     * signal.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<typename Type>
    void disconnect(Type &value_or_instance) {
        disconnect(&value_or_instance);
    }

    /**
     * @brief Disconnects free functions with payload or bound members from a
     * signal.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<typename Type>
    void disconnect(Type *value_or_instance) {
        if(value_or_instance) {
            auto &calls = signal->calls;
            calls.erase(std::remove_if(calls.begin(), calls.end(), [value_or_instance](const auto &delegate) {
                return delegate.instance() == value_or_instance;
            }), calls.end());
        }
    }

    /*! @brief Disconnects all the listeners from a signal. */
    void disconnect() {
        signal->calls.clear();
    }

private:
    difference_type offset;
    signal_type *signal;
};


/**
 * @brief Deduction guide.
 *
 * It allows to deduce the function type of a sink directly from the signal it
 * refers to.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
sink(sigh<Ret(Args...)> &)
-> sink<Ret(Args...)>;


}


#endif

// #include "component.hpp"

// #include "entity.hpp"

// #include "fwd.hpp"

// #include "sparse_set.hpp"



namespace entt {


/**
 * @brief Basic storage implementation.
 *
 * This class is a refinement of a sparse set that associates an object to an
 * entity. The main purpose of this class is to extend sparse sets to store
 * components in a registry. It guarantees fast access both to the elements and
 * to the entities.
 *
 * @note
 * Entities and objects have the same order.
 *
 * @note
 * Internal data structures arrange elements to maximize performance. There are
 * no guarantees that objects are returned in the insertion order when iterate
 * a storage. Do not make assumption on the order in any case.
 *
 * @warning
 * Empty types aren't explicitly instantiated. Therefore, many of the functions
 * normally available for non-empty types will not be available for empty ones.
 *
 * @sa sparse_set<Entity>
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Type Type of objects assigned to the entities.
 * @tparam Allocator Type of allocator used to manage memory and elements.
 */
template<typename Entity, typename Type, typename Allocator, typename = void>
class basic_storage_impl: public basic_sparse_set<Entity, typename std::allocator_traits<Allocator>::template rebind_alloc<Entity>> {
    static constexpr auto packed_page = ENTT_PACKED_PAGE;

    using comp_traits = component_traits<Type>;

    using underlying_type = basic_sparse_set<Entity, typename std::allocator_traits<Allocator>::template rebind_alloc<Entity>>;
    using difference_type = typename entt_traits<Entity>::difference_type;

    using alloc_traits = typename std::allocator_traits<Allocator>::template rebind_traits<Type>;
    using alloc_pointer = typename alloc_traits::pointer;
    using alloc_const_pointer = typename alloc_traits::const_pointer;

    using bucket_alloc_traits = typename std::allocator_traits<Allocator>::template rebind_traits<alloc_pointer>;
    using bucket_alloc_pointer = typename bucket_alloc_traits::pointer;

    using bucket_alloc_const_type = typename std::allocator_traits<Allocator>::template rebind_alloc<alloc_const_pointer>;
    using bucket_alloc_const_pointer = typename std::allocator_traits<bucket_alloc_const_type>::const_pointer;

    static_assert(alloc_traits::propagate_on_container_move_assignment::value);
    static_assert(bucket_alloc_traits::propagate_on_container_move_assignment::value);

    template<typename Value>
    struct storage_iterator final {
        using difference_type = typename basic_storage_impl::difference_type;
        using value_type = Value;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::random_access_iterator_tag;

        storage_iterator() ENTT_NOEXCEPT = default;

        storage_iterator(bucket_alloc_pointer const *ref, const typename basic_storage_impl::difference_type idx) ENTT_NOEXCEPT
            : packed{ref},
              index{idx}
        {}

        storage_iterator & operator++() ENTT_NOEXCEPT {
            return --index, *this;
        }

        storage_iterator operator++(int) ENTT_NOEXCEPT {
            storage_iterator orig = *this;
            return ++(*this), orig;
        }

        storage_iterator & operator--() ENTT_NOEXCEPT {
            return ++index, *this;
        }

        storage_iterator operator--(int) ENTT_NOEXCEPT {
            storage_iterator orig = *this;
            return operator--(), orig;
        }

        storage_iterator & operator+=(const difference_type value) ENTT_NOEXCEPT {
            index -= value;
            return *this;
        }

        storage_iterator operator+(const difference_type value) const ENTT_NOEXCEPT {
            storage_iterator copy = *this;
            return (copy += value);
        }

        storage_iterator & operator-=(const difference_type value) ENTT_NOEXCEPT {
            return (*this += -value);
        }

        storage_iterator operator-(const difference_type value) const ENTT_NOEXCEPT {
            return (*this + -value);
        }

        difference_type operator-(const storage_iterator &other) const ENTT_NOEXCEPT {
            return other.index - index;
        }

        [[nodiscard]] reference operator[](const difference_type value) const ENTT_NOEXCEPT {
            const auto pos = size_type(index-value-1);
            return (*packed)[page(pos)][offset(pos)];
        }

        [[nodiscard]] bool operator==(const storage_iterator &other) const ENTT_NOEXCEPT {
            return other.index == index;
        }

        [[nodiscard]] bool operator!=(const storage_iterator &other) const ENTT_NOEXCEPT {
            return !(*this == other);
        }

        [[nodiscard]] bool operator<(const storage_iterator &other) const ENTT_NOEXCEPT {
            return index > other.index;
        }

        [[nodiscard]] bool operator>(const storage_iterator &other) const ENTT_NOEXCEPT {
            return index < other.index;
        }

        [[nodiscard]] bool operator<=(const storage_iterator &other) const ENTT_NOEXCEPT {
            return !(*this > other);
        }

        [[nodiscard]] bool operator>=(const storage_iterator &other) const ENTT_NOEXCEPT {
            return !(*this < other);
        }

        [[nodiscard]] pointer operator->() const ENTT_NOEXCEPT {
            const auto pos = size_type(index-1u);
            return std::addressof((*packed)[page(pos)][offset(pos)]);
        }

        [[nodiscard]] reference operator*() const ENTT_NOEXCEPT {
            return *operator->();
        }

    private:
        bucket_alloc_pointer const *packed;
        difference_type index;
    };

    [[nodiscard]] static auto page(const std::size_t pos) ENTT_NOEXCEPT {
        return pos / packed_page;
    }

    [[nodiscard]] static auto offset(const std::size_t pos) ENTT_NOEXCEPT {
        return pos & (packed_page - 1);
    }

    void release_memory() {
        if(packed) {
            // no-throw stable erase iteration
            underlying_type::clear();

            for(size_type pos{}; pos < bucket; ++pos) {
                alloc_traits::deallocate(allocator, packed[pos], packed_page);
                bucket_alloc_traits::destroy(bucket_allocator, std::addressof(packed[pos]));
            }

            bucket_alloc_traits::deallocate(bucket_allocator, packed, bucket);
        }
    }

    void maybe_resize_packed(const std::size_t req) {
        ENTT_ASSERT(!(req < underlying_type::size()), "Invalid request");

        if(const auto length = (req + packed_page - 1u) / packed_page; bucket != length) {
            const auto mem = bucket_alloc_traits::allocate(bucket_allocator, length);

            if(bucket > length) {
                std::uninitialized_copy(packed, packed + length, mem);

                for(auto pos = length; pos < bucket; ++pos) {
                    alloc_traits::deallocate(allocator, packed[pos], packed_page);
                }
            } else {
                std::uninitialized_copy(packed, packed + bucket, mem);

                size_type pos{};

                ENTT_TRY {
                    for(pos = bucket; pos < length; ++pos) {
                        auto pg = alloc_traits::allocate(allocator, packed_page);
                        bucket_alloc_traits::construct(bucket_allocator, std::addressof(mem[pos]), pg);
                    }
                } ENTT_CATCH {
                    for(auto next = bucket; next < pos; ++next) {
                        alloc_traits::deallocate(allocator, mem[next], packed_page);
                    }

                    std::destroy(mem, mem + pos);
                    bucket_alloc_traits::deallocate(bucket_allocator, mem, length);
                    ENTT_THROW;
                }
            }

            std::destroy(packed, packed + bucket);
            bucket_alloc_traits::deallocate(bucket_allocator, packed, bucket);

            packed = mem;
            bucket = length;
        }
    }

    template<typename... Args>
    auto & push_back(Args &&... args) {
        const auto length = underlying_type::size();
        ENTT_ASSERT(length < (bucket * packed_page), "No more space left");
        auto *instance = std::addressof(packed[page(length)][offset(length)]);

        if constexpr(std::is_aggregate_v<value_type>) {
            alloc_traits::construct(allocator, instance, Type{std::forward<Args>(args)...});
        } else {
            alloc_traits::construct(allocator, instance, std::forward<Args>(args)...);
        }

        return *instance;
    }

    void pop_back() {
        alloc_traits::destroy(allocator, std::addressof(packed[underlying_type::size()]));
    }

protected:
    /*! @copydoc basic_sparse_set::swap_at */
    void swap_at(const std::size_t lhs, const std::size_t rhs) final {
        std::swap(packed[page(lhs)][offset(lhs)], packed[page(rhs)][offset(rhs)]);
    }

    /*! @copydoc basic_sparse_set::move_and_pop */
    void move_and_pop(const std::size_t from, const std::size_t to) final {
        auto *instance = std::addressof(packed[page(to)][offset(to)]);
        auto *other = std::addressof(packed[page(from)][offset(from)]);
        alloc_traits::construct(allocator, instance, std::move(*other));
        alloc_traits::destroy(allocator, other);
    }

    /*! @copydoc basic_sparse_set::swap_and_pop */
    void swap_and_pop(const std::size_t pos) final {
        const auto length = underlying_type::size();
        auto &&elem = packed[page(pos)][offset(pos)];
        auto &&last = packed[page(length)][offset(length)];

        // support for nosy destructors
        [[maybe_unused]] auto unused = std::move(elem);
        elem = std::move(last);
        alloc_traits::destroy(allocator, std::addressof(last));
    }

    /*! @copydoc basic_sparse_set::in_place_pop */
    void in_place_pop(const std::size_t pos) final {
        alloc_traits::destroy(allocator, std::addressof(packed[page(pos)][offset(pos)]));
    }

public:
    /*! @brief Allocator type. */
    using allocator_type = typename alloc_traits::allocator_type;
    /*! @brief Type of the objects assigned to entities. */
    using value_type = Type;
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Pointer type to contained elements. */
    using pointer = bucket_alloc_pointer;
    /*! @brief Constant pointer type to contained elements. */
    using const_pointer = bucket_alloc_const_pointer;
    /*! @brief Random access iterator type. */
    using iterator = storage_iterator<value_type>;
    /*! @brief Constant random access iterator type. */
    using const_iterator = storage_iterator<const value_type>;
    /*! @brief Reverse iterator type. */
    using reverse_iterator = std::reverse_iterator<iterator>;
    /*! @brief Constant reverse iterator type. */
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * @brief Default constructor.
     * @param alloc Allocator to use (possibly default-constructed).
     */
    explicit basic_storage_impl(const allocator_type &alloc = {})
        : underlying_type{deletion_policy{comp_traits::in_place_delete::value}, alloc},
          allocator{alloc},
          bucket_allocator{alloc},
          packed{bucket_alloc_traits::allocate(bucket_allocator, 0u)},
          bucket{}
    {}

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    basic_storage_impl(basic_storage_impl &&other) ENTT_NOEXCEPT
        : underlying_type{std::move(other)},
          allocator{std::move(other.allocator)},
          bucket_allocator{std::move(other.bucket_allocator)},
          packed{std::exchange(other.packed, bucket_alloc_pointer{})},
          bucket{std::exchange(other.bucket, 0u)}
    {}

    /*! @brief Default destructor. */
    ~basic_storage_impl() override {
        release_memory();
    }

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This sparse set.
     */
    basic_storage_impl & operator=(basic_storage_impl &&other) ENTT_NOEXCEPT {
        release_memory();

        underlying_type::operator=(std::move(other));

        allocator = std::move(other.allocator);
        bucket_allocator = std::move(other.bucket_allocator);
        packed = std::exchange(other.packed, bucket_alloc_pointer{});
        bucket = std::exchange(other.bucket, 0u);

        return *this;
    }

    /**
     * @brief Increases the capacity of a storage.
     *
     * If the new capacity is greater than the current capacity, new storage is
     * allocated, otherwise the method does nothing.
     *
     * @param cap Desired capacity.
     */
    void reserve(const size_type cap) {
        underlying_type::reserve(cap);

        if(cap > (bucket * packed_page)) {
            maybe_resize_packed(cap);
        }
    }

    /**
     * @brief Returns the number of elements that a storage has currently
     * allocated space for.
     * @return Capacity of the storage.
     */
    [[nodiscard]] size_type capacity() const ENTT_NOEXCEPT {
        return bucket * packed_page;
    }

    /*! @brief Requests the removal of unused capacity. */
    void shrink_to_fit() {
        underlying_type::shrink_to_fit();
        maybe_resize_packed(underlying_type::size());
    }

    /**
     * @brief Direct access to the array of objects.
     * @return A pointer to the array of objects.
     */
    [[nodiscard]] const_pointer raw() const ENTT_NOEXCEPT {
        return packed;
    }

    /*! @copydoc raw */
    [[nodiscard]] pointer raw() ENTT_NOEXCEPT {
        return packed;
    }

    /**
     * @brief Returns an iterator to the beginning.
     *
     * The returned iterator points to the first instance of the internal array.
     * If the storage is empty, the returned iterator will be equal to `end()`.
     *
     * @return An iterator to the first instance of the internal array.
     */
    [[nodiscard]] const_iterator cbegin() const ENTT_NOEXCEPT {
        const difference_type pos = underlying_type::size();
        return const_iterator{std::addressof(packed), pos};
    }

    /*! @copydoc cbegin */
    [[nodiscard]] const_iterator begin() const ENTT_NOEXCEPT {
        return cbegin();
    }

    /*! @copydoc begin */
    [[nodiscard]] iterator begin() ENTT_NOEXCEPT {
        const difference_type pos = underlying_type::size();
        return iterator{std::addressof(packed), pos};
    }

    /**
     * @brief Returns an iterator to the end.
     *
     * The returned iterator points to the element following the last instance
     * of the internal array. Attempting to dereference the returned iterator
     * results in undefined behavior.
     *
     * @return An iterator to the element following the last instance of the
     * internal array.
     */
    [[nodiscard]] const_iterator cend() const ENTT_NOEXCEPT {
        return const_iterator{std::addressof(packed), {}};
    }

    /*! @copydoc cend */
    [[nodiscard]] const_iterator end() const ENTT_NOEXCEPT {
        return cend();
    }

    /*! @copydoc end */
    [[nodiscard]] iterator end() ENTT_NOEXCEPT {
        return iterator{std::addressof(packed), {}};
    }

    /**
     * @brief Returns a reverse iterator to the beginning.
     *
     * The returned iterator points to the first instance of the reversed
     * internal array. If the storage is empty, the returned iterator will be
     * equal to `rend()`.
     *
     * @return An iterator to the first instance of the reversed internal array.
     */
    [[nodiscard]] const_reverse_iterator crbegin() const ENTT_NOEXCEPT {
        return std::make_reverse_iterator(cend());
    }

    /*! @copydoc crbegin */
    [[nodiscard]] const_reverse_iterator rbegin() const ENTT_NOEXCEPT {
        return crbegin();
    }

    /*! @copydoc rbegin */
    [[nodiscard]] reverse_iterator rbegin() ENTT_NOEXCEPT {
        return std::make_reverse_iterator(end());
    }

    /**
     * @brief Returns a reverse iterator to the end.
     *
     * The returned iterator points to the element following the last instance
     * of the reversed internal array. Attempting to dereference the returned
     * iterator results in undefined behavior.
     *
     * @return An iterator to the element following the last instance of the
     * reversed internal array.
     */
    [[nodiscard]] const_reverse_iterator crend() const ENTT_NOEXCEPT {
        return std::make_reverse_iterator(cbegin());
    }

    /*! @copydoc crend */
    [[nodiscard]] const_reverse_iterator rend() const ENTT_NOEXCEPT {
        return crend();
    }

    /*! @copydoc rend */
    [[nodiscard]] reverse_iterator rend() ENTT_NOEXCEPT {
        return std::make_reverse_iterator(begin());
    }

    /**
     * @brief Returns the object assigned to an entity.
     *
     * @warning
     * Attempting to use an entity that doesn't belong to the storage results in
     * undefined behavior.
     *
     * @param entt A valid entity identifier.
     * @return The object assigned to the entity.
     */
    [[nodiscard]] const value_type & get(const entity_type entt) const ENTT_NOEXCEPT {
        const auto idx = underlying_type::index(entt);
        return packed[page(idx)][offset(idx)];
    }

    /*! @copydoc get */
    [[nodiscard]] value_type & get(const entity_type entt) ENTT_NOEXCEPT {
        return const_cast<value_type &>(std::as_const(*this).get(entt));
    }

    /**
     * @brief Assigns an entity to a storage and constructs its object.
     *
     * This version accept both types that can be constructed in place directly
     * and types like aggregates that do not work well with a placement new as
     * performed usually under the hood during an _emplace back_.
     *
     * @warning
     * Attempting to use an entity that already belongs to the storage results
     * in undefined behavior.
     *
     * @tparam Args Types of arguments to use to construct the object.
     * @param entt A valid entity identifier.
     * @param args Parameters to use to construct an object for the entity.
     * @return A reference to the newly created object.
     */
    template<typename... Args>
    value_type & emplace(const entity_type entt, Args &&... args) {
        maybe_resize_packed(underlying_type::size() + 1u);
        auto &value = push_back(std::forward<Args>(args)...);

        ENTT_TRY {
            underlying_type::emplace(entt);
        } ENTT_CATCH {
            pop_back();
            ENTT_THROW;
        }

        return value;
    }

    /**
     * @brief Updates the instance assigned to a given entity in-place.
     * @tparam Func Types of the function objects to invoke.
     * @param entity A valid entity identifier.
     * @param func Valid function objects.
     * @return A reference to the updated instance.
     */
    template<typename... Func>
    decltype(auto) patch(const entity_type entity, Func &&... func) {
        const auto idx = underlying_type::index(entity);
        auto &&elem = packed[page(idx)][offset(idx)];
        (std::forward<Func>(func)(elem), ...);
        return elem;
    }

    /**
     * @brief Assigns one or more entities to a storage and constructs their
     * objects from a given instance.
     *
     * @warning
     * Attempting to assign an entity that already belongs to the storage
     * results in undefined behavior.
     *
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @param value An instance of the object to construct.
     */
    template<typename It>
    void insert(It first, It last, const value_type &value = {}) {
        const auto sz = underlying_type::size() + std::distance(first, last);
        underlying_type::reserve(sz);
        maybe_resize_packed(sz);

        for(; first != last; ++first) {
            push_back(value);

            ENTT_TRY {
                underlying_type::emplace(*first);
            } ENTT_CATCH {
                pop_back();
                ENTT_THROW;
            }
        }
    }

    /**
     * @brief Assigns one or more entities to a storage and constructs their
     * objects from a given range.
     *
     * @sa construct
     *
     * @tparam EIt Type of input iterator.
     * @tparam CIt Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @param from An iterator to the first element of the range of objects.
     */
    template<typename EIt, typename CIt, typename = std::enable_if_t<std::is_same_v<std::decay_t<typename std::iterator_traits<CIt>::value_type>, value_type>>>
    void insert(EIt first, EIt last, CIt from) {
        const auto sz = underlying_type::size() + std::distance(first, last);
        underlying_type::reserve(sz);
        maybe_resize_packed(sz);

        for(; first != last; ++first, ++from) {
            push_back(*from);

            ENTT_TRY {
                underlying_type::emplace(*first);
            } ENTT_CATCH {
                pop_back();
                ENTT_THROW;
            }
        }
    }

    /**
     * @brief Sort elements according to the given comparison function.
     *
     * The comparison function object must return `true` if the first element
     * is _less_ than the second one, `false` otherwise. The signature of the
     * comparison function should be equivalent to one of the following:
     *
     * @code{.cpp}
     * bool(const Entity, const Entity);
     * bool(const Type &, const Type &);
     * @endcode
     *
     * Moreover, the comparison function object shall induce a
     * _strict weak ordering_ on the values.
     *
     * The sort function oject must offer a member function template
     * `operator()` that accepts three arguments:
     *
     * * An iterator to the first element of the range to sort.
     * * An iterator past the last element of the range to sort.
     * * A comparison function to use to compare the elements.
     *
     * @warning
     * Empty types are never instantiated. Therefore, only comparison function
     * objects that require to return entities rather than components are
     * accepted.
     *
     * @tparam Compare Type of comparison function object.
     * @tparam Sort Type of sort function object.
     * @tparam Args Types of arguments to forward to the sort function object.
     * @param length Number of elements to sort.
     * @param compare A valid comparison function object.
     * @param algo A valid sort function object.
     * @param args Arguments to forward to the sort function object, if any.
     */
    template<typename Compare, typename Sort = std_sort, typename... Args>
    void sort_n(const size_type length, Compare compare, Sort algo = Sort{}, Args &&... args) {
        if constexpr(std::is_invocable_v<Compare, const value_type &, const value_type &>) {
            underlying_type::sort_n(length, [this, compare = std::move(compare)](const auto lhs, const auto rhs) {
                const auto ilhs = underlying_type::index(lhs), irhs = underlying_type::index(rhs);
                return compare(std::as_const(packed[page(ilhs)][offset(ilhs)]), std::as_const(packed[page(irhs)][offset(irhs)]));
            }, std::move(algo), std::forward<Args>(args)...);
        } else {
            underlying_type::sort_n(length, std::move(compare), std::move(algo), std::forward<Args>(args)...);
        }
    }

    /**
     * @brief Sort all elements according to the given comparison function.
     *
     * @sa sort_n
     *
     * @tparam Compare Type of comparison function object.
     * @tparam Sort Type of sort function object.
     * @tparam Args Types of arguments to forward to the sort function object.
     * @param compare A valid comparison function object.
     * @param algo A valid sort function object.
     * @param args Arguments to forward to the sort function object, if any.
     */
    template<typename Compare, typename Sort = std_sort, typename... Args>
    void sort(Compare compare, Sort algo = Sort{}, Args &&... args) {
        sort_n(underlying_type::size(), std::move(compare), std::move(algo), std::forward<Args>(args)...);
    }

private:
    typename alloc_traits::allocator_type allocator;
    typename bucket_alloc_traits::allocator_type bucket_allocator;
    bucket_alloc_pointer packed;
    std::size_t bucket;
};


/*! @copydoc basic_storage_impl */
template<typename Entity, typename Type, typename Allocator>
class basic_storage_impl<Entity, Type, Allocator, std::enable_if_t<component_traits<Type>::ignore_if_empty::value && std::is_empty_v<Type>>>
    : public basic_sparse_set<Entity, typename std::allocator_traits<Allocator>::template rebind_alloc<Entity>>
{
    using comp_traits = component_traits<Type>;
    using underlying_type = basic_sparse_set<Entity, typename std::allocator_traits<Allocator>::template rebind_alloc<Entity>>;
    using alloc_traits = typename std::allocator_traits<Allocator>::template rebind_traits<Type>;

public:
    /*! @brief Allocator type. */
    using allocator_type = typename alloc_traits::allocator_type;
    /*! @brief Type of the objects assigned to entities. */
    using value_type = Type;
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;

    /**
     * @brief Default constructor.
     * @param alloc Allocator to use (possibly default-constructed).
     */
    explicit basic_storage_impl(const allocator_type &alloc = {})
        : underlying_type{deletion_policy{comp_traits::in_place_delete::value}, alloc}
    {}

    /**
     * @brief Fake get function.
     *
     * @warning
     * Attempting to use an entity that doesn't belong to the storage results in
     * undefined behavior.
     *
     * @param entt A valid entity identifier.
     */
    void get([[maybe_unused]] const entity_type entt) const ENTT_NOEXCEPT {
        ENTT_ASSERT(underlying_type::contains(entt), "Storage does not contain entity");
    }

    /**
     * @brief Assigns an entity to a storage and constructs its object.
     *
     * @warning
     * Attempting to use an entity that already belongs to the storage results
     * in undefined behavior.
     *
     * @tparam Args Types of arguments to use to construct the object.
     * @param entt A valid entity identifier.
     * @param args Parameters to use to construct an object for the entity.
     */
    template<typename... Args>
    void emplace(const entity_type entt, Args &&... args) {
        [[maybe_unused]] value_type instance{std::forward<Args>(args)...};
        underlying_type::emplace(entt);
    }

    /**
    * @brief Updates the instance assigned to a given entity in-place.
    * @tparam Func Types of the function objects to invoke.
    * @param entity A valid entity identifier.
    * @param func Valid function objects.
    */
    template<typename... Func>
    void patch([[maybe_unused]] const entity_type entity, Func &&... func) {
        ENTT_ASSERT(underlying_type::contains(entity), "Storage does not contain entity");
        (std::forward<Func>(func)(), ...);
    }

    /**
     * @brief Assigns one or more entities to a storage.
     *
     * @warning
     * Attempting to assign an entity that already belongs to the storage
     * results in undefined behavior.
     *
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     */
    template<typename It>
    void insert(It first, It last, const value_type & = {}) {
        underlying_type::insert(first, last);
    }
};


/**
 * @brief Mixin type to use to wrap basic storage classes.
 * @tparam Type The type of the underlying storage.
 */
template<typename Type>
struct storage_adapter_mixin: Type {
    static_assert(std::is_same_v<typename Type::value_type, std::decay_t<typename Type::value_type>>, "Invalid object type");

    /*! @brief Type of the objects assigned to entities. */
    using value_type = typename Type::value_type;
    /*! @brief Underlying entity identifier. */
    using entity_type = typename Type::entity_type;

    /*! @brief Inherited constructors. */
    using Type::Type;

    /**
     * @brief Assigns entities to a storage.
     * @tparam Args Types of arguments to use to construct the object.
     * @param entity A valid entity identifier.
     * @param args Parameters to use to initialize the object.
     * @return A reference to the newly created object.
     */
    template<typename... Args>
    decltype(auto) emplace(basic_registry<entity_type> &, const entity_type entity, Args &&... args) {
        return Type::emplace(entity, std::forward<Args>(args)...);
    }

    /**
     * @brief Assigns entities to a storage.
     * @tparam It Type of input iterator.
     * @tparam Args Types of arguments to use to construct the objects assigned
     * to the entities.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @param args Parameters to use to initialize the objects assigned to the
     * entities.
     */
    template<typename It, typename... Args>
    void insert(basic_registry<entity_type> &, It first, It last, Args &&... args) {
        Type::insert(first, last, std::forward<Args>(args)...);
    }

    /**
     * @brief Patches the given instance for an entity.
     * @tparam Func Types of the function objects to invoke.
     * @param entity A valid entity identifier.
     * @param func Valid function objects.
     * @return A reference to the patched instance.
     */
    template<typename... Func>
    decltype(auto) patch(basic_registry<entity_type> &, const entity_type entity, Func &&... func) {
        return Type::patch(entity, std::forward<Func>(func)...);
    }
};


/**
 * @brief Mixin type to use to add signal support to storage types.
 * @tparam Type The type of the underlying storage.
 */
template<typename Type>
class sigh_storage_mixin final: public Type {
    /*! @copydoc basic_sparse_set::about_to_pop */
    void about_to_pop(const typename Type::entity_type entity, void *ud) final {
        ENTT_ASSERT(ud != nullptr, "Invalid pointer to registry");
        destruction.publish(*static_cast<basic_registry<typename Type::entity_type> *>(ud), entity);
        Type::about_to_pop(entity, ud);
    }

public:
    /*! @brief Underlying value type. */
    using value_type = typename Type::value_type;
    /*! @brief Underlying entity identifier. */
    using entity_type = typename Type::entity_type;

    /*! @brief Inherited constructors. */
    using Type::Type;

    /**
     * @brief Returns a sink object.
     *
     * The sink returned by this function can be used to receive notifications
     * whenever a new instance is created and assigned to an entity.<br/>
     * The function type for a listener is equivalent to:
     *
     * @code{.cpp}
     * void(basic_registry<entity_type> &, entity_type);
     * @endcode
     *
     * Listeners are invoked **after** the object has been assigned to the
     * entity.
     *
     * @sa sink
     *
     * @return A temporary sink object.
     */
    [[nodiscard]] auto on_construct() ENTT_NOEXCEPT {
        return sink{construction};
    }

    /**
     * @brief Returns a sink object.
     *
     * The sink returned by this function can be used to receive notifications
     * whenever an instance is explicitly updated.<br/>
     * The function type for a listener is equivalent to:
     *
     * @code{.cpp}
     * void(basic_registry<entity_type> &, entity_type);
     * @endcode
     *
     * Listeners are invoked **after** the object has been updated.
     *
     * @sa sink
     *
     * @return A temporary sink object.
     */
    [[nodiscard]] auto on_update() ENTT_NOEXCEPT {
        return sink{update};
    }

    /**
     * @brief Returns a sink object.
     *
     * The sink returned by this function can be used to receive notifications
     * whenever an instance is removed from an entity and thus destroyed.<br/>
     * The function type for a listener is equivalent to:
     *
     * @code{.cpp}
     * void(basic_registry<entity_type> &, entity_type);
     * @endcode
     *
     * Listeners are invoked **before** the object has been removed from the
     * entity.
     *
     * @sa sink
     *
     * @return A temporary sink object.
     */
    [[nodiscard]] auto on_destroy() ENTT_NOEXCEPT {
        return sink{destruction};
    }

    /**
     * @brief Assigns entities to a storage.
     * @tparam Args Types of arguments to use to construct the object.
     * @param owner The registry that issued the request.
     * @param entity A valid entity identifier.
     * @param args Parameters to use to initialize the object.
     * @return A reference to the newly created object.
     */
    template<typename... Args>
    decltype(auto) emplace(basic_registry<entity_type> &owner, const entity_type entity, Args &&... args) {
        Type::emplace(entity, std::forward<Args>(args)...);
        construction.publish(owner, entity);
        return this->get(entity);
    }

    /**
     * @brief Assigns entities to a storage.
     * @tparam It Type of input iterator.
     * @tparam Args Types of arguments to use to construct the objects assigned
     * to the entities.
     * @param owner The registry that issued the request.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @param args Parameters to use to initialize the objects assigned to the
     * entities.
     */
    template<typename It, typename... Args>
    void insert(basic_registry<entity_type> &owner, It first, It last, Args &&... args) {
        Type::insert(first, last, std::forward<Args>(args)...);

        if(!construction.empty()) {
            for(; first != last; ++first) {
                construction.publish(owner, *first);
            }
        }
    }

    /**
     * @brief Patches the given instance for an entity.
     * @tparam Func Types of the function objects to invoke.
     * @param owner The registry that issued the request.
     * @param entity A valid entity identifier.
     * @param func Valid function objects.
     * @return A reference to the patched instance.
     */
    template<typename... Func>
    decltype(auto) patch(basic_registry<entity_type> &owner, const entity_type entity, Func &&... func) {
        Type::patch(entity, std::forward<Func>(func)...);
        update.publish(owner, entity);
        return this->get(entity);
    }

private:
    sigh<void(basic_registry<entity_type> &, const entity_type)> construction{};
    sigh<void(basic_registry<entity_type> &, const entity_type)> destruction{};
    sigh<void(basic_registry<entity_type> &, const entity_type)> update{};
};


/**
 * @brief Storage implementation dispatcher.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Type Type of objects assigned to the entities.
 * @tparam Allocator Type of allocator used to manage memory and elements.
 */
template<typename Entity, typename Type, typename Allocator>
struct basic_storage: basic_storage_impl<Entity, Type, Allocator> {
    using basic_storage_impl<Entity, Type, Allocator>::basic_storage_impl;
};


/**
 * @brief Provides a common way to access certain properties of storage types.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Type Type of objects managed by the storage class.
 */
template<typename Entity, typename Type, typename = void>
struct storage_traits {
    /*! @brief Resulting type after component-to-storage conversion. */
    using storage_type = sigh_storage_mixin<basic_storage<Entity, Type>>;
};


/**
 * @brief Gets the element assigned to an entity from a storage, if any.
 * @tparam Type Storage type.
 * @param container A valid instance of a storage class.
 * @param entity A valid entity identifier.
 * @return A possibly empty tuple containing the requested element.
 */
template<typename Type>
[[nodiscard]] auto get_as_tuple([[maybe_unused]] Type &container, [[maybe_unused]] const typename Type::entity_type entity) {
    static_assert(std::is_same_v<std::remove_const_t<Type>, typename storage_traits<typename Type::entity_type, typename Type::value_type>::storage_type>, "Invalid storage");

    if constexpr(std::is_void_v<decltype(container.get({}))>) {
        return std::make_tuple();
    } else {
        return std::forward_as_tuple(container.get(entity));
    }
}


}


#endif

// #include "utility.hpp"
#ifndef ENTT_ENTITY_UTILITY_HPP
#define ENTT_ENTITY_UTILITY_HPP


// #include "../core/type_traits.hpp"



namespace entt {


/**
 * @brief Alias for exclusion lists.
 * @tparam Type List of types.
 */
template<typename... Type>
struct exclude_t: type_list<Type...> {};


/**
 * @brief Variable template for exclusion lists.
 * @tparam Type List of types.
 */
template<typename... Type>
inline constexpr exclude_t<Type...> exclude{};


/**
 * @brief Alias for lists of observed components.
 * @tparam Type List of types.
 */
template<typename... Type>
struct get_t: type_list<Type...>{};


/**
 * @brief Variable template for lists of observed components.
 * @tparam Type List of types.
 */
template<typename... Type>
inline constexpr get_t<Type...> get{};


}


#endif



namespace entt {


/**
 * @brief Group.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error, but for a few reasonable cases.
 */
template<typename...>
class basic_group;


/**
 * @brief Non-owning group.
 *
 * A non-owning group returns all entities and only the entities that have at
 * least the given components. Moreover, it's guaranteed that the entity list
 * is tightly packed in memory for fast iterations.
 *
 * @b Important
 *
 * Iterators aren't invalidated if:
 *
 * * New instances of the given components are created and assigned to entities.
 * * The entity currently pointed is modified (as an example, if one of the
 *   given components is removed from the entity to which the iterator points).
 * * The entity currently pointed is destroyed.
 *
 * In all other cases, modifying the pools iterated by the group in any way
 * invalidates all the iterators and using them results in undefined behavior.
 *
 * @note
 * Groups share references to the underlying data structures of the registry
 * that generated them. Therefore any change to the entities and to the
 * components made by means of the registry are immediately reflected by all the
 * groups.<br/>
 * Moreover, sorting a non-owning group affects all the instances of the same
 * group (it means that users don't have to call `sort` on each instance to sort
 * all of them because they _share_ entities and components).
 *
 * @warning
 * Lifetime of a group must not overcome that of the registry that generated it.
 * In any other case, attempting to use a group results in undefined behavior.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Exclude Types of components used to filter the group.
 * @tparam Get Type of components observed by the group.
 */
template<typename Entity, typename... Exclude, typename... Get>
class basic_group<Entity, exclude_t<Exclude...>, get_t<Get...>> final {
    /*! @brief A registry is allowed to create groups. */
    friend class basic_registry<Entity>;

    using basic_common_type = basic_sparse_set<Entity>;

    template<typename Component>
    using storage_type = constness_as_t<typename storage_traits<Entity, std::remove_const_t<Component>>::storage_type, Component>;

    class iterable final {
        template<typename It>
        struct iterable_iterator final {
            using difference_type = std::ptrdiff_t;
            using value_type = decltype(std::tuple_cat(std::tuple<Entity>{}, std::declval<basic_group>().get({})));
            using pointer = void;
            using reference = value_type;
            using iterator_category = std::input_iterator_tag;

            template<typename... Args>
            iterable_iterator(It from, const std::tuple<storage_type<Get> *...> &args) ENTT_NOEXCEPT
                : it{from},
                  pools{args}
            {}

            iterable_iterator & operator++() ENTT_NOEXCEPT {
                return ++it, *this;
            }

            iterable_iterator operator++(int) ENTT_NOEXCEPT {
                iterable_iterator orig = *this;
                return ++(*this), orig;
            }

            [[nodiscard]] reference operator*() const ENTT_NOEXCEPT {
                const auto entt = *it;
                return std::tuple_cat(std::make_tuple(entt), get_as_tuple(*std::get<storage_type<Get> *>(pools), entt)...);
            }

            [[nodiscard]] bool operator==(const iterable_iterator &other) const ENTT_NOEXCEPT {
                return other.it == it;
            }

            [[nodiscard]] bool operator!=(const iterable_iterator &other) const ENTT_NOEXCEPT {
                return !(*this == other);
            }

        private:
            It it;
            std::tuple<storage_type<Get> *...> pools;
        };

    public:
        using iterator = iterable_iterator<typename basic_common_type::iterator>;
        using reverse_iterator = iterable_iterator<typename basic_common_type::reverse_iterator>;

        iterable(basic_common_type * const ref, const std::tuple<storage_type<Get> *...> &cpools)
            : handler{ref},
              pools{cpools}
        {}

        [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
            return handler ? iterator{handler->begin(), pools} : iterator{{}, pools};
        }

        [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
            return handler ? iterator{handler->end(), pools} : iterator{{}, pools};
        }

        [[nodiscard]] reverse_iterator rbegin() const ENTT_NOEXCEPT {
            return handler ? reverse_iterator{handler->rbegin(), pools} : reverse_iterator{{}, pools};
        }

        [[nodiscard]] reverse_iterator rend() const ENTT_NOEXCEPT {
            return handler ? reverse_iterator{handler->rend(), pools} : reverse_iterator{{}, pools};
        }

    private:
        basic_common_type * const handler;
        const std::tuple<storage_type<Get> *...> pools;
    };

    basic_group(basic_common_type &ref, storage_type<Get> &... gpool) ENTT_NOEXCEPT
        : handler{&ref},
          pools{&gpool...}
    {}

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Random access iterator type. */
    using iterator = typename basic_common_type::iterator;
    /*! @brief Reversed iterator type. */
    using reverse_iterator = typename basic_common_type::reverse_iterator;
    /*! @brief Iterable group type. */
    using iterable_group = iterable;

    /*! @brief Default constructor to use to create empty, invalid groups. */
    basic_group() ENTT_NOEXCEPT
        : handler{}
    {}

    /**
     * @brief Returns the number of entities that have the given components.
     * @return Number of entities that have the given components.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return *this ? handler->size() : size_type{};
    }

    /**
     * @brief Returns the number of elements that a group has currently
     * allocated space for.
     * @return Capacity of the group.
     */
    [[nodiscard]] size_type capacity() const ENTT_NOEXCEPT {
        return *this ? handler->capacity() : size_type{};
    }

    /*! @brief Requests the removal of unused capacity. */
    void shrink_to_fit() {
        if(*this) {
            handler->shrink_to_fit();
        }
    }

    /**
     * @brief Checks whether a group is empty.
     * @return True if the group is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return !*this || handler->empty();
    }

    /**
     * @brief Direct access to the list of entities.
     *
     * The returned pointer is such that range `[data(), data() + size())` is
     * always a valid range, even if the container is empty.
     *
     * @return A pointer to the array of entities.
     */
    [[nodiscard]] auto data() const ENTT_NOEXCEPT {
        return *this ? handler->data() : nullptr;
    }

    /**
     * @brief Returns an iterator to the first entity of the group.
     *
     * The returned iterator points to the first entity of the group. If the
     * group is empty, the returned iterator will be equal to `end()`.
     *
     * @return An iterator to the first entity of the group.
     */
    [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
        return *this ? handler->begin() : iterator{};
    }

    /**
     * @brief Returns an iterator that is past the last entity of the group.
     *
     * The returned iterator points to the entity following the last entity of
     * the group. Attempting to dereference the returned iterator results in
     * undefined behavior.
     *
     * @return An iterator to the entity following the last entity of the
     * group.
     */
    [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
        return *this ? handler->end() : iterator{};
    }

    /**
     * @brief Returns an iterator to the first entity of the reversed group.
     *
     * The returned iterator points to the first entity of the reversed group.
     * If the group is empty, the returned iterator will be equal to `rend()`.
     *
     * @return An iterator to the first entity of the reversed group.
     */
    [[nodiscard]] reverse_iterator rbegin() const ENTT_NOEXCEPT {
        return *this ? handler->rbegin() : reverse_iterator{};
    }

    /**
     * @brief Returns an iterator that is past the last entity of the reversed
     * group.
     *
     * The returned iterator points to the entity following the last entity of
     * the reversed group. Attempting to dereference the returned iterator
     * results in undefined behavior.
     *
     * @return An iterator to the entity following the last entity of the
     * reversed group.
     */
    [[nodiscard]] reverse_iterator rend() const ENTT_NOEXCEPT {
        return *this ? handler->rend() : reverse_iterator{};
    }

    /**
     * @brief Returns the first entity of the group, if any.
     * @return The first entity of the group if one exists, the null entity
     * otherwise.
     */
    [[nodiscard]] entity_type front() const {
        const auto it = begin();
        return it != end() ? *it : null;
    }

    /**
     * @brief Returns the last entity of the group, if any.
     * @return The last entity of the group if one exists, the null entity
     * otherwise.
     */
    [[nodiscard]] entity_type back() const {
        const auto it = rbegin();
        return it != rend() ? *it : null;
    }

    /**
     * @brief Finds an entity.
     * @param entt A valid entity identifier.
     * @return An iterator to the given entity if it's found, past the end
     * iterator otherwise.
     */
    [[nodiscard]] iterator find(const entity_type entt) const {
        const auto it = *this ? handler->find(entt) : iterator{};
        return it != end() && *it == entt ? it : end();
    }

    /**
     * @brief Returns the identifier that occupies the given position.
     * @param pos Position of the element to return.
     * @return The identifier that occupies the given position.
     */
    [[nodiscard]] entity_type operator[](const size_type pos) const {
        return begin()[pos];
    }

    /**
     * @brief Checks if a group is properly initialized.
     * @return True if the group is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return handler != nullptr;
    }

    /**
     * @brief Checks if a group contains an entity.
     * @param entt A valid entity identifier.
     * @return True if the group contains the given entity, false otherwise.
     */
    [[nodiscard]] bool contains(const entity_type entt) const {
        return *this && handler->contains(entt);
    }

    /**
     * @brief Returns the components assigned to the given entity.
     *
     * Prefer this function instead of `registry::get` during iterations. It has
     * far better performance than its counterpart.
     *
     * @warning
     * Attempting to use an invalid component type results in a compilation
     * error. Attempting to use an entity that doesn't belong to the group
     * results in undefined behavior.
     *
     * @tparam Component Types of components to get.
     * @param entt A valid entity identifier.
     * @return The components assigned to the entity.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) get(const entity_type entt) const {
        ENTT_ASSERT(contains(entt), "Group does not contain entity");

        if constexpr(sizeof...(Component) == 0) {
            return std::tuple_cat(get_as_tuple(*std::get<storage_type<Get> *>(pools), entt)...);
        } else if constexpr(sizeof...(Component) == 1) {
            return (std::get<storage_type<Component> *>(pools)->get(entt), ...);
        } else {
            return std::tuple_cat(get_as_tuple(*std::get<storage_type<Component> *>(pools), entt)...);
        }
    }

    /**
     * @brief Iterates entities and components and applies the given function
     * object to them.
     *
     * The function object is invoked for each entity. It is provided with the
     * entity itself and a set of references to non-empty components. The
     * _constness_ of the components is as requested.<br/>
     * The signature of the function must be equivalent to one of the following
     * forms:
     *
     * @code{.cpp}
     * void(const entity_type, Type &...);
     * void(Type &...);
     * @endcode
     *
     * @note
     * Empty types aren't explicitly instantiated and therefore they are never
     * returned during iterations.
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void each(Func func) const {
        for(const auto entt: *this) {
            if constexpr(is_applicable_v<Func, decltype(std::tuple_cat(std::tuple<entity_type>{}, std::declval<basic_group>().get({})))>) {
                std::apply(func, std::tuple_cat(std::make_tuple(entt), get(entt)));
            } else {
                std::apply(func, get(entt));
            }
        }
    }

    /**
     * @brief Returns an iterable object to use to _visit_ the group.
     *
     * The iterable object returns tuples that contain the current entity and a
     * set of references to its non-empty components. The _constness_ of the
     * components is as requested.
     *
     * @note
     * Empty types aren't explicitly instantiated and therefore they are never
     * returned during iterations.
     *
     * @return An iterable object to use to _visit_ the group.
     */
    [[nodiscard]] iterable_group each() const ENTT_NOEXCEPT {
        return iterable_group{handler, pools};
    }

    /**
     * @brief Sort a group according to the given comparison function.
     *
     * Sort the group so that iterating it with a couple of iterators returns
     * entities and components in the expected order. See `begin` and `end` for
     * more details.
     *
     * The comparison function object must return `true` if the first element
     * is _less_ than the second one, `false` otherwise. The signature of the
     * comparison function should be equivalent to one of the following:
     *
     * @code{.cpp}
     * bool(std::tuple<Component &...>, std::tuple<Component &...>);
     * bool(const Component &..., const Component &...);
     * bool(const Entity, const Entity);
     * @endcode
     *
     * Where `Component` are such that they are iterated by the group.<br/>
     * Moreover, the comparison function object shall induce a
     * _strict weak ordering_ on the values.
     *
     * The sort function oject must offer a member function template
     * `operator()` that accepts three arguments:
     *
     * * An iterator to the first element of the range to sort.
     * * An iterator past the last element of the range to sort.
     * * A comparison function to use to compare the elements.
     *
     * @tparam Component Optional types of components to compare.
     * @tparam Compare Type of comparison function object.
     * @tparam Sort Type of sort function object.
     * @tparam Args Types of arguments to forward to the sort function object.
     * @param compare A valid comparison function object.
     * @param algo A valid sort function object.
     * @param args Arguments to forward to the sort function object, if any.
     */
    template<typename... Component, typename Compare, typename Sort = std_sort, typename... Args>
    void sort(Compare compare, Sort algo = Sort{}, Args &&... args) {
        if(*this) {
            if constexpr(sizeof...(Component) == 0) {
                static_assert(std::is_invocable_v<Compare, const entity_type, const entity_type>, "Invalid comparison function");
                handler->sort(std::move(compare), std::move(algo), std::forward<Args>(args)...);
            }  else if constexpr(sizeof...(Component) == 1) {
                handler->sort([this, compare = std::move(compare)](const entity_type lhs, const entity_type rhs) {
                    return compare((std::get<storage_type<Component> *>(pools)->get(lhs), ...), (std::get<storage_type<Component> *>(pools)->get(rhs), ...));
                }, std::move(algo), std::forward<Args>(args)...);
            } else {
                handler->sort([this, compare = std::move(compare)](const entity_type lhs, const entity_type rhs) {
                    return compare(std::forward_as_tuple(std::get<storage_type<Component> *>(pools)->get(lhs)...), std::forward_as_tuple(std::get<storage_type<Component> *>(pools)->get(rhs)...));
                }, std::move(algo), std::forward<Args>(args)...);
            }
        }
    }

    /**
     * @brief Sort the shared pool of entities according to the given component.
     *
     * Non-owning groups of the same type share with the registry a pool of
     * entities with its own order that doesn't depend on the order of any pool
     * of components. Users can order the underlying data structure so that it
     * respects the order of the pool of the given component.
     *
     * @note
     * The shared pool of entities and thus its order is affected by the changes
     * to each and every pool that it tracks. Therefore changes to those pools
     * can quickly ruin the order imposed to the pool of entities shared between
     * the non-owning groups.
     *
     * @tparam Component Type of component to use to impose the order.
     */
    template<typename Component>
    void sort() const {
        if(*this) {
            handler->respect(*std::get<storage_type<Component> *>(pools));
        }
    }

private:
    basic_common_type * const handler;
    const std::tuple<storage_type<Get> *...> pools;
};


/**
 * @brief Owning group.
 *
 * Owning groups return all entities and only the entities that have at least
 * the given components. Moreover:
 *
 * * It's guaranteed that the entity list is tightly packed in memory for fast
 *   iterations.
 * * It's guaranteed that the lists of owned components are tightly packed in
 *   memory for even faster iterations and to allow direct access.
 * * They stay true to the order of the owned components and all instances have
 *   the same order in memory.
 *
 * The more types of components are owned by a group, the faster it is to
 * iterate them.
 *
 * @b Important
 *
 * Iterators aren't invalidated if:
 *
 * * New instances of the given components are created and assigned to entities.
 * * The entity currently pointed is modified (as an example, if one of the
 *   given components is removed from the entity to which the iterator points).
 * * The entity currently pointed is destroyed.
 *
 * In all other cases, modifying the pools iterated by the group in any way
 * invalidates all the iterators and using them results in undefined behavior.
 *
 * @note
 * Groups share references to the underlying data structures of the registry
 * that generated them. Therefore any change to the entities and to the
 * components made by means of the registry are immediately reflected by all the
 * groups.
 * Moreover, sorting an owning group affects all the instance of the same group
 * (it means that users don't have to call `sort` on each instance to sort all
 * of them because they share the underlying data structure).
 *
 * @warning
 * Lifetime of a group must not overcome that of the registry that generated it.
 * In any other case, attempting to use a group results in undefined behavior.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Exclude Types of components used to filter the group.
 * @tparam Get Types of components observed by the group.
 * @tparam Owned Types of components owned by the group.
 */
template<typename Entity, typename... Exclude, typename... Get, typename... Owned>
class basic_group<Entity, exclude_t<Exclude...>, get_t<Get...>, Owned...> final {
    /*! @brief A registry is allowed to create groups. */
    friend class basic_registry<Entity>;

    using basic_common_type = basic_sparse_set<Entity>;

    template<typename Component>
    using storage_type = constness_as_t<typename storage_traits<Entity, std::remove_const_t<Component>>::storage_type, Component>;

    class iterable final {
        template<typename, typename>
        struct iterable_iterator;

        template<typename It, typename... OIt>
        struct iterable_iterator<It, type_list<OIt...>> final {
            using difference_type = std::ptrdiff_t;
            using value_type = decltype(std::tuple_cat(std::tuple<Entity>{}, std::declval<basic_group>().get({})));
            using pointer = void;
            using reference = value_type;
            using iterator_category = std::input_iterator_tag;

            template<typename... Other>
            iterable_iterator(It from, const std::tuple<Other...> &other, const std::tuple<storage_type<Get> *...> &cpools) ENTT_NOEXCEPT
                : it{from},
                  owned{std::get<OIt>(other)...},
                  get{cpools}
            {}

            iterable_iterator & operator++() ENTT_NOEXCEPT {
                return ++it, (++std::get<OIt>(owned), ...), *this;
            }

            iterable_iterator operator++(int) ENTT_NOEXCEPT {
                iterable_iterator orig = *this;
                return ++(*this), orig;
            }

            [[nodiscard]] reference operator*() const ENTT_NOEXCEPT {
                return std::tuple_cat(
                    std::make_tuple(*it),
                    std::forward_as_tuple(*std::get<OIt>(owned)...),
                    get_as_tuple(*std::get<storage_type<Get> *>(get), *it)...
                );
            }

            [[nodiscard]] bool operator==(const iterable_iterator &other) const ENTT_NOEXCEPT {
                return other.it == it;
            }

            [[nodiscard]] bool operator!=(const iterable_iterator &other) const ENTT_NOEXCEPT {
                return !(*this == other);
            }

        private:
            It it;
            std::tuple<OIt...> owned;
            std::tuple<storage_type<Get> *...> get;
        };

    public:
        using iterator = iterable_iterator<
            typename basic_common_type::iterator,
            type_list_cat_t<std::conditional_t<std::is_void_v<decltype(std::declval<storage_type<Owned>>().get({}))>, type_list<>, type_list<decltype(std::declval<storage_type<Owned>>().end())>>...>
        >;
        using reverse_iterator = iterable_iterator<
            typename basic_common_type::reverse_iterator,
            type_list_cat_t<std::conditional_t<std::is_void_v<decltype(std::declval<storage_type<Owned>>().get({}))>, type_list<>, type_list<decltype(std::declval<storage_type<Owned>>().rbegin())>>...>
        >;

        iterable(std::tuple<storage_type<Owned> *..., storage_type<Get> *...> cpools, const std::size_t * const extent)
            : pools{cpools},
              length{extent}
        {}

        [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
            return length ? iterator{
                std::get<0>(pools)->basic_common_type::end() - *length,
                std::make_tuple((std::get<storage_type<Owned> *>(pools)->end() - *length)...),
                std::make_tuple(std::get<storage_type<Get> *>(pools)...)
            } : iterator{{}, std::make_tuple(decltype(std::get<storage_type<Owned> *>(pools)->end()){}...), std::make_tuple(std::get<storage_type<Get> *>(pools)...)};
        }

        [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
            return length ? iterator{
                std::get<0>(pools)->basic_common_type::end(),
                std::make_tuple((std::get<storage_type<Owned> *>(pools)->end())...),
                std::make_tuple(std::get<storage_type<Get> *>(pools)...)
            } : iterator{{}, std::make_tuple(decltype(std::get<storage_type<Owned> *>(pools)->end()){}...), std::make_tuple(std::get<storage_type<Get> *>(pools)...)};
        }

        [[nodiscard]] reverse_iterator rbegin() const ENTT_NOEXCEPT {
            return length ? reverse_iterator{
                std::get<0>(pools)->basic_common_type::rbegin(),
                std::make_tuple((std::get<storage_type<Owned> *>(pools)->rbegin())...),
                std::make_tuple(std::get<storage_type<Get> *>(pools)...)
            } : reverse_iterator{{}, std::make_tuple(decltype(std::get<storage_type<Owned> *>(pools)->rbegin()){}...), std::make_tuple(std::get<storage_type<Get> *>(pools)...)};
        }

        [[nodiscard]] reverse_iterator rend() const ENTT_NOEXCEPT {
            return length ? reverse_iterator{
                std::get<0>(pools)->basic_common_type::rbegin() + *length,
                std::make_tuple((std::get<storage_type<Owned> *>(pools)->rbegin() + *length)...),
                std::make_tuple(std::get<storage_type<Get> *>(pools)...)
            } : reverse_iterator{{}, std::make_tuple(decltype(std::get<storage_type<Owned> *>(pools)->rbegin()){}...), std::make_tuple(std::get<storage_type<Get> *>(pools)...)};
        }

    private:
        const std::tuple<storage_type<Owned> *..., storage_type<Get> *...> pools;
        const std::size_t * const length;
    };

    basic_group(const std::size_t &extent, storage_type<Owned> &... opool, storage_type<Get> &... gpool) ENTT_NOEXCEPT
        : pools{&opool..., &gpool...},
          length{&extent}
    {}

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Random access iterator type. */
    using iterator = typename basic_common_type::iterator;
    /*! @brief Reversed iterator type. */
    using reverse_iterator = typename basic_common_type::reverse_iterator;
    /*! @brief Iterable group type. */
    using iterable_group = iterable;

    /*! @brief Default constructor to use to create empty, invalid groups. */
    basic_group() ENTT_NOEXCEPT
        : length{}
    {}

    /**
     * @brief Returns the number of entities that have the given components.
     * @return Number of entities that have the given components.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return *this ? *length : size_type{};
    }

    /**
     * @brief Checks whether a group is empty.
     * @return True if the group is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return !*this || !*length;
    }

    /**
     * @brief Direct access to the raw representation offered by the storage.
     *
     * @warning
     * This function is only available for owned types.
     *
     * @tparam Component Type of component in which one is interested.
     * @return A pointer to the array of components.
     */
    template<typename Component>
    [[nodiscard]] auto raw() const ENTT_NOEXCEPT {
        static_assert((std::is_same_v<Component, Owned> || ...), "Non-owned type");
        auto *cpool = std::get<storage_type<Component> *>(pools);
        return cpool ? cpool->raw() : decltype(cpool->raw()){};
    }

    /**
     * @brief Direct access to the list of entities.
     *
     * The returned pointer is such that range `[data(), data() + size())` is
     * always a valid range, even if the container is empty.
     *
     * @return A pointer to the array of entities.
     */
    [[nodiscard]] auto data() const ENTT_NOEXCEPT {
        return *this ? std::get<0>(pools)->data() : nullptr;
    }

    /**
     * @brief Returns an iterator to the first entity of the group.
     *
     * The returned iterator points to the first entity of the group. If the
     * group is empty, the returned iterator will be equal to `end()`.
     *
     * @return An iterator to the first entity of the group.
     */
    [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
        return *this ? (std::get<0>(pools)->basic_common_type::end() - *length) : iterator{};
    }

    /**
     * @brief Returns an iterator that is past the last entity of the group.
     *
     * The returned iterator points to the entity following the last entity of
     * the group. Attempting to dereference the returned iterator results in
     * undefined behavior.
     *
     * @return An iterator to the entity following the last entity of the
     * group.
     */
    [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
        return *this ? std::get<0>(pools)->basic_common_type::end() : iterator{};
    }

    /**
     * @brief Returns an iterator to the first entity of the reversed group.
     *
     * The returned iterator points to the first entity of the reversed group.
     * If the group is empty, the returned iterator will be equal to `rend()`.
     *
     * @return An iterator to the first entity of the reversed group.
     */
    [[nodiscard]] reverse_iterator rbegin() const ENTT_NOEXCEPT {
        return *this ? std::get<0>(pools)->basic_common_type::rbegin() : reverse_iterator{};
    }

    /**
     * @brief Returns an iterator that is past the last entity of the reversed
     * group.
     *
     * The returned iterator points to the entity following the last entity of
     * the reversed group. Attempting to dereference the returned iterator
     * results in undefined behavior.
     *
     * @return An iterator to the entity following the last entity of the
     * reversed group.
     */
    [[nodiscard]] reverse_iterator rend() const ENTT_NOEXCEPT {
        return *this ? (std::get<0>(pools)->basic_common_type::rbegin() + *length) : reverse_iterator{};
    }

    /**
     * @brief Returns the first entity of the group, if any.
     * @return The first entity of the group if one exists, the null entity
     * otherwise.
     */
    [[nodiscard]] entity_type front() const {
        const auto it = begin();
        return it != end() ? *it : null;
    }

    /**
     * @brief Returns the last entity of the group, if any.
     * @return The last entity of the group if one exists, the null entity
     * otherwise.
     */
    [[nodiscard]] entity_type back() const {
        const auto it = rbegin();
        return it != rend() ? *it : null;
    }

    /**
     * @brief Finds an entity.
     * @param entt A valid entity identifier.
     * @return An iterator to the given entity if it's found, past the end
     * iterator otherwise.
     */
    [[nodiscard]] iterator find(const entity_type entt) const {
        const auto it = *this ? std::get<0>(pools)->find(entt) : iterator{};
        return it != end() && it >= begin() && *it == entt ? it : end();
    }

    /**
     * @brief Returns the identifier that occupies the given position.
     * @param pos Position of the element to return.
     * @return The identifier that occupies the given position.
     */
    [[nodiscard]] entity_type operator[](const size_type pos) const {
        return begin()[pos];
    }

    /**
     * @brief Checks if a group is properly initialized.
     * @return True if the group is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return length != nullptr;
    }

    /**
     * @brief Checks if a group contains an entity.
     * @param entt A valid entity identifier.
     * @return True if the group contains the given entity, false otherwise.
     */
    [[nodiscard]] bool contains(const entity_type entt) const {
        return *this && std::get<0>(pools)->contains(entt) && (std::get<0>(pools)->index(entt) < (*length));
    }

    /**
     * @brief Returns the components assigned to the given entity.
     *
     * Prefer this function instead of `registry::get` during iterations. It has
     * far better performance than its counterpart.
     *
     * @warning
     * Attempting to use an invalid component type results in a compilation
     * error. Attempting to use an entity that doesn't belong to the group
     * results in undefined behavior.
     *
     * @tparam Component Types of components to get.
     * @param entt A valid entity identifier.
     * @return The components assigned to the entity.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) get(const entity_type entt) const {
        ENTT_ASSERT(contains(entt), "Group does not contain entity");

        if constexpr(sizeof...(Component) == 0) {
            return std::tuple_cat(get_as_tuple(*std::get<storage_type<Owned> *>(pools), entt)..., get_as_tuple(*std::get<storage_type<Get> *>(pools), entt)...);
        } else if constexpr(sizeof...(Component) == 1) {
            return (std::get<storage_type<Component> *>(pools)->get(entt), ...);
        } else {
            return std::tuple_cat(get_as_tuple(*std::get<storage_type<Component> *>(pools), entt)...);
        }
    }

    /**
     * @brief Iterates entities and components and applies the given function
     * object to them.
     *
     * The function object is invoked for each entity. It is provided with the
     * entity itself and a set of references to non-empty components. The
     * _constness_ of the components is as requested.<br/>
     * The signature of the function must be equivalent to one of the following
     * forms:
     *
     * @code{.cpp}
     * void(const entity_type, Type &...);
     * void(Type &...);
     * @endcode
     *
     * @note
     * Empty types aren't explicitly instantiated and therefore they are never
     * returned during iterations.
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void each(Func func) const {
        for(auto args: each()) {
            if constexpr(is_applicable_v<Func, decltype(std::tuple_cat(std::tuple<entity_type>{}, std::declval<basic_group>().get({})))>) {
                std::apply(func, args);
            } else {
                std::apply([&func](auto, auto &&... less) { func(std::forward<decltype(less)>(less)...); }, args);
            }
        }
    }

    /**
     * @brief Returns an iterable object to use to _visit_ the group.
     *
     * The iterable object returns tuples that contain the current entity and a
     * set of references to its non-empty components. The _constness_ of the
     * components is as requested.
     *
     * @note
     * Empty types aren't explicitly instantiated and therefore they are never
     * returned during iterations.
     *
     * @return An iterable object to use to _visit_ the group.
     */
    [[nodiscard]] iterable_group each() const ENTT_NOEXCEPT {
        return iterable_group{pools, length};
    }

    /**
     * @brief Sort a group according to the given comparison function.
     *
     * Sort the group so that iterating it with a couple of iterators returns
     * entities and components in the expected order. See `begin` and `end` for
     * more details.
     *
     * The comparison function object must return `true` if the first element
     * is _less_ than the second one, `false` otherwise. The signature of the
     * comparison function should be equivalent to one of the following:
     *
     * @code{.cpp}
     * bool(std::tuple<Component &...>, std::tuple<Component &...>);
     * bool(const Component &, const Component &);
     * bool(const Entity, const Entity);
     * @endcode
     *
     * Where `Component` are either owned types or not but still such that they
     * are iterated by the group.<br/>
     * Moreover, the comparison function object shall induce a
     * _strict weak ordering_ on the values.
     *
     * The sort function oject must offer a member function template
     * `operator()` that accepts three arguments:
     *
     * * An iterator to the first element of the range to sort.
     * * An iterator past the last element of the range to sort.
     * * A comparison function to use to compare the elements.
     *
     * @tparam Component Optional types of components to compare.
     * @tparam Compare Type of comparison function object.
     * @tparam Sort Type of sort function object.
     * @tparam Args Types of arguments to forward to the sort function object.
     * @param compare A valid comparison function object.
     * @param algo A valid sort function object.
     * @param args Arguments to forward to the sort function object, if any.
     */
    template<typename... Component, typename Compare, typename Sort = std_sort, typename... Args>
    void sort(Compare compare, Sort algo = Sort{}, Args &&... args) const {
        auto *cpool = std::get<0>(pools);

        if constexpr(sizeof...(Component) == 0) {
            static_assert(std::is_invocable_v<Compare, const entity_type, const entity_type>, "Invalid comparison function");
            cpool->sort_n(*length, std::move(compare), std::move(algo), std::forward<Args>(args)...);
        } else if constexpr(sizeof...(Component) == 1) {
            cpool->sort_n(*length, [this, compare = std::move(compare)](const entity_type lhs, const entity_type rhs) {
                return compare((std::get<storage_type<Component> *>(pools)->get(lhs), ...), (std::get<storage_type<Component> *>(pools)->get(rhs), ...));
            }, std::move(algo), std::forward<Args>(args)...);
        } else {
            cpool->sort_n(*length, [this, compare = std::move(compare)](const entity_type lhs, const entity_type rhs) {
                return compare(std::forward_as_tuple(std::get<storage_type<Component> *>(pools)->get(lhs)...), std::forward_as_tuple(std::get<storage_type<Component> *>(pools)->get(rhs)...));
            }, std::move(algo), std::forward<Args>(args)...);
        }

        [this](auto *head, auto *... other) {
            for(auto next = *length; next; --next) {
                const auto pos = next - 1;
                [[maybe_unused]] const auto entt = head->data()[pos];
                (other->swap(other->data()[pos], entt), ...);
            }
        }(std::get<storage_type<Owned> *>(pools)...);
    }

private:
    const std::tuple<storage_type<Owned> *..., storage_type<Get> *...> pools;
    const size_type * const length;
};


}


#endif

// #include "entity/handle.hpp"
#ifndef ENTT_ENTITY_HANDLE_HPP
#define ENTT_ENTITY_HANDLE_HPP


#include <tuple>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/type_traits.hpp"

// #include "fwd.hpp"

// #include "registry.hpp"
#ifndef ENTT_ENTITY_REGISTRY_HPP
#define ENTT_ENTITY_REGISTRY_HPP


#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// #include "../config/config.h"

// #include "../core/algorithm.hpp"

// #include "../core/any.hpp"
#ifndef ENTT_CORE_ANY_HPP
#define ENTT_CORE_ANY_HPP


#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/utility.hpp"
#ifndef ENTT_CORE_UTILITY_HPP
#define ENTT_CORE_UTILITY_HPP


#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/*! @brief Identity function object (waiting for C++20). */
struct identity {
    /**
     * @brief Returns its argument unchanged.
     * @tparam Type Type of the argument.
     * @param value The actual argument.
     * @return The submitted value as-is.
     */
    template<class Type>
    [[nodiscard]] constexpr Type && operator()(Type &&value) const ENTT_NOEXCEPT {
        return std::forward<Type>(value);
    }
};


/**
 * @brief Constant utility to disambiguate overloaded members of a class.
 * @tparam Type Type of the desired overload.
 * @tparam Class Type of class to which the member belongs.
 * @param member A valid pointer to a member.
 * @return Pointer to the member.
 */
template<typename Type, typename Class>
[[nodiscard]] constexpr auto overload(Type Class:: *member) ENTT_NOEXCEPT { return member; }


/**
 * @brief Constant utility to disambiguate overloaded functions.
 * @tparam Func Function type of the desired overload.
 * @param func A valid pointer to a function.
 * @return Pointer to the function.
 */
template<typename Func>
[[nodiscard]] constexpr auto overload(Func *func) ENTT_NOEXCEPT { return func; }


/**
 * @brief Helper type for visitors.
 * @tparam Func Types of function objects.
 */
template<class... Func>
struct overloaded: Func... {
    using Func::operator()...;
};


/**
 * @brief Deduction guide.
 * @tparam Func Types of function objects.
 */
template<class... Func>
overloaded(Func...)
-> overloaded<Func...>;


/**
 * @brief Basic implementation of a y-combinator.
 * @tparam Func Type of a potentially recursive function.
 */
template<class Func>
struct y_combinator {
    /**
     * @brief Constructs a y-combinator from a given function.
     * @param recursive A potentially recursive function.
     */
    y_combinator(Func recursive):
        func{std::move(recursive)}
    {}

    /**
     * @brief Invokes a y-combinator and therefore its underlying function.
     * @tparam Args Types of arguments to use to invoke the underlying function.
     * @param args Parameters to use to invoke the underlying function.
     * @return Return value of the underlying function, if any.
     */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) const {
        return func(*this, std::forward<Args>(args)...);
    }

    /*! @copydoc operator()() */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) {
        return func(*this, std::forward<Args>(args)...);
    }

private:
    Func func;
};


}


#endif

// #include "fwd.hpp"

// #include "type_info.hpp"
#ifndef ENTT_CORE_TYPE_INFO_HPP
#define ENTT_CORE_TYPE_INFO_HPP


#include <string_view>
#include <type_traits>
// #include "../config/config.h"

// #include "../core/attribute.h"
#ifndef ENTT_CORE_ATTRIBUTE_H
#define ENTT_CORE_ATTRIBUTE_H


#ifndef ENTT_EXPORT
#   if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#       define ENTT_EXPORT __declspec(dllexport)
#       define ENTT_IMPORT __declspec(dllimport)
#       define ENTT_HIDDEN
#   elif defined __GNUC__ && __GNUC__ >= 4
#       define ENTT_EXPORT __attribute__((visibility("default")))
#       define ENTT_IMPORT __attribute__((visibility("default")))
#       define ENTT_HIDDEN __attribute__((visibility("hidden")))
#   else /* Unsupported compiler */
#       define ENTT_EXPORT
#       define ENTT_IMPORT
#       define ENTT_HIDDEN
#   endif
#endif


#ifndef ENTT_API
#   if defined ENTT_API_EXPORT
#       define ENTT_API ENTT_EXPORT
#   elif defined ENTT_API_IMPORT
#       define ENTT_API ENTT_IMPORT
#   else /* No API */
#       define ENTT_API
#   endif
#endif


#endif

// #include "hashed_string.hpp"
#ifndef ENTT_CORE_HASHED_STRING_HPP
#define ENTT_CORE_HASHED_STRING_HPP


#include <cstddef>
#include <cstdint>
// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
struct fnv1a_traits;


template<>
struct fnv1a_traits<std::uint32_t> {
    using type = std::uint32_t;
    static constexpr std::uint32_t offset = 2166136261;
    static constexpr std::uint32_t prime = 16777619;
};


template<>
struct fnv1a_traits<std::uint64_t> {
    using type = std::uint64_t;
    static constexpr std::uint64_t offset = 14695981039346656037ull;
    static constexpr std::uint64_t prime = 1099511628211ull;
};


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Zero overhead unique identifier.
 *
 * A hashed string is a compile-time tool that allows users to use
 * human-readable identifers in the codebase while using their numeric
 * counterparts at runtime.<br/>
 * Because of that, a hashed string can also be used in constant expressions if
 * required.
 *
 * @tparam Char Character type.
 */
template<typename Char>
class basic_hashed_string {
    using traits_type = internal::fnv1a_traits<id_type>;

    struct const_wrapper {
        // non-explicit constructor on purpose
        constexpr const_wrapper(const Char *curr) ENTT_NOEXCEPT: str{curr} {}
        const Char *str;
    };

    // Fowler–Noll–Vo hash function v. 1a - the good
    [[nodiscard]] static constexpr id_type helper(const Char *curr) ENTT_NOEXCEPT {
        auto value = traits_type::offset;

        while(*curr != 0) {
            value = (value ^ static_cast<traits_type::type>(*(curr++))) * traits_type::prime;
        }

        return value;
    }

public:
    /*! @brief Character type. */
    using value_type = Char;
    /*! @brief Unsigned integer type. */
    using hash_type = id_type;

    /**
     * @brief Returns directly the numeric representation of a string view.
     * @param str Human-readable identifer.
     * @param size Length of the string to hash.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static constexpr hash_type value(const value_type *str, std::size_t size) ENTT_NOEXCEPT {
        id_type partial{traits_type::offset};
        while(size--) { partial = (partial^(str++)[0])*traits_type::prime; }
        return partial;
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * const auto value = basic_hashed_string<char>::to_value("my.png");
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param str Human-readable identifer.
     * @return The numeric representation of the string.
     */
    template<std::size_t N>
    [[nodiscard]] static constexpr hash_type value(const value_type (&str)[N]) ENTT_NOEXCEPT {
        return helper(str);
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static hash_type value(const_wrapper wrapper) ENTT_NOEXCEPT {
        return helper(wrapper.str);
    }

    /*! @brief Constructs an empty hashed string. */
    constexpr basic_hashed_string() ENTT_NOEXCEPT
        : str{nullptr}, hash{}
    {}

    /**
     * @brief Constructs a hashed string from an array of const characters.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * basic_hashed_string<char> hs{"my.png"};
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param curr Human-readable identifer.
     */
    template<std::size_t N>
    constexpr basic_hashed_string(const value_type (&curr)[N]) ENTT_NOEXCEPT
        : str{curr}, hash{helper(curr)}
    {}

    /**
     * @brief Explicit constructor on purpose to avoid constructing a hashed
     * string directly from a `const value_type *`.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     */
    explicit constexpr basic_hashed_string(const_wrapper wrapper) ENTT_NOEXCEPT
        : str{wrapper.str}, hash{helper(wrapper.str)}
    {}

    /**
     * @brief Returns the human-readable representation of a hashed string.
     * @return The string used to initialize the instance.
     */
    [[nodiscard]] constexpr const value_type * data() const ENTT_NOEXCEPT {
        return str;
    }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr hash_type value() const ENTT_NOEXCEPT {
        return hash;
    }

    /*! @copydoc data */
    [[nodiscard]] constexpr operator const value_type *() const ENTT_NOEXCEPT { return data(); }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr operator hash_type() const ENTT_NOEXCEPT { return value(); }

    /**
     * @brief Compares two hashed strings.
     * @param other Hashed string with which to compare.
     * @return True if the two hashed strings are identical, false otherwise.
     */
    [[nodiscard]] constexpr bool operator==(const basic_hashed_string &other) const ENTT_NOEXCEPT {
        return hash == other.hash;
    }

private:
    const value_type *str;
    hash_type hash;
};


/**
 * @brief Deduction guide.
 *
 * It allows to deduce the character type of the hashed string directly from a
 * human-readable identifer provided to the constructor.
 *
 * @tparam Char Character type.
 * @tparam N Number of characters of the identifier.
 * @param str Human-readable identifer.
 */
template<typename Char, std::size_t N>
basic_hashed_string(const Char (&str)[N])
-> basic_hashed_string<Char>;


/**
 * @brief Compares two hashed strings.
 * @tparam Char Character type.
 * @param lhs A valid hashed string.
 * @param rhs A valid hashed string.
 * @return True if the two hashed strings are identical, false otherwise.
 */
template<typename Char>
[[nodiscard]] constexpr bool operator!=(const basic_hashed_string<Char> &lhs, const basic_hashed_string<Char> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/*! @brief Aliases for common character types. */
using hashed_string = basic_hashed_string<char>;


/*! @brief Aliases for common character types. */
using hashed_wstring = basic_hashed_string<wchar_t>;


inline namespace literals {


/**
 * @brief User defined literal for hashed strings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed string.
 */
[[nodiscard]] constexpr entt::hashed_string operator"" _hs(const char *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_string{str};
}


/**
 * @brief User defined literal for hashed wstrings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed wstring.
 */
[[nodiscard]] constexpr entt::hashed_wstring operator"" _hws(const wchar_t *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_wstring{str};
}


}


}


#endif

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


struct ENTT_API type_seq final {
    [[nodiscard]] static id_type next() ENTT_NOEXCEPT {
        static ENTT_MAYBE_ATOMIC(id_type) value{};
        return value++;
    }
};


template<typename Type>
[[nodiscard]] constexpr auto stripped_type_name() ENTT_NOEXCEPT {
#if defined ENTT_PRETTY_FUNCTION
    std::string_view pretty_function{ENTT_PRETTY_FUNCTION};
    auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(ENTT_PRETTY_FUNCTION_PREFIX)+1);
    auto value = pretty_function.substr(first, pretty_function.find_last_of(ENTT_PRETTY_FUNCTION_SUFFIX) - first);
    return value;
#else
    return std::string_view{""};
#endif
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr std::string_view type_name(int) ENTT_NOEXCEPT {
    constexpr auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type>
[[nodiscard]] static std::string_view type_name(char) ENTT_NOEXCEPT {
    static const auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr id_type type_hash(int) ENTT_NOEXCEPT {
    constexpr auto stripped = stripped_type_name<Type>();
    constexpr auto value = hashed_string::value(stripped.data(), stripped.size());
    return value;
}


template<typename Type>
[[nodiscard]] static id_type type_hash(char) ENTT_NOEXCEPT {
    static const auto value = [](const auto stripped) {
        return hashed_string::value(stripped.data(), stripped.size());
    }(stripped_type_name<Type>());
    return value;
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Type sequential identifier.
 * @tparam Type Type for which to generate a sequential identifier.
 */
template<typename Type, typename = void>
struct ENTT_API type_seq final {
    /**
     * @brief Returns the sequential identifier of a given type.
     * @return The sequential identifier of a given type.
     */
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const id_type value = internal::type_seq::next();
        return value;
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type hash.
 * @tparam Type Type for which to generate a hash value.
 */
template<typename Type, typename = void>
struct type_hash final {
    /**
     * @brief Returns the numeric representation of a given type.
     * @return The numeric representation of the given type.
     */
#if defined ENTT_PRETTY_FUNCTION
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return internal::type_hash<Type>(0);
#else
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return type_seq<Type>::value();
#endif
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type name.
 * @tparam Type Type for which to generate a name.
 */
template<typename Type, typename = void>
struct type_name final {
    /**
     * @brief Returns the name of a given type.
     * @return The name of the given type.
     */
    [[nodiscard]] static constexpr std::string_view value() ENTT_NOEXCEPT {
        return internal::type_name<Type>(0);
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator std::string_view() const ENTT_NOEXCEPT { return value(); }
};


/*! @brief Implementation specific information about a type. */
class type_info final {
    template<typename>
    friend type_info type_id() ENTT_NOEXCEPT;

    type_info(id_type seq_v, id_type hash_v, std::string_view name_v) ENTT_NOEXCEPT
        : seq_value{seq_v},
          hash_value{hash_v},
          name_value{name_v}
    {}

public:
    /*! @brief Default constructor. */
    type_info() ENTT_NOEXCEPT
        : type_info({}, {}, {})
    {}

    /*! @brief Default copy constructor. */
    type_info(const type_info &) ENTT_NOEXCEPT = default;
    /*! @brief Default move constructor. */
    type_info(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Default copy assignment operator.
     * @return This type info object.
     */
    type_info & operator=(const type_info &) ENTT_NOEXCEPT = default;

    /**
     * @brief Default move assignment operator.
     * @return This type info object.
     */
    type_info & operator=(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Checks if a type info object is properly initialized.
     * @return True if the object is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return name_value.data() != nullptr;
    }

    /**
     * @brief Type sequential identifier.
     * @return Type sequential identifier.
     */
    [[nodiscard]] id_type seq() const ENTT_NOEXCEPT {
        return seq_value;
    }

    /**
     * @brief Type hash.
     * @return Type hash.
     */
    [[nodiscard]] id_type hash() const ENTT_NOEXCEPT {
        return hash_value;
    }

    /**
     * @brief Type name.
     * @return Type name.
     */
    [[nodiscard]] std::string_view name() const ENTT_NOEXCEPT {
        return name_value;
    }

    /**
     * @brief Compares the contents of two type info objects.
     * @param other Object with which to compare.
     * @return False if the two contents differ, true otherwise.
     */
    [[nodiscard]] bool operator==(const type_info &other) const ENTT_NOEXCEPT {
        return hash_value == other.hash_value;
    }

private:
    id_type seq_value;
    id_type hash_value;
    std::string_view name_value;
};


/**
 * @brief Compares the contents of two type info objects.
 * @param lhs A type info object.
 * @param rhs A type info object.
 * @return True if the two contents differ, false otherwise.
 */
[[nodiscard]] inline bool operator!=(const type_info &lhs, const type_info &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Returns the type info object for a given type.
 * @tparam Type Type for which to generate a type info object.
 * @return The type info object for the given type.
 */
template<typename Type>
[[nodiscard]] type_info type_id() ENTT_NOEXCEPT {
    return type_info{
        type_seq<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_name<std::remove_cv_t<std::remove_reference_t<Type>>>::value()
    };
}


}


#endif

// #include "type_traits.hpp"



namespace entt {


/**
 * @brief A SBO friendly, type-safe container for single values of any type.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 */
template<std::size_t Len, std::size_t Align>
class basic_any {
    enum class operation: std::uint8_t { COPY, MOVE, DTOR, COMP, ADDR, CADDR, TYPE };
    enum class policy: std::uint8_t { OWNER, REF, CREF };

    using storage_type = std::aligned_storage_t<Len + !Len, Align>;
    using vtable_type = const void *(const operation, const basic_any &, void *);

    template<typename Type>
    static constexpr bool in_situ = Len && alignof(Type) <= alignof(storage_type) && sizeof(Type) <= sizeof(storage_type) && std::is_nothrow_move_constructible_v<Type>;

    template<typename Type>
    [[nodiscard]] static constexpr policy type_to_policy() {
        if constexpr(std::is_lvalue_reference_v<Type>) {
            if constexpr(std::is_const_v<std::remove_reference_t<Type>>) {
                return policy::CREF;
            } else {
                return policy::REF;
            }
        } else {
            return policy::OWNER;
        }
    }

    template<typename Type>
    [[nodiscard]] static bool compare(const void *lhs, const void *rhs) {
        if constexpr(!std::is_function_v<Type> && is_equality_comparable_v<Type>) {
            return *static_cast<const Type *>(lhs) == *static_cast<const Type *>(rhs);
        } else {
            return lhs == rhs;
        }
    }

    template<typename Type>
    static const void * basic_vtable([[maybe_unused]] const operation op, [[maybe_unused]] const basic_any &from, [[maybe_unused]] void *to) {
        static_assert(std::is_same_v<std::remove_reference_t<std::remove_const_t<Type>>, Type>, "Invalid type");

        if constexpr(!std::is_void_v<Type>) {
            const Type *instance = (in_situ<Type> && from.mode == policy::OWNER)
                ? ENTT_LAUNDER(reinterpret_cast<const Type *>(&from.storage))
                : static_cast<const Type *>(from.instance);

            switch(op) {
            case operation::COPY:
                if constexpr(std::is_copy_constructible_v<Type>) {
                    static_cast<basic_any *>(to)->emplace<Type>(*instance);
                }
                break;
            case operation::MOVE:
                if constexpr(in_situ<Type>) {
                    if(from.mode == policy::OWNER) {
                        return new (&static_cast<basic_any *>(to)->storage) Type{std::move(*const_cast<Type *>(instance))};
                    }
                }

                return (static_cast<basic_any *>(to)->instance = std::exchange(const_cast<basic_any &>(from).instance, nullptr));
            case operation::DTOR:
                if(from.mode == policy::OWNER) {
                    if constexpr(in_situ<Type>) {
                        instance->~Type();
                    } else if constexpr(std::is_array_v<Type>) {
                        delete[] instance;
                    } else {
                        delete instance;
                    }
                }
                break;
            case operation::COMP:
                return compare<Type>(instance, (*static_cast<const basic_any **>(to))->data()) ? to : nullptr;
            case operation::ADDR:
                if(from.mode == policy::CREF) {
                    return nullptr;
                }
                [[fallthrough]];
            case operation::CADDR:
                return instance;
            case operation::TYPE:
                *static_cast<type_info *>(to) = type_id<Type>();
                break;
            }
        }

        return nullptr;
    }

    template<typename Type, typename... Args>
    void initialize([[maybe_unused]] Args &&... args) {
        if constexpr(!std::is_void_v<Type>) {
            if constexpr(std::is_lvalue_reference_v<Type>) {
                static_assert(sizeof...(Args) == 1u && (std::is_lvalue_reference_v<Args> && ...), "Invalid arguments");
                instance = (std::addressof(args), ...);
            } else if constexpr(in_situ<Type>) {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    new (&storage) Type{std::forward<Args>(args)...};
                } else {
                    new (&storage) Type(std::forward<Args>(args)...);
                }
            } else {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    instance = new Type{std::forward<Args>(args)...};
                } else {
                    instance = new Type(std::forward<Args>(args)...);
                }
            }
        }
    }

    basic_any(const basic_any &other, const policy pol) ENTT_NOEXCEPT
        : instance{other.data()},
          vtable{other.vtable},
          mode{pol}
    {}

public:
    /*! @brief Size of the internal storage. */
    static constexpr auto length = Len;
    /*! @brief Alignment requirement. */
    static constexpr auto alignment = Align;

    /*! @brief Default constructor. */
    basic_any() ENTT_NOEXCEPT
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {}

    /**
     * @brief Constructs a wrapper by directly initializing the new object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    explicit basic_any(std::in_place_type_t<Type>, Args &&... args)
        : instance{},
          vtable{&basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>},
          mode{type_to_policy<Type>()}
    {
        initialize<Type>(std::forward<Args>(args)...);
    }

    /**
     * @brief Constructs a wrapper that holds an unmanaged object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type>
    basic_any(std::reference_wrapper<Type> value) ENTT_NOEXCEPT
        : basic_any{}
    {
        // invokes deprecated assignment operator (and avoids issues with vs2017)
        *this = value;
    }

    /**
     * @brief Constructs a wrapper from a given value.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>>>
    basic_any(Type &&value)
        : instance{},
          vtable{&basic_vtable<std::decay_t<Type>>},
          mode{policy::OWNER}
    {
        initialize<std::decay_t<Type>>(std::forward<Type>(value));
    }

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    basic_any(const basic_any &other)
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {
        other.vtable(operation::COPY, other, this);
    }

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    basic_any(basic_any &&other) ENTT_NOEXCEPT
        : instance{},
          vtable{other.vtable},
          mode{other.mode}
    {
        vtable(operation::MOVE, other, this);
    }

    /*! @brief Frees the internal storage, whatever it means. */
    ~basic_any() {
        vtable(operation::DTOR, *this, nullptr);
    }

    /**
     * @brief Copy assignment operator.
     * @param other The instance to copy from.
     * @return This any object.
     */
    basic_any & operator=(const basic_any &other) {
        reset();
        other.vtable(operation::COPY, other, this);
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This any object.
     */
    basic_any & operator=(basic_any &&other) ENTT_NOEXCEPT {
        std::exchange(vtable, other.vtable)(operation::DTOR, *this, nullptr);
        other.vtable(operation::MOVE, other, this);
        mode = other.mode;
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    [[deprecated("Use std::in_place_type<T &>, entt::make_any<T &>, emplace<Type &> or forward_as_any instead")]]
    basic_any & operator=(std::reference_wrapper<Type> value) ENTT_NOEXCEPT {
        emplace<Type &>(value.get());
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>, basic_any &>
    operator=(Type &&value) {
        emplace<std::decay_t<Type>>(std::forward<Type>(value));
        return *this;
    }

    /**
     * @brief Returns the type of the contained object.
     * @return The type of the contained object, if any.
     */
    [[nodiscard]] type_info type() const ENTT_NOEXCEPT {
        type_info info{};
        vtable(operation::TYPE, *this, &info);
        return info;
    }

    /**
     * @brief Returns an opaque pointer to the contained instance.
     * @return An opaque pointer the contained instance, if any.
     */
    [[nodiscard]] const void * data() const ENTT_NOEXCEPT {
        return vtable(operation::CADDR, *this, nullptr);
    }

    /*! @copydoc data */
    [[nodiscard]] void * data() ENTT_NOEXCEPT {
        return const_cast<void *>(vtable(operation::ADDR, *this, nullptr));
    }

    /**
     * @brief Replaces the contained object by creating a new instance directly.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    void emplace(Args &&... args) {
        std::exchange(vtable, &basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>)(operation::DTOR, *this, nullptr);
        mode = type_to_policy<Type>();
        initialize<Type>(std::forward<Args>(args)...);
    }

    /*! @brief Destroys contained object */
    void reset() {
        std::exchange(vtable, &basic_vtable<void>)(operation::DTOR, *this, nullptr);
        mode = policy::OWNER;
    }

    /**
     * @brief Returns false if a wrapper is empty, true otherwise.
     * @return False if the wrapper is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(vtable(operation::CADDR, *this, nullptr) == nullptr);
    }

    /**
     * @brief Checks if two wrappers differ in their content.
     * @param other Wrapper with which to compare.
     * @return False if the two objects differ in their content, true otherwise.
     */
    bool operator==(const basic_any &other) const ENTT_NOEXCEPT {
        const basic_any *trampoline = &other;
        return type() == other.type() && (vtable(operation::COMP, *this, &trampoline) || !other.data());
    }

    /**
     * @brief Aliasing constructor.
     * @return A wrapper that shares a reference to an unmanaged object.
     */
    [[nodiscard]] basic_any as_ref() ENTT_NOEXCEPT {
        return basic_any{*this, (mode == policy::CREF ? policy::CREF : policy::REF)};
    }

    /*! @copydoc as_ref */
    [[nodiscard]] basic_any as_ref() const ENTT_NOEXCEPT {
        return basic_any{*this, policy::CREF};
    }

    /**
     * @brief Returns true if a wrapper owns its object, false otherwise.
     * @return True if the wrapper owns its object, false otherwise.
     */
    [[nodiscard]] bool owner() const ENTT_NOEXCEPT {
        return (mode == policy::OWNER);
    }

private:
    union { const void *instance; storage_type storage; };
    vtable_type *vtable;
    policy mode;
};


/**
 * @brief Checks if two wrappers differ in their content.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param lhs A wrapper, either empty or not.
 * @param rhs A wrapper, either empty or not.
 * @return True if the two wrappers differ in their content, false otherwise.
 */
template<std::size_t Len, std::size_t Align>
[[nodiscard]] inline bool operator!=(const basic_any<Len, Align> &lhs, const basic_any<Len, Align> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Performs type-safe access to the contained object.
 * @tparam Type Type to which conversion is required.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param data Target any object.
 * @return The element converted to the requested type.
 */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(const basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    const auto * const instance = any_cast<std::remove_reference_t<Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &&data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(std::move(*instance));
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
const Type * any_cast(const basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    return (data->type() == type_id<Type>() ? static_cast<const Type *>(data->data()) : nullptr);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type * any_cast(basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    // last attempt to make wrappers for const references return their values
    return (data->type() == type_id<Type>() ? static_cast<Type *>(static_cast<constness_as_t<basic_any<Len, Align>, Type> *>(data)->data()) : nullptr);
}


/**
 * @brief Constructs a wrapper from a given type, passing it all arguments.
 * @tparam Type Type of object to use to initialize the wrapper.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Args Types of arguments to use to construct the new instance.
 * @param args Parameters to use to construct the instance.
 * @return A properly initialized wrapper for an object of the given type.
 */
template<typename Type, std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename... Args>
basic_any<Len, Align> make_any(Args &&... args) {
    return basic_any<Len, Align>{std::in_place_type<Type>, std::forward<Args>(args)...};
}


/**
 * @brief Forwards its argument and avoids copies for lvalue references.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Type Type of argument to use to construct the new instance.
 * @param value Parameter to use to construct the instance.
 * @return A properly initialized and not necessarily owning wrapper.
 */
template<std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename Type>
basic_any<Len, Align> forward_as_any(Type &&value) {
    return basic_any<Len, Align>{std::in_place_type<std::conditional_t<std::is_rvalue_reference_v<Type>, std::decay_t<Type>, Type>>, std::forward<Type>(value)};
}


}


#endif

// #include "../core/fwd.hpp"

// #include "../core/type_info.hpp"

// #include "../core/type_traits.hpp"

// #include "component.hpp"

// #include "entity.hpp"

// #include "fwd.hpp"

// #include "group.hpp"

// #include "poly_storage.hpp"
#ifndef ENTT_ENTITY_POLY_STORAGE_HPP
#define ENTT_ENTITY_POLY_STORAGE_HPP


#include <cstddef>
#include <tuple>
// #include "../core/type_info.hpp"

// #include "../core/type_traits.hpp"

// #include "../poly/poly.hpp"
#ifndef ENTT_POLY_POLY_HPP
#define ENTT_POLY_POLY_HPP


#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "../core/any.hpp"
#ifndef ENTT_CORE_ANY_HPP
#define ENTT_CORE_ANY_HPP


#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "../core/utility.hpp"
#ifndef ENTT_CORE_UTILITY_HPP
#define ENTT_CORE_UTILITY_HPP


#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/*! @brief Identity function object (waiting for C++20). */
struct identity {
    /**
     * @brief Returns its argument unchanged.
     * @tparam Type Type of the argument.
     * @param value The actual argument.
     * @return The submitted value as-is.
     */
    template<class Type>
    [[nodiscard]] constexpr Type && operator()(Type &&value) const ENTT_NOEXCEPT {
        return std::forward<Type>(value);
    }
};


/**
 * @brief Constant utility to disambiguate overloaded members of a class.
 * @tparam Type Type of the desired overload.
 * @tparam Class Type of class to which the member belongs.
 * @param member A valid pointer to a member.
 * @return Pointer to the member.
 */
template<typename Type, typename Class>
[[nodiscard]] constexpr auto overload(Type Class:: *member) ENTT_NOEXCEPT { return member; }


/**
 * @brief Constant utility to disambiguate overloaded functions.
 * @tparam Func Function type of the desired overload.
 * @param func A valid pointer to a function.
 * @return Pointer to the function.
 */
template<typename Func>
[[nodiscard]] constexpr auto overload(Func *func) ENTT_NOEXCEPT { return func; }


/**
 * @brief Helper type for visitors.
 * @tparam Func Types of function objects.
 */
template<class... Func>
struct overloaded: Func... {
    using Func::operator()...;
};


/**
 * @brief Deduction guide.
 * @tparam Func Types of function objects.
 */
template<class... Func>
overloaded(Func...)
-> overloaded<Func...>;


/**
 * @brief Basic implementation of a y-combinator.
 * @tparam Func Type of a potentially recursive function.
 */
template<class Func>
struct y_combinator {
    /**
     * @brief Constructs a y-combinator from a given function.
     * @param recursive A potentially recursive function.
     */
    y_combinator(Func recursive):
        func{std::move(recursive)}
    {}

    /**
     * @brief Invokes a y-combinator and therefore its underlying function.
     * @tparam Args Types of arguments to use to invoke the underlying function.
     * @param args Parameters to use to invoke the underlying function.
     * @return Return value of the underlying function, if any.
     */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) const {
        return func(*this, std::forward<Args>(args)...);
    }

    /*! @copydoc operator()() */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) {
        return func(*this, std::forward<Args>(args)...);
    }

private:
    Func func;
};


}


#endif

// #include "fwd.hpp"
#ifndef ENTT_CORE_FWD_HPP
#define ENTT_CORE_FWD_HPP


#include <type_traits>
// #include "../config/config.h"



namespace entt {


template<std::size_t Len = sizeof(double[2]), std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_any;


/*! @brief Alias declaration for type identifiers. */
using id_type = ENTT_ID_TYPE;


/*! @brief Alias declaration for the most common use case. */
using any = basic_any<>;


}


#endif

// #include "type_info.hpp"
#ifndef ENTT_CORE_TYPE_INFO_HPP
#define ENTT_CORE_TYPE_INFO_HPP


#include <string_view>
#include <type_traits>
// #include "../config/config.h"

// #include "../core/attribute.h"
#ifndef ENTT_CORE_ATTRIBUTE_H
#define ENTT_CORE_ATTRIBUTE_H


#ifndef ENTT_EXPORT
#   if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#       define ENTT_EXPORT __declspec(dllexport)
#       define ENTT_IMPORT __declspec(dllimport)
#       define ENTT_HIDDEN
#   elif defined __GNUC__ && __GNUC__ >= 4
#       define ENTT_EXPORT __attribute__((visibility("default")))
#       define ENTT_IMPORT __attribute__((visibility("default")))
#       define ENTT_HIDDEN __attribute__((visibility("hidden")))
#   else /* Unsupported compiler */
#       define ENTT_EXPORT
#       define ENTT_IMPORT
#       define ENTT_HIDDEN
#   endif
#endif


#ifndef ENTT_API
#   if defined ENTT_API_EXPORT
#       define ENTT_API ENTT_EXPORT
#   elif defined ENTT_API_IMPORT
#       define ENTT_API ENTT_IMPORT
#   else /* No API */
#       define ENTT_API
#   endif
#endif


#endif

// #include "hashed_string.hpp"
#ifndef ENTT_CORE_HASHED_STRING_HPP
#define ENTT_CORE_HASHED_STRING_HPP


#include <cstddef>
#include <cstdint>
// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
struct fnv1a_traits;


template<>
struct fnv1a_traits<std::uint32_t> {
    using type = std::uint32_t;
    static constexpr std::uint32_t offset = 2166136261;
    static constexpr std::uint32_t prime = 16777619;
};


template<>
struct fnv1a_traits<std::uint64_t> {
    using type = std::uint64_t;
    static constexpr std::uint64_t offset = 14695981039346656037ull;
    static constexpr std::uint64_t prime = 1099511628211ull;
};


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Zero overhead unique identifier.
 *
 * A hashed string is a compile-time tool that allows users to use
 * human-readable identifers in the codebase while using their numeric
 * counterparts at runtime.<br/>
 * Because of that, a hashed string can also be used in constant expressions if
 * required.
 *
 * @tparam Char Character type.
 */
template<typename Char>
class basic_hashed_string {
    using traits_type = internal::fnv1a_traits<id_type>;

    struct const_wrapper {
        // non-explicit constructor on purpose
        constexpr const_wrapper(const Char *curr) ENTT_NOEXCEPT: str{curr} {}
        const Char *str;
    };

    // Fowler–Noll–Vo hash function v. 1a - the good
    [[nodiscard]] static constexpr id_type helper(const Char *curr) ENTT_NOEXCEPT {
        auto value = traits_type::offset;

        while(*curr != 0) {
            value = (value ^ static_cast<traits_type::type>(*(curr++))) * traits_type::prime;
        }

        return value;
    }

public:
    /*! @brief Character type. */
    using value_type = Char;
    /*! @brief Unsigned integer type. */
    using hash_type = id_type;

    /**
     * @brief Returns directly the numeric representation of a string view.
     * @param str Human-readable identifer.
     * @param size Length of the string to hash.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static constexpr hash_type value(const value_type *str, std::size_t size) ENTT_NOEXCEPT {
        id_type partial{traits_type::offset};
        while(size--) { partial = (partial^(str++)[0])*traits_type::prime; }
        return partial;
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * const auto value = basic_hashed_string<char>::to_value("my.png");
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param str Human-readable identifer.
     * @return The numeric representation of the string.
     */
    template<std::size_t N>
    [[nodiscard]] static constexpr hash_type value(const value_type (&str)[N]) ENTT_NOEXCEPT {
        return helper(str);
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static hash_type value(const_wrapper wrapper) ENTT_NOEXCEPT {
        return helper(wrapper.str);
    }

    /*! @brief Constructs an empty hashed string. */
    constexpr basic_hashed_string() ENTT_NOEXCEPT
        : str{nullptr}, hash{}
    {}

    /**
     * @brief Constructs a hashed string from an array of const characters.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * basic_hashed_string<char> hs{"my.png"};
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param curr Human-readable identifer.
     */
    template<std::size_t N>
    constexpr basic_hashed_string(const value_type (&curr)[N]) ENTT_NOEXCEPT
        : str{curr}, hash{helper(curr)}
    {}

    /**
     * @brief Explicit constructor on purpose to avoid constructing a hashed
     * string directly from a `const value_type *`.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     */
    explicit constexpr basic_hashed_string(const_wrapper wrapper) ENTT_NOEXCEPT
        : str{wrapper.str}, hash{helper(wrapper.str)}
    {}

    /**
     * @brief Returns the human-readable representation of a hashed string.
     * @return The string used to initialize the instance.
     */
    [[nodiscard]] constexpr const value_type * data() const ENTT_NOEXCEPT {
        return str;
    }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr hash_type value() const ENTT_NOEXCEPT {
        return hash;
    }

    /*! @copydoc data */
    [[nodiscard]] constexpr operator const value_type *() const ENTT_NOEXCEPT { return data(); }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr operator hash_type() const ENTT_NOEXCEPT { return value(); }

    /**
     * @brief Compares two hashed strings.
     * @param other Hashed string with which to compare.
     * @return True if the two hashed strings are identical, false otherwise.
     */
    [[nodiscard]] constexpr bool operator==(const basic_hashed_string &other) const ENTT_NOEXCEPT {
        return hash == other.hash;
    }

private:
    const value_type *str;
    hash_type hash;
};


/**
 * @brief Deduction guide.
 *
 * It allows to deduce the character type of the hashed string directly from a
 * human-readable identifer provided to the constructor.
 *
 * @tparam Char Character type.
 * @tparam N Number of characters of the identifier.
 * @param str Human-readable identifer.
 */
template<typename Char, std::size_t N>
basic_hashed_string(const Char (&str)[N])
-> basic_hashed_string<Char>;


/**
 * @brief Compares two hashed strings.
 * @tparam Char Character type.
 * @param lhs A valid hashed string.
 * @param rhs A valid hashed string.
 * @return True if the two hashed strings are identical, false otherwise.
 */
template<typename Char>
[[nodiscard]] constexpr bool operator!=(const basic_hashed_string<Char> &lhs, const basic_hashed_string<Char> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/*! @brief Aliases for common character types. */
using hashed_string = basic_hashed_string<char>;


/*! @brief Aliases for common character types. */
using hashed_wstring = basic_hashed_string<wchar_t>;


inline namespace literals {


/**
 * @brief User defined literal for hashed strings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed string.
 */
[[nodiscard]] constexpr entt::hashed_string operator"" _hs(const char *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_string{str};
}


/**
 * @brief User defined literal for hashed wstrings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed wstring.
 */
[[nodiscard]] constexpr entt::hashed_wstring operator"" _hws(const wchar_t *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_wstring{str};
}


}


}


#endif

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


struct ENTT_API type_seq final {
    [[nodiscard]] static id_type next() ENTT_NOEXCEPT {
        static ENTT_MAYBE_ATOMIC(id_type) value{};
        return value++;
    }
};


template<typename Type>
[[nodiscard]] constexpr auto stripped_type_name() ENTT_NOEXCEPT {
#if defined ENTT_PRETTY_FUNCTION
    std::string_view pretty_function{ENTT_PRETTY_FUNCTION};
    auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(ENTT_PRETTY_FUNCTION_PREFIX)+1);
    auto value = pretty_function.substr(first, pretty_function.find_last_of(ENTT_PRETTY_FUNCTION_SUFFIX) - first);
    return value;
#else
    return std::string_view{""};
#endif
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr std::string_view type_name(int) ENTT_NOEXCEPT {
    constexpr auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type>
[[nodiscard]] static std::string_view type_name(char) ENTT_NOEXCEPT {
    static const auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr id_type type_hash(int) ENTT_NOEXCEPT {
    constexpr auto stripped = stripped_type_name<Type>();
    constexpr auto value = hashed_string::value(stripped.data(), stripped.size());
    return value;
}


template<typename Type>
[[nodiscard]] static id_type type_hash(char) ENTT_NOEXCEPT {
    static const auto value = [](const auto stripped) {
        return hashed_string::value(stripped.data(), stripped.size());
    }(stripped_type_name<Type>());
    return value;
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Type sequential identifier.
 * @tparam Type Type for which to generate a sequential identifier.
 */
template<typename Type, typename = void>
struct ENTT_API type_seq final {
    /**
     * @brief Returns the sequential identifier of a given type.
     * @return The sequential identifier of a given type.
     */
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const id_type value = internal::type_seq::next();
        return value;
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type hash.
 * @tparam Type Type for which to generate a hash value.
 */
template<typename Type, typename = void>
struct type_hash final {
    /**
     * @brief Returns the numeric representation of a given type.
     * @return The numeric representation of the given type.
     */
#if defined ENTT_PRETTY_FUNCTION
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return internal::type_hash<Type>(0);
#else
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return type_seq<Type>::value();
#endif
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type name.
 * @tparam Type Type for which to generate a name.
 */
template<typename Type, typename = void>
struct type_name final {
    /**
     * @brief Returns the name of a given type.
     * @return The name of the given type.
     */
    [[nodiscard]] static constexpr std::string_view value() ENTT_NOEXCEPT {
        return internal::type_name<Type>(0);
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator std::string_view() const ENTT_NOEXCEPT { return value(); }
};


/*! @brief Implementation specific information about a type. */
class type_info final {
    template<typename>
    friend type_info type_id() ENTT_NOEXCEPT;

    type_info(id_type seq_v, id_type hash_v, std::string_view name_v) ENTT_NOEXCEPT
        : seq_value{seq_v},
          hash_value{hash_v},
          name_value{name_v}
    {}

public:
    /*! @brief Default constructor. */
    type_info() ENTT_NOEXCEPT
        : type_info({}, {}, {})
    {}

    /*! @brief Default copy constructor. */
    type_info(const type_info &) ENTT_NOEXCEPT = default;
    /*! @brief Default move constructor. */
    type_info(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Default copy assignment operator.
     * @return This type info object.
     */
    type_info & operator=(const type_info &) ENTT_NOEXCEPT = default;

    /**
     * @brief Default move assignment operator.
     * @return This type info object.
     */
    type_info & operator=(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Checks if a type info object is properly initialized.
     * @return True if the object is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return name_value.data() != nullptr;
    }

    /**
     * @brief Type sequential identifier.
     * @return Type sequential identifier.
     */
    [[nodiscard]] id_type seq() const ENTT_NOEXCEPT {
        return seq_value;
    }

    /**
     * @brief Type hash.
     * @return Type hash.
     */
    [[nodiscard]] id_type hash() const ENTT_NOEXCEPT {
        return hash_value;
    }

    /**
     * @brief Type name.
     * @return Type name.
     */
    [[nodiscard]] std::string_view name() const ENTT_NOEXCEPT {
        return name_value;
    }

    /**
     * @brief Compares the contents of two type info objects.
     * @param other Object with which to compare.
     * @return False if the two contents differ, true otherwise.
     */
    [[nodiscard]] bool operator==(const type_info &other) const ENTT_NOEXCEPT {
        return hash_value == other.hash_value;
    }

private:
    id_type seq_value;
    id_type hash_value;
    std::string_view name_value;
};


/**
 * @brief Compares the contents of two type info objects.
 * @param lhs A type info object.
 * @param rhs A type info object.
 * @return True if the two contents differ, false otherwise.
 */
[[nodiscard]] inline bool operator!=(const type_info &lhs, const type_info &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Returns the type info object for a given type.
 * @tparam Type Type for which to generate a type info object.
 * @return The type info object for the given type.
 */
template<typename Type>
[[nodiscard]] type_info type_id() ENTT_NOEXCEPT {
    return type_info{
        type_seq<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_name<std::remove_cv_t<std::remove_reference_t<Type>>>::value()
    };
}


}


#endif

// #include "type_traits.hpp"
#ifndef ENTT_CORE_TYPE_TRAITS_HPP
#define ENTT_CORE_TYPE_TRAITS_HPP


#include <cstddef>
#include <utility>
#include <type_traits>
// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @brief Utility class to disambiguate overloaded functions.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
struct choice_t
    // Unfortunately, doxygen cannot parse such a construct.
    /*! @cond TURN_OFF_DOXYGEN */
    : choice_t<N-1>
    /*! @endcond */
{};


/*! @copybrief choice_t */
template<>
struct choice_t<0> {};


/**
 * @brief Variable template for the choice trick.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
inline constexpr choice_t<N> choice{};


/**
 * @brief Identity type trait.
 *
 * Useful to establish non-deduced contexts in template argument deduction
 * (waiting for C++20) or to provide types through function arguments.
 *
 * @tparam Type A type.
 */
template<typename Type>
struct type_identity {
    /*! @brief Identity type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Type A type.
 */
template<typename Type>
using type_identity_t = typename type_identity<Type>::type;


/**
 * @brief A type-only `sizeof` wrapper that returns 0 where `sizeof` complains.
 * @tparam Type The type of which to return the size.
 * @tparam The size of the type if `sizeof` accepts it, 0 otherwise.
 */
template<typename Type, typename = void>
struct size_of: std::integral_constant<std::size_t, 0u> {};


/*! @copydoc size_of */
template<typename Type>
struct size_of<Type, std::void_t<decltype(sizeof(Type))>>
    : std::integral_constant<std::size_t, sizeof(Type)>
{};


/**
 * @brief Helper variable template.
 * @tparam Type The type of which to return the size.
 */
template<class Type>
inline constexpr auto size_of_v = size_of<Type>::value;


/**
 * @brief Using declaration to be used to _repeat_ the same type a number of
 * times equal to the size of a given parameter pack.
 * @tparam Type A type to repeat.
 */
template<typename Type, typename>
using unpack_as_t = Type;


/**
 * @brief Helper variable template to be used to _repeat_ the same value a
 * number of times equal to the size of a given parameter pack.
 * @tparam Value A value to repeat.
 */
template<auto Value, typename>
inline constexpr auto unpack_as_v = Value;


/**
 * @brief Wraps a static constant.
 * @tparam Value A static constant.
 */
template<auto Value>
using integral_constant = std::integral_constant<decltype(Value), Value>;


/**
 * @brief Alias template to facilitate the creation of named values.
 * @tparam Value A constant value at least convertible to `id_type`.
 */
template<id_type Value>
using tag = integral_constant<Value>;


/**
 * @brief A class to use to push around lists of types, nothing more.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list {
    /*! @brief Type list type. */
    using type = type_list;
    /*! @brief Compile-time number of elements in the type list. */
    static constexpr auto size = sizeof...(Type);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct type_list_element;


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Index Index of the type to return.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<std::size_t Index, typename Type, typename... Other>
struct type_list_element<Index, type_list<Type, Other...>>
    : type_list_element<Index - 1u, type_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<typename Type, typename... Other>
struct type_list_element<0u, type_list<Type, Other...>> {
    /*! @brief Searched type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the type to return.
 * @tparam List Type list to search into.
 */
template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @return A type list composed by the types of both the type lists.
 */
template<typename... Type, typename... Other>
constexpr type_list<Type..., Other...> operator+(type_list<Type...>, type_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_cat;


/*! @brief Concatenates multiple type lists. */
template<>
struct type_list_cat<> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<>;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @tparam List Other type lists, if any.
 */
template<typename... Type, typename... Other, typename... List>
struct type_list_cat<type_list<Type...>, type_list<Other...>, List...> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = typename type_list_cat<type_list<Type..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list_cat<type_list<Type...>> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<Type...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists to concatenate.
 */
template<typename... List>
using type_list_cat_t = typename type_list_cat<List...>::type;


/*! @brief Primary template isn't defined on purpose. */
template<typename>
struct type_list_unique;


/**
 * @brief Removes duplicates types from a type list.
 * @tparam Type One of the types provided by the given type list.
 * @tparam Other The other types provided by the given type list.
 */
template<typename Type, typename... Other>
struct type_list_unique<type_list<Type, Other...>> {
    /*! @brief A type list without duplicate types. */
    using type = std::conditional_t<
        std::disjunction_v<std::is_same<Type, Other>...>,
        typename type_list_unique<type_list<Other...>>::type,
        type_list_cat_t<type_list<Type>, typename type_list_unique<type_list<Other...>>::type>
    >;
};


/*! @brief Removes duplicates types from a type list. */
template<>
struct type_list_unique<type_list<>> {
    /*! @brief A type list without duplicate types. */
    using type = type_list<>;
};


/**
 * @brief Helper type.
 * @tparam Type A type list.
 */
template<typename Type>
using type_list_unique_t = typename type_list_unique<Type>::type;


/**
 * @brief Provides the member constant `value` to true if a type list contains a
 * given type, false otherwise.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<typename List, typename Type>
struct type_list_contains;


/**
 * @copybrief type_list_contains
 * @tparam Type Types provided by the type list.
 * @tparam Other Type to look for.
 */
template<typename... Type, typename Other>
struct type_list_contains<type_list<Type...>, Other>: std::disjunction<std::is_same<Type, Other>...> {};


/**
 * @brief Helper variable template.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<class List, typename Type>
inline constexpr auto type_list_contains_v = type_list_contains<List, Type>::value;


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_diff;


/**
 * @brief Computes the difference between two type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 */
template<typename... Type, typename... Other>
struct type_list_diff<type_list<Type...>, type_list<Other...>> {
    /*! @brief A type list that is the difference between the two type lists. */
    using type = type_list_cat_t<std::conditional_t<type_list_contains_v<type_list<Other...>, Type>, type_list<>, type_list<Type>>...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists between which to compute the difference.
 */
template<typename... List>
using type_list_diff_t = typename type_list_diff<List...>::type;


/**
 * @brief A class to use to push around lists of constant values, nothing more.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list {
    /*! @brief Value list type. */
    using type = value_list;
    /*! @brief Compile-time number of elements in the value list. */
    static constexpr auto size = sizeof...(Value);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct value_list_element;


/**
 * @brief Provides compile-time indexed access to the values of a value list.
 * @tparam Index Index of the value to return.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<std::size_t Index, auto Value, auto... Other>
struct value_list_element<Index, value_list<Value, Other...>>
    : value_list_element<Index - 1u, value_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<auto Value, auto... Other>
struct value_list_element<0u, value_list<Value, Other...>> {
    /*! @brief Searched value. */
    static constexpr auto value = Value;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the value to return.
 * @tparam List Value list to search into.
 */
template<std::size_t Index, typename List>
inline constexpr auto value_list_element_v = value_list_element<Index, List>::value;


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @return A value list composed by the values of both the value lists.
 */
template<auto... Value, auto... Other>
constexpr value_list<Value..., Other...> operator+(value_list<Value...>, value_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct value_list_cat;


/*! @brief Concatenates multiple value lists. */
template<>
struct value_list_cat<> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<>;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @tparam List Other value lists, if any.
 */
template<auto... Value, auto... Other, typename... List>
struct value_list_cat<value_list<Value...>, value_list<Other...>, List...> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = typename value_list_cat<value_list<Value..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list_cat<value_list<Value...>> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<Value...>;
};


/**
 * @brief Helper type.
 * @tparam List Value lists to concatenate.
 */
template<typename... List>
using value_list_cat_t = typename value_list_cat<List...>::type;


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
[[nodiscard]] constexpr bool is_equality_comparable(...) { return false; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<0>)
-> decltype(std::declval<Type>() == std::declval<Type>()) { return true; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<1>)
-> decltype(std::declval<typename Type::value_type>(), std::declval<Type>() == std::declval<Type>()) {
    if constexpr(std::is_same_v<typename Type::value_type, Type>) {
        return is_equality_comparable<Type>(choice<0>);
    } else {
        return is_equality_comparable<typename Type::value_type>(choice<2>);
    }
}


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<2>)
-> decltype(std::declval<typename Type::mapped_type>(), std::declval<Type>() == std::declval<Type>()) {
    return is_equality_comparable<typename Type::key_type>(choice<2>) && is_equality_comparable<typename Type::mapped_type>(choice<2>);
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Provides the member constant `value` to true if a given type is
 * equality comparable, false otherwise.
 * @tparam Type Potentially equality comparable type.
 */
template<typename Type, typename = void>
struct is_equality_comparable: std::bool_constant<internal::is_equality_comparable<Type>(choice<2>)> {};


/**
 * @brief Helper variable template.
 * @tparam Type Potentially equality comparable type.
 */
template<class Type>
inline constexpr auto is_equality_comparable_v = is_equality_comparable<Type>::value;


/*! @brief Same as std::is_invocable, but with tuples. */
template<typename, typename>
struct is_applicable: std::false_type {};


/**
 * @copybrief is_applicable
 * @tparam Func A valid function type.
 * @tparam Tuple Tuple-like type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
* @copybrief is_applicable
* @tparam Func A valid function type.
* @tparam Tuple Tuple-like type.
* @tparam Args The list of arguments to use to probe the function type.
*/
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, const Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, typename Args>
inline constexpr auto is_applicable_v = is_applicable<Func, Args>::value;


/*! @brief Same as std::is_invocable_r, but with tuples for arguments. */
template<typename, typename, typename>
struct is_applicable_r: std::false_type {};


/**
 * @copybrief is_applicable_r
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename... Args>
struct is_applicable_r<Ret, Func, std::tuple<Args...>>: std::is_invocable_r<Ret, Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename Args>
inline constexpr auto is_applicable_r_v = is_applicable_r<Ret, Func, Args>::value;


/**
* @brief Provides the member constant `value` to true if a given type is
* complete, false otherwise.
* @tparam Type Potential complete type.
*/
template<typename Type, typename = void>
struct is_complete: std::false_type {};


/*! @copydoc is_complete */
template<typename Type>
struct is_complete<Type, std::void_t<decltype(sizeof(Type))>>: std::true_type {};


/**
* @brief Helper variable template.
* @tparam Type Potential complete type.
*/
template<typename Type>
inline constexpr auto is_complete_v = is_complete<Type>::value;


/**
 * @brief Transcribes the constness of a type to another type.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
struct constness_as {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::remove_const_t<To>;
};


/*! @copydoc constness_as */
template<typename To, typename From>
struct constness_as<To, const From> {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::add_const_t<To>;
};


/**
 * @brief Alias template to facilitate the transcription of the constness.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;


/**
 * @brief Extracts the class of a non-static member object or function.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
class member_class {
    static_assert(std::is_member_pointer_v<Member>, "Invalid pointer type to non-static member object or function");

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...));

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...) const);

    template<typename Class, typename Type>
    static Class * clazz(Type Class:: *);

public:
    /*! @brief The class of the given non-static member object or function. */
    using type = std::remove_pointer_t<decltype(clazz(std::declval<Member>()))>;
};


/**
 * @brief Helper type.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
using member_class_t = typename member_class<Member>::type;


}


#endif



namespace entt {


/**
 * @brief A SBO friendly, type-safe container for single values of any type.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 */
template<std::size_t Len, std::size_t Align>
class basic_any {
    enum class operation: std::uint8_t { COPY, MOVE, DTOR, COMP, ADDR, CADDR, TYPE };
    enum class policy: std::uint8_t { OWNER, REF, CREF };

    using storage_type = std::aligned_storage_t<Len + !Len, Align>;
    using vtable_type = const void *(const operation, const basic_any &, void *);

    template<typename Type>
    static constexpr bool in_situ = Len && alignof(Type) <= alignof(storage_type) && sizeof(Type) <= sizeof(storage_type) && std::is_nothrow_move_constructible_v<Type>;

    template<typename Type>
    [[nodiscard]] static constexpr policy type_to_policy() {
        if constexpr(std::is_lvalue_reference_v<Type>) {
            if constexpr(std::is_const_v<std::remove_reference_t<Type>>) {
                return policy::CREF;
            } else {
                return policy::REF;
            }
        } else {
            return policy::OWNER;
        }
    }

    template<typename Type>
    [[nodiscard]] static bool compare(const void *lhs, const void *rhs) {
        if constexpr(!std::is_function_v<Type> && is_equality_comparable_v<Type>) {
            return *static_cast<const Type *>(lhs) == *static_cast<const Type *>(rhs);
        } else {
            return lhs == rhs;
        }
    }

    template<typename Type>
    static const void * basic_vtable([[maybe_unused]] const operation op, [[maybe_unused]] const basic_any &from, [[maybe_unused]] void *to) {
        static_assert(std::is_same_v<std::remove_reference_t<std::remove_const_t<Type>>, Type>, "Invalid type");

        if constexpr(!std::is_void_v<Type>) {
            const Type *instance = (in_situ<Type> && from.mode == policy::OWNER)
                ? ENTT_LAUNDER(reinterpret_cast<const Type *>(&from.storage))
                : static_cast<const Type *>(from.instance);

            switch(op) {
            case operation::COPY:
                if constexpr(std::is_copy_constructible_v<Type>) {
                    static_cast<basic_any *>(to)->emplace<Type>(*instance);
                }
                break;
            case operation::MOVE:
                if constexpr(in_situ<Type>) {
                    if(from.mode == policy::OWNER) {
                        return new (&static_cast<basic_any *>(to)->storage) Type{std::move(*const_cast<Type *>(instance))};
                    }
                }

                return (static_cast<basic_any *>(to)->instance = std::exchange(const_cast<basic_any &>(from).instance, nullptr));
            case operation::DTOR:
                if(from.mode == policy::OWNER) {
                    if constexpr(in_situ<Type>) {
                        instance->~Type();
                    } else if constexpr(std::is_array_v<Type>) {
                        delete[] instance;
                    } else {
                        delete instance;
                    }
                }
                break;
            case operation::COMP:
                return compare<Type>(instance, (*static_cast<const basic_any **>(to))->data()) ? to : nullptr;
            case operation::ADDR:
                if(from.mode == policy::CREF) {
                    return nullptr;
                }
                [[fallthrough]];
            case operation::CADDR:
                return instance;
            case operation::TYPE:
                *static_cast<type_info *>(to) = type_id<Type>();
                break;
            }
        }

        return nullptr;
    }

    template<typename Type, typename... Args>
    void initialize([[maybe_unused]] Args &&... args) {
        if constexpr(!std::is_void_v<Type>) {
            if constexpr(std::is_lvalue_reference_v<Type>) {
                static_assert(sizeof...(Args) == 1u && (std::is_lvalue_reference_v<Args> && ...), "Invalid arguments");
                instance = (std::addressof(args), ...);
            } else if constexpr(in_situ<Type>) {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    new (&storage) Type{std::forward<Args>(args)...};
                } else {
                    new (&storage) Type(std::forward<Args>(args)...);
                }
            } else {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    instance = new Type{std::forward<Args>(args)...};
                } else {
                    instance = new Type(std::forward<Args>(args)...);
                }
            }
        }
    }

    basic_any(const basic_any &other, const policy pol) ENTT_NOEXCEPT
        : instance{other.data()},
          vtable{other.vtable},
          mode{pol}
    {}

public:
    /*! @brief Size of the internal storage. */
    static constexpr auto length = Len;
    /*! @brief Alignment requirement. */
    static constexpr auto alignment = Align;

    /*! @brief Default constructor. */
    basic_any() ENTT_NOEXCEPT
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {}

    /**
     * @brief Constructs a wrapper by directly initializing the new object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    explicit basic_any(std::in_place_type_t<Type>, Args &&... args)
        : instance{},
          vtable{&basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>},
          mode{type_to_policy<Type>()}
    {
        initialize<Type>(std::forward<Args>(args)...);
    }

    /**
     * @brief Constructs a wrapper that holds an unmanaged object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type>
    basic_any(std::reference_wrapper<Type> value) ENTT_NOEXCEPT
        : basic_any{}
    {
        // invokes deprecated assignment operator (and avoids issues with vs2017)
        *this = value;
    }

    /**
     * @brief Constructs a wrapper from a given value.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>>>
    basic_any(Type &&value)
        : instance{},
          vtable{&basic_vtable<std::decay_t<Type>>},
          mode{policy::OWNER}
    {
        initialize<std::decay_t<Type>>(std::forward<Type>(value));
    }

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    basic_any(const basic_any &other)
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {
        other.vtable(operation::COPY, other, this);
    }

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    basic_any(basic_any &&other) ENTT_NOEXCEPT
        : instance{},
          vtable{other.vtable},
          mode{other.mode}
    {
        vtable(operation::MOVE, other, this);
    }

    /*! @brief Frees the internal storage, whatever it means. */
    ~basic_any() {
        vtable(operation::DTOR, *this, nullptr);
    }

    /**
     * @brief Copy assignment operator.
     * @param other The instance to copy from.
     * @return This any object.
     */
    basic_any & operator=(const basic_any &other) {
        reset();
        other.vtable(operation::COPY, other, this);
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This any object.
     */
    basic_any & operator=(basic_any &&other) ENTT_NOEXCEPT {
        std::exchange(vtable, other.vtable)(operation::DTOR, *this, nullptr);
        other.vtable(operation::MOVE, other, this);
        mode = other.mode;
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    [[deprecated("Use std::in_place_type<T &>, entt::make_any<T &>, emplace<Type &> or forward_as_any instead")]]
    basic_any & operator=(std::reference_wrapper<Type> value) ENTT_NOEXCEPT {
        emplace<Type &>(value.get());
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>, basic_any &>
    operator=(Type &&value) {
        emplace<std::decay_t<Type>>(std::forward<Type>(value));
        return *this;
    }

    /**
     * @brief Returns the type of the contained object.
     * @return The type of the contained object, if any.
     */
    [[nodiscard]] type_info type() const ENTT_NOEXCEPT {
        type_info info{};
        vtable(operation::TYPE, *this, &info);
        return info;
    }

    /**
     * @brief Returns an opaque pointer to the contained instance.
     * @return An opaque pointer the contained instance, if any.
     */
    [[nodiscard]] const void * data() const ENTT_NOEXCEPT {
        return vtable(operation::CADDR, *this, nullptr);
    }

    /*! @copydoc data */
    [[nodiscard]] void * data() ENTT_NOEXCEPT {
        return const_cast<void *>(vtable(operation::ADDR, *this, nullptr));
    }

    /**
     * @brief Replaces the contained object by creating a new instance directly.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    void emplace(Args &&... args) {
        std::exchange(vtable, &basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>)(operation::DTOR, *this, nullptr);
        mode = type_to_policy<Type>();
        initialize<Type>(std::forward<Args>(args)...);
    }

    /*! @brief Destroys contained object */
    void reset() {
        std::exchange(vtable, &basic_vtable<void>)(operation::DTOR, *this, nullptr);
        mode = policy::OWNER;
    }

    /**
     * @brief Returns false if a wrapper is empty, true otherwise.
     * @return False if the wrapper is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(vtable(operation::CADDR, *this, nullptr) == nullptr);
    }

    /**
     * @brief Checks if two wrappers differ in their content.
     * @param other Wrapper with which to compare.
     * @return False if the two objects differ in their content, true otherwise.
     */
    bool operator==(const basic_any &other) const ENTT_NOEXCEPT {
        const basic_any *trampoline = &other;
        return type() == other.type() && (vtable(operation::COMP, *this, &trampoline) || !other.data());
    }

    /**
     * @brief Aliasing constructor.
     * @return A wrapper that shares a reference to an unmanaged object.
     */
    [[nodiscard]] basic_any as_ref() ENTT_NOEXCEPT {
        return basic_any{*this, (mode == policy::CREF ? policy::CREF : policy::REF)};
    }

    /*! @copydoc as_ref */
    [[nodiscard]] basic_any as_ref() const ENTT_NOEXCEPT {
        return basic_any{*this, policy::CREF};
    }

    /**
     * @brief Returns true if a wrapper owns its object, false otherwise.
     * @return True if the wrapper owns its object, false otherwise.
     */
    [[nodiscard]] bool owner() const ENTT_NOEXCEPT {
        return (mode == policy::OWNER);
    }

private:
    union { const void *instance; storage_type storage; };
    vtable_type *vtable;
    policy mode;
};


/**
 * @brief Checks if two wrappers differ in their content.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param lhs A wrapper, either empty or not.
 * @param rhs A wrapper, either empty or not.
 * @return True if the two wrappers differ in their content, false otherwise.
 */
template<std::size_t Len, std::size_t Align>
[[nodiscard]] inline bool operator!=(const basic_any<Len, Align> &lhs, const basic_any<Len, Align> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Performs type-safe access to the contained object.
 * @tparam Type Type to which conversion is required.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param data Target any object.
 * @return The element converted to the requested type.
 */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(const basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    const auto * const instance = any_cast<std::remove_reference_t<Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &&data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(std::move(*instance));
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
const Type * any_cast(const basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    return (data->type() == type_id<Type>() ? static_cast<const Type *>(data->data()) : nullptr);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type * any_cast(basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    // last attempt to make wrappers for const references return their values
    return (data->type() == type_id<Type>() ? static_cast<Type *>(static_cast<constness_as_t<basic_any<Len, Align>, Type> *>(data)->data()) : nullptr);
}


/**
 * @brief Constructs a wrapper from a given type, passing it all arguments.
 * @tparam Type Type of object to use to initialize the wrapper.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Args Types of arguments to use to construct the new instance.
 * @param args Parameters to use to construct the instance.
 * @return A properly initialized wrapper for an object of the given type.
 */
template<typename Type, std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename... Args>
basic_any<Len, Align> make_any(Args &&... args) {
    return basic_any<Len, Align>{std::in_place_type<Type>, std::forward<Args>(args)...};
}


/**
 * @brief Forwards its argument and avoids copies for lvalue references.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Type Type of argument to use to construct the new instance.
 * @param value Parameter to use to construct the instance.
 * @return A properly initialized and not necessarily owning wrapper.
 */
template<std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename Type>
basic_any<Len, Align> forward_as_any(Type &&value) {
    return basic_any<Len, Align>{std::in_place_type<std::conditional_t<std::is_rvalue_reference_v<Type>, std::decay_t<Type>, Type>>, std::forward<Type>(value)};
}


}


#endif

// #include "../core/type_info.hpp"

// #include "../core/type_traits.hpp"

// #include "fwd.hpp"
#ifndef ENTT_POLY_FWD_HPP
#define ENTT_POLY_FWD_HPP


#include <type_traits>


namespace entt {


template<typename, std::size_t Len, std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_poly;


/**
 * @brief Alias declaration for the most common use case.
 * @tparam Concept Concept descriptor.
 */
template<typename Concept>
using poly = basic_poly<Concept, sizeof(double[2])>;


}


#endif



namespace entt {


/*! @brief Inspector class used to infer the type of the virtual table. */
struct poly_inspector {
    /**
     * @brief Generic conversion operator (definition only).
     * @tparam Type Type to which conversion is requested.
     */
    template <class Type>
    operator Type &&() const;

    /**
     * @brief Dummy invocation function (definition only).
     * @tparam Member Index of the function to invoke.
     * @tparam Args Types of arguments to pass to the function.
     * @param args The arguments to pass to the function.
     * @return A poly inspector convertible to any type.
     */
    template<auto Member, typename... Args>
    poly_inspector invoke(Args &&... args) const;

    /*! @copydoc invoke */
    template<auto Member, typename... Args>
    poly_inspector invoke(Args &&... args);
};


/**
 * @brief Static virtual table factory.
 * @tparam Concept Concept descriptor.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 */
template<typename Concept, std::size_t Len, std::size_t Align>
class poly_vtable {
    using inspector = typename Concept::template type<poly_inspector>;

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(*)(inspector &, Args...)) -> Ret(*)(basic_any<Len, Align> &, Args...);

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(*)(const inspector &, Args...)) -> Ret(*)(const basic_any<Len, Align> &, Args...);

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(*)(Args...)) -> Ret(*)(const basic_any<Len, Align> &, Args...);

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(inspector:: *)(Args...)) -> Ret(*)(basic_any<Len, Align> &, Args...);

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(inspector:: *)(Args...) const) -> Ret(*)(const basic_any<Len, Align> &, Args...);

    template<auto... Candidate>
    static auto make_vtable(value_list<Candidate...>)
    -> decltype(std::make_tuple(vtable_entry(Candidate)...));

    template<typename... Func>
    [[nodiscard]] static constexpr auto make_vtable(type_list<Func...>)  {
        if constexpr(sizeof...(Func) == 0) {
            return decltype(make_vtable(typename Concept::template impl<inspector>{})){};
        } else if constexpr((std::is_function_v<Func> && ...)) {
            return decltype(std::make_tuple(vtable_entry(std::declval<Func inspector:: *>())...)){};
        }
    }

    template<typename Type, auto Candidate, typename Ret, typename Any, typename... Args>
    static void fill_vtable_entry(Ret(* &entry)(Any &, Args...)) {
        if constexpr(std::is_invocable_r_v<Ret, decltype(Candidate), Args...>) {
            entry = +[](Any &, Args... args) -> Ret {
                return std::invoke(Candidate, std::forward<Args>(args)...);
            };
        } else {
            entry = +[](Any &instance, Args... args) -> Ret {
                return static_cast<Ret>(std::invoke(Candidate, any_cast<constness_as_t<Type, Any> &>(instance), std::forward<Args>(args)...));
            };
        }
    }

    template<typename Type, auto... Index>
    [[nodiscard]] static auto fill_vtable(std::index_sequence<Index...>) {
        type impl{};
        (fill_vtable_entry<Type, value_list_element_v<Index, typename Concept::template impl<Type>>>(std::get<Index>(impl)), ...);
        return impl;
    }

public:
    /*! @brief Virtual table type. */
    using type = decltype(make_vtable(Concept{}));

    /**
     * @brief Returns a static virtual table for a specific concept and type.
     * @tparam Type The type for which to generate the virtual table.
     * @return A static virtual table for the given concept and type.
     */
    template<typename Type>
    [[nodiscard]] static const auto * instance() {
        static_assert(std::is_same_v<Type, std::decay_t<Type>>, "Type differs from its decayed form");
        static const auto vtable = fill_vtable<Type>(std::make_index_sequence<Concept::template impl<Type>::size>{});
        return &vtable;
    }
};


/**
 * @brief Poly base class used to inject functionalities into concepts.
 * @tparam Poly The outermost poly class.
 */
template<typename Poly>
struct poly_base {
    /**
     * @brief Invokes a function from the static virtual table.
     * @tparam Member Index of the function to invoke.
     * @tparam Args Types of arguments to pass to the function.
     * @param self A reference to the poly object that made the call.
     * @param args The arguments to pass to the function.
     * @return The return value of the invoked function, if any.
     */
    template<auto Member, typename... Args>
    [[nodiscard]] decltype(auto) invoke(const poly_base &self, Args &&... args) const {
        const auto &poly = static_cast<const Poly &>(self);
        return std::get<Member>(*poly.vtable)(poly.storage, std::forward<Args>(args)...);
    }

    /*! @copydoc invoke */
    template<auto Member, typename... Args>
    [[nodiscard]] decltype(auto) invoke(poly_base &self, Args &&... args) {
        auto &poly = static_cast<Poly &>(self);
        return std::get<Member>(*poly.vtable)(poly.storage, std::forward<Args>(args)...);
    }
};


/**
 * @brief Shortcut for calling `poly_base<Type>::invoke`.
 * @tparam Member Index of the function to invoke.
 * @tparam Poly A fully defined poly object.
 * @tparam Args Types of arguments to pass to the function.
 * @param self A reference to the poly object that made the call.
 * @param args The arguments to pass to the function.
 * @return The return value of the invoked function, if any.
 */
template<auto Member, typename Poly, typename... Args>
decltype(auto) poly_call(Poly &&self, Args &&... args) {
    return std::forward<Poly>(self).template invoke<Member>(self, std::forward<Args>(args)...);
}


/**
 * @brief Static polymorphism made simple and within everyone's reach.
 *
 * Static polymorphism is a very powerful tool in C++, albeit sometimes
 * cumbersome to obtain.<br/>
 * This class aims to make it simple and easy to use.
 *
 * @note
 * Both deduced and defined static virtual tables are supported.<br/>
 * Moreover, the `poly` class template also works with unmanaged objects.
 *
 * @tparam Concept Concept descriptor.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 */
template<typename Concept, std::size_t Len, std::size_t Align>
class basic_poly: private Concept::template type<poly_base<basic_poly<Concept, Len, Align>>> {
    /*! @brief A poly base is allowed to snoop into a poly object. */
    friend struct poly_base<basic_poly>;

    using vtable_type = typename poly_vtable<Concept, Len, Align>::type;

public:
    /*! @brief Concept type. */
    using concept_type = typename Concept::template type<poly_base<basic_poly>>;

    /*! @brief Default constructor. */
    basic_poly() ENTT_NOEXCEPT
        : storage{},
          vtable{}
    {}

    /**
     * @brief Constructs a poly by directly initializing the new object.
     * @tparam Type Type of object to use to initialize the poly.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    explicit basic_poly(std::in_place_type_t<Type>, Args &&... args)
        : storage{std::in_place_type<Type>, std::forward<Args>(args)...},
          vtable{poly_vtable<Concept, Len, Align>::template instance<std::remove_const_t<std::remove_reference_t<Type>>>()}
    {}

    /**
     * @brief Constructs a poly from a given value.
     * @tparam Type Type of object to use to initialize the poly.
     * @param value An instance of an object to use to initialize the poly.
     */
    template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::remove_cv_t<std::remove_reference_t<Type>>, basic_poly>>>
    basic_poly(Type &&value) ENTT_NOEXCEPT
        : basic_poly{std::in_place_type<std::remove_cv_t<std::remove_reference_t<Type>>>, std::forward<Type>(value)}
    {}

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    basic_poly(const basic_poly &other) = default;

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    basic_poly(basic_poly &&other) ENTT_NOEXCEPT
        : basic_poly{}
    {
        swap(*this, other);
    }

    /**
     * @brief Assignment operator.
     * @param other The instance to assign from.
     * @return This poly object.
     */
    basic_poly & operator=(basic_poly other) {
        swap(other, *this);
        return *this;
    }

    /**
     * @brief Returns the type of the contained object.
     * @return The type of the contained object, if any.
     */
    [[nodiscard]] type_info type() const ENTT_NOEXCEPT {
        return storage.type();
    }

    /**
     * @brief Returns an opaque pointer to the contained instance.
     * @return An opaque pointer the contained instance, if any.
     */
    [[nodiscard]] const void * data() const ENTT_NOEXCEPT {
        return storage.data();
    }

    /*! @copydoc data */
    [[nodiscard]] void * data() ENTT_NOEXCEPT {
        return storage.data();
    }

    /**
     * @brief Replaces the contained object by creating a new instance directly.
     * @tparam Type Type of object to use to initialize the poly.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    void emplace(Args &&... args) {
        *this = basic_poly{std::in_place_type<Type>, std::forward<Args>(args)...};
    }

    /*! @brief Destroys contained object */
    void reset() {
        *this = basic_poly{};
    }

    /**
     * @brief Returns false if a poly is empty, true otherwise.
     * @return False if the poly is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(vtable == nullptr);
    }

    /**
     * @brief Returns a pointer to the underlying concept.
     * @return A pointer to the underlying concept.
     */
    [[nodiscard]] concept_type * operator->() ENTT_NOEXCEPT {
        return this;
    }

    /*! @copydoc operator-> */
    [[nodiscard]] const concept_type * operator->() const ENTT_NOEXCEPT {
        return this;
    }

    /**
     * @brief Swaps two poly objects.
     * @param lhs A valid poly object.
     * @param rhs A valid poly object.
     */
    friend void swap(basic_poly &lhs, basic_poly &rhs) {
        using std::swap;
        swap(lhs.storage, rhs.storage);
        swap(lhs.vtable, rhs.vtable);
    }

    /**
     * @brief Aliasing constructor.
     * @return A poly that shares a reference to an unmanaged object.
     */
    [[nodiscard]] basic_poly as_ref() ENTT_NOEXCEPT {
        basic_poly ref = std::as_const(*this).as_ref();
        ref.storage = storage.as_ref();
        return ref;
    }

    /*! @copydoc as_ref */
    [[nodiscard]] basic_poly as_ref() const ENTT_NOEXCEPT {
        basic_poly ref{};
        ref.storage = storage.as_ref();
        ref.vtable = vtable;
        return ref;
    }

private:
    basic_any<Len, Align> storage;
    const vtable_type *vtable;
};


}


#endif

// #include "fwd.hpp"



namespace entt {


/**
 * @brief Basic poly storage implementation.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
struct Storage: type_list<type_info() const ENTT_NOEXCEPT> {
    /**
     * @brief Concept definition.
     * @tparam Base Opaque base class from which to inherit.
     */
    template<typename Base>
    struct type: Base {
        /**
         * @brief Returns a type info for the contained objects.
         * @return The type info for the contained objects.
         */
        type_info value_type() const ENTT_NOEXCEPT {
            return poly_call<0>(*this);
        }
    };

    /**
     * @brief Concept implementation.
     * @tparam Type Type for which to generate an implementation.
     */
    template<typename Type>
    using impl = value_list<&type_id<typename Type::value_type>>;
};


/**
 * @brief Defines the poly storage type associate with a given entity type.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity, typename = void>
struct poly_storage_traits {
    /*! @brief Poly storage type for the given entity type. */
    using storage_type = poly<Storage<Entity>>;
};


}


#endif

// #include "runtime_view.hpp"
#ifndef ENTT_ENTITY_RUNTIME_VIEW_HPP
#define ENTT_ENTITY_RUNTIME_VIEW_HPP


#include <iterator>
#include <vector>
#include <utility>
#include <algorithm>
#include <type_traits>
// #include "../config/config.h"

// #include "entity.hpp"

// #include "sparse_set.hpp"

// #include "fwd.hpp"



namespace entt {


/**
 * @brief Runtime view.
 *
 * Runtime views iterate over those entities that have at least all the given
 * components in their bags. During initialization, a runtime view looks at the
 * number of entities available for each component and picks up a reference to
 * the smallest set of candidate entities in order to get a performance boost
 * when iterate.<br/>
 * Order of elements during iterations are highly dependent on the order of the
 * underlying data structures. See sparse_set and its specializations for more
 * details.
 *
 * @b Important
 *
 * Iterators aren't invalidated if:
 *
 * * New instances of the given components are created and assigned to entities.
 * * The entity currently pointed is modified (as an example, if one of the
 *   given components is removed from the entity to which the iterator points).
 * * The entity currently pointed is destroyed.
 *
 * In all the other cases, modifying the pools of the given components in any
 * way invalidates all the iterators and using them results in undefined
 * behavior.
 *
 * @note
 * Views share references to the underlying data structures of the registry that
 * generated them. Therefore any change to the entities and to the components
 * made by means of the registry are immediately reflected by the views, unless
 * a pool was missing when the view was built (in this case, the view won't
 * have a valid reference and won't be updated accordingly).
 *
 * @warning
 * Lifetime of a view must not overcome that of the registry that generated it.
 * In any other case, attempting to use a view results in undefined behavior.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
class basic_runtime_view final {
    using basic_common_type = basic_sparse_set<Entity>;
    using underlying_iterator = typename basic_common_type::iterator;

    class view_iterator final {
        [[nodiscard]] bool valid() const {
            const auto entt = *it;

            return (!stable_storage || (entt != tombstone))
                && std::all_of(pools->begin()++, pools->end(), [entt](const auto *curr) { return curr->contains(entt); })
                && std::none_of(filter->cbegin(), filter->cend(), [entt](const auto *curr) { return curr && curr->contains(entt); });
        }

    public:
        using difference_type = typename underlying_iterator::difference_type;
        using value_type = typename underlying_iterator::value_type;
        using pointer = typename underlying_iterator::pointer;
        using reference = typename underlying_iterator::reference;
        using iterator_category = std::bidirectional_iterator_tag;

        view_iterator() ENTT_NOEXCEPT = default;

        view_iterator(const std::vector<const basic_common_type *> &cpools, const std::vector<const basic_common_type *> &ignore, underlying_iterator curr) ENTT_NOEXCEPT
            : pools{&cpools},
              filter{&ignore},
              it{curr},
              stable_storage{std::any_of(pools->cbegin(), pools->cend(), [](const basic_common_type *cpool) { return (cpool->policy() == deletion_policy::in_place); })}
        {
            if(it != (*pools)[0]->end() && !valid()) {
                ++(*this);
            }
        }

        view_iterator & operator++() {
            while(++it != (*pools)[0]->end() && !valid());
            return *this;
        }

        view_iterator operator++(int) {
            view_iterator orig = *this;
            return ++(*this), orig;
        }

        view_iterator & operator--() ENTT_NOEXCEPT {
            while(--it != (*pools)[0]->begin() && !valid());
            return *this;
        }

        view_iterator operator--(int) ENTT_NOEXCEPT {
            view_iterator orig = *this;
            return operator--(), orig;
        }

        [[nodiscard]] bool operator==(const view_iterator &other) const ENTT_NOEXCEPT {
            return other.it == it;
        }

        [[nodiscard]] bool operator!=(const view_iterator &other) const ENTT_NOEXCEPT {
            return !(*this == other);
        }

        [[nodiscard]] pointer operator->() const {
            return it.operator->();
        }

        [[nodiscard]] reference operator*() const {
            return *operator->();
        }

    private:
        const std::vector<const basic_common_type *> *pools;
        const std::vector<const basic_common_type *> *filter;
        underlying_iterator it;
        bool stable_storage;
    };

    [[nodiscard]] bool valid() const {
        return !pools.empty() && pools.front();
    }

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Bidirectional iterator type. */
    using iterator = view_iterator;

    /*! @brief Default constructor to use to create empty, invalid views. */
    basic_runtime_view() ENTT_NOEXCEPT
        : pools{},
          filter{}
    {}

    /**
     * @brief Constructs a runtime view from a set of storage classes.
     * @param cpools The storage for the types to iterate.
     * @param epools The storage for the types used to filter the view.
     */
    basic_runtime_view(std::vector<const basic_common_type *> cpools, std::vector<const basic_common_type *> epools) ENTT_NOEXCEPT
        : pools{std::move(cpools)},
          filter{std::move(epools)}
    {
        // brings the best candidate (if any) on front of the vector
        std::rotate(pools.begin(), std::min_element(pools.begin(), pools.end(), [](const auto *lhs, const auto *rhs) {
            return (!lhs && rhs) || (lhs && rhs && lhs->size() < rhs->size());
        }), pools.end());
    }

    /**
     * @brief Estimates the number of entities iterated by the view.
     * @return Estimated number of entities iterated by the view.
     */
    [[nodiscard]] size_type size_hint() const {
        return valid() ? pools.front()->size() : size_type{};
    }

    /**
     * @brief Returns an iterator to the first entity that has the given
     * components.
     *
     * The returned iterator points to the first entity that has the given
     * components. If the view is empty, the returned iterator will be equal to
     * `end()`.
     *
     * @return An iterator to the first entity that has the given components.
     */
    [[nodiscard]] iterator begin() const {
        return valid() ? iterator{pools, filter, pools[0]->begin()} : iterator{};
    }

    /**
     * @brief Returns an iterator that is past the last entity that has the
     * given components.
     *
     * The returned iterator points to the entity following the last entity that
     * has the given components. Attempting to dereference the returned iterator
     * results in undefined behavior.
     *
     * @return An iterator to the entity following the last entity that has the
     * given components.
     */
    [[nodiscard]] iterator end() const {
        return valid() ? iterator{pools, filter, pools[0]->end()} : iterator{};
    }

    /**
     * @brief Checks if a view contains an entity.
     * @param entt A valid entity identifier.
     * @return True if the view contains the given entity, false otherwise.
     */
    [[nodiscard]] bool contains(const entity_type entt) const {
        return valid() && std::all_of(pools.cbegin(), pools.cend(), [entt](const auto *curr) { return curr->contains(entt); })
                && std::none_of(filter.cbegin(), filter.cend(), [entt](const auto *curr) { return curr && curr->contains(entt); });
    }

    /**
     * @brief Iterates entities and applies the given function object to them.
     *
     * The function object is invoked for each entity. It is provided only with
     * the entity itself. To get the components, users can use the registry with
     * which the view was built.<br/>
     * The signature of the function should be equivalent to the following:
     *
     * @code{.cpp}
     * void(const entity_type);
     * @endcode
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void each(Func func) const {
        for(const auto entity: *this) {
            func(entity);
        }
    }

private:
    std::vector<const basic_common_type *> pools;
    std::vector<const basic_common_type *> filter;
};


}


#endif

// #include "sparse_set.hpp"

// #include "storage.hpp"

// #include "utility.hpp"

// #include "view.hpp"
#ifndef ENTT_ENTITY_VIEW_HPP
#define ENTT_ENTITY_VIEW_HPP


#include <iterator>
#include <array>
#include <tuple>
#include <utility>
#include <algorithm>
#include <type_traits>
// #include "../config/config.h"

// #include "../core/type_traits.hpp"

// #include "component.hpp"

// #include "entity.hpp"

// #include "fwd.hpp"

// #include "sparse_set.hpp"

// #include "storage.hpp"

// #include "utility.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename Policy, typename It, std::size_t AllOf, std::size_t NoneOf>
class view_iterator final {
    using basic_common_type = basic_sparse_set<typename std::iterator_traits<It>::value_type>;

    [[nodiscard]] bool valid() const {
        const auto entt = *it;
        return Policy::accept(entt)
            && std::apply([entt](const auto *... curr) { return (curr->contains(entt) && ...); }, pools)
            && std::apply([entt](const auto *... curr) { return (!curr->contains(entt) && ...); }, filter);
    }

public:
    using difference_type = typename std::iterator_traits<It>::difference_type;
    using value_type = typename std::iterator_traits<It>::value_type;
    using pointer = typename std::iterator_traits<It>::pointer;
    using reference = typename std::iterator_traits<It>::reference;
    using iterator_category = std::bidirectional_iterator_tag;

    view_iterator() ENTT_NOEXCEPT
        : first{},
          last{},
          it{},
          pools{},
          filter{}
    {}

    view_iterator(It from, It to, It curr, std::array<const basic_common_type *, AllOf> all_of, std::array<const basic_common_type *, NoneOf> none_of) ENTT_NOEXCEPT
        : first{from},
          last{to},
          it{curr},
          pools{all_of},
          filter{none_of}
    {
        if(it != last && !valid()) {
            ++(*this);
        }
    }

    view_iterator & operator++() ENTT_NOEXCEPT {
        while(++it != last && !valid());
        return *this;
    }

    view_iterator operator++(int) ENTT_NOEXCEPT {
        view_iterator orig = *this;
        return ++(*this), orig;
    }

    view_iterator & operator--() ENTT_NOEXCEPT {
        while(--it != first && !valid());
        return *this;
    }

    view_iterator operator--(int) ENTT_NOEXCEPT {
        view_iterator orig = *this;
        return operator--(), orig;
    }

    [[nodiscard]] bool operator==(const view_iterator &other) const ENTT_NOEXCEPT {
        return other.it == it;
    }

    [[nodiscard]] bool operator!=(const view_iterator &other) const ENTT_NOEXCEPT {
        return !(*this == other);
    }

    [[nodiscard]] pointer operator->() const {
        return &*it;
    }

    [[nodiscard]] reference operator*() const {
        return *operator->();
    }

private:
    It first;
    It last;
    It it;
    std::array<const basic_common_type *, AllOf> pools;
    std::array<const basic_common_type *, NoneOf> filter;
};


}


/*! @brief Stable storage policy, aimed at pointer stability. */
struct stable_storage_policy {
    /**
    * @cond TURN_OFF_DOXYGEN
    * Internal details not to be documented.
    */
    template<typename Entity>
    [[nodiscard]] static constexpr bool accept(const Entity entity) ENTT_NOEXCEPT {
        return entity != tombstone;
    }
    /**
    * Internal details not to be documented.
    * @endcond
    */
};


/*! @brief Packed storage policy, aimed at faster linear iteration. */
struct packed_storage_policy {
    /**
    * @cond TURN_OFF_DOXYGEN
    * Internal details not to be documented.
    */
    template<typename Entity>
    [[nodiscard]] static constexpr bool accept(const Entity) ENTT_NOEXCEPT {
        return true;
    }
    /**
    * Internal details not to be documented.
    * @endcond
    */
};


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief View implementation.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error, but for a few reasonable cases.
 */
template<typename...>
class basic_view_impl;


/*! @brief View implementation dispatcher. */
template<typename...>
struct basic_view;


/**
 * @brief Multi component view.
 *
 * Multi component views iterate over those entities that have at least all the
 * given components in their bags. During initialization, a multi component view
 * looks at the number of entities available for each component and uses the
 * smallest set in order to get a performance boost when iterate.
 *
 * @b Important
 *
 * Iterators aren't invalidated if:
 *
 * * New instances of the given components are created and assigned to entities.
 * * The entity currently pointed is modified (as an example, if one of the
 *   given components is removed from the entity to which the iterator points).
 * * The entity currently pointed is destroyed.
 *
 * In all other cases, modifying the pools iterated by the view in any way
 * invalidates all the iterators and using them results in undefined behavior.
 *
 * @note
 * Views share references to the underlying data structures of the registry that
 * generated them. Therefore any change to the entities and to the components
 * made by means of the registry are immediately reflected by views.
 *
 * @warning
 * Lifetime of a view must not overcome that of the registry that generated it.
 * In any other case, attempting to use a view results in undefined behavior.
 *
 * @tparam Policy Common (stricter) storage policy.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Exclude Types of components used to filter the view.
 * @tparam Component Types of components iterated by the view.
 */
template<typename Policy, typename Entity, typename... Exclude, typename... Component>
class basic_view_impl<Policy, Entity, exclude_t<Exclude...>, Component...> {
    using basic_common_type = basic_sparse_set<Entity>;

    template<typename Comp>
    using storage_type = constness_as_t<typename storage_traits<Entity, std::remove_const_t<Comp>>::storage_type, Comp>;

    class iterable final {
        template<typename It>
        struct iterable_iterator final {
            using difference_type = std::ptrdiff_t;
            using value_type = decltype(std::tuple_cat(std::tuple<Entity>{}, std::declval<basic_view_impl>().get({})));
            using pointer = void;
            using reference = value_type;
            using iterator_category = std::input_iterator_tag;

            iterable_iterator(It from, const basic_view_impl *parent) ENTT_NOEXCEPT
                : it{from},
                  view{parent}
            {}

            iterable_iterator & operator++() ENTT_NOEXCEPT {
                return ++it, *this;
            }

            iterable_iterator operator++(int) ENTT_NOEXCEPT {
                iterable_iterator orig = *this;
                return ++(*this), orig;
            }

            [[nodiscard]] reference operator*() const ENTT_NOEXCEPT {
                return std::tuple_cat(std::make_tuple(*it), view->get(*it));
            }

            [[nodiscard]] bool operator==(const iterable_iterator &other) const ENTT_NOEXCEPT {
                return other.it == it;
            }

            [[nodiscard]] bool operator!=(const iterable_iterator &other) const ENTT_NOEXCEPT {
                return !(*this == other);
            }

        private:
            It it;
            const basic_view_impl *view;
        };

    public:
        using iterator = iterable_iterator<internal::view_iterator<Policy, typename basic_common_type::iterator, sizeof...(Component) - 1u, sizeof...(Exclude)>>;
        using reverse_iterator = iterable_iterator<internal::view_iterator<Policy, typename basic_common_type::reverse_iterator, sizeof...(Component) - 1u, sizeof...(Exclude)>>;

        iterable(const basic_view_impl &parent)
            : view{parent}
        {}

        [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
            return { view.begin(), &view };
        }

        [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
            return { view.end(), &view };
        }

        [[nodiscard]] reverse_iterator rbegin() const ENTT_NOEXCEPT {
            return { view.rbegin(), &view };
        }

        [[nodiscard]] reverse_iterator rend() const ENTT_NOEXCEPT {
            return { view.rend(), &view };
        }

    private:
        const basic_view_impl view;
    };

    [[nodiscard]] const auto * candidate() const ENTT_NOEXCEPT {
        return (std::min)({ static_cast<const basic_common_type *>(std::get<storage_type<Component> *>(pools))... }, [](const auto *lhs, const auto *rhs) {
            return lhs->size() < rhs->size();
        });
    }

    [[nodiscard]] auto pools_to_unchecked_array() const ENTT_NOEXCEPT {
        std::size_t pos{};
        std::array<const basic_common_type *, sizeof...(Component) - 1u> other{};
        (static_cast<void>(std::get<storage_type<Component> *>(pools) == view ? void() : void(other[pos++] = std::get<storage_type<Component> *>(pools))), ...);
        return other;
    }

    [[nodiscard]] auto filter_to_array() const ENTT_NOEXCEPT {
        return std::array<const basic_common_type *, sizeof...(Exclude)>{std::get<const storage_type<Exclude> *>(filter)...};
    }

    template<typename Comp, typename It>
    [[nodiscard]] auto dispatch_get([[maybe_unused]] It &it, [[maybe_unused]] const Entity entt) const {
        if constexpr(std::is_same_v<typename std::iterator_traits<It>::value_type, typename storage_type<Comp>::value_type>) {
            return std::forward_as_tuple(*it);
        } else {
            return get_as_tuple(*std::get<storage_type<Comp> *>(pools), entt);
        }
    }

    template<typename Comp, typename Func>
    void traverse(Func func) const {
        if constexpr(std::is_void_v<decltype(std::get<storage_type<Comp> *>(pools)->get({}))>) {
            for(const auto entt: static_cast<const basic_common_type &>(*std::get<storage_type<Comp> *>(pools))) {
                if(Policy::accept(entt) && ((std::is_same_v<Comp, Component> || std::get<storage_type<Component> *>(pools)->contains(entt)) && ...)
                    && (!std::get<const storage_type<Exclude> *>(filter)->contains(entt) && ...))
                {
                    if constexpr(is_applicable_v<Func, decltype(std::tuple_cat(std::tuple<entity_type>{}, std::declval<basic_view_impl>().get({})))>) {
                        std::apply(func, std::tuple_cat(std::make_tuple(entt), get(entt)));
                    } else {
                        std::apply(func, get(entt));
                    }
                }
            }
        } else {
            auto it = std::get<storage_type<Comp> *>(pools)->begin();

            for(const auto entt: static_cast<const basic_common_type &>(*std::get<storage_type<Comp> *>(pools))) {
                if(Policy::accept(entt) && ((std::is_same_v<Comp, Component> || std::get<storage_type<Component> *>(pools)->contains(entt)) && ...)
                    && (!std::get<const storage_type<Exclude> *>(filter)->contains(entt) && ...))
                {
                    if constexpr(is_applicable_v<Func, decltype(std::tuple_cat(std::tuple<entity_type>{}, std::declval<basic_view_impl>().get({})))>) {
                        std::apply(func, std::tuple_cat(std::make_tuple(entt), dispatch_get<Component>(it, entt)...));
                    } else {
                        std::apply(func, std::tuple_cat(dispatch_get<Component>(it, entt)...));
                    }
                }

                ++it;
            }
        }
    }

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Bidirectional iterator type. */
    using iterator = internal::view_iterator<Policy, typename basic_common_type::iterator, sizeof...(Component) - 1u, sizeof...(Exclude)>;
    /*! @brief Reverse iterator type. */
    using reverse_iterator = internal::view_iterator<Policy, typename basic_common_type::reverse_iterator, sizeof...(Component) - 1u, sizeof...(Exclude)>;
    /*! @brief Iterable view type. */
    using iterable_view = iterable;

    /*! @brief Default constructor to use to create empty, invalid views. */
    basic_view_impl() ENTT_NOEXCEPT
        : view{}
    {}

    /**
     * @brief Constructs a multi-type view from a set of storage classes.
     * @param component The storage for the types to iterate.
     * @param epool The storage for the types used to filter the view.
     */
    basic_view_impl(storage_type<Component> &... component, const storage_type<Exclude> &... epool) ENTT_NOEXCEPT
        : pools{&component...},
          filter{&epool...},
          view{candidate()}
    {}

    /**
     * @brief Forces the type to use to drive iterations.
     * @tparam Comp Type of component to use to drive the iteration.
     */
    template<typename Comp>
    void use() const ENTT_NOEXCEPT {
        view = std::get<storage_type<Comp> *>(pools);
    }

    /**
     * @brief Estimates the number of entities iterated by the view.
     * @return Estimated number of entities iterated by the view.
     */
    [[nodiscard]] size_type size_hint() const ENTT_NOEXCEPT {
        return view->size();
    }

    /**
     * @brief Returns an iterator to the first entity of the view.
     *
     * The returned iterator points to the first entity of the view. If the view
     * is empty, the returned iterator will be equal to `end()`.
     *
     * @return An iterator to the first entity of the view.
     */
    [[nodiscard]] iterator begin() const {
        return iterator{view->begin(), view->end(), view->begin(), pools_to_unchecked_array(), filter_to_array()};
    }

    /**
     * @brief Returns an iterator that is past the last entity of the view.
     *
     * The returned iterator points to the entity following the last entity of
     * the view. Attempting to dereference the returned iterator results in
     * undefined behavior.
     *
     * @return An iterator to the entity following the last entity of the view.
     */
    [[nodiscard]] iterator end() const {
        return iterator{view->begin(), view->end(), view->end(), pools_to_unchecked_array(), filter_to_array()};
    }

    /**
     * @brief Returns an iterator to the first entity of the reversed view.
     *
     * The returned iterator points to the first entity of the reversed view. If
     * the view is empty, the returned iterator will be equal to `rend()`.
     *
     * @return An iterator to the first entity of the reversed view.
     */
    [[nodiscard]] reverse_iterator rbegin() const {
        return reverse_iterator{view->rbegin(), view->rend(), view->rbegin(), pools_to_unchecked_array(), filter_to_array()};
    }

    /**
     * @brief Returns an iterator that is past the last entity of the reversed
     * view.
     *
     * The returned iterator points to the entity following the last entity of
     * the reversed view. Attempting to dereference the returned iterator
     * results in undefined behavior.
     *
     * @return An iterator to the entity following the last entity of the
     * reversed view.
     */
    [[nodiscard]] reverse_iterator rend() const {
        return reverse_iterator{view->rbegin(), view->rend(), view->rend(), pools_to_unchecked_array(), filter_to_array()};
    }

    /**
     * @brief Returns the first entity of the view, if any.
     * @return The first entity of the view if one exists, the null entity
     * otherwise.
     */
    [[nodiscard]] entity_type front() const {
        const auto it = begin();
        return it != end() ? *it : null;
    }

    /**
     * @brief Returns the last entity of the view, if any.
     * @return The last entity of the view if one exists, the null entity
     * otherwise.
     */
    [[nodiscard]] entity_type back() const {
        const auto it = rbegin();
        return it != rend() ? *it : null;
    }

    /**
     * @brief Finds an entity.
     * @param entt A valid entity identifier.
     * @return An iterator to the given entity if it's found, past the end
     * iterator otherwise.
     */
    [[nodiscard]] iterator find(const entity_type entt) const {
        const auto it = iterator{view->begin(), view->end(), view->find(entt), pools_to_unchecked_array(), filter_to_array()};
        return (it != end() && *it == entt) ? it : end();
    }

    /**
     * @brief Checks if a view is properly initialized.
     * @return True if the view is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return view != nullptr;
    }

    /**
     * @brief Checks if a view contains an entity.
     * @param entt A valid entity identifier.
     * @return True if the view contains the given entity, false otherwise.
     */
    [[nodiscard]] bool contains(const entity_type entt) const {
        return (std::get<storage_type<Component> *>(pools)->contains(entt) && ...) && (!std::get<const storage_type<Exclude> *>(filter)->contains(entt) && ...);
    }

    /**
     * @brief Returns the components assigned to the given entity.
     *
     * Prefer this function instead of `registry::get` during iterations. It has
     * far better performance than its counterpart.
     *
     * @warning
     * Attempting to use an invalid component type results in a compilation
     * error. Attempting to use an entity that doesn't belong to the view
     * results in undefined behavior.
     *
     * @tparam Comp Types of components to get.
     * @param entt A valid entity identifier.
     * @return The components assigned to the entity.
     */
    template<typename... Comp>
    [[nodiscard]] decltype(auto) get([[maybe_unused]] const entity_type entt) const {
        ENTT_ASSERT(contains(entt), "View does not contain entity");

        if constexpr(sizeof...(Comp) == 0) {
            return std::tuple_cat(get_as_tuple(*std::get<storage_type<Component> *>(pools), entt)...);
        } else if constexpr(sizeof...(Comp) == 1) {
            return (std::get<storage_type<Comp> *>(pools)->get(entt), ...);
        } else {
            return std::tuple_cat(get_as_tuple(*std::get<storage_type<Comp> *>(pools), entt)...);
        }
    }

    /**
     * @brief Iterates entities and components and applies the given function
     * object to them.
     *
     * The function object is invoked for each entity. It is provided with the
     * entity itself and a set of references to non-empty components. The
     * _constness_ of the components is as requested.<br/>
     * The signature of the function must be equivalent to one of the following
     * forms:
     *
     * @code{.cpp}
     * void(const entity_type, Type &...);
     * void(Type &...);
     * @endcode
     *
     * @note
     * Empty types aren't explicitly instantiated and therefore they are never
     * returned during iterations.
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void each(Func func) const {
        ((std::get<storage_type<Component> *>(pools) == view ? traverse<Component>(std::move(func)) : void()), ...);
    }

    /**
     * @brief Iterates entities and components and applies the given function
     * object to them.
     *
     * The pool of the suggested component is used to lead the iterations. The
     * returned entities will therefore respect the order of the pool associated
     * with that type.
     *
     * @sa each
     *
     * @tparam Comp Type of component to use to drive the iteration.
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Comp, typename Func>
    void each(Func func) const {
        use<Comp>();
        traverse<Comp>(std::move(func));
    }

    /**
     * @brief Returns an iterable object to use to _visit_ the view.
     *
     * The iterable object returns tuples that contain the current entity and a
     * set of references to its non-empty components. The _constness_ of the
     * components is as requested.
     *
     * @note
     * Empty types aren't explicitly instantiated and therefore they are never
     * returned during iterations.
     *
     * @return An iterable object to use to _visit_ the view.
     */
    [[nodiscard]] iterable_view each() const ENTT_NOEXCEPT {
        return iterable_view{*this};
    }

    /**
     * @brief Returns an iterable object to use to _visit_ the view.
     *
     * The pool of the suggested component is used to lead the iterations. The
     * returned elements will therefore respect the order of the pool associated
     * with that type.
     *
     * @sa each
     *
     * @tparam Comp Type of component to use to drive the iteration.
     * @return An iterable object to use to _visit_ the view.
     */
    template<typename Comp>
    [[nodiscard]] iterable_view each() const ENTT_NOEXCEPT {
        use<Comp>();
        return iterable_view{*this};
    }

    /**
     * @brief Combines two views in a _more specific_ one (friend function).
     * @tparam Id A valid entity type (see entt_traits for more details).
     * @tparam ELhs Filter list of the first view.
     * @tparam CLhs Component list of the first view.
     * @tparam ERhs Filter list of the second view.
     * @tparam CRhs Component list of the second view.
     * @return A more specific view.
     */
    template<typename Id, typename... ELhs, typename... CLhs, typename... ERhs, typename... CRhs>
    friend auto operator|(const basic_view<Id, exclude_t<ELhs...>, CLhs...> &, const basic_view<Id, exclude_t<ERhs...>, CRhs...> &);

private:
    const std::tuple<storage_type<Component> *...> pools;
    const std::tuple<const storage_type<Exclude> *...> filter;
    mutable const basic_common_type *view;
};


/**
 * @brief Single component view specialization.
 *
 * Single component views are specialized in order to get a boost in terms of
 * performance. This kind of views can access the underlying data structure
 * directly and avoid superfluous checks.
 *
 * @b Important
 *
 * Iterators aren't invalidated if:
 *
 * * New instances of the given component are created and assigned to entities.
 * * The entity currently pointed is modified (as an example, the given
 *   component is removed from the entity to which the iterator points).
 * * The entity currently pointed is destroyed.
 *
 * In all other cases, modifying the pool iterated by the view in any way
 * invalidates all the iterators and using them results in undefined behavior.
 *
 * @note
 * Views share a reference to the underlying data structure of the registry that
 * generated them. Therefore any change to the entities and to the components
 * made by means of the registry are immediately reflected by views.
 *
 * @warning
 * Lifetime of a view must not overcome that of the registry that generated it.
 * In any other case, attempting to use a view results in undefined behavior.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Component Type of component iterated by the view.
 */
template<typename Entity, typename Component>
class basic_view_impl<packed_storage_policy, Entity, exclude_t<>, Component> {
    using basic_common_type = basic_sparse_set<Entity>;
    using storage_type = constness_as_t<typename storage_traits<Entity, std::remove_const_t<Component>>::storage_type, Component>;

    class iterable final {
        template<typename... It>
        struct iterable_iterator final {
            using difference_type = std::ptrdiff_t;
            using value_type = decltype(std::tuple_cat(std::tuple<Entity>{}, std::declval<basic_view_impl>().get({})));
            using pointer = void;
            using reference = value_type;
            using iterator_category = std::input_iterator_tag;

            template<typename... Discard>
            iterable_iterator(It... from, Discard...) ENTT_NOEXCEPT
                : it{from...}
            {}

            iterable_iterator & operator++() ENTT_NOEXCEPT {
                return (++std::get<It>(it), ...), *this;
            }

            iterable_iterator operator++(int) ENTT_NOEXCEPT {
                iterable_iterator orig = *this;
                return ++(*this), orig;
            }

            [[nodiscard]] reference operator*() const ENTT_NOEXCEPT {
                return { *std::get<It>(it)... };
            }

            [[nodiscard]] bool operator==(const iterable_iterator &other) const ENTT_NOEXCEPT {
                return std::get<0>(other.it) == std::get<0>(it);
            }

            [[nodiscard]] bool operator!=(const iterable_iterator &other) const ENTT_NOEXCEPT {
                return !(*this == other);
            }

        private:
            std::tuple<It...> it;
        };

    public:
        using iterator = std::conditional_t<
            std::is_void_v<decltype(std::declval<storage_type>().get({}))>,
            iterable_iterator<typename basic_common_type::iterator>,
            iterable_iterator<typename basic_common_type::iterator, decltype(std::declval<storage_type>().begin())>
        >;
        using reverse_iterator = std::conditional_t<
            std::is_void_v<decltype(std::declval<storage_type>().get({}))>,
            iterable_iterator<typename basic_common_type::reverse_iterator>,
            iterable_iterator<typename basic_common_type::reverse_iterator, decltype(std::declval<storage_type>().rbegin())>
        >;

        iterable(storage_type &ref)
            : pool{&ref}
        {}

        [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
            return iterator{pool->basic_common_type::begin(), pool->begin()};
        }

        [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
            return iterator{pool->basic_common_type::end(), pool->end()};
        }

        [[nodiscard]] reverse_iterator rbegin() const ENTT_NOEXCEPT {
            return reverse_iterator{pool->basic_common_type::rbegin(), pool->rbegin()};
        }

        [[nodiscard]] reverse_iterator rend() const ENTT_NOEXCEPT {
            return reverse_iterator{pool->basic_common_type::rend(), pool->rend()};
        }

    private:
        storage_type * const pool;
    };

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Random access iterator type. */
    using iterator = typename basic_common_type::iterator;
    /*! @brief Reversed iterator type. */
    using reverse_iterator = typename basic_common_type::reverse_iterator;
    /*! @brief Iterable view type. */
    using iterable_view = iterable;

    /*! @brief Default constructor to use to create empty, invalid views. */
    basic_view_impl() ENTT_NOEXCEPT
        : pools{},
          filter{}
    {}

    /**
     * @brief Constructs a single-type view from a storage class.
     * @param ref The storage for the type to iterate.
     */
    basic_view_impl(storage_type &ref) ENTT_NOEXCEPT
        : pools{&ref},
          filter{}
    {}

    /**
     * @brief Returns the number of entities that have the given component.
     * @return Number of entities that have the given component.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return std::get<0>(pools)->size();
    }

    /**
     * @brief Checks whether a view is empty.
     * @return True if the view is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return std::get<0>(pools)->empty();
    }

    /**
     * @brief Direct access to the raw representation offered by the storage.
     * @return A pointer to the array of components.
     */
    [[nodiscard]] auto raw() const ENTT_NOEXCEPT {
        return std::get<0>(pools)->raw();
    }

    /**
     * @brief Direct access to the list of entities.
     *
     * The returned pointer is such that range `[data(), data() + size())` is
     * always a valid range, even if the container is empty.
     *
     * @return A pointer to the array of entities.
     */
    [[nodiscard]] auto data() const ENTT_NOEXCEPT {
        return std::get<0>(pools)->data();
    }

    /**
     * @brief Returns an iterator to the first entity of the view.
     *
     * The returned iterator points to the first entity of the view. If the view
     * is empty, the returned iterator will be equal to `end()`.
     *
     * @return An iterator to the first entity of the view.
     */
    [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
        return std::get<0>(pools)->basic_common_type::begin();
    }

    /**
     * @brief Returns an iterator that is past the last entity of the view.
     *
     * The returned iterator points to the entity following the last entity of
     * the view. Attempting to dereference the returned iterator results in
     * undefined behavior.
     *
     * @return An iterator to the entity following the last entity of the view.
     */
    [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
        return std::get<0>(pools)->basic_common_type::end();
    }

    /**
     * @brief Returns an iterator to the first entity of the reversed view.
     *
     * The returned iterator points to the first entity of the reversed view. If
     * the view is empty, the returned iterator will be equal to `rend()`.
     *
     * @return An iterator to the first entity of the reversed view.
     */
    [[nodiscard]] reverse_iterator rbegin() const ENTT_NOEXCEPT {
        return std::get<0>(pools)->basic_common_type::rbegin();
    }

    /**
     * @brief Returns an iterator that is past the last entity of the reversed
     * view.
     *
     * The returned iterator points to the entity following the last entity of
     * the reversed view. Attempting to dereference the returned iterator
     * results in undefined behavior.
     *
     * @return An iterator to the entity following the last entity of the
     * reversed view.
     */
    [[nodiscard]] reverse_iterator rend() const ENTT_NOEXCEPT {
        return std::get<0>(pools)->basic_common_type::rend();
    }

    /**
     * @brief Returns the first entity of the view, if any.
     * @return The first entity of the view if one exists, the null entity
     * otherwise.
     */
    [[nodiscard]] entity_type front() const {
        const auto it = begin();
        return it != end() ? *it : null;
    }

    /**
     * @brief Returns the last entity of the view, if any.
     * @return The last entity of the view if one exists, the null entity
     * otherwise.
     */
    [[nodiscard]] entity_type back() const {
        const auto it = rbegin();
        return it != rend() ? *it : null;
    }

    /**
     * @brief Finds an entity.
     * @param entt A valid entity identifier.
     * @return An iterator to the given entity if it's found, past the end
     * iterator otherwise.
     */
    [[nodiscard]] iterator find(const entity_type entt) const {
        const auto it = std::get<0>(pools)->find(entt);
        return it != end() && *it == entt ? it : end();
    }

    /**
     * @brief Returns the identifier that occupies the given position.
     * @param pos Position of the element to return.
     * @return The identifier that occupies the given position.
     */
    [[nodiscard]] entity_type operator[](const size_type pos) const {
        return begin()[pos];
    }

    /**
     * @brief Checks if a view is properly initialized.
     * @return True if the view is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return std::get<0>(pools) != nullptr;
    }

    /**
     * @brief Checks if a view contains an entity.
     * @param entt A valid entity identifier.
     * @return True if the view contains the given entity, false otherwise.
     */
    [[nodiscard]] bool contains(const entity_type entt) const {
        return std::get<0>(pools)->contains(entt);
    }

    /**
     * @brief Returns the component assigned to the given entity.
     *
     * Prefer this function instead of `registry::get` during iterations. It has
     * far better performance than its counterpart.
     *
     * @warning
     * Attempting to use an invalid component type results in a compilation
     * error. Attempting to use an entity that doesn't belong to the view
     * results in undefined behavior.
     *
     * @tparam Comp Types of components to get.
     * @param entt A valid entity identifier.
     * @return The component assigned to the entity.
     */
    template<typename... Comp>
    [[nodiscard]] decltype(auto) get(const entity_type entt) const {
        ENTT_ASSERT(contains(entt), "View does not contain entity");

        if constexpr(sizeof...(Comp) == 0) {
            return get_as_tuple(*std::get<0>(pools), entt);
        } else {
            static_assert(std::is_same_v<Comp..., Component>, "Invalid component type");
            return std::get<0>(pools)->get(entt);
        }
    }

    /**
     * @brief Iterates entities and components and applies the given function
     * object to them.
     *
     * The function object is invoked for each entity. It is provided with the
     * entity itself and a reference to the component if it's a non-empty one.
     * The _constness_ of the component is as requested.<br/>
     * The signature of the function must be equivalent to one of the following
     * forms:
     *
     * @code{.cpp}
     * void(const entity_type, Component &);
     * void(Component &);
     * @endcode
     *
     * @note
     * Empty types aren't explicitly instantiated and therefore they are never
     * returned during iterations.
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void each(Func func) const {
        if constexpr(std::is_void_v<decltype(std::get<0>(pools)->get({}))>) {
            if constexpr(std::is_invocable_v<Func>) {
                for(auto pos = size(); pos; --pos) {
                    func();
                }
            } else {
                for(auto entity: *this) {
                    func(entity);
                }
            }
        } else {
            if constexpr(is_applicable_v<Func, decltype(*each().begin())>) {
                for(const auto pack: each()) {
                    std::apply(func, pack);
                }
            } else {
                for(auto &&component: *std::get<0>(pools)) {
                    func(component);
                }
            }
        }
    }

    /**
     * @brief Returns an iterable object to use to _visit_ the view.
     *
     * The iterable object returns tuples that contain the current entity and a
     * reference to its component if it's a non-empty one. The _constness_ of
     * the component is as requested.
     *
     * @note
     * Empty types aren't explicitly instantiated and therefore they are never
     * returned during iterations.
     *
     * @return An iterable object to use to _visit_ the view.
     */
    [[nodiscard]] iterable_view each() const ENTT_NOEXCEPT {
        return iterable_view{*std::get<0>(pools)};
    }

    /**
     * @brief Combines two views in a _more specific_ one (friend function).
     * @tparam Id A valid entity type (see entt_traits for more details).
     * @tparam ELhs Filter list of the first view.
     * @tparam CLhs Component list of the first view.
     * @tparam ERhs Filter list of the second view.
     * @tparam CRhs Component list of the second view.
     * @return A more specific view.
     */
    template<typename Id, typename... ELhs, typename... CLhs, typename... ERhs, typename... CRhs>
    friend auto operator|(const basic_view<Id, exclude_t<ELhs...>, CLhs...> &, const basic_view<Id, exclude_t<ERhs...>, CRhs...> &);

private:
    const std::tuple<storage_type *> pools;
    const std::tuple<> filter;
};


/**
 * @brief View implementation dispatcher.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Exclude Types of components used to filter the view.
 * @tparam Component Types of components iterated by the view.
 */
template<typename Entity, typename... Exclude, typename... Component>
struct basic_view<Entity, exclude_t<Exclude...>, Component...>
    : basic_view_impl<std::conditional_t<std::disjunction_v<typename component_traits<std::remove_const_t<Component>>::in_place_delete...>, stable_storage_policy, packed_storage_policy>, Entity, exclude_t<Exclude...>, Component...>
{
    /*! @brief Most restrictive storage policy of all component types. */
    using storage_policy = std::conditional_t<std::disjunction_v<typename component_traits<std::remove_const_t<Component>>::in_place_delete...>, stable_storage_policy, packed_storage_policy>;
    using basic_view_impl<storage_policy, Entity, exclude_t<Exclude...>, Component...>::basic_view_impl;
};


/**
 * @brief Deduction guide.
 * @tparam Storage Type of storage classes used to create the view.
 * @param storage The storage for the types to iterate.
 */
template<typename... Storage>
basic_view(Storage &... storage)
-> basic_view<std::common_type_t<typename Storage::entity_type...>, entt::exclude_t<>, constness_as_t<typename Storage::value_type, Storage>...>;


/**
 * @brief Combines two views in a _more specific_ one.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam ELhs Filter list of the first view.
 * @tparam CLhs Component list of the first view.
 * @tparam ERhs Filter list of the second view.
 * @tparam CRhs Component list of the second view.
 * @param lhs A valid reference to the first view.
 * @param rhs A valid reference to the second view.
 * @return A more specific view.
 */
template<typename Entity, typename... ELhs, typename... CLhs, typename... ERhs, typename... CRhs>
[[nodiscard]] auto operator|(const basic_view<Entity, exclude_t<ELhs...>, CLhs...> &lhs, const basic_view<Entity, exclude_t<ERhs...>, CRhs...> &rhs) {
    using view_type = basic_view<Entity, exclude_t<ELhs..., ERhs...>, CLhs..., CRhs...>;
    return std::apply([](auto *... storage) { return view_type{*storage...}; }, std::tuple_cat(lhs.pools, rhs.pools, lhs.filter, rhs.filter));
}


}


#endif



namespace entt {


/**
 * @brief Fast and reliable entity-component system.
 *
 * The registry is the core class of the entity-component framework.<br/>
 * It stores entities and arranges pools of components on a per request basis.
 * By means of a registry, users can manage entities and components, then create
 * views or groups to iterate them.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
class basic_registry {
    using traits_type = entt_traits<Entity>;
    using poly_storage_type = typename poly_storage_traits<Entity>::storage_type;

    template<typename Component>
    using storage_type = constness_as_t<typename storage_traits<Entity, std::remove_const_t<Component>>::storage_type, Component>;

    struct pool_data {
        poly_storage_type poly;
        std::unique_ptr<basic_sparse_set<Entity>> pool{};
    };

    template<typename...>
    struct group_handler;

    template<typename... Exclude, typename... Get, typename... Owned>
    struct group_handler<exclude_t<Exclude...>, get_t<Get...>, Owned...> {
        static_assert(!std::disjunction_v<typename component_traits<Owned>::in_place_delete...>, "Groups do not support in-place delete");
        static_assert(std::conjunction_v<std::is_same<Owned, std::remove_const_t<Owned>>..., std::is_same<Get, std::remove_const_t<Get>>..., std::is_same<Exclude, std::remove_const_t<Exclude>>...>, "One or more component types are invalid");
        std::conditional_t<sizeof...(Owned) == 0, basic_sparse_set<Entity>, std::size_t> current{};

        template<typename Component>
        void maybe_valid_if(basic_registry &owner, const Entity entt) {
            [[maybe_unused]] const auto cpools = std::make_tuple(owner.assure<Owned>()...);

            const auto is_valid = ((std::is_same_v<Component, Owned> || std::get<storage_type<Owned> *>(cpools)->contains(entt)) && ...)
                    && ((std::is_same_v<Component, Get> || owner.assure<Get>()->contains(entt)) && ...)
                    && ((std::is_same_v<Component, Exclude> || !owner.assure<Exclude>()->contains(entt)) && ...);

            if constexpr(sizeof...(Owned) == 0) {
                if(is_valid && !current.contains(entt)) {
                    current.emplace(entt);
                }
            } else {
                if(is_valid && !(std::get<0>(cpools)->index(entt) < current)) {
                    const auto pos = current++;
                    (std::get<storage_type<Owned> *>(cpools)->swap(std::get<storage_type<Owned> *>(cpools)->data()[pos], entt), ...);
                }
            }
        }

        void discard_if([[maybe_unused]] basic_registry &owner, const Entity entt) {
            if constexpr(sizeof...(Owned) == 0) {
                current.remove(entt);
            } else {
                if(const auto cpools = std::make_tuple(owner.assure<Owned>()...); std::get<0>(cpools)->contains(entt) && (std::get<0>(cpools)->index(entt) < current)) {
                    const auto pos = --current;
                    (std::get<storage_type<Owned> *>(cpools)->swap(std::get<storage_type<Owned> *>(cpools)->data()[pos], entt), ...);
                }
            }
        }
    };

    struct group_data {
        std::size_t size;
        std::unique_ptr<void, void(*)(void *)> group;
        bool (* owned)(const id_type) ENTT_NOEXCEPT;
        bool (* get)(const id_type) ENTT_NOEXCEPT;
        bool (* exclude)(const id_type) ENTT_NOEXCEPT;
    };

    template<typename Component>
    [[nodiscard]] storage_type<Component> * assure() const {
        static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types not allowed");
        const auto index = type_seq<Component>::value();

        if(!(index < pools.size())) {
            pools.resize(size_type(index)+1u);
        }

        if(auto &&pdata = pools[index]; !pdata.pool) {
            pdata.pool.reset(new storage_type<Component>());
            pdata.poly.template emplace<storage_type<Component> &>(*static_cast<storage_type<Component> *>(pdata.pool.get()));
        }

        return static_cast<storage_type<Component> *>(pools[index].pool.get());
    }

    template<typename Component>
    [[nodiscard]] const storage_type<Component> * pool_if_exists() const ENTT_NOEXCEPT {
        static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types not allowed");
        const auto index = type_seq<Component>::value();
        return (!(index < pools.size()) || !pools[index].pool) ? nullptr : static_cast<const storage_type<Component> *>(pools[index].pool.get());
    }

    auto generate_identifier(const std::size_t pos) ENTT_NOEXCEPT {
        ENTT_ASSERT(pos < traits_type::to_integral(null), "No entities available");
        return traits_type::construct(static_cast<typename traits_type::entity_type>(pos), {});
    }

    auto recycle_identifier() ENTT_NOEXCEPT {
        ENTT_ASSERT(free_list != null, "No entities available");
        const auto curr = traits_type::to_entity(free_list);
        free_list = (tombstone | entities[curr]);
        return (entities[curr] = traits_type::construct(curr, traits_type::to_version(entities[curr])));
    }

    auto release_entity(const Entity entity, const typename traits_type::version_type version) {
        const auto entt = traits_type::to_entity(entity);
        entities[entt] = traits_type::construct(traits_type::to_entity(free_list), version + (version == traits_type::to_version(tombstone)));
        free_list = (tombstone | entity);
        return traits_type::to_version(entities[entt]);
    }

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Underlying version type. */
    using version_type = typename traits_type::version_type;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Poly storage type. */
    using poly_storage = typename poly_storage_traits<Entity>::storage_type;

    /**
     * @brief Returns the entity identifier without the version.
     * @param entity An entity identifier, either valid or not.
     * @return The entity identifier without the version.
     */
    [[nodiscard]] static entity_type entity(const entity_type entity) ENTT_NOEXCEPT {
        return traits_type::construct(traits_type::to_entity(entity), {});
    }

    /**
     * @brief Returns the version stored along with an entity identifier.
     * @param entity An entity identifier, either valid or not.
     * @return The version stored along with the given entity identifier.
     */
    [[nodiscard]] static version_type version(const entity_type entity) ENTT_NOEXCEPT {
        return traits_type::to_version(entity);
    }

    /*! @brief Default constructor. */
    basic_registry() = default;

    /*! @brief Default move constructor. */
    basic_registry(basic_registry &&) = default;

    /*! @brief Default move assignment operator. @return This registry. */
    basic_registry & operator=(basic_registry &&) = default;

    /**
     * @brief Prepares a pool for the given type if required.
     * @tparam Component Type of component for which to prepare a pool.
     */
    template<typename Component>
    void prepare() {
        // suppress the warning due to the [[nodiscard]] attribute
        static_cast<void>(assure<Component>());
    }

    /**
     * @brief Returns a poly storage for a given type.
     * @param info The type for which to return a poly storage.
     * @return A valid poly storage if a pool for the given type exists, an
     * empty and thus invalid element otherwise.
     */
    poly_storage & storage(const type_info info) {
        ENTT_ASSERT(info.seq() < pools.size() && pools[info.seq()].poly, "Storage not available");
        return pools[info.seq()].poly;
    }

    /*! @copydoc storage */
    const poly_storage & storage(const type_info info) const {
        ENTT_ASSERT(info.seq() < pools.size() && pools[info.seq()].poly, "Storage not available");
        return pools[info.seq()].poly;
    }

    /**
     * @brief Returns the number of existing components of the given type.
     * @tparam Component Type of component of which to return the size.
     * @return Number of existing components of the given type.
     */
    template<typename Component>
    [[nodiscard]] size_type size() const {
        const auto *cpool = pool_if_exists<Component>();
        return cpool ? cpool->size() : size_type{};
    }

    /**
     * @brief Returns the number of entities created so far.
     * @return Number of entities created so far.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return entities.size();
    }

    /**
     * @brief Returns the number of entities still in use.
     * @return Number of entities still in use.
     */
    [[nodiscard]] size_type alive() const {
        auto sz = entities.size();

        for(auto curr = free_list; curr != null; --sz) {
            curr = entities[traits_type::to_entity(curr)];
        }

        return sz;
    }

    /**
     * @brief Increases the capacity of the registry or of the pools for the
     * given components.
     *
     * If no components are specified, the capacity of the registry is
     * increased, that is the number of entities it contains. Otherwise the
     * capacity of the pools for the given components is increased.<br/>
     * In both cases, if the new capacity is greater than the current capacity,
     * new storage is allocated, otherwise the method does nothing.
     *
     * @tparam Component Types of components for which to reserve storage.
     * @param cap Desired capacity.
     */
    template<typename... Component>
    void reserve(const size_type cap) {
        if constexpr(sizeof...(Component) == 0) {
            entities.reserve(cap);
        } else {
            (assure<Component>()->reserve(cap), ...);
        }
    }

    /**
     * @brief Reserves enough space to store `count` pools.
     * @param count Number of pools to reserve space for.
     */
    [[deprecated("No longer supported")]]
    void reserve_pools(const size_t count) {
        pools.reserve(count);
    }

    /**
     * @brief Returns the capacity of the pool for the given component.
     * @tparam Component Type of component in which one is interested.
     * @return Capacity of the pool of the given component.
     */
    template<typename Component>
    [[nodiscard]] size_type capacity() const {
        const auto *cpool = pool_if_exists<Component>();
        return cpool ? cpool->capacity() : size_type{};
    }

    /**
     * @brief Returns the number of entities that a registry has currently
     * allocated space for.
     * @return Capacity of the registry.
     */
    [[nodiscard]] size_type capacity() const ENTT_NOEXCEPT {
        return entities.capacity();
    }

    /**
     * @brief Requests the removal of unused capacity for the given components.
     * @tparam Component Types of components for which to reclaim unused
     * capacity.
     */
    template<typename... Component>
    void shrink_to_fit() {
        (assure<Component>()->shrink_to_fit(), ...);
    }

    /**
     * @brief Checks whether the registry or the pools of the given components
     * are empty.
     *
     * A registry is considered empty when it doesn't contain entities that are
     * still in use.
     *
     * @tparam Component Types of components in which one is interested.
     * @return True if the registry or the pools of the given components are
     * empty, false otherwise.
     */
    template<typename... Component>
    [[nodiscard]] bool empty() const {
        if constexpr(sizeof...(Component) == 0) {
            return !alive();
        } else {
            return [](const auto *... cpool) { return ((!cpool || cpool->empty()) && ...); }(pool_if_exists<Component>()...);
        }
    }

    /**
     * @brief Direct access to the list of entities of a registry.
     *
     * The returned pointer is such that range `[data(), data() + size())` is
     * always a valid range, even if the container is empty.
     *
     * @warning
     * This list contains both valid and destroyed entities and isn't suitable
     * for direct use.
     *
     * @return A pointer to the array of entities.
     */
    [[nodiscard]] const entity_type * data() const ENTT_NOEXCEPT {
        return entities.data();
    }

    /**
     * @brief Returns the head of the list of destroyed entities.
     *
     * This function is intended for use in conjunction with `assign`.<br/>
     * The returned entity has an invalid identifier in all cases.
     *
     * @return The head of the list of destroyed entities.
     */
    [[nodiscard]] entity_type destroyed() const ENTT_NOEXCEPT {
        return free_list;
    }

    /**
     * @brief Checks if an entity identifier refers to a valid entity.
     * @param entity An entity identifier, either valid or not.
     * @return True if the identifier is valid, false otherwise.
     */
    [[nodiscard]] bool valid(const entity_type entity) const {
        const auto pos = size_type(traits_type::to_entity(entity));
        return (pos < entities.size() && entities[pos] == entity);
    }

    /**
     * @brief Returns the actual version for an entity identifier.
     *
     * @warning
     * Attempting to use an entity that doesn't belong to the registry results
     * in undefined behavior. An entity belongs to the registry even if it has
     * been previously destroyed and/or recycled.
     *
     * @param entity A valid entity identifier.
     * @return Actual version for the given entity identifier.
     */
    [[nodiscard]] version_type current(const entity_type entity) const {
        const auto pos = size_type(traits_type::to_entity(entity));
        ENTT_ASSERT(pos < entities.size(), "Entity does not exist");
        return version(entities[pos]);
    }

    /**
     * @brief Creates a new entity and returns it.
     *
     * There are two kinds of possible entity identifiers:
     *
     * * Newly created ones in case no entities have been previously destroyed.
     * * Recycled ones with updated versions.
     *
     * @return A valid entity identifier.
     */
    [[nodiscard]] entity_type create() {
        return (free_list == null) ? entities.emplace_back(generate_identifier(entities.size())) : recycle_identifier();
    }

    /**
     * @brief Creates a new entity and returns it.
     *
     * @sa create
     *
     * If the requested entity isn't in use, the suggested identifier is created
     * and returned. Otherwise, a new identifier is generated.
     *
     * @param hint Required entity identifier.
     * @return A valid entity identifier.
     */
    [[nodiscard]] entity_type create(const entity_type hint) {
        const auto length = entities.size();

        if(hint == null || hint == tombstone) {
            return create();
        } else if(const auto req = traits_type::to_entity(hint); !(req < length)) {
            entities.resize(size_type(req) + 1u, null);

            for(auto pos = length; pos < req; ++pos) {
                release_entity(generate_identifier(pos), {});
            }

            return (entities[req] = hint);
        } else if(const auto curr = traits_type::to_entity(entities[req]); req == curr) {
            return create();
        } else {
            auto *it = &free_list;
            for(; traits_type::to_entity(*it) != req; it = &entities[traits_type::to_entity(*it)]);
            *it = traits_type::construct(curr, traits_type::to_version(*it));
            return (entities[req] = hint);
        }
    }

    /**
     * @brief Assigns each element in a range an entity.
     *
     * @sa create
     *
     * @tparam It Type of forward iterator.
     * @param first An iterator to the first element of the range to generate.
     * @param last An iterator past the last element of the range to generate.
     */
    template<typename It>
    void create(It first, It last) {
        for(; free_list != null && first != last; ++first) {
            *first = recycle_identifier();
        }

        const auto length = entities.size();
        entities.resize(length + std::distance(first, last), null);

        for(auto pos = length; first != last; ++first, ++pos) {
            *first = entities[pos] = generate_identifier(pos);
        }
    }

    /**
     * @brief Assigns entities to an empty registry.
     *
     * This function is intended for use in conjunction with `data`, `size` and
     * `destroyed`.<br/>
     * Don't try to inject ranges of randomly generated entities nor the _wrong_
     * head for the list of destroyed entities. There is no guarantee that a
     * registry will continue to work properly in this case.
     *
     * @warning
     * There must be no entities still alive for this to work properly.
     *
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @param destroyed The head of the list of destroyed entities.
     */
    template<typename It>
    void assign(It first, It last, const entity_type destroyed) {
        ENTT_ASSERT(!alive(), "Entities still alive");
        entities.assign(first, last);
        free_list = destroyed;
    }

    /**
     * @brief Destroys an entity.
     *
     * The version is updated and the identifier can be recycled at any time.
     *
     * @warning
     * Adding or removing components to an entity that is being destroyed can
     * result in undefined behavior.
     *
     * @warning
     * Attempting to use an invalid entity results in undefined behavior.
     *
     * @param entity A valid entity identifier.
     * @return The version of the recycled entity.
     */
    version_type destroy(const entity_type entity) {
        return destroy(entity, traits_type::to_version(entity) + 1u);
    }

    /**
     * @brief Destroys an entity.
     *
     * The suggested version or the valid version closest to the suggested one
     * is used instead of the implicitly generated version.
     *
     * @sa destroy
     *
     * @param entity A valid entity identifier.
     * @param version A desired version upon destruction.
     * @return The version actually assigned to the entity.
     */
    version_type destroy(const entity_type entity, const version_type version) {
        ENTT_ASSERT(valid(entity), "Invalid entity");

        for(auto pos = pools.size(); pos; --pos) {
            pools[pos-1].pool && pools[pos-1].pool->remove(entity, this);
        }

        return release_entity(entity, version);
    }

    /**
     * @brief Destroys all the entities in a range.
     *
     * @sa destroy
     *
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     */
    template<typename It>
    void destroy(It first, It last) {
        for(; first != last; ++first) {
            destroy(*first, version(*first) + 1u);
        }
    }

    /**
     * @brief Assigns the given component to an entity.
     *
     * A new instance of the given component is created and initialized with the
     * arguments provided (the component must have a proper constructor or be of
     * aggregate type). Then the component is assigned to the given entity.
     *
     * @warning
     * Attempting to use an invalid entity or to assign a component to an entity
     * that already owns it results in undefined behavior.
     *
     * @tparam Component Type of component to create.
     * @tparam Args Types of arguments to use to construct the component.
     * @param entity A valid entity identifier.
     * @param args Parameters to use to initialize the component.
     * @return A reference to the newly created component.
     */
    template<typename Component, typename... Args>
    decltype(auto) emplace(const entity_type entity, Args &&... args) {
        ENTT_ASSERT(valid(entity), "Invalid entity");
        return assure<Component>()->emplace(*this, entity, std::forward<Args>(args)...);
    }

    /**
     * @brief Assigns each entity in a range the given component.
     *
     * @sa emplace
     *
     * @tparam Component Type of component to create.
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @param value An instance of the component to assign.
     */
    template<typename Component, typename It>
    void insert(It first, It last, const Component &value = {}) {
        ENTT_ASSERT(std::all_of(first, last, [this](const auto entity) { return valid(entity); }), "Invalid entity");
        assure<Component>()->insert(*this, first, last, value);
    }

    /**
     * @brief Assigns each entity in a range the given components.
     *
     * @sa emplace
     *
     * @tparam Component Type of component to create.
     * @tparam EIt Type of input iterator.
     * @tparam CIt Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @param from An iterator to the first element of the range of components.
     */
    template<typename Component, typename EIt, typename CIt, typename = std::enable_if_t<std::is_same_v<std::decay_t<typename std::iterator_traits<CIt>::value_type>, Component>>>
    void insert(EIt first, EIt last, CIt from) {
        static_assert(std::is_constructible_v<Component, typename std::iterator_traits<CIt>::value_type>, "Invalid value type");
        ENTT_ASSERT(std::all_of(first, last, [this](const auto entity) { return valid(entity); }), "Invalid entity");
        assure<Component>()->insert(*this, first, last, from);
    }

    /**
     * @brief Assigns or replaces the given component for an entity.
     *
     * Equivalent to the following snippet (pseudocode):
     *
     * @code{.cpp}
     * auto &component = registry.all_of<Component>(entity) ? registry.replace<Component>(entity, args...) : registry.emplace<Component>(entity, args...);
     * @endcode
     *
     * Prefer this function anyway because it has slightly better performance.
     *
     * @warning
     * Attempting to use an invalid entity results in undefined behavior.
     *
     * @tparam Component Type of component to assign or replace.
     * @tparam Args Types of arguments to use to construct the component.
     * @param entity A valid entity identifier.
     * @param args Parameters to use to initialize the component.
     * @return A reference to the newly created component.
     */
    template<typename Component, typename... Args>
    decltype(auto) emplace_or_replace(const entity_type entity, Args &&... args) {
        ENTT_ASSERT(valid(entity), "Invalid entity");
        auto *cpool = assure<Component>();

        return cpool->contains(entity)
            ? cpool->patch(*this, entity, [&args...](auto &... curr) { ((curr = Component{std::forward<Args>(args)...}), ...); })
            : cpool->emplace(*this, entity, std::forward<Args>(args)...);
    }

    /**
     * @brief Patches the given component for an entity.
     *
     * The signature of the functions should be equivalent to the following:
     *
     * @code{.cpp}
     * void(Component &);
     * @endcode
     *
     * @note
     * Empty types aren't explicitly instantiated and therefore they are never
     * returned. However, this function can be used to trigger an update signal
     * for them.
     *
     * @warning
     * Attempting to use an invalid entity or to patch a component of an entity
     * that doesn't own it results in undefined behavior.
     *
     * @tparam Component Type of component to patch.
     * @tparam Func Types of the function objects to invoke.
     * @param entity A valid entity identifier.
     * @param func Valid function objects.
     * @return A reference to the patched component.
     */
    template<typename Component, typename... Func>
    decltype(auto) patch(const entity_type entity, Func &&... func) {
        ENTT_ASSERT(valid(entity), "Invalid entity");
        return assure<Component>()->patch(*this, entity, std::forward<Func>(func)...);
    }

    /**
     * @brief Replaces the given component for an entity.
     *
     * A new instance of the given component is created and initialized with the
     * arguments provided (the component must have a proper constructor or be of
     * aggregate type). Then the component is assigned to the given entity.
     *
     * @warning
     * Attempting to use an invalid entity or to replace a component of an
     * entity that doesn't own it results in undefined behavior.
     *
     * @tparam Component Type of component to replace.
     * @tparam Args Types of arguments to use to construct the component.
     * @param entity A valid entity identifier.
     * @param args Parameters to use to initialize the component.
     * @return A reference to the component being replaced.
     */
    template<typename Component, typename... Args>
    decltype(auto) replace(const entity_type entity, Args &&... args) {
        return assure<Component>()->patch(*this, entity, [&args...](auto &... curr) { ((curr = Component{std::forward<Args>(args)...}), ...); });
    }

    /**
     * @brief Removes the given components from an entity.
     *
     * @warning
     * Attempting to use an invalid entity results in undefined behavior.
     *
     * @tparam Component Types of components to remove.
     * @param entity A valid entity identifier.
     * @return The number of components actually removed.
     */
    template<typename... Component>
    size_type remove(const entity_type entity) {
        ENTT_ASSERT(valid(entity), "Invalid entity");
        static_assert(sizeof...(Component) > 0, "Provide one or more component types");
        return (assure<Component>()->remove(entity, this) + ... + size_type{});
    }

    /**
     * @brief Removes the given components from all the entities in a range.
     *
     * @sa remove
     *
     * @tparam Component Types of components to remove.
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     * @return The number of components actually removed.
     */
    template<typename... Component, typename It>
    size_type remove(It first, It last) {
        size_type count{};

        for(; first != last; ++first) {
            count += remove<Component...>(*first);
        }

        return count;
    }

    /**
     * @brief Erases the given components from an entity.
     *
     * @warning
     * Attempting to use an invalid entity or to erase a component from an
     * entity that doesn't own it results in undefined behavior.
     *
     * @tparam Component Types of components to erase.
     * @param entity A valid entity identifier.
     */
    template<typename... Component>
    void erase(const entity_type entity) {
        ENTT_ASSERT(valid(entity), "Invalid entity");
        static_assert(sizeof...(Component) > 0, "Provide one or more component types");
        (assure<Component>()->erase(entity, this), ...);
    }

    /**
     * @brief Erases the given components from all the entities in a range.
     *
     * @sa erase
     *
     * @tparam Component Types of components to erase.
     * @tparam It Type of input iterator.
     * @param first An iterator to the first element of the range of entities.
     * @param last An iterator past the last element of the range of entities.
     */
    template<typename... Component, typename It>
    void erase(It first, It last) {
        for(; first != last; ++first) {
            erase<Component...>(*first);
        }
    }

    /**
     * @brief Removes all tombstones from a registry or only the pools for the
     * given components.
     * @tparam Component Types of components for which to clear all tombstones.
     */
    template<typename... Component>
    void compact() {
        if constexpr(sizeof...(Component) == 0) {
            for(auto pos = pools.size(); pos; --pos) {
                if(auto &pdata = pools[pos-1]; pdata.pool) {
                    pdata.pool->compact();
                }
            }
        } else {
            (assure<Component>()->compact(), ...);
        }
    }

    /*! @copydoc remove */
    template<typename... Component>
    [[deprecated("Use ::remove instead")]]
    size_type remove_if_exists(const entity_type entity) {
        return remove<Component...>(entity);
    }

    /**
     * @brief Removes all the components from an entity and makes it orphaned.
     *
     * @warning
     * In case there are listeners that observe the destruction of components
     * and assign other components to the entity in their bodies, the result of
     * invoking this function may not be as expected. In the worst case, it
     * could lead to undefined behavior.
     *
     * @warning
     * Attempting to use an invalid entity results in undefined behavior.
     *
     * @param entity A valid entity identifier.
     */
    [[deprecated("Use ::destroy(entity)/::create(entity) instead")]]
    void remove_all(const entity_type entity) {
        ENTT_ASSERT(valid(entity), "Invalid entity");

        for(auto pos = pools.size(); pos; --pos) {
            pools[pos-1].pool && pools[pos-1].pool->remove(entity, this);
        }
    }

    /**
     * @brief Checks if an entity has all the given components.
     *
     * @warning
     * Attempting to use an invalid entity results in undefined behavior.
     *
     * @tparam Component Components for which to perform the check.
     * @param entity A valid entity identifier.
     * @return True if the entity has all the components, false otherwise.
     */
    template<typename... Component>
    [[nodiscard]] bool all_of(const entity_type entity) const {
        ENTT_ASSERT(valid(entity), "Invalid entity");
        return [entity](const auto *... cpool) { return ((cpool && cpool->contains(entity)) && ...); }(pool_if_exists<Component>()...);
    }

    /**
     * @brief Checks if an entity has at least one of the given components.
     *
     * @warning
     * Attempting to use an invalid entity results in undefined behavior.
     *
     * @tparam Component Components for which to perform the check.
     * @param entity A valid entity identifier.
     * @return True if the entity has at least one of the given components,
     * false otherwise.
     */
    template<typename... Component>
    [[nodiscard]] bool any_of(const entity_type entity) const {
        ENTT_ASSERT(valid(entity), "Invalid entity");
        return [entity](const auto *... cpool) { return !((!cpool || !cpool->contains(entity)) && ...); }(pool_if_exists<Component>()...);
    }

    /**
     * @brief Returns references to the given components for an entity.
     *
     * @warning
     * Attempting to use an invalid entity or to get a component from an entity
     * that doesn't own it results in undefined behavior.
     *
     * @tparam Component Types of components to get.
     * @param entity A valid entity identifier.
     * @return References to the components owned by the entity.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) get([[maybe_unused]] const entity_type entity) const {
        ENTT_ASSERT(valid(entity), "Invalid entity");

        if constexpr(sizeof...(Component) == 1) {
            const auto *cpool = pool_if_exists<std::remove_const_t<Component>...>();
            ENTT_ASSERT(cpool, "Storage not available");
            return cpool->get(entity);
        } else {
            return std::forward_as_tuple(get<Component>(entity)...);
        }
    }

    /*! @copydoc get */
    template<typename... Component>
    [[nodiscard]] decltype(auto) get([[maybe_unused]] const entity_type entity) {
        ENTT_ASSERT(valid(entity), "Invalid entity");

        if constexpr(sizeof...(Component) == 1) {
            return (const_cast<Component &>(assure<std::remove_const_t<Component>>()->get(entity)), ...);
        } else {
            return std::forward_as_tuple(get<Component>(entity)...);
        }
    }

    /**
     * @brief Returns a reference to the given component for an entity.
     *
     * In case the entity doesn't own the component, the parameters provided are
     * used to construct it.<br/>
     * Equivalent to the following snippet (pseudocode):
     *
     * @code{.cpp}
     * auto &component = registry.all_of<Component>(entity) ? registry.get<Component>(entity) : registry.emplace<Component>(entity, args...);
     * @endcode
     *
     * Prefer this function anyway because it has slightly better performance.
     *
     * @warning
     * Attempting to use an invalid entity results in undefined behavior.
     *
     * @tparam Component Type of component to get.
     * @tparam Args Types of arguments to use to construct the component.
     * @param entity A valid entity identifier.
     * @param args Parameters to use to initialize the component.
     * @return Reference to the component owned by the entity.
     */
    template<typename Component, typename... Args>
    [[nodiscard]] decltype(auto) get_or_emplace(const entity_type entity, Args &&... args) {
        ENTT_ASSERT(valid(entity), "Invalid entity");
        auto *cpool = assure<Component>();
        return cpool->contains(entity) ? cpool->get(entity) : cpool->emplace(*this, entity, std::forward<Args>(args)...);
    }

    /**
     * @brief Returns pointers to the given components for an entity.
     *
     * @warning
     * Attempting to use an invalid entity results in undefined behavior.
     *
     * @note
     * The registry retains ownership of the pointed-to components.
     *
     * @tparam Component Types of components to get.
     * @param entity A valid entity identifier.
     * @return Pointers to the components owned by the entity.
     */
    template<typename... Component>
    [[nodiscard]] auto try_get([[maybe_unused]] const entity_type entity) const {
        ENTT_ASSERT(valid(entity), "Invalid entity");

        if constexpr(sizeof...(Component) == 1) {
            const auto *cpool = pool_if_exists<std::remove_const_t<Component>...>();
            return (cpool && cpool->contains(entity)) ? &cpool->get(entity) : nullptr;
        } else {
            return std::make_tuple(try_get<Component>(entity)...);
        }
    }

    /*! @copydoc try_get */
    template<typename... Component>
    [[nodiscard]] auto try_get([[maybe_unused]] const entity_type entity) {
        ENTT_ASSERT(valid(entity), "Invalid entity");

        if constexpr(sizeof...(Component) == 1) {
            return (const_cast<Component *>(std::as_const(*this).template try_get<Component>(entity)), ...);
        } else {
            return std::make_tuple(try_get<Component>(entity)...);
        }
    }

    /**
     * @brief Clears a whole registry or the pools for the given components.
     * @tparam Component Types of components to remove from their entities.
     */
    template<typename... Component>
    void clear() {
        if constexpr(sizeof...(Component) == 0) {
            for(auto pos = pools.size(); pos; --pos) {
                if(auto &pdata = pools[pos-1]; pdata.pool) {
                    pdata.pool->clear(this);
                }
            }

            each([this](const auto entity) { release_entity(entity, version(entity) + 1u); });
        } else {
            (assure<Component>()->clear(this), ...);
        }
    }

    /**
     * @brief Iterates all the entities that are still in use.
     *
     * The function object is invoked for each entity that is still in use.<br/>
     * The signature of the function should be equivalent to the following:
     *
     * @code{.cpp}
     * void(const Entity);
     * @endcode
     *
     * This function is fairly slow and should not be used frequently. However,
     * it's useful for iterating all the entities still in use, regardless of
     * their components.
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void each(Func func) const {
        if(free_list == null) {
            for(auto pos = entities.size(); pos; --pos) {
                func(entities[pos-1]);
            }
        } else {
            for(auto pos = entities.size(); pos; --pos) {
                if(const auto entity = entities[pos - 1]; traits_type::to_entity(entity) == (pos - 1)) {
                    func(entity);
                }
            }
        }
    }

    /**
     * @brief Checks if an entity has components assigned.
     * @param entity A valid entity identifier.
     * @return True if the entity has no components assigned, false otherwise.
     */
    [[nodiscard]] bool orphan(const entity_type entity) const {
        ENTT_ASSERT(valid(entity), "Invalid entity");
        return std::none_of(pools.cbegin(), pools.cend(), [entity](auto &&pdata) { return pdata.pool && pdata.pool->contains(entity); });
    }

    /**
     * @brief Iterates orphans and applies them the given function object.
     *
     * The function object is invoked for each entity that is still in use and
     * has no components assigned.<br/>
     * The signature of the function should be equivalent to the following:
     *
     * @code{.cpp}
     * void(const Entity);
     * @endcode
     *
     * This function can be very slow and should not be used frequently.
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void orphans(Func func) const {
        each([this, &func](const auto entity) {
            if(orphan(entity)) {
                func(entity);
            }
        });
    }

    /**
     * @brief Returns a sink object for the given component.
     *
     * The sink returned by this function can be used to receive notifications
     * whenever a new instance of the given component is created and assigned to
     * an entity.<br/>
     * The function type for a listener is equivalent to:
     *
     * @code{.cpp}
     * void(basic_registry<Entity> &, Entity);
     * @endcode
     *
     * Listeners are invoked **after** the component has been assigned to the
     * entity.
     *
     * @sa sink
     *
     * @tparam Component Type of component of which to get the sink.
     * @return A temporary sink object.
     */
    template<typename Component>
    [[nodiscard]] auto on_construct() {
        return assure<Component>()->on_construct();
    }

    /**
     * @brief Returns a sink object for the given component.
     *
     * The sink returned by this function can be used to receive notifications
     * whenever an instance of the given component is explicitly updated.<br/>
     * The function type for a listener is equivalent to:
     *
     * @code{.cpp}
     * void(basic_registry<Entity> &, Entity);
     * @endcode
     *
     * Listeners are invoked **after** the component has been updated.
     *
     * @sa sink
     *
     * @tparam Component Type of component of which to get the sink.
     * @return A temporary sink object.
     */
    template<typename Component>
    [[nodiscard]] auto on_update() {
        return assure<Component>()->on_update();
    }

    /**
     * @brief Returns a sink object for the given component.
     *
     * The sink returned by this function can be used to receive notifications
     * whenever an instance of the given component is removed from an entity and
     * thus destroyed.<br/>
     * The function type for a listener is equivalent to:
     *
     * @code{.cpp}
     * void(basic_registry<Entity> &, Entity);
     * @endcode
     *
     * Listeners are invoked **before** the component has been removed from the
     * entity.
     *
     * @sa sink
     *
     * @tparam Component Type of component of which to get the sink.
     * @return A temporary sink object.
     */
    template<typename Component>
    [[nodiscard]] auto on_destroy() {
        return assure<Component>()->on_destroy();
    }

    /**
     * @brief Returns a view for the given components.
     *
     * This kind of objects are created on the fly and share with the registry
     * its internal data structures.<br/>
     * Feel free to discard a view after the use. Creating and destroying a view
     * is an incredibly cheap operation because they do not require any type of
     * initialization.<br/>
     * As a rule of thumb, storing a view should never be an option.
     *
     * Views do their best to iterate the smallest set of candidate entities.
     * In particular:
     *
     * * Single component views are incredibly fast and iterate a packed array
     *   of entities, all of which has the given component.
     * * Multi component views look at the number of entities available for each
     *   component and pick up a reference to the smallest set of candidates to
     *   test for the given components.
     *
     * Views in no way affect the functionalities of the registry nor those of
     * the underlying pools.
     *
     * @note
     * Multi component views are pretty fast. However their performance tend to
     * degenerate when the number of components to iterate grows up and the most
     * of the entities have all the given components.<br/>
     * To get a performance boost, consider using a group instead.
     *
     * @tparam Component Type of components used to construct the view.
     * @tparam Exclude Types of components used to filter the view.
     * @return A newly created view.
     */
    template<typename... Component, typename... Exclude>
    [[nodiscard]] basic_view<Entity, exclude_t<Exclude...>, Component...> view(exclude_t<Exclude...> = {}) const {
        static_assert(sizeof...(Component) > 0, "Exclusion-only views are not supported");
        static_assert((std::is_const_v<Component> && ...), "Invalid non-const type");
        return { *assure<std::remove_const_t<Component>>()..., *assure<Exclude>()... };
    }

    /*! @copydoc view */
    template<typename... Component, typename... Exclude>
    [[nodiscard]] basic_view<Entity, exclude_t<Exclude...>, Component...> view(exclude_t<Exclude...> = {}) {
        static_assert(sizeof...(Component) > 0, "Exclusion-only views are not supported");
        return { *assure<std::remove_const_t<Component>>()..., *assure<Exclude>()... };
    }

    /**
     * @brief Returns a runtime view for the given components.
     *
     * This kind of objects are created on the fly and share with the registry
     * its internal data structures.<br/>
     * Users should throw away the view after use. Fortunately, creating and
     * destroying a runtime view is an incredibly cheap operation because they
     * do not require any type of initialization.<br/>
     * As a rule of thumb, storing a view should never be an option.
     *
     * Runtime views are to be used when users want to construct a view from
     * some external inputs and don't know at compile-time what are the required
     * components.
     *
     * @tparam ItComp Type of input iterator for the components to use to
     * construct the view.
     * @tparam ItExcl Type of input iterator for the components to use to filter
     * the view.
     * @param first An iterator to the first element of the range of components
     * to use to construct the view.
     * @param last An iterator past the last element of the range of components
     * to use to construct the view.
     * @param from An iterator to the first element of the range of components
     * to use to filter the view.
     * @param to An iterator past the last element of the range of components to
     * use to filter the view.
     * @return A newly created runtime view.
     */
    template<typename ItComp, typename ItExcl = id_type *>
    [[nodiscard]] basic_runtime_view<Entity> runtime_view(ItComp first, ItComp last, ItExcl from = {}, ItExcl to = {}) const {
        std::vector<const basic_sparse_set<Entity> *> component(std::distance(first, last));
        std::vector<const basic_sparse_set<Entity> *> filter(std::distance(from, to));

        std::transform(first, last, component.begin(), [this](const auto ctype) {
            const auto it = std::find_if(pools.cbegin(), pools.cend(), [ctype](auto &&pdata) { return pdata.poly && pdata.poly->value_type().hash() == ctype; });
            return it == pools.cend() ? nullptr : it->pool.get();
        });

        std::transform(from, to, filter.begin(), [this](const auto ctype) {
            const auto it = std::find_if(pools.cbegin(), pools.cend(), [ctype](auto &&pdata) { return pdata.poly && pdata.poly->value_type().hash() == ctype; });
            return it == pools.cend() ? nullptr : it->pool.get();
        });

        return { std::move(component), std::move(filter) };
    }

    /**
     * @brief Returns a group for the given components.
     *
     * This kind of objects are created on the fly and share with the registry
     * its internal data structures.<br/>
     * Feel free to discard a group after the use. Creating and destroying a
     * group is an incredibly cheap operation because they do not require any
     * type of initialization, but for the first time they are requested.<br/>
     * As a rule of thumb, storing a group should never be an option.
     *
     * Groups support exclusion lists and can own types of components. The more
     * types are owned by a group, the faster it is to iterate entities and
     * components.<br/>
     * However, groups also affect some features of the registry such as the
     * creation and destruction of components, which will consequently be
     * slightly slower (nothing that can be noticed in most cases).
     *
     * @note
     * Pools of components that are owned by a group cannot be sorted anymore.
     * The group takes the ownership of the pools and arrange components so as
     * to iterate them as fast as possible.
     *
     * @tparam Owned Types of components owned by the group.
     * @tparam Get Types of components observed by the group.
     * @tparam Exclude Types of components used to filter the group.
     * @return A newly created group.
     */
    template<typename... Owned, typename... Get, typename... Exclude>
    [[nodiscard]] basic_group<Entity, exclude_t<Exclude...>, get_t<Get...>, Owned...> group(get_t<Get...>, exclude_t<Exclude...> = {}) {
        static_assert(sizeof...(Owned) + sizeof...(Get) > 0, "Exclusion-only groups are not supported");
        static_assert(sizeof...(Owned) + sizeof...(Get) + sizeof...(Exclude) > 1, "Single component groups are not allowed");

        using handler_type = group_handler<exclude_t<Exclude...>, get_t<std::remove_const_t<Get>...>, std::remove_const_t<Owned>...>;

        const auto cpools = std::make_tuple(assure<std::remove_const_t<Owned>>()..., assure<std::remove_const_t<Get>>()...);
        constexpr auto size = sizeof...(Owned) + sizeof...(Get) + sizeof...(Exclude);
        handler_type *handler = nullptr;

        if(auto it = std::find_if(groups.cbegin(), groups.cend(), [size](const auto &gdata) {
            return gdata.size == size
                && (gdata.owned(type_hash<std::remove_const_t<Owned>>::value()) && ...)
                && (gdata.get(type_hash<std::remove_const_t<Get>>::value()) && ...)
                && (gdata.exclude(type_hash<Exclude>::value()) && ...);
        }); it != groups.cend())
        {
            handler = static_cast<handler_type *>(it->group.get());
        }

        if(!handler) {
            group_data candidate = {
                size,
                { new handler_type{}, [](void *instance) { delete static_cast<handler_type *>(instance); } },
                []([[maybe_unused]] const id_type ctype) ENTT_NOEXCEPT { return ((ctype == type_hash<std::remove_const_t<Owned>>::value()) || ...); },
                []([[maybe_unused]] const id_type ctype) ENTT_NOEXCEPT { return ((ctype == type_hash<std::remove_const_t<Get>>::value()) || ...); },
                []([[maybe_unused]] const id_type ctype) ENTT_NOEXCEPT { return ((ctype == type_hash<Exclude>::value()) || ...); },
            };

            handler = static_cast<handler_type *>(candidate.group.get());

            const void *maybe_valid_if = nullptr;
            const void *discard_if = nullptr;

            if constexpr(sizeof...(Owned) == 0) {
                groups.push_back(std::move(candidate));
            } else {
                ENTT_ASSERT(std::all_of(groups.cbegin(), groups.cend(), [size](const auto &gdata) {
                    const auto overlapping = (0u + ... + gdata.owned(type_hash<std::remove_const_t<Owned>>::value()));
                    const auto sz = overlapping + (0u + ... + gdata.get(type_hash<std::remove_const_t<Get>>::value())) + (0u + ... + gdata.exclude(type_hash<Exclude>::value()));
                    return !overlapping || ((sz == size) || (sz == gdata.size));
                }), "Conflicting groups");

                const auto next = std::find_if_not(groups.cbegin(), groups.cend(), [size](const auto &gdata) {
                    return !(0u + ... + gdata.owned(type_hash<std::remove_const_t<Owned>>::value())) || (size > gdata.size);
                });

                const auto prev = std::find_if(std::make_reverse_iterator(next), groups.crend(), [](const auto &gdata) {
                    return (0u + ... + gdata.owned(type_hash<std::remove_const_t<Owned>>::value()));
                });

                maybe_valid_if = (next == groups.cend() ? maybe_valid_if : next->group.get());
                discard_if = (prev == groups.crend() ? discard_if : prev->group.get());
                groups.insert(next, std::move(candidate));
            }

            (on_construct<std::remove_const_t<Owned>>().before(maybe_valid_if).template connect<&handler_type::template maybe_valid_if<std::remove_const_t<Owned>>>(*handler), ...);
            (on_construct<std::remove_const_t<Get>>().before(maybe_valid_if).template connect<&handler_type::template maybe_valid_if<std::remove_const_t<Get>>>(*handler), ...);
            (on_destroy<Exclude>().before(maybe_valid_if).template connect<&handler_type::template maybe_valid_if<Exclude>>(*handler), ...);

            (on_destroy<std::remove_const_t<Owned>>().before(discard_if).template connect<&handler_type::discard_if>(*handler), ...);
            (on_destroy<std::remove_const_t<Get>>().before(discard_if).template connect<&handler_type::discard_if>(*handler), ...);
            (on_construct<Exclude>().before(discard_if).template connect<&handler_type::discard_if>(*handler), ...);

            if constexpr(sizeof...(Owned) == 0) {
                for(const auto entity: view<Owned..., Get...>(exclude<Exclude...>)) {
                    handler->current.emplace(entity);
                }
            } else {
                // we cannot iterate backwards because we want to leave behind valid entities in case of owned types
                for(auto *first = std::get<0>(cpools)->data(), *last = first + std::get<0>(cpools)->size(); first != last; ++first) {
                    handler->template maybe_valid_if<type_list_element_t<0, type_list<std::remove_const_t<Owned>...>>>(*this, *first);
                }
            }
        }

        return { handler->current, *std::get<storage_type<std::remove_const_t<Owned>> *>(cpools)..., *std::get<storage_type<std::remove_const_t<Get>> *>(cpools)... };
    }

    /**
     * @brief Returns a group for the given components.
     *
     * @sa group
     *
     * @tparam Owned Types of components owned by the group.
     * @tparam Get Types of components observed by the group.
     * @tparam Exclude Types of components used to filter the group.
     * @return A newly created group.
     */
    template<typename... Owned, typename... Get, typename... Exclude>
    [[nodiscard]] basic_group<Entity, exclude_t<Exclude...>, get_t<Get...>, Owned...> group_if_exists(get_t<Get...>, exclude_t<Exclude...> = {}) const {
        static_assert(std::conjunction_v<std::is_const<Owned>..., std::is_const<Get>...>, "Invalid non-const type");

        if(auto it = std::find_if(groups.cbegin(), groups.cend(), [](const auto &gdata) {
            return gdata.size == (sizeof...(Owned) + sizeof...(Get) + sizeof...(Exclude))
                && (gdata.owned(type_hash<std::remove_const_t<Owned>>::value()) && ...)
                && (gdata.get(type_hash<std::remove_const_t<Get>>::value()) && ...)
                && (gdata.exclude(type_hash<Exclude>::value()) && ...);
            }); it == groups.cend())
        {
            return {};
        } else {
            using handler_type = group_handler<exclude_t<Exclude...>, get_t<std::remove_const_t<Get>...>, std::remove_const_t<Owned>...>;
            return { static_cast<handler_type *>(it->group.get())->current, *pool_if_exists<std::remove_const_t<Owned>>()... , *pool_if_exists<std::remove_const_t<Get>>()... };
        }
    }

    /**
     * @brief Returns a group for the given components.
     *
     * @sa group
     *
     * @tparam Owned Types of components owned by the group.
     * @tparam Exclude Types of components used to filter the group.
     * @return A newly created group.
     */
    template<typename... Owned, typename... Exclude>
    [[nodiscard]] basic_group<Entity, exclude_t<Exclude...>, get_t<>, Owned...> group(exclude_t<Exclude...> = {}) {
        return group<Owned...>(get_t<>{}, exclude<Exclude...>);
    }

    /**
     * @brief Returns a group for the given components.
     *
     * @sa group_if_exists
     *
     * @tparam Owned Types of components owned by the group.
     * @tparam Exclude Types of components used to filter the group.
     * @return A newly created group.
     */
    template<typename... Owned, typename... Exclude>
    [[nodiscard]] basic_group<Entity, exclude_t<Exclude...>, get_t<>, Owned...> group_if_exists(exclude_t<Exclude...> = {}) const {
        return group_if_exists<Owned...>(get_t<>{}, exclude<Exclude...>);
    }

    /**
     * @brief Checks whether the given components belong to any group.
     * @tparam Component Types of components in which one is interested.
     * @return True if the pools of the given components are sortable, false
     * otherwise.
     */
    template<typename... Component>
    [[nodiscard]] bool sortable() const {
        return std::none_of(groups.cbegin(), groups.cend(), [](auto &&gdata) { return (gdata.owned(type_hash<std::remove_const_t<Component>>::value()) || ...); });
    }

    /**
     * @brief Checks whether a group can be sorted.
     * @tparam Owned Types of components owned by the group.
     * @tparam Get Types of components observed by the group.
     * @tparam Exclude Types of components used to filter the group.
     * @return True if the group can be sorted, false otherwise.
     */
    template<typename... Owned, typename... Get, typename... Exclude>
    [[nodiscard]] bool sortable(const basic_group<Entity, exclude_t<Exclude...>, get_t<Get...>, Owned...> &) ENTT_NOEXCEPT {
        constexpr auto size = sizeof...(Owned) + sizeof...(Get) + sizeof...(Exclude);
        return std::find_if(groups.cbegin(), groups.cend(), [size](const auto &gdata) {
            return (0u + ... + gdata.owned(type_hash<std::remove_const_t<Owned>>::value())) && (size < gdata.size);
        }) == groups.cend();
    }

    /**
     * @brief Sorts the pool of entities for the given component.
     *
     * The order of the elements in a pool is highly affected by assignments
     * of components to entities and deletions. Components are arranged to
     * maximize the performance during iterations and users should not make any
     * assumption on the order.<br/>
     * This function can be used to impose an order to the elements in the pool
     * of the given component. The order is kept valid until a component of the
     * given type is assigned or removed from an entity.
     *
     * The comparison function object must return `true` if the first element
     * is _less_ than the second one, `false` otherwise. The signature of the
     * comparison function should be equivalent to one of the following:
     *
     * @code{.cpp}
     * bool(const Entity, const Entity);
     * bool(const Component &, const Component &);
     * @endcode
     *
     * Moreover, the comparison function object shall induce a
     * _strict weak ordering_ on the values.
     *
     * The sort function oject must offer a member function template
     * `operator()` that accepts three arguments:
     *
     * * An iterator to the first element of the range to sort.
     * * An iterator past the last element of the range to sort.
     * * A comparison function to use to compare the elements.
     *
     * The comparison funtion object received by the sort function object hasn't
     * necessarily the type of the one passed along with the other parameters to
     * this member function.
     *
     * @warning
     * Pools of components owned by a group cannot be sorted.
     *
     * @tparam Component Type of components to sort.
     * @tparam Compare Type of comparison function object.
     * @tparam Sort Type of sort function object.
     * @tparam Args Types of arguments to forward to the sort function object.
     * @param compare A valid comparison function object.
     * @param algo A valid sort function object.
     * @param args Arguments to forward to the sort function object, if any.
     */
    template<typename Component, typename Compare, typename Sort = std_sort, typename... Args>
    void sort(Compare compare, Sort algo = Sort{}, Args &&... args) {
        ENTT_ASSERT(sortable<Component>(), "Cannot sort owned storage");
        assure<Component>()->sort(std::move(compare), std::move(algo), std::forward<Args>(args)...);
    }

    /**
     * @brief Sorts two pools of components in the same way.
     *
     * The order of the elements in a pool is highly affected by assignments
     * of components to entities and deletions. Components are arranged to
     * maximize the performance during iterations and users should not make any
     * assumption on the order.
     *
     * It happens that different pools of components must be sorted the same way
     * because of runtime and/or performance constraints. This function can be
     * used to order a pool of components according to the order between the
     * entities in another pool of components.
     *
     * @b How @b it @b works
     *
     * Being `A` and `B` the two sets where `B` is the master (the one the order
     * of which rules) and `A` is the slave (the one to sort), after a call to
     * this function an iterator for `A` will return the entities according to
     * the following rules:
     *
     * * All the entities in `A` that are also in `B` are returned first
     *   according to the order they have in `B`.
     * * All the entities in `A` that are not in `B` are returned in no
     *   particular order after all the other entities.
     *
     * Any subsequent change to `B` won't affect the order in `A`.
     *
     * @warning
     * Pools of components owned by a group cannot be sorted.
     *
     * @tparam To Type of components to sort.
     * @tparam From Type of components to use to sort.
     */
    template<typename To, typename From>
    void sort() {
        ENTT_ASSERT(sortable<To>(), "Cannot sort owned storage");
        assure<To>()->respect(*assure<From>());
    }

    /**
     * @brief Visits an entity and returns the type info for its components.
     *
     * The signature of the function should be equivalent to the following:
     *
     * @code{.cpp}
     * void(const type_info);
     * @endcode
     *
     * Returned identifiers are those of the components owned by the entity.
     *
     * @sa type_info
     *
     * @warning
     * It's not specified whether a component attached to or removed from the
     * given entity during the visit is returned or not to the caller.
     *
     * @tparam Func Type of the function object to invoke.
     * @param entity A valid entity identifier.
     * @param func A valid function object.
     */
    template<typename Func>
    void visit(entity_type entity, Func func) const {
        for(auto pos = pools.size(); pos; --pos) {
            if(const auto &pdata = pools[pos-1]; pdata.pool && pdata.pool->contains(entity)) {
                func(pdata.poly->value_type());
            }
        }
    }

    /**
     * @brief Visits a registry and returns the type info for its components.
     *
     * The signature of the function should be equivalent to the following:
     *
     * @code{.cpp}
     * void(const type_info);
     * @endcode
     *
     * Returned identifiers are those of the components managed by the registry.
     *
     * @sa type_info
     *
     * @warning
     * It's not specified whether a component for which a pool is created during
     * the visit is returned or not to the caller.
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void visit(Func func) const {
        for(auto pos = pools.size(); pos; --pos) {
            if(const auto &pdata = pools[pos-1]; pdata.pool) {
                func(pdata.poly->value_type());
            }
        }
    }

    /**
     * @brief Binds an object to the context of the registry.
     *
     * If the value already exists it is overwritten, otherwise a new instance
     * of the given type is created and initialized with the arguments provided.
     *
     * @tparam Type Type of object to set.
     * @tparam Args Types of arguments to use to construct the object.
     * @param args Parameters to use to initialize the value.
     * @return A reference to the newly created object.
     */
    template<typename Type, typename... Args>
    Type & set(Args &&... args) {
        unset<Type>();
        vars.emplace_back(std::in_place_type<Type>, std::forward<Args>(args)...);
        return any_cast<Type &>(vars.back());
    }

    /**
     * @brief Unsets a context variable if it exists.
     * @tparam Type Type of object to set.
     */
    template<typename Type>
    void unset() {
        vars.erase(std::remove_if(vars.begin(), vars.end(), [type = type_id<Type>()](auto &&var) { return var.type() == type; }), vars.end());
    }

    /**
     * @brief Binds an object to the context of the registry.
     *
     * In case the context doesn't contain the given object, the parameters
     * provided are used to construct it.
     *
     * @tparam Type Type of object to set.
     * @tparam Args Types of arguments to use to construct the object.
     * @param args Parameters to use to initialize the object.
     * @return A reference to the object in the context of the registry.
     */
    template<typename Type, typename... Args>
    [[nodiscard]] Type & ctx_or_set(Args &&... args) {
        auto *value = try_ctx<Type>();
        return value ? *value : set<Type>(std::forward<Args>(args)...);
    }

    /**
     * @brief Returns a pointer to an object in the context of the registry.
     * @tparam Type Type of object to get.
     * @return A pointer to the object if it exists in the context of the
     * registry, a null pointer otherwise.
     */
    template<typename Type>
    [[nodiscard]] Type * try_ctx() const {
        auto it = std::find_if(vars.cbegin(), vars.cend(), [type = type_id<Type>()](auto &&var) { return var.type() == type; });
        return it == vars.cend() ? nullptr : any_cast<Type>(&*it);
    }

    /*! @copydoc try_ctx */
    template<typename Type>
    [[nodiscard]] Type * try_ctx() {
        auto it = std::find_if(vars.begin(), vars.end(), [type = type_id<Type>()](auto &&var) { return var.type() == type; });
        return it == vars.end() ? nullptr : any_cast<Type>(&*it);
    }

    /**
     * @brief Returns a reference to an object in the context of the registry.
     *
     * @warning
     * Attempting to get a context variable that doesn't exist results in
     * undefined behavior.
     *
     * @tparam Type Type of object to get.
     * @return A valid reference to the object in the context of the registry.
     */
    template<typename Type>
    [[nodiscard]] Type & ctx() const {
        auto it = std::find_if(vars.cbegin(), vars.cend(), [type = type_id<Type>()](auto &&var) { return var.type() == type; });
        ENTT_ASSERT(it != vars.cend(), "Invalid instance");
        return any_cast<Type &>(*it);
    }

    /*! @copydoc ctx */
    template<typename Type>
    [[nodiscard]] Type & ctx() {
        auto it = std::find_if(vars.begin(), vars.end(), [type = type_id<Type>()](auto &&var) { return var.type() == type; });
        ENTT_ASSERT(it != vars.end(), "Invalid instance");
        return any_cast<Type &>(*it);
    }

    /**
     * @brief Visits a registry and returns the type info for its context
     * variables.
     *
     * The signature of the function should be equivalent to the following:
     *
     * @code{.cpp}
     * void(const type_info);
     * @endcode
     *
     * Returned identifiers are those of the context variables currently set.
     *
     * @sa type_info
     *
     * @warning
     * It's not specified whether a context variable created during the visit is
     * returned or not to the caller.
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void ctx(Func func) const {
        for(auto pos = vars.size(); pos; --pos) {
            func(vars[pos-1].type());
        }
    }

private:
    std::vector<basic_any<0u>> vars{};
    mutable std::vector<pool_data> pools{};
    std::vector<group_data> groups{};
    std::vector<entity_type> entities{};
    entity_type free_list{tombstone};
};


}


#endif



namespace entt {


/**
 * @brief Non-owning handle to an entity.
 *
 * Tiny wrapper around a registry and an entity.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Type Types to which to restrict the scope of a handle.
 */
template<typename Entity, typename... Type>
struct basic_handle {
    /*! @brief Type of registry accepted by the handle. */
    using registry_type = constness_as_t<basic_registry<std::remove_const_t<Entity>>, Entity>;
    /*! @brief Underlying entity identifier. */
    using entity_type = typename registry_type::entity_type;
    /*! @brief Underlying version type. */
    using version_type = typename registry_type::version_type;
    /*! @brief Unsigned integer type. */
    using size_type = typename registry_type::size_type;

    /*! @brief Constructs an invalid handle. */
    basic_handle() ENTT_NOEXCEPT
        : reg{}, entt{null}
    {}

    /**
     * @brief Constructs a handle from a given registry and entity.
     * @param ref An instance of the registry class.
     * @param value An entity identifier.
     */
    basic_handle(registry_type &ref, entity_type value) ENTT_NOEXCEPT
        : reg{&ref}, entt{value}
    {}

    /**
     * @brief Compares two handles.
     * @tparam Args Template parameters of the handle with which to compare.
     * @param other Handle with which to compare.
     * @return True if both handles refer to the same registry and the same
     * entity, false otherwise.
     */
    template<typename... Args>
    [[nodiscard]] bool operator==(const basic_handle<Args...> &other) const ENTT_NOEXCEPT {
        return reg == other.registry() && entt == other.entity();
    }

    /**
     * @brief Constructs a const handle from a non-const one.
     * @tparam Other A valid entity type (see entt_traits for more details).
     * @tparam Args Scope of the handle to construct.
     * @return A const handle referring to the same registry and the same
     * entity.
     */
    template<typename Other, typename... Args>
    operator basic_handle<Other, Args...>() const ENTT_NOEXCEPT {
        static_assert(
            (std::is_same_v<Other, Entity> || std::is_same_v<std::remove_const_t<Other>, Entity>)
            && (sizeof...(Type) == 0 || ((sizeof...(Args) != 0 && sizeof...(Args) <= sizeof...(Type)) && ... && (type_list_contains_v<type_list<Type...>, Args>))),
            "Invalid conversion between different handles"
        );

        return reg ? basic_handle<Other, Args...>{*reg, entt} : basic_handle<Other, Args...>{};
    }

    /**
     * @brief Converts a handle to its underlying entity.
     * @return An entity identifier.
     */
    [[nodiscard]] operator entity_type() const ENTT_NOEXCEPT {
        return entity();
    }

    /**
     * @brief Checks if a handle refers to non-null registry pointer and entity.
     * @return True if the handle refers to non-null registry and entity, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return reg && reg->valid(entt);
    }

    /**
     * @brief Checks if a handle refers to a valid entity or not.
     * @return True if the handle refers to a valid entity, false otherwise.
     */
    [[nodiscard]] bool valid() const {
        return reg->valid(entt);
    }

    /**
     * @brief Returns a pointer to the underlying registry, if any.
     * @return A pointer to the underlying registry, if any.
     */
    [[nodiscard]] registry_type * registry() const ENTT_NOEXCEPT {
        return reg;
    }

    /**
     * @brief Returns the entity associated with a handle.
     * @return The entity associated with the handle.
     */
    [[nodiscard]] entity_type entity() const ENTT_NOEXCEPT {
        return entt;
    }

    /**
     * @brief Destroys the entity associated with a handle.
     * @sa basic_registry::destroy
     */
    void destroy() {
        reg->destroy(entt);
    }

    /**
     * @brief Destroys the entity associated with a handle.
     * @sa basic_registry::destroy
     * @param version A desired version upon destruction.
     */
    void destroy(const version_type version) {
        reg->destroy(entt, version);
    }

    /**
     * @brief Assigns the given component to a handle.
     * @sa basic_registry::emplace
     * @tparam Component Type of component to create.
     * @tparam Args Types of arguments to use to construct the component.
     * @param args Parameters to use to initialize the component.
     * @return A reference to the newly created component.
     */
    template<typename Component, typename... Args>
    decltype(auto) emplace(Args &&... args) const {
        static_assert(((sizeof...(Type) == 0) || ... || std::is_same_v<Component, Type>), "Invalid type");
        return reg->template emplace<Component>(entt, std::forward<Args>(args)...);
    }

    /**
     * @brief Assigns or replaces the given component for a handle.
     * @sa basic_registry::emplace_or_replace
     * @tparam Component Type of component to assign or replace.
     * @tparam Args Types of arguments to use to construct the component.
     * @param args Parameters to use to initialize the component.
     * @return A reference to the newly created component.
     */
    template<typename Component, typename... Args>
    decltype(auto) emplace_or_replace(Args &&... args) const {
        static_assert(((sizeof...(Type) == 0) || ... || std::is_same_v<Component, Type>), "Invalid type");
        return reg->template emplace_or_replace<Component>(entt, std::forward<Args>(args)...);
    }

    /**
     * @brief Patches the given component for a handle.
     * @sa basic_registry::patch
     * @tparam Component Type of component to patch.
     * @tparam Func Types of the function objects to invoke.
     * @param func Valid function objects.
     * @return A reference to the patched component.
     */
    template<typename Component, typename... Func>
    decltype(auto) patch(Func &&... func) const {
        static_assert(((sizeof...(Type) == 0) || ... || std::is_same_v<Component, Type>), "Invalid type");
        return reg->template patch<Component>(entt, std::forward<Func>(func)...);
    }

    /**
     * @brief Replaces the given component for a handle.
     * @sa basic_registry::replace
     * @tparam Component Type of component to replace.
     * @tparam Args Types of arguments to use to construct the component.
     * @param args Parameters to use to initialize the component.
     * @return A reference to the component being replaced.
     */
    template<typename Component, typename... Args>
    decltype(auto) replace(Args &&... args) const {
        static_assert(((sizeof...(Type) == 0) || ... || std::is_same_v<Component, Type>), "Invalid type");
        return reg->template replace<Component>(entt, std::forward<Args>(args)...);
    }

    /**
     * @brief Removes the given components from a handle.
     * @sa basic_registry::remove
     * @tparam Component Types of components to remove.
     * @return The number of components actually removed.
     */
    template<typename... Component>
    size_type remove() const {
        static_assert(sizeof...(Type) == 0 || (type_list_contains_v<type_list<Type...>, Component> && ...), "Invalid type");
        return reg->template remove<Component...>(entt);
    }

    /**
     * @brief Erases the given components from a handle.
     * @sa basic_registry::erase
     * @tparam Component Types of components to erase.
     */
    template<typename... Component>
    void erase() const {
        static_assert(sizeof...(Type) == 0 || (type_list_contains_v<type_list<Type...>, Component> && ...), "Invalid type");
        reg->template erase<Component...>(entt);
    }

    /*! @copydoc remove */
    template<typename... Component>
    [[deprecated("Use ::remove instead")]]
    size_type remove_if_exists() const {
        return remove<Component...>();
    }

    /**
     * @brief Removes all the components from a handle and makes it orphaned.
     * @sa basic_registry::remove_all
     */
    [[deprecated("No longer supported")]]
    void remove_all() const {
        static_assert(sizeof...(Type) == 0, "Invalid operation");
        reg->remove_all(entt);
    }

    /**
     * @brief Checks if a handle has all the given components.
     * @sa basic_registry::all_of
     * @tparam Component Components for which to perform the check.
     * @return True if the handle has all the components, false otherwise.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) all_of() const {
        return reg->template all_of<Component...>(entt);
    }

    /**
     * @brief Checks if a handle has at least one of the given components.
     * @sa basic_registry::any_of
     * @tparam Component Components for which to perform the check.
     * @return True if the handle has at least one of the given components,
     * false otherwise.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) any_of() const {
        return reg->template any_of<Component...>(entt);
    }

    /**
     * @brief Returns references to the given components for a handle.
     * @sa basic_registry::get
     * @tparam Component Types of components to get.
     * @return References to the components owned by the handle.
     */
    template<typename... Component>
    [[nodiscard]] decltype(auto) get() const {
        static_assert(sizeof...(Type) == 0 || (type_list_contains_v<type_list<Type...>, Component> && ...), "Invalid type");
        return reg->template get<Component...>(entt);
    }

    /**
     * @brief Returns a reference to the given component for a handle.
     * @sa basic_registry::get_or_emplace
     * @tparam Component Type of component to get.
     * @tparam Args Types of arguments to use to construct the component.
     * @param args Parameters to use to initialize the component.
     * @return Reference to the component owned by the handle.
     */
    template<typename Component, typename... Args>
    [[nodiscard]] decltype(auto) get_or_emplace(Args &&... args) const {
        static_assert(((sizeof...(Type) == 0) || ... || std::is_same_v<Component, Type>), "Invalid type");
        return reg->template get_or_emplace<Component>(entt, std::forward<Args>(args)...);
    }

    /**
     * @brief Returns pointers to the given components for a handle.
     * @sa basic_registry::try_get
     * @tparam Component Types of components to get.
     * @return Pointers to the components owned by the handle.
     */
    template<typename... Component>
    [[nodiscard]] auto try_get() const {
        static_assert(sizeof...(Type) == 0 || (type_list_contains_v<type_list<Type...>, Component> && ...), "Invalid type");
        return reg->template try_get<Component...>(entt);
    }

    /**
     * @brief Checks if a handle has components assigned.
     * @return True if the handle has no components assigned, false otherwise.
     */
    [[nodiscard]] bool orphan() const {
        return reg->orphan(entt);
    }

    /**
     * @brief Visits a handle and returns the types for its components.
     * @sa basic_registry::visit
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void visit(Func &&func) const {
        reg->visit(entt, std::forward<Func>(func));
    }

private:
    registry_type *reg;
    entity_type entt;
};


/**
 * @brief Compares two handles.
 * @tparam Type A valid entity type (see entt_traits for more details).
 * @tparam Other A valid entity type (see entt_traits for more details).
 * @param lhs A valid handle.
 * @param rhs A valid handle.
 * @return False if both handles refer to the same registry and the same
 * entity, true otherwise.
 */
template<typename Type, typename Other>
bool operator!=(const basic_handle<Type> &lhs, const basic_handle<Other> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Deduction guide.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
basic_handle(basic_registry<Entity> &, Entity)
-> basic_handle<Entity>;


/**
 * @brief Deduction guide.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
basic_handle(const basic_registry<Entity> &, Entity)
-> basic_handle<const Entity>;


}


#endif

// #include "entity/helper.hpp"
#ifndef ENTT_ENTITY_HELPER_HPP
#define ENTT_ENTITY_HELPER_HPP


#include <type_traits>
// #include "../config/config.h"

// #include "../core/fwd.hpp"

// #include "../core/type_traits.hpp"

// #include "../signal/delegate.hpp"

// #include "registry.hpp"

// #include "fwd.hpp"



namespace entt {


/**
 * @brief Converts a registry to a view.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
struct as_view {
    /*! @brief Underlying entity identifier. */
    using entity_type = std::remove_const_t<Entity>;
    /*! @brief Type of registry to convert. */
    using registry_type = constness_as_t<basic_registry<entity_type>, Entity>;

    /**
     * @brief Constructs a converter for a given registry.
     * @param source A valid reference to a registry.
     */
    as_view(registry_type &source) ENTT_NOEXCEPT: reg{source} {}

    /**
     * @brief Conversion function from a registry to a view.
     * @tparam Exclude Types of components used to filter the view.
     * @tparam Component Type of components used to construct the view.
     * @return A newly created view.
     */
    template<typename Exclude, typename... Component>
    operator basic_view<entity_type, Exclude, Component...>() const {
        return reg.template view<Component...>(Exclude{});
    }

private:
    registry_type &reg;
};


/**
 * @brief Deduction guide.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
as_view(basic_registry<Entity> &) -> as_view<Entity>;


/**
 * @brief Deduction guide.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
as_view(const basic_registry<Entity> &) -> as_view<const Entity>;


/**
 * @brief Converts a registry to a group.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
struct as_group {
    /*! @brief Underlying entity identifier. */
    using entity_type = std::remove_const_t<Entity>;
    /*! @brief Type of registry to convert. */
    using registry_type = constness_as_t<basic_registry<entity_type>, Entity>;

    /**
     * @brief Constructs a converter for a given registry.
     * @param source A valid reference to a registry.
     */
    as_group(registry_type &source) ENTT_NOEXCEPT: reg{source} {}

    /**
     * @brief Conversion function from a registry to a group.
     * @tparam Exclude Types of components used to filter the group.
     * @tparam Get Types of components observed by the group.
     * @tparam Owned Types of components owned by the group.
     * @return A newly created group.
     */
    template<typename Exclude, typename Get, typename... Owned>
    operator basic_group<entity_type, Exclude, Get, Owned...>() const {
        if constexpr(std::is_const_v<registry_type>) {
            return reg.template group_if_exists<Owned...>(Get{}, Exclude{});
        } else {
            return reg.template group<Owned...>(Get{}, Exclude{});
        }
    }

private:
    registry_type &reg;
};


/**
 * @brief Deduction guide.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
as_group(basic_registry<Entity> &) -> as_group<Entity>;


/**
 * @brief Deduction guide.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
as_group(const basic_registry<Entity> &) -> as_group<const Entity>;



/**
 * @brief Helper to create a listener that directly invokes a member function.
 * @tparam Member Member function to invoke on a component of the given type.
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @param reg A registry that contains the given entity and its components.
 * @param entt Entity from which to get the component.
 */
template<auto Member, typename Entity = entity>
void invoke(basic_registry<Entity> &reg, const Entity entt) {
    static_assert(std::is_member_function_pointer_v<decltype(Member)>, "Invalid pointer to non-static member function");
    delegate<void(basic_registry<Entity> &, const Entity)> func;
    func.template connect<Member>(reg.template get<member_class_t<decltype(Member)>>(entt));
    func(reg, entt);
}


/**
 * @brief Returns the entity associated with a given component.
 *
 * @warning
 * Currently, this function only works correctly with the default pool as it
 * makes assumptions about how the components are laid out.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 * @tparam Component Type of component.
 * @param reg A registry that contains the given entity and its components.
 * @param instance A valid component instance.
 * @return The entity associated with the given component.
 */
template<typename Entity, typename Component>
Entity to_entity(const basic_registry<Entity> &reg, const Component &instance) {
    const auto view = reg.template view<const Component>();
    const auto *addr = std::addressof(instance);

    for(auto it = view.rbegin(), last = view.rend(); it < last; it += ENTT_PACKED_PAGE) {
        if(const auto dist = (addr - std::addressof(view.template get<const Component>(*it))); dist >= 0 && dist < ENTT_PACKED_PAGE) {
            return *(it + dist);
        }
    }

    return entt::null;
}


}


#endif

// #include "entity/observer.hpp"
#ifndef ENTT_ENTITY_OBSERVER_HPP
#define ENTT_ENTITY_OBSERVER_HPP


#include <limits>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <type_traits>
// #include "../config/config.h"

// #include "../core/type_traits.hpp"

// #include "../signal/delegate.hpp"

// #include "registry.hpp"

// #include "storage.hpp"

// #include "utility.hpp"

// #include "entity.hpp"

// #include "fwd.hpp"



namespace entt {


/*! @brief Grouping matcher. */
template<typename...>
struct matcher {};


/**
 * @brief Collector.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error, but for a few reasonable cases.
 */
template<typename...>
struct basic_collector;


/**
 * @brief Collector.
 *
 * A collector contains a set of rules (literally, matchers) to use to track
 * entities.<br/>
 * Its main purpose is to generate a descriptor that allows an observer to know
 * how to connect to a registry.
 */
template<>
struct basic_collector<> {
    /**
     * @brief Adds a grouping matcher to the collector.
     * @tparam AllOf Types of components tracked by the matcher.
     * @tparam NoneOf Types of components used to filter out entities.
     * @return The updated collector.
     */
    template<typename... AllOf, typename... NoneOf>
    static constexpr auto group(exclude_t<NoneOf...> = {}) ENTT_NOEXCEPT {
        return basic_collector<matcher<type_list<>, type_list<>, type_list<NoneOf...>, AllOf...>>{};
    }

    /**
     * @brief Adds an observing matcher to the collector.
     * @tparam AnyOf Type of component for which changes should be detected.
     * @return The updated collector.
     */
    template<typename AnyOf>
    static constexpr auto update() ENTT_NOEXCEPT {
        return basic_collector<matcher<type_list<>, type_list<>, AnyOf>>{};
    }
};

/**
 * @brief Collector.
 * @copydetails basic_collector<>
 * @tparam Reject Untracked types used to filter out entities.
 * @tparam Require Untracked types required by the matcher.
 * @tparam Rule Specific details of the current matcher.
 * @tparam Other Other matchers.
 */
template<typename... Reject, typename... Require, typename... Rule, typename... Other>
struct basic_collector<matcher<type_list<Reject...>, type_list<Require...>, Rule...>, Other...> {
    /*! @brief Current matcher. */
    using current_type = matcher<type_list<Reject...>, type_list<Require...>, Rule...>;

    /**
     * @brief Adds a grouping matcher to the collector.
     * @tparam AllOf Types of components tracked by the matcher.
     * @tparam NoneOf Types of components used to filter out entities.
     * @return The updated collector.
     */
    template<typename... AllOf, typename... NoneOf>
    static constexpr auto group(exclude_t<NoneOf...> = {}) ENTT_NOEXCEPT {
        return basic_collector<matcher<type_list<>, type_list<>, type_list<NoneOf...>, AllOf...>, current_type, Other...>{};
    }

    /**
     * @brief Adds an observing matcher to the collector.
     * @tparam AnyOf Type of component for which changes should be detected.
     * @return The updated collector.
     */
    template<typename AnyOf>
    static constexpr auto update() ENTT_NOEXCEPT {
        return basic_collector<matcher<type_list<>, type_list<>, AnyOf>, current_type, Other...>{};
    }

    /**
     * @brief Updates the filter of the last added matcher.
     * @tparam AllOf Types of components required by the matcher.
     * @tparam NoneOf Types of components used to filter out entities.
     * @return The updated collector.
     */
    template<typename... AllOf, typename... NoneOf>
    static constexpr auto where(exclude_t<NoneOf...> = {}) ENTT_NOEXCEPT {
        using extended_type = matcher<type_list<Reject..., NoneOf...>, type_list<Require..., AllOf...>, Rule...>;
        return basic_collector<extended_type, Other...>{};
    }
};


/*! @brief Variable template used to ease the definition of collectors. */
inline constexpr basic_collector<> collector{};


/**
 * @brief Observer.
 *
 * An observer returns all the entities and only the entities that fit the
 * requirements of at least one matcher. Moreover, it's guaranteed that the
 * entity list is tightly packed in memory for fast iterations.<br/>
 * In general, observers don't stay true to the order of any set of components.
 *
 * Observers work mainly with two types of matchers, provided through a
 * collector:
 *
 * * Observing matcher: an observer will return at least all the living entities
 *   for which one or more of the given components have been updated and not yet
 *   destroyed.
 * * Grouping matcher: an observer will return at least all the living entities
 *   that would have entered the given group if it existed and that would have
 *   not yet left it.
 *
 * If an entity respects the requirements of multiple matchers, it will be
 * returned once and only once by the observer in any case.
 *
 * Matchers support also filtering by means of a _where_ clause that accepts
 * both a list of types and an exclusion list.<br/>
 * Whenever a matcher finds that an entity matches its requirements, the
 * condition of the filter is verified before to register the entity itself.
 * Moreover, a registered entity isn't returned by the observer if the condition
 * set by the filter is broken in the meantime.
 *
 * @b Important
 *
 * Iterators aren't invalidated if:
 *
 * * New instances of the given components are created and assigned to entities.
 * * The entity currently pointed is modified (as an example, if one of the
 *   given components is removed from the entity to which the iterator points).
 * * The entity currently pointed is destroyed.
 *
 * In all the other cases, modifying the pools of the given components in any
 * way invalidates all the iterators and using them results in undefined
 * behavior.
 *
 * @warning
 * Lifetime of an observer doesn't necessarily have to overcome that of the
 * registry to which it is connected. However, the observer must be disconnected
 * from the registry before being destroyed to avoid crashes due to dangling
 * pointers.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
class basic_observer {
    using payload_type = std::uint32_t;

    template<typename>
    struct matcher_handler;

    template<typename... Reject, typename... Require, typename AnyOf>
    struct matcher_handler<matcher<type_list<Reject...>, type_list<Require...>, AnyOf>> {
        template<std::size_t Index>
        static void maybe_valid_if(basic_observer &obs, basic_registry<Entity> &reg, const Entity entt) {
            if(reg.template all_of<Require...>(entt) && !reg.template any_of<Reject...>(entt)) {
                if(!obs.storage.contains(entt)) {
                    obs.storage.emplace(entt);
                }

                obs.storage.get(entt) |= (1 << Index);
            }
        }

        template<std::size_t Index>
        static void discard_if(basic_observer &obs, basic_registry<Entity> &, const Entity entt) {
            if(obs.storage.contains(entt) && !(obs.storage.get(entt) &= (~(1 << Index)))) {
                obs.storage.erase(entt);
            }
        }

        template<std::size_t Index>
        static void connect(basic_observer &obs, basic_registry<Entity> &reg) {
            (reg.template on_destroy<Require>().template connect<&discard_if<Index>>(obs), ...);
            (reg.template on_construct<Reject>().template connect<&discard_if<Index>>(obs), ...);
            reg.template on_update<AnyOf>().template connect<&maybe_valid_if<Index>>(obs);
            reg.template on_destroy<AnyOf>().template connect<&discard_if<Index>>(obs);
        }

        static void disconnect(basic_observer &obs, basic_registry<Entity> &reg) {
            (reg.template on_destroy<Require>().disconnect(obs), ...);
            (reg.template on_construct<Reject>().disconnect(obs), ...);
            reg.template on_update<AnyOf>().disconnect(obs);
            reg.template on_destroy<AnyOf>().disconnect(obs);
        }
    };

    template<typename... Reject, typename... Require, typename... NoneOf, typename... AllOf>
    struct matcher_handler<matcher<type_list<Reject...>, type_list<Require...>, type_list<NoneOf...>, AllOf...>> {
        template<std::size_t Index, typename... Ignore>
        static void maybe_valid_if(basic_observer &obs, basic_registry<Entity> &reg, const Entity entt) {
            if([&reg, entt]() {
                if constexpr(sizeof...(Ignore) == 0) {
                    return reg.template all_of<AllOf..., Require...>(entt) && !reg.template any_of<NoneOf..., Reject...>(entt);
                } else {
                    return reg.template all_of<AllOf..., Require...>(entt) && ((std::is_same_v<Ignore..., NoneOf> || !reg.template any_of<NoneOf>(entt)) && ...) && !reg.template any_of<Reject...>(entt);
                }
            }())
            {
                if(!obs.storage.contains(entt)) {
                    obs.storage.emplace(entt);
                }

                obs.storage.get(entt) |= (1 << Index);
            }
        }

        template<std::size_t Index>
        static void discard_if(basic_observer &obs, basic_registry<Entity> &, const Entity entt) {
            if(obs.storage.contains(entt) && !(obs.storage.get(entt) &= (~(1 << Index)))) {
                obs.storage.erase(entt);
            }
        }

        template<std::size_t Index>
        static void connect(basic_observer &obs, basic_registry<Entity> &reg) {
            (reg.template on_destroy<Require>().template connect<&discard_if<Index>>(obs), ...);
            (reg.template on_construct<Reject>().template connect<&discard_if<Index>>(obs), ...);
            (reg.template on_construct<AllOf>().template connect<&maybe_valid_if<Index>>(obs), ...);
            (reg.template on_destroy<NoneOf>().template connect<&maybe_valid_if<Index, NoneOf>>(obs), ...);
            (reg.template on_destroy<AllOf>().template connect<&discard_if<Index>>(obs), ...);
            (reg.template on_construct<NoneOf>().template connect<&discard_if<Index>>(obs), ...);
        }

        static void disconnect(basic_observer &obs, basic_registry<Entity> &reg) {
            (reg.template on_destroy<Require>().disconnect(obs), ...);
            (reg.template on_construct<Reject>().disconnect(obs), ...);
            (reg.template on_construct<AllOf>().disconnect(obs), ...);
            (reg.template on_destroy<NoneOf>().disconnect(obs), ...);
            (reg.template on_destroy<AllOf>().disconnect(obs), ...);
            (reg.template on_construct<NoneOf>().disconnect(obs), ...);
        }
    };

    template<typename... Matcher>
    static void disconnect(basic_registry<Entity> &reg, basic_observer &obs) {
        (matcher_handler<Matcher>::disconnect(obs, reg), ...);
    }

    template<typename... Matcher, std::size_t... Index>
    void connect(basic_registry<Entity> &reg, std::index_sequence<Index...>) {
        static_assert(sizeof...(Matcher) < std::numeric_limits<payload_type>::digits, "Too many matchers");
        (matcher_handler<Matcher>::template connect<Index>(*this, reg), ...);
        release.template connect<&basic_observer::disconnect<Matcher...>>(reg);
    }

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Random access iterator type. */
    using iterator = typename basic_sparse_set<Entity>::iterator;

    /*! @brief Default constructor. */
    basic_observer()
        : release{},
          storage{}
    {}

    /*! @brief Default copy constructor, deleted on purpose. */
    basic_observer(const basic_observer &) = delete;
    /*! @brief Default move constructor, deleted on purpose. */
    basic_observer(basic_observer &&) = delete;

    /**
     * @brief Creates an observer and connects it to a given registry.
     * @tparam Matcher Types of matchers to use to initialize the observer.
     * @param reg A valid reference to a registry.
     */
    template<typename... Matcher>
    basic_observer(basic_registry<entity_type> &reg, basic_collector<Matcher...>)
        : basic_observer{}
    {
        connect<Matcher...>(reg, std::index_sequence_for<Matcher...>{});
    }

    /*! @brief Default destructor. */
    ~basic_observer() = default;

    /**
     * @brief Default copy assignment operator, deleted on purpose.
     * @return This observer.
     */
    basic_observer & operator=(const basic_observer &) = delete;

    /**
     * @brief Default move assignment operator, deleted on purpose.
     * @return This observer.
     */
    basic_observer & operator=(basic_observer &&) = delete;

    /**
     * @brief Connects an observer to a given registry.
     * @tparam Matcher Types of matchers to use to initialize the observer.
     * @param reg A valid reference to a registry.
     */
    template<typename... Matcher>
    void connect(basic_registry<entity_type> &reg, basic_collector<Matcher...>) {
        disconnect();
        connect<Matcher...>(reg, std::index_sequence_for<Matcher...>{});
        storage.clear();
    }

    /*! @brief Disconnects an observer from the registry it keeps track of. */
    void disconnect() {
        if(release) {
            release(*this);
            release.reset();
        }
    }

    /**
     * @brief Returns the number of elements in an observer.
     * @return Number of elements.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return storage.size();
    }

    /**
     * @brief Checks whether an observer is empty.
     * @return True if the observer is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return storage.empty();
    }

    /**
     * @brief Direct access to the list of entities of the observer.
     *
     * The returned pointer is such that range `[data(), data() + size())` is
     * always a valid range, even if the container is empty.
     *
     * @note
     * Entities are in the reverse order as returned by the `begin`/`end`
     * iterators.
     *
     * @return A pointer to the array of entities.
     */
    [[nodiscard]] const entity_type * data() const ENTT_NOEXCEPT {
        return storage.data();
    }

    /**
     * @brief Returns an iterator to the first entity of the observer.
     *
     * The returned iterator points to the first entity of the observer. If the
     * container is empty, the returned iterator will be equal to `end()`.
     *
     * @return An iterator to the first entity of the observer.
     */
    [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
        return storage.basic_sparse_set<entity_type>::begin();
    }

    /**
     * @brief Returns an iterator that is past the last entity of the observer.
     *
     * The returned iterator points to the entity following the last entity of
     * the observer. Attempting to dereference the returned iterator results in
     * undefined behavior.
     *
     * @return An iterator to the entity following the last entity of the
     * observer.
     */
    [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
        return storage.basic_sparse_set<entity_type>::end();
    }

    /*! @brief Clears the underlying container. */
    void clear() ENTT_NOEXCEPT {
        storage.clear();
    }

    /**
     * @brief Iterates entities and applies the given function object to them.
     *
     * The function object is invoked for each entity.<br/>
     * The signature of the function must be equivalent to the following form:
     *
     * @code{.cpp}
     * void(const entity_type);
     * @endcode
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void each(Func func) const {
        for(const auto entity: *this) {
            func(entity);
        }
    }

    /**
     * @brief Iterates entities and applies the given function object to them,
     * then clears the observer.
     *
     * @sa each
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void each(Func func) {
        std::as_const(*this).each(std::move(func));
        clear();
    }

private:
    delegate<void(basic_observer &)> release;
    basic_storage<entity_type, payload_type> storage;
};


}


#endif

// #include "entity/organizer.hpp"
#ifndef ENTT_ENTITY_ORGANIZER_HPP
#define ENTT_ENTITY_ORGANIZER_HPP


#include <cstddef>
#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
// #include "../core/type_info.hpp"

// #include "../core/type_traits.hpp"

// #include "fwd.hpp"

// #include "helper.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
struct is_view: std::false_type {};

template<typename Entity, typename... Exclude, typename... Component>
struct is_view<basic_view<Entity, exclude_t<Exclude...>, Component...>>: std::true_type {};

template<typename Type>
inline constexpr bool is_view_v = is_view<Type>::value;


template<typename Type, typename Override>
struct unpack_type {
    using ro = std::conditional_t<
        type_list_contains_v<Override, std::add_const_t<Type>> || (std::is_const_v<Type> && !type_list_contains_v<Override, std::remove_const_t<Type>>),
        type_list<std::remove_const_t<Type>>,
        type_list<>
    >;

    using rw = std::conditional_t<
        type_list_contains_v<Override, std::remove_const_t<Type>> || (!std::is_const_v<Type> && !type_list_contains_v<Override, std::add_const_t<Type>>),
        type_list<Type>,
        type_list<>
    >;
};

template<typename Entity, typename... Override>
struct unpack_type<basic_registry<Entity>, type_list<Override...>> {
    using ro = type_list<>;
    using rw = type_list<>;
};

template<typename Entity, typename... Override>
struct unpack_type<const basic_registry<Entity>, type_list<Override...>>
    : unpack_type<basic_registry<Entity>, type_list<Override...>>
{};

template<typename Entity, typename... Exclude, typename... Component, typename... Override>
struct unpack_type<basic_view<Entity, exclude_t<Exclude...>, Component...>, type_list<Override...>> {
    using ro = type_list_cat_t<type_list<Exclude...>, typename unpack_type<Component, type_list<Override...>>::ro...>;
    using rw = type_list_cat_t<typename unpack_type<Component, type_list<Override...>>::rw...>;
};

template<typename Entity, typename... Exclude, typename... Component, typename... Override>
struct unpack_type<const basic_view<Entity, exclude_t<Exclude...>, Component...>, type_list<Override...>>
    : unpack_type<basic_view<Entity, exclude_t<Exclude...>, Component...>, type_list<Override...>>
{};


template<typename, typename>
struct resource;

template<typename... Args, typename... Req>
struct resource<type_list<Args...>, type_list<Req...>> {
    using args = type_list<std::remove_const_t<Args>...>;
    using ro = type_list_cat_t<typename unpack_type<Args, type_list<Req...>>::ro..., typename unpack_type<Req, type_list<>>::ro...>;
    using rw = type_list_cat_t<typename unpack_type<Args, type_list<Req...>>::rw..., typename unpack_type<Req, type_list<>>::rw...>;
};


template<typename... Req, typename Ret, typename... Args>
resource<type_list<std::remove_reference_t<Args>...>, type_list<Req...>> free_function_to_resource(Ret(*)(Args...));

template<typename... Req, typename Ret, typename Type, typename... Args>
resource<type_list<std::remove_reference_t<Args>...>, type_list<Req...>> constrained_function_to_resource(Ret(*)(Type &, Args...));

template<typename... Req, typename Ret, typename Class, typename... Args>
resource<type_list<std::remove_reference_t<Args>...>, type_list<Req...>> constrained_function_to_resource(Ret(Class:: *)(Args...));

template<typename... Req, typename Ret, typename Class, typename... Args>
resource<type_list<std::remove_reference_t<Args>...>, type_list<Req...>> constrained_function_to_resource(Ret(Class:: *)(Args...) const);

template<typename... Req>
resource<type_list<>, type_list<Req...>> to_resource();


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Utility class for creating a static task graph.
 *
 * This class offers minimal support (but sufficient in many cases) for creating
 * an execution graph from functions and their requirements on resources.<br/>
 * Note that the resulting tasks aren't executed in any case. This isn't the
 * goal of the tool. Instead, they are returned to the user in the form of a
 * graph that allows for safe execution.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
class basic_organizer final {
    using callback_type = void(const void *, entt::basic_registry<Entity> &);
    using prepare_type = void(entt::basic_registry<Entity> &);
    using dependency_type = std::size_t(const bool, type_info *, const std::size_t);

    struct vertex_data final {
        std::size_t ro_count{};
        std::size_t rw_count{};
        const char *name{};
        const void *payload{};
        callback_type *callback{};
        dependency_type *dependency;
        prepare_type *prepare{};
        type_info info{};
    };

    template<typename Type>
    [[nodiscard]] static decltype(auto) extract(basic_registry<Entity> &reg) {
        if constexpr(std::is_same_v<Type, basic_registry<Entity>>) {
            return reg;
        } else if constexpr(internal::is_view_v<Type>) {
            return as_view{reg};
        } else {
            return reg.template ctx_or_set<std::remove_reference_t<Type>>();
        }
    }

    template<typename... Args>
    [[nodiscard]] static auto to_args(basic_registry<Entity> &reg, type_list<Args...>) {
        return std::tuple<decltype(extract<Args>(reg))...>(extract<Args>(reg)...);
    }

    template<typename... Type>
    static std::size_t fill_dependencies(type_list<Type...>, [[maybe_unused]] type_info *buffer, [[maybe_unused]] const std::size_t count) {
        if constexpr(sizeof...(Type) == 0u) {
            return {};
        } else {
            type_info info[sizeof...(Type)]{type_id<Type>()...};
            const auto length = (std::min)(count, sizeof...(Type));
            std::copy_n(info, length, buffer);
            return length;
        }
    }

    template<typename... RO, typename... RW>
    void track_dependencies(std::size_t index, const bool requires_registry, type_list<RO...>, type_list<RW...>) {
        dependencies[type_hash<basic_registry<Entity>>::value()].emplace_back(index, requires_registry || (sizeof...(RO) + sizeof...(RW) == 0u));
        (dependencies[type_hash<RO>::value()].emplace_back(index, false), ...);
        (dependencies[type_hash<RW>::value()].emplace_back(index, true), ...);
    }

    [[nodiscard]] std::vector<bool> adjacency_matrix() {
        const auto length = vertices.size();
        std::vector<bool> edges(length * length, false);

        // creates the ajacency matrix
        for(const auto &deps: dependencies) {
            const auto last = deps.second.cend();
            auto it = deps.second.cbegin();

            while(it != last) {
                if(it->second) {
                    // rw item
                    if(auto curr = it++; it != last) {
                        if(it->second) {
                            edges[curr->first * length + it->first] = true;
                        } else {
                            if(const auto next = std::find_if(it, last, [](const auto &elem) { return elem.second; }); next != last) {
                                for(; it != next; ++it) {
                                    edges[curr->first * length + it->first] = true;
                                    edges[it->first * length + next->first] = true;
                                }
                            } else {
                                for(; it != next; ++it) {
                                    edges[curr->first * length + it->first] = true;
                                }
                            }
                        }
                    }
                } else {
                    // ro item, possibly only on first iteration
                    if(const auto next = std::find_if(it, last, [](const auto &elem) { return elem.second; }); next != last) {
                        for(; it != next; ++it) {
                            edges[it->first * length + next->first] = true;
                        }
                    } else {
                        it = last;
                    }
                }
            }
        }

        // computes the transitive closure
        for(std::size_t vk{}; vk < length; ++vk) {
            for(std::size_t vi{}; vi < length; ++vi) {
                for(std::size_t vj{}; vj < length; ++vj) {
                    edges[vi * length + vj] = edges[vi * length + vj] || (edges[vi * length + vk] && edges[vk * length + vj]);
                }
            }
        }

        // applies the transitive reduction
        for(std::size_t vert{}; vert < length; ++vert) {
            edges[vert * length + vert] = false;
        }

        for(std::size_t vj{}; vj < length; ++vj) {
            for(std::size_t vi{}; vi < length; ++vi) {
                if(edges[vi * length + vj]) {
                    for(std::size_t vk{}; vk < length; ++vk) {
                        if(edges[vj * length + vk]) {
                            edges[vi * length + vk] = false;
                        }
                    }
                }
            }
        }

        return edges;
    }

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Raw task function type. */
    using function_type = callback_type;

    /*! @brief Vertex type of a task graph defined as an adjacency list. */
    struct vertex {
        /**
         * @brief Constructs a vertex of the task graph.
         * @param vtype True if the vertex is a top-level one, false otherwise.
         * @param data The data associated with the vertex.
         * @param edges The indices of the children in the adjacency list.
         */
        vertex(const bool vtype, vertex_data data, std::vector<std::size_t> edges)
            : is_top_level{vtype},
              node{std::move(data)},
              reachable{std::move(edges)}
        {}

        /**
         * @brief Fills a buffer with the type info objects for the writable
         * resources of a vertex.
         * @param buffer A buffer pre-allocated by the user.
         * @param length The length of the user-supplied buffer.
         * @return The number of type info objects written to the buffer.
         */
        size_type ro_dependency(type_info *buffer, const std::size_t length) const ENTT_NOEXCEPT {
            return node.dependency(false, buffer, length);
        }

        /**
         * @brief Fills a buffer with the type info objects for the read-only
         * resources of a vertex.
         * @param buffer A buffer pre-allocated by the user.
         * @param length The length of the user-supplied buffer.
         * @return The number of type info objects written to the buffer.
         */
        size_type rw_dependency(type_info *buffer, const std::size_t length) const ENTT_NOEXCEPT {
            return node.dependency(true, buffer, length);
        }

        /**
         * @brief Returns the number of read-only resources of a vertex.
         * @return The number of read-only resources of the vertex.
         */
        size_type ro_count() const ENTT_NOEXCEPT {
            return node.ro_count;
        }

        /**
         * @brief Returns the number of writable resources of a vertex.
         * @return The number of writable resources of the vertex.
         */
        size_type rw_count() const ENTT_NOEXCEPT {
            return node.rw_count;
        }

        /**
         * @brief Checks if a vertex is also a top-level one.
         * @return True if the vertex is a top-level one, false otherwise.
         */
        bool top_level() const ENTT_NOEXCEPT {
            return is_top_level;
        }

        /**
         * @brief Returns a type info object associated with a vertex.
         * @return A properly initialized type info object.
         */
        type_info info() const ENTT_NOEXCEPT {
            return node.info;
        }

        /**
         * @brief Returns a user defined name associated with a vertex, if any.
         * @return The user defined name associated with the vertex, if any.
         */
        const char * name() const ENTT_NOEXCEPT {
            return node.name;
        }

        /**
         * @brief Returns the function associated with a vertex.
         * @return The function associated with the vertex.
         */
        function_type * callback() const ENTT_NOEXCEPT {
            return node.callback;
        }

        /**
         * @brief Returns the payload associated with a vertex, if any.
         * @return The payload associated with the vertex, if any.
         */
        const void * data() const ENTT_NOEXCEPT {
            return node.payload;
        }

        /**
         * @brief Returns the list of nodes reachable from a given vertex.
         * @return The list of nodes reachable from the vertex.
         */
        const std::vector<std::size_t> & children() const ENTT_NOEXCEPT {
            return reachable;
        }

        /**
         * @brief Prepares a registry and assures that all required resources
         * are properly instantiated before using them.
         * @param reg A valid registry.
         */
        void prepare(basic_registry<entity_type> &reg) const {
            node.prepare ? node.prepare(reg) : void();
        }

    private:
        bool is_top_level;
        vertex_data node;
        std::vector<std::size_t> reachable;
    };

    /**
     * @brief Adds a free function to the task list.
     * @tparam Candidate Function to add to the task list.
     * @tparam Req Additional requirements and/or override resource access mode.
     * @param name Optional name to associate with the task.
     */
    template<auto Candidate, typename... Req>
    void emplace(const char *name = nullptr) {
        using resource_type = decltype(internal::free_function_to_resource<Req...>(Candidate));
        constexpr auto requires_registry = type_list_contains_v<typename resource_type::args, basic_registry<entity_type>>;

        callback_type *callback = +[](const void *, basic_registry<entity_type> &reg) {
            std::apply(Candidate, to_args(reg, typename resource_type::args{}));
        };

        track_dependencies(vertices.size(), requires_registry, typename resource_type::ro{}, typename resource_type::rw{});

        vertices.push_back({
            resource_type::ro::size,
            resource_type::rw::size,
            name,
            nullptr,
            callback,
            +[](const bool rw, type_info *buffer, const std::size_t length) { return rw ? fill_dependencies(typename resource_type::rw{}, buffer, length) : fill_dependencies(typename resource_type::ro{}, buffer, length); },
            +[](basic_registry<entity_type> &reg) { void(to_args(reg, typename resource_type::args{})); },
            type_id<std::integral_constant<decltype(Candidate), Candidate>>()
        });
    }

    /**
     * @brief Adds a free function with payload or a member function with an
     * instance to the task list.
     * @tparam Candidate Function or member to add to the task list.
     * @tparam Req Additional requirements and/or override resource access mode.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     * @param name Optional name to associate with the task.
     */
    template<auto Candidate, typename... Req, typename Type>
    void emplace(Type &value_or_instance, const char *name = nullptr) {
        using resource_type = decltype(internal::constrained_function_to_resource<Req...>(Candidate));
        constexpr auto requires_registry = type_list_contains_v<typename resource_type::args, basic_registry<entity_type>>;

        callback_type *callback = +[](const void *payload, basic_registry<entity_type> &reg) {
            Type *curr = static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(payload));
            std::apply(Candidate, std::tuple_cat(std::forward_as_tuple(*curr), to_args(reg, typename resource_type::args{})));
        };

        track_dependencies(vertices.size(), requires_registry, typename resource_type::ro{}, typename resource_type::rw{});

        vertices.push_back({
            resource_type::ro::size,
            resource_type::rw::size,
            name,
            &value_or_instance,
            callback,
            +[](const bool rw, type_info *buffer, const std::size_t length) {
                return rw ? fill_dependencies(typename resource_type::rw{}, buffer, length) : fill_dependencies(typename resource_type::ro{}, buffer, length);
            },
            +[](basic_registry<entity_type> &reg) {
                void(to_args(reg, typename resource_type::args{}));
            },
            type_id<std::integral_constant<decltype(Candidate), Candidate>>()
        });
    }

    /**
     * @brief Adds an user defined function with optional payload to the task
     * list.
     * @tparam Req Additional requirements and/or override resource access mode.
     * @param func Function to add to the task list.
     * @param payload User defined arbitrary data.
     * @param name Optional name to associate with the task.
     */
    template<typename... Req>
    void emplace(function_type *func, const void *payload = nullptr, const char *name = nullptr) {
        using resource_type = internal::resource<type_list<>, type_list<Req...>>;
        track_dependencies(vertices.size(), true, typename resource_type::ro{}, typename resource_type::rw{});

        vertices.push_back({
            resource_type::ro::size,
            resource_type::rw::size,
            name,
            payload,
            func,
            +[](const bool rw, type_info *buffer, const std::size_t length) {
                return rw ? fill_dependencies(typename resource_type::rw{}, buffer, length) : fill_dependencies(typename resource_type::ro{}, buffer, length);
            },
            nullptr,
            type_info{}
        });
    }

    /**
     * @brief Generates a task graph for the current content.
     * @return The adjacency list of the task graph.
     */
    std::vector<vertex> graph() {
        const auto edges = adjacency_matrix();

        // creates the adjacency list
        std::vector<vertex> adjacency_list{};
        adjacency_list.reserve(vertices.size());

        for(std::size_t col{}, length = vertices.size(); col < length; ++col) {
            std::vector<std::size_t> reachable{};
            const auto row = col * length;
            bool is_top_level = true;

            for(std::size_t next{}; next < length; ++next) {
                if(edges[row + next]) {
                    reachable.push_back(next);
                }
            }

            for(std::size_t next{}; next < length && is_top_level; ++next) {
                is_top_level = !edges[next * length + col];
            }

            adjacency_list.emplace_back(is_top_level, vertices[col], std::move(reachable));
        }

        return adjacency_list;
    }

    /*! @brief Erases all elements from a container. */
    void clear() {
        dependencies.clear();
        vertices.clear();
    }

private:
    std::unordered_map<entt::id_type, std::vector<std::pair<std::size_t, bool>>> dependencies;
    std::vector<vertex_data> vertices;
};


}


#endif

// #include "entity/poly_storage.hpp"

// #include "entity/registry.hpp"

// #include "entity/runtime_view.hpp"

// #include "entity/snapshot.hpp"
#ifndef ENTT_ENTITY_SNAPSHOT_HPP
#define ENTT_ENTITY_SNAPSHOT_HPP


#include <array>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
// #include "../config/config.h"

// #include "../core/type_traits.hpp"

// #include "entity.hpp"

// #include "fwd.hpp"

// #include "registry.hpp"



namespace entt {


/**
 * @brief Utility class to create snapshots from a registry.
 *
 * A _snapshot_ can be either a dump of the entire registry or a narrower
 * selection of components of interest.<br/>
 * This type can be used in both cases if provided with a correctly configured
 * output archive.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
class basic_snapshot {
    using traits_type = entt_traits<Entity>;

    template<typename Component, typename Archive, typename It>
    void get(Archive &archive, std::size_t sz, It first, It last) const {
        const auto view = reg->template view<std::add_const_t<Component>>();
        archive(typename traits_type::entity_type(sz));

        while(first != last) {
            const auto entt = *(first++);

            if(reg->template all_of<Component>(entt)) {
                std::apply(archive, std::tuple_cat(std::make_tuple(entt), view.get(entt)));
            }
        }
    }

    template<typename... Component, typename Archive, typename It, std::size_t... Index>
    void component(Archive &archive, It first, It last, std::index_sequence<Index...>) const {
        std::array<std::size_t, sizeof...(Index)> size{};
        auto begin = first;

        while(begin != last) {
            const auto entt = *(begin++);
            ((reg->template all_of<Component>(entt) ? ++size[Index] : size[Index]), ...);
        }

        (get<Component>(archive, size[Index], first, last), ...);
    }

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;

    /**
     * @brief Constructs an instance that is bound to a given registry.
     * @param source A valid reference to a registry.
     */
    basic_snapshot(const basic_registry<entity_type> &source) ENTT_NOEXCEPT
        : reg{&source}
    {}

    /*! @brief Default move constructor. */
    basic_snapshot(basic_snapshot &&) = default;

    /*! @brief Default move assignment operator. @return This snapshot. */
    basic_snapshot & operator=(basic_snapshot &&) = default;

    /**
     * @brief Puts aside all the entities from the underlying registry.
     *
     * Entities are serialized along with their versions. Destroyed entities are
     * taken in consideration as well by this function.
     *
     * @tparam Archive Type of output archive.
     * @param archive A valid reference to an output archive.
     * @return An object of this type to continue creating the snapshot.
     */
    template<typename Archive>
    const basic_snapshot & entities(Archive &archive) const {
        const auto sz = reg->size();

        archive(typename traits_type::entity_type(sz));

        for(auto first = reg->data(), last = first + sz; first != last; ++first) {
            archive(*first);
        }

        archive(reg->destroyed());

        return *this;
    }

    /**
     * @brief Puts aside the given components.
     *
     * Each instance is serialized together with the entity to which it belongs.
     * Entities are serialized along with their versions.
     *
     * @tparam Component Types of components to serialize.
     * @tparam Archive Type of output archive.
     * @param archive A valid reference to an output archive.
     * @return An object of this type to continue creating the snapshot.
     */
    template<typename... Component, typename Archive>
    const basic_snapshot & component(Archive &archive) const {
        if constexpr(sizeof...(Component) == 1u) {
            const auto view = reg->template view<const Component...>();
            (component<Component>(archive, view.data(), view.data() + view.size()), ...);
            return *this;
        } else {
            (component<Component>(archive), ...);
            return *this;
        }
    }

    /**
     * @brief Puts aside the given components for the entities in a range.
     *
     * Each instance is serialized together with the entity to which it belongs.
     * Entities are serialized along with their versions.
     *
     * @tparam Component Types of components to serialize.
     * @tparam Archive Type of output archive.
     * @tparam It Type of input iterator.
     * @param archive A valid reference to an output archive.
     * @param first An iterator to the first element of the range to serialize.
     * @param last An iterator past the last element of the range to serialize.
     * @return An object of this type to continue creating the snapshot.
     */
    template<typename... Component, typename Archive, typename It>
    const basic_snapshot & component(Archive &archive, It first, It last) const {
        component<Component...>(archive, first, last, std::index_sequence_for<Component...>{});
        return *this;
    }

private:
    const basic_registry<entity_type> *reg;
};


/**
 * @brief Utility class to restore a snapshot as a whole.
 *
 * A snapshot loader requires that the destination registry be empty and loads
 * all the data at once while keeping intact the identifiers that the entities
 * originally had.<br/>
 * An example of use is the implementation of a save/restore utility.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
class basic_snapshot_loader {
    using traits_type = entt_traits<Entity>;

    template<typename Type, typename Archive>
    void assign(Archive &archive) const {
        typename traits_type::entity_type length{};
        archive(length);

        entity_type entt{};

        if constexpr(std::tuple_size_v<decltype(reg->template view<Type>().get({}))> == 0) {
            while(length--) {
                archive(entt);
                const auto entity = reg->valid(entt) ? entt : reg->create(entt);
                ENTT_ASSERT(entity == entt, "Entity not available for use");
                reg->template emplace<Type>(entity);
            }
        } else {
            Type instance{};

            while(length--) {
                archive(entt, instance);
                const auto entity = reg->valid(entt) ? entt : reg->create(entt);
                ENTT_ASSERT(entity == entt, "Entity not available for use");
                reg->template emplace<Type>(entity, std::move(instance));
            }
        }
    }

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;

    /**
     * @brief Constructs an instance that is bound to a given registry.
     * @param source A valid reference to a registry.
     */
    basic_snapshot_loader(basic_registry<entity_type> &source) ENTT_NOEXCEPT
        : reg{&source}
    {
        // restoring a snapshot as a whole requires a clean registry
        ENTT_ASSERT(reg->empty(), "Registry must be empty");
    }

    /*! @brief Default move constructor. */
    basic_snapshot_loader(basic_snapshot_loader &&) = default;

    /*! @brief Default move assignment operator. @return This loader. */
    basic_snapshot_loader & operator=(basic_snapshot_loader &&) = default;

    /**
     * @brief Restores entities that were in use during serialization.
     *
     * This function restores the entities that were in use during serialization
     * and gives them the versions they originally had.
     *
     * @tparam Archive Type of input archive.
     * @param archive A valid reference to an input archive.
     * @return A valid loader to continue restoring data.
     */
    template<typename Archive>
    const basic_snapshot_loader & entities(Archive &archive) const {
        typename traits_type::entity_type length{};

        archive(length);
        std::vector<entity_type> all(length);

        for(decltype(length) pos{}; pos < length; ++pos) {
            archive(all[pos]);
        }

        entity_type destroyed;
        archive(destroyed);

        reg->assign(all.cbegin(), all.cend(), destroyed);

        return *this;
    }

    /**
     * @brief Restores components and assigns them to the right entities.
     *
     * The template parameter list must be exactly the same used during
     * serialization. In the event that the entity to which the component is
     * assigned doesn't exist yet, the loader will take care to create it with
     * the version it originally had.
     *
     * @tparam Component Types of components to restore.
     * @tparam Archive Type of input archive.
     * @param archive A valid reference to an input archive.
     * @return A valid loader to continue restoring data.
     */
    template<typename... Component, typename Archive>
    const basic_snapshot_loader & component(Archive &archive) const {
        (assign<Component>(archive), ...);
        return *this;
    }

    /**
     * @brief Destroys those entities that have no components.
     *
     * In case all the entities were serialized but only part of the components
     * was saved, it could happen that some of the entities have no components
     * once restored.<br/>
     * This functions helps to identify and destroy those entities.
     *
     * @return A valid loader to continue restoring data.
     */
    const basic_snapshot_loader & orphans() const {
        reg->orphans([this](const auto entt) {
            reg->destroy(entt);
        });

        return *this;
    }

private:
    basic_registry<entity_type> *reg;
};


/**
 * @brief Utility class for _continuous loading_.
 *
 * A _continuous loader_ is designed to load data from a source registry to a
 * (possibly) non-empty destination. The loader can accommodate in a registry
 * more than one snapshot in a sort of _continuous loading_ that updates the
 * destination one step at a time.<br/>
 * Identifiers that entities originally had are not transferred to the target.
 * Instead, the loader maps remote identifiers to local ones while restoring a
 * snapshot.<br/>
 * An example of use is the implementation of a client-server applications with
 * the requirement of transferring somehow parts of the representation side to
 * side.
 *
 * @tparam Entity A valid entity type (see entt_traits for more details).
 */
template<typename Entity>
class basic_continuous_loader {
    using traits_type = entt_traits<Entity>;

    void destroy(Entity entt) {
        if(const auto it = remloc.find(entt); it == remloc.cend()) {
            const auto local = reg->create();
            remloc.emplace(entt, std::make_pair(local, true));
            reg->destroy(local);
        }
    }

    void restore(Entity entt) {
        const auto it = remloc.find(entt);

        if(it == remloc.cend()) {
            const auto local = reg->create();
            remloc.emplace(entt, std::make_pair(local, true));
        } else {
            if(!reg->valid(remloc[entt].first)) {
                remloc[entt].first = reg->create();
            }

            // set the dirty flag
            remloc[entt].second = true;
        }
    }

    template<typename Container>
    auto update(int, Container &container)
    -> decltype(typename Container::mapped_type{}, void()) {
        // map like container
        Container other;

        for(auto &&pair: container) {
            using first_type = std::remove_const_t<typename std::decay_t<decltype(pair)>::first_type>;
            using second_type = typename std::decay_t<decltype(pair)>::second_type;

            if constexpr(std::is_same_v<first_type, entity_type> && std::is_same_v<second_type, entity_type>) {
                other.emplace(map(pair.first), map(pair.second));
            } else if constexpr(std::is_same_v<first_type, entity_type>) {
                other.emplace(map(pair.first), std::move(pair.second));
            } else {
                static_assert(std::is_same_v<second_type, entity_type>, "Neither the key nor the value are of entity type");
                other.emplace(std::move(pair.first), map(pair.second));
            }
        }

        std::swap(container, other);
    }

    template<typename Container>
    auto update(char, Container &container)
    -> decltype(typename Container::value_type{}, void()) {
        // vector like container
        static_assert(std::is_same_v<typename Container::value_type, entity_type>, "Invalid value type");

        for(auto &&entt: container) {
            entt = map(entt);
        }
    }

    template<typename Other, typename Type, typename Member>
    void update([[maybe_unused]] Other &instance, [[maybe_unused]] Member Type:: *member) {
        if constexpr(!std::is_same_v<Other, Type>) {
            return;
        } else if constexpr(std::is_same_v<Member, entity_type>) {
            instance.*member = map(instance.*member);
        } else {
            // maybe a container? let's try...
            update(0, instance.*member);
        }
    }

    template<typename Component>
    void remove_if_exists() {
        for(auto &&ref: remloc) {
            const auto local = ref.second.first;

            if(reg->valid(local)) {
                reg->template remove<Component>(local);
            }
        }
    }

    template<typename Other, typename Archive, typename... Type, typename... Member>
    void assign(Archive &archive, [[maybe_unused]] Member Type:: *... member) {
        typename traits_type::entity_type length{};
        archive(length);

        entity_type entt{};

        if constexpr(std::tuple_size_v<decltype(reg->template view<Other>().get({}))> == 0) {
            while(length--) {
                archive(entt);
                restore(entt);
                reg->template emplace_or_replace<Other>(map(entt));
            }
        } else {
            Other instance{};

            while(length--) {
                archive(entt, instance);
                (update(instance, member), ...);
                restore(entt);
                reg->template emplace_or_replace<Other>(map(entt), std::move(instance));
            }
        }
    }

public:
    /*! @brief Underlying entity identifier. */
    using entity_type = Entity;

    /**
     * @brief Constructs an instance that is bound to a given registry.
     * @param source A valid reference to a registry.
     */
    basic_continuous_loader(basic_registry<entity_type> &source) ENTT_NOEXCEPT
        : reg{&source}
    {}

    /*! @brief Default move constructor. */
    basic_continuous_loader(basic_continuous_loader &&) = default;

    /*! @brief Default move assignment operator. @return This loader. */
    basic_continuous_loader & operator=(basic_continuous_loader &&) = default;

    /**
     * @brief Restores entities that were in use during serialization.
     *
     * This function restores the entities that were in use during serialization
     * and creates local counterparts for them if required.
     *
     * @tparam Archive Type of input archive.
     * @param archive A valid reference to an input archive.
     * @return A non-const reference to this loader.
     */
    template<typename Archive>
    basic_continuous_loader & entities(Archive &archive) {
        typename traits_type::entity_type length{};
        entity_type entt{};

        archive(length);

        for(decltype(length) pos{}; pos < length; ++pos) {
            archive(entt);

            if(const auto entity = traits_type::to_entity(entt); entity == pos) {
                restore(entt);
            } else {
                destroy(entt);
            }
        }

        // discards the head of the list of destroyed entities
        archive(entt);

        return *this;
    }

    /**
     * @brief Restores components and assigns them to the right entities.
     *
     * The template parameter list must be exactly the same used during
     * serialization. In the event that the entity to which the component is
     * assigned doesn't exist yet, the loader will take care to create a local
     * counterpart for it.<br/>
     * Members can be either data members of type entity_type or containers of
     * entities. In both cases, the loader will visit them and update the
     * entities by replacing each one with its local counterpart.
     *
     * @tparam Component Type of component to restore.
     * @tparam Archive Type of input archive.
     * @tparam Type Types of components to update with local counterparts.
     * @tparam Member Types of members to update with their local counterparts.
     * @param archive A valid reference to an input archive.
     * @param member Members to update with their local counterparts.
     * @return A non-const reference to this loader.
     */
    template<typename... Component, typename Archive, typename... Type, typename... Member>
    basic_continuous_loader & component(Archive &archive, Member Type:: *... member) {
        (remove_if_exists<Component>(), ...);
        (assign<Component>(archive, member...), ...);
        return *this;
    }

    /**
     * @brief Helps to purge entities that no longer have a conterpart.
     *
     * Users should invoke this member function after restoring each snapshot,
     * unless they know exactly what they are doing.
     *
     * @return A non-const reference to this loader.
     */
    basic_continuous_loader & shrink() {
        auto it = remloc.begin();

        while(it != remloc.cend()) {
            const auto local = it->second.first;
            bool &dirty = it->second.second;

            if(dirty) {
                dirty = false;
                ++it;
            } else {
                if(reg->valid(local)) {
                    reg->destroy(local);
                }

                it = remloc.erase(it);
            }
        }

        return *this;
    }

    /**
     * @brief Destroys those entities that have no components.
     *
     * In case all the entities were serialized but only part of the components
     * was saved, it could happen that some of the entities have no components
     * once restored.<br/>
     * This functions helps to identify and destroy those entities.
     *
     * @return A non-const reference to this loader.
     */
    basic_continuous_loader & orphans() {
        reg->orphans([this](const auto entt) {
            reg->destroy(entt);
        });

        return *this;
    }

    /**
     * @brief Tests if a loader knows about a given entity.
     * @param entt An entity identifier.
     * @return True if `entity` is managed by the loader, false otherwise.
     */
    [[nodiscard]] bool contains(entity_type entt) const ENTT_NOEXCEPT {
        return (remloc.find(entt) != remloc.cend());
    }

    /**
     * @brief Returns the identifier to which an entity refers.
     * @param entt An entity identifier.
     * @return The local identifier if any, the null entity otherwise.
     */
    [[nodiscard]] entity_type map(entity_type entt) const ENTT_NOEXCEPT {
        const auto it = remloc.find(entt);
        entity_type other = null;

        if(it != remloc.cend()) {
            other = it->second.first;
        }

        return other;
    }

private:
    std::unordered_map<entity_type, std::pair<entity_type, bool>> remloc;
    basic_registry<entity_type> *reg;
};


}


#endif

// #include "entity/sparse_set.hpp"

// #include "entity/storage.hpp"

// #include "entity/utility.hpp"

// #include "entity/view.hpp"

// #include "locator/locator.hpp"
#ifndef ENTT_LOCATOR_LOCATOR_HPP
#define ENTT_LOCATOR_LOCATOR_HPP


#include <memory>
#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/**
 * @brief Service locator, nothing more.
 *
 * A service locator can be used to do what it promises: locate services.<br/>
 * Usually service locators are tightly bound to the services they expose and
 * thus it's hard to define a general purpose class to do that. This template
 * based implementation tries to fill the gap and to get rid of the burden of
 * defining a different specific locator for each application.
 *
 * @tparam Service Type of service managed by the locator.
 */
template<typename Service>
struct service_locator {
    /*! @brief Type of service offered. */
    using service_type = Service;

    /*! @brief Default constructor, deleted on purpose. */
    service_locator() = delete;
    /*! @brief Default destructor, deleted on purpose. */
    ~service_locator() = delete;

    /**
     * @brief Tests if a valid service implementation is set.
     * @return True if the service is set, false otherwise.
     */
    [[nodiscard]] static bool empty() ENTT_NOEXCEPT {
        return !static_cast<bool>(service);
    }

    /**
     * @brief Returns a weak pointer to a service implementation, if any.
     *
     * Clients of a service shouldn't retain references to it. The recommended
     * way is to retrieve the service implementation currently set each and
     * every time the need of using it arises. Otherwise users can incur in
     * unexpected behaviors.
     *
     * @return A reference to the service implementation currently set, if any.
     */
    [[nodiscard]] static std::weak_ptr<Service> get() ENTT_NOEXCEPT {
        return service;
    }

    /**
     * @brief Returns a weak reference to a service implementation, if any.
     *
     * Clients of a service shouldn't retain references to it. The recommended
     * way is to retrieve the service implementation currently set each and
     * every time the need of using it arises. Otherwise users can incur in
     * unexpected behaviors.
     *
     * @warning
     * In case no service implementation has been set, a call to this function
     * results in undefined behavior.
     *
     * @return A reference to the service implementation currently set, if any.
     */
    [[nodiscard]] static Service & ref() ENTT_NOEXCEPT {
        return *service;
    }

    /**
     * @brief Sets or replaces a service.
     * @tparam Impl Type of the new service to use.
     * @tparam Args Types of arguments to use to construct the service.
     * @param args Parameters to use to construct the service.
     */
    template<typename Impl = Service, typename... Args>
    static void set(Args &&... args) {
        service = std::make_shared<Impl>(std::forward<Args>(args)...);
    }

    /**
     * @brief Sets or replaces a service.
     * @param ptr Service to use to replace the current one.
     */
    static void set(std::shared_ptr<Service> ptr) {
        ENTT_ASSERT(static_cast<bool>(ptr), "Null service not allowed");
        service = std::move(ptr);
    }

    /**
     * @brief Resets a service.
     *
     * The service is no longer valid after a reset.
     */
    static void reset() {
        service.reset();
    }

private:
    inline static std::shared_ptr<Service> service = nullptr;
};


}


#endif

// #include "meta/adl_pointer.hpp"
#ifndef ENTT_META_ADL_POINTER_HPP
#define ENTT_META_ADL_POINTER_HPP


namespace entt {


/**
 * @brief ADL based lookup function for dereferencing meta pointer-like types.
 * @tparam Type Element type.
 * @param value A pointer-like object.
 * @return The value returned from the dereferenced pointer.
 */
template<typename Type>
decltype(auto) dereference_meta_pointer_like(const Type &value) {
    return *value;
}


/**
 * @brief Fake ADL based lookup function for meta pointer-like types.
 * @tparam Type Element type.
 */
template<typename Type>
struct adl_meta_pointer_like {
    /**
     * @brief Uses the default ADL based lookup method to resolve the call.
     * @param value A pointer-like object.
     * @return The value returned from the dereferenced pointer.
     */
    static decltype(auto) dereference(const Type &value) {
        return dereference_meta_pointer_like(value);
    }
};


}


#endif

// #include "meta/container.hpp"
#ifndef ENTT_META_CONTAINER_HPP
#define ENTT_META_CONTAINER_HPP


#include <array>
#include <map>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "../core/type_traits.hpp"
#ifndef ENTT_CORE_TYPE_TRAITS_HPP
#define ENTT_CORE_TYPE_TRAITS_HPP


#include <cstddef>
#include <utility>
#include <type_traits>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "fwd.hpp"
#ifndef ENTT_CORE_FWD_HPP
#define ENTT_CORE_FWD_HPP


#include <type_traits>
// #include "../config/config.h"



namespace entt {


template<std::size_t Len = sizeof(double[2]), std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_any;


/*! @brief Alias declaration for type identifiers. */
using id_type = ENTT_ID_TYPE;


/*! @brief Alias declaration for the most common use case. */
using any = basic_any<>;


}


#endif



namespace entt {


/**
 * @brief Utility class to disambiguate overloaded functions.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
struct choice_t
    // Unfortunately, doxygen cannot parse such a construct.
    /*! @cond TURN_OFF_DOXYGEN */
    : choice_t<N-1>
    /*! @endcond */
{};


/*! @copybrief choice_t */
template<>
struct choice_t<0> {};


/**
 * @brief Variable template for the choice trick.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
inline constexpr choice_t<N> choice{};


/**
 * @brief Identity type trait.
 *
 * Useful to establish non-deduced contexts in template argument deduction
 * (waiting for C++20) or to provide types through function arguments.
 *
 * @tparam Type A type.
 */
template<typename Type>
struct type_identity {
    /*! @brief Identity type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Type A type.
 */
template<typename Type>
using type_identity_t = typename type_identity<Type>::type;


/**
 * @brief A type-only `sizeof` wrapper that returns 0 where `sizeof` complains.
 * @tparam Type The type of which to return the size.
 * @tparam The size of the type if `sizeof` accepts it, 0 otherwise.
 */
template<typename Type, typename = void>
struct size_of: std::integral_constant<std::size_t, 0u> {};


/*! @copydoc size_of */
template<typename Type>
struct size_of<Type, std::void_t<decltype(sizeof(Type))>>
    : std::integral_constant<std::size_t, sizeof(Type)>
{};


/**
 * @brief Helper variable template.
 * @tparam Type The type of which to return the size.
 */
template<class Type>
inline constexpr auto size_of_v = size_of<Type>::value;


/**
 * @brief Using declaration to be used to _repeat_ the same type a number of
 * times equal to the size of a given parameter pack.
 * @tparam Type A type to repeat.
 */
template<typename Type, typename>
using unpack_as_t = Type;


/**
 * @brief Helper variable template to be used to _repeat_ the same value a
 * number of times equal to the size of a given parameter pack.
 * @tparam Value A value to repeat.
 */
template<auto Value, typename>
inline constexpr auto unpack_as_v = Value;


/**
 * @brief Wraps a static constant.
 * @tparam Value A static constant.
 */
template<auto Value>
using integral_constant = std::integral_constant<decltype(Value), Value>;


/**
 * @brief Alias template to facilitate the creation of named values.
 * @tparam Value A constant value at least convertible to `id_type`.
 */
template<id_type Value>
using tag = integral_constant<Value>;


/**
 * @brief A class to use to push around lists of types, nothing more.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list {
    /*! @brief Type list type. */
    using type = type_list;
    /*! @brief Compile-time number of elements in the type list. */
    static constexpr auto size = sizeof...(Type);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct type_list_element;


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Index Index of the type to return.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<std::size_t Index, typename Type, typename... Other>
struct type_list_element<Index, type_list<Type, Other...>>
    : type_list_element<Index - 1u, type_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<typename Type, typename... Other>
struct type_list_element<0u, type_list<Type, Other...>> {
    /*! @brief Searched type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the type to return.
 * @tparam List Type list to search into.
 */
template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @return A type list composed by the types of both the type lists.
 */
template<typename... Type, typename... Other>
constexpr type_list<Type..., Other...> operator+(type_list<Type...>, type_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_cat;


/*! @brief Concatenates multiple type lists. */
template<>
struct type_list_cat<> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<>;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @tparam List Other type lists, if any.
 */
template<typename... Type, typename... Other, typename... List>
struct type_list_cat<type_list<Type...>, type_list<Other...>, List...> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = typename type_list_cat<type_list<Type..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list_cat<type_list<Type...>> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<Type...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists to concatenate.
 */
template<typename... List>
using type_list_cat_t = typename type_list_cat<List...>::type;


/*! @brief Primary template isn't defined on purpose. */
template<typename>
struct type_list_unique;


/**
 * @brief Removes duplicates types from a type list.
 * @tparam Type One of the types provided by the given type list.
 * @tparam Other The other types provided by the given type list.
 */
template<typename Type, typename... Other>
struct type_list_unique<type_list<Type, Other...>> {
    /*! @brief A type list without duplicate types. */
    using type = std::conditional_t<
        std::disjunction_v<std::is_same<Type, Other>...>,
        typename type_list_unique<type_list<Other...>>::type,
        type_list_cat_t<type_list<Type>, typename type_list_unique<type_list<Other...>>::type>
    >;
};


/*! @brief Removes duplicates types from a type list. */
template<>
struct type_list_unique<type_list<>> {
    /*! @brief A type list without duplicate types. */
    using type = type_list<>;
};


/**
 * @brief Helper type.
 * @tparam Type A type list.
 */
template<typename Type>
using type_list_unique_t = typename type_list_unique<Type>::type;


/**
 * @brief Provides the member constant `value` to true if a type list contains a
 * given type, false otherwise.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<typename List, typename Type>
struct type_list_contains;


/**
 * @copybrief type_list_contains
 * @tparam Type Types provided by the type list.
 * @tparam Other Type to look for.
 */
template<typename... Type, typename Other>
struct type_list_contains<type_list<Type...>, Other>: std::disjunction<std::is_same<Type, Other>...> {};


/**
 * @brief Helper variable template.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<class List, typename Type>
inline constexpr auto type_list_contains_v = type_list_contains<List, Type>::value;


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_diff;


/**
 * @brief Computes the difference between two type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 */
template<typename... Type, typename... Other>
struct type_list_diff<type_list<Type...>, type_list<Other...>> {
    /*! @brief A type list that is the difference between the two type lists. */
    using type = type_list_cat_t<std::conditional_t<type_list_contains_v<type_list<Other...>, Type>, type_list<>, type_list<Type>>...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists between which to compute the difference.
 */
template<typename... List>
using type_list_diff_t = typename type_list_diff<List...>::type;


/**
 * @brief A class to use to push around lists of constant values, nothing more.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list {
    /*! @brief Value list type. */
    using type = value_list;
    /*! @brief Compile-time number of elements in the value list. */
    static constexpr auto size = sizeof...(Value);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct value_list_element;


/**
 * @brief Provides compile-time indexed access to the values of a value list.
 * @tparam Index Index of the value to return.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<std::size_t Index, auto Value, auto... Other>
struct value_list_element<Index, value_list<Value, Other...>>
    : value_list_element<Index - 1u, value_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<auto Value, auto... Other>
struct value_list_element<0u, value_list<Value, Other...>> {
    /*! @brief Searched value. */
    static constexpr auto value = Value;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the value to return.
 * @tparam List Value list to search into.
 */
template<std::size_t Index, typename List>
inline constexpr auto value_list_element_v = value_list_element<Index, List>::value;


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @return A value list composed by the values of both the value lists.
 */
template<auto... Value, auto... Other>
constexpr value_list<Value..., Other...> operator+(value_list<Value...>, value_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct value_list_cat;


/*! @brief Concatenates multiple value lists. */
template<>
struct value_list_cat<> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<>;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @tparam List Other value lists, if any.
 */
template<auto... Value, auto... Other, typename... List>
struct value_list_cat<value_list<Value...>, value_list<Other...>, List...> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = typename value_list_cat<value_list<Value..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list_cat<value_list<Value...>> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<Value...>;
};


/**
 * @brief Helper type.
 * @tparam List Value lists to concatenate.
 */
template<typename... List>
using value_list_cat_t = typename value_list_cat<List...>::type;


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
[[nodiscard]] constexpr bool is_equality_comparable(...) { return false; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<0>)
-> decltype(std::declval<Type>() == std::declval<Type>()) { return true; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<1>)
-> decltype(std::declval<typename Type::value_type>(), std::declval<Type>() == std::declval<Type>()) {
    if constexpr(std::is_same_v<typename Type::value_type, Type>) {
        return is_equality_comparable<Type>(choice<0>);
    } else {
        return is_equality_comparable<typename Type::value_type>(choice<2>);
    }
}


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<2>)
-> decltype(std::declval<typename Type::mapped_type>(), std::declval<Type>() == std::declval<Type>()) {
    return is_equality_comparable<typename Type::key_type>(choice<2>) && is_equality_comparable<typename Type::mapped_type>(choice<2>);
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Provides the member constant `value` to true if a given type is
 * equality comparable, false otherwise.
 * @tparam Type Potentially equality comparable type.
 */
template<typename Type, typename = void>
struct is_equality_comparable: std::bool_constant<internal::is_equality_comparable<Type>(choice<2>)> {};


/**
 * @brief Helper variable template.
 * @tparam Type Potentially equality comparable type.
 */
template<class Type>
inline constexpr auto is_equality_comparable_v = is_equality_comparable<Type>::value;


/*! @brief Same as std::is_invocable, but with tuples. */
template<typename, typename>
struct is_applicable: std::false_type {};


/**
 * @copybrief is_applicable
 * @tparam Func A valid function type.
 * @tparam Tuple Tuple-like type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
* @copybrief is_applicable
* @tparam Func A valid function type.
* @tparam Tuple Tuple-like type.
* @tparam Args The list of arguments to use to probe the function type.
*/
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, const Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, typename Args>
inline constexpr auto is_applicable_v = is_applicable<Func, Args>::value;


/*! @brief Same as std::is_invocable_r, but with tuples for arguments. */
template<typename, typename, typename>
struct is_applicable_r: std::false_type {};


/**
 * @copybrief is_applicable_r
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename... Args>
struct is_applicable_r<Ret, Func, std::tuple<Args...>>: std::is_invocable_r<Ret, Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename Args>
inline constexpr auto is_applicable_r_v = is_applicable_r<Ret, Func, Args>::value;


/**
* @brief Provides the member constant `value` to true if a given type is
* complete, false otherwise.
* @tparam Type Potential complete type.
*/
template<typename Type, typename = void>
struct is_complete: std::false_type {};


/*! @copydoc is_complete */
template<typename Type>
struct is_complete<Type, std::void_t<decltype(sizeof(Type))>>: std::true_type {};


/**
* @brief Helper variable template.
* @tparam Type Potential complete type.
*/
template<typename Type>
inline constexpr auto is_complete_v = is_complete<Type>::value;


/**
 * @brief Transcribes the constness of a type to another type.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
struct constness_as {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::remove_const_t<To>;
};


/*! @copydoc constness_as */
template<typename To, typename From>
struct constness_as<To, const From> {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::add_const_t<To>;
};


/**
 * @brief Alias template to facilitate the transcription of the constness.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;


/**
 * @brief Extracts the class of a non-static member object or function.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
class member_class {
    static_assert(std::is_member_pointer_v<Member>, "Invalid pointer type to non-static member object or function");

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...));

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...) const);

    template<typename Class, typename Type>
    static Class * clazz(Type Class:: *);

public:
    /*! @brief The class of the given non-static member object or function. */
    using type = std::remove_pointer_t<decltype(clazz(std::declval<Member>()))>;
};


/**
 * @brief Helper type.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
using member_class_t = typename member_class<Member>::type;


}


#endif

// #include "type_traits.hpp"
#ifndef ENTT_META_TYPE_TRAITS_HPP
#define ENTT_META_TYPE_TRAITS_HPP


#include <type_traits>


namespace entt {


/**
 * @brief Traits class template to be specialized to enable support for meta
 * template information.
 */
template<typename>
struct meta_template_traits;


/**
 * @brief Traits class template to be specialized to enable support for meta
 * sequence containers.
 */
template<typename>
struct meta_sequence_container_traits;


/**
 * @brief Traits class template to be specialized to enable support for meta
 * associative containers.
 */
template<typename>
struct meta_associative_container_traits;


/**
 * @brief Provides the member constant `value` to true if a meta associative
 * container claims to wrap a key-only type, false otherwise.
 * @tparam Type Potentially key-only meta associative container type.
 */
template<typename, typename = void>
struct is_key_only_meta_associative_container: std::true_type {};


/*! @copydoc is_key_only_meta_associative_container */
template<typename Type>
struct is_key_only_meta_associative_container<Type, std::void_t<typename meta_associative_container_traits<Type>::type::mapped_type>>
    : std::false_type
{};


/**
 * @brief Helper variable template.
 * @tparam Type Potentially key-only meta associative container type.
 */
template<typename Type>
inline constexpr auto is_key_only_meta_associative_container_v = is_key_only_meta_associative_container<Type>::value;


/**
 * @brief Provides the member constant `value` to true if a given type is a
 * pointer-like type from the point of view of the meta system, false otherwise.
 * @tparam Type Potentially pointer-like type.
 */
template<typename>
struct is_meta_pointer_like: std::false_type {};


/**
 * @brief Partial specialization to ensure that const pointer-like types are
 * also accepted.
 * @tparam Type Potentially pointer-like type.
 */
template<typename Type>
struct is_meta_pointer_like<const Type>: is_meta_pointer_like<Type> {};


/**
 * @brief Helper variable template.
 * @tparam Type Potentially pointer-like type.
 */
template<typename Type>
inline constexpr auto is_meta_pointer_like_v = is_meta_pointer_like<Type>::value;


}


#endif



namespace entt {


/**
 * @brief Container traits.
 * @tparam Container Type of the underlying container.
 * @tparam Trait Traits associated with the underlying container.
 */
template<typename Container, template<typename> class... Trait>
struct meta_container_traits: public Trait<Container>... {
    /*! @brief Type of container. */
    using type = Container;
};


/**
 * @brief Basic STL-compatible container traits
 * @tparam Container The type of the container.
 */
template<typename Container>
struct basic_container {
    /**
     * @brief Returns the size of the given container.
     * @param cont The container for which to return the size.
     * @return The size of the given container.
     */
    [[nodiscard]] static typename Container::size_type size(const Container &cont) ENTT_NOEXCEPT {
        return cont.size();
    }

    /**
     * @brief Returns an iterator to the first element of the given container.
     * @param cont The container for which to return the iterator.
     * @return An iterator to the first element of the given container.
     */
    [[nodiscard]] static typename Container::iterator begin(Container &cont) {
        return cont.begin();
    }

    /**
     * @brief Returns an iterator to the first element of the given container.
     * @param cont The container for which to return the iterator.
     * @return An iterator to the first element of the given container.
     */
    [[nodiscard]] static typename Container::const_iterator cbegin(const Container &cont) {
        return cont.begin();
    }

    /**
     * @brief Returns an iterator past the last element of the given container.
     * @param cont The container for which to return the iterator.
     * @return An iterator past the last element of the given container.
     */
    [[nodiscard]] static typename Container::iterator end(Container &cont) {
        return cont.end();
    }

    /**
     * @brief Returns an iterator past the last element of the given container.
     * @param cont The container for which to return the iterator.
     * @return An iterator past the last element of the given container.
     */
    [[nodiscard]] static typename Container::const_iterator cend(const Container &cont) {
        return cont.end();
    }
};


/**
 * @brief Basic STL-compatible associative container traits
 * @tparam Container The type of the container.
 */
template<typename Container>
struct basic_associative_container {
    /**
     * @brief Returns an iterator to the element with key equivalent to the
     * given one, if any.
     * @param cont The container in which to search for the element.
     * @param key The key of the element to search.
     * @return An iterator to the element with the given key, if any.
     */
    [[nodiscard]] static typename Container::iterator find(Container &cont, const typename Container::key_type &key) {
        return cont.find(key);
    }

    /*! @copydoc find */
    [[nodiscard]] static typename Container::const_iterator cfind(const Container &cont, const typename Container::key_type &key) {
        return cont.find(key);
    }
};


/**
 * @brief Basic STL-compatible dynamic container traits
 * @tparam Container The type of the container.
 */
template<typename Container>
struct basic_dynamic_container {
    /**
     * @brief Clears the content of the given container.
     * @param cont The container for which to clear the content.
     * @return True in case of success, false otherwise.
     */
    [[nodiscard]] static bool clear([[maybe_unused]] Container &cont) {
        return cont.clear(), true;
    }
};


/**
 * @brief Basic STL-compatible dynamic associative container traits
 * @tparam Container The type of the container.
 */
template<typename Container>
struct basic_dynamic_associative_container {
    /**
     * @brief Removes the specified element from the given container.
     * @param cont The container from which to remove the element.
     * @param key The element to remove.
     * @return A bool denoting whether the removal took place.
     */
    [[nodiscard]] static bool erase([[maybe_unused]] Container &cont, [[maybe_unused]] const typename Container::key_type &key) {
        const auto sz = cont.size();
        return cont.erase(key) != sz;
    }
};


/**
 * @brief Basic STL-compatible sequence container traits
 * @tparam Container The type of the container.
 */
template<typename Container>
struct basic_sequence_container {
    /**
     * @brief Returns a reference to the element at the specified location of
     * the given container (no bounds checking is performed).
     * @param cont The container from which to get the element.
     * @param pos The position of the element to return.
     * @return A reference to the requested element.
     */
    [[nodiscard]] static typename Container::reference get(Container &cont, typename Container::size_type pos) {
        return cont[pos];
    }

    /*! @copydoc get */
    [[nodiscard]] static typename Container::const_reference cget(const Container &cont, typename Container::size_type pos) {
        return cont[pos];
    }
};


/**
 * @brief STL-compatible dynamic associative key-only container traits
 * @tparam Container The type of the container.
 */
template<typename Container>
struct dynamic_associative_key_only_container {
    /**
     * @brief Inserts an element into the given container.
     * @param cont The container in which to insert the element.
     * @param key The element to insert.
     * @return A bool denoting whether the insertion took place.
     */
    [[nodiscard]] static bool insert([[maybe_unused]] Container &cont, [[maybe_unused]] const typename Container::key_type &key) {
        return cont.insert(key).second;
    }
};


/**
 * @brief STL-compatible dynamic key-value associative container traits
 * @tparam Container The type of the container.
 */
template<typename Container>
struct dynamic_associative_key_value_container {
    /**
     * @brief Inserts an element (a key/value pair) into the given container.
     * @param cont The container in which to insert the element.
     * @param key The key of the element to insert.
     * @param value The value of the element to insert.
     * @return A bool denoting whether the insertion took place.
     */
    [[nodiscard]] static bool insert([[maybe_unused]] Container &cont, [[maybe_unused]] const typename Container::key_type &key, [[maybe_unused]] const typename Container::mapped_type &value) {
        return cont.insert(std::make_pair(key, value)).second;
    }
};


/**
 * @brief STL-compatible dynamic sequence container traits
 * @tparam Container The type of the container.
 */
template<typename Container>
struct dynamic_sequence_container {
    /**
     * @brief Resizes the given container to contain the given number of
     * elements.
     * @param cont The container to resize.
     * @param sz The new size of the container.
     * @return True in case of success, false otherwise.
     */
    [[nodiscard]] static bool resize([[maybe_unused]] Container &cont, [[maybe_unused]] typename Container::size_type sz) {
        return cont.resize(sz), true;
    }

    /**
     * @brief Inserts an element at the specified location of the given
     * container.
     * @param cont The container into which to insert the element.
     * @param it Iterator before which the element will be inserted.
     * @param value Element value to insert.
     * @return A pair consisting of an iterator to the inserted element (in case
     * of success) and a bool denoting whether the insertion took place.
     */
    [[nodiscard]] static std::pair<typename Container::iterator, bool> insert([[maybe_unused]] Container &cont, [[maybe_unused]] typename Container::const_iterator it, [[maybe_unused]] const typename Container::value_type &value) {
        return { cont.insert(it, value), true };
    }

    /**
     * @brief Removes the element at the specified location from the given
     * container.
     * @param cont The container from which to remove the element.
     * @param it Iterator to the element to remove.
     * @return A pair consisting of an iterator following the last removed
     * element (in case of success) and a bool denoting whether the insertion
     * took place.
     */
    [[nodiscard]] static std::pair<typename Container::iterator, bool> erase([[maybe_unused]] Container &cont, [[maybe_unused]] typename Container::const_iterator it) {
        return { cont.erase(it), true };
    }
};


/**
 * @brief STL-compatible fixed sequence container traits
 * @tparam Container The type of the container.
 */
template<typename Container>
struct fixed_sequence_container {
    /**
     * @brief Does nothing.
     * @return False to indicate failure in all cases.
     */
    [[nodiscard]] static bool resize(const Container &, typename Container::size_type) {
        return false;
    }

    /**
     * @brief Does nothing.
     * @return False to indicate failure in all cases.
     */
    [[nodiscard]] static bool clear(const Container &) {
        return false;
    }

    /**
     * @brief Does nothing.
     * @return A pair consisting of an invalid iterator and a false value to
     * indicate failure in all cases.
     */
    [[nodiscard]] static std::pair<typename Container::iterator, bool> insert(const Container &, typename Container::const_iterator, const typename Container::value_type &) {
        return { {}, false };
    }

    /**
     * @brief Does nothing.
     * @return A pair consisting of an invalid iterator and a false value to
     * indicate failure in all cases.
     */
    [[nodiscard]] static std::pair<typename Container::iterator, bool> erase(const Container &, typename Container::const_iterator) {
        return { {}, false };
    }
};


/**
 * @brief Meta sequence container traits for `std::vector`s of any type.
 * @tparam Type The type of elements.
 * @tparam Args Other arguments.
 */
template<typename Type, typename... Args>
struct meta_sequence_container_traits<std::vector<Type, Args...>>
        : meta_container_traits<
              std::vector<Type, Args...>,
              basic_container,
              basic_dynamic_container,
              basic_sequence_container,
              dynamic_sequence_container
          >
{};


/**
 * @brief Meta sequence container traits for `std::array`s of any type.
 * @tparam Type The type of elements.
 * @tparam N The number of elements.
 */
template<typename Type, auto N>
struct meta_sequence_container_traits<std::array<Type, N>>
        : meta_container_traits<
              std::array<Type, N>,
              basic_container,
              basic_sequence_container,
              fixed_sequence_container
          >
{};


/**
 * @brief Meta associative container traits for `std::map`s of any type.
 * @tparam Key The key type of elements.
 * @tparam Value The value type of elements.
 * @tparam Args Other arguments.
 */
template<typename Key, typename Value, typename... Args>
struct meta_associative_container_traits<std::map<Key, Value, Args...>>
        : meta_container_traits<
              std::map<Key, Value, Args...>,
              basic_container,
              basic_associative_container,
              basic_dynamic_container,
              basic_dynamic_associative_container,
              dynamic_associative_key_value_container
          >
{};


/**
 * @brief Meta associative container traits for `std::unordered_map`s of any
 * type.
 * @tparam Key The key type of elements.
 * @tparam Value The value type of elements.
 * @tparam Args Other arguments.
 */
template<typename Key, typename Value, typename... Args>
struct meta_associative_container_traits<std::unordered_map<Key, Value, Args...>>
        : meta_container_traits<
              std::unordered_map<Key, Value, Args...>,
              basic_container,
              basic_associative_container,
              basic_dynamic_container,
              basic_dynamic_associative_container,
              dynamic_associative_key_value_container
          >
{};


/**
 * @brief Meta associative container traits for `std::set`s of any type.
 * @tparam Key The type of elements.
 * @tparam Args Other arguments.
 */
template<typename Key, typename... Args>
struct meta_associative_container_traits<std::set<Key, Args...>>
        : meta_container_traits<
              std::set<Key, Args...>,
              basic_container,
              basic_associative_container,
              basic_dynamic_container,
              basic_dynamic_associative_container,
              dynamic_associative_key_only_container
          >
{};


/**
 * @brief Meta associative container traits for `std::unordered_set`s of any
 * type.
 * @tparam Key The type of elements.
 * @tparam Args Other arguments.
 */
template<typename Key, typename... Args>
struct meta_associative_container_traits<std::unordered_set<Key, Args...>>
        : meta_container_traits<
              std::unordered_set<Key, Args...>,
              basic_container,
              basic_associative_container,
              basic_dynamic_container,
              basic_dynamic_associative_container,
              dynamic_associative_key_only_container
          >
{};


}


#endif

// #include "meta/ctx.hpp"
#ifndef ENTT_META_CTX_HPP
#define ENTT_META_CTX_HPP


// #include "../core/attribute.h"
#ifndef ENTT_CORE_ATTRIBUTE_H
#define ENTT_CORE_ATTRIBUTE_H


#ifndef ENTT_EXPORT
#   if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#       define ENTT_EXPORT __declspec(dllexport)
#       define ENTT_IMPORT __declspec(dllimport)
#       define ENTT_HIDDEN
#   elif defined __GNUC__ && __GNUC__ >= 4
#       define ENTT_EXPORT __attribute__((visibility("default")))
#       define ENTT_IMPORT __attribute__((visibility("default")))
#       define ENTT_HIDDEN __attribute__((visibility("hidden")))
#   else /* Unsupported compiler */
#       define ENTT_EXPORT
#       define ENTT_IMPORT
#       define ENTT_HIDDEN
#   endif
#endif


#ifndef ENTT_API
#   if defined ENTT_API_EXPORT
#       define ENTT_API ENTT_EXPORT
#   elif defined ENTT_API_IMPORT
#       define ENTT_API ENTT_IMPORT
#   else /* No API */
#       define ENTT_API
#   endif
#endif


#endif

// #include "../config/config.h"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


struct meta_type_node;


struct ENTT_API meta_context {
    // we could use the lines below but VS2017 returns with an ICE if combined with ENTT_API despite the code being valid C++
    //     inline static meta_type_node *local = nullptr;
    //     inline static meta_type_node **global = &local;

    [[nodiscard]] static meta_type_node * & local() ENTT_NOEXCEPT {
        static meta_type_node *chain = nullptr;
        return chain;
    }

    [[nodiscard]] static meta_type_node ** & global() ENTT_NOEXCEPT {
        static meta_type_node **chain = &local();
        return chain;
    }
};


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/*! @brief Opaque container for a meta context. */
struct meta_ctx {
    /**
     * @brief Binds the meta system to a given context.
     * @param other A valid context to which to bind.
     */
    static void bind(meta_ctx other) ENTT_NOEXCEPT {
        internal::meta_context::global() = other.ctx;
    }

private:
    internal::meta_type_node **ctx{&internal::meta_context::local()};
};


}


#endif

// #include "meta/factory.hpp"
#ifndef ENTT_META_FACTORY_HPP
#define ENTT_META_FACTORY_HPP


#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/fwd.hpp"

// #include "../core/type_info.hpp"
#ifndef ENTT_CORE_TYPE_INFO_HPP
#define ENTT_CORE_TYPE_INFO_HPP


#include <string_view>
#include <type_traits>
// #include "../config/config.h"

// #include "../core/attribute.h"
#ifndef ENTT_CORE_ATTRIBUTE_H
#define ENTT_CORE_ATTRIBUTE_H


#ifndef ENTT_EXPORT
#   if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#       define ENTT_EXPORT __declspec(dllexport)
#       define ENTT_IMPORT __declspec(dllimport)
#       define ENTT_HIDDEN
#   elif defined __GNUC__ && __GNUC__ >= 4
#       define ENTT_EXPORT __attribute__((visibility("default")))
#       define ENTT_IMPORT __attribute__((visibility("default")))
#       define ENTT_HIDDEN __attribute__((visibility("hidden")))
#   else /* Unsupported compiler */
#       define ENTT_EXPORT
#       define ENTT_IMPORT
#       define ENTT_HIDDEN
#   endif
#endif


#ifndef ENTT_API
#   if defined ENTT_API_EXPORT
#       define ENTT_API ENTT_EXPORT
#   elif defined ENTT_API_IMPORT
#       define ENTT_API ENTT_IMPORT
#   else /* No API */
#       define ENTT_API
#   endif
#endif


#endif

// #include "hashed_string.hpp"
#ifndef ENTT_CORE_HASHED_STRING_HPP
#define ENTT_CORE_HASHED_STRING_HPP


#include <cstddef>
#include <cstdint>
// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
struct fnv1a_traits;


template<>
struct fnv1a_traits<std::uint32_t> {
    using type = std::uint32_t;
    static constexpr std::uint32_t offset = 2166136261;
    static constexpr std::uint32_t prime = 16777619;
};


template<>
struct fnv1a_traits<std::uint64_t> {
    using type = std::uint64_t;
    static constexpr std::uint64_t offset = 14695981039346656037ull;
    static constexpr std::uint64_t prime = 1099511628211ull;
};


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Zero overhead unique identifier.
 *
 * A hashed string is a compile-time tool that allows users to use
 * human-readable identifers in the codebase while using their numeric
 * counterparts at runtime.<br/>
 * Because of that, a hashed string can also be used in constant expressions if
 * required.
 *
 * @tparam Char Character type.
 */
template<typename Char>
class basic_hashed_string {
    using traits_type = internal::fnv1a_traits<id_type>;

    struct const_wrapper {
        // non-explicit constructor on purpose
        constexpr const_wrapper(const Char *curr) ENTT_NOEXCEPT: str{curr} {}
        const Char *str;
    };

    // Fowler–Noll–Vo hash function v. 1a - the good
    [[nodiscard]] static constexpr id_type helper(const Char *curr) ENTT_NOEXCEPT {
        auto value = traits_type::offset;

        while(*curr != 0) {
            value = (value ^ static_cast<traits_type::type>(*(curr++))) * traits_type::prime;
        }

        return value;
    }

public:
    /*! @brief Character type. */
    using value_type = Char;
    /*! @brief Unsigned integer type. */
    using hash_type = id_type;

    /**
     * @brief Returns directly the numeric representation of a string view.
     * @param str Human-readable identifer.
     * @param size Length of the string to hash.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static constexpr hash_type value(const value_type *str, std::size_t size) ENTT_NOEXCEPT {
        id_type partial{traits_type::offset};
        while(size--) { partial = (partial^(str++)[0])*traits_type::prime; }
        return partial;
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * const auto value = basic_hashed_string<char>::to_value("my.png");
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param str Human-readable identifer.
     * @return The numeric representation of the string.
     */
    template<std::size_t N>
    [[nodiscard]] static constexpr hash_type value(const value_type (&str)[N]) ENTT_NOEXCEPT {
        return helper(str);
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static hash_type value(const_wrapper wrapper) ENTT_NOEXCEPT {
        return helper(wrapper.str);
    }

    /*! @brief Constructs an empty hashed string. */
    constexpr basic_hashed_string() ENTT_NOEXCEPT
        : str{nullptr}, hash{}
    {}

    /**
     * @brief Constructs a hashed string from an array of const characters.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * basic_hashed_string<char> hs{"my.png"};
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param curr Human-readable identifer.
     */
    template<std::size_t N>
    constexpr basic_hashed_string(const value_type (&curr)[N]) ENTT_NOEXCEPT
        : str{curr}, hash{helper(curr)}
    {}

    /**
     * @brief Explicit constructor on purpose to avoid constructing a hashed
     * string directly from a `const value_type *`.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     */
    explicit constexpr basic_hashed_string(const_wrapper wrapper) ENTT_NOEXCEPT
        : str{wrapper.str}, hash{helper(wrapper.str)}
    {}

    /**
     * @brief Returns the human-readable representation of a hashed string.
     * @return The string used to initialize the instance.
     */
    [[nodiscard]] constexpr const value_type * data() const ENTT_NOEXCEPT {
        return str;
    }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr hash_type value() const ENTT_NOEXCEPT {
        return hash;
    }

    /*! @copydoc data */
    [[nodiscard]] constexpr operator const value_type *() const ENTT_NOEXCEPT { return data(); }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr operator hash_type() const ENTT_NOEXCEPT { return value(); }

    /**
     * @brief Compares two hashed strings.
     * @param other Hashed string with which to compare.
     * @return True if the two hashed strings are identical, false otherwise.
     */
    [[nodiscard]] constexpr bool operator==(const basic_hashed_string &other) const ENTT_NOEXCEPT {
        return hash == other.hash;
    }

private:
    const value_type *str;
    hash_type hash;
};


/**
 * @brief Deduction guide.
 *
 * It allows to deduce the character type of the hashed string directly from a
 * human-readable identifer provided to the constructor.
 *
 * @tparam Char Character type.
 * @tparam N Number of characters of the identifier.
 * @param str Human-readable identifer.
 */
template<typename Char, std::size_t N>
basic_hashed_string(const Char (&str)[N])
-> basic_hashed_string<Char>;


/**
 * @brief Compares two hashed strings.
 * @tparam Char Character type.
 * @param lhs A valid hashed string.
 * @param rhs A valid hashed string.
 * @return True if the two hashed strings are identical, false otherwise.
 */
template<typename Char>
[[nodiscard]] constexpr bool operator!=(const basic_hashed_string<Char> &lhs, const basic_hashed_string<Char> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/*! @brief Aliases for common character types. */
using hashed_string = basic_hashed_string<char>;


/*! @brief Aliases for common character types. */
using hashed_wstring = basic_hashed_string<wchar_t>;


inline namespace literals {


/**
 * @brief User defined literal for hashed strings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed string.
 */
[[nodiscard]] constexpr entt::hashed_string operator"" _hs(const char *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_string{str};
}


/**
 * @brief User defined literal for hashed wstrings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed wstring.
 */
[[nodiscard]] constexpr entt::hashed_wstring operator"" _hws(const wchar_t *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_wstring{str};
}


}


}


#endif

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


struct ENTT_API type_seq final {
    [[nodiscard]] static id_type next() ENTT_NOEXCEPT {
        static ENTT_MAYBE_ATOMIC(id_type) value{};
        return value++;
    }
};


template<typename Type>
[[nodiscard]] constexpr auto stripped_type_name() ENTT_NOEXCEPT {
#if defined ENTT_PRETTY_FUNCTION
    std::string_view pretty_function{ENTT_PRETTY_FUNCTION};
    auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(ENTT_PRETTY_FUNCTION_PREFIX)+1);
    auto value = pretty_function.substr(first, pretty_function.find_last_of(ENTT_PRETTY_FUNCTION_SUFFIX) - first);
    return value;
#else
    return std::string_view{""};
#endif
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr std::string_view type_name(int) ENTT_NOEXCEPT {
    constexpr auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type>
[[nodiscard]] static std::string_view type_name(char) ENTT_NOEXCEPT {
    static const auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr id_type type_hash(int) ENTT_NOEXCEPT {
    constexpr auto stripped = stripped_type_name<Type>();
    constexpr auto value = hashed_string::value(stripped.data(), stripped.size());
    return value;
}


template<typename Type>
[[nodiscard]] static id_type type_hash(char) ENTT_NOEXCEPT {
    static const auto value = [](const auto stripped) {
        return hashed_string::value(stripped.data(), stripped.size());
    }(stripped_type_name<Type>());
    return value;
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Type sequential identifier.
 * @tparam Type Type for which to generate a sequential identifier.
 */
template<typename Type, typename = void>
struct ENTT_API type_seq final {
    /**
     * @brief Returns the sequential identifier of a given type.
     * @return The sequential identifier of a given type.
     */
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const id_type value = internal::type_seq::next();
        return value;
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type hash.
 * @tparam Type Type for which to generate a hash value.
 */
template<typename Type, typename = void>
struct type_hash final {
    /**
     * @brief Returns the numeric representation of a given type.
     * @return The numeric representation of the given type.
     */
#if defined ENTT_PRETTY_FUNCTION
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return internal::type_hash<Type>(0);
#else
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return type_seq<Type>::value();
#endif
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type name.
 * @tparam Type Type for which to generate a name.
 */
template<typename Type, typename = void>
struct type_name final {
    /**
     * @brief Returns the name of a given type.
     * @return The name of the given type.
     */
    [[nodiscard]] static constexpr std::string_view value() ENTT_NOEXCEPT {
        return internal::type_name<Type>(0);
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator std::string_view() const ENTT_NOEXCEPT { return value(); }
};


/*! @brief Implementation specific information about a type. */
class type_info final {
    template<typename>
    friend type_info type_id() ENTT_NOEXCEPT;

    type_info(id_type seq_v, id_type hash_v, std::string_view name_v) ENTT_NOEXCEPT
        : seq_value{seq_v},
          hash_value{hash_v},
          name_value{name_v}
    {}

public:
    /*! @brief Default constructor. */
    type_info() ENTT_NOEXCEPT
        : type_info({}, {}, {})
    {}

    /*! @brief Default copy constructor. */
    type_info(const type_info &) ENTT_NOEXCEPT = default;
    /*! @brief Default move constructor. */
    type_info(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Default copy assignment operator.
     * @return This type info object.
     */
    type_info & operator=(const type_info &) ENTT_NOEXCEPT = default;

    /**
     * @brief Default move assignment operator.
     * @return This type info object.
     */
    type_info & operator=(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Checks if a type info object is properly initialized.
     * @return True if the object is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return name_value.data() != nullptr;
    }

    /**
     * @brief Type sequential identifier.
     * @return Type sequential identifier.
     */
    [[nodiscard]] id_type seq() const ENTT_NOEXCEPT {
        return seq_value;
    }

    /**
     * @brief Type hash.
     * @return Type hash.
     */
    [[nodiscard]] id_type hash() const ENTT_NOEXCEPT {
        return hash_value;
    }

    /**
     * @brief Type name.
     * @return Type name.
     */
    [[nodiscard]] std::string_view name() const ENTT_NOEXCEPT {
        return name_value;
    }

    /**
     * @brief Compares the contents of two type info objects.
     * @param other Object with which to compare.
     * @return False if the two contents differ, true otherwise.
     */
    [[nodiscard]] bool operator==(const type_info &other) const ENTT_NOEXCEPT {
        return hash_value == other.hash_value;
    }

private:
    id_type seq_value;
    id_type hash_value;
    std::string_view name_value;
};


/**
 * @brief Compares the contents of two type info objects.
 * @param lhs A type info object.
 * @param rhs A type info object.
 * @return True if the two contents differ, false otherwise.
 */
[[nodiscard]] inline bool operator!=(const type_info &lhs, const type_info &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Returns the type info object for a given type.
 * @tparam Type Type for which to generate a type info object.
 * @return The type info object for the given type.
 */
template<typename Type>
[[nodiscard]] type_info type_id() ENTT_NOEXCEPT {
    return type_info{
        type_seq<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_name<std::remove_cv_t<std::remove_reference_t<Type>>>::value()
    };
}


}


#endif

// #include "../core/type_traits.hpp"

// #include "meta.hpp"
#ifndef ENTT_META_META_HPP
#define ENTT_META_META_HPP


#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/any.hpp"
#ifndef ENTT_CORE_ANY_HPP
#define ENTT_CORE_ANY_HPP


#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/utility.hpp"
#ifndef ENTT_CORE_UTILITY_HPP
#define ENTT_CORE_UTILITY_HPP


#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/*! @brief Identity function object (waiting for C++20). */
struct identity {
    /**
     * @brief Returns its argument unchanged.
     * @tparam Type Type of the argument.
     * @param value The actual argument.
     * @return The submitted value as-is.
     */
    template<class Type>
    [[nodiscard]] constexpr Type && operator()(Type &&value) const ENTT_NOEXCEPT {
        return std::forward<Type>(value);
    }
};


/**
 * @brief Constant utility to disambiguate overloaded members of a class.
 * @tparam Type Type of the desired overload.
 * @tparam Class Type of class to which the member belongs.
 * @param member A valid pointer to a member.
 * @return Pointer to the member.
 */
template<typename Type, typename Class>
[[nodiscard]] constexpr auto overload(Type Class:: *member) ENTT_NOEXCEPT { return member; }


/**
 * @brief Constant utility to disambiguate overloaded functions.
 * @tparam Func Function type of the desired overload.
 * @param func A valid pointer to a function.
 * @return Pointer to the function.
 */
template<typename Func>
[[nodiscard]] constexpr auto overload(Func *func) ENTT_NOEXCEPT { return func; }


/**
 * @brief Helper type for visitors.
 * @tparam Func Types of function objects.
 */
template<class... Func>
struct overloaded: Func... {
    using Func::operator()...;
};


/**
 * @brief Deduction guide.
 * @tparam Func Types of function objects.
 */
template<class... Func>
overloaded(Func...)
-> overloaded<Func...>;


/**
 * @brief Basic implementation of a y-combinator.
 * @tparam Func Type of a potentially recursive function.
 */
template<class Func>
struct y_combinator {
    /**
     * @brief Constructs a y-combinator from a given function.
     * @param recursive A potentially recursive function.
     */
    y_combinator(Func recursive):
        func{std::move(recursive)}
    {}

    /**
     * @brief Invokes a y-combinator and therefore its underlying function.
     * @tparam Args Types of arguments to use to invoke the underlying function.
     * @param args Parameters to use to invoke the underlying function.
     * @return Return value of the underlying function, if any.
     */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) const {
        return func(*this, std::forward<Args>(args)...);
    }

    /*! @copydoc operator()() */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) {
        return func(*this, std::forward<Args>(args)...);
    }

private:
    Func func;
};


}


#endif

// #include "fwd.hpp"

// #include "type_info.hpp"

// #include "type_traits.hpp"



namespace entt {


/**
 * @brief A SBO friendly, type-safe container for single values of any type.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 */
template<std::size_t Len, std::size_t Align>
class basic_any {
    enum class operation: std::uint8_t { COPY, MOVE, DTOR, COMP, ADDR, CADDR, TYPE };
    enum class policy: std::uint8_t { OWNER, REF, CREF };

    using storage_type = std::aligned_storage_t<Len + !Len, Align>;
    using vtable_type = const void *(const operation, const basic_any &, void *);

    template<typename Type>
    static constexpr bool in_situ = Len && alignof(Type) <= alignof(storage_type) && sizeof(Type) <= sizeof(storage_type) && std::is_nothrow_move_constructible_v<Type>;

    template<typename Type>
    [[nodiscard]] static constexpr policy type_to_policy() {
        if constexpr(std::is_lvalue_reference_v<Type>) {
            if constexpr(std::is_const_v<std::remove_reference_t<Type>>) {
                return policy::CREF;
            } else {
                return policy::REF;
            }
        } else {
            return policy::OWNER;
        }
    }

    template<typename Type>
    [[nodiscard]] static bool compare(const void *lhs, const void *rhs) {
        if constexpr(!std::is_function_v<Type> && is_equality_comparable_v<Type>) {
            return *static_cast<const Type *>(lhs) == *static_cast<const Type *>(rhs);
        } else {
            return lhs == rhs;
        }
    }

    template<typename Type>
    static const void * basic_vtable([[maybe_unused]] const operation op, [[maybe_unused]] const basic_any &from, [[maybe_unused]] void *to) {
        static_assert(std::is_same_v<std::remove_reference_t<std::remove_const_t<Type>>, Type>, "Invalid type");

        if constexpr(!std::is_void_v<Type>) {
            const Type *instance = (in_situ<Type> && from.mode == policy::OWNER)
                ? ENTT_LAUNDER(reinterpret_cast<const Type *>(&from.storage))
                : static_cast<const Type *>(from.instance);

            switch(op) {
            case operation::COPY:
                if constexpr(std::is_copy_constructible_v<Type>) {
                    static_cast<basic_any *>(to)->emplace<Type>(*instance);
                }
                break;
            case operation::MOVE:
                if constexpr(in_situ<Type>) {
                    if(from.mode == policy::OWNER) {
                        return new (&static_cast<basic_any *>(to)->storage) Type{std::move(*const_cast<Type *>(instance))};
                    }
                }

                return (static_cast<basic_any *>(to)->instance = std::exchange(const_cast<basic_any &>(from).instance, nullptr));
            case operation::DTOR:
                if(from.mode == policy::OWNER) {
                    if constexpr(in_situ<Type>) {
                        instance->~Type();
                    } else if constexpr(std::is_array_v<Type>) {
                        delete[] instance;
                    } else {
                        delete instance;
                    }
                }
                break;
            case operation::COMP:
                return compare<Type>(instance, (*static_cast<const basic_any **>(to))->data()) ? to : nullptr;
            case operation::ADDR:
                if(from.mode == policy::CREF) {
                    return nullptr;
                }
                [[fallthrough]];
            case operation::CADDR:
                return instance;
            case operation::TYPE:
                *static_cast<type_info *>(to) = type_id<Type>();
                break;
            }
        }

        return nullptr;
    }

    template<typename Type, typename... Args>
    void initialize([[maybe_unused]] Args &&... args) {
        if constexpr(!std::is_void_v<Type>) {
            if constexpr(std::is_lvalue_reference_v<Type>) {
                static_assert(sizeof...(Args) == 1u && (std::is_lvalue_reference_v<Args> && ...), "Invalid arguments");
                instance = (std::addressof(args), ...);
            } else if constexpr(in_situ<Type>) {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    new (&storage) Type{std::forward<Args>(args)...};
                } else {
                    new (&storage) Type(std::forward<Args>(args)...);
                }
            } else {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    instance = new Type{std::forward<Args>(args)...};
                } else {
                    instance = new Type(std::forward<Args>(args)...);
                }
            }
        }
    }

    basic_any(const basic_any &other, const policy pol) ENTT_NOEXCEPT
        : instance{other.data()},
          vtable{other.vtable},
          mode{pol}
    {}

public:
    /*! @brief Size of the internal storage. */
    static constexpr auto length = Len;
    /*! @brief Alignment requirement. */
    static constexpr auto alignment = Align;

    /*! @brief Default constructor. */
    basic_any() ENTT_NOEXCEPT
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {}

    /**
     * @brief Constructs a wrapper by directly initializing the new object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    explicit basic_any(std::in_place_type_t<Type>, Args &&... args)
        : instance{},
          vtable{&basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>},
          mode{type_to_policy<Type>()}
    {
        initialize<Type>(std::forward<Args>(args)...);
    }

    /**
     * @brief Constructs a wrapper that holds an unmanaged object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type>
    basic_any(std::reference_wrapper<Type> value) ENTT_NOEXCEPT
        : basic_any{}
    {
        // invokes deprecated assignment operator (and avoids issues with vs2017)
        *this = value;
    }

    /**
     * @brief Constructs a wrapper from a given value.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>>>
    basic_any(Type &&value)
        : instance{},
          vtable{&basic_vtable<std::decay_t<Type>>},
          mode{policy::OWNER}
    {
        initialize<std::decay_t<Type>>(std::forward<Type>(value));
    }

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    basic_any(const basic_any &other)
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {
        other.vtable(operation::COPY, other, this);
    }

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    basic_any(basic_any &&other) ENTT_NOEXCEPT
        : instance{},
          vtable{other.vtable},
          mode{other.mode}
    {
        vtable(operation::MOVE, other, this);
    }

    /*! @brief Frees the internal storage, whatever it means. */
    ~basic_any() {
        vtable(operation::DTOR, *this, nullptr);
    }

    /**
     * @brief Copy assignment operator.
     * @param other The instance to copy from.
     * @return This any object.
     */
    basic_any & operator=(const basic_any &other) {
        reset();
        other.vtable(operation::COPY, other, this);
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This any object.
     */
    basic_any & operator=(basic_any &&other) ENTT_NOEXCEPT {
        std::exchange(vtable, other.vtable)(operation::DTOR, *this, nullptr);
        other.vtable(operation::MOVE, other, this);
        mode = other.mode;
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    [[deprecated("Use std::in_place_type<T &>, entt::make_any<T &>, emplace<Type &> or forward_as_any instead")]]
    basic_any & operator=(std::reference_wrapper<Type> value) ENTT_NOEXCEPT {
        emplace<Type &>(value.get());
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>, basic_any &>
    operator=(Type &&value) {
        emplace<std::decay_t<Type>>(std::forward<Type>(value));
        return *this;
    }

    /**
     * @brief Returns the type of the contained object.
     * @return The type of the contained object, if any.
     */
    [[nodiscard]] type_info type() const ENTT_NOEXCEPT {
        type_info info{};
        vtable(operation::TYPE, *this, &info);
        return info;
    }

    /**
     * @brief Returns an opaque pointer to the contained instance.
     * @return An opaque pointer the contained instance, if any.
     */
    [[nodiscard]] const void * data() const ENTT_NOEXCEPT {
        return vtable(operation::CADDR, *this, nullptr);
    }

    /*! @copydoc data */
    [[nodiscard]] void * data() ENTT_NOEXCEPT {
        return const_cast<void *>(vtable(operation::ADDR, *this, nullptr));
    }

    /**
     * @brief Replaces the contained object by creating a new instance directly.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    void emplace(Args &&... args) {
        std::exchange(vtable, &basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>)(operation::DTOR, *this, nullptr);
        mode = type_to_policy<Type>();
        initialize<Type>(std::forward<Args>(args)...);
    }

    /*! @brief Destroys contained object */
    void reset() {
        std::exchange(vtable, &basic_vtable<void>)(operation::DTOR, *this, nullptr);
        mode = policy::OWNER;
    }

    /**
     * @brief Returns false if a wrapper is empty, true otherwise.
     * @return False if the wrapper is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(vtable(operation::CADDR, *this, nullptr) == nullptr);
    }

    /**
     * @brief Checks if two wrappers differ in their content.
     * @param other Wrapper with which to compare.
     * @return False if the two objects differ in their content, true otherwise.
     */
    bool operator==(const basic_any &other) const ENTT_NOEXCEPT {
        const basic_any *trampoline = &other;
        return type() == other.type() && (vtable(operation::COMP, *this, &trampoline) || !other.data());
    }

    /**
     * @brief Aliasing constructor.
     * @return A wrapper that shares a reference to an unmanaged object.
     */
    [[nodiscard]] basic_any as_ref() ENTT_NOEXCEPT {
        return basic_any{*this, (mode == policy::CREF ? policy::CREF : policy::REF)};
    }

    /*! @copydoc as_ref */
    [[nodiscard]] basic_any as_ref() const ENTT_NOEXCEPT {
        return basic_any{*this, policy::CREF};
    }

    /**
     * @brief Returns true if a wrapper owns its object, false otherwise.
     * @return True if the wrapper owns its object, false otherwise.
     */
    [[nodiscard]] bool owner() const ENTT_NOEXCEPT {
        return (mode == policy::OWNER);
    }

private:
    union { const void *instance; storage_type storage; };
    vtable_type *vtable;
    policy mode;
};


/**
 * @brief Checks if two wrappers differ in their content.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param lhs A wrapper, either empty or not.
 * @param rhs A wrapper, either empty or not.
 * @return True if the two wrappers differ in their content, false otherwise.
 */
template<std::size_t Len, std::size_t Align>
[[nodiscard]] inline bool operator!=(const basic_any<Len, Align> &lhs, const basic_any<Len, Align> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Performs type-safe access to the contained object.
 * @tparam Type Type to which conversion is required.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param data Target any object.
 * @return The element converted to the requested type.
 */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(const basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    const auto * const instance = any_cast<std::remove_reference_t<Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &&data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(std::move(*instance));
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
const Type * any_cast(const basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    return (data->type() == type_id<Type>() ? static_cast<const Type *>(data->data()) : nullptr);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type * any_cast(basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    // last attempt to make wrappers for const references return their values
    return (data->type() == type_id<Type>() ? static_cast<Type *>(static_cast<constness_as_t<basic_any<Len, Align>, Type> *>(data)->data()) : nullptr);
}


/**
 * @brief Constructs a wrapper from a given type, passing it all arguments.
 * @tparam Type Type of object to use to initialize the wrapper.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Args Types of arguments to use to construct the new instance.
 * @param args Parameters to use to construct the instance.
 * @return A properly initialized wrapper for an object of the given type.
 */
template<typename Type, std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename... Args>
basic_any<Len, Align> make_any(Args &&... args) {
    return basic_any<Len, Align>{std::in_place_type<Type>, std::forward<Args>(args)...};
}


/**
 * @brief Forwards its argument and avoids copies for lvalue references.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Type Type of argument to use to construct the new instance.
 * @param value Parameter to use to construct the instance.
 * @return A properly initialized and not necessarily owning wrapper.
 */
template<std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename Type>
basic_any<Len, Align> forward_as_any(Type &&value) {
    return basic_any<Len, Align>{std::in_place_type<std::conditional_t<std::is_rvalue_reference_v<Type>, std::decay_t<Type>, Type>>, std::forward<Type>(value)};
}


}


#endif

// #include "../core/fwd.hpp"

// #include "../core/utility.hpp"
#ifndef ENTT_CORE_UTILITY_HPP
#define ENTT_CORE_UTILITY_HPP


#include <utility>
// #include "../config/config.h"



namespace entt {


/*! @brief Identity function object (waiting for C++20). */
struct identity {
    /**
     * @brief Returns its argument unchanged.
     * @tparam Type Type of the argument.
     * @param value The actual argument.
     * @return The submitted value as-is.
     */
    template<class Type>
    [[nodiscard]] constexpr Type && operator()(Type &&value) const ENTT_NOEXCEPT {
        return std::forward<Type>(value);
    }
};


/**
 * @brief Constant utility to disambiguate overloaded members of a class.
 * @tparam Type Type of the desired overload.
 * @tparam Class Type of class to which the member belongs.
 * @param member A valid pointer to a member.
 * @return Pointer to the member.
 */
template<typename Type, typename Class>
[[nodiscard]] constexpr auto overload(Type Class:: *member) ENTT_NOEXCEPT { return member; }


/**
 * @brief Constant utility to disambiguate overloaded functions.
 * @tparam Func Function type of the desired overload.
 * @param func A valid pointer to a function.
 * @return Pointer to the function.
 */
template<typename Func>
[[nodiscard]] constexpr auto overload(Func *func) ENTT_NOEXCEPT { return func; }


/**
 * @brief Helper type for visitors.
 * @tparam Func Types of function objects.
 */
template<class... Func>
struct overloaded: Func... {
    using Func::operator()...;
};


/**
 * @brief Deduction guide.
 * @tparam Func Types of function objects.
 */
template<class... Func>
overloaded(Func...)
-> overloaded<Func...>;


/**
 * @brief Basic implementation of a y-combinator.
 * @tparam Func Type of a potentially recursive function.
 */
template<class Func>
struct y_combinator {
    /**
     * @brief Constructs a y-combinator from a given function.
     * @param recursive A potentially recursive function.
     */
    y_combinator(Func recursive):
        func{std::move(recursive)}
    {}

    /**
     * @brief Invokes a y-combinator and therefore its underlying function.
     * @tparam Args Types of arguments to use to invoke the underlying function.
     * @param args Parameters to use to invoke the underlying function.
     * @return Return value of the underlying function, if any.
     */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) const {
        return func(*this, std::forward<Args>(args)...);
    }

    /*! @copydoc operator()() */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) {
        return func(*this, std::forward<Args>(args)...);
    }

private:
    Func func;
};


}


#endif

// #include "../core/type_info.hpp"

// #include "../core/type_traits.hpp"

// #include "adl_pointer.hpp"

// #include "ctx.hpp"

// #include "node.hpp"
#ifndef ENTT_META_NODE_HPP
#define ENTT_META_NODE_HPP


#include <cstddef>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/attribute.h"

// #include "../core/fwd.hpp"

// #include "../core/type_info.hpp"

// #include "../core/type_traits.hpp"

// #include "type_traits.hpp"



namespace entt {


class meta_any;
class meta_type;
struct meta_handle;


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


struct meta_type_node;


struct meta_prop_node {
    meta_prop_node * next;
    const meta_any &id;
    meta_any &value;
};


struct meta_base_node {
    meta_type_node * const parent;
    meta_base_node * next;
    meta_type_node *(* const type)() ENTT_NOEXCEPT;
    const void *(* const cast)(const void *) ENTT_NOEXCEPT;
};


struct meta_conv_node {
    meta_type_node * const parent;
    meta_conv_node * next;
    meta_type_node *(* const type)() ENTT_NOEXCEPT;
    meta_any(* const conv)(const void *);
};


struct meta_ctor_node {
    using size_type = std::size_t;
    meta_type_node * const parent;
    meta_ctor_node * next;
    meta_prop_node * prop;
    const size_type arity;
    meta_type(* const arg)(const size_type) ENTT_NOEXCEPT;
    meta_any(* const invoke)(meta_any * const);
};


struct meta_data_node {
    id_type id;
    meta_type_node * const parent;
    meta_data_node * next;
    meta_prop_node * prop;
    const bool is_const;
    const bool is_static;
    meta_type_node *(* const type)() ENTT_NOEXCEPT;
    bool(* const set)(meta_handle, meta_any);
    meta_any(* const get)(meta_handle);
};


struct meta_func_node {
    using size_type = std::size_t;
    id_type id;
    meta_type_node * const parent;
    meta_func_node * next;
    meta_prop_node * prop;
    const size_type arity;
    const bool is_const;
    const bool is_static;
    meta_type_node *(* const ret)() ENTT_NOEXCEPT;
    meta_type(* const arg)(const size_type) ENTT_NOEXCEPT;
    meta_any(* const invoke)(meta_handle, meta_any *);
};


struct meta_template_info {
    using size_type = std::size_t;
    const bool is_template_specialization;
    const size_type arity;
    meta_type_node *(* const type)() ENTT_NOEXCEPT;
    meta_type_node *(* const arg)(const size_type) ENTT_NOEXCEPT;
};


struct meta_type_node {
    using size_type = std::size_t;
    const type_info info;
    id_type id;
    meta_type_node * next;
    meta_prop_node * prop;
    const size_type size_of;
    const bool is_void;
    const bool is_integral;
    const bool is_floating_point;
    const bool is_array;
    const bool is_enum;
    const bool is_union;
    const bool is_class;
    const bool is_pointer;
    const bool is_function_pointer;
    const bool is_member_object_pointer;
    const bool is_member_function_pointer;
    const bool is_pointer_like;
    const bool is_sequence_container;
    const bool is_associative_container;
    const meta_template_info template_info;
    const size_type rank;
    size_type(* const extent)(const size_type) ENTT_NOEXCEPT ;
    meta_type_node *(* const remove_pointer)() ENTT_NOEXCEPT;
    meta_type_node *(* const remove_extent)() ENTT_NOEXCEPT;
    meta_ctor_node * const def_ctor;
    meta_ctor_node *ctor{nullptr};
    meta_base_node *base{nullptr};
    meta_conv_node *conv{nullptr};
    meta_data_node *data{nullptr};
    meta_func_node *func{nullptr};
    void(* dtor)(void *){nullptr};
};


template<auto Member, typename Op, typename Node>
auto meta_visit(const Op &op, const Node *node)
-> std::decay_t<decltype(node->*Member)> {
    for(auto *curr = node->*Member; curr; curr = curr->next) {
        if(op(curr)) {
            return curr;
        }
    }

    if constexpr(std::is_same_v<Node, meta_type_node>) {
        for(auto *curr = node->base; curr; curr = curr->next) {
            if(auto *ret = meta_visit<Member>(op, curr->type()); ret) {
                return ret;
            }
        }
    }

    return nullptr;
}


template<typename... Args>
meta_type_node * meta_arg_node(type_list<Args...>, const std::size_t index) ENTT_NOEXCEPT;


template<typename Type>
class ENTT_API meta_node {
    static_assert(std::is_same_v<Type, std::remove_cv_t<std::remove_reference_t<Type>>>, "Invalid type");

    template<std::size_t... Index>
    [[nodiscard]] static auto extent(const meta_type_node::size_type dim, std::index_sequence<Index...>) ENTT_NOEXCEPT {
        meta_type_node::size_type ext{};
        ((ext = (dim == Index ? std::extent_v<Type, Index> : ext)), ...);
        return ext;
    }

    [[nodiscard]] static meta_ctor_node * meta_default_constructor([[maybe_unused]] meta_type_node *type) ENTT_NOEXCEPT {
        if constexpr(std::is_default_constructible_v<Type>) {
            static meta_ctor_node node{
                type,
                nullptr,
                nullptr,
                0u,
                nullptr,
                [](meta_any * const) { return meta_any{std::in_place_type<Type>}; }
            };

            return &node;
        } else {
            return nullptr;
        }
    }

    [[nodiscard]] static meta_template_info meta_template_descriptor() ENTT_NOEXCEPT {
        if constexpr(is_complete_v<meta_template_traits<Type>>) {
            return {
                true,
                meta_template_traits<Type>::args_type::size,
                &meta_node<typename meta_template_traits<Type>::class_type>::resolve,
                [](const std::size_t index) ENTT_NOEXCEPT {
                    return meta_arg_node(typename meta_template_traits<Type>::args_type{}, index);
                }
            };
        } else {
            return { false, 0u, nullptr, nullptr };
        }
    }

public:
    [[nodiscard]] static meta_type_node * resolve() ENTT_NOEXCEPT {
        static meta_type_node node{
            type_id<Type>(),
            {},
            nullptr,
            nullptr,
            size_of_v<Type>,
            std::is_void_v<Type>,
            std::is_integral_v<Type>,
            std::is_floating_point_v<Type>,
            std::is_array_v<Type>,
            std::is_enum_v<Type>,
            std::is_union_v<Type>,
            std::is_class_v<Type>,
            std::is_pointer_v<Type>,
            std::is_pointer_v<Type> && std::is_function_v<std::remove_pointer_t<Type>>,
            std::is_member_object_pointer_v<Type>,
            std::is_member_function_pointer_v<Type>,
            is_meta_pointer_like_v<Type>,
            is_complete_v<meta_sequence_container_traits<Type>>,
            is_complete_v<meta_associative_container_traits<Type>>,
            meta_template_descriptor(),
            std::rank_v<Type>,
            [](meta_type_node::size_type dim) ENTT_NOEXCEPT { return extent(dim, std::make_index_sequence<std::rank_v<Type>>{}); },
            &meta_node<std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<Type>>>>::resolve,
            &meta_node<std::remove_cv_t<std::remove_reference_t<std::remove_extent_t<Type>>>>::resolve,
            meta_default_constructor(&node),
            meta_default_constructor(&node)
        };

        return &node;
    }
};


template<typename Type>
struct meta_info: meta_node<std::remove_cv_t<std::remove_reference_t<Type>>> {};


template<typename... Args>
meta_type_node * meta_arg_node(type_list<Args...>, const std::size_t index) ENTT_NOEXCEPT {
    meta_type_node *args[sizeof...(Args) + 1u]{nullptr, internal::meta_info<Args>::resolve()...};
    return args[index + 1u];
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


}


#endif

// #include "range.hpp"
#ifndef ENTT_META_RANGE_HPP
#define ENTT_META_RANGE_HPP


#include <cstddef>
#include <iterator>


namespace entt {


/**
 * @brief Iterable range to use to iterate all types of meta objects.
 * @tparam Type Type of meta objects returned.
 * @tparam Node Type of meta nodes iterated.
 */
template<typename Type, typename Node = typename Type::node_type>
class meta_range {
    struct range_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = Type;
        using pointer = void;
        using reference = value_type;
        using iterator_category = std::input_iterator_tag;
        using node_type = Node;

        range_iterator() ENTT_NOEXCEPT = default;

        range_iterator(node_type *head) ENTT_NOEXCEPT
            : it{head}
        {}

        range_iterator & operator++() ENTT_NOEXCEPT {
            return (it = it->next), *this;
        }

        range_iterator operator++(int) ENTT_NOEXCEPT {
            range_iterator orig = *this;
            return ++(*this), orig;
        }

        [[nodiscard]] reference operator*() const ENTT_NOEXCEPT {
            return it;
        }

        [[nodiscard]] bool operator==(const range_iterator &other) const ENTT_NOEXCEPT {
            return other.it == it;
        }

        [[nodiscard]] bool operator!=(const range_iterator &other) const ENTT_NOEXCEPT {
            return !(*this == other);
        }

    private:
        node_type *it{};
    };

public:
    /*! @brief Node type. */
    using node_type = Node;
    /*! @brief Input iterator type. */
    using iterator = range_iterator;

    /*! @brief Default constructor. */
    meta_range() ENTT_NOEXCEPT = default;

    /**
     * @brief Constructs a meta range from a given node.
     * @param head The underlying node with which to construct the range.
     */
    meta_range(node_type *head)
        : node{head}
    {}

    /**
     * @brief Returns an iterator to the beginning.
     * @return An iterator to the first meta object of the range.
     */
    [[nodiscard]] iterator begin() const ENTT_NOEXCEPT {
        return iterator{node};
    }

    /**
     * @brief Returns an iterator to the end.
     * @return An iterator to the element following the last meta object of the
     * range.
     */
    [[nodiscard]] iterator end() const ENTT_NOEXCEPT {
        return iterator{};
    }

private:
    node_type *node{nullptr};
};


}


#endif

// #include "type_traits.hpp"



namespace entt {


class meta_any;
class meta_type;


/*! @brief Proxy object for sequence containers. */
class meta_sequence_container {
    template<typename>
    struct meta_sequence_container_proxy;

    class meta_iterator;

public:
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Meta iterator type. */
    using iterator = meta_iterator;

    /*! @brief Default constructor. */
    meta_sequence_container() ENTT_NOEXCEPT = default;

    /**
     * @brief Construct a proxy object for sequence containers.
     * @tparam Type Type of container to wrap.
     * @param instance The container to wrap.
     */
    template<typename Type>
    meta_sequence_container(std::in_place_type_t<Type>, any instance) ENTT_NOEXCEPT
        : value_type_fn{&meta_sequence_container_proxy<Type>::value_type},
          size_fn{&meta_sequence_container_proxy<Type>::size},
          resize_fn{&meta_sequence_container_proxy<Type>::resize},
          clear_fn{&meta_sequence_container_proxy<Type>::clear},
          begin_fn{&meta_sequence_container_proxy<Type>::begin},
          end_fn{&meta_sequence_container_proxy<Type>::end},
          insert_fn{&meta_sequence_container_proxy<Type>::insert},
          erase_fn{&meta_sequence_container_proxy<Type>::erase},
          get_fn{&meta_sequence_container_proxy<Type>::get},
          storage{std::move(instance)}
    {}

    [[nodiscard]] inline meta_type value_type() const ENTT_NOEXCEPT;
    [[nodiscard]] inline size_type size() const ENTT_NOEXCEPT;
    inline bool resize(size_type);
    inline bool clear();
    [[nodiscard]] inline iterator begin();
    [[nodiscard]] inline iterator end();
    inline std::pair<iterator, bool> insert(iterator, meta_any);
    inline std::pair<iterator, bool> erase(iterator);
    [[nodiscard]] inline meta_any operator[](size_type);
    [[nodiscard]] inline explicit operator bool() const ENTT_NOEXCEPT;

private:
    meta_type(* value_type_fn)() ENTT_NOEXCEPT = nullptr;
    size_type(* size_fn)(const any &) ENTT_NOEXCEPT = nullptr;
    bool(* resize_fn)(any &, size_type) = nullptr;
    bool(* clear_fn)(any &) = nullptr;
    iterator(* begin_fn)(any &) = nullptr;
    iterator(* end_fn)(any &) = nullptr;
    std::pair<iterator, bool>(* insert_fn)(any &, iterator, meta_any &) = nullptr;
    std::pair<iterator, bool>(* erase_fn)(any &, iterator) = nullptr;
    meta_any(* get_fn)(any &, size_type) = nullptr;
    any storage{};
};


/*! @brief Proxy object for associative containers. */
class meta_associative_container {
    template<typename>
    struct meta_associative_container_proxy;

    class meta_iterator;

public:
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Meta iterator type. */
    using iterator = meta_iterator;

    /*! @brief Default constructor. */
    meta_associative_container() ENTT_NOEXCEPT = default;

    /**
     * @brief Construct a proxy object for associative containers.
     * @tparam Type Type of container to wrap.
     * @param instance The container to wrap.
     */
    template<typename Type>
    meta_associative_container(std::in_place_type_t<Type>, any instance) ENTT_NOEXCEPT
        : key_only_container{is_key_only_meta_associative_container_v<Type>},
          key_type_fn{&meta_associative_container_proxy<Type>::key_type},
          mapped_type_fn{&meta_associative_container_proxy<Type>::mapped_type},
          value_type_fn{&meta_associative_container_proxy<Type>::value_type},
          size_fn{&meta_associative_container_proxy<Type>::size},
          clear_fn{&meta_associative_container_proxy<Type>::clear},
          begin_fn{&meta_associative_container_proxy<Type>::begin},
          end_fn{&meta_associative_container_proxy<Type>::end},
          insert_fn{&meta_associative_container_proxy<Type>::insert},
          erase_fn{&meta_associative_container_proxy<Type>::erase},
          find_fn{&meta_associative_container_proxy<Type>::find},
          storage{std::move(instance)}
    {}

    [[nodiscard]] inline bool key_only() const ENTT_NOEXCEPT;
    [[nodiscard]] inline meta_type key_type() const ENTT_NOEXCEPT;
    [[nodiscard]] inline meta_type mapped_type() const ENTT_NOEXCEPT;
    [[nodiscard]] inline meta_type value_type() const ENTT_NOEXCEPT;
    [[nodiscard]] inline size_type size() const ENTT_NOEXCEPT;
    inline bool clear();
    [[nodiscard]] inline iterator begin();
    [[nodiscard]] inline iterator end();
    inline bool insert(meta_any, meta_any);
    inline bool erase(meta_any);
    [[nodiscard]] inline iterator find(meta_any);
    [[nodiscard]] inline explicit operator bool() const ENTT_NOEXCEPT;

private:
    bool key_only_container{};
    meta_type(* key_type_fn)() ENTT_NOEXCEPT = nullptr;
    meta_type(* mapped_type_fn)() ENTT_NOEXCEPT = nullptr;
    meta_type(* value_type_fn)() ENTT_NOEXCEPT = nullptr;
    size_type(* size_fn)(const any &) ENTT_NOEXCEPT = nullptr;
    bool(* clear_fn)(any &) = nullptr;
    iterator(* begin_fn)(any &) = nullptr;
    iterator(* end_fn)(any &) = nullptr;
    bool(* insert_fn)(any &, meta_any &, meta_any &) = nullptr;
    bool(* erase_fn)(any &, meta_any &) = nullptr;
    iterator(* find_fn)(any &, meta_any &) = nullptr;
    any storage{};
};


/*! @brief Opaque wrapper for values of any type. */
class meta_any {
    enum class operation { DTOR, DEREF, SEQ, ASSOC };

    using vtable_type = void(const operation, const any &, void *);

    template<typename Type>
    static void basic_vtable([[maybe_unused]] const operation op, [[maybe_unused]] const any &from, [[maybe_unused]] void *to) {
        static_assert(std::is_same_v<std::remove_reference_t<std::remove_const_t<Type>>, Type>, "Invalid type");

        if constexpr(!std::is_void_v<Type>) {
            switch(op) {
            case operation::DTOR:
                if(auto *curr = static_cast<internal::meta_type_node *>(to); curr->dtor && from.owner()) {
                    curr->dtor(const_cast<any &>(from).data());
                }
                break;
            case operation::DEREF:
                if constexpr(is_meta_pointer_like_v<Type>) {
                    using element_type = std::remove_const_t<typename std::pointer_traits<Type>::element_type>;

                    if constexpr(std::is_function_v<element_type>) {
                        *static_cast<meta_any *>(to) = any_cast<Type>(from);
                    } else if constexpr(!std::is_same_v<std::remove_const_t<typename std::pointer_traits<Type>::element_type>, void>) {
                        using in_place_type = decltype(adl_meta_pointer_like<Type>::dereference(any_cast<const Type &>(from)));
                        static_cast<meta_any *>(to)->emplace<in_place_type>(adl_meta_pointer_like<Type>::dereference(any_cast<const Type &>(from)));
                    }
                }
            case operation::SEQ:
                if constexpr(is_complete_v<meta_sequence_container_traits<Type>>) {
                    *static_cast<meta_sequence_container *>(to) = { std::in_place_type<Type>, std::move(const_cast<any &>(from)) };
                }
                break;
            case operation::ASSOC:
                if constexpr(is_complete_v<meta_associative_container_traits<Type>>) {
                    *static_cast<meta_associative_container *>(to) = { std::in_place_type<Type>, std::move(const_cast<any &>(from)) };
                }
                break;
            }
        }
    }

    meta_any(const meta_any &other, any ref) ENTT_NOEXCEPT
        : storage{std::move(ref)},
          node{storage ? other.node : nullptr},
          vtable{storage ? other.vtable : &basic_vtable<void>}
    {}

public:
    /*! @brief Default constructor. */
    meta_any() ENTT_NOEXCEPT
        : storage{},
          node{},
          vtable{&basic_vtable<void>}
    {}

    /**
     * @brief Constructs a wrapper by directly initializing the new object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    explicit meta_any(std::in_place_type_t<Type>, Args &&... args)
        : storage{std::in_place_type<Type>, std::forward<Args>(args)...},
          node{internal::meta_info<Type>::resolve()},
          vtable{&basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>}
    {}

    /**
     * @brief Constructs a wrapper that holds an unmanaged object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type>
    meta_any(std::reference_wrapper<Type> value)
        : meta_any{}
    {
        // invokes deprecated assignment operator (and avoids issues with vs2017)
        *this = value;
    }

    /**
     * @brief Constructs a wrapper from a given value.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, meta_any>>>
    meta_any(Type &&value)
        : storage{std::forward<Type>(value)},
          node{internal::meta_info<std::decay_t<Type>>::resolve()},
          vtable{&basic_vtable<std::decay_t<Type>>}
    {}

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    meta_any(const meta_any &other) = default;

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    meta_any(meta_any &&other) ENTT_NOEXCEPT
        : storage{std::move(other.storage)},
          node{std::exchange(other.node, nullptr)},
          vtable{std::exchange(other.vtable, &basic_vtable<void>)}
    {}

    /*! @brief Frees the internal storage, whatever it means. */
    ~meta_any() {
        vtable(operation::DTOR, storage, node);
    }

    /**
     * @brief Copy assignment operator.
     * @param other The instance to copy from.
     * @return This meta any object.
     */
    meta_any & operator=(const meta_any &other) {
        std::exchange(vtable, other.vtable)(operation::DTOR, storage, node);
        storage = other.storage;
        node = other.node;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This meta any object.
     */
    meta_any & operator=(meta_any &&other) ENTT_NOEXCEPT {
        std::exchange(vtable, std::exchange(other.vtable, &basic_vtable<void>))(operation::DTOR, storage, node);
        storage = std::move(other.storage);
        node = std::exchange(other.node, nullptr);
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This meta any object.
     */
    template<typename Type>
    [[deprecated("Use std::in_place_type<T &>, entt::make_meta<T &>, emplace<Type &> or forward_as_meta instead")]]
    meta_any & operator=(std::reference_wrapper<Type> value) {
        emplace<Type &>(value.get());
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This meta any object.
     */
    template<typename Type>
    std::enable_if_t<!std::is_same_v<std::decay_t<Type>, meta_any>, meta_any &>
    operator=(Type &&value) {
        emplace<std::decay_t<Type>>(std::forward<Type>(value));
        return *this;
    }

    /**
     * @brief Returns the type of the underlying object.
     * @return The type of the underlying object, if any.
     */
    [[nodiscard]] inline meta_type type() const ENTT_NOEXCEPT;

    /**
     * @brief Returns an opaque pointer to the contained instance.
     * @return An opaque pointer the contained instance, if any.
     */
    [[nodiscard]] const void * data() const ENTT_NOEXCEPT {
        return storage.data();
    }

    /*! @copydoc data */
    [[nodiscard]] void * data() ENTT_NOEXCEPT {
        return storage.data();
    }

    /**
     * @brief Invokes the underlying function, if possible.
     *
     * @sa meta_func::invoke
     *
     * @tparam Args Types of arguments to use to invoke the function.
     * @param id Unique identifier.
     * @param args Parameters to use to invoke the function.
     * @return A wrapper containing the returned value, if any.
     */
    template<typename... Args>
    meta_any invoke(const id_type id, Args &&... args) const;

    /*! @copydoc invoke */
    template<typename... Args>
    meta_any invoke(const id_type id, Args &&... args);

    /**
     * @brief Sets the value of a given variable.
     *
     * The type of the value must be such that a cast or conversion to the type
     * of the variable is possible. Otherwise, invoking the setter does nothing.
     *
     * @tparam Type Type of value to assign.
     * @param id Unique identifier.
     * @param value Parameter to use to set the underlying variable.
     * @return True in case of success, false otherwise.
     */
    template<typename Type>
    bool set(const id_type id, Type &&value);

    /**
     * @brief Gets the value of a given variable.
     * @param id Unique identifier.
     * @return A wrapper containing the value of the underlying variable.
     */
    [[nodiscard]] meta_any get(const id_type id) const;

    /*! @copydoc get */
    [[nodiscard]] meta_any get(const id_type id);

    /**
     * @brief Tries to cast an instance to a given type.
     * @tparam Type Type to which to cast the instance.
     * @return A (possibly null) pointer to the contained instance.
     */
    template<typename Type>
    [[nodiscard]] const Type * try_cast() const {
        if(node) {
            if(const auto info = type_id<Type>(); node->info == info) {
                return any_cast<Type>(&storage);
            } else if(const auto *base = internal::meta_visit<&internal::meta_type_node::base>([info](const auto *curr) { return curr->type()->info == info; }, node); base) {
                return static_cast<const Type *>(base->cast(storage.data()));
            }
        }

        return nullptr;
    }

    /*! @copydoc try_cast */
    template<typename Type>
    [[nodiscard]] Type * try_cast() {
        if(node) {
            if(const auto info = type_id<Type>(); node->info == info) {
                return any_cast<Type>(&storage);
            } else if(const auto *base = internal::meta_visit<&internal::meta_type_node::base>([info](const auto *curr) { return curr->type()->info == info; }, node); base) {
                return static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(base->cast(static_cast<constness_as_t<any, Type> &>(storage).data())));
            }
        }

        return nullptr;
    }

    /**
     * @brief Tries to cast an instance to a given type.
     *
     * The type of the instance must be such that the cast is possible.
     *
     * @warning
     * Attempting to perform an invalid cast results in undefined behavior.
     *
     * @tparam Type Type to which to cast the instance.
     * @return A reference to the contained instance.
     */
    template<typename Type>
    [[nodiscard]] Type cast() const {
        auto * const instance = try_cast<std::remove_reference_t<Type>>();
        ENTT_ASSERT(instance, "Invalid instance");
        return static_cast<Type>(*instance);
    }

    /*! @copydoc cast */
    template<typename Type>
    [[nodiscard]] Type cast() {
        // forces const on non-reference types to make them work also with wrappers for const references
        auto * const instance = try_cast<std::remove_reference_t<const Type>>();
        ENTT_ASSERT(instance, "Invalid instance");
        return static_cast<Type>(*instance);
    }

    /**
     * @brief Converts an object in such a way that a given cast becomes viable.
     * @tparam Type Type to which the cast is requested.
     * @return A valid meta any object if there exists a viable conversion, an
     * invalid one otherwise.
     */
    template<typename Type>
    [[nodiscard]] meta_any allow_cast() const {
        if(try_cast<std::remove_reference_t<Type>>() != nullptr) {
            return as_ref();
        } else if(node) {
            if(const auto * const conv = internal::meta_visit<&internal::meta_type_node::conv>([info = type_id<Type>()](const auto *curr) { return curr->type()->info == info; }, node); conv) {
                return conv->conv(storage.data());
            }
        }

        return {};
    }

    /**
     * @brief Converts an object in such a way that a given cast becomes viable.
     * @tparam Type Type to which the cast is requested.
     * @return True if there exists a viable conversion, false otherwise.
     */
    template<typename Type>
    bool allow_cast() {
        // forces const on non-reference types to make them work also with wrappers for const references
        if(try_cast<std::remove_reference_t<const Type>>() != nullptr) {
            return true;
        } else if(node) {
            if(const auto * const conv = internal::meta_visit<&internal::meta_type_node::conv>([info = type_id<Type>()](const auto *curr) { return curr->type()->info == info; }, node); conv) {
                *this = conv->conv(std::as_const(storage).data());
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Replaces the contained object by creating a new instance directly.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    void emplace(Args &&... args) {
        std::exchange(vtable, &basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>)(operation::DTOR, storage, node);
        storage.emplace<Type>(std::forward<Args>(args)...);
        node = internal::meta_info<Type>::resolve();
    }

    /*! @brief Destroys contained object */
    void reset() {
        std::exchange(vtable, &basic_vtable<void>)(operation::DTOR, storage, node);
        storage.reset();
        node = nullptr;
    }

    /**
     * @brief Returns a sequence container proxy.
     * @return A sequence container proxy for the underlying object.
     */
    [[nodiscard]] meta_sequence_container as_sequence_container() ENTT_NOEXCEPT {
        meta_sequence_container proxy;
        vtable(operation::SEQ, storage.as_ref(), &proxy);
        return proxy;
    }

    /*! @copydoc as_sequence_container */
    [[nodiscard]] meta_sequence_container as_sequence_container() const ENTT_NOEXCEPT {
        meta_sequence_container proxy;
        vtable(operation::SEQ, storage.as_ref(), &proxy);
        return proxy;
    }

    /**
     * @brief Returns an associative container proxy.
     * @return An associative container proxy for the underlying object.
     */
    [[nodiscard]] meta_associative_container as_associative_container() ENTT_NOEXCEPT {
        meta_associative_container proxy;
        vtable(operation::ASSOC, storage.as_ref(), &proxy);
        return proxy;
    }

    /*! @copydoc as_associative_container */
    [[nodiscard]] meta_associative_container as_associative_container() const ENTT_NOEXCEPT {
        meta_associative_container proxy;
        vtable(operation::ASSOC, storage.as_ref(), &proxy);
        return proxy;
    }

    /**
     * @brief Indirection operator for dereferencing opaque objects.
     * @return A wrapper that shares a reference to an unmanaged object if the
     * wrapped element is dereferenceable, an invalid meta any otherwise.
     */
    [[nodiscard]] meta_any operator*() const ENTT_NOEXCEPT {
        meta_any ret{};
        vtable(operation::DEREF, storage, &ret);
        return ret;
    }

    /**
     * @brief Returns false if a wrapper is invalid, true otherwise.
     * @return False if the wrapper is invalid, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(node == nullptr);
    }

    /**
     * @brief Checks if two wrappers differ in their content.
     * @param other Wrapper with which to compare.
     * @return False if the two objects differ in their content, true otherwise.
     */
    [[nodiscard]] bool operator==(const meta_any &other) const {
        return (!node && !other.node) || (node && other.node && node->info == other.node->info && storage == other.storage);
    }

    /**
     * @brief Aliasing constructor.
     * @return A wrapper that shares a reference to an unmanaged object.
     */
    [[nodiscard]] meta_any as_ref() ENTT_NOEXCEPT {
        return meta_any{*this, storage.as_ref()};
    }

    /*! @copydoc as_ref */
    [[nodiscard]] meta_any as_ref() const ENTT_NOEXCEPT {
        return meta_any{*this, storage.as_ref()};
    }

private:
    any storage;
    internal::meta_type_node *node;
    vtable_type *vtable;
};


/**
 * @brief Checks if two wrappers differ in their content.
 * @param lhs A wrapper, either empty or not.
 * @param rhs A wrapper, either empty or not.
 * @return True if the two wrappers differ in their content, false otherwise.
 */
[[nodiscard]] inline bool operator!=(const meta_any &lhs, const meta_any &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Constructs a wrapper from a given type, passing it all arguments.
 * @tparam Type Type of object to use to initialize the wrapper.
 * @tparam Args Types of arguments to use to construct the new instance.
 * @param args Parameters to use to construct the instance.
 * @return A properly initialized wrapper for an object of the given type.
 */
template<typename Type, typename... Args>
meta_any make_meta(Args &&... args) {
    return meta_any{std::in_place_type<Type>, std::forward<Args>(args)...};
}


/**
 * @brief Forwards its argument and avoids copies for lvalue references.
 * @tparam Type Type of argument to use to construct the new instance.
 * @param value Parameter to use to construct the instance.
 * @return A properly initialized and not necessarily owning wrapper.
 */
template<typename Type>
meta_any forward_as_meta(Type &&value) {
    return meta_any{std::in_place_type<std::conditional_t<std::is_rvalue_reference_v<Type>, std::decay_t<Type>, Type>>, std::forward<Type>(value)};
}


/**
 * @brief Opaque pointers to instances of any type.
 *
 * A handle doesn't perform copies and isn't responsible for the contained
 * object. It doesn't prolong the lifetime of the pointed instance.<br/>
 * Handles are used to generate references to actual objects when needed.
 */
struct meta_handle {
    /*! @brief Default constructor. */
    meta_handle() = default;


    /*! @brief Default copy constructor, deleted on purpose. */
    meta_handle(const meta_handle &) = delete;

    /*! @brief Default move constructor. */
    meta_handle(meta_handle &&) = default;

    /**
     * @brief Default copy assignment operator, deleted on purpose.
     * @return This meta handle.
     */
    meta_handle & operator=(const meta_handle &) = delete;

    /**
     * @brief Default move assignment operator.
     * @return This meta handle.
     */
    meta_handle & operator=(meta_handle &&) = default;

    /**
     * @brief Creates a handle that points to an unmanaged object.
     * @tparam Type Type of object to use to initialize the handle.
     * @param value An instance of an object to use to initialize the handle.
     */
    template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, meta_handle>>>
    meta_handle(Type &value) ENTT_NOEXCEPT
        : meta_handle{}
    {
        if constexpr(std::is_same_v<std::decay_t<Type>, meta_any>) {
            any = value.as_ref();
        } else {
            any.emplace<Type &>(value);
        }
    }

    /**
     * @brief Returns false if a handle is invalid, true otherwise.
     * @return False if the handle is invalid, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return static_cast<bool>(any);
    }

    /**
     * @brief Access operator for accessing the contained opaque object.
     * @return A wrapper that shares a reference to an unmanaged object.
     */
    [[nodiscard]] meta_any * operator->() {
        return &any;
    }

    /*! @copydoc operator-> */
    [[nodiscard]] const meta_any * operator->() const {
        return &any;
    }

private:
    meta_any any;
};


/*! @brief Opaque wrapper for properties of any type. */
struct meta_prop {
    /*! @brief Node type. */
    using node_type = internal::meta_prop_node;

    /**
     * @brief Constructs an instance from a given node.
     * @param curr The underlying node with which to construct the instance.
     */
    meta_prop(const node_type *curr = nullptr) ENTT_NOEXCEPT
        : node{curr}
    {}

    /**
     * @brief Returns the stored key as a const reference.
     * @return A wrapper containing the key stored with the property.
     */
    [[nodiscard]] meta_any key() const {
        return node->id.as_ref();
    }

    /**
     * @brief Returns the stored value by copy.
     * @return A wrapper containing the value stored with the property.
     */
    [[nodiscard]] meta_any value() const {
        return node->value;
    }

    /**
     * @brief Returns true if an object is valid, false otherwise.
     * @return True if the object is valid, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(node == nullptr);
    }

private:
    const node_type *node;
};


/*! @brief Opaque wrapper for constructors. */
struct meta_ctor {
    /*! @brief Node type. */
    using node_type = internal::meta_ctor_node;
    /*! @brief Unsigned integer type. */
    using size_type = typename node_type::size_type;

    /*! @copydoc meta_prop::meta_prop */
    meta_ctor(const node_type *curr = nullptr) ENTT_NOEXCEPT
        : node{curr}
    {}

    /**
     * @brief Returns the type to which an object belongs.
     * @return The type to which the object belongs.
     */
    [[nodiscard]] inline meta_type parent() const ENTT_NOEXCEPT;

    /**
     * @brief Returns the number of arguments accepted by a constructor.
     * @return The number of arguments accepted by the constructor.
     */
    [[nodiscard]] size_type arity() const ENTT_NOEXCEPT {
        return node->arity;
    }

    /**
     * @brief Returns the type of the i-th argument of a constructor.
     * @param index Index of the argument of which to return the type.
     * @return The type of the i-th argument of a constructor.
     */
    [[nodiscard]] meta_type arg(size_type index) const ENTT_NOEXCEPT;

    /**
     * @brief Creates an instance of the underlying type, if possible.
     *
     * Parameters must be such that a cast or conversion to the required types
     * is possible. Otherwise, an empty and thus invalid wrapper is returned.
     *
     * @param args Parameters to use to construct the instance.
     * @param sz Number of parameters to use to construct the instance.
     * @return A wrapper containing the new instance, if any.
     */
    [[nodiscard]] meta_any invoke(meta_any * const args, const size_type sz) const {
        return sz == arity() ? node->invoke(args) : meta_any{};
    }

    /**
     * @copybrief invoke
     *
     * @sa invoke
     *
     * @tparam Args Types of arguments to use to construct the instance.
     * @param args Parameters to use to construct the instance.
     * @return A wrapper containing the new instance, if any.
     */
    template<typename... Args>
    [[nodiscard]] meta_any invoke([[maybe_unused]] Args &&... args) const {
        meta_any arguments[sizeof...(Args) + 1u]{std::forward<Args>(args)...};
        return invoke(arguments, sizeof...(Args));
    }

    /**
     * @brief Returns a range to use to visit all properties.
     * @return An iterable range to use to visit all properties.
     */
    [[nodiscard]] meta_range<meta_prop> prop() const ENTT_NOEXCEPT {
        return node->prop;
    }

    /**
     * @brief Returns the property associated with a given key.
     * @param key The key to use to search for a property.
     * @return The property associated with the given key, if any.
     */
    [[nodiscard]] meta_prop prop(meta_any key) const {
        return internal::meta_visit<&node_type::prop>([&key](const auto *curr) { return curr->id == key; }, node);
    }

    /**
     * @brief Returns true if an object is valid, false otherwise.
     * @return True if the object is valid, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(node == nullptr);
    }

private:
    const node_type *node;
};


/*! @brief Opaque wrapper for data members. */
struct meta_data {
    /*! @brief Node type. */
    using node_type = internal::meta_data_node;

    /*! @copydoc meta_prop::meta_prop */
    meta_data(const node_type *curr = nullptr) ENTT_NOEXCEPT
        : node{curr}
    {}

    /*! @copydoc meta_type::id */
    [[nodiscard]] id_type id() const ENTT_NOEXCEPT {
        return node->id;
    }

    /*! @copydoc meta_ctor::parent */
    [[nodiscard]] inline meta_type parent() const ENTT_NOEXCEPT;

    /**
     * @brief Indicates whether a data member is constant or not.
     * @return True if the data member is constant, false otherwise.
     */
    [[nodiscard]] bool is_const() const ENTT_NOEXCEPT {
        return node->is_const;
    }

    /**
     * @brief Indicates whether a data member is static or not.
     * @return True if the data member is static, false otherwise.
     */
    [[nodiscard]] bool is_static() const ENTT_NOEXCEPT {
        return node->is_static;
    }

    /*! @copydoc meta_any::type */
    [[nodiscard]] inline meta_type type() const ENTT_NOEXCEPT;

    /**
     * @brief Sets the value of a given variable.
     *
     * It must be possible to cast the instance to the parent type of the data
     * member. Otherwise, invoking the setter results in an undefined
     * behavior.<br/>
     * The type of the value must be such that a cast or conversion to the type
     * of the variable is possible. Otherwise, invoking the setter does nothing.
     *
     * @tparam Type Type of value to assign.
     * @param instance An opaque instance of the underlying type.
     * @param value Parameter to use to set the underlying variable.
     * @return True in case of success, false otherwise.
     */
    template<typename Type>
    bool set(meta_handle instance, Type &&value) const {
        return node->set && node->set(std::move(instance), std::forward<Type>(value));
    }

    /**
     * @brief Gets the value of a given variable.
     *
     * It must be possible to cast the instance to the parent type of the data
     * member. Otherwise, invoking the getter results in an undefined behavior.
     *
     * @param instance An opaque instance of the underlying type.
     * @return A wrapper containing the value of the underlying variable.
     */
    [[nodiscard]] meta_any get(meta_handle instance) const {
        return node->get(std::move(instance));
    }

    /*! @copydoc meta_ctor::prop */
    [[nodiscard]] meta_range<meta_prop> prop() const ENTT_NOEXCEPT {
        return node->prop;
    }

    /**
     * @brief Returns the property associated with a given key.
     * @param key The key to use to search for a property.
     * @return The property associated with the given key, if any.
     */
    [[nodiscard]] meta_prop prop(meta_any key) const {
        return internal::meta_visit<&node_type::prop>([&key](const auto *curr) { return curr->id == key; }, node);
    }

    /**
     * @brief Returns true if an object is valid, false otherwise.
     * @return True if the object is valid, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(node == nullptr);
    }

private:
    const node_type *node;
};


/*! @brief Opaque wrapper for member functions. */
struct meta_func {
    /*! @brief Node type. */
    using node_type = internal::meta_func_node;
    /*! @brief Unsigned integer type. */
    using size_type = typename node_type::size_type;

    /*! @copydoc meta_prop::meta_prop */
    meta_func(const node_type *curr = nullptr) ENTT_NOEXCEPT
        : node{curr}
    {}

    /*! @copydoc meta_type::id */
    [[nodiscard]] id_type id() const ENTT_NOEXCEPT {
        return node->id;
    }

    /*! @copydoc meta_ctor::parent */
    [[nodiscard]] inline meta_type parent() const ENTT_NOEXCEPT;

    /**
     * @brief Returns the number of arguments accepted by a member function.
     * @return The number of arguments accepted by the member function.
     */
    [[nodiscard]] size_type arity() const ENTT_NOEXCEPT {
        return node->arity;
    }

    /**
     * @brief Indicates whether a member function is constant or not.
     * @return True if the member function is constant, false otherwise.
     */
    [[nodiscard]] bool is_const() const ENTT_NOEXCEPT {
        return node->is_const;
    }

    /**
     * @brief Indicates whether a member function is static or not.
     * @return True if the member function is static, false otherwise.
     */
    [[nodiscard]] bool is_static() const ENTT_NOEXCEPT {
        return node->is_static;
    }

    /**
     * @brief Returns the return type of a member function.
     * @return The return type of the member function.
     */
    [[nodiscard]] inline meta_type ret() const ENTT_NOEXCEPT;

    /**
     * @brief Returns the type of the i-th argument of a member function.
     * @param index Index of the argument of which to return the type.
     * @return The type of the i-th argument of a member function.
     */
    [[nodiscard]] inline meta_type arg(size_type index) const ENTT_NOEXCEPT;

    /**
     * @brief Invokes the underlying function, if possible.
     *
     * To invoke a member function, the parameters must be such that a cast or
     * conversion to the required types is possible. Otherwise, an empty and
     * thus invalid wrapper is returned.<br/>
     * It must be possible to cast the instance to the parent type of the member
     * function. Otherwise, invoking the underlying function results in an
     * undefined behavior.
     *
     * @param instance An opaque instance of the underlying type.
     * @param args Parameters to use to invoke the function.
     * @param sz Number of parameters to use to invoke the function.
     * @return A wrapper containing the returned value, if any.
     */
    meta_any invoke(meta_handle instance, meta_any * const args, const size_type sz) const {
        return sz == arity() ? node->invoke(std::move(instance), args) : meta_any{};
    }

    /**
     * @copybrief invoke
     *
     * @sa invoke
     *
     * @tparam Args Types of arguments to use to invoke the function.
     * @param instance An opaque instance of the underlying type.
     * @param args Parameters to use to invoke the function.
     * @return A wrapper containing the new instance, if any.
     */
    template<typename... Args>
    meta_any invoke(meta_handle instance, Args &&... args) const {
        meta_any arguments[sizeof...(Args) + 1u]{std::forward<Args>(args)...};
        return invoke(std::move(instance), arguments, sizeof...(Args));
    }

    /*! @copydoc meta_ctor::prop */
    [[nodiscard]] meta_range<meta_prop> prop() const ENTT_NOEXCEPT {
        return node->prop;
    }

    /**
     * @brief Returns the property associated with a given key.
     * @param key The key to use to search for a property.
     * @return The property associated with the given key, if any.
     */
    [[nodiscard]] meta_prop prop(meta_any key) const {
        return internal::meta_visit<&node_type::prop>([&key](const auto *curr) { return curr->id == key; }, node);
    }

    /**
     * @brief Returns true if an object is valid, false otherwise.
     * @return True if the object is valid, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(node == nullptr);
    }

private:
    const node_type *node;
};


/*! @brief Opaque wrapper for types. */
class meta_type {
    static bool can_cast_or_convert(const internal::meta_type_node *type, const type_info info) ENTT_NOEXCEPT {
        if(type->info == info) {
            return true;
        }

        for(const auto *curr = type->conv; curr; curr = curr->next) {
            if(curr->type()->info == info) {
                return true;
            }
        }

        for(const auto *curr = type->base; curr; curr = curr->next) {
            if(auto *target = curr->type(); can_cast_or_convert(target, info)) {
                return true;
            }
        }

        return false;
    }

    template<typename... Args, auto... Index>
    [[nodiscard]] static const internal::meta_ctor_node * ctor(const internal::meta_ctor_node *curr, std::index_sequence<Index...>) {
        for(; curr; curr = curr->next) {
            if(curr->arity == sizeof...(Args) && (can_cast_or_convert(internal::meta_info<Args>::resolve(), curr->arg(Index).info()) && ...)) {
                return curr;
            }
        }

        return nullptr;
    }

    template<auto... Member, typename Node>
    void unregister_all(Node **curr) {
        while(*curr) {
            (unregister_all(&((*curr)->*Member)), ...);
            *curr = std::exchange((*curr)->next, nullptr);
        }
    }

public:
    /*! @brief Node type. */
    using node_type = internal::meta_type_node;
    /*! @brief Node type. */
    using base_node_type = internal::meta_base_node;
    /*! @brief Unsigned integer type. */
    using size_type = typename node_type::size_type;

    /*! @copydoc meta_prop::meta_prop */
    meta_type(node_type *curr = nullptr) ENTT_NOEXCEPT
        : node{curr}
    {}

    /**
     * @brief Constructs an instance from a given base node.
     * @param curr The base node with which to construct the instance.
     */
    meta_type(base_node_type *curr) ENTT_NOEXCEPT
        : node{curr ? curr->type() : nullptr}
    {}

    /**
     * @brief Returns the type info object of the underlying type.
     * @return The type info object of the underlying type.
     */
    [[nodiscard]] type_info info() const ENTT_NOEXCEPT {
        return node->info;
    }

    /**
     * @brief Returns the identifier assigned to a type.
     * @return The identifier assigned to the type.
     */
    [[nodiscard]] id_type id() const ENTT_NOEXCEPT {
        return node->id;
    }

    /**
     * @brief Returns the size of the underlying type if known.
     * @return The size of the underlying type if known, 0 otherwise.
     */
    [[nodiscard]] size_type size_of() const ENTT_NOEXCEPT {
        return node->size_of;
    }

    /**
     * @brief Checks whether a type refers to void or not.
     * @return True if the underlying type is void, false otherwise.
     */
    [[nodiscard]] bool is_void() const ENTT_NOEXCEPT {
        return node->is_void;
    }

    /**
     * @brief Checks whether a type refers to an integral type or not.
     * @return True if the underlying type is an integral type, false otherwise.
     */
    [[nodiscard]] bool is_integral() const ENTT_NOEXCEPT {
        return node->is_integral;
    }

    /**
     * @brief Checks whether a type refers to a floating-point type or not.
     * @return True if the underlying type is a floating-point type, false
     * otherwise.
     */
    [[nodiscard]] bool is_floating_point() const ENTT_NOEXCEPT {
        return node->is_floating_point;
    }

    /**
     * @brief Checks whether a type refers to an array type or not.
     * @return True if the underlying type is an array type, false otherwise.
     */
    [[nodiscard]] bool is_array() const ENTT_NOEXCEPT {
        return node->is_array;
    }

    /**
     * @brief Checks whether a type refers to an enum or not.
     * @return True if the underlying type is an enum, false otherwise.
     */
    [[nodiscard]] bool is_enum() const ENTT_NOEXCEPT {
        return node->is_enum;
    }

    /**
     * @brief Checks whether a type refers to an union or not.
     * @return True if the underlying type is an union, false otherwise.
     */
    [[nodiscard]] bool is_union() const ENTT_NOEXCEPT {
        return node->is_union;
    }

    /**
     * @brief Checks whether a type refers to a class or not.
     * @return True if the underlying type is a class, false otherwise.
     */
    [[nodiscard]] bool is_class() const ENTT_NOEXCEPT {
        return node->is_class;
    }

    /**
     * @brief Checks whether a type refers to a pointer or not.
     * @return True if the underlying type is a pointer, false otherwise.
     */
    [[nodiscard]] bool is_pointer() const ENTT_NOEXCEPT {
        return node->is_pointer;
    }

    /**
     * @brief Checks whether a type refers to a function pointer or not.
     * @return True if the underlying type is a function pointer, false
     * otherwise.
     */
    [[nodiscard]] bool is_function_pointer() const ENTT_NOEXCEPT {
        return node->is_function_pointer;
    }

    /**
     * @brief Checks whether a type refers to a pointer to data member or not.
     * @return True if the underlying type is a pointer to data member, false
     * otherwise.
     */
    [[nodiscard]] bool is_member_object_pointer() const ENTT_NOEXCEPT {
        return node->is_member_object_pointer;
    }

    /**
     * @brief Checks whether a type refers to a pointer to member function or
     * not.
     * @return True if the underlying type is a pointer to member function,
     * false otherwise.
     */
    [[nodiscard]] bool is_member_function_pointer() const ENTT_NOEXCEPT {
        return node->is_member_function_pointer;
    }

    /**
     * @brief Checks whether a type is a pointer-like type or not.
     * @return True if the underlying type is a pointer-like one, false
     * otherwise.
     */
    [[nodiscard]] bool is_pointer_like() const ENTT_NOEXCEPT {
        return node->is_pointer_like;
    }

    /**
     * @brief Checks whether a type refers to a sequence container or not.
     * @return True if the type is a sequence container, false otherwise.
     */
    [[nodiscard]] bool is_sequence_container() const ENTT_NOEXCEPT {
        return node->is_sequence_container;
    }

    /**
     * @brief Checks whether a type refers to an associative container or not.
     * @return True if the type is an associative container, false otherwise.
     */
    [[nodiscard]] bool is_associative_container() const ENTT_NOEXCEPT {
        return node->is_associative_container;
    }

    /**
     * @brief Checks whether a type refers to a recognized class template
     * specialization or not.
     * @return True if the type is a recognized class template specialization,
     * false otherwise.
     */
    [[nodiscard]] bool is_template_specialization() const ENTT_NOEXCEPT {
        return node->template_info.is_template_specialization;
    }

    /**
     * @brief Returns the number of template arguments, if any.
     * @return The number of template arguments, if any.
     */
    [[nodiscard]] size_type template_arity() const ENTT_NOEXCEPT {
        return node->template_info.arity;
    }

    /**
     * @brief Returns a tag for the class template of the underlying type.
     *
     * @sa meta_class_template_tag
     *
     * @return The tag for the class template of the underlying type.
     */
    [[nodiscard]] inline meta_type template_type() const ENTT_NOEXCEPT {
        return is_template_specialization() ?  node->template_info.type() : meta_type{};
    }

    /**
     * @brief Returns the type of the i-th template argument of a type.
     * @param index Index of the template argument of which to return the type.
     * @return The type of the i-th template argument of a type.
     */
    [[nodiscard]] inline meta_type template_arg(size_type index) const ENTT_NOEXCEPT {
        return index < template_arity() ? node->template_info.arg(index) : meta_type{};
    }

    /**
     * @brief Provides the number of dimensions of an array type.
     * @return The number of dimensions in case of array types, 0 otherwise.
     */
    [[nodiscard]] size_type rank() const ENTT_NOEXCEPT {
        return node->rank;
    }

    /**
     * @brief The number of elements along the given dimension of an array type.
     * @param dim The dimension of which to return the number of elements.
     * @return The number of elements along the given dimension in case of array
     * types, 0 otherwise.
     */
    [[nodiscard]] size_type extent(size_type dim = {}) const ENTT_NOEXCEPT {
        return node->extent(dim);
    }

    /**
     * @brief Provides the type for which the pointer is defined.
     * @return The type for which the pointer is defined or this type if it
     * doesn't refer to a pointer type.
     */
    [[nodiscard]] meta_type remove_pointer() const ENTT_NOEXCEPT {
        return node->remove_pointer();
    }

    /**
     * @brief Provides the type for which the array is defined.
     * @return The type for which the array is defined or this type if it
     * doesn't refer to an array type.
     */
    [[nodiscard]] meta_type remove_extent() const ENTT_NOEXCEPT {
        return node->remove_extent();
    }

    /**
     * @brief Returns a range to use to visit top-level base meta types.
     * @return An iterable range to use to visit top-level base meta types.
     */
    [[nodiscard]] meta_range<meta_type, internal::meta_base_node> base() const ENTT_NOEXCEPT {
        return node->base;
    }

    /**
     * @brief Returns the base meta type associated with a given identifier.
     * @param id Unique identifier.
     * @return The base meta type associated with the given identifier, if any.
     */
    [[nodiscard]] meta_type base(const id_type id) const {
        return internal::meta_visit<&node_type::base>([id](const auto *curr) { return curr->type()->id == id; }, node);
    }

    /**
     * @brief Returns a range to use to visit top-level constructors.
     * @return An iterable range to use to visit top-level constructors.
     */
    [[nodiscard]] meta_range<meta_ctor> ctor() const ENTT_NOEXCEPT {
        return node->ctor;
    }

    /**
     * @brief Returns a constructor for a given list of types of arguments.
     * @tparam Args Constructor arguments.
     * @return The requested constructor, if any.
     */
    template<typename... Args>
    [[nodiscard]] meta_ctor ctor() const {
        return ctor<Args...>(node->ctor, std::make_index_sequence<sizeof...(Args)>{});
    }

    /**
     * @brief Returns a range to use to visit top-level data.
     * @return An iterable range to use to visit top-level data.
     */
    [[nodiscard]] meta_range<meta_data> data() const ENTT_NOEXCEPT {
        return node->data;
    }

    /**
     * @brief Returns the data associated with a given identifier.
     *
     * The data of the base classes will also be visited, if any.
     *
     * @param id Unique identifier.
     * @return The data associated with the given identifier, if any.
     */
    [[nodiscard]] meta_data data(const id_type id) const {
        return internal::meta_visit<&node_type::data>([id](const auto *curr) { return curr->id == id; }, node);
    }

    /**
     * @brief Returns a range to use to visit top-level functions.
     * @return An iterable range to use to visit top-level functions.
     */
    [[nodiscard]] meta_range<meta_func> func() const ENTT_NOEXCEPT {
        return node->func;
    }

    /**
     * @brief Returns the function associated with a given identifier.
     *
     * The functions of the base classes will also be visited, if any.<br/>
     * In the case of overloaded functions, the first one with the required
     * identifier will be returned.
     *
     * @param id Unique identifier.
     * @return The function associated with the given identifier, if any.
     */
    [[nodiscard]] meta_func func(const id_type id) const {
        return internal::meta_visit<&node_type::func>([id](const auto *curr) { return curr->id == id; }, node);
    }

    /**
     * @brief Creates an instance of the underlying type, if possible.
     *
     * Parameters must be such that a cast or conversion to the required types
     * is possible. Otherwise, an empty and thus invalid wrapper is returned.
     *
     * @param args Parameters to use to construct the instance.
     * @param sz Number of parameters to use to construct the instance.
     * @return A wrapper containing the new instance, if any.
     */
    [[nodiscard]] meta_any construct(meta_any * const args, const size_type sz) const {
        meta_any ret{};
        internal::meta_visit<&node_type::ctor>([args, sz, &ret](const auto *curr) { return (curr->arity == sz) && (ret = curr->invoke(args)); }, node);
        return ret;
    }

    /**
     * @copybrief construct
     *
     * @sa construct
     *
     * @tparam Args Types of arguments to use to construct the instance.
     * @param args Parameters to use to construct the instance.
     * @return A wrapper containing the new instance, if any.
     */
    template<typename... Args>
    [[nodiscard]] meta_any construct(Args &&... args) const {
        meta_any arguments[sizeof...(Args) + 1u]{std::forward<Args>(args)...};
        return construct(arguments, sizeof...(Args));
    }

    /**
     * @brief Invokes a function given an identifier, if possible.
     *
     * It must be possible to cast the instance to the parent type of the member
     * function. Otherwise, invoking the underlying function results in an
     * undefined behavior.
     *
     * @sa meta_func::invoke
     *
     * @param id Unique identifier.
     * @param instance An opaque instance of the underlying type.
     * @param args Parameters to use to invoke the function.
     * @param sz Number of parameters to use to invoke the function.
     * @return A wrapper containing the returned value, if any.
     */
    meta_any invoke(const id_type id, meta_handle instance, meta_any * const args, const size_type sz) const {
        const internal::meta_func_node* candidate{};
        size_type extent{sz + 1u};
        bool ambiguous{};

        for(auto *it = internal::meta_visit<&node_type::func>([id, sz](const auto *curr) { return curr->id == id && curr->arity == sz; }, node); it && it->id == id && it->arity == sz; it = it->next) {
            size_type direct{};
            size_type ext{};

            for(size_type next{}; next < sz && next == (direct + ext); ++next) {
                const auto type = args[next].type();
                const auto req = it->arg(next).info();
                type.info() == req ? ++direct : (ext += can_cast_or_convert(type.node, req));
            }

            if((direct + ext) == sz) {
                if(ext < extent) {
                    candidate = it;
                    extent = ext;
                    ambiguous = false;
                } else if(ext == extent) {
                    ambiguous = true;
                }
            }
        }

        return (candidate && !ambiguous) ? candidate->invoke(std::move(instance), args) : meta_any{};
    }

    /**
     * @copybrief invoke
     *
     * @sa invoke
     *
     * @param id Unique identifier.
     * @tparam Args Types of arguments to use to invoke the function.
     * @param instance An opaque instance of the underlying type.
     * @param args Parameters to use to invoke the function.
     * @return A wrapper containing the new instance, if any.
     */
    template<typename... Args>
    meta_any invoke(const id_type id, meta_handle instance, Args &&... args) const {
        meta_any arguments[sizeof...(Args) + 1u]{std::forward<Args>(args)...};
        return invoke(id, std::move(instance), arguments, sizeof...(Args));
    }

    /**
     * @brief Sets the value of a given variable.
     *
     * It must be possible to cast the instance to the parent type of the data
     * member. Otherwise, invoking the setter results in an undefined
     * behavior.<br/>
     * The type of the value must be such that a cast or conversion to the type
     * of the variable is possible. Otherwise, invoking the setter does nothing.
     *
     * @tparam Type Type of value to assign.
     * @param id Unique identifier.
     * @param instance An opaque instance of the underlying type.
     * @param value Parameter to use to set the underlying variable.
     * @return True in case of success, false otherwise.
     */
    template<typename Type>
    bool set(const id_type id, meta_handle instance, Type &&value) const {
        const auto candidate = data(id);
        return candidate && candidate.set(std::move(instance), std::forward<Type>(value));
    }

    /**
     * @brief Gets the value of a given variable.
     *
     * It must be possible to cast the instance to the parent type of the data
     * member. Otherwise, invoking the getter results in an undefined behavior.
     *
     * @param id Unique identifier.
     * @param instance An opaque instance of the underlying type.
     * @return A wrapper containing the value of the underlying variable.
     */
    [[nodiscard]] meta_any get(const id_type id, meta_handle instance) const {
        const auto candidate = data(id);
        return candidate ? candidate.get(std::move(instance)) : meta_any{};
    }

    /**
     * @brief Returns a range to use to visit top-level properties.
     * @return An iterable range to use to visit top-level properties.
     */
    [[nodiscard]] meta_range<meta_prop> prop() const ENTT_NOEXCEPT {
        return node->prop;
    }

    /**
     * @brief Returns the property associated with a given key.
     *
     * Properties of the base classes will also be visited, if any.
     *
     * @param key The key to use to search for a property.
     * @return The property associated with the given key, if any.
     */
    [[nodiscard]] meta_prop prop(meta_any key) const {
        return internal::meta_visit<&node_type::prop>([&key](const auto *curr) { return curr->id == key; }, node);
    }

    /**
     * @brief Returns true if an object is valid, false otherwise.
     * @return True if the object is valid, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(node == nullptr);
    }

    /**
     * @brief Checks if two objects refer to the same type.
     * @param other The object with which to compare.
     * @return True if the objects refer to the same type, false otherwise.
     */
    [[nodiscard]] bool operator==(const meta_type &other) const ENTT_NOEXCEPT {
        return (!node && !other.node) || (node && other.node && node->info == other.node->info);
    }

    /**
     * @brief Resets a type and all its parts.
     *
     * This function resets a type and all its data members, member functions
     * and properties, as well as its constructors, destructors and conversion
     * functions if any.<br/>
     * Base classes aren't reset but the link between the two types is removed.
     *
     * The type is also removed from the list of searchable types.
     */
    void reset() ENTT_NOEXCEPT {
        for(auto** it = internal::meta_context::global(); *it; it = &(*it)->next) {
            if(*it == node) {
                *it = (*it)->next;
                break;
            }
        }

        unregister_all(&node->prop);
        unregister_all(&node->base);
        unregister_all(&node->conv);
        unregister_all<&internal::meta_ctor_node::prop>(&node->ctor);
        unregister_all<&internal::meta_data_node::prop>(&node->data);
        unregister_all<&internal::meta_func_node::prop>(&node->func);

        node->id = {};
        node->ctor = node->def_ctor;
        node->dtor = nullptr;
    }

private:
    node_type *node;
};


/**
 * @brief Checks if two objects refer to the same type.
 * @param lhs An object, either valid or not.
 * @param rhs An object, either valid or not.
 * @return False if the objects refer to the same node, true otherwise.
 */
[[nodiscard]] inline bool operator!=(const meta_type &lhs, const meta_type &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


[[nodiscard]] inline meta_type meta_any::type() const ENTT_NOEXCEPT {
    return node;
}


template<typename... Args>
meta_any meta_any::invoke(const id_type id, Args &&... args) const {
    return type().invoke(id, *this, std::forward<Args>(args)...);
}


template<typename... Args>
meta_any meta_any::invoke(const id_type id, Args &&... args) {
    return type().invoke(id, *this, std::forward<Args>(args)...);
}


template<typename Type>
bool meta_any::set(const id_type id, Type &&value) {
    return type().set(id, *this, std::forward<Type>(value));
}


[[nodiscard]] inline meta_any meta_any::get(const id_type id) const {
    return type().get(id, *this);
}


[[nodiscard]] inline meta_any meta_any::get(const id_type id) {
    return type().get(id, *this);
}


[[nodiscard]] inline meta_type meta_ctor::parent() const ENTT_NOEXCEPT {
    return node->parent;
}


[[nodiscard]] inline meta_type meta_ctor::arg(size_type index) const ENTT_NOEXCEPT {
    return index < arity() ? node->arg(index) : meta_type{};
}


[[nodiscard]] inline meta_type meta_data::parent() const ENTT_NOEXCEPT {
    return node->parent;
}


[[nodiscard]] inline meta_type meta_data::type() const ENTT_NOEXCEPT {
    return node->type();
}


[[nodiscard]] inline meta_type meta_func::parent() const ENTT_NOEXCEPT {
    return node->parent;
}


[[nodiscard]] inline meta_type meta_func::ret() const ENTT_NOEXCEPT {
    return node->ret();
}


[[nodiscard]] inline meta_type meta_func::arg(size_type index) const ENTT_NOEXCEPT {
    return index < arity() ? node->arg(index) : meta_type{};
}


/*! @brief Opaque iterator for sequence containers. */
class meta_sequence_container::meta_iterator {
    /*! @brief A sequence container can access the underlying iterator. */
    friend class meta_sequence_container;

    enum class operation { INCR, DEREF };

    using vtable_type = void(const operation, const any &, void *);

    template<typename It>
    static void basic_vtable(const operation op, const any &from, void *to) {
        switch(op) {
        case operation::INCR:
            ++any_cast<It &>(const_cast<any &>(from));
            break;
        case operation::DEREF:
            static_cast<meta_any *>(to)->emplace<typename std::iterator_traits<It>::reference>(*any_cast<const It &>(from));
            break;
        }
    }

public:
    /*! @brief Signed integer type. */
    using difference_type = std::ptrdiff_t;
    /*! @brief Type of elements returned by the iterator. */
    using value_type = meta_any;
    /*! @brief Pointer type, `void` on purpose. */
    using pointer = void;
    /*! @brief Reference type, it is **not** an actual reference. */
    using reference = value_type;
    /*! @brief Iterator category. */
    using iterator_category = std::input_iterator_tag;

    /*! @brief Default constructor. */
    meta_iterator() ENTT_NOEXCEPT = default;

    /**
     * @brief Constructs a meta iterator from a given iterator.
     * @tparam It Type of actual iterator with which to build the meta iterator.
     * @param iter The actual iterator with which to build the meta iterator.
     */
    template<typename It>
    meta_iterator(It iter)
        : vtable{&basic_vtable<It>},
          handle{std::move(iter)}
    {}

    /*! @brief Pre-increment operator. @return This iterator. */
    meta_iterator & operator++() ENTT_NOEXCEPT {
        return vtable(operation::INCR, handle, nullptr), *this;
    }

    /*! @brief Post-increment operator. @return This iterator. */
    meta_iterator operator++(int) ENTT_NOEXCEPT {
        meta_iterator orig = *this;
        return ++(*this), orig;
    }

    /**
     * @brief Checks if two iterators refer to the same element.
     * @param other The iterator with which to compare.
     * @return True if the iterators refer to the same element, false otherwise.
     */
    [[nodiscard]] bool operator==(const meta_iterator &other) const ENTT_NOEXCEPT {
        return handle == other.handle;
    }

    /**
     * @brief Checks if two iterators refer to the same element.
     * @param other The iterator with which to compare.
     * @return False if the iterators refer to the same element, true otherwise.
     */
    [[nodiscard]] bool operator!=(const meta_iterator &other) const ENTT_NOEXCEPT {
        return !(*this == other);
    }

    /**
     * @brief Indirection operator.
     * @return The element to which the iterator points.
     */
    [[nodiscard]] reference operator*() const {
        meta_any other;
        vtable(operation::DEREF, handle, &other);
        return other;
    }

    /**
     * @brief Returns false if an iterator is invalid, true otherwise.
     * @return False if the iterator is invalid, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return static_cast<bool>(handle);
    }

private:
    vtable_type *vtable{};
    any handle{};
};


template<typename Type>
struct meta_sequence_container::meta_sequence_container_proxy {
    using traits_type = meta_sequence_container_traits<Type>;

    [[nodiscard]] static meta_type value_type() ENTT_NOEXCEPT {
        return internal::meta_info<typename Type::value_type>::resolve();
    }

    [[nodiscard]] static size_type size(const any &container) ENTT_NOEXCEPT {
        return traits_type::size(any_cast<const Type &>(container));
    }

    [[nodiscard]] static bool resize(any &container, size_type sz) {
        auto * const cont = any_cast<Type>(&container);
        return cont && traits_type::resize(*cont, sz);
    }

    [[nodiscard]] static bool clear(any &container) {
        auto * const cont = any_cast<Type>(&container);
        return cont && traits_type::clear(*cont);
    }

    [[nodiscard]] static iterator begin(any &container) {
        if(auto * const cont = any_cast<Type>(&container); cont) {
            return iterator{traits_type::begin(*cont)};
        }

        return iterator{traits_type::cbegin(any_cast<const Type &>(container))};
    }

    [[nodiscard]] static iterator end(any &container) {
        if(auto * const cont = any_cast<Type>(&container); cont) {
            return iterator{traits_type::end(*cont)};
        }

        return iterator{traits_type::cend(any_cast<const Type &>(container))};
    }

    [[nodiscard]] static std::pair<iterator, bool> insert(any &container, iterator it, meta_any &value) {
        if(auto * const cont = any_cast<Type>(&container); cont) {
            // this abomination is necessary because only on macos value_type and const_reference are different types for std::vector<bool>
            if(value.allow_cast<typename Type::const_reference>() || value.allow_cast<typename Type::value_type>()) {
                const auto *element = value.try_cast<std::remove_reference_t<typename Type::const_reference>>();
                auto ret = traits_type::insert(*cont, any_cast<const typename Type::iterator &>(it.handle), element ? *element : value.cast<typename Type::value_type>());
                return { iterator{std::move(ret.first)}, ret.second };
            }
        }

        return {};
    }

    [[nodiscard]] static std::pair<iterator, bool> erase(any &container, iterator it) {
        if(auto * const cont = any_cast<Type>(&container); cont) {
            auto ret = traits_type::erase(*cont, any_cast<const typename Type::iterator &>(it.handle));
            return { iterator{std::move(ret.first)}, ret.second };
        }

        return {};
    }

    [[nodiscard]] static meta_any get(any &container, size_type pos) {
        if(auto * const cont = any_cast<Type>(&container); cont) {
            return meta_any{std::in_place_type<typename Type::reference>, traits_type::get(*cont, pos)};
        }

        return meta_any{std::in_place_type<typename Type::const_reference>, traits_type::cget(any_cast<const Type &>(container), pos)};
    }
};


/**
 * @brief Returns the meta value type of a container.
 * @return The meta value type of the container.
 */
[[nodiscard]] inline meta_type meta_sequence_container::value_type() const ENTT_NOEXCEPT {
    return value_type_fn();
}


/**
 * @brief Returns the size of a container.
 * @return The size of the container.
 */
[[nodiscard]] inline meta_sequence_container::size_type meta_sequence_container::size() const ENTT_NOEXCEPT {
    return size_fn(storage);
}


/**
 * @brief Resizes a container to contain a given number of elements.
 * @param sz The new size of the container.
 * @return True in case of success, false otherwise.
 */
inline bool meta_sequence_container::resize(size_type sz) {
    return resize_fn(storage, sz);
}


/**
 * @brief Clears the content of a container.
 * @return True in case of success, false otherwise.
 */
inline bool meta_sequence_container::clear() {
    return clear_fn(storage);
}


/**
 * @brief Returns an iterator to the first element of a container.
 * @return An iterator to the first element of the container.
 */
[[nodiscard]] inline meta_sequence_container::iterator meta_sequence_container::begin() {
    return begin_fn(storage);
}


/**
 * @brief Returns an iterator that is past the last element of a container.
 * @return An iterator that is past the last element of the container.
 */
[[nodiscard]] inline meta_sequence_container::iterator meta_sequence_container::end() {
    return end_fn(storage);
}


/**
 * @brief Inserts an element at a specified location of a container.
 * @param it Iterator before which the element will be inserted.
 * @param value Element value to insert.
 * @return A pair consisting of an iterator to the inserted element (in case of
 * success) and a bool denoting whether the insertion took place.
 */
inline std::pair<meta_sequence_container::iterator, bool> meta_sequence_container::insert(iterator it, meta_any value) {
    return insert_fn(storage, it, value);
}


/**
 * @brief Removes a given element from a container.
 * @param it Iterator to the element to remove.
 * @return A pair consisting of an iterator following the last removed element
 * (in case of success) and a bool denoting whether the insertion took place.
 */
inline std::pair<meta_sequence_container::iterator, bool> meta_sequence_container::erase(iterator it) {
    return erase_fn(storage, it);
}


/**
 * @brief Returns a reference to the element at a given location of a container
 * (no bounds checking is performed).
 * @param pos The position of the element to return.
 * @return A reference to the requested element properly wrapped.
 */
[[nodiscard]] inline meta_any meta_sequence_container::operator[](size_type pos) {
    return get_fn(storage, pos);
}


/**
 * @brief Returns false if a proxy is invalid, true otherwise.
 * @return False if the proxy is invalid, true otherwise.
 */
[[nodiscard]] inline meta_sequence_container::operator bool() const ENTT_NOEXCEPT {
    return static_cast<bool>(storage);
}


/*! @brief Opaque iterator for associative containers. */
class meta_associative_container::meta_iterator {
    enum operation { INCR, DEREF };

    using vtable_type = void(const operation, const any &, void *);

    template<bool KeyOnly, typename It>
    static void basic_vtable(const operation op, const any &from, void *to) {
        switch(op) {
        case operation::INCR:
            ++any_cast<It &>(const_cast<any &>(from));
            break;
        case operation::DEREF:
            const auto &it = any_cast<const It &>(from);
            if constexpr(KeyOnly) {
                static_cast<std::pair<meta_any, meta_any> *>(to)->first.emplace<decltype(*it)>(*it);
            } else {
                static_cast<std::pair<meta_any, meta_any> *>(to)->first.emplace<decltype((it->first))>(it->first);
                static_cast<std::pair<meta_any, meta_any> *>(to)->second.emplace<decltype((it->second))>(it->second);
            }
            break;
        }
    }

public:
    /*! @brief Signed integer type. */
    using difference_type = std::ptrdiff_t;
    /*! @brief Type of elements returned by the iterator. */
    using value_type = std::pair<meta_any, meta_any>;
    /*! @brief Pointer type, `void` on purpose. */
    using pointer = void;
    /*! @brief Reference type, it is **not** an actual reference. */
    using reference = value_type;
    /*! @brief Iterator category. */
    using iterator_category = std::input_iterator_tag;

    /*! @brief Default constructor. */
    meta_iterator() ENTT_NOEXCEPT = default;

    /**
     * @brief Constructs an meta iterator from a given iterator.
     * @tparam KeyOnly True if the container is also key-only, false otherwise.
     * @tparam It Type of actual iterator with which to build the meta iterator.
     * @param iter The actual iterator with which to build the meta iterator.
     */
    template<bool KeyOnly, typename It>
    meta_iterator(std::integral_constant<bool, KeyOnly>, It iter)
        : vtable{&basic_vtable<KeyOnly, It>},
          handle{std::move(iter)}
    {}

    /*! @brief Pre-increment operator. @return This iterator. */
    meta_iterator & operator++() ENTT_NOEXCEPT {
        return vtable(operation::INCR, handle, nullptr), *this;
    }

    /*! @brief Post-increment operator. @return This iterator. */
    meta_iterator operator++(int) ENTT_NOEXCEPT {
        meta_iterator orig = *this;
        return ++(*this), orig;
    }

    /**
     * @brief Checks if two iterators refer to the same element.
     * @param other The iterator with which to compare.
     * @return True if the iterators refer to the same element, false otherwise.
     */
    [[nodiscard]] bool operator==(const meta_iterator &other) const ENTT_NOEXCEPT {
        return handle == other.handle;
    }

    /**
     * @brief Checks if two iterators refer to the same element.
     * @param other The iterator with which to compare.
     * @return False if the iterators refer to the same element, true otherwise.
     */
    [[nodiscard]] bool operator!=(const meta_iterator &other) const ENTT_NOEXCEPT {
        return !(*this == other);
    }

    /**
     * @brief Indirection operator.
     * @return The element to which the iterator points.
     */
    [[nodiscard]] reference operator*() const {
        reference other;
        vtable(operation::DEREF, handle, &other);
        return other;
    }

    /**
     * @brief Returns false if an iterator is invalid, true otherwise.
     * @return False if the iterator is invalid, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return static_cast<bool>(handle);
    }

private:
    vtable_type *vtable{};
    any handle{};
};


template<typename Type>
struct meta_associative_container::meta_associative_container_proxy {
    using traits_type = meta_associative_container_traits<Type>;

    [[nodiscard]] static meta_type key_type() ENTT_NOEXCEPT {
        return internal::meta_info<typename Type::key_type>::resolve();
    }

    [[nodiscard]] static meta_type mapped_type() ENTT_NOEXCEPT {
        if constexpr(is_key_only_meta_associative_container_v<Type>) {
            return meta_type{};
        } else {
            return internal::meta_info<typename Type::mapped_type>::resolve();
        }
    }

    [[nodiscard]] static meta_type value_type() ENTT_NOEXCEPT {
        return internal::meta_info<typename Type::value_type>::resolve();
    }

    [[nodiscard]] static size_type size(const any &container) ENTT_NOEXCEPT {
        return traits_type::size(any_cast<const Type &>(container));
    }

    [[nodiscard]] static bool clear(any &container) {
        auto * const cont = any_cast<Type>(&container);
        return cont && traits_type::clear(*cont);
    }

    [[nodiscard]] static iterator begin(any &container) {
        if(auto * const cont = any_cast<Type>(&container); cont) {
            return iterator{is_key_only_meta_associative_container<Type>{}, traits_type::begin(*cont)};
        }

        return iterator{is_key_only_meta_associative_container<Type>{}, traits_type::cbegin(any_cast<const Type &>(container))};
    }

    [[nodiscard]] static iterator end(any &container) {
        if(auto * const cont = any_cast<Type>(&container); cont) {
            return iterator{is_key_only_meta_associative_container<Type>{}, traits_type::end(*cont)};
        }

        return iterator{is_key_only_meta_associative_container<Type>{}, traits_type::cend(any_cast<const Type &>(container))};
    }

    [[nodiscard]] static bool insert(any &container, meta_any &key, meta_any &value) {
        if(auto * const cont = any_cast<Type>(&container); cont && key.allow_cast<const typename Type::key_type &>()) {
            if constexpr(is_key_only_meta_associative_container_v<Type>) {
                return traits_type::insert(*cont, key.cast<const typename Type::key_type &>());
            } else {
                return value.allow_cast<const typename Type::mapped_type &>()
                    && traits_type::insert(*cont, key.cast<const typename Type::key_type &>(), value.cast<const typename Type::mapped_type &>());
            }
        }

        return false;
    }

    [[nodiscard]] static bool erase(any &container, meta_any &key) {
        if(auto * const cont = any_cast<Type>(&container); cont && key.allow_cast<const typename Type::key_type &>()) {
            return traits_type::erase(*cont, key.cast<const typename Type::key_type &>());
        }

        return false;
    }

    [[nodiscard]] static iterator find(any &container, meta_any &key) {
        if(key.allow_cast<const typename Type::key_type &>()) {
            if(auto * const cont = any_cast<Type>(&container); cont) {
                return iterator{is_key_only_meta_associative_container<Type>{}, traits_type::find(*cont, key.cast<const typename Type::key_type &>())};
            }

            return iterator{is_key_only_meta_associative_container<Type>{}, traits_type::cfind(any_cast<const Type &>(container), key.cast<const typename Type::key_type &>())};
        }

        return {};
    }
};


/**
 * @brief Returns true if a container is also key-only, false otherwise.
 * @return True if the associative container is also key-only, false otherwise.
 */
[[nodiscard]] inline bool meta_associative_container::key_only() const ENTT_NOEXCEPT {
    return key_only_container;
}


/**
 * @brief Returns the meta key type of a container.
 * @return The meta key type of the a container.
 */
[[nodiscard]] inline meta_type meta_associative_container::key_type() const ENTT_NOEXCEPT {
    return key_type_fn();
}


/**
 * @brief Returns the meta mapped type of a container.
 * @return The meta mapped type of the a container.
 */
[[nodiscard]] inline meta_type meta_associative_container::mapped_type() const ENTT_NOEXCEPT {
    return mapped_type_fn();
}


/*! @copydoc meta_sequence_container::value_type */
[[nodiscard]] inline meta_type meta_associative_container::value_type() const ENTT_NOEXCEPT {
    return value_type_fn();
}


/*! @copydoc meta_sequence_container::size */
[[nodiscard]] inline meta_associative_container::size_type meta_associative_container::size() const ENTT_NOEXCEPT {
    return size_fn(storage);
}


/*! @copydoc meta_sequence_container::clear */
inline bool meta_associative_container::clear() {
    return clear_fn(storage);
}


/*! @copydoc meta_sequence_container::begin */
[[nodiscard]] inline meta_associative_container::iterator meta_associative_container::begin() {
    return begin_fn(storage);
}


/*! @copydoc meta_sequence_container::end */
[[nodiscard]] inline meta_associative_container::iterator meta_associative_container::end() {
    return end_fn(storage);
}


/**
 * @brief Inserts an element (a key/value pair) into a container.
 * @param key The key of the element to insert.
 * @param value The value of the element to insert.
 * @return A bool denoting whether the insertion took place.
 */
inline bool meta_associative_container::insert(meta_any key, meta_any value = {}) {
    return insert_fn(storage, key, value);
}


/**
 * @brief Removes the specified element from a container.
 * @param key The key of the element to remove.
 * @return A bool denoting whether the removal took place.
 */
inline bool meta_associative_container::erase(meta_any key) {
    return erase_fn(storage, key);
}


/**
 * @brief Returns an iterator to the element with a given key, if any.
 * @param key The key of the element to search.
 * @return An iterator to the element with the given key, if any.
 */
[[nodiscard]] inline meta_associative_container::iterator meta_associative_container::find(meta_any key) {
    return find_fn(storage, key);
}


/**
 * @brief Returns false if a proxy is invalid, true otherwise.
 * @return False if the proxy is invalid, true otherwise.
 */
[[nodiscard]] inline meta_associative_container::operator bool() const ENTT_NOEXCEPT {
    return static_cast<bool>(storage);
}


}


#endif

// #include "node.hpp"

// #include "policy.hpp"
#ifndef ENTT_META_POLICY_HPP
#define ENTT_META_POLICY_HPP


namespace entt {


/*! @brief Empty class type used to request the _as ref_ policy. */
struct as_ref_t {};


/*! @brief Empty class type used to request the _as cref_ policy. */
struct as_cref_t {};


/*! @brief Empty class type used to request the _as-is_ policy. */
struct as_is_t {};


/*! @brief Empty class type used to request the _as void_ policy. */
struct as_void_t {};


}


#endif

// #include "utility.hpp"
#ifndef ENTT_META_UTILITY_HPP
#define ENTT_META_UTILITY_HPP


#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
// #include "../config/config.h"

// #include "../core/type_traits.hpp"

// #include "meta.hpp"

// #include "node.hpp"

// #include "policy.hpp"



namespace entt {


/*! @brief Primary template isn't defined on purpose. */
template<typename, typename>
struct meta_function_descriptor;


/**
 * @brief Meta function descriptor.
 * @tparam Type Reflected type to which the meta function is associated.
 * @tparam Ret Function return type.
 * @tparam Class Actual owner of the member function.
 * @tparam Args Function arguments.
 */
template<typename Type, typename Ret, typename Class, typename... Args>
struct meta_function_descriptor<Type, Ret(Class:: *)(Args...) const> {
    /*! @brief Meta function return type. */
    using return_type = Ret;
    /*! @brief Meta function arguments. */
    using args_type = std::conditional_t<std::is_same_v<Type, Class>, type_list<Args...>, type_list<const Class &, Args...>>;

    /*! @brief True if the meta function is const, false otherwise. */
    static constexpr auto is_const = true;
    /*! @brief True if the meta function is static, false otherwise. */
    static constexpr auto is_static = !std::is_same_v<Type, Class>;
};


/**
 * @brief Meta function descriptor.
 * @tparam Type Reflected type to which the meta function is associated.
 * @tparam Ret Function return type.
 * @tparam Class Actual owner of the member function.
 * @tparam Args Function arguments.
 */
template<typename Type, typename Ret, typename Class, typename... Args>
struct meta_function_descriptor<Type, Ret(Class:: *)(Args...)> {
    /*! @brief Meta function return type. */
    using return_type = Ret;
    /*! @brief Meta function arguments. */
    using args_type = std::conditional_t<std::is_same_v<Type, Class>, type_list<Args...>, type_list<Class &, Args...>>;

    /*! @brief True if the meta function is const, false otherwise. */
    static constexpr auto is_const = false;
    /*! @brief True if the meta function is static, false otherwise. */
    static constexpr auto is_static = !std::is_same_v<Type, Class>;
};


/**
 * @brief Meta function descriptor.
 * @tparam Type Reflected type to which the meta function is associated.
 * @tparam Ret Function return type.
 * @tparam Args Function arguments.
 */
template<typename Type, typename Ret, typename... Args>
struct meta_function_descriptor<Type, Ret(*)(Args...)> {
    /*! @brief Meta function return type. */
    using return_type = Ret;
    /*! @brief Meta function arguments. */
    using args_type = type_list<Args...>;

    /*! @brief True if the meta function is const, false otherwise. */
    static constexpr auto is_const = false;
    /*! @brief True if the meta function is static, false otherwise. */
    static constexpr auto is_static = true;
};


/**
 * @brief Meta function helper.
 *
 * Converts a function type to be associated with a reflected type into its meta
 * function descriptor.
 *
 * @tparam Type Reflected type to which the meta function is associated.
 * @tparam Candidate The actual function to associate with the reflected type.
 */
template<typename Type, typename Candidate>
class meta_function_helper {
    template<typename Ret, typename... Args, typename Class>
    static constexpr meta_function_descriptor<Type, Ret(Class:: *)(Args...) const> get_rid_of_noexcept(Ret(Class:: *)(Args...) const);

    template<typename Ret, typename... Args, typename Class>
    static constexpr meta_function_descriptor<Type, Ret(Class:: *)(Args...)> get_rid_of_noexcept(Ret(Class:: *)(Args...));

    template<typename Ret, typename... Args>
    static constexpr meta_function_descriptor<Type, Ret(*)(Args...)> get_rid_of_noexcept(Ret(*)(Args...));

public:
    /*! @brief The meta function descriptor of the given function. */
    using type = decltype(get_rid_of_noexcept(std::declval<Candidate>()));
};


/**
 * @brief Helper type.
 * @tparam Type Reflected type to which the meta function is associated.
 * @tparam Candidate The actual function to associate with the reflected type.
 */
template<typename Type, typename Candidate>
using meta_function_helper_t = typename meta_function_helper<Type, Candidate>::type;


/**
 * @brief Returns the meta type of the i-th element of a list of arguments.
 * @tparam Args Actual types of arguments.
 * @return The meta type of the i-th element of the list of arguments.
 */
template<typename... Args>
[[nodiscard]] static meta_type meta_arg(type_list<Args...>, const std::size_t index) ENTT_NOEXCEPT {
    return internal::meta_arg_node(type_list<Args...>{}, index);
}


/**
 * @brief Constructs an instance given a list of erased parameters, if possible.
 * @tparam Type Actual type of the instance to construct.
 * @tparam Args Types of arguments expected.
 * @tparam Index Indexes to use to extract erased arguments from their list.
 * @param args Parameters to use to construct the instance.
 * @return A meta any containing the new instance, if any.
 */
template<typename Type, typename... Args, std::size_t... Index>
[[nodiscard]] meta_any meta_construct(meta_any * const args, std::index_sequence<Index...>) {
    if(((args+Index)->allow_cast<Args>() && ...)) {
        return Type{(args+Index)->cast<Args>()...};
    }

    return {};
}


/**
 * @brief Sets the value of a given variable.
 * @tparam Type Reflected type to which the variable is associated.
 * @tparam Data The actual variable to set.
 * @param instance An opaque instance of the underlying type, if required.
 * @param value Parameter to use to set the variable.
 * @return True in case of success, false otherwise.
 */
template<typename Type, auto Data>
[[nodiscard]] bool meta_setter([[maybe_unused]] meta_handle instance, [[maybe_unused]] meta_any value) {
    if constexpr(!std::is_same_v<decltype(Data), Type> && !std::is_same_v<decltype(Data), std::nullptr_t>) {
        if constexpr(std::is_function_v<std::remove_reference_t<std::remove_pointer_t<decltype(Data)>>>) {
            using data_type = type_list_element_t<1u, typename meta_function_helper_t<Type, decltype(Data)>::args_type>;

            if(auto * const clazz = instance->try_cast<Type>(); clazz && value.allow_cast<data_type>()) {
                Data(*clazz, value.cast<data_type>());
                return true;
            }
        } else if constexpr(std::is_member_function_pointer_v<decltype(Data)>) {
            using data_type = type_list_element_t<0u, typename meta_function_helper_t<Type, decltype(Data)>::args_type>;

            if(auto * const clazz = instance->try_cast<Type>(); clazz && value.allow_cast<data_type>()) {
                (clazz->*Data)(value.cast<data_type>());
                return true;
            }
        } else if constexpr(std::is_member_object_pointer_v<decltype(Data)>) {
            using data_type = std::remove_reference_t<decltype(std::declval<Type>().*Data)>;

            if constexpr(!std::is_array_v<data_type> && !std::is_const_v<data_type>) {
                if(auto * const clazz = instance->try_cast<Type>(); clazz && value.allow_cast<data_type>()) {
                    clazz->*Data = value.cast<data_type>();
                    return true;
                }
            }
        } else {
            using data_type = std::remove_reference_t<decltype(*Data)>;

            if constexpr(!std::is_array_v<data_type> && !std::is_const_v<data_type>) {
                if(value.allow_cast<data_type>()) {
                    *Data = value.cast<data_type>();
                    return true;
                }
            }
        }
    }

    return false;
}


/**
 * @brief Wraps a value depending on the given policy.
 * @tparam Policy Optional policy (no policy set by default).
 * @tparam Type Type of value to wrap.
 * @param value Value to wrap.
 * @return A meta any containing the returned value.
 */
template<typename Policy = as_is_t, typename Type>
meta_any meta_dispatch(Type &&value) {
    if constexpr(std::is_same_v<Policy, as_void_t>) {
        return meta_any{std::in_place_type<void>, std::forward<Type>(value)};
    } else if constexpr(std::is_same_v<Policy, as_ref_t>) {
        return meta_any{std::in_place_type<Type>, std::forward<Type>(value)};
    } else if constexpr(std::is_same_v<Policy, as_cref_t>) {
        static_assert(std::is_lvalue_reference_v<Type>, "Invalid type");
        return meta_any{std::in_place_type<const std::remove_reference_t<Type> &>, std::as_const(value)};
    } else {
        static_assert(std::is_same_v<Policy, as_is_t>, "Policy not supported");
        return meta_any{std::forward<Type>(value)};
    }
}


/**
 * @brief Gets the value of a given variable.
 * @tparam Type Reflected type to which the variable is associated.
 * @tparam Data The actual variable to get.
 * @tparam Policy Optional policy (no policy set by default).
 * @param instance An opaque instance of the underlying type, if required.
 * @return A meta any containing the value of the underlying variable.
 */
template<typename Type, auto Data, typename Policy = as_is_t>
[[nodiscard]] meta_any meta_getter([[maybe_unused]] meta_handle instance) {
    if constexpr(std::is_function_v<std::remove_reference_t<std::remove_pointer_t<decltype(Data)>>>) {
        auto * const clazz = instance->try_cast<std::conditional_t<std::is_invocable_v<decltype(Data), const Type &>, const Type, Type>>();
        return clazz ? meta_dispatch<Policy>(Data(*clazz)) : meta_any{};
    } else if constexpr(std::is_member_function_pointer_v<decltype(Data)>) {
        auto * const clazz = instance->try_cast<std::conditional_t<std::is_invocable_v<decltype(Data), const Type &>, const Type, Type>>();
        return clazz ? meta_dispatch<Policy>((clazz->*Data)()) : meta_any{};
    } else if constexpr(std::is_member_object_pointer_v<decltype(Data)>) {
        if constexpr(!std::is_array_v<std::remove_cv_t<std::remove_reference_t<decltype(std::declval<Type>().*Data)>>>) {
            if(auto * clazz = instance->try_cast<Type>(); clazz) {
                return meta_dispatch<Policy>(clazz->*Data);
            } else if(auto * fallback = instance->try_cast<const Type>(); fallback) {
                return meta_dispatch<Policy>(fallback->*Data);
            }
        }

        return meta_any{};
    } else if constexpr(std::is_pointer_v<decltype(Data)>) {
        if constexpr(std::is_array_v<std::remove_pointer_t<decltype(Data)>>) {
            return meta_any{};
        } else {
            return meta_dispatch<Policy>(*Data);
        }
    } else {
        return meta_dispatch<Policy>(Data);
    }
}


/**
 * @brief Invokes a function given a list of erased parameters, if possible.
 * @tparam Type Reflected type to which the function is associated.
 * @tparam Candidate The actual function to invoke.
 * @tparam Policy Optional policy (no policy set by default).
 * @tparam Index Indexes to use to extract erased arguments from their list.
 * @param instance An opaque instance of the underlying type, if required.
 * @param args Parameters to use to invoke the function.
 * @return A meta any containing the returned value, if any.
 */
template<typename Type, auto Candidate, typename Policy = as_is_t, std::size_t... Index>
[[nodiscard]] std::enable_if_t<!std::is_invocable_v<decltype(Candidate)>, meta_any> meta_invoke([[maybe_unused]] meta_handle instance, meta_any *args, std::index_sequence<Index...>) {
    using descriptor = meta_function_helper_t<Type, decltype(Candidate)>;

    const auto invoke = [](auto &&maybe_clazz, auto &&... other) {
        if constexpr(std::is_member_function_pointer_v<decltype(Candidate)>) {
            if constexpr(std::is_void_v<typename descriptor::return_type>) {
                (std::forward<decltype(maybe_clazz)>(maybe_clazz).*Candidate)(std::forward<decltype(other)>(other)...);
                return meta_any{std::in_place_type<void>};
            } else {
                return meta_dispatch<Policy>((std::forward<decltype(maybe_clazz)>(maybe_clazz).*Candidate)(std::forward<decltype(other)>(other)...));
            }
        } else {
            if constexpr(std::is_void_v<typename descriptor::return_type>) {
                Candidate(std::forward<decltype(maybe_clazz)>(maybe_clazz), std::forward<decltype(other)>(other)...);
                return meta_any{std::in_place_type<void>};
            } else {
                return meta_dispatch<Policy>(Candidate(std::forward<decltype(maybe_clazz)>(maybe_clazz), std::forward<decltype(other)>(other)...));
            }
        }
    };

    if constexpr(std::is_invocable_v<decltype(Candidate), const Type &, type_list_element_t<Index, typename descriptor::args_type>...>) {
        if(const auto * const clazz = instance->try_cast<const Type>(); clazz && ((args+Index)->allow_cast<type_list_element_t<Index, typename descriptor::args_type>>() && ...)) {
            return invoke(*clazz, (args+Index)->cast<type_list_element_t<Index, typename descriptor::args_type>>()...);
        }
    } else if constexpr(std::is_invocable_v<decltype(Candidate), Type &, type_list_element_t<Index, typename descriptor::args_type>...>) {
        if(auto * const clazz = instance->try_cast<Type>(); clazz && ((args+Index)->allow_cast<type_list_element_t<Index, typename descriptor::args_type>>() && ...)) {
            return invoke(*clazz, (args+Index)->cast<type_list_element_t<Index, typename descriptor::args_type>>()...);
        }
    } else {
        if(((args+Index)->allow_cast<type_list_element_t<Index, typename descriptor::args_type>>() && ...)) {
            return invoke((args+Index)->cast<type_list_element_t<Index, typename descriptor::args_type>>()...);
        }
    }

    return meta_any{};
}


/**
 * @brief Invokes a function given a list of erased parameters, if possible.
 * @tparam Type Reflected type to which the function is associated.
 * @tparam Candidate The actual function to invoke.
 * @tparam Policy Optional policy (no policy set by default).
 * @tparam Index Indexes to use to extract erased arguments from their list.
 * @return A meta any containing the returned value, if any.
 */
template<typename Type, auto Candidate, typename Policy = as_is_t, std::size_t... Index>
[[nodiscard]] std::enable_if_t<std::is_invocable_v<decltype(Candidate)>, meta_any> meta_invoke(meta_handle, meta_any *, std::index_sequence<Index...>) {
    if constexpr(std::is_void_v<decltype(Candidate())>) {
        Candidate();
        return meta_any{std::in_place_type<void>};
    } else {
        return meta_dispatch<Policy>(Candidate());
    }
}


}


#endif



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename Node>
[[nodiscard]] bool find_if(const Node *candidate, const Node *node) ENTT_NOEXCEPT {
    return node && (node == candidate || find_if(candidate, node->next));
}


template<typename Id, typename Node>
[[nodiscard]] bool find_if_not(const Id id, Node *node, const Node *owner) ENTT_NOEXCEPT {
    if constexpr(std::is_pointer_v<Id>) {
        return node && ((*node->id == *id && node != owner) || find_if_not(id, node->next, owner));
    } else {
        return node && ((node->id == id && node != owner) || find_if_not(id, node->next, owner));
    }
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Meta factory to be used for reflection purposes.
 *
 * The meta factory is an utility class used to reflect types, data members and
 * functions of all sorts. This class ensures that the underlying web of types
 * is built correctly and performs some checks in debug mode to ensure that
 * there are no subtle errors at runtime.
 */
template<typename...>
struct meta_factory;


/**
 * @brief Extended meta factory to be used for reflection purposes.
 * @tparam Type Reflected type for which the factory was created.
 * @tparam Spec Property specialization pack used to disambiguate overloads.
 */
template<typename Type, typename... Spec>
struct meta_factory<Type, Spec...>: public meta_factory<Type> {
private:
    template<std::size_t Step = 0, std::size_t... Index, typename... Property, typename... Other>
    void unpack(std::index_sequence<Index...>, std::tuple<Property...> property, Other &&... other) {
        unroll<Step>(choice<3>, std::move(std::get<Index>(property))..., std::forward<Other>(other)...);
    }

    template<std::size_t Step = 0, typename... Property, typename... Other>
    void unroll(choice_t<3>, std::tuple<Property...> property, Other &&... other) {
        unpack<Step>(std::index_sequence_for<Property...>{}, std::move(property), std::forward<Other>(other)...);
    }

    template<std::size_t Step = 0, typename... Property, typename... Other>
    void unroll(choice_t<2>, std::pair<Property...> property, Other &&... other) {
        assign<Step>(std::move(property.first), std::move(property.second));
        unroll<Step+1>(choice<3>, std::forward<Other>(other)...);
    }

    template<std::size_t Step = 0, typename Property, typename... Other>
    std::enable_if_t<!std::is_invocable_v<Property>>
    unroll(choice_t<1>, Property &&property, Other &&... other) {
        assign<Step>(std::forward<Property>(property));
        unroll<Step+1>(choice<3>, std::forward<Other>(other)...);
    }

    template<std::size_t Step = 0, typename Func, typename... Other>
    void unroll(choice_t<0>, Func &&invocable, Other &&... other) {
        unroll<Step>(choice<3>, std::forward<Func>(invocable)(), std::forward<Other>(other)...);
    }

    template<std::size_t>
    void unroll(choice_t<0>) {}

    template<std::size_t = 0, typename Key>
    void assign(Key &&key, meta_any value = {}) {
        static meta_any property[2u]{};

        static internal::meta_prop_node node{
            nullptr,
            property[0u],
            property[1u]
        };

        entt::meta_any instance{std::forward<Key>(key)};
        ENTT_ASSERT(!internal::find_if_not(&instance, *curr, &node), "Duplicate key");
        property[0u] = std::move(instance);
        property[1u] = std::move(value);

        if(!internal::find_if(&node, *curr)) {
            node.next = *curr;
            *curr = &node;
        }
    }

public:
    /**
     * @brief Constructs an extended factory from a given node.
     * @param target The underlying node to which to assign the properties.
     */
    meta_factory(internal::meta_prop_node **target) ENTT_NOEXCEPT
        : curr{target}
    {}

    /**
     * @brief Assigns a property to the last meta object created.
     *
     * Both the key and the value (if any) must be at least copy constructible.
     *
     * @tparam PropertyOrKey Type of the property or property key.
     * @tparam Value Optional type of the property value.
     * @param property_or_key Property or property key.
     * @param value Optional property value.
     * @return A meta factory for the parent type.
     */
    template<typename PropertyOrKey, typename... Value>
    auto prop(PropertyOrKey &&property_or_key, Value &&... value) && {
        if constexpr(sizeof...(Value) == 0) {
            unroll(choice<3>, std::forward<PropertyOrKey>(property_or_key));
        } else {
            assign(std::forward<PropertyOrKey>(property_or_key), std::forward<Value>(value)...);
        }

        return meta_factory<Type, Spec..., PropertyOrKey, Value...>{curr};
    }

    /**
     * @brief Assigns properties to the last meta object created.
     *
     * Both the keys and the values (if any) must be at least copy
     * constructible.
     *
     * @tparam Property Types of the properties.
     * @param property Properties to assign to the last meta object created.
     * @return A meta factory for the parent type.
     */
    template <typename... Property>
    auto props(Property... property) && {
        unroll(choice<3>, std::forward<Property>(property)...);
        return meta_factory<Type, Spec..., Property...>{curr};
    }

private:
    internal::meta_prop_node **curr;
};


/**
 * @brief Basic meta factory to be used for reflection purposes.
 * @tparam Type Reflected type for which the factory was created.
 */
template<typename Type>
struct meta_factory<Type> {
    /**
     * @brief Makes a meta type _searchable_.
     * @param id Optional unique identifier.
     * @return An extended meta factory for the given type.
     */
    auto type(const id_type id = type_hash<Type>::value()) {
        auto * const node = internal::meta_info<Type>::resolve();

        ENTT_ASSERT(!internal::find_if_not(id, *internal::meta_context::global(), node), "Duplicate identifier");
        node->id = id;

        if(!internal::find_if(node, *internal::meta_context::global())) {
            node->next = *internal::meta_context::global();
            *internal::meta_context::global() = node;
        }

        return meta_factory<Type, Type>{&node->prop};
    }

    /**
     * @brief Assigns a meta base to a meta type.
     *
     * A reflected base class must be a real base class of the reflected type.
     *
     * @tparam Base Type of the base class to assign to the meta type.
     * @return A meta factory for the parent type.
     */
    template<typename Base>
    auto base() ENTT_NOEXCEPT {
        static_assert(std::is_base_of_v<Base, Type>, "Invalid base type");
        auto * const type = internal::meta_info<Type>::resolve();

        static internal::meta_base_node node{
            type,
            nullptr,
            &internal::meta_info<Base>::resolve,
            [](const void *instance) ENTT_NOEXCEPT -> const void * {
                return static_cast<const Base *>(static_cast<const Type *>(instance));
            }
        };

        if(!internal::find_if(&node, type->base)) {
            node.next = type->base;
            type->base = &node;
        }

        return meta_factory<Type>{};
    }

    /**
     * @brief Assigns a meta conversion function to a meta type.
     *
     * Conversion functions can be either free functions or member
     * functions.<br/>
     * In case of free functions, they must accept a const reference to an
     * instance of the parent type as an argument. In case of member functions,
     * they should have no arguments at all.
     *
     * @tparam Candidate The actual function to use for the conversion.
     * @return A meta factory for the parent type.
     */
    template<auto Candidate>
    std::enable_if_t<std::is_member_function_pointer_v<decltype(Candidate)>, meta_factory<Type>> conv() ENTT_NOEXCEPT {
        using conv_type = std::invoke_result_t<decltype(Candidate), Type &>;
        auto * const type = internal::meta_info<Type>::resolve();

        static internal::meta_conv_node node{
            type,
            nullptr,
            &internal::meta_info<conv_type>::resolve,
            [](const void *instance) -> meta_any {
                return (static_cast<const Type *>(instance)->*Candidate)();
            }
        };

        if(!internal::find_if(&node, type->conv)) {
            node.next = type->conv;
            type->conv = &node;
        }

        return meta_factory<Type>{};
    }

    /*! @copydoc conv */
    template<auto Candidate>
    std::enable_if_t<!std::is_member_function_pointer_v<decltype(Candidate)>, meta_factory<Type>> conv() ENTT_NOEXCEPT {
        using conv_type = std::invoke_result_t<decltype(Candidate), Type &>;
        auto * const type = internal::meta_info<Type>::resolve();

        static internal::meta_conv_node node{
            type,
            nullptr,
            &internal::meta_info<conv_type>::resolve,
            [](const void *instance) -> meta_any {
                return Candidate(*static_cast<const Type *>(instance));
            }
        };

        if(!internal::find_if(&node, type->conv)) {
            node.next = type->conv;
            type->conv = &node;
        }

        return meta_factory<Type>{};
    }

    /**
     * @brief Assigns a meta conversion function to a meta type.
     *
     * The given type must be such that an instance of the reflected type can be
     * converted to it.
     *
     * @tparam To Type of the conversion function to assign to the meta type.
     * @return A meta factory for the parent type.
     */
    template<typename To>
    auto conv() ENTT_NOEXCEPT {
        static_assert(std::is_convertible_v<Type, To>, "Could not convert to the required type");
        auto * const type = internal::meta_info<Type>::resolve();

        static internal::meta_conv_node node{
            type,
            nullptr,
            &internal::meta_info<To>::resolve,
            [](const void *instance) -> meta_any {
                return static_cast<To>(*static_cast<const Type *>(instance));
            }
        };

        if(!internal::find_if(&node, type->conv)) {
            node.next = type->conv;
            type->conv = &node;
        }

        return meta_factory<Type>{};
    }

    /**
     * @brief Assigns a meta constructor to a meta type.
     *
     * Both member functions and free function can be assigned to meta types in
     * the role of constructors. All that is required is that they return an
     * instance of the underlying type.<br/>
     * From a client's point of view, nothing changes if a constructor of a meta
     * type is a built-in one or not.
     *
     * @tparam Candidate The actual function to use as a constructor.
     * @tparam Policy Optional policy (no policy set by default).
     * @return An extended meta factory for the parent type.
     */
    template<auto Candidate, typename Policy = as_is_t>
    auto ctor() ENTT_NOEXCEPT {
        using descriptor = meta_function_helper_t<Type, decltype(Candidate)>;
        static_assert(std::is_same_v<std::decay_t<typename descriptor::return_type>, Type>, "The function doesn't return an object of the required type");
        auto * const type = internal::meta_info<Type>::resolve();

        static internal::meta_ctor_node node{
            type,
            nullptr,
            nullptr,
            descriptor::args_type::size,
            [](const typename internal::meta_ctor_node::size_type index) ENTT_NOEXCEPT {
                return meta_arg(typename descriptor::args_type{}, index);
            },
            [](meta_any * const args) {
                return meta_invoke<Type, Candidate, Policy>({}, args, std::make_index_sequence<descriptor::args_type::size>{});
            }
        };

        if(!internal::find_if(&node, type->ctor)) {
            node.next = type->ctor;
            type->ctor = &node;
        }

        return meta_factory<Type, std::integral_constant<decltype(Candidate), Candidate>>{&node.prop};
    }

    /**
     * @brief Assigns a meta constructor to a meta type.
     *
     * A meta constructor is uniquely identified by the types of its arguments
     * and is such that there exists an actual constructor of the underlying
     * type that can be invoked with parameters whose types are those given.
     *
     * @tparam Args Types of arguments to use to construct an instance.
     * @return An extended meta factory for the parent type.
     */
    template<typename... Args>
    auto ctor() ENTT_NOEXCEPT {
        using descriptor = meta_function_helper_t<Type, Type(*)(Args...)>;
        auto * const type = internal::meta_info<Type>::resolve();

        static internal::meta_ctor_node node{
            type,
            nullptr,
            nullptr,
            descriptor::args_type::size,
            [](const typename internal::meta_ctor_node::size_type index) ENTT_NOEXCEPT {
                return meta_arg(typename descriptor::args_type{}, index);
            },
            [](meta_any * const args) {
                return meta_construct<Type, Args...>(args, std::make_index_sequence<descriptor::args_type::size>{});
            }
        };

        if(!internal::find_if(&node, type->ctor)) {
            node.next = type->ctor;
            type->ctor = &node;
        }

        return meta_factory<Type, Type(Args...)>{&node.prop};
    }

    /**
     * @brief Assigns a meta destructor to a meta type.
     *
     * Free functions can be assigned to meta types in the role of destructors.
     * The signature of the function should identical to the following:
     *
     * @code{.cpp}
     * void(Type &);
     * @endcode
     *
     * The purpose is to give users the ability to free up resources that
     * require special treatment before an object is actually destroyed.
     *
     * @tparam Func The actual function to use as a destructor.
     * @return A meta factory for the parent type.
     */
    template<auto Func>
    auto dtor() ENTT_NOEXCEPT {
        static_assert(std::is_invocable_v<decltype(Func), Type &>, "The function doesn't accept an object of the type provided");
        auto * const type = internal::meta_info<Type>::resolve();

        type->dtor = [](void *instance) {
            Func(*static_cast<Type *>(instance));
        };

        return meta_factory<Type>{};
    }

    /**
     * @brief Assigns a meta data to a meta type.
     *
     * Both data members and static and global variables, as well as constants
     * of any kind, can be assigned to a meta type.<br/>
     * From a client's point of view, all the variables associated with the
     * reflected object will appear as if they were part of the type itself.
     *
     * @tparam Data The actual variable to attach to the meta type.
     * @tparam Policy Optional policy (no policy set by default).
     * @param id Unique identifier.
     * @return An extended meta factory for the parent type.
     */
    template<auto Data, typename Policy = as_is_t>
    auto data(const id_type id) ENTT_NOEXCEPT {
        if constexpr(std::is_member_object_pointer_v<decltype(Data)>) {
            return data<Data, Data, Policy>(id);
        } else {
            using data_type = std::remove_pointer_t<decltype(Data)>;
            auto * const type = internal::meta_info<Type>::resolve();

            static internal::meta_data_node node{
                {},
                type,
                nullptr,
                nullptr,
                std::is_same_v<Type, data_type> || std::is_const_v<data_type>,
                true,
                &internal::meta_info<data_type>::resolve,
                &meta_setter<Type, Data>,
                &meta_getter<Type, Data, Policy>
            };

            ENTT_ASSERT(!internal::find_if_not(id, type->data, &node), "Duplicate identifier");
            node.id = id;

            if(!internal::find_if(&node, type->data)) {
                node.next = type->data;
                type->data = &node;
            }

            return meta_factory<Type, std::integral_constant<decltype(Data), Data>>{&node.prop};
        }
    }

    /**
     * @brief Assigns a meta data to a meta type by means of its setter and
     * getter.
     *
     * Setters and getters can be either free functions, member functions or a
     * mix of them.<br/>
     * In case of free functions, setters and getters must accept a reference to
     * an instance of the parent type as their first argument. A setter has then
     * an extra argument of a type convertible to that of the parameter to
     * set.<br/>
     * In case of member functions, getters have no arguments at all, while
     * setters has an argument of a type convertible to that of the parameter to
     * set.
     *
     * @tparam Setter The actual function to use as a setter.
     * @tparam Getter The actual function to use as a getter.
     * @tparam Policy Optional policy (no policy set by default).
     * @param id Unique identifier.
     * @return An extended meta factory for the parent type.
     */
    template<auto Setter, auto Getter, typename Policy = as_is_t>
    auto data(const id_type id) ENTT_NOEXCEPT {
        using underlying_type = std::remove_reference_t<std::invoke_result_t<decltype(Getter), Type &>>;
        auto * const type = internal::meta_info<Type>::resolve();

        static internal::meta_data_node node{
            {},
            type,
            nullptr,
            nullptr,
            std::is_same_v<decltype(Setter), std::nullptr_t> || (std::is_member_object_pointer_v<decltype(Setter)> && std::is_const_v<underlying_type>),
            false,
            &internal::meta_info<underlying_type>::resolve,
            &meta_setter<Type, Setter>,
            &meta_getter<Type, Getter, Policy>
        };

        ENTT_ASSERT(!internal::find_if_not(id, type->data, &node), "Duplicate identifier");
        node.id = id;

        if(!internal::find_if(&node, type->data)) {
            node.next = type->data;
            type->data = &node;
        }

        return meta_factory<Type, std::integral_constant<decltype(Setter), Setter>, std::integral_constant<decltype(Getter), Getter>>{&node.prop};
    }

    /**
     * @brief Assigns a meta funcion to a meta type.
     *
     * Both member functions and free functions can be assigned to a meta
     * type.<br/>
     * From a client's point of view, all the functions associated with the
     * reflected object will appear as if they were part of the type itself.
     *
     * @tparam Candidate The actual function to attach to the meta type.
     * @tparam Policy Optional policy (no policy set by default).
     * @param id Unique identifier.
     * @return An extended meta factory for the parent type.
     */
    template<auto Candidate, typename Policy = as_is_t>
    auto func(const id_type id) ENTT_NOEXCEPT {
        using descriptor = meta_function_helper_t<Type, decltype(Candidate)>;
        auto * const type = internal::meta_info<Type>::resolve();

        static internal::meta_func_node node{
            {},
            type,
            nullptr,
            nullptr,
            descriptor::args_type::size,
            descriptor::is_const,
            descriptor::is_static,
            &internal::meta_info<std::conditional_t<std::is_same_v<Policy, as_void_t>, void, typename descriptor::return_type>>::resolve,
            [](const typename internal::meta_func_node::size_type index) ENTT_NOEXCEPT {
                return meta_arg(typename descriptor::args_type{}, index);
            },
            [](meta_handle instance, meta_any *args) {
                return meta_invoke<Type, Candidate, Policy>(std::move(instance), args, std::make_index_sequence<descriptor::args_type::size>{});
            }
        };

        for(auto *it = &type->func; *it; it = &(*it)->next) {
            if(*it == &node) {
                *it = node.next;
                break;
            }
        }

        internal::meta_func_node **it = &type->func;
        for(; *it && (*it)->id != id; it = &(*it)->next);
        for(; *it && (*it)->id == id && (*it)->arity < node.arity; it = &(*it)->next);

        node.id = id;
        node.next = *it;
        *it = &node;

        return meta_factory<Type, std::integral_constant<decltype(Candidate), Candidate>>{&node.prop};
    }
};


/**
 * @brief Utility function to use for reflection.
 *
 * This is the point from which everything starts.<br/>
 * By invoking this function with a type that is not yet reflected, a meta type
 * is created to which it will be possible to attach meta objects through a
 * dedicated factory.
 *
 * @tparam Type Type to reflect.
 * @return A meta factory for the given type.
 */
template<typename Type>
[[nodiscard]] auto meta() ENTT_NOEXCEPT {
    auto * const node = internal::meta_info<Type>::resolve();
    // extended meta factory to allow assigning properties to opaque meta types
    return meta_factory<Type, Type>{&node->prop};
}


}


#endif

// #include "meta/meta.hpp"

// #include "meta/node.hpp"

// #include "meta/pointer.hpp"
#ifndef ENTT_META_POINTER_HPP
#define ENTT_META_POINTER_HPP


#include <memory>
#include <type_traits>
// #include "type_traits.hpp"



namespace entt {


/**
 * @brief Makes plain pointers pointer-like types for the meta system.
 * @tparam Type Element type.
 */
template<typename Type>
struct is_meta_pointer_like<Type *>
        : std::true_type
{};


/**
 * @brief Partial specialization used to reject pointers to arrays.
 * @tparam Type Type of elements of the array.
 * @tparam N Number of elements of the array.
 */
template<typename Type, std::size_t N>
struct is_meta_pointer_like<Type(*)[N]>
    : std::false_type
{};


/**
 * @brief Makes `std::shared_ptr`s of any type pointer-like types for the meta
 * system.
 * @tparam Type Element type.
 */
template<typename Type>
struct is_meta_pointer_like<std::shared_ptr<Type>>
        : std::true_type
{};


/**
 * @brief Makes `std::unique_ptr`s of any type pointer-like types for the meta
 * system.
 * @tparam Type Element type.
 * @tparam Args Other arguments.
 */
template<typename Type, typename... Args>
struct is_meta_pointer_like<std::unique_ptr<Type, Args...>>
        : std::true_type
{};


}


#endif

// #include "meta/policy.hpp"

// #include "meta/range.hpp"

// #include "meta/resolve.hpp"
#ifndef ENTT_META_RESOLVE_HPP
#define ENTT_META_RESOLVE_HPP


#include <algorithm>
// #include "../core/type_info.hpp"

// #include "ctx.hpp"

// #include "meta.hpp"

// #include "node.hpp"

// #include "range.hpp"



namespace entt {


/**
 * @brief Returns the meta type associated with a given type.
 * @tparam Type Type to use to search for a meta type.
 * @return The meta type associated with the given type, if any.
 */
template<typename Type>
[[nodiscard]] meta_type resolve() ENTT_NOEXCEPT {
    return internal::meta_info<Type>::resolve();
}


/**
 * @brief Returns a range to use to visit all meta types.
 * @return An iterable range to use to visit all meta types.
 */
[[nodiscard]] inline meta_range<meta_type> resolve() {
    return *internal::meta_context::global();
}


/**
 * @brief Returns the meta type associated with a given identifier, if any.
 * @param id Unique identifier.
 * @return The meta type associated with the given identifier, if any.
 */
[[nodiscard]] inline meta_type resolve(const id_type id) ENTT_NOEXCEPT {
    for(auto *curr = *internal::meta_context::global(); curr; curr = curr->next) {
        if(curr->id == id) {
            return curr;
        }
    }

    return {};
}


/**
 * @brief Returns the meta type associated with a given type info object, if
 * any.
 * @param info The type info object of the requested type.
 * @return The meta type associated with the given type info object, if any.
 */
[[nodiscard]] inline meta_type resolve(const type_info info) ENTT_NOEXCEPT {
    for(auto *curr = *internal::meta_context::global(); curr; curr = curr->next) {
        if(curr->info == info) {
            return curr;
        }
    }

    return {};
}


}


#endif

// #include "meta/template.hpp"
#ifndef ENTT_META_TEMPLATE_HPP
#define ENTT_META_TEMPLATE_HPP


// #include "../core/type_traits.hpp"



namespace entt {


/*! @brief Utility class to disambiguate class templates. */
template<template<typename...> typename>
struct meta_class_template_tag {};


/**
 * @brief General purpose traits class for generating meta template information.
 * @tparam Clazz Type of class template.
 * @tparam Args Types of template arguments.
 */
template<template<typename...> typename Clazz, typename... Args>
struct meta_template_traits<Clazz<Args...>> {
    /*! @brief Wrapped class template. */
    using class_type = meta_class_template_tag<Clazz>;
    /*! @brief List of template arguments. */
    using args_type = type_list<Args...>;
};


}


#endif

// #include "meta/type_traits.hpp"

// #include "meta/utility.hpp"

// #include "platform/android-ndk-r17.hpp"
#ifndef ENTT_PLATFORM_ANDROID_NDK_R17_HPP
#define ENTT_PLATFORM_ANDROID_NDK_R17_HPP


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


#ifdef __ANDROID__
#include <android/ndk-version.h>
#if __NDK_MAJOR__ == 17


#include <functional>
#include <type_traits>
#include <utility>


namespace std {


namespace internal {


template<typename Func, typename... Args>
constexpr auto is_invocable(int) -> decltype(std::invoke(std::declval<Func>(), std::declval<Args>()...), std::true_type{});


template<typename, typename...>
constexpr std::false_type is_invocable(...);


template<typename Ret, typename Func, typename... Args>
constexpr auto is_invocable_r(int)
-> std::enable_if_t<decltype(std::is_convertible_v<decltype(std::invoke(std::declval<Func>(), std::declval<Args>()...)), Ret>, std::true_type>;


template<typename, typename, typename...>
constexpr std::false_type is_invocable_r(...);


}


template<typename Func, typename... Args>
struct is_invocable: decltype(internal::is_invocable<Func, Args...>(0)) {};


template<typename Func, typename... Argsv>
inline constexpr bool is_invocable_v = std::is_invocable<Func, Args...>::value;


template<typename Ret, typename Func, typename... Args>
struct is_invocable_r: decltype(internal::is_invocable_r<Ret, Func, Args...>(0)) {};


template<typename Ret, typename Func, typename... Args>
inline constexpr bool is_invocable_r_v = std::is_invocable_r<Ret, Func, Args...>::value;


template<typename Func, typename...Args>
struct invoke_result {
    using type = decltype(std::invoke(std::declval<Func>(), std::declval<Args>()...));
};


template<typename Func, typename... Args>
using invoke_result_t = typename std::invoke_result<Func, Args...>::type;


}


#endif
#endif


/**
 * Internal details not to be documented.
 * @endcond
 */


#endif

// #include "poly/poly.hpp"
#ifndef ENTT_POLY_POLY_HPP
#define ENTT_POLY_POLY_HPP


#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "../core/any.hpp"
#ifndef ENTT_CORE_ANY_HPP
#define ENTT_CORE_ANY_HPP


#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "../core/utility.hpp"
#ifndef ENTT_CORE_UTILITY_HPP
#define ENTT_CORE_UTILITY_HPP


#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/*! @brief Identity function object (waiting for C++20). */
struct identity {
    /**
     * @brief Returns its argument unchanged.
     * @tparam Type Type of the argument.
     * @param value The actual argument.
     * @return The submitted value as-is.
     */
    template<class Type>
    [[nodiscard]] constexpr Type && operator()(Type &&value) const ENTT_NOEXCEPT {
        return std::forward<Type>(value);
    }
};


/**
 * @brief Constant utility to disambiguate overloaded members of a class.
 * @tparam Type Type of the desired overload.
 * @tparam Class Type of class to which the member belongs.
 * @param member A valid pointer to a member.
 * @return Pointer to the member.
 */
template<typename Type, typename Class>
[[nodiscard]] constexpr auto overload(Type Class:: *member) ENTT_NOEXCEPT { return member; }


/**
 * @brief Constant utility to disambiguate overloaded functions.
 * @tparam Func Function type of the desired overload.
 * @param func A valid pointer to a function.
 * @return Pointer to the function.
 */
template<typename Func>
[[nodiscard]] constexpr auto overload(Func *func) ENTT_NOEXCEPT { return func; }


/**
 * @brief Helper type for visitors.
 * @tparam Func Types of function objects.
 */
template<class... Func>
struct overloaded: Func... {
    using Func::operator()...;
};


/**
 * @brief Deduction guide.
 * @tparam Func Types of function objects.
 */
template<class... Func>
overloaded(Func...)
-> overloaded<Func...>;


/**
 * @brief Basic implementation of a y-combinator.
 * @tparam Func Type of a potentially recursive function.
 */
template<class Func>
struct y_combinator {
    /**
     * @brief Constructs a y-combinator from a given function.
     * @param recursive A potentially recursive function.
     */
    y_combinator(Func recursive):
        func{std::move(recursive)}
    {}

    /**
     * @brief Invokes a y-combinator and therefore its underlying function.
     * @tparam Args Types of arguments to use to invoke the underlying function.
     * @param args Parameters to use to invoke the underlying function.
     * @return Return value of the underlying function, if any.
     */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) const {
        return func(*this, std::forward<Args>(args)...);
    }

    /*! @copydoc operator()() */
    template <class... Args>
    decltype(auto) operator()(Args &&... args) {
        return func(*this, std::forward<Args>(args)...);
    }

private:
    Func func;
};


}


#endif

// #include "fwd.hpp"
#ifndef ENTT_CORE_FWD_HPP
#define ENTT_CORE_FWD_HPP


#include <type_traits>
// #include "../config/config.h"



namespace entt {


template<std::size_t Len = sizeof(double[2]), std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_any;


/*! @brief Alias declaration for type identifiers. */
using id_type = ENTT_ID_TYPE;


/*! @brief Alias declaration for the most common use case. */
using any = basic_any<>;


}


#endif

// #include "type_info.hpp"
#ifndef ENTT_CORE_TYPE_INFO_HPP
#define ENTT_CORE_TYPE_INFO_HPP


#include <string_view>
#include <type_traits>
// #include "../config/config.h"

// #include "../core/attribute.h"
#ifndef ENTT_CORE_ATTRIBUTE_H
#define ENTT_CORE_ATTRIBUTE_H


#ifndef ENTT_EXPORT
#   if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#       define ENTT_EXPORT __declspec(dllexport)
#       define ENTT_IMPORT __declspec(dllimport)
#       define ENTT_HIDDEN
#   elif defined __GNUC__ && __GNUC__ >= 4
#       define ENTT_EXPORT __attribute__((visibility("default")))
#       define ENTT_IMPORT __attribute__((visibility("default")))
#       define ENTT_HIDDEN __attribute__((visibility("hidden")))
#   else /* Unsupported compiler */
#       define ENTT_EXPORT
#       define ENTT_IMPORT
#       define ENTT_HIDDEN
#   endif
#endif


#ifndef ENTT_API
#   if defined ENTT_API_EXPORT
#       define ENTT_API ENTT_EXPORT
#   elif defined ENTT_API_IMPORT
#       define ENTT_API ENTT_IMPORT
#   else /* No API */
#       define ENTT_API
#   endif
#endif


#endif

// #include "hashed_string.hpp"
#ifndef ENTT_CORE_HASHED_STRING_HPP
#define ENTT_CORE_HASHED_STRING_HPP


#include <cstddef>
#include <cstdint>
// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
struct fnv1a_traits;


template<>
struct fnv1a_traits<std::uint32_t> {
    using type = std::uint32_t;
    static constexpr std::uint32_t offset = 2166136261;
    static constexpr std::uint32_t prime = 16777619;
};


template<>
struct fnv1a_traits<std::uint64_t> {
    using type = std::uint64_t;
    static constexpr std::uint64_t offset = 14695981039346656037ull;
    static constexpr std::uint64_t prime = 1099511628211ull;
};


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Zero overhead unique identifier.
 *
 * A hashed string is a compile-time tool that allows users to use
 * human-readable identifers in the codebase while using their numeric
 * counterparts at runtime.<br/>
 * Because of that, a hashed string can also be used in constant expressions if
 * required.
 *
 * @tparam Char Character type.
 */
template<typename Char>
class basic_hashed_string {
    using traits_type = internal::fnv1a_traits<id_type>;

    struct const_wrapper {
        // non-explicit constructor on purpose
        constexpr const_wrapper(const Char *curr) ENTT_NOEXCEPT: str{curr} {}
        const Char *str;
    };

    // Fowler–Noll–Vo hash function v. 1a - the good
    [[nodiscard]] static constexpr id_type helper(const Char *curr) ENTT_NOEXCEPT {
        auto value = traits_type::offset;

        while(*curr != 0) {
            value = (value ^ static_cast<traits_type::type>(*(curr++))) * traits_type::prime;
        }

        return value;
    }

public:
    /*! @brief Character type. */
    using value_type = Char;
    /*! @brief Unsigned integer type. */
    using hash_type = id_type;

    /**
     * @brief Returns directly the numeric representation of a string view.
     * @param str Human-readable identifer.
     * @param size Length of the string to hash.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static constexpr hash_type value(const value_type *str, std::size_t size) ENTT_NOEXCEPT {
        id_type partial{traits_type::offset};
        while(size--) { partial = (partial^(str++)[0])*traits_type::prime; }
        return partial;
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * const auto value = basic_hashed_string<char>::to_value("my.png");
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param str Human-readable identifer.
     * @return The numeric representation of the string.
     */
    template<std::size_t N>
    [[nodiscard]] static constexpr hash_type value(const value_type (&str)[N]) ENTT_NOEXCEPT {
        return helper(str);
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static hash_type value(const_wrapper wrapper) ENTT_NOEXCEPT {
        return helper(wrapper.str);
    }

    /*! @brief Constructs an empty hashed string. */
    constexpr basic_hashed_string() ENTT_NOEXCEPT
        : str{nullptr}, hash{}
    {}

    /**
     * @brief Constructs a hashed string from an array of const characters.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * basic_hashed_string<char> hs{"my.png"};
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param curr Human-readable identifer.
     */
    template<std::size_t N>
    constexpr basic_hashed_string(const value_type (&curr)[N]) ENTT_NOEXCEPT
        : str{curr}, hash{helper(curr)}
    {}

    /**
     * @brief Explicit constructor on purpose to avoid constructing a hashed
     * string directly from a `const value_type *`.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     */
    explicit constexpr basic_hashed_string(const_wrapper wrapper) ENTT_NOEXCEPT
        : str{wrapper.str}, hash{helper(wrapper.str)}
    {}

    /**
     * @brief Returns the human-readable representation of a hashed string.
     * @return The string used to initialize the instance.
     */
    [[nodiscard]] constexpr const value_type * data() const ENTT_NOEXCEPT {
        return str;
    }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr hash_type value() const ENTT_NOEXCEPT {
        return hash;
    }

    /*! @copydoc data */
    [[nodiscard]] constexpr operator const value_type *() const ENTT_NOEXCEPT { return data(); }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr operator hash_type() const ENTT_NOEXCEPT { return value(); }

    /**
     * @brief Compares two hashed strings.
     * @param other Hashed string with which to compare.
     * @return True if the two hashed strings are identical, false otherwise.
     */
    [[nodiscard]] constexpr bool operator==(const basic_hashed_string &other) const ENTT_NOEXCEPT {
        return hash == other.hash;
    }

private:
    const value_type *str;
    hash_type hash;
};


/**
 * @brief Deduction guide.
 *
 * It allows to deduce the character type of the hashed string directly from a
 * human-readable identifer provided to the constructor.
 *
 * @tparam Char Character type.
 * @tparam N Number of characters of the identifier.
 * @param str Human-readable identifer.
 */
template<typename Char, std::size_t N>
basic_hashed_string(const Char (&str)[N])
-> basic_hashed_string<Char>;


/**
 * @brief Compares two hashed strings.
 * @tparam Char Character type.
 * @param lhs A valid hashed string.
 * @param rhs A valid hashed string.
 * @return True if the two hashed strings are identical, false otherwise.
 */
template<typename Char>
[[nodiscard]] constexpr bool operator!=(const basic_hashed_string<Char> &lhs, const basic_hashed_string<Char> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/*! @brief Aliases for common character types. */
using hashed_string = basic_hashed_string<char>;


/*! @brief Aliases for common character types. */
using hashed_wstring = basic_hashed_string<wchar_t>;


inline namespace literals {


/**
 * @brief User defined literal for hashed strings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed string.
 */
[[nodiscard]] constexpr entt::hashed_string operator"" _hs(const char *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_string{str};
}


/**
 * @brief User defined literal for hashed wstrings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed wstring.
 */
[[nodiscard]] constexpr entt::hashed_wstring operator"" _hws(const wchar_t *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_wstring{str};
}


}


}


#endif

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


struct ENTT_API type_seq final {
    [[nodiscard]] static id_type next() ENTT_NOEXCEPT {
        static ENTT_MAYBE_ATOMIC(id_type) value{};
        return value++;
    }
};


template<typename Type>
[[nodiscard]] constexpr auto stripped_type_name() ENTT_NOEXCEPT {
#if defined ENTT_PRETTY_FUNCTION
    std::string_view pretty_function{ENTT_PRETTY_FUNCTION};
    auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(ENTT_PRETTY_FUNCTION_PREFIX)+1);
    auto value = pretty_function.substr(first, pretty_function.find_last_of(ENTT_PRETTY_FUNCTION_SUFFIX) - first);
    return value;
#else
    return std::string_view{""};
#endif
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr std::string_view type_name(int) ENTT_NOEXCEPT {
    constexpr auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type>
[[nodiscard]] static std::string_view type_name(char) ENTT_NOEXCEPT {
    static const auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr id_type type_hash(int) ENTT_NOEXCEPT {
    constexpr auto stripped = stripped_type_name<Type>();
    constexpr auto value = hashed_string::value(stripped.data(), stripped.size());
    return value;
}


template<typename Type>
[[nodiscard]] static id_type type_hash(char) ENTT_NOEXCEPT {
    static const auto value = [](const auto stripped) {
        return hashed_string::value(stripped.data(), stripped.size());
    }(stripped_type_name<Type>());
    return value;
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Type sequential identifier.
 * @tparam Type Type for which to generate a sequential identifier.
 */
template<typename Type, typename = void>
struct ENTT_API type_seq final {
    /**
     * @brief Returns the sequential identifier of a given type.
     * @return The sequential identifier of a given type.
     */
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const id_type value = internal::type_seq::next();
        return value;
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type hash.
 * @tparam Type Type for which to generate a hash value.
 */
template<typename Type, typename = void>
struct type_hash final {
    /**
     * @brief Returns the numeric representation of a given type.
     * @return The numeric representation of the given type.
     */
#if defined ENTT_PRETTY_FUNCTION
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return internal::type_hash<Type>(0);
#else
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return type_seq<Type>::value();
#endif
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type name.
 * @tparam Type Type for which to generate a name.
 */
template<typename Type, typename = void>
struct type_name final {
    /**
     * @brief Returns the name of a given type.
     * @return The name of the given type.
     */
    [[nodiscard]] static constexpr std::string_view value() ENTT_NOEXCEPT {
        return internal::type_name<Type>(0);
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator std::string_view() const ENTT_NOEXCEPT { return value(); }
};


/*! @brief Implementation specific information about a type. */
class type_info final {
    template<typename>
    friend type_info type_id() ENTT_NOEXCEPT;

    type_info(id_type seq_v, id_type hash_v, std::string_view name_v) ENTT_NOEXCEPT
        : seq_value{seq_v},
          hash_value{hash_v},
          name_value{name_v}
    {}

public:
    /*! @brief Default constructor. */
    type_info() ENTT_NOEXCEPT
        : type_info({}, {}, {})
    {}

    /*! @brief Default copy constructor. */
    type_info(const type_info &) ENTT_NOEXCEPT = default;
    /*! @brief Default move constructor. */
    type_info(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Default copy assignment operator.
     * @return This type info object.
     */
    type_info & operator=(const type_info &) ENTT_NOEXCEPT = default;

    /**
     * @brief Default move assignment operator.
     * @return This type info object.
     */
    type_info & operator=(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Checks if a type info object is properly initialized.
     * @return True if the object is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return name_value.data() != nullptr;
    }

    /**
     * @brief Type sequential identifier.
     * @return Type sequential identifier.
     */
    [[nodiscard]] id_type seq() const ENTT_NOEXCEPT {
        return seq_value;
    }

    /**
     * @brief Type hash.
     * @return Type hash.
     */
    [[nodiscard]] id_type hash() const ENTT_NOEXCEPT {
        return hash_value;
    }

    /**
     * @brief Type name.
     * @return Type name.
     */
    [[nodiscard]] std::string_view name() const ENTT_NOEXCEPT {
        return name_value;
    }

    /**
     * @brief Compares the contents of two type info objects.
     * @param other Object with which to compare.
     * @return False if the two contents differ, true otherwise.
     */
    [[nodiscard]] bool operator==(const type_info &other) const ENTT_NOEXCEPT {
        return hash_value == other.hash_value;
    }

private:
    id_type seq_value;
    id_type hash_value;
    std::string_view name_value;
};


/**
 * @brief Compares the contents of two type info objects.
 * @param lhs A type info object.
 * @param rhs A type info object.
 * @return True if the two contents differ, false otherwise.
 */
[[nodiscard]] inline bool operator!=(const type_info &lhs, const type_info &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Returns the type info object for a given type.
 * @tparam Type Type for which to generate a type info object.
 * @return The type info object for the given type.
 */
template<typename Type>
[[nodiscard]] type_info type_id() ENTT_NOEXCEPT {
    return type_info{
        type_seq<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_name<std::remove_cv_t<std::remove_reference_t<Type>>>::value()
    };
}


}


#endif

// #include "type_traits.hpp"
#ifndef ENTT_CORE_TYPE_TRAITS_HPP
#define ENTT_CORE_TYPE_TRAITS_HPP


#include <cstddef>
#include <utility>
#include <type_traits>
// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @brief Utility class to disambiguate overloaded functions.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
struct choice_t
    // Unfortunately, doxygen cannot parse such a construct.
    /*! @cond TURN_OFF_DOXYGEN */
    : choice_t<N-1>
    /*! @endcond */
{};


/*! @copybrief choice_t */
template<>
struct choice_t<0> {};


/**
 * @brief Variable template for the choice trick.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
inline constexpr choice_t<N> choice{};


/**
 * @brief Identity type trait.
 *
 * Useful to establish non-deduced contexts in template argument deduction
 * (waiting for C++20) or to provide types through function arguments.
 *
 * @tparam Type A type.
 */
template<typename Type>
struct type_identity {
    /*! @brief Identity type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Type A type.
 */
template<typename Type>
using type_identity_t = typename type_identity<Type>::type;


/**
 * @brief A type-only `sizeof` wrapper that returns 0 where `sizeof` complains.
 * @tparam Type The type of which to return the size.
 * @tparam The size of the type if `sizeof` accepts it, 0 otherwise.
 */
template<typename Type, typename = void>
struct size_of: std::integral_constant<std::size_t, 0u> {};


/*! @copydoc size_of */
template<typename Type>
struct size_of<Type, std::void_t<decltype(sizeof(Type))>>
    : std::integral_constant<std::size_t, sizeof(Type)>
{};


/**
 * @brief Helper variable template.
 * @tparam Type The type of which to return the size.
 */
template<class Type>
inline constexpr auto size_of_v = size_of<Type>::value;


/**
 * @brief Using declaration to be used to _repeat_ the same type a number of
 * times equal to the size of a given parameter pack.
 * @tparam Type A type to repeat.
 */
template<typename Type, typename>
using unpack_as_t = Type;


/**
 * @brief Helper variable template to be used to _repeat_ the same value a
 * number of times equal to the size of a given parameter pack.
 * @tparam Value A value to repeat.
 */
template<auto Value, typename>
inline constexpr auto unpack_as_v = Value;


/**
 * @brief Wraps a static constant.
 * @tparam Value A static constant.
 */
template<auto Value>
using integral_constant = std::integral_constant<decltype(Value), Value>;


/**
 * @brief Alias template to facilitate the creation of named values.
 * @tparam Value A constant value at least convertible to `id_type`.
 */
template<id_type Value>
using tag = integral_constant<Value>;


/**
 * @brief A class to use to push around lists of types, nothing more.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list {
    /*! @brief Type list type. */
    using type = type_list;
    /*! @brief Compile-time number of elements in the type list. */
    static constexpr auto size = sizeof...(Type);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct type_list_element;


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Index Index of the type to return.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<std::size_t Index, typename Type, typename... Other>
struct type_list_element<Index, type_list<Type, Other...>>
    : type_list_element<Index - 1u, type_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<typename Type, typename... Other>
struct type_list_element<0u, type_list<Type, Other...>> {
    /*! @brief Searched type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the type to return.
 * @tparam List Type list to search into.
 */
template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @return A type list composed by the types of both the type lists.
 */
template<typename... Type, typename... Other>
constexpr type_list<Type..., Other...> operator+(type_list<Type...>, type_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_cat;


/*! @brief Concatenates multiple type lists. */
template<>
struct type_list_cat<> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<>;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @tparam List Other type lists, if any.
 */
template<typename... Type, typename... Other, typename... List>
struct type_list_cat<type_list<Type...>, type_list<Other...>, List...> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = typename type_list_cat<type_list<Type..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list_cat<type_list<Type...>> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<Type...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists to concatenate.
 */
template<typename... List>
using type_list_cat_t = typename type_list_cat<List...>::type;


/*! @brief Primary template isn't defined on purpose. */
template<typename>
struct type_list_unique;


/**
 * @brief Removes duplicates types from a type list.
 * @tparam Type One of the types provided by the given type list.
 * @tparam Other The other types provided by the given type list.
 */
template<typename Type, typename... Other>
struct type_list_unique<type_list<Type, Other...>> {
    /*! @brief A type list without duplicate types. */
    using type = std::conditional_t<
        std::disjunction_v<std::is_same<Type, Other>...>,
        typename type_list_unique<type_list<Other...>>::type,
        type_list_cat_t<type_list<Type>, typename type_list_unique<type_list<Other...>>::type>
    >;
};


/*! @brief Removes duplicates types from a type list. */
template<>
struct type_list_unique<type_list<>> {
    /*! @brief A type list without duplicate types. */
    using type = type_list<>;
};


/**
 * @brief Helper type.
 * @tparam Type A type list.
 */
template<typename Type>
using type_list_unique_t = typename type_list_unique<Type>::type;


/**
 * @brief Provides the member constant `value` to true if a type list contains a
 * given type, false otherwise.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<typename List, typename Type>
struct type_list_contains;


/**
 * @copybrief type_list_contains
 * @tparam Type Types provided by the type list.
 * @tparam Other Type to look for.
 */
template<typename... Type, typename Other>
struct type_list_contains<type_list<Type...>, Other>: std::disjunction<std::is_same<Type, Other>...> {};


/**
 * @brief Helper variable template.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<class List, typename Type>
inline constexpr auto type_list_contains_v = type_list_contains<List, Type>::value;


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_diff;


/**
 * @brief Computes the difference between two type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 */
template<typename... Type, typename... Other>
struct type_list_diff<type_list<Type...>, type_list<Other...>> {
    /*! @brief A type list that is the difference between the two type lists. */
    using type = type_list_cat_t<std::conditional_t<type_list_contains_v<type_list<Other...>, Type>, type_list<>, type_list<Type>>...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists between which to compute the difference.
 */
template<typename... List>
using type_list_diff_t = typename type_list_diff<List...>::type;


/**
 * @brief A class to use to push around lists of constant values, nothing more.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list {
    /*! @brief Value list type. */
    using type = value_list;
    /*! @brief Compile-time number of elements in the value list. */
    static constexpr auto size = sizeof...(Value);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct value_list_element;


/**
 * @brief Provides compile-time indexed access to the values of a value list.
 * @tparam Index Index of the value to return.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<std::size_t Index, auto Value, auto... Other>
struct value_list_element<Index, value_list<Value, Other...>>
    : value_list_element<Index - 1u, value_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<auto Value, auto... Other>
struct value_list_element<0u, value_list<Value, Other...>> {
    /*! @brief Searched value. */
    static constexpr auto value = Value;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the value to return.
 * @tparam List Value list to search into.
 */
template<std::size_t Index, typename List>
inline constexpr auto value_list_element_v = value_list_element<Index, List>::value;


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @return A value list composed by the values of both the value lists.
 */
template<auto... Value, auto... Other>
constexpr value_list<Value..., Other...> operator+(value_list<Value...>, value_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct value_list_cat;


/*! @brief Concatenates multiple value lists. */
template<>
struct value_list_cat<> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<>;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @tparam List Other value lists, if any.
 */
template<auto... Value, auto... Other, typename... List>
struct value_list_cat<value_list<Value...>, value_list<Other...>, List...> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = typename value_list_cat<value_list<Value..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list_cat<value_list<Value...>> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<Value...>;
};


/**
 * @brief Helper type.
 * @tparam List Value lists to concatenate.
 */
template<typename... List>
using value_list_cat_t = typename value_list_cat<List...>::type;


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
[[nodiscard]] constexpr bool is_equality_comparable(...) { return false; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<0>)
-> decltype(std::declval<Type>() == std::declval<Type>()) { return true; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<1>)
-> decltype(std::declval<typename Type::value_type>(), std::declval<Type>() == std::declval<Type>()) {
    if constexpr(std::is_same_v<typename Type::value_type, Type>) {
        return is_equality_comparable<Type>(choice<0>);
    } else {
        return is_equality_comparable<typename Type::value_type>(choice<2>);
    }
}


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<2>)
-> decltype(std::declval<typename Type::mapped_type>(), std::declval<Type>() == std::declval<Type>()) {
    return is_equality_comparable<typename Type::key_type>(choice<2>) && is_equality_comparable<typename Type::mapped_type>(choice<2>);
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Provides the member constant `value` to true if a given type is
 * equality comparable, false otherwise.
 * @tparam Type Potentially equality comparable type.
 */
template<typename Type, typename = void>
struct is_equality_comparable: std::bool_constant<internal::is_equality_comparable<Type>(choice<2>)> {};


/**
 * @brief Helper variable template.
 * @tparam Type Potentially equality comparable type.
 */
template<class Type>
inline constexpr auto is_equality_comparable_v = is_equality_comparable<Type>::value;


/*! @brief Same as std::is_invocable, but with tuples. */
template<typename, typename>
struct is_applicable: std::false_type {};


/**
 * @copybrief is_applicable
 * @tparam Func A valid function type.
 * @tparam Tuple Tuple-like type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
* @copybrief is_applicable
* @tparam Func A valid function type.
* @tparam Tuple Tuple-like type.
* @tparam Args The list of arguments to use to probe the function type.
*/
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, const Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, typename Args>
inline constexpr auto is_applicable_v = is_applicable<Func, Args>::value;


/*! @brief Same as std::is_invocable_r, but with tuples for arguments. */
template<typename, typename, typename>
struct is_applicable_r: std::false_type {};


/**
 * @copybrief is_applicable_r
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename... Args>
struct is_applicable_r<Ret, Func, std::tuple<Args...>>: std::is_invocable_r<Ret, Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename Args>
inline constexpr auto is_applicable_r_v = is_applicable_r<Ret, Func, Args>::value;


/**
* @brief Provides the member constant `value` to true if a given type is
* complete, false otherwise.
* @tparam Type Potential complete type.
*/
template<typename Type, typename = void>
struct is_complete: std::false_type {};


/*! @copydoc is_complete */
template<typename Type>
struct is_complete<Type, std::void_t<decltype(sizeof(Type))>>: std::true_type {};


/**
* @brief Helper variable template.
* @tparam Type Potential complete type.
*/
template<typename Type>
inline constexpr auto is_complete_v = is_complete<Type>::value;


/**
 * @brief Transcribes the constness of a type to another type.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
struct constness_as {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::remove_const_t<To>;
};


/*! @copydoc constness_as */
template<typename To, typename From>
struct constness_as<To, const From> {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::add_const_t<To>;
};


/**
 * @brief Alias template to facilitate the transcription of the constness.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;


/**
 * @brief Extracts the class of a non-static member object or function.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
class member_class {
    static_assert(std::is_member_pointer_v<Member>, "Invalid pointer type to non-static member object or function");

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...));

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...) const);

    template<typename Class, typename Type>
    static Class * clazz(Type Class:: *);

public:
    /*! @brief The class of the given non-static member object or function. */
    using type = std::remove_pointer_t<decltype(clazz(std::declval<Member>()))>;
};


/**
 * @brief Helper type.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
using member_class_t = typename member_class<Member>::type;


}


#endif



namespace entt {


/**
 * @brief A SBO friendly, type-safe container for single values of any type.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 */
template<std::size_t Len, std::size_t Align>
class basic_any {
    enum class operation: std::uint8_t { COPY, MOVE, DTOR, COMP, ADDR, CADDR, TYPE };
    enum class policy: std::uint8_t { OWNER, REF, CREF };

    using storage_type = std::aligned_storage_t<Len + !Len, Align>;
    using vtable_type = const void *(const operation, const basic_any &, void *);

    template<typename Type>
    static constexpr bool in_situ = Len && alignof(Type) <= alignof(storage_type) && sizeof(Type) <= sizeof(storage_type) && std::is_nothrow_move_constructible_v<Type>;

    template<typename Type>
    [[nodiscard]] static constexpr policy type_to_policy() {
        if constexpr(std::is_lvalue_reference_v<Type>) {
            if constexpr(std::is_const_v<std::remove_reference_t<Type>>) {
                return policy::CREF;
            } else {
                return policy::REF;
            }
        } else {
            return policy::OWNER;
        }
    }

    template<typename Type>
    [[nodiscard]] static bool compare(const void *lhs, const void *rhs) {
        if constexpr(!std::is_function_v<Type> && is_equality_comparable_v<Type>) {
            return *static_cast<const Type *>(lhs) == *static_cast<const Type *>(rhs);
        } else {
            return lhs == rhs;
        }
    }

    template<typename Type>
    static const void * basic_vtable([[maybe_unused]] const operation op, [[maybe_unused]] const basic_any &from, [[maybe_unused]] void *to) {
        static_assert(std::is_same_v<std::remove_reference_t<std::remove_const_t<Type>>, Type>, "Invalid type");

        if constexpr(!std::is_void_v<Type>) {
            const Type *instance = (in_situ<Type> && from.mode == policy::OWNER)
                ? ENTT_LAUNDER(reinterpret_cast<const Type *>(&from.storage))
                : static_cast<const Type *>(from.instance);

            switch(op) {
            case operation::COPY:
                if constexpr(std::is_copy_constructible_v<Type>) {
                    static_cast<basic_any *>(to)->emplace<Type>(*instance);
                }
                break;
            case operation::MOVE:
                if constexpr(in_situ<Type>) {
                    if(from.mode == policy::OWNER) {
                        return new (&static_cast<basic_any *>(to)->storage) Type{std::move(*const_cast<Type *>(instance))};
                    }
                }

                return (static_cast<basic_any *>(to)->instance = std::exchange(const_cast<basic_any &>(from).instance, nullptr));
            case operation::DTOR:
                if(from.mode == policy::OWNER) {
                    if constexpr(in_situ<Type>) {
                        instance->~Type();
                    } else if constexpr(std::is_array_v<Type>) {
                        delete[] instance;
                    } else {
                        delete instance;
                    }
                }
                break;
            case operation::COMP:
                return compare<Type>(instance, (*static_cast<const basic_any **>(to))->data()) ? to : nullptr;
            case operation::ADDR:
                if(from.mode == policy::CREF) {
                    return nullptr;
                }
                [[fallthrough]];
            case operation::CADDR:
                return instance;
            case operation::TYPE:
                *static_cast<type_info *>(to) = type_id<Type>();
                break;
            }
        }

        return nullptr;
    }

    template<typename Type, typename... Args>
    void initialize([[maybe_unused]] Args &&... args) {
        if constexpr(!std::is_void_v<Type>) {
            if constexpr(std::is_lvalue_reference_v<Type>) {
                static_assert(sizeof...(Args) == 1u && (std::is_lvalue_reference_v<Args> && ...), "Invalid arguments");
                instance = (std::addressof(args), ...);
            } else if constexpr(in_situ<Type>) {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    new (&storage) Type{std::forward<Args>(args)...};
                } else {
                    new (&storage) Type(std::forward<Args>(args)...);
                }
            } else {
                if constexpr(sizeof...(Args) != 0u && std::is_aggregate_v<Type>) {
                    instance = new Type{std::forward<Args>(args)...};
                } else {
                    instance = new Type(std::forward<Args>(args)...);
                }
            }
        }
    }

    basic_any(const basic_any &other, const policy pol) ENTT_NOEXCEPT
        : instance{other.data()},
          vtable{other.vtable},
          mode{pol}
    {}

public:
    /*! @brief Size of the internal storage. */
    static constexpr auto length = Len;
    /*! @brief Alignment requirement. */
    static constexpr auto alignment = Align;

    /*! @brief Default constructor. */
    basic_any() ENTT_NOEXCEPT
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {}

    /**
     * @brief Constructs a wrapper by directly initializing the new object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    explicit basic_any(std::in_place_type_t<Type>, Args &&... args)
        : instance{},
          vtable{&basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>},
          mode{type_to_policy<Type>()}
    {
        initialize<Type>(std::forward<Args>(args)...);
    }

    /**
     * @brief Constructs a wrapper that holds an unmanaged object.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type>
    basic_any(std::reference_wrapper<Type> value) ENTT_NOEXCEPT
        : basic_any{}
    {
        // invokes deprecated assignment operator (and avoids issues with vs2017)
        *this = value;
    }

    /**
     * @brief Constructs a wrapper from a given value.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     */
    template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>>>
    basic_any(Type &&value)
        : instance{},
          vtable{&basic_vtable<std::decay_t<Type>>},
          mode{policy::OWNER}
    {
        initialize<std::decay_t<Type>>(std::forward<Type>(value));
    }

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    basic_any(const basic_any &other)
        : instance{},
          vtable{&basic_vtable<void>},
          mode{policy::OWNER}
    {
        other.vtable(operation::COPY, other, this);
    }

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    basic_any(basic_any &&other) ENTT_NOEXCEPT
        : instance{},
          vtable{other.vtable},
          mode{other.mode}
    {
        vtable(operation::MOVE, other, this);
    }

    /*! @brief Frees the internal storage, whatever it means. */
    ~basic_any() {
        vtable(operation::DTOR, *this, nullptr);
    }

    /**
     * @brief Copy assignment operator.
     * @param other The instance to copy from.
     * @return This any object.
     */
    basic_any & operator=(const basic_any &other) {
        reset();
        other.vtable(operation::COPY, other, this);
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This any object.
     */
    basic_any & operator=(basic_any &&other) ENTT_NOEXCEPT {
        std::exchange(vtable, other.vtable)(operation::DTOR, *this, nullptr);
        other.vtable(operation::MOVE, other, this);
        mode = other.mode;
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    [[deprecated("Use std::in_place_type<T &>, entt::make_any<T &>, emplace<Type &> or forward_as_any instead")]]
    basic_any & operator=(std::reference_wrapper<Type> value) ENTT_NOEXCEPT {
        emplace<Type &>(value.get());
        return *this;
    }

    /**
     * @brief Value assignment operator.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @param value An instance of an object to use to initialize the wrapper.
     * @return This any object.
     */
    template<typename Type>
    std::enable_if_t<!std::is_same_v<std::decay_t<Type>, basic_any>, basic_any &>
    operator=(Type &&value) {
        emplace<std::decay_t<Type>>(std::forward<Type>(value));
        return *this;
    }

    /**
     * @brief Returns the type of the contained object.
     * @return The type of the contained object, if any.
     */
    [[nodiscard]] type_info type() const ENTT_NOEXCEPT {
        type_info info{};
        vtable(operation::TYPE, *this, &info);
        return info;
    }

    /**
     * @brief Returns an opaque pointer to the contained instance.
     * @return An opaque pointer the contained instance, if any.
     */
    [[nodiscard]] const void * data() const ENTT_NOEXCEPT {
        return vtable(operation::CADDR, *this, nullptr);
    }

    /*! @copydoc data */
    [[nodiscard]] void * data() ENTT_NOEXCEPT {
        return const_cast<void *>(vtable(operation::ADDR, *this, nullptr));
    }

    /**
     * @brief Replaces the contained object by creating a new instance directly.
     * @tparam Type Type of object to use to initialize the wrapper.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    void emplace(Args &&... args) {
        std::exchange(vtable, &basic_vtable<std::remove_const_t<std::remove_reference_t<Type>>>)(operation::DTOR, *this, nullptr);
        mode = type_to_policy<Type>();
        initialize<Type>(std::forward<Args>(args)...);
    }

    /*! @brief Destroys contained object */
    void reset() {
        std::exchange(vtable, &basic_vtable<void>)(operation::DTOR, *this, nullptr);
        mode = policy::OWNER;
    }

    /**
     * @brief Returns false if a wrapper is empty, true otherwise.
     * @return False if the wrapper is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(vtable(operation::CADDR, *this, nullptr) == nullptr);
    }

    /**
     * @brief Checks if two wrappers differ in their content.
     * @param other Wrapper with which to compare.
     * @return False if the two objects differ in their content, true otherwise.
     */
    bool operator==(const basic_any &other) const ENTT_NOEXCEPT {
        const basic_any *trampoline = &other;
        return type() == other.type() && (vtable(operation::COMP, *this, &trampoline) || !other.data());
    }

    /**
     * @brief Aliasing constructor.
     * @return A wrapper that shares a reference to an unmanaged object.
     */
    [[nodiscard]] basic_any as_ref() ENTT_NOEXCEPT {
        return basic_any{*this, (mode == policy::CREF ? policy::CREF : policy::REF)};
    }

    /*! @copydoc as_ref */
    [[nodiscard]] basic_any as_ref() const ENTT_NOEXCEPT {
        return basic_any{*this, policy::CREF};
    }

    /**
     * @brief Returns true if a wrapper owns its object, false otherwise.
     * @return True if the wrapper owns its object, false otherwise.
     */
    [[nodiscard]] bool owner() const ENTT_NOEXCEPT {
        return (mode == policy::OWNER);
    }

private:
    union { const void *instance; storage_type storage; };
    vtable_type *vtable;
    policy mode;
};


/**
 * @brief Checks if two wrappers differ in their content.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param lhs A wrapper, either empty or not.
 * @param rhs A wrapper, either empty or not.
 * @return True if the two wrappers differ in their content, false otherwise.
 */
template<std::size_t Len, std::size_t Align>
[[nodiscard]] inline bool operator!=(const basic_any<Len, Align> &lhs, const basic_any<Len, Align> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Performs type-safe access to the contained object.
 * @tparam Type Type to which conversion is required.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 * @param data Target any object.
 * @return The element converted to the requested type.
 */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(const basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    const auto * const instance = any_cast<std::remove_reference_t<Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(*instance);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type any_cast(basic_any<Len, Align> &&data) ENTT_NOEXCEPT {
    // forces const on non-reference types to make them work also with wrappers for const references
    auto * const instance = any_cast<std::remove_reference_t<const Type>>(&data);
    ENTT_ASSERT(instance, "Invalid instance");
    return static_cast<Type>(std::move(*instance));
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
const Type * any_cast(const basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    return (data->type() == type_id<Type>() ? static_cast<const Type *>(data->data()) : nullptr);
}


/*! @copydoc any_cast */
template<typename Type, std::size_t Len, std::size_t Align>
Type * any_cast(basic_any<Len, Align> *data) ENTT_NOEXCEPT {
    // last attempt to make wrappers for const references return their values
    return (data->type() == type_id<Type>() ? static_cast<Type *>(static_cast<constness_as_t<basic_any<Len, Align>, Type> *>(data)->data()) : nullptr);
}


/**
 * @brief Constructs a wrapper from a given type, passing it all arguments.
 * @tparam Type Type of object to use to initialize the wrapper.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Args Types of arguments to use to construct the new instance.
 * @param args Parameters to use to construct the instance.
 * @return A properly initialized wrapper for an object of the given type.
 */
template<typename Type, std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename... Args>
basic_any<Len, Align> make_any(Args &&... args) {
    return basic_any<Len, Align>{std::in_place_type<Type>, std::forward<Args>(args)...};
}


/**
 * @brief Forwards its argument and avoids copies for lvalue references.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 * @tparam Type Type of argument to use to construct the new instance.
 * @param value Parameter to use to construct the instance.
 * @return A properly initialized and not necessarily owning wrapper.
 */
template<std::size_t Len = basic_any<>::length, std::size_t Align = basic_any<Len>::alignment, typename Type>
basic_any<Len, Align> forward_as_any(Type &&value) {
    return basic_any<Len, Align>{std::in_place_type<std::conditional_t<std::is_rvalue_reference_v<Type>, std::decay_t<Type>, Type>>, std::forward<Type>(value)};
}


}


#endif

// #include "../core/type_info.hpp"

// #include "../core/type_traits.hpp"

// #include "fwd.hpp"
#ifndef ENTT_POLY_FWD_HPP
#define ENTT_POLY_FWD_HPP


#include <type_traits>


namespace entt {


template<typename, std::size_t Len, std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_poly;


/**
 * @brief Alias declaration for the most common use case.
 * @tparam Concept Concept descriptor.
 */
template<typename Concept>
using poly = basic_poly<Concept, sizeof(double[2])>;


}


#endif



namespace entt {


/*! @brief Inspector class used to infer the type of the virtual table. */
struct poly_inspector {
    /**
     * @brief Generic conversion operator (definition only).
     * @tparam Type Type to which conversion is requested.
     */
    template <class Type>
    operator Type &&() const;

    /**
     * @brief Dummy invocation function (definition only).
     * @tparam Member Index of the function to invoke.
     * @tparam Args Types of arguments to pass to the function.
     * @param args The arguments to pass to the function.
     * @return A poly inspector convertible to any type.
     */
    template<auto Member, typename... Args>
    poly_inspector invoke(Args &&... args) const;

    /*! @copydoc invoke */
    template<auto Member, typename... Args>
    poly_inspector invoke(Args &&... args);
};


/**
 * @brief Static virtual table factory.
 * @tparam Concept Concept descriptor.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Alignment requirement.
 */
template<typename Concept, std::size_t Len, std::size_t Align>
class poly_vtable {
    using inspector = typename Concept::template type<poly_inspector>;

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(*)(inspector &, Args...)) -> Ret(*)(basic_any<Len, Align> &, Args...);

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(*)(const inspector &, Args...)) -> Ret(*)(const basic_any<Len, Align> &, Args...);

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(*)(Args...)) -> Ret(*)(const basic_any<Len, Align> &, Args...);

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(inspector:: *)(Args...)) -> Ret(*)(basic_any<Len, Align> &, Args...);

    template<typename Ret, typename... Args>
    static auto vtable_entry(Ret(inspector:: *)(Args...) const) -> Ret(*)(const basic_any<Len, Align> &, Args...);

    template<auto... Candidate>
    static auto make_vtable(value_list<Candidate...>)
    -> decltype(std::make_tuple(vtable_entry(Candidate)...));

    template<typename... Func>
    [[nodiscard]] static constexpr auto make_vtable(type_list<Func...>)  {
        if constexpr(sizeof...(Func) == 0) {
            return decltype(make_vtable(typename Concept::template impl<inspector>{})){};
        } else if constexpr((std::is_function_v<Func> && ...)) {
            return decltype(std::make_tuple(vtable_entry(std::declval<Func inspector:: *>())...)){};
        }
    }

    template<typename Type, auto Candidate, typename Ret, typename Any, typename... Args>
    static void fill_vtable_entry(Ret(* &entry)(Any &, Args...)) {
        if constexpr(std::is_invocable_r_v<Ret, decltype(Candidate), Args...>) {
            entry = +[](Any &, Args... args) -> Ret {
                return std::invoke(Candidate, std::forward<Args>(args)...);
            };
        } else {
            entry = +[](Any &instance, Args... args) -> Ret {
                return static_cast<Ret>(std::invoke(Candidate, any_cast<constness_as_t<Type, Any> &>(instance), std::forward<Args>(args)...));
            };
        }
    }

    template<typename Type, auto... Index>
    [[nodiscard]] static auto fill_vtable(std::index_sequence<Index...>) {
        type impl{};
        (fill_vtable_entry<Type, value_list_element_v<Index, typename Concept::template impl<Type>>>(std::get<Index>(impl)), ...);
        return impl;
    }

public:
    /*! @brief Virtual table type. */
    using type = decltype(make_vtable(Concept{}));

    /**
     * @brief Returns a static virtual table for a specific concept and type.
     * @tparam Type The type for which to generate the virtual table.
     * @return A static virtual table for the given concept and type.
     */
    template<typename Type>
    [[nodiscard]] static const auto * instance() {
        static_assert(std::is_same_v<Type, std::decay_t<Type>>, "Type differs from its decayed form");
        static const auto vtable = fill_vtable<Type>(std::make_index_sequence<Concept::template impl<Type>::size>{});
        return &vtable;
    }
};


/**
 * @brief Poly base class used to inject functionalities into concepts.
 * @tparam Poly The outermost poly class.
 */
template<typename Poly>
struct poly_base {
    /**
     * @brief Invokes a function from the static virtual table.
     * @tparam Member Index of the function to invoke.
     * @tparam Args Types of arguments to pass to the function.
     * @param self A reference to the poly object that made the call.
     * @param args The arguments to pass to the function.
     * @return The return value of the invoked function, if any.
     */
    template<auto Member, typename... Args>
    [[nodiscard]] decltype(auto) invoke(const poly_base &self, Args &&... args) const {
        const auto &poly = static_cast<const Poly &>(self);
        return std::get<Member>(*poly.vtable)(poly.storage, std::forward<Args>(args)...);
    }

    /*! @copydoc invoke */
    template<auto Member, typename... Args>
    [[nodiscard]] decltype(auto) invoke(poly_base &self, Args &&... args) {
        auto &poly = static_cast<Poly &>(self);
        return std::get<Member>(*poly.vtable)(poly.storage, std::forward<Args>(args)...);
    }
};


/**
 * @brief Shortcut for calling `poly_base<Type>::invoke`.
 * @tparam Member Index of the function to invoke.
 * @tparam Poly A fully defined poly object.
 * @tparam Args Types of arguments to pass to the function.
 * @param self A reference to the poly object that made the call.
 * @param args The arguments to pass to the function.
 * @return The return value of the invoked function, if any.
 */
template<auto Member, typename Poly, typename... Args>
decltype(auto) poly_call(Poly &&self, Args &&... args) {
    return std::forward<Poly>(self).template invoke<Member>(self, std::forward<Args>(args)...);
}


/**
 * @brief Static polymorphism made simple and within everyone's reach.
 *
 * Static polymorphism is a very powerful tool in C++, albeit sometimes
 * cumbersome to obtain.<br/>
 * This class aims to make it simple and easy to use.
 *
 * @note
 * Both deduced and defined static virtual tables are supported.<br/>
 * Moreover, the `poly` class template also works with unmanaged objects.
 *
 * @tparam Concept Concept descriptor.
 * @tparam Len Size of the storage reserved for the small buffer optimization.
 * @tparam Align Optional alignment requirement.
 */
template<typename Concept, std::size_t Len, std::size_t Align>
class basic_poly: private Concept::template type<poly_base<basic_poly<Concept, Len, Align>>> {
    /*! @brief A poly base is allowed to snoop into a poly object. */
    friend struct poly_base<basic_poly>;

    using vtable_type = typename poly_vtable<Concept, Len, Align>::type;

public:
    /*! @brief Concept type. */
    using concept_type = typename Concept::template type<poly_base<basic_poly>>;

    /*! @brief Default constructor. */
    basic_poly() ENTT_NOEXCEPT
        : storage{},
          vtable{}
    {}

    /**
     * @brief Constructs a poly by directly initializing the new object.
     * @tparam Type Type of object to use to initialize the poly.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    explicit basic_poly(std::in_place_type_t<Type>, Args &&... args)
        : storage{std::in_place_type<Type>, std::forward<Args>(args)...},
          vtable{poly_vtable<Concept, Len, Align>::template instance<std::remove_const_t<std::remove_reference_t<Type>>>()}
    {}

    /**
     * @brief Constructs a poly from a given value.
     * @tparam Type Type of object to use to initialize the poly.
     * @param value An instance of an object to use to initialize the poly.
     */
    template<typename Type, typename = std::enable_if_t<!std::is_same_v<std::remove_cv_t<std::remove_reference_t<Type>>, basic_poly>>>
    basic_poly(Type &&value) ENTT_NOEXCEPT
        : basic_poly{std::in_place_type<std::remove_cv_t<std::remove_reference_t<Type>>>, std::forward<Type>(value)}
    {}

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    basic_poly(const basic_poly &other) = default;

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    basic_poly(basic_poly &&other) ENTT_NOEXCEPT
        : basic_poly{}
    {
        swap(*this, other);
    }

    /**
     * @brief Assignment operator.
     * @param other The instance to assign from.
     * @return This poly object.
     */
    basic_poly & operator=(basic_poly other) {
        swap(other, *this);
        return *this;
    }

    /**
     * @brief Returns the type of the contained object.
     * @return The type of the contained object, if any.
     */
    [[nodiscard]] type_info type() const ENTT_NOEXCEPT {
        return storage.type();
    }

    /**
     * @brief Returns an opaque pointer to the contained instance.
     * @return An opaque pointer the contained instance, if any.
     */
    [[nodiscard]] const void * data() const ENTT_NOEXCEPT {
        return storage.data();
    }

    /*! @copydoc data */
    [[nodiscard]] void * data() ENTT_NOEXCEPT {
        return storage.data();
    }

    /**
     * @brief Replaces the contained object by creating a new instance directly.
     * @tparam Type Type of object to use to initialize the poly.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<typename Type, typename... Args>
    void emplace(Args &&... args) {
        *this = basic_poly{std::in_place_type<Type>, std::forward<Args>(args)...};
    }

    /*! @brief Destroys contained object */
    void reset() {
        *this = basic_poly{};
    }

    /**
     * @brief Returns false if a poly is empty, true otherwise.
     * @return False if the poly is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return !(vtable == nullptr);
    }

    /**
     * @brief Returns a pointer to the underlying concept.
     * @return A pointer to the underlying concept.
     */
    [[nodiscard]] concept_type * operator->() ENTT_NOEXCEPT {
        return this;
    }

    /*! @copydoc operator-> */
    [[nodiscard]] const concept_type * operator->() const ENTT_NOEXCEPT {
        return this;
    }

    /**
     * @brief Swaps two poly objects.
     * @param lhs A valid poly object.
     * @param rhs A valid poly object.
     */
    friend void swap(basic_poly &lhs, basic_poly &rhs) {
        using std::swap;
        swap(lhs.storage, rhs.storage);
        swap(lhs.vtable, rhs.vtable);
    }

    /**
     * @brief Aliasing constructor.
     * @return A poly that shares a reference to an unmanaged object.
     */
    [[nodiscard]] basic_poly as_ref() ENTT_NOEXCEPT {
        basic_poly ref = std::as_const(*this).as_ref();
        ref.storage = storage.as_ref();
        return ref;
    }

    /*! @copydoc as_ref */
    [[nodiscard]] basic_poly as_ref() const ENTT_NOEXCEPT {
        basic_poly ref{};
        ref.storage = storage.as_ref();
        ref.vtable = vtable;
        return ref;
    }

private:
    basic_any<Len, Align> storage;
    const vtable_type *vtable;
};


}


#endif

// #include "process/process.hpp"
#ifndef ENTT_PROCESS_PROCESS_HPP
#define ENTT_PROCESS_PROCESS_HPP


#include <utility>
#include <type_traits>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/**
 * @brief Base class for processes.
 *
 * This class stays true to the CRTP idiom. Derived classes must specify what's
 * the intended type for elapsed times.<br/>
 * A process should expose publicly the following member functions whether
 * required:
 *
 * * @code{.cpp}
 *   void update(Delta, void *);
 *   @endcode
 *
 *   It's invoked once per tick until a process is explicitly aborted or it
 *   terminates either with or without errors. Even though it's not mandatory to
 *   declare this member function, as a rule of thumb each process should at
 *   least define it to work properly. The `void *` parameter is an opaque
 *   pointer to user data (if any) forwarded directly to the process during an
 *   update.
 *
 * * @code{.cpp}
 *   void init();
 *   @endcode
 *
 *   It's invoked when the process joins the running queue of a scheduler. This
 *   happens as soon as it's attached to the scheduler if the process is a top
 *   level one, otherwise when it replaces its parent if the process is a
 *   continuation.
 *
 * * @code{.cpp}
 *   void succeeded();
 *   @endcode
 *
 *   It's invoked in case of success, immediately after an update and during the
 *   same tick.
 *
 * * @code{.cpp}
 *   void failed();
 *   @endcode
 *
 *   It's invoked in case of errors, immediately after an update and during the
 *   same tick.
 *
 * * @code{.cpp}
 *   void aborted();
 *   @endcode
 *
 *   It's invoked only if a process is explicitly aborted. There is no guarantee
 *   that it executes in the same tick, this depends solely on whether the
 *   process is aborted immediately or not.
 *
 * Derived classes can change the internal state of a process by invoking the
 * `succeed` and `fail` protected member functions and even pause or unpause the
 * process itself.
 *
 * @sa scheduler
 *
 * @tparam Derived Actual type of process that extends the class template.
 * @tparam Delta Type to use to provide elapsed time.
 */
template<typename Derived, typename Delta>
class process {
    enum class state: unsigned int {
        UNINITIALIZED = 0,
        RUNNING,
        PAUSED,
        SUCCEEDED,
        FAILED,
        ABORTED,
        FINISHED,
        REJECTED
    };

    template<typename Target = Derived>
    auto next(std::integral_constant<state, state::UNINITIALIZED>)
    -> decltype(std::declval<Target>().init(), void()) {
        static_cast<Target *>(this)->init();
    }

    template<typename Target = Derived>
    auto next(std::integral_constant<state, state::RUNNING>, Delta delta, void *data)
    -> decltype(std::declval<Target>().update(delta, data), void()) {
        static_cast<Target *>(this)->update(delta, data);
    }

    template<typename Target = Derived>
    auto next(std::integral_constant<state, state::SUCCEEDED>)
    -> decltype(std::declval<Target>().succeeded(), void()) {
        static_cast<Target *>(this)->succeeded();
    }

    template<typename Target = Derived>
    auto next(std::integral_constant<state, state::FAILED>)
    -> decltype(std::declval<Target>().failed(), void()) {
        static_cast<Target *>(this)->failed();
    }

    template<typename Target = Derived>
    auto next(std::integral_constant<state, state::ABORTED>)
    -> decltype(std::declval<Target>().aborted(), void()) {
        static_cast<Target *>(this)->aborted();
    }

    void next(...) const ENTT_NOEXCEPT {}

protected:
    /**
     * @brief Terminates a process with success if it's still alive.
     *
     * The function is idempotent and it does nothing if the process isn't
     * alive.
     */
    void succeed() ENTT_NOEXCEPT {
        if(alive()) {
            current = state::SUCCEEDED;
        }
    }

    /**
     * @brief Terminates a process with errors if it's still alive.
     *
     * The function is idempotent and it does nothing if the process isn't
     * alive.
     */
    void fail() ENTT_NOEXCEPT {
        if(alive()) {
            current = state::FAILED;
        }
    }

    /**
     * @brief Stops a process if it's in a running state.
     *
     * The function is idempotent and it does nothing if the process isn't
     * running.
     */
    void pause() ENTT_NOEXCEPT {
        if(current == state::RUNNING) {
            current = state::PAUSED;
        }
    }

    /**
     * @brief Restarts a process if it's paused.
     *
     * The function is idempotent and it does nothing if the process isn't
     * paused.
     */
    void unpause() ENTT_NOEXCEPT {
        if(current  == state::PAUSED) {
            current  = state::RUNNING;
        }
    }

public:
    /*! @brief Type used to provide elapsed time. */
    using delta_type = Delta;

    /*! @brief Default destructor. */
    virtual ~process() {
        static_assert(std::is_base_of_v<process, Derived>, "Incorrect use of the class template");
    }

    /**
     * @brief Aborts a process if it's still alive.
     *
     * The function is idempotent and it does nothing if the process isn't
     * alive.
     *
     * @param immediately Requests an immediate operation.
     */
    void abort(const bool immediately = false) {
        if(alive()) {
            current = state::ABORTED;

            if(immediately) {
                tick({});
            }
        }
    }

    /**
     * @brief Returns true if a process is either running or paused.
     * @return True if the process is still alive, false otherwise.
     */
    [[nodiscard]] bool alive() const ENTT_NOEXCEPT {
        return current == state::RUNNING || current == state::PAUSED;
    }

    /**
     * @brief Returns true if a process is already terminated.
     * @return True if the process is terminated, false otherwise.
     */
    [[nodiscard]] bool finished() const ENTT_NOEXCEPT {
        return current == state::FINISHED;
    }

    /**
     * @brief Returns true if a process is currently paused.
     * @return True if the process is paused, false otherwise.
     */
    [[nodiscard]] bool paused() const ENTT_NOEXCEPT {
        return current == state::PAUSED;
    }

    /**
     * @brief Returns true if a process terminated with errors.
     * @return True if the process terminated with errors, false otherwise.
     */
    [[nodiscard]] bool rejected() const ENTT_NOEXCEPT {
        return current == state::REJECTED;
    }

    /**
     * @brief Updates a process and its internal state if required.
     * @param delta Elapsed time.
     * @param data Optional data.
     */
    void tick(const Delta delta, void *data = nullptr) {
        switch (current) {
        case state::UNINITIALIZED:
            next(std::integral_constant<state, state::UNINITIALIZED>{});
            current = state::RUNNING;
            break;
        case state::RUNNING:
            next(std::integral_constant<state, state::RUNNING>{}, delta, data);
            break;
        default:
            // suppress warnings
            break;
        }

        // if it's dead, it must be notified and removed immediately
        switch(current) {
        case state::SUCCEEDED:
            next(std::integral_constant<state, state::SUCCEEDED>{});
            current = state::FINISHED;
            break;
        case state::FAILED:
            next(std::integral_constant<state, state::FAILED>{});
            current = state::REJECTED;
            break;
        case state::ABORTED:
            next(std::integral_constant<state, state::ABORTED>{});
            current = state::REJECTED;
            break;
        default:
            // suppress warnings
            break;
        }
    }

private:
    state current{state::UNINITIALIZED};
};


/**
 * @brief Adaptor for lambdas and functors to turn them into processes.
 *
 * Lambdas and functors can't be used directly with a scheduler for they are not
 * properly defined processes with managed life cycles.<br/>
 * This class helps in filling the gap and turning lambdas and functors into
 * full featured processes usable by a scheduler.
 *
 * The signature of the function call operator should be equivalent to the
 * following:
 *
 * @code{.cpp}
 * void(Delta delta, void *data, auto succeed, auto fail);
 * @endcode
 *
 * Where:
 *
 * * `delta` is the elapsed time.
 * * `data` is an opaque pointer to user data if any, `nullptr` otherwise.
 * * `succeed` is a function to call when a process terminates with success.
 * * `fail` is a function to call when a process terminates with errors.
 *
 * The signature of the function call operator of both `succeed` and `fail`
 * is equivalent to the following:
 *
 * @code{.cpp}
 * void();
 * @endcode
 *
 * Usually users shouldn't worry about creating adaptors. A scheduler will
 * create them internally each and avery time a lambda or a functor is used as
 * a process.
 *
 * @sa process
 * @sa scheduler
 *
 * @tparam Func Actual type of process.
 * @tparam Delta Type to use to provide elapsed time.
 */
template<typename Func, typename Delta>
struct process_adaptor: process<process_adaptor<Func, Delta>, Delta>, private Func {
    /**
     * @brief Constructs a process adaptor from a lambda or a functor.
     * @tparam Args Types of arguments to use to initialize the actual process.
     * @param args Parameters to use to initialize the actual process.
     */
    template<typename... Args>
    process_adaptor(Args &&... args)
        : Func{std::forward<Args>(args)...}
    {}

    /**
     * @brief Updates a process and its internal state if required.
     * @param delta Elapsed time.
     * @param data Optional data.
     */
    void update(const Delta delta, void *data) {
        Func::operator()(delta, data, [this]() { this->succeed(); }, [this]() { this->fail(); });
    }
};


}


#endif

// #include "process/scheduler.hpp"
#ifndef ENTT_PROCESS_SCHEDULER_HPP
#define ENTT_PROCESS_SCHEDULER_HPP


#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <type_traits>
// #include "../config/config.h"

// #include "process.hpp"



namespace entt {


/**
 * @brief Cooperative scheduler for processes.
 *
 * A cooperative scheduler runs processes and helps managing their life cycles.
 *
 * Each process is invoked once per tick. If a process terminates, it's
 * removed automatically from the scheduler and it's never invoked again.<br/>
 * A process can also have a child. In this case, the process is replaced with
 * its child when it terminates if it returns with success. In case of errors,
 * both the process and its child are discarded.
 *
 * Example of use (pseudocode):
 *
 * @code{.cpp}
 * scheduler.attach([](auto delta, void *, auto succeed, auto fail) {
 *     // code
 * }).then<my_process>(arguments...);
 * @endcode
 *
 * In order to invoke all scheduled processes, call the `update` member function
 * passing it the elapsed time to forward to the tasks.
 *
 * @sa process
 *
 * @tparam Delta Type to use to provide elapsed time.
 */
template<typename Delta>
class scheduler {
    struct process_handler {
        using instance_type = std::unique_ptr<void, void(*)(void *)>;
        using update_fn_type = bool(process_handler &, Delta, void *);
        using abort_fn_type = void(process_handler &, bool);
        using next_type = std::unique_ptr<process_handler>;

        instance_type instance;
        update_fn_type *update;
        abort_fn_type *abort;
        next_type next;
    };

    struct continuation {
        continuation(process_handler *ref)
            : handler{ref}
        {}

        template<typename Proc, typename... Args>
        continuation then(Args &&... args) {
            static_assert(std::is_base_of_v<process<Proc, Delta>, Proc>, "Invalid process type");
            auto proc = typename process_handler::instance_type{new Proc{std::forward<Args>(args)...}, &scheduler::deleter<Proc>};
            handler->next.reset(new process_handler{std::move(proc), &scheduler::update<Proc>, &scheduler::abort<Proc>, nullptr});
            handler = handler->next.get();
            return *this;
        }

        template<typename Func>
        continuation then(Func &&func) {
            return then<process_adaptor<std::decay_t<Func>, Delta>>(std::forward<Func>(func));
        }

    private:
        process_handler *handler;
    };

    template<typename Proc>
    [[nodiscard]] static bool update(process_handler &handler, const Delta delta, void *data) {
        auto *process = static_cast<Proc *>(handler.instance.get());
        process->tick(delta, data);

        if(process->rejected()) {
            return true;
        } else if(process->finished()) {
            if(handler.next) {
                handler = std::move(*handler.next);
                // forces the process to exit the uninitialized state
                return handler.update(handler, {}, nullptr);
            }

            return true;
        }

        return false;
    }

    template<typename Proc>
    static void abort(process_handler &handler, const bool immediately) {
        static_cast<Proc *>(handler.instance.get())->abort(immediately);
    }

    template<typename Proc>
    static void deleter(void *proc) {
        delete static_cast<Proc *>(proc);
    }

public:
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;

    /*! @brief Default constructor. */
    scheduler() = default;

    /*! @brief Default move constructor. */
    scheduler(scheduler &&) = default;

    /*! @brief Default move assignment operator. @return This scheduler. */
    scheduler & operator=(scheduler &&) = default;

    /**
     * @brief Number of processes currently scheduled.
     * @return Number of processes currently scheduled.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return handlers.size();
    }

    /**
     * @brief Returns true if at least a process is currently scheduled.
     * @return True if there are scheduled processes, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return handlers.empty();
    }

    /**
     * @brief Discards all scheduled processes.
     *
     * Processes aren't aborted. They are discarded along with their children
     * and never executed again.
     */
    void clear() {
        handlers.clear();
    }

    /**
     * @brief Schedules a process for the next tick.
     *
     * Returned value is an opaque object that can be used to attach a child to
     * the given process. The child is automatically scheduled when the process
     * terminates and only if the process returns with success.
     *
     * Example of use (pseudocode):
     *
     * @code{.cpp}
     * // schedules a task in the form of a process class
     * scheduler.attach<my_process>(arguments...)
     * // appends a child in the form of a lambda function
     * .then([](auto delta, void *, auto succeed, auto fail) {
     *     // code
     * })
     * // appends a child in the form of another process class
     * .then<my_other_process>();
     * @endcode
     *
     * @tparam Proc Type of process to schedule.
     * @tparam Args Types of arguments to use to initialize the process.
     * @param args Parameters to use to initialize the process.
     * @return An opaque object to use to concatenate processes.
     */
    template<typename Proc, typename... Args>
    auto attach(Args &&... args) {
        static_assert(std::is_base_of_v<process<Proc, Delta>, Proc>, "Invalid process type");
        auto proc = typename process_handler::instance_type{new Proc{std::forward<Args>(args)...}, &scheduler::deleter<Proc>};
        process_handler handler{std::move(proc), &scheduler::update<Proc>, &scheduler::abort<Proc>, nullptr};
        // forces the process to exit the uninitialized state
        handler.update(handler, {}, nullptr);
        return continuation{&handlers.emplace_back(std::move(handler))};
    }

    /**
     * @brief Schedules a process for the next tick.
     *
     * A process can be either a lambda or a functor. The scheduler wraps both
     * of them in a process adaptor internally.<br/>
     * The signature of the function call operator should be equivalent to the
     * following:
     *
     * @code{.cpp}
     * void(Delta delta, void *data, auto succeed, auto fail);
     * @endcode
     *
     * Where:
     *
     * * `delta` is the elapsed time.
     * * `data` is an opaque pointer to user data if any, `nullptr` otherwise.
     * * `succeed` is a function to call when a process terminates with success.
     * * `fail` is a function to call when a process terminates with errors.
     *
     * The signature of the function call operator of both `succeed` and `fail`
     * is equivalent to the following:
     *
     * @code{.cpp}
     * void();
     * @endcode
     *
     * Returned value is an opaque object that can be used to attach a child to
     * the given process. The child is automatically scheduled when the process
     * terminates and only if the process returns with success.
     *
     * Example of use (pseudocode):
     *
     * @code{.cpp}
     * // schedules a task in the form of a lambda function
     * scheduler.attach([](auto delta, void *, auto succeed, auto fail) {
     *     // code
     * })
     * // appends a child in the form of another lambda function
     * .then([](auto delta, void *, auto succeed, auto fail) {
     *     // code
     * })
     * // appends a child in the form of a process class
     * .then<my_process>(arguments...);
     * @endcode
     *
     * @sa process_adaptor
     *
     * @tparam Func Type of process to schedule.
     * @param func Either a lambda or a functor to use as a process.
     * @return An opaque object to use to concatenate processes.
     */
    template<typename Func>
    auto attach(Func &&func) {
        using Proc = process_adaptor<std::decay_t<Func>, Delta>;
        return attach<Proc>(std::forward<Func>(func));
    }

    /**
     * @brief Updates all scheduled processes.
     *
     * All scheduled processes are executed in no specific order.<br/>
     * If a process terminates with success, it's replaced with its child, if
     * any. Otherwise, if a process terminates with an error, it's removed along
     * with its child.
     *
     * @param delta Elapsed time.
     * @param data Optional data.
     */
    void update(const Delta delta, void *data = nullptr) {
        auto sz = handlers.size();

        for(auto pos = handlers.size(); pos; --pos) {
            auto &handler = handlers[pos-1];

            if(const auto dead = handler.update(handler, delta, data); dead) {
                std::swap(handler, handlers[--sz]);
            }
        }

        handlers.erase(handlers.begin() + sz, handlers.end());
    }

    /**
     * @brief Aborts all scheduled processes.
     *
     * Unless an immediate operation is requested, the abort is scheduled for
     * the next tick. Processes won't be executed anymore in any case.<br/>
     * Once a process is fully aborted and thus finished, it's discarded along
     * with its child, if any.
     *
     * @param immediately Requests an immediate operation.
     */
    void abort(const bool immediately = false) {
        decltype(handlers) exec;
        exec.swap(handlers);

        for(auto &&handler: exec) {
            handler.abort(handler, immediately);
        }

        std::move(handlers.begin(), handlers.end(), std::back_inserter(exec));
        handlers.swap(exec);
    }

private:
    std::vector<process_handler> handlers{};
};


}


#endif

// #include "resource/cache.hpp"
#ifndef ENTT_RESOURCE_CACHE_HPP
#define ENTT_RESOURCE_CACHE_HPP


#include <type_traits>
#include <unordered_map>
#include <utility>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "../core/fwd.hpp"
#ifndef ENTT_CORE_FWD_HPP
#define ENTT_CORE_FWD_HPP


#include <type_traits>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


template<std::size_t Len = sizeof(double[2]), std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_any;


/*! @brief Alias declaration for type identifiers. */
using id_type = ENTT_ID_TYPE;


/*! @brief Alias declaration for the most common use case. */
using any = basic_any<>;


}


#endif

// #include "handle.hpp"
#ifndef ENTT_RESOURCE_HANDLE_HPP
#define ENTT_RESOURCE_HANDLE_HPP


#include <memory>
#include <utility>
// #include "../config/config.h"

// #include "fwd.hpp"
#ifndef ENTT_RESOURCE_FWD_HPP
#define ENTT_RESOURCE_FWD_HPP


namespace entt {


template<typename>
struct resource_cache;


template<typename>
struct resource_handle;


template<typename, typename>
class resource_loader;


}


#endif



namespace entt {


/**
 * @brief Shared resource handle.
 *
 * A shared resource handle is a small class that wraps a resource and keeps it
 * alive even if it's deleted from the cache. It can be either copied or
 * moved. A handle shares a reference to the same resource with all the other
 * handles constructed for the same identifier.<br/>
 * As a rule of thumb, resources should never be copied nor moved. Handles are
 * the way to go to keep references to them.
 *
 * @tparam Resource Type of resource managed by a handle.
 */
template<typename Resource>
struct resource_handle {
    /*! @brief Default constructor. */
    resource_handle() ENTT_NOEXCEPT = default;

    /**
     * @brief Creates a handle from a shared pointer, namely a resource.
     * @param res A pointer to a properly initialized resource.
     */
    resource_handle(std::shared_ptr<Resource> res) ENTT_NOEXCEPT
        : resource{std::move(res)}
    {}

    /**
     * @brief Gets a reference to the managed resource.
     *
     * @warning
     * The behavior is undefined if the handle doesn't contain a resource.
     *
     * @return A reference to the managed resource.
     */
    [[nodiscard]] const Resource & get() const ENTT_NOEXCEPT {
        ENTT_ASSERT(static_cast<bool>(resource), "Invalid resource");
        return *resource;
    }

    /*! @copydoc get */
    [[nodiscard]] Resource & get() ENTT_NOEXCEPT {
        return const_cast<Resource &>(std::as_const(*this).get());
    }

    /*! @copydoc get */
    [[nodiscard]] operator const Resource & () const ENTT_NOEXCEPT {
        return get();
    }

    /*! @copydoc get */
    [[nodiscard]] operator Resource & () ENTT_NOEXCEPT {
        return get();
    }

    /*! @copydoc get */
    [[nodiscard]] const Resource & operator *() const ENTT_NOEXCEPT {
        return get();
    }

    /*! @copydoc get */
    [[nodiscard]] Resource & operator *() ENTT_NOEXCEPT {
        return get();
    }

    /**
     * @brief Gets a pointer to the managed resource.
     *
     * @warning
     * The behavior is undefined if the handle doesn't contain a resource.
     *
     * @return A pointer to the managed resource or `nullptr` if the handle
     * contains no resource at all.
     */
    [[nodiscard]] const Resource * operator->() const ENTT_NOEXCEPT {
        return resource.get();
    }

    /*! @copydoc operator-> */
    [[nodiscard]] Resource * operator->() ENTT_NOEXCEPT {
        return const_cast<Resource *>(std::as_const(*this).operator->());
    }

    /**
     * @brief Returns true if a handle contains a resource, false otherwise.
     * @return True if the handle contains a resource, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return static_cast<bool>(resource);
    }

private:
    std::shared_ptr<Resource> resource;
};


}


#endif

// #include "loader.hpp"
#ifndef ENTT_RESOURCE_LOADER_HPP
#define ENTT_RESOURCE_LOADER_HPP


// #include "fwd.hpp"

// #include "handle.hpp"



namespace entt {


/**
 * @brief Base class for resource loaders.
 *
 * Resource loaders must inherit from this class and stay true to the CRTP
 * idiom. Moreover, a resource loader must expose a public, const member
 * function named `load` that accepts a variable number of arguments and returns
 * a handle to the resource just created.<br/>
 * As an example:
 *
 * @code{.cpp}
 * struct my_resource {};
 *
 * struct my_loader: entt::resource_loader<my_loader, my_resource> {
 *     resource_handle<my_resource> load(int value) const {
 *         // use the integer value somehow
 *         return std::make_shared<my_resource>();
 *     }
 * };
 * @endcode
 *
 * In general, resource loaders should not have a state or retain data of any
 * type. They should let the cache manage their resources instead.
 *
 * @note
 * Base class and CRTP idiom aren't strictly required with the current
 * implementation. One could argue that a cache can easily work with loaders of
 * any type. However, future changes won't be breaking ones by forcing the use
 * of a base class today and that's why the model is already in its place.
 *
 * @tparam Loader Type of the derived class.
 * @tparam Resource Type of resource for which to use the loader.
 */
template<typename Loader, typename Resource>
class resource_loader {
    /*! @brief Resource loaders are friends of their caches. */
    friend struct resource_cache<Resource>;

    /**
     * @brief Loads the resource and returns it.
     * @tparam Args Types of arguments for the loader.
     * @param args Arguments for the loader.
     * @return The resource just loaded or an empty pointer in case of errors.
     */
    template<typename... Args>
    [[nodiscard]] resource_handle<Resource> get(Args &&... args) const {
        return static_cast<const Loader *>(this)->load(std::forward<Args>(args)...);
    }
};


}


#endif

// #include "fwd.hpp"



namespace entt {


/**
 * @brief Simple cache for resources of a given type.
 *
 * Minimal implementation of a cache for resources of a given type. It doesn't
 * offer much functionalities but it's suitable for small or medium sized
 * applications and can be freely inherited to add targeted functionalities for
 * large sized applications.
 *
 * @tparam Resource Type of resources managed by a cache.
 */
template<typename Resource>
struct resource_cache {
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Type of resources managed by a cache. */
    using resource_type = Resource;

    /*! @brief Default constructor. */
    resource_cache() = default;

    /*! @brief Default move constructor. */
    resource_cache(resource_cache &&) = default;

    /*! @brief Default move assignment operator. @return This cache. */
    resource_cache & operator=(resource_cache &&) = default;

    /**
     * @brief Number of resources managed by a cache.
     * @return Number of resources currently stored.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return resources.size();
    }

    /**
     * @brief Returns true if a cache contains no resources, false otherwise.
     * @return True if the cache contains no resources, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return resources.empty();
    }

    /**
     * @brief Clears a cache and discards all its resources.
     *
     * Handles are not invalidated and the memory used by a resource isn't
     * freed as long as at least a handle keeps the resource itself alive.
     */
    void clear() ENTT_NOEXCEPT {
        resources.clear();
    }

    /**
     * @brief Loads the resource that corresponds to a given identifier.
     *
     * In case an identifier isn't already present in the cache, it loads its
     * resource and stores it aside for future uses. Arguments are forwarded
     * directly to the loader in order to construct properly the requested
     * resource.
     *
     * @note
     * If the identifier is already present in the cache, this function does
     * nothing and the arguments are simply discarded.
     *
     * @warning
     * If the resource cannot be loaded correctly, the returned handle will be
     * invalid and any use of it will result in undefined behavior.
     *
     * @tparam Loader Type of loader to use to load the resource if required.
     * @tparam Args Types of arguments to use to load the resource if required.
     * @param id Unique resource identifier.
     * @param args Arguments to use to load the resource if required.
     * @return A handle for the given resource.
     */
    template<typename Loader, typename... Args>
    resource_handle<Resource> load(const id_type id, Args &&... args) {
        static_assert(std::is_base_of_v<resource_loader<Loader, Resource>, Loader>, "Invalid loader type");

        if(auto it = resources.find(id); it == resources.cend()) {
            if(auto handle = temp<Loader>(std::forward<Args>(args)...); handle) {
                return (resources[id] = std::move(handle));
            }
        } else {
            return it->second;
        }

        return {};
    }

    /**
     * @brief Reloads a resource or loads it for the first time if not present.
     *
     * Equivalent to the following snippet (pseudocode):
     *
     * @code{.cpp}
     * cache.discard(id);
     * cache.load(id, args...);
     * @endcode
     *
     * Arguments are forwarded directly to the loader in order to construct
     * properly the requested resource.
     *
     * @warning
     * If the resource cannot be loaded correctly, the returned handle will be
     * invalid and any use of it will result in undefined behavior.
     *
     * @tparam Loader Type of loader to use to load the resource.
     * @tparam Args Types of arguments to use to load the resource.
     * @param id Unique resource identifier.
     * @param args Arguments to use to load the resource.
     * @return A handle for the given resource.
     */
    template<typename Loader, typename... Args>
    resource_handle<Resource> reload(const id_type id, Args &&... args) {
        return (discard(id), load<Loader>(id, std::forward<Args>(args)...));
    }

    /**
     * @brief Creates a temporary handle for a resource.
     *
     * Arguments are forwarded directly to the loader in order to construct
     * properly the requested resource. The handle isn't stored aside and the
     * cache isn't in charge of the lifetime of the resource itself.
     *
     * @tparam Loader Type of loader to use to load the resource.
     * @tparam Args Types of arguments to use to load the resource.
     * @param args Arguments to use to load the resource.
     * @return A handle for the given resource.
     */
    template<typename Loader, typename... Args>
    [[nodiscard]] resource_handle<Resource> temp(Args &&... args) const {
        return Loader{}.get(std::forward<Args>(args)...);
    }

    /**
     * @brief Creates a handle for a given resource identifier.
     *
     * A resource handle can be in a either valid or invalid state. In other
     * terms, a resource handle is properly initialized with a resource if the
     * cache contains the resource itself. Otherwise the returned handle is
     * uninitialized and accessing it results in undefined behavior.
     *
     * @sa resource_handle
     *
     * @param id Unique resource identifier.
     * @return A handle for the given resource.
     */
    [[nodiscard]] resource_handle<Resource> handle(const id_type id) const {
        if(auto it = resources.find(id); it != resources.cend()) {
            return it->second;
        }

        return {};
    }

    /**
     * @brief Checks if a cache contains a given identifier.
     * @param id Unique resource identifier.
     * @return True if the cache contains the resource, false otherwise.
     */
    [[nodiscard]] bool contains(const id_type id) const {
        return (resources.find(id) != resources.cend());
    }

    /**
     * @brief Discards the resource that corresponds to a given identifier.
     *
     * Handles are not invalidated and the memory used by the resource isn't
     * freed as long as at least a handle keeps the resource itself alive.
     *
     * @param id Unique resource identifier.
     */
    void discard(const id_type id) {
        if(auto it = resources.find(id); it != resources.end()) {
            resources.erase(it);
        }
    }

    /**
     * @brief Iterates all resources.
     *
     * The function object is invoked for each element. It is provided with
     * either the resource identifier, the resource handle or both of them.<br/>
     * The signature of the function must be equivalent to one of the following
     * forms:
     *
     * @code{.cpp}
     * void(const entt::id_type);
     * void(entt::resource_handle<Resource>);
     * void(const entt::id_type, entt::resource_handle<Resource>);
     * @endcode
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template <typename Func>
    void each(Func func) const {
        auto begin = resources.begin();
        auto end = resources.end();

        while(begin != end) {
            auto curr = begin++;

            if constexpr(std::is_invocable_v<Func, id_type>) {
                func(curr->first);
            } else if constexpr(std::is_invocable_v<Func, resource_handle<Resource>>) {
                func(curr->second);
            } else {
                func(curr->first, curr->second);
            }
        }
    }

private:
    std::unordered_map<id_type, resource_handle<Resource>> resources;
};


}


#endif

// #include "resource/handle.hpp"

// #include "resource/loader.hpp"

// #include "signal/delegate.hpp"
#ifndef ENTT_SIGNAL_DELEGATE_HPP
#define ENTT_SIGNAL_DELEGATE_HPP


#include <tuple>
#include <cstddef>
#include <utility>
#include <functional>
#include <type_traits>
// #include "../core/type_traits.hpp"
#ifndef ENTT_CORE_TYPE_TRAITS_HPP
#define ENTT_CORE_TYPE_TRAITS_HPP


#include <cstddef>
#include <utility>
#include <type_traits>
// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif

// #include "fwd.hpp"
#ifndef ENTT_CORE_FWD_HPP
#define ENTT_CORE_FWD_HPP


#include <type_traits>
// #include "../config/config.h"



namespace entt {


template<std::size_t Len = sizeof(double[2]), std::size_t = alignof(typename std::aligned_storage_t<Len + !Len>)>
class basic_any;


/*! @brief Alias declaration for type identifiers. */
using id_type = ENTT_ID_TYPE;


/*! @brief Alias declaration for the most common use case. */
using any = basic_any<>;


}


#endif



namespace entt {


/**
 * @brief Utility class to disambiguate overloaded functions.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
struct choice_t
    // Unfortunately, doxygen cannot parse such a construct.
    /*! @cond TURN_OFF_DOXYGEN */
    : choice_t<N-1>
    /*! @endcond */
{};


/*! @copybrief choice_t */
template<>
struct choice_t<0> {};


/**
 * @brief Variable template for the choice trick.
 * @tparam N Number of choices available.
 */
template<std::size_t N>
inline constexpr choice_t<N> choice{};


/**
 * @brief Identity type trait.
 *
 * Useful to establish non-deduced contexts in template argument deduction
 * (waiting for C++20) or to provide types through function arguments.
 *
 * @tparam Type A type.
 */
template<typename Type>
struct type_identity {
    /*! @brief Identity type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Type A type.
 */
template<typename Type>
using type_identity_t = typename type_identity<Type>::type;


/**
 * @brief A type-only `sizeof` wrapper that returns 0 where `sizeof` complains.
 * @tparam Type The type of which to return the size.
 * @tparam The size of the type if `sizeof` accepts it, 0 otherwise.
 */
template<typename Type, typename = void>
struct size_of: std::integral_constant<std::size_t, 0u> {};


/*! @copydoc size_of */
template<typename Type>
struct size_of<Type, std::void_t<decltype(sizeof(Type))>>
    : std::integral_constant<std::size_t, sizeof(Type)>
{};


/**
 * @brief Helper variable template.
 * @tparam Type The type of which to return the size.
 */
template<class Type>
inline constexpr auto size_of_v = size_of<Type>::value;


/**
 * @brief Using declaration to be used to _repeat_ the same type a number of
 * times equal to the size of a given parameter pack.
 * @tparam Type A type to repeat.
 */
template<typename Type, typename>
using unpack_as_t = Type;


/**
 * @brief Helper variable template to be used to _repeat_ the same value a
 * number of times equal to the size of a given parameter pack.
 * @tparam Value A value to repeat.
 */
template<auto Value, typename>
inline constexpr auto unpack_as_v = Value;


/**
 * @brief Wraps a static constant.
 * @tparam Value A static constant.
 */
template<auto Value>
using integral_constant = std::integral_constant<decltype(Value), Value>;


/**
 * @brief Alias template to facilitate the creation of named values.
 * @tparam Value A constant value at least convertible to `id_type`.
 */
template<id_type Value>
using tag = integral_constant<Value>;


/**
 * @brief A class to use to push around lists of types, nothing more.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list {
    /*! @brief Type list type. */
    using type = type_list;
    /*! @brief Compile-time number of elements in the type list. */
    static constexpr auto size = sizeof...(Type);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct type_list_element;


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Index Index of the type to return.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<std::size_t Index, typename Type, typename... Other>
struct type_list_element<Index, type_list<Type, Other...>>
    : type_list_element<Index - 1u, type_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Type First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<typename Type, typename... Other>
struct type_list_element<0u, type_list<Type, Other...>> {
    /*! @brief Searched type. */
    using type = Type;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the type to return.
 * @tparam List Type list to search into.
 */
template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @return A type list composed by the types of both the type lists.
 */
template<typename... Type, typename... Other>
constexpr type_list<Type..., Other...> operator+(type_list<Type...>, type_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_cat;


/*! @brief Concatenates multiple type lists. */
template<>
struct type_list_cat<> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<>;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 * @tparam List Other type lists, if any.
 */
template<typename... Type, typename... Other, typename... List>
struct type_list_cat<type_list<Type...>, type_list<Other...>, List...> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = typename type_list_cat<type_list<Type..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple type lists.
 * @tparam Type Types provided by the type list.
 */
template<typename... Type>
struct type_list_cat<type_list<Type...>> {
    /*! @brief A type list composed by the types of all the type lists. */
    using type = type_list<Type...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists to concatenate.
 */
template<typename... List>
using type_list_cat_t = typename type_list_cat<List...>::type;


/*! @brief Primary template isn't defined on purpose. */
template<typename>
struct type_list_unique;


/**
 * @brief Removes duplicates types from a type list.
 * @tparam Type One of the types provided by the given type list.
 * @tparam Other The other types provided by the given type list.
 */
template<typename Type, typename... Other>
struct type_list_unique<type_list<Type, Other...>> {
    /*! @brief A type list without duplicate types. */
    using type = std::conditional_t<
        std::disjunction_v<std::is_same<Type, Other>...>,
        typename type_list_unique<type_list<Other...>>::type,
        type_list_cat_t<type_list<Type>, typename type_list_unique<type_list<Other...>>::type>
    >;
};


/*! @brief Removes duplicates types from a type list. */
template<>
struct type_list_unique<type_list<>> {
    /*! @brief A type list without duplicate types. */
    using type = type_list<>;
};


/**
 * @brief Helper type.
 * @tparam Type A type list.
 */
template<typename Type>
using type_list_unique_t = typename type_list_unique<Type>::type;


/**
 * @brief Provides the member constant `value` to true if a type list contains a
 * given type, false otherwise.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<typename List, typename Type>
struct type_list_contains;


/**
 * @copybrief type_list_contains
 * @tparam Type Types provided by the type list.
 * @tparam Other Type to look for.
 */
template<typename... Type, typename Other>
struct type_list_contains<type_list<Type...>, Other>: std::disjunction<std::is_same<Type, Other>...> {};


/**
 * @brief Helper variable template.
 * @tparam List Type list.
 * @tparam Type Type to look for.
 */
template<class List, typename Type>
inline constexpr auto type_list_contains_v = type_list_contains<List, Type>::value;


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct type_list_diff;


/**
 * @brief Computes the difference between two type lists.
 * @tparam Type Types provided by the first type list.
 * @tparam Other Types provided by the second type list.
 */
template<typename... Type, typename... Other>
struct type_list_diff<type_list<Type...>, type_list<Other...>> {
    /*! @brief A type list that is the difference between the two type lists. */
    using type = type_list_cat_t<std::conditional_t<type_list_contains_v<type_list<Other...>, Type>, type_list<>, type_list<Type>>...>;
};


/**
 * @brief Helper type.
 * @tparam List Type lists between which to compute the difference.
 */
template<typename... List>
using type_list_diff_t = typename type_list_diff<List...>::type;


/**
 * @brief A class to use to push around lists of constant values, nothing more.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list {
    /*! @brief Value list type. */
    using type = value_list;
    /*! @brief Compile-time number of elements in the value list. */
    static constexpr auto size = sizeof...(Value);
};


/*! @brief Primary template isn't defined on purpose. */
template<std::size_t, typename>
struct value_list_element;


/**
 * @brief Provides compile-time indexed access to the values of a value list.
 * @tparam Index Index of the value to return.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<std::size_t Index, auto Value, auto... Other>
struct value_list_element<Index, value_list<Value, Other...>>
    : value_list_element<Index - 1u, value_list<Other...>>
{};


/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam Value First value provided by the value list.
 * @tparam Other Other values provided by the value list.
 */
template<auto Value, auto... Other>
struct value_list_element<0u, value_list<Value, Other...>> {
    /*! @brief Searched value. */
    static constexpr auto value = Value;
};


/**
 * @brief Helper type.
 * @tparam Index Index of the value to return.
 * @tparam List Value list to search into.
 */
template<std::size_t Index, typename List>
inline constexpr auto value_list_element_v = value_list_element<Index, List>::value;


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @return A value list composed by the values of both the value lists.
 */
template<auto... Value, auto... Other>
constexpr value_list<Value..., Other...> operator+(value_list<Value...>, value_list<Other...>) { return {}; }


/*! @brief Primary template isn't defined on purpose. */
template<typename...>
struct value_list_cat;


/*! @brief Concatenates multiple value lists. */
template<>
struct value_list_cat<> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<>;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the first value list.
 * @tparam Other Values provided by the second value list.
 * @tparam List Other value lists, if any.
 */
template<auto... Value, auto... Other, typename... List>
struct value_list_cat<value_list<Value...>, value_list<Other...>, List...> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = typename value_list_cat<value_list<Value..., Other...>, List...>::type;
};


/**
 * @brief Concatenates multiple value lists.
 * @tparam Value Values provided by the value list.
 */
template<auto... Value>
struct value_list_cat<value_list<Value...>> {
    /*! @brief A value list composed by the values of all the value lists. */
    using type = value_list<Value...>;
};


/**
 * @brief Helper type.
 * @tparam List Value lists to concatenate.
 */
template<typename... List>
using value_list_cat_t = typename value_list_cat<List...>::type;


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
[[nodiscard]] constexpr bool is_equality_comparable(...) { return false; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<0>)
-> decltype(std::declval<Type>() == std::declval<Type>()) { return true; }


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<1>)
-> decltype(std::declval<typename Type::value_type>(), std::declval<Type>() == std::declval<Type>()) {
    if constexpr(std::is_same_v<typename Type::value_type, Type>) {
        return is_equality_comparable<Type>(choice<0>);
    } else {
        return is_equality_comparable<typename Type::value_type>(choice<2>);
    }
}


template<typename Type>
[[nodiscard]] constexpr auto is_equality_comparable(choice_t<2>)
-> decltype(std::declval<typename Type::mapped_type>(), std::declval<Type>() == std::declval<Type>()) {
    return is_equality_comparable<typename Type::key_type>(choice<2>) && is_equality_comparable<typename Type::mapped_type>(choice<2>);
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Provides the member constant `value` to true if a given type is
 * equality comparable, false otherwise.
 * @tparam Type Potentially equality comparable type.
 */
template<typename Type, typename = void>
struct is_equality_comparable: std::bool_constant<internal::is_equality_comparable<Type>(choice<2>)> {};


/**
 * @brief Helper variable template.
 * @tparam Type Potentially equality comparable type.
 */
template<class Type>
inline constexpr auto is_equality_comparable_v = is_equality_comparable<Type>::value;


/*! @brief Same as std::is_invocable, but with tuples. */
template<typename, typename>
struct is_applicable: std::false_type {};


/**
 * @copybrief is_applicable
 * @tparam Func A valid function type.
 * @tparam Tuple Tuple-like type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
* @copybrief is_applicable
* @tparam Func A valid function type.
* @tparam Tuple Tuple-like type.
* @tparam Args The list of arguments to use to probe the function type.
*/
template<typename Func, template<typename...> class Tuple, typename... Args>
struct is_applicable<Func, const Tuple<Args...>>: std::is_invocable<Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Func, typename Args>
inline constexpr auto is_applicable_v = is_applicable<Func, Args>::value;


/*! @brief Same as std::is_invocable_r, but with tuples for arguments. */
template<typename, typename, typename>
struct is_applicable_r: std::false_type {};


/**
 * @copybrief is_applicable_r
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename... Args>
struct is_applicable_r<Ret, Func, std::tuple<Args...>>: std::is_invocable_r<Ret, Func, Args...> {};


/**
 * @brief Helper variable template.
 * @tparam Ret The type to which the return type of the function should be
 * convertible.
 * @tparam Func A valid function type.
 * @tparam Args The list of arguments to use to probe the function type.
 */
template<typename Ret, typename Func, typename Args>
inline constexpr auto is_applicable_r_v = is_applicable_r<Ret, Func, Args>::value;


/**
* @brief Provides the member constant `value` to true if a given type is
* complete, false otherwise.
* @tparam Type Potential complete type.
*/
template<typename Type, typename = void>
struct is_complete: std::false_type {};


/*! @copydoc is_complete */
template<typename Type>
struct is_complete<Type, std::void_t<decltype(sizeof(Type))>>: std::true_type {};


/**
* @brief Helper variable template.
* @tparam Type Potential complete type.
*/
template<typename Type>
inline constexpr auto is_complete_v = is_complete<Type>::value;


/**
 * @brief Transcribes the constness of a type to another type.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
struct constness_as {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::remove_const_t<To>;
};


/*! @copydoc constness_as */
template<typename To, typename From>
struct constness_as<To, const From> {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::add_const_t<To>;
};


/**
 * @brief Alias template to facilitate the transcription of the constness.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;


/**
 * @brief Extracts the class of a non-static member object or function.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
class member_class {
    static_assert(std::is_member_pointer_v<Member>, "Invalid pointer type to non-static member object or function");

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...));

    template<typename Class, typename Ret, typename... Args>
    static Class * clazz(Ret(Class:: *)(Args...) const);

    template<typename Class, typename Type>
    static Class * clazz(Type Class:: *);

public:
    /*! @brief The class of the given non-static member object or function. */
    using type = std::remove_pointer_t<decltype(clazz(std::declval<Member>()))>;
};


/**
 * @brief Helper type.
 * @tparam Member A pointer to a non-static member object or function.
 */
template<typename Member>
using member_class_t = typename member_class<Member>::type;


}


#endif

// #include "../config/config.h"
#ifndef ENTT_CONFIG_CONFIG_H
#define ENTT_CONFIG_CONFIG_H


#if defined(__cpp_exceptions) && !defined(ENTT_NOEXCEPTION)
#   define ENTT_NOEXCEPT noexcept
#   define ENTT_THROW throw
#   define ENTT_TRY try
#   define ENTT_CATCH catch(...)
#else
#   define ENTT_NOEXCEPT
#   define ENTT_THROW
#   define ENTT_TRY if(true)
#   define ENTT_CATCH if(false)
#endif


#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606L
#   include <new>
#   define ENTT_LAUNDER(expr) std::launder(expr)
#else
#   define ENTT_LAUNDER(expr) expr
#endif


#ifndef ENTT_USE_ATOMIC
#   define ENTT_MAYBE_ATOMIC(Type) Type
#else
#   include <atomic>
#   define ENTT_MAYBE_ATOMIC(Type) std::atomic<Type>
#endif


#ifndef ENTT_ID_TYPE
#   include <cstdint>
#   define ENTT_ID_TYPE std::uint32_t
#endif


#ifdef ENTT_SPARSE_PAGE
	static_assert(ENTT_SPARSE_PAGE && ((ENTT_SPARSE_PAGE & (ENTT_SPARSE_PAGE - 1)) == 0), "ENTT_SPARSE_PAGE must be a power of two");
#else
#   define ENTT_SPARSE_PAGE 4096
#endif


#ifdef ENTT_PACKED_PAGE
static_assert(ENTT_PACKED_PAGE && ((ENTT_PACKED_PAGE & (ENTT_PACKED_PAGE - 1)) == 0), "ENTT_PACKED_PAGE must be a power of two");
#else
#   define ENTT_PACKED_PAGE 1024
#endif


#ifdef ENTT_DISABLE_ASSERT
#   undef ENTT_ASSERT
#   define ENTT_ASSERT(...) (void(0))
#elif !defined ENTT_ASSERT
#   include <cassert>
#   define ENTT_ASSERT(condition, ...) assert(condition)
#endif


#ifdef ENTT_NO_ETO
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::false_type
#else
#   include <type_traits>
#   define ENTT_IGNORE_IF_EMPTY std::true_type
#endif


#ifndef ENTT_STANDARD_CPP
#    if defined __clang__ || defined __GNUC__
#       define ENTT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       define ENTT_PRETTY_FUNCTION_PREFIX '='
#       define ENTT_PRETTY_FUNCTION_SUFFIX ']'
#    elif defined _MSC_VER
#       define ENTT_PRETTY_FUNCTION __FUNCSIG__
#       define ENTT_PRETTY_FUNCTION_PREFIX '<'
#       define ENTT_PRETTY_FUNCTION_SUFFIX '>'
#   endif
#endif


#endif



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename Ret, typename... Args>
auto function_pointer(Ret(*)(Args...)) -> Ret(*)(Args...);


template<typename Ret, typename Type, typename... Args, typename Other>
auto function_pointer(Ret(*)(Type, Args...), Other &&) -> Ret(*)(Args...);


template<typename Class, typename Ret, typename... Args, typename... Other>
auto function_pointer(Ret(Class:: *)(Args...), Other &&...) -> Ret(*)(Args...);


template<typename Class, typename Ret, typename... Args, typename... Other>
auto function_pointer(Ret(Class:: *)(Args...) const, Other &&...) -> Ret(*)(Args...);


template<typename Class, typename Type, typename... Other>
auto function_pointer(Type Class:: *, Other &&...) -> Type(*)();


template<typename... Type>
using function_pointer_t = decltype(internal::function_pointer(std::declval<Type>()...));


template<typename... Class, typename Ret, typename... Args>
[[nodiscard]] constexpr auto index_sequence_for(Ret(*)(Args...)) {
    return std::index_sequence_for<Class..., Args...>{};
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/*! @brief Used to wrap a function or a member of a specified type. */
template<auto>
struct connect_arg_t {};


/*! @brief Constant of type connect_arg_t used to disambiguate calls. */
template<auto Func>
inline constexpr connect_arg_t<Func> connect_arg{};


/**
 * @brief Basic delegate implementation.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error unless the template parameter is a function type.
 */
template<typename>
class delegate;


/**
 * @brief Utility class to use to send around functions and members.
 *
 * Unmanaged delegate for function pointers and members. Users of this class are
 * in charge of disconnecting instances before deleting them.
 *
 * A delegate can be used as a general purpose invoker without memory overhead
 * for free functions possibly with payloads and bound or unbound members.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
class delegate<Ret(Args...)> {
    template<auto Candidate, std::size_t... Index>
    [[nodiscard]] auto wrap(std::index_sequence<Index...>) ENTT_NOEXCEPT {
        return [](const void *, Args... args) -> Ret {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);
            return static_cast<Ret>(std::invoke(Candidate, std::forward<type_list_element_t<Index, type_list<Args...>>>(std::get<Index>(arguments))...));
        };
    }

    template<auto Candidate, typename Type, std::size_t... Index>
    [[nodiscard]] auto wrap(Type &, std::index_sequence<Index...>) ENTT_NOEXCEPT {
        return [](const void *payload, Args... args) -> Ret {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);
            Type *curr = static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(payload));
            return static_cast<Ret>(std::invoke(Candidate, *curr, std::forward<type_list_element_t<Index, type_list<Args...>>>(std::get<Index>(arguments))...));
        };
    }

    template<auto Candidate, typename Type, std::size_t... Index>
    [[nodiscard]] auto wrap(Type *, std::index_sequence<Index...>) ENTT_NOEXCEPT {
        return [](const void *payload, Args... args) -> Ret {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);
            Type *curr = static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(payload));
            return static_cast<Ret>(std::invoke(Candidate, curr, std::forward<type_list_element_t<Index, type_list<Args...>>>(std::get<Index>(arguments))...));
        };
    }

public:
    /*! @brief Function type of the contained target. */
    using function_type = Ret(const void *, Args...);
    /*! @brief Function type of the delegate. */
    using type = Ret(Args...);
    /*! @brief Return type of the delegate. */
    using result_type = Ret;

    /*! @brief Default constructor. */
    delegate() ENTT_NOEXCEPT
        : fn{nullptr}, data{nullptr}
    {}

    /**
     * @brief Constructs a delegate and connects a free function or an unbound
     * member.
     * @tparam Candidate Function or member to connect to the delegate.
     */
    template<auto Candidate>
    delegate(connect_arg_t<Candidate>) ENTT_NOEXCEPT {
        connect<Candidate>();
    }

    /**
     * @brief Constructs a delegate and connects a free function with payload or
     * a bound member.
     * @tparam Candidate Function or member to connect to the delegate.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<auto Candidate, typename Type>
    delegate(connect_arg_t<Candidate>, Type &&value_or_instance) ENTT_NOEXCEPT {
        connect<Candidate>(std::forward<Type>(value_or_instance));
    }

    /**
     * @brief Constructs a delegate and connects an user defined function with
     * optional payload.
     * @param function Function to connect to the delegate.
     * @param payload User defined arbitrary data.
     */
    delegate(function_type *function, const void *payload = nullptr) ENTT_NOEXCEPT {
        connect(function, payload);
    }

    /**
     * @brief Connects a free function or an unbound member to a delegate.
     * @tparam Candidate Function or member to connect to the delegate.
     */
    template<auto Candidate>
    void connect() ENTT_NOEXCEPT {
        data = nullptr;

        if constexpr(std::is_invocable_r_v<Ret, decltype(Candidate), Args...>) {
            fn = [](const void *, Args... args) -> Ret {
                return Ret(std::invoke(Candidate, std::forward<Args>(args)...));
            };
        } else if constexpr(std::is_member_pointer_v<decltype(Candidate)>) {
            fn = wrap<Candidate>(internal::index_sequence_for<type_list_element_t<0, type_list<Args...>>>(internal::function_pointer_t<decltype(Candidate)>{}));
        } else {
            fn = wrap<Candidate>(internal::index_sequence_for(internal::function_pointer_t<decltype(Candidate)>{}));
        }
    }

    /**
     * @brief Connects a free function with payload or a bound member to a
     * delegate.
     *
     * The delegate isn't responsible for the connected object or the payload.
     * Users must always guarantee that the lifetime of the instance overcomes
     * the one of the delegate.<br/>
     * When used to connect a free function with payload, its signature must be
     * such that the instance is the first argument before the ones used to
     * define the delegate itself.
     *
     * @tparam Candidate Function or member to connect to the delegate.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid reference that fits the purpose.
     */
    template<auto Candidate, typename Type>
    void connect(Type &value_or_instance) ENTT_NOEXCEPT {
        data = &value_or_instance;

        if constexpr(std::is_invocable_r_v<Ret, decltype(Candidate), Type &, Args...>) {
            fn = [](const void *payload, Args... args) -> Ret {
                Type *curr = static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(payload));
                return Ret(std::invoke(Candidate, *curr, std::forward<Args>(args)...));
            };
        } else {
            fn = wrap<Candidate>(value_or_instance, internal::index_sequence_for(internal::function_pointer_t<decltype(Candidate), Type>{}));
        }
    }

    /**
     * @brief Connects a free function with payload or a bound member to a
     * delegate.
     *
     * @sa connect(Type &)
     *
     * @tparam Candidate Function or member to connect to the delegate.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid pointer that fits the purpose.
     */
    template<auto Candidate, typename Type>
    void connect(Type *value_or_instance) ENTT_NOEXCEPT {
        data = value_or_instance;

        if constexpr(std::is_invocable_r_v<Ret, decltype(Candidate), Type *, Args...>) {
            fn = [](const void *payload, Args... args) -> Ret {
                Type *curr = static_cast<Type *>(const_cast<constness_as_t<void, Type> *>(payload));
                return Ret(std::invoke(Candidate, curr, std::forward<Args>(args)...));
            };
        } else {
            fn = wrap<Candidate>(value_or_instance, internal::index_sequence_for(internal::function_pointer_t<decltype(Candidate), Type>{}));
        }
    }

    /**
     * @brief Connects an user defined function with optional payload to a
     * delegate.
     *
     * The delegate isn't responsible for the connected object or the payload.
     * Users must always guarantee that the lifetime of an instance overcomes
     * the one of the delegate.<br/>
     * The payload is returned as the first argument to the target function in
     * all cases.
     *
     * @param function Function to connect to the delegate.
     * @param payload User defined arbitrary data.
     */
    void connect(function_type *function, const void *payload = nullptr) ENTT_NOEXCEPT {
        fn = function;
        data = payload;
    }

    /**
     * @brief Resets a delegate.
     *
     * After a reset, a delegate cannot be invoked anymore.
     */
    void reset() ENTT_NOEXCEPT {
        fn = nullptr;
        data = nullptr;
    }

    /**
     * @brief Returns the instance or the payload linked to a delegate, if any.
     * @return An opaque pointer to the underlying data.
     */
    [[nodiscard]] const void * instance() const ENTT_NOEXCEPT {
        return data;
    }

    /**
     * @brief Triggers a delegate.
     *
     * The delegate invokes the underlying function and returns the result.
     *
     * @warning
     * Attempting to trigger an invalid delegate results in undefined
     * behavior.
     *
     * @param args Arguments to use to invoke the underlying function.
     * @return The value returned by the underlying function.
     */
    Ret operator()(Args... args) const {
        ENTT_ASSERT(static_cast<bool>(*this), "Uninitialized delegate");
        return fn(data, std::forward<Args>(args)...);
    }

    /**
     * @brief Checks whether a delegate actually stores a listener.
     * @return False if the delegate is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        // no need to test also data
        return !(fn == nullptr);
    }

    /**
     * @brief Compares the contents of two delegates.
     * @param other Delegate with which to compare.
     * @return False if the two contents differ, true otherwise.
     */
    [[nodiscard]] bool operator==(const delegate<Ret(Args...)> &other) const ENTT_NOEXCEPT {
        return fn == other.fn && data == other.data;
    }

private:
    function_type *fn;
    const void *data;
};


/**
 * @brief Compares the contents of two delegates.
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 * @param lhs A valid delegate object.
 * @param rhs A valid delegate object.
 * @return True if the two contents differ, false otherwise.
 */
template<typename Ret, typename... Args>
[[nodiscard]] bool operator!=(const delegate<Ret(Args...)> &lhs, const delegate<Ret(Args...)> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Deduction guide.
 * @tparam Candidate Function or member to connect to the delegate.
 */
template<auto Candidate>
delegate(connect_arg_t<Candidate>)
-> delegate<std::remove_pointer_t<internal::function_pointer_t<decltype(Candidate)>>>;


/**
 * @brief Deduction guide.
 * @tparam Candidate Function or member to connect to the delegate.
 * @tparam Type Type of class or type of payload.
 */
template<auto Candidate, typename Type>
delegate(connect_arg_t<Candidate>, Type &&)
-> delegate<std::remove_pointer_t<internal::function_pointer_t<decltype(Candidate), Type>>>;


/**
 * @brief Deduction guide.
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
delegate(Ret(*)(const void *, Args...), const void * = nullptr)
-> delegate<Ret(Args...)>;


}


#endif

// #include "signal/dispatcher.hpp"
#ifndef ENTT_SIGNAL_DISPATCHER_HPP
#define ENTT_SIGNAL_DISPATCHER_HPP


#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
// #include "../config/config.h"

// #include "../core/fwd.hpp"

// #include "../core/type_info.hpp"
#ifndef ENTT_CORE_TYPE_INFO_HPP
#define ENTT_CORE_TYPE_INFO_HPP


#include <string_view>
#include <type_traits>
// #include "../config/config.h"

// #include "../core/attribute.h"
#ifndef ENTT_CORE_ATTRIBUTE_H
#define ENTT_CORE_ATTRIBUTE_H


#ifndef ENTT_EXPORT
#   if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#       define ENTT_EXPORT __declspec(dllexport)
#       define ENTT_IMPORT __declspec(dllimport)
#       define ENTT_HIDDEN
#   elif defined __GNUC__ && __GNUC__ >= 4
#       define ENTT_EXPORT __attribute__((visibility("default")))
#       define ENTT_IMPORT __attribute__((visibility("default")))
#       define ENTT_HIDDEN __attribute__((visibility("hidden")))
#   else /* Unsupported compiler */
#       define ENTT_EXPORT
#       define ENTT_IMPORT
#       define ENTT_HIDDEN
#   endif
#endif


#ifndef ENTT_API
#   if defined ENTT_API_EXPORT
#       define ENTT_API ENTT_EXPORT
#   elif defined ENTT_API_IMPORT
#       define ENTT_API ENTT_IMPORT
#   else /* No API */
#       define ENTT_API
#   endif
#endif


#endif

// #include "hashed_string.hpp"
#ifndef ENTT_CORE_HASHED_STRING_HPP
#define ENTT_CORE_HASHED_STRING_HPP


#include <cstddef>
#include <cstdint>
// #include "../config/config.h"

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


template<typename>
struct fnv1a_traits;


template<>
struct fnv1a_traits<std::uint32_t> {
    using type = std::uint32_t;
    static constexpr std::uint32_t offset = 2166136261;
    static constexpr std::uint32_t prime = 16777619;
};


template<>
struct fnv1a_traits<std::uint64_t> {
    using type = std::uint64_t;
    static constexpr std::uint64_t offset = 14695981039346656037ull;
    static constexpr std::uint64_t prime = 1099511628211ull;
};


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Zero overhead unique identifier.
 *
 * A hashed string is a compile-time tool that allows users to use
 * human-readable identifers in the codebase while using their numeric
 * counterparts at runtime.<br/>
 * Because of that, a hashed string can also be used in constant expressions if
 * required.
 *
 * @tparam Char Character type.
 */
template<typename Char>
class basic_hashed_string {
    using traits_type = internal::fnv1a_traits<id_type>;

    struct const_wrapper {
        // non-explicit constructor on purpose
        constexpr const_wrapper(const Char *curr) ENTT_NOEXCEPT: str{curr} {}
        const Char *str;
    };

    // Fowler–Noll–Vo hash function v. 1a - the good
    [[nodiscard]] static constexpr id_type helper(const Char *curr) ENTT_NOEXCEPT {
        auto value = traits_type::offset;

        while(*curr != 0) {
            value = (value ^ static_cast<traits_type::type>(*(curr++))) * traits_type::prime;
        }

        return value;
    }

public:
    /*! @brief Character type. */
    using value_type = Char;
    /*! @brief Unsigned integer type. */
    using hash_type = id_type;

    /**
     * @brief Returns directly the numeric representation of a string view.
     * @param str Human-readable identifer.
     * @param size Length of the string to hash.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static constexpr hash_type value(const value_type *str, std::size_t size) ENTT_NOEXCEPT {
        id_type partial{traits_type::offset};
        while(size--) { partial = (partial^(str++)[0])*traits_type::prime; }
        return partial;
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * const auto value = basic_hashed_string<char>::to_value("my.png");
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param str Human-readable identifer.
     * @return The numeric representation of the string.
     */
    template<std::size_t N>
    [[nodiscard]] static constexpr hash_type value(const value_type (&str)[N]) ENTT_NOEXCEPT {
        return helper(str);
    }

    /**
     * @brief Returns directly the numeric representation of a string.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     * @return The numeric representation of the string.
     */
    [[nodiscard]] static hash_type value(const_wrapper wrapper) ENTT_NOEXCEPT {
        return helper(wrapper.str);
    }

    /*! @brief Constructs an empty hashed string. */
    constexpr basic_hashed_string() ENTT_NOEXCEPT
        : str{nullptr}, hash{}
    {}

    /**
     * @brief Constructs a hashed string from an array of const characters.
     *
     * Forcing template resolution avoids implicit conversions. An
     * human-readable identifier can be anything but a plain, old bunch of
     * characters.<br/>
     * Example of use:
     * @code{.cpp}
     * basic_hashed_string<char> hs{"my.png"};
     * @endcode
     *
     * @tparam N Number of characters of the identifier.
     * @param curr Human-readable identifer.
     */
    template<std::size_t N>
    constexpr basic_hashed_string(const value_type (&curr)[N]) ENTT_NOEXCEPT
        : str{curr}, hash{helper(curr)}
    {}

    /**
     * @brief Explicit constructor on purpose to avoid constructing a hashed
     * string directly from a `const value_type *`.
     * @param wrapper Helps achieving the purpose by relying on overloading.
     */
    explicit constexpr basic_hashed_string(const_wrapper wrapper) ENTT_NOEXCEPT
        : str{wrapper.str}, hash{helper(wrapper.str)}
    {}

    /**
     * @brief Returns the human-readable representation of a hashed string.
     * @return The string used to initialize the instance.
     */
    [[nodiscard]] constexpr const value_type * data() const ENTT_NOEXCEPT {
        return str;
    }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr hash_type value() const ENTT_NOEXCEPT {
        return hash;
    }

    /*! @copydoc data */
    [[nodiscard]] constexpr operator const value_type *() const ENTT_NOEXCEPT { return data(); }

    /**
     * @brief Returns the numeric representation of a hashed string.
     * @return The numeric representation of the instance.
     */
    [[nodiscard]] constexpr operator hash_type() const ENTT_NOEXCEPT { return value(); }

    /**
     * @brief Compares two hashed strings.
     * @param other Hashed string with which to compare.
     * @return True if the two hashed strings are identical, false otherwise.
     */
    [[nodiscard]] constexpr bool operator==(const basic_hashed_string &other) const ENTT_NOEXCEPT {
        return hash == other.hash;
    }

private:
    const value_type *str;
    hash_type hash;
};


/**
 * @brief Deduction guide.
 *
 * It allows to deduce the character type of the hashed string directly from a
 * human-readable identifer provided to the constructor.
 *
 * @tparam Char Character type.
 * @tparam N Number of characters of the identifier.
 * @param str Human-readable identifer.
 */
template<typename Char, std::size_t N>
basic_hashed_string(const Char (&str)[N])
-> basic_hashed_string<Char>;


/**
 * @brief Compares two hashed strings.
 * @tparam Char Character type.
 * @param lhs A valid hashed string.
 * @param rhs A valid hashed string.
 * @return True if the two hashed strings are identical, false otherwise.
 */
template<typename Char>
[[nodiscard]] constexpr bool operator!=(const basic_hashed_string<Char> &lhs, const basic_hashed_string<Char> &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/*! @brief Aliases for common character types. */
using hashed_string = basic_hashed_string<char>;


/*! @brief Aliases for common character types. */
using hashed_wstring = basic_hashed_string<wchar_t>;


inline namespace literals {


/**
 * @brief User defined literal for hashed strings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed string.
 */
[[nodiscard]] constexpr entt::hashed_string operator"" _hs(const char *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_string{str};
}


/**
 * @brief User defined literal for hashed wstrings.
 * @param str The literal without its suffix.
 * @return A properly initialized hashed wstring.
 */
[[nodiscard]] constexpr entt::hashed_wstring operator"" _hws(const wchar_t *str, std::size_t) ENTT_NOEXCEPT {
    return entt::hashed_wstring{str};
}


}


}


#endif

// #include "fwd.hpp"



namespace entt {


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


namespace internal {


struct ENTT_API type_seq final {
    [[nodiscard]] static id_type next() ENTT_NOEXCEPT {
        static ENTT_MAYBE_ATOMIC(id_type) value{};
        return value++;
    }
};


template<typename Type>
[[nodiscard]] constexpr auto stripped_type_name() ENTT_NOEXCEPT {
#if defined ENTT_PRETTY_FUNCTION
    std::string_view pretty_function{ENTT_PRETTY_FUNCTION};
    auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(ENTT_PRETTY_FUNCTION_PREFIX)+1);
    auto value = pretty_function.substr(first, pretty_function.find_last_of(ENTT_PRETTY_FUNCTION_SUFFIX) - first);
    return value;
#else
    return std::string_view{""};
#endif
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr std::string_view type_name(int) ENTT_NOEXCEPT {
    constexpr auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type>
[[nodiscard]] static std::string_view type_name(char) ENTT_NOEXCEPT {
    static const auto value = stripped_type_name<Type>();
    return value;
}


template<typename Type, auto = stripped_type_name<Type>().find_first_of('.')>
[[nodiscard]] static constexpr id_type type_hash(int) ENTT_NOEXCEPT {
    constexpr auto stripped = stripped_type_name<Type>();
    constexpr auto value = hashed_string::value(stripped.data(), stripped.size());
    return value;
}


template<typename Type>
[[nodiscard]] static id_type type_hash(char) ENTT_NOEXCEPT {
    static const auto value = [](const auto stripped) {
        return hashed_string::value(stripped.data(), stripped.size());
    }(stripped_type_name<Type>());
    return value;
}


}


/**
 * Internal details not to be documented.
 * @endcond
 */


/**
 * @brief Type sequential identifier.
 * @tparam Type Type for which to generate a sequential identifier.
 */
template<typename Type, typename = void>
struct ENTT_API type_seq final {
    /**
     * @brief Returns the sequential identifier of a given type.
     * @return The sequential identifier of a given type.
     */
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const id_type value = internal::type_seq::next();
        return value;
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type hash.
 * @tparam Type Type for which to generate a hash value.
 */
template<typename Type, typename = void>
struct type_hash final {
    /**
     * @brief Returns the numeric representation of a given type.
     * @return The numeric representation of the given type.
     */
#if defined ENTT_PRETTY_FUNCTION
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return internal::type_hash<Type>(0);
#else
    [[nodiscard]] static constexpr id_type value() ENTT_NOEXCEPT {
        return type_seq<Type>::value();
#endif
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator id_type() const ENTT_NOEXCEPT { return value(); }
};


/**
 * @brief Type name.
 * @tparam Type Type for which to generate a name.
 */
template<typename Type, typename = void>
struct type_name final {
    /**
     * @brief Returns the name of a given type.
     * @return The name of the given type.
     */
    [[nodiscard]] static constexpr std::string_view value() ENTT_NOEXCEPT {
        return internal::type_name<Type>(0);
    }

    /*! @copydoc value */
    [[nodiscard]] constexpr operator std::string_view() const ENTT_NOEXCEPT { return value(); }
};


/*! @brief Implementation specific information about a type. */
class type_info final {
    template<typename>
    friend type_info type_id() ENTT_NOEXCEPT;

    type_info(id_type seq_v, id_type hash_v, std::string_view name_v) ENTT_NOEXCEPT
        : seq_value{seq_v},
          hash_value{hash_v},
          name_value{name_v}
    {}

public:
    /*! @brief Default constructor. */
    type_info() ENTT_NOEXCEPT
        : type_info({}, {}, {})
    {}

    /*! @brief Default copy constructor. */
    type_info(const type_info &) ENTT_NOEXCEPT = default;
    /*! @brief Default move constructor. */
    type_info(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Default copy assignment operator.
     * @return This type info object.
     */
    type_info & operator=(const type_info &) ENTT_NOEXCEPT = default;

    /**
     * @brief Default move assignment operator.
     * @return This type info object.
     */
    type_info & operator=(type_info &&) ENTT_NOEXCEPT = default;

    /**
     * @brief Checks if a type info object is properly initialized.
     * @return True if the object is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return name_value.data() != nullptr;
    }

    /**
     * @brief Type sequential identifier.
     * @return Type sequential identifier.
     */
    [[nodiscard]] id_type seq() const ENTT_NOEXCEPT {
        return seq_value;
    }

    /**
     * @brief Type hash.
     * @return Type hash.
     */
    [[nodiscard]] id_type hash() const ENTT_NOEXCEPT {
        return hash_value;
    }

    /**
     * @brief Type name.
     * @return Type name.
     */
    [[nodiscard]] std::string_view name() const ENTT_NOEXCEPT {
        return name_value;
    }

    /**
     * @brief Compares the contents of two type info objects.
     * @param other Object with which to compare.
     * @return False if the two contents differ, true otherwise.
     */
    [[nodiscard]] bool operator==(const type_info &other) const ENTT_NOEXCEPT {
        return hash_value == other.hash_value;
    }

private:
    id_type seq_value;
    id_type hash_value;
    std::string_view name_value;
};


/**
 * @brief Compares the contents of two type info objects.
 * @param lhs A type info object.
 * @param rhs A type info object.
 * @return True if the two contents differ, false otherwise.
 */
[[nodiscard]] inline bool operator!=(const type_info &lhs, const type_info &rhs) ENTT_NOEXCEPT {
    return !(lhs == rhs);
}


/**
 * @brief Returns the type info object for a given type.
 * @tparam Type Type for which to generate a type info object.
 * @return The type info object for the given type.
 */
template<typename Type>
[[nodiscard]] type_info type_id() ENTT_NOEXCEPT {
    return type_info{
        type_seq<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_hash<std::remove_cv_t<std::remove_reference_t<Type>>>::value(),
        type_name<std::remove_cv_t<std::remove_reference_t<Type>>>::value()
    };
}


}


#endif

// #include "sigh.hpp"
#ifndef ENTT_SIGNAL_SIGH_HPP
#define ENTT_SIGNAL_SIGH_HPP


#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>
#include <type_traits>
// #include "../config/config.h"

// #include "delegate.hpp"

// #include "fwd.hpp"
#ifndef ENTT_SIGNAL_FWD_HPP
#define ENTT_SIGNAL_FWD_HPP


namespace entt {


template<typename>
class delegate;


class dispatcher;


template<typename>
class emitter;


class connection;


struct scoped_connection;


template<typename>
class sink;


template<typename>
class sigh;


}


#endif



namespace entt {


/**
 * @brief Sink class.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error unless the template parameter is a function type.
 *
 * @tparam Function A valid function type.
 */
template<typename Function>
class sink;


/**
 * @brief Unmanaged signal handler.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error unless the template parameter is a function type.
 *
 * @tparam Function A valid function type.
 */
template<typename Function>
class sigh;


/**
 * @brief Unmanaged signal handler.
 *
 * It works directly with references to classes and pointers to member functions
 * as well as pointers to free functions. Users of this class are in charge of
 * disconnecting instances before deleting them.
 *
 * This class serves mainly two purposes:
 *
 * * Creating signals to use later to notify a bunch of listeners.
 * * Collecting results from a set of functions like in a voting system.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
class sigh<Ret(Args...)> {
    /*! @brief A sink is allowed to modify a signal. */
    friend class sink<Ret(Args...)>;

public:
    /*! @brief Unsigned integer type. */
    using size_type = std::size_t;
    /*! @brief Sink type. */
    using sink_type = sink<Ret(Args...)>;

    /**
     * @brief Instance type when it comes to connecting member functions.
     * @tparam Class Type of class to which the member function belongs.
     */
    template<typename Class>
    using instance_type = Class *;

    /**
     * @brief Number of listeners connected to the signal.
     * @return Number of listeners currently connected.
     */
    [[nodiscard]] size_type size() const ENTT_NOEXCEPT {
        return calls.size();
    }

    /**
     * @brief Returns false if at least a listener is connected to the signal.
     * @return True if the signal has no listeners connected, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return calls.empty();
    }

    /**
     * @brief Triggers a signal.
     *
     * All the listeners are notified. Order isn't guaranteed.
     *
     * @param args Arguments to use to invoke listeners.
     */
    void publish(Args... args) const {
        for(auto &&call: std::as_const(calls)) {
            call(args...);
        }
    }

    /**
     * @brief Collects return values from the listeners.
     *
     * The collector must expose a call operator with the following properties:
     *
     * * The return type is either `void` or such that it's convertible to
     *   `bool`. In the second case, a true value will stop the iteration.
     * * The list of parameters is empty if `Ret` is `void`, otherwise it
     *   contains a single element such that `Ret` is convertible to it.
     *
     * @tparam Func Type of collector to use, if any.
     * @param func A valid function object.
     * @param args Arguments to use to invoke listeners.
     */
    template<typename Func>
    void collect(Func func, Args... args) const {
        for(auto &&call: calls) {
            if constexpr(std::is_void_v<Ret>) {
                if constexpr(std::is_invocable_r_v<bool, Func>) {
                    call(args...);
                    if(func()) { break; }
                } else {
                    call(args...);
                    func();
                }
            } else {
                if constexpr(std::is_invocable_r_v<bool, Func, Ret>) {
                    if(func(call(args...))) { break; }
                } else {
                    func(call(args...));
                }
            }
        }
    }

private:
    std::vector<delegate<Ret(Args...)>> calls;
};


/**
 * @brief Connection class.
 *
 * Opaque object the aim of which is to allow users to release an already
 * estabilished connection without having to keep a reference to the signal or
 * the sink that generated it.
 */
class connection {
    /*! @brief A sink is allowed to create connection objects. */
    template<typename>
    friend class sink;

    connection(delegate<void(void *)> fn, void *ref)
        : disconnect{fn}, signal{ref}
    {}

public:
    /*! @brief Default constructor. */
    connection() = default;

    /**
     * @brief Checks whether a connection is properly initialized.
     * @return True if the connection is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return static_cast<bool>(disconnect);
    }

    /*! @brief Breaks the connection. */
    void release() {
        if(disconnect) {
            disconnect(signal);
            disconnect.reset();
        }
    }

private:
    delegate<void(void *)> disconnect;
    void *signal{};
};


/**
 * @brief Scoped connection class.
 *
 * Opaque object the aim of which is to allow users to release an already
 * estabilished connection without having to keep a reference to the signal or
 * the sink that generated it.<br/>
 * A scoped connection automatically breaks the link between the two objects
 * when it goes out of scope.
 */
struct scoped_connection {
    /*! @brief Default constructor. */
    scoped_connection() = default;

    /**
     * @brief Constructs a scoped connection from a basic connection.
     * @param other A valid connection object.
     */
    scoped_connection(const connection &other)
        : conn{other}
    {}

    /*! @brief Default copy constructor, deleted on purpose. */
    scoped_connection(const scoped_connection &) = delete;

    /*! @brief Automatically breaks the link on destruction. */
    ~scoped_connection() {
        conn.release();
    }

    /**
     * @brief Default copy assignment operator, deleted on purpose.
     * @return This scoped connection.
     */
    scoped_connection & operator=(const scoped_connection &) = delete;

    /**
     * @brief Acquires a connection.
     * @param other The connection object to acquire.
     * @return This scoped connection.
     */
    scoped_connection & operator=(connection other) {
        conn = std::move(other);
        return *this;
    }

    /**
     * @brief Checks whether a scoped connection is properly initialized.
     * @return True if the connection is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const ENTT_NOEXCEPT {
        return static_cast<bool>(conn);
    }

    /*! @brief Breaks the connection. */
    void release() {
        conn.release();
    }

private:
    connection conn;
};


/**
 * @brief Sink class.
 *
 * A sink is used to connect listeners to signals and to disconnect them.<br/>
 * The function type for a listener is the one of the signal to which it
 * belongs.
 *
 * The clear separation between a signal and a sink permits to store the former
 * as private data member without exposing the publish functionality to the
 * users of the class.
 *
 * @warning
 * Lifetime of a sink must not overcome that of the signal to which it refers.
 * In any other case, attempting to use a sink results in undefined behavior.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
class sink<Ret(Args...)> {
    using signal_type = sigh<Ret(Args...)>;
    using difference_type = typename std::iterator_traits<typename decltype(signal_type::calls)::iterator>::difference_type;

    template<auto Candidate, typename Type>
    static void release(Type value_or_instance, void *signal) {
        sink{*static_cast<signal_type *>(signal)}.disconnect<Candidate>(value_or_instance);
    }

    template<auto Candidate>
    static void release(void *signal) {
        sink{*static_cast<signal_type *>(signal)}.disconnect<Candidate>();
    }

public:
    /**
     * @brief Constructs a sink that is allowed to modify a given signal.
     * @param ref A valid reference to a signal object.
     */
    sink(sigh<Ret(Args...)> &ref) ENTT_NOEXCEPT
        : offset{},
          signal{&ref}
    {}

    /**
     * @brief Returns false if at least a listener is connected to the sink.
     * @return True if the sink has no listeners connected, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return signal->calls.empty();
    }

    /**
     * @brief Returns a sink that connects before a given free function or an
     * unbound member.
     * @tparam Function A valid free function pointer.
     * @return A properly initialized sink object.
     */
    template<auto Function>
    [[nodiscard]] sink before() {
        delegate<Ret(Args...)> call{};
        call.template connect<Function>();

        const auto &calls = signal->calls;
        const auto it = std::find(calls.cbegin(), calls.cend(), std::move(call));

        sink other{*this};
        other.offset = std::distance(it, calls.cend());
        return other;
    }

    /**
     * @brief Returns a sink that connects before a free function with payload
     * or a bound member.
     * @tparam Candidate Member or free function to look for.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     * @return A properly initialized sink object.
     */
    template<auto Candidate, typename Type>
    [[nodiscard]] sink before(Type &&value_or_instance) {
        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>(value_or_instance);

        const auto &calls = signal->calls;
        const auto it = std::find(calls.cbegin(), calls.cend(), std::move(call));

        sink other{*this};
        other.offset = std::distance(it, calls.cend());
        return other;
    }

    /**
     * @brief Returns a sink that connects before a given instance or specific
     * payload.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     * @return A properly initialized sink object.
     */
    template<typename Type>
    [[nodiscard]] sink before(Type &value_or_instance) {
        return before(&value_or_instance);
    }

    /**
     * @brief Returns a sink that connects before a given instance or specific
     * payload.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid pointer that fits the purpose.
     * @return A properly initialized sink object.
     */
    template<typename Type>
    [[nodiscard]] sink before(Type *value_or_instance) {
        sink other{*this};

        if(value_or_instance) {
            const auto &calls = signal->calls;
            const auto it = std::find_if(calls.cbegin(), calls.cend(), [value_or_instance](const auto &delegate) {
                return delegate.instance() == value_or_instance;
            });

            other.offset = std::distance(it, calls.cend());
        }

        return other;
    }

    /**
     * @brief Returns a sink that connects before anything else.
     * @return A properly initialized sink object.
     */
    [[nodiscard]] sink before() {
        sink other{*this};
        other.offset = signal->calls.size();
        return other;
    }

    /**
     * @brief Connects a free function or an unbound member to a signal.
     *
     * The signal handler performs checks to avoid multiple connections for the
     * same function.
     *
     * @tparam Candidate Function or member to connect to the signal.
     * @return A properly initialized connection object.
     */
    template<auto Candidate>
    connection connect() {
        disconnect<Candidate>();

        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>();
        signal->calls.insert(signal->calls.end() - offset, std::move(call));

        delegate<void(void *)> conn{};
        conn.template connect<&release<Candidate>>();
        return { std::move(conn), signal };
    }

    /**
     * @brief Connects a free function with payload or a bound member to a
     * signal.
     *
     * The signal isn't responsible for the connected object or the payload.
     * Users must always guarantee that the lifetime of the instance overcomes
     * the one of the signal. On the other side, the signal handler performs
     * checks to avoid multiple connections for the same function.<br/>
     * When used to connect a free function with payload, its signature must be
     * such that the instance is the first argument before the ones used to
     * define the signal itself.
     *
     * @tparam Candidate Function or member to connect to the signal.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     * @return A properly initialized connection object.
     */
    template<auto Candidate, typename Type>
    connection connect(Type &&value_or_instance) {
        disconnect<Candidate>(value_or_instance);

        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>(value_or_instance);
        signal->calls.insert(signal->calls.end() - offset, std::move(call));

        delegate<void(void *)> conn{};
        conn.template connect<&release<Candidate, Type>>(value_or_instance);
        return { std::move(conn), signal };
    }

    /**
     * @brief Disconnects a free function or an unbound member from a signal.
     * @tparam Candidate Function or member to disconnect from the signal.
     */
    template<auto Candidate>
    void disconnect() {
        auto &calls = signal->calls;
        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>();
        calls.erase(std::remove(calls.begin(), calls.end(), std::move(call)), calls.end());
    }

    /**
     * @brief Disconnects a free function with payload or a bound member from a
     * signal.
     * @tparam Candidate Function or member to disconnect from the signal.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<auto Candidate, typename Type>
    void disconnect(Type &&value_or_instance) {
        auto &calls = signal->calls;
        delegate<Ret(Args...)> call{};
        call.template connect<Candidate>(value_or_instance);
        calls.erase(std::remove(calls.begin(), calls.end(), std::move(call)), calls.end());
    }

    /**
     * @brief Disconnects free functions with payload or bound members from a
     * signal.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<typename Type>
    void disconnect(Type &value_or_instance) {
        disconnect(&value_or_instance);
    }

    /**
     * @brief Disconnects free functions with payload or bound members from a
     * signal.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<typename Type>
    void disconnect(Type *value_or_instance) {
        if(value_or_instance) {
            auto &calls = signal->calls;
            calls.erase(std::remove_if(calls.begin(), calls.end(), [value_or_instance](const auto &delegate) {
                return delegate.instance() == value_or_instance;
            }), calls.end());
        }
    }

    /*! @brief Disconnects all the listeners from a signal. */
    void disconnect() {
        signal->calls.clear();
    }

private:
    difference_type offset;
    signal_type *signal;
};


/**
 * @brief Deduction guide.
 *
 * It allows to deduce the function type of a sink directly from the signal it
 * refers to.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template<typename Ret, typename... Args>
sink(sigh<Ret(Args...)> &)
-> sink<Ret(Args...)>;


}


#endif



namespace entt {


/**
 * @brief Basic dispatcher implementation.
 *
 * A dispatcher can be used either to trigger an immediate event or to enqueue
 * events to be published all together once per tick.<br/>
 * Listeners are provided in the form of member functions. For each event of
 * type `Event`, listeners are such that they can be invoked with an argument of
 * type `Event &`, no matter what the return type is.
 *
 * The dispatcher creates instances of the `sigh` class internally. Refer to the
 * documentation of the latter for more details.
 */
class dispatcher {
    struct basic_pool {
        virtual ~basic_pool() = default;
        virtual void publish() = 0;
        virtual void disconnect(void *) = 0;
        virtual void clear() ENTT_NOEXCEPT = 0;
    };

    template<typename Event>
    struct pool_handler final: basic_pool {
        static_assert(std::is_same_v<Event, std::decay_t<Event>>, "Invalid event type");

        using signal_type = sigh<void(Event &)>;
        using sink_type = typename signal_type::sink_type;

        void publish() override {
            const auto length = events.size();

            for(std::size_t pos{}; pos < length; ++pos) {
                signal.publish(events[pos]);
            }

            events.erase(events.cbegin(), events.cbegin()+length);
        }

        void disconnect(void *instance) override {
            sink().disconnect(instance);
        }

        void clear() ENTT_NOEXCEPT override {
            events.clear();
        }

        [[nodiscard]] sink_type sink() ENTT_NOEXCEPT {
            return entt::sink{signal};
        }

        template<typename... Args>
        void trigger(Args &&... args) {
            Event instance{std::forward<Args>(args)...};
            signal.publish(instance);
        }

        template<typename... Args>
        void enqueue(Args &&... args) {
            if constexpr(std::is_aggregate_v<Event>) {
                events.push_back(Event{std::forward<Args>(args)...});
            } else {
                events.emplace_back(std::forward<Args>(args)...);
            }
        }

    private:
        signal_type signal{};
        std::vector<Event> events;
    };

    template<typename Event>
    [[nodiscard]] pool_handler<Event> & assure() {
        const auto index = type_seq<Event>::value();

        if(!(index < pools.size())) {
            pools.resize(std::size_t(index)+1u);
        }

        if(!pools[index]) {
            pools[index].reset(new pool_handler<Event>{});
        }

        return static_cast<pool_handler<Event> &>(*pools[index]);
    }

public:
    /*! @brief Default constructor. */
    dispatcher() = default;

    /*! @brief Default move constructor. */
    dispatcher(dispatcher &&) = default;

    /*! @brief Default move assignment operator. @return This dispatcher. */
    dispatcher & operator=(dispatcher &&) = default;

    /**
     * @brief Returns a sink object for the given event.
     *
     * A sink is an opaque object used to connect listeners to events.
     *
     * The function type for a listener is _compatible_ with:
     * @code{.cpp}
     * void(Event &);
     * @endcode
     *
     * The order of invocation of the listeners isn't guaranteed.
     *
     * @sa sink
     *
     * @tparam Event Type of event of which to get the sink.
     * @return A temporary sink object.
     */
    template<typename Event>
    [[nodiscard]] auto sink() {
        return assure<Event>().sink();
    }

    /**
     * @brief Triggers an immediate event of the given type.
     *
     * All the listeners registered for the given type are immediately notified.
     * The event is discarded after the execution.
     *
     * @tparam Event Type of event to trigger.
     * @tparam Args Types of arguments to use to construct the event.
     * @param args Arguments to use to construct the event.
     */
    template<typename Event, typename... Args>
    void trigger(Args &&... args) {
        assure<Event>().trigger(std::forward<Args>(args)...);
    }

    /**
     * @brief Triggers an immediate event of the given type.
     *
     * All the listeners registered for the given type are immediately notified.
     * The event is discarded after the execution.
     *
     * @tparam Event Type of event to trigger.
     * @param event An instance of the given type of event.
     */
    template<typename Event>
    void trigger(Event &&event) {
        assure<std::decay_t<Event>>().trigger(std::forward<Event>(event));
    }

    /**
     * @brief Enqueues an event of the given type.
     *
     * An event of the given type is queued. No listener is invoked. Use the
     * `update` member function to notify listeners when ready.
     *
     * @tparam Event Type of event to enqueue.
     * @tparam Args Types of arguments to use to construct the event.
     * @param args Arguments to use to construct the event.
     */
    template<typename Event, typename... Args>
    void enqueue(Args &&... args) {
        assure<Event>().enqueue(std::forward<Args>(args)...);
    }

    /**
     * @brief Enqueues an event of the given type.
     *
     * An event of the given type is queued. No listener is invoked. Use the
     * `update` member function to notify listeners when ready.
     *
     * @tparam Event Type of event to enqueue.
     * @param event An instance of the given type of event.
     */
    template<typename Event>
    void enqueue(Event &&event) {
        assure<std::decay_t<Event>>().enqueue(std::forward<Event>(event));
    }

    /**
     * @brief Utility function to disconnect everything related to a given value
     * or instance from a dispatcher.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<typename Type>
    void disconnect(Type &value_or_instance) {
        disconnect(&value_or_instance);
    }

    /**
     * @brief Utility function to disconnect everything related to a given value
     * or instance from a dispatcher.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid object that fits the purpose.
     */
    template<typename Type>
    void disconnect(Type *value_or_instance) {
        for(auto &&cpool: pools) {
            if(cpool) {
                cpool->disconnect(value_or_instance);
            }
        }
    }

    /**
     * @brief Discards all the events queued so far.
     *
     * If no types are provided, the dispatcher will clear all the existing
     * pools.
     *
     * @tparam Event Type of events to discard.
     */
    template<typename... Event>
    void clear() {
        if constexpr(sizeof...(Event) == 0) {
            for(auto &&cpool: pools) {
                if(cpool) {
                    cpool->clear();
                }
            }
        } else {
            (assure<Event>().clear(), ...);
        }
    }

    /**
     * @brief Delivers all the pending events of the given type.
     *
     * This method is blocking and it doesn't return until all the events are
     * delivered to the registered listeners. It's responsibility of the users
     * to reduce at a minimum the time spent in the bodies of the listeners.
     *
     * @tparam Event Type of events to send.
     */
    template<typename Event>
    void update() {
        assure<Event>().publish();
    }

    /**
     * @brief Delivers all the pending events.
     *
     * This method is blocking and it doesn't return until all the events are
     * delivered to the registered listeners. It's responsibility of the users
     * to reduce at a minimum the time spent in the bodies of the listeners.
     */
    void update() const {
        for(auto pos = pools.size(); pos; --pos) {
            if(auto &&cpool = pools[pos-1]; cpool) {
                cpool->publish();
            }
        }
    }

private:
    std::vector<std::unique_ptr<basic_pool>> pools;
};


}


#endif

// #include "signal/emitter.hpp"
#ifndef ENTT_SIGNAL_EMITTER_HPP
#define ENTT_SIGNAL_EMITTER_HPP


#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
// #include "../config/config.h"

// #include "../core/fwd.hpp"

// #include "../core/type_info.hpp"



namespace entt {


/**
 * @brief General purpose event emitter.
 *
 * The emitter class template follows the CRTP idiom. To create a custom emitter
 * type, derived classes must inherit directly from the base class as:
 *
 * @code{.cpp}
 * struct my_emitter: emitter<my_emitter> {
 *     // ...
 * }
 * @endcode
 *
 * Pools for the type of events are created internally on the fly. It's not
 * required to specify in advance the full list of accepted types.<br/>
 * Moreover, whenever an event is published, an emitter provides the listeners
 * with a reference to itself along with a reference to the event. Therefore
 * listeners have an handy way to work with it without incurring in the need of
 * capturing a reference to the emitter.
 *
 * @tparam Derived Actual type of emitter that extends the class template.
 */
template<typename Derived>
class emitter {
    struct basic_pool {
        virtual ~basic_pool() = default;
        virtual bool empty() const ENTT_NOEXCEPT = 0;
        virtual void clear() ENTT_NOEXCEPT = 0;
    };

    template<typename Event>
    struct pool_handler final: basic_pool {
        static_assert(std::is_same_v<Event, std::decay_t<Event>>, "Invalid event type");

        using listener_type = std::function<void(Event &, Derived &)>;
        using element_type = std::pair<bool, listener_type>;
        using container_type = std::list<element_type>;
        using connection_type = typename container_type::iterator;

        [[nodiscard]] bool empty() const ENTT_NOEXCEPT override {
            auto pred = [](auto &&element) { return element.first; };

            return std::all_of(once_list.cbegin(), once_list.cend(), pred) &&
                    std::all_of(on_list.cbegin(), on_list.cend(), pred);
        }

        void clear() ENTT_NOEXCEPT override {
            if(publishing) {
                for(auto &&element: once_list) {
                    element.first = true;
                }

                for(auto &&element: on_list) {
                    element.first = true;
                }
            } else {
                once_list.clear();
                on_list.clear();
            }
        }

        connection_type once(listener_type listener) {
            return once_list.emplace(once_list.cend(), false, std::move(listener));
        }

        connection_type on(listener_type listener) {
            return on_list.emplace(on_list.cend(), false, std::move(listener));
        }

        void erase(connection_type conn) {
            conn->first = true;

            if(!publishing) {
                auto pred = [](auto &&element) { return element.first; };
                once_list.remove_if(pred);
                on_list.remove_if(pred);
            }
        }

        void publish(Event &event, Derived &ref) {
            container_type swap_list;
            once_list.swap(swap_list);

            publishing = true;

            for(auto &&element: on_list) {
                element.first ? void() : element.second(event, ref);
            }

            for(auto &&element: swap_list) {
                element.first ? void() : element.second(event, ref);
            }

            publishing = false;

            on_list.remove_if([](auto &&element) { return element.first; });
        }

    private:
        bool publishing{false};
        container_type once_list{};
        container_type on_list{};
    };

    template<typename Event>
    [[nodiscard]] pool_handler<Event> * assure() {
        const auto index = type_seq<Event>::value();

        if(!(index < pools.size())) {
            pools.resize(std::size_t(index)+1u);
        }

        if(!pools[index]) {
            pools[index].reset(new pool_handler<Event>{});
        }

        return static_cast<pool_handler<Event> *>(pools[index].get());
    }

    template<typename Event>
    [[nodiscard]] const pool_handler<Event> * assure() const {
        const auto index = type_seq<Event>::value();
        return (!(index < pools.size()) || !pools[index]) ? nullptr : static_cast<const pool_handler<Event> *>(pools[index].get());
    }

public:
    /** @brief Type of listeners accepted for the given event. */
    template<typename Event>
    using listener = typename pool_handler<Event>::listener_type;

    /**
     * @brief Generic connection type for events.
     *
     * Type of the connection object returned by the event emitter whenever a
     * listener for the given type is registered.<br/>
     * It can be used to break connections still in use.
     *
     * @tparam Event Type of event for which the connection is created.
     */
    template<typename Event>
    struct connection: private pool_handler<Event>::connection_type {
        /** @brief Event emitters are friend classes of connections. */
        friend class emitter;

        /*! @brief Default constructor. */
        connection() = default;

        /**
         * @brief Creates a connection that wraps its underlying instance.
         * @param conn A connection object to wrap.
         */
        connection(typename pool_handler<Event>::connection_type conn)
            : pool_handler<Event>::connection_type{std::move(conn)}
        {}
    };

    /*! @brief Default constructor. */
    emitter() = default;

    /*! @brief Default destructor. */
    virtual ~emitter() {
        static_assert(std::is_base_of_v<emitter<Derived>, Derived>, "Incorrect use of the class template");
    }

    /*! @brief Default move constructor. */
    emitter(emitter &&) = default;

    /*! @brief Default move assignment operator. @return This emitter. */
    emitter & operator=(emitter &&) = default;

    /**
     * @brief Emits the given event.
     *
     * All the listeners registered for the specific event type are invoked with
     * the given event. The event type must either have a proper constructor for
     * the arguments provided or be an aggregate type.
     *
     * @tparam Event Type of event to publish.
     * @tparam Args Types of arguments to use to construct the event.
     * @param args Parameters to use to initialize the event.
     */
    template<typename Event, typename... Args>
    void publish(Args &&... args) {
        Event instance{std::forward<Args>(args)...};
        assure<Event>()->publish(instance, *static_cast<Derived *>(this));
    }

    /**
     * @brief Registers a long-lived listener with the event emitter.
     *
     * This method can be used to register a listener designed to be invoked
     * more than once for the given event type.<br/>
     * The connection returned by the method can be freely discarded. It's meant
     * to be used later to disconnect the listener if required.
     *
     * The listener is as a callable object that can be moved and the type of
     * which is _compatible_ with `void(Event &, Derived &)`.
     *
     * @note
     * Whenever an event is emitted, the emitter provides the listener with a
     * reference to the derived class. Listeners don't have to capture those
     * instances for later uses.
     *
     * @tparam Event Type of event to which to connect the listener.
     * @param instance The listener to register.
     * @return Connection object that can be used to disconnect the listener.
     */
    template<typename Event>
    connection<Event> on(listener<Event> instance) {
        return assure<Event>()->on(std::move(instance));
    }

    /**
     * @brief Registers a short-lived listener with the event emitter.
     *
     * This method can be used to register a listener designed to be invoked
     * only once for the given event type.<br/>
     * The connection returned by the method can be freely discarded. It's meant
     * to be used later to disconnect the listener if required.
     *
     * The listener is as a callable object that can be moved and the type of
     * which is _compatible_ with `void(Event &, Derived &)`.
     *
     * @note
     * Whenever an event is emitted, the emitter provides the listener with a
     * reference to the derived class. Listeners don't have to capture those
     * instances for later uses.
     *
     * @tparam Event Type of event to which to connect the listener.
     * @param instance The listener to register.
     * @return Connection object that can be used to disconnect the listener.
     */
    template<typename Event>
    connection<Event> once(listener<Event> instance) {
        return assure<Event>()->once(std::move(instance));
    }

    /**
     * @brief Disconnects a listener from the event emitter.
     *
     * Do not use twice the same connection to disconnect a listener, it results
     * in undefined behavior. Once used, discard the connection object.
     *
     * @tparam Event Type of event of the connection.
     * @param conn A valid connection.
     */
    template<typename Event>
    void erase(connection<Event> conn) {
        assure<Event>()->erase(std::move(conn));
    }

    /**
     * @brief Disconnects all the listeners for the given event type.
     *
     * All the connections previously returned for the given event are
     * invalidated. Using them results in undefined behavior.
     *
     * @tparam Event Type of event to reset.
     */
    template<typename Event>
    void clear() {
        assure<Event>()->clear();
    }

    /**
     * @brief Disconnects all the listeners.
     *
     * All the connections previously returned are invalidated. Using them
     * results in undefined behavior.
     */
    void clear() ENTT_NOEXCEPT {
        for(auto &&cpool: pools) {
            if(cpool) {
                cpool->clear();
            }
        }
    }

    /**
     * @brief Checks if there are listeners registered for the specific event.
     * @tparam Event Type of event to test.
     * @return True if there are no listeners registered, false otherwise.
     */
    template<typename Event>
    [[nodiscard]] bool empty() const {
        const auto *cpool = assure<Event>();
        return !cpool || cpool->empty();
    }

    /**
     * @brief Checks if there are listeners registered with the event emitter.
     * @return True if there are no listeners registered, false otherwise.
     */
    [[nodiscard]] bool empty() const ENTT_NOEXCEPT {
        return std::all_of(pools.cbegin(), pools.cend(), [](auto &&cpool) {
            return !cpool || cpool->empty();
        });
    }

private:
    std::vector<std::unique_ptr<basic_pool>> pools{};
};


}


#endif

// #include "signal/sigh.hpp"

