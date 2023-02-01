// This is a separate file to enfore the systems' header files to not even #include <registry.hpp>
#pragma once

#include "system.h"

#include <entt/entity/registry.hpp>

namespace detail
{

template<typename SystemT, typename... Comps>
auto make_system(entt::registry& r, comp_list<Comps...>&&) -> SystemT
{
  return SystemT{
    .storages = {r.storage<Comps>()...}
    };
}

}

template<typename SystemT>
auto make_system(entt::registry& r) -> SystemT
{
  return detail::make_system<SystemT>(r, typename SystemT::comps_list{});
}
