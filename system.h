#pragma once

#include <entt/core/type_traits.hpp>
#include <entt/entity/storage.hpp>
#include <entt/entity/view.hpp>

#include <tuple>
#include <type_traits>

template<typename T>
using storage_traits_type = typename entt::storage_traits<entt::entity, T>::storage_type;

template<typename... Comp>
using comp_list = entt::type_list<Comp...>;

template<typename...>
struct System;

template<typename... ConstComps, typename... MutableComps>
struct System<comp_list<ConstComps...>, comp_list<MutableComps...>>
{
    using const_comp_list = comp_list<ConstComps...>;
    using mutable_comp_list = comp_list<MutableComps...>;

    std::tuple<const storage_traits_type<ConstComps>&...> const_storages;
    std::tuple<storage_traits_type<MutableComps>&...> mutable_sorages;
};

namespace detail
{

template<typename T>
auto& get_storage(auto&& system)
{
    if constexpr (std::is_const_v<T>)
    {
        return std::get<const storage_traits_type<std::remove_const_t<T>>&>(system.const_storages);
    }
    else
    {
        return std::get<storage_traits_type<T>&>(system.mutable_sorages);
    }
}

}

template<typename T>
auto& get_storage(auto&& system)
{
    static_assert(std::negation_v<std::is_const<T>>);
    return detail::get_storage<T>(system);
}

template<typename... Ts>
auto get_view(auto&& system)
{
    static_assert(std::conjunction_v<std::is_const<Ts>...>);
    // if constexpr (std::conjunction_v<std::is_const<Ts>...>)
    // {
    //     return entt::basic_view<entt::entity, entt::get_t<const Ts...>, entt::exclude_t<>>(get_storage<Ts>(system)...);
    // }
    // else if constexpr (std::conjunction_v<std::negation<std::is_const<Ts>>...>)
    {
        // return entt::basic_view<entt::entity, entt::get_t<Ts...>, entt::exclude_t<>>(get_storage<Ts>(system)...);
        return entt::basic_view(detail::get_storage<Ts>(system)...);
    }
    // else
    // {
    //     return (... | entt::basic_view<entt::entity, entt::get_t<Ts>, entt::exclude_t<>>(get_storage<Ts>(system)));
    // }
}
