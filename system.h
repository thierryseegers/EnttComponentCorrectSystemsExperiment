#pragma once

#include <entt/core/type_traits.hpp>
#include <entt/entity/storage.hpp>
#include <entt/entity/view.hpp>

#include <functional>
#include <tuple>
#include <type_traits>

template<template<typename> class F, typename... Ts>
struct filtered_type_list
{
  using type = entt::type_list_cat_t<std::conditional_t<F<Ts>::value,
                                                        entt::type_list<>,
                                                        entt::type_list<Ts>>...>;
};

template<template<typename> class F, typename... Ts>
struct filtered_type_list<F, entt::type_list<Ts>...>
{
    using type = typename filtered_type_list<F, Ts...>::type;
};

template<typename T>
using is_mutable = std::negation<std::is_const<T>>;

template<typename T>
using storage_traits_type = typename entt::storage_traits<entt::entity, T>::storage_type;

// `const T` => `const storage_traits_type<T>&`
// `T`       =>       `storage_traits_type<T>&`
template<typename Comp>
using storage_type_t = typename std::conditional_t<
                           std::is_const<Comp>::value,
                           const storage_traits_type<std::remove_const_t<Comp>>&,
                           storage_traits_type<Comp>&
                       >;

// A `std::tuple` of `storage_type_t`s.
template<typename... Comp>
struct storages
{
    using type = decltype(
                     std::tuple_cat(
                         std::declval<
                             std::tuple<storage_type_t<Comp>>
                         >()...
                     )
                 );
};

// namespace system
// {

template<typename... Comps>
struct System
{
    using comps_list = entt::type_list<Comps...>;
    using const_comps_list = filtered_type_list<std::is_const, Comps...>;
    using mutable_comps_list = filtered_type_list<is_mutable, Comps...>;

    typename storages<Comps...>::type storages;
};

// struct system
// {
//     virtual void update() const = 0;
// };

// }

namespace detail
{

template<typename Comp>
auto& get_storage(auto&& system)
{
    return std::get<storage_type_t<Comp>>(system.storages);
}

}

// We disallow getting a storage to const components, forcing the use of a view.
template<typename... Comps>
auto get_storages(auto&& system)
{
    static_assert(std::conjunction_v<std::negation<std::is_const<Comps>>...>);
    return std::make_tuple(std::reference_wrapper(detail::get_storage<Comps>(system))...);
}

// We disallow getting a view to mutable components, forcing the use of a storage
// and encouraging modifying exclusively through `patch`, `emplace`, etc.
template<typename... Comps>
auto get_view(auto&& system)
{
    static_assert(std::conjunction_v<std::is_const<Comps>...>);
    return entt::basic_view{detail::get_storage<Comps>(system)...};
}
