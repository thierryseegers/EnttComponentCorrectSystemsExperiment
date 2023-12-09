// This is a separate file to enforce the systems' header files to not even #include <registry.hpp>
#pragma once

#include "system.h"

#include <entt/entity/observer.hpp>
#include <entt/entity/registry.hpp>

#include <memory>

namespace systems
{

namespace detail
{

template<>
constexpr auto make_collector_group(auto&& collector)
{
    return collector;
}

template<typename Comp, typename... Rest>
constexpr auto make_collector_group(auto&& collector = entt::collector)
{
    return make_collector_group<Rest...>(collector.template group<Comp>());
}

template<>
constexpr auto make_collector_update(auto&& collector)
{
    return collector;
}

template<typename Comp, typename... Rest>
constexpr auto make_collector_update(auto&& collector = entt::collector)
{
    return make_collector_update<Rest...>(collector.template update<Comp>());
}

template<typename... Comps>
constexpr auto make_collector(entt::type_list<Comps...>&&)
{
    return make_collector_update<std::remove_const_t<Comps>...>(make_collector_group<std::remove_const_t<Comps>...>(entt::collector));
}

template<typename SystemDescription, typename... Comps>
auto make_system(entt::registry& r, entt::type_list<Comps...>&&) -> SystemDescription
{
  return SystemDescription{
      .storages = {r.storage<Comps>()...},
      // .observer = make_observer<Comps...>(r)//std::make_unique<typename SystemDescription::observer_t>(r)};
  };
}

}

template<typename SystemDescription>
auto make_system(entt::registry &r) -> system
{
  return {detail::make_system<SystemDescription>(r, typename SystemDescription::all_comps_list{}),
          std::make_unique<entt::observer>(r, detail::make_collector(typename SystemDescription::triggered_by_type_list{}))};
}

}

// namespace detail
// {
//   template<typename...>
//   struct make_system;

//   template <typename SystemDescription, typename... Comps>
//   struct make_system<SystemDescription, comp_list<Comps...>>
//   {
//     inline static SystemDescription operator()(entt::registry& r)
//     {
//       return SystemDescription{.observers = std::make_unique<typename SystemDescription::Observers>(r), .storages = {r.storage<Comps>()...}}
//     }
//   };
// }

// template<typename SystemDescription>
// SystemDescription make_system(entt::registry& r)
// {
//   return detail::make_system<SystemDescription, typename SystemDescription::comps_list>::create(r);
// }
