#pragma once

#include <entt/core/type_traits.hpp>
#include <entt/entity/storage.hpp>
#include <entt/entity/view.hpp>

#include <tuple>
#include <type_traits>

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

template<typename... Comp>
using comp_list = entt::type_list<Comp...>;

template<typename...>
struct System;

template<typename... ConstComps, typename... MutableComps>
struct System<comp_list<ConstComps...>, comp_list<MutableComps...>>
{
    using const_comps_list = comp_list<std::add_const_t<ConstComps>...>;
    using mutable_comps_list = comp_list<MutableComps...>;
    using comps_list = comp_list<std::add_const_t<ConstComps>..., MutableComps...>;

    typename storages<std::add_const_t<ConstComps>..., MutableComps...>::type storages;
};

namespace detail
{

template<typename Comp>
auto& get_storage(auto&& system)
{
    return std::get<storage_type_t<Comp>>(system.storages);
}

}

// We disallow getting a storage to const components, forcing the use of a view.
template<typename T>
auto& get_storage(auto&& system)
{
    static_assert(std::negation_v<std::is_const<T>>);
    return detail::get_storage<T>(system);
}

// We disallow getting a view to mutable components, forcing the use of a storage
// and encouraging modifying exclusively through `patch`, `emplace`, etc.
template<typename... Ts>
auto get_view(auto&& system)
{
    static_assert(std::conjunction_v<std::is_const<Ts>...>);
    return entt::basic_view(detail::get_storage<Ts>(system)...);
}
