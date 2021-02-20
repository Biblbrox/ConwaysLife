#ifndef TYPELIST_HPP
#define TYPELIST_HPP

namespace ecs::types
{
    /**
    * TypeList declaration
    * @tparam Args
    */
    template<typename... Args>
    struct TypeList;

    template<typename H, typename... T>
    struct TypeList<H, T...>
    {
        using Head = H;
        using Tail = TypeList<T...>;
    };

    template<>
    struct TypeList<>
    {
    };

    /**
     * Insert Type to TypeList
     * @tparam H
     * @tparam TypeList
     */
    template<typename... Args>
    struct AddToTypeList;

    template<typename H, typename... T>
    struct AddToTypeList<H, TypeList<T...>>
    {
        using TL = TypeList<T..., H>;
    };

    template<size_t counter, typename... Args>
    struct GetFromTypeListHelper
    {
        using TL = GetFromTypeListHelper<
                counter - 1, typename TypeList<Args...>::Tail>;
    };

    template<typename... Args>
    struct GetFromTypeListHelper<0, TypeList<Args...>>
    {
        using TL = typename TypeList<Args...>::Head;
    };

    template<size_t idx, typename... Args>
    struct GetFromTypeList
    {
        using Type = typename GetFromTypeListHelper<idx, TypeList<Args...>>::TL;
    };

    template<typename TypeList>
    struct Length
    {
        static int const value = 1 + Length<typename TypeList::Tail>::value;
    };

    template<>
    struct Length<TypeList<>>
    {
        static int const value = 0;
    };

    /**
     * Variadic version of std::is_base_of
     * @tparam Type
     * @tparam TypeList
     */
    template<typename Type, typename TypeList>
    struct IsBaseOfRec
    {
        static const bool value =
                std::is_base_of_v<Type, typename TypeList::Head>
                && IsBaseOfRec<Type, typename TypeList::Tail>::value;
    };

    template<typename Type>
    struct IsBaseOfRec<Type, TypeList<>>
    {
        static const bool value = true;
    };

    /**
     * Apply unary functor to each element in TypeList (TL)
     * Also apply binary functor to each pair of elements
     * from right to left which is result of unary functor.
     * Both unary and binary functor must return some value
     * They can't be void.
     * Final return value of this function is result of binary
     * function.
     * @tparam TL
     * @tparam UnFunctor
     * @tparam BinFunctor
     * @param unfunc
     * @param binfunc
     * @return
     */
    template<class TL, class UnFunctor, class BinFunctor>
    constexpr auto typeListReduce(UnFunctor &&unfunc, BinFunctor &&binfunc)
    {
        using ArgType = typename TL::Head;

        static_assert(Length<TL>::value >= 2,
                      "Length<TypeList<Args...>>::value >= 2");
        static_assert(std::is_invocable_v<UnFunctor, ArgType>,
                      "Unary functor must be invokable");
        static_assert(std::is_invocable_v<BinFunctor,
                              decltype(std::invoke_result_t<UnFunctor, ArgType>()),
                              decltype(std::invoke_result_t<UnFunctor, ArgType>())>,
                      "Binary functor must be invokable");
        static_assert(!std::is_same_v<
                              decltype(std::invoke_result_t<UnFunctor, ArgType>()), void>,
                      "Return type of unary functor can't be void");
        static_assert(!std::is_same_v<
                              decltype(std::invoke_result_t<BinFunctor,
                                      decltype(std::invoke_result_t<UnFunctor, ArgType>()),
                                      decltype(std::invoke_result_t<UnFunctor, ArgType>())>()), void>,
                      "Return type of binary functor can't be void");

        ArgType val;
        auto res = unfunc(val);

        if constexpr (Length<TL>::value == 2) { // Base case
            typename TL::Tail::Head tmp;
            return binfunc(res, unfunc(tmp));
        } else { // Recursion
            return binfunc(res,
                           typeListReduce<typename TL::Tail>(unfunc, binfunc));
        }
    }


    inline int type_id_seq = 0;
    template< typename T > inline const int type_id = type_id_seq++;
//    template<typename T>
//    constexpr size_t type_id() noexcept
//    {
//        return typeid(T).hash_code();
//    }
};

#endif //TYPELIST_HPP
