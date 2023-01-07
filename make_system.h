// This is a separate file to enfore the systems' header files to not even #include <registry.hpp>
#pragma once

#include "system.h"

#include <entt/entity/registry.hpp>

namespace detail
{

template<typename SystemT, typename... ConstComps, typename... MutableComps>
auto make_system(entt::registry& r, comp_list<ConstComps...>&&, comp_list<MutableComps...>&&) -> SystemT
{
  return SystemT{
    .const_storages = {r.storage<ConstComps>()...},
    .mutable_sorages = {r.storage<MutableComps>()...}
    };
}

}

template<typename SystemT>
auto make_system(entt::registry& r) -> SystemT
{
  return detail::make_system<SystemT>(r, typename SystemT::const_comp_list{}, typename SystemT::mutable_comp_list{});
}
