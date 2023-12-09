#pragma once

#include <entt/core/type_traits.hpp>
#include <entt/entity/observer.hpp>
#include <entt/entity/storage.hpp>
#include <entt/entity/view.hpp>
#include <entt/meta/resolve.hpp>

#include <memory>
#include <span>
#include <string>
#include <tuple>
#include <type_traits>

#include <iostream>

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
template<typename... Comps>
struct storages
{
    using type = decltype(
                     std::tuple_cat(
                         std::declval<
                             std::tuple<storage_type_t<Comps>>
                         >()...
                     )
                 );
};

template<typename... Comps>
struct storages<entt::type_list<Comps...>>
{
    using type = typename storages<Comps...>::type;
};

template<typename... Ts>
using storages_t = typename storages<Ts...>::type;

namespace systems
{

// The components whose modifications should trigger a system's update.
template<typename... Comps>
struct triggered_by
{
  using type_list = entt::type_list<std::add_const_t<Comps>...>;
};

// The components updated during a system's update.
template<typename... Comps>
struct updates
{
  using type_list = entt::type_list<Comps...>;
};

template<typename SystemDescription>
void update(SystemDescription&, const std::span<const entt::entity>& triggering_entities);

template<typename...>
struct description;

template<typename... TriggeredByComps, typename... UpdatesComps>
struct description<triggered_by<TriggeredByComps...>, updates<UpdatesComps...>>
{
    using triggered_by_type_list = typename triggered_by<std::add_const_t<TriggeredByComps>...>::type_list;
    using updates_type_list = typename updates<UpdatesComps...>::type_list;
    using all_comps_list = typename entt::type_list_cat_t<triggered_by_type_list, updates_type_list>;

    storages_t<all_comps_list> storages;

//   std::vector<entt::type_info> read_types_info;
//   std::vector<entt::type_info> write_types_info;

};

class system
{
private:
  struct concept_
  {
      virtual ~concept_() {}
      virtual void update() = 0;
    //   virtual const std::vector<entt::type_info>& read_types_info() const = 0;
    //   virtual const std::vector<entt::type_info>& write_types_info() const = 0;
  };

  template<typename SystemDescription>
  struct model : public concept_
  {
    explicit model(SystemDescription&& description, std::unique_ptr<entt::observer>&& observer)
      : description{std::move(description)}
      , observer{std::move(observer)}
    {}

    virtual void update() override
    {
        if(!observer->empty())
        {
            systems::update(description, std::span<const entt::entity>{observer->data(), observer->size()});
            observer->clear();
        }
    }
    
    // const std::vector<entt::type_info>& read_types_info() const override
    // {
    //   return system_info.read_types_info;
    // }
    
    // const std::vector<entt::type_info>& write_types_info() const override
    // {
    //   return system_info.write_types_info;
    // }

    SystemDescription description;
    std::unique_ptr<entt::observer> observer;
  };

public:
  template<typename SystemDescription>
  system(SystemDescription&& description, std::unique_ptr<entt::observer>&& observer)
//   : type_info{entt::type_id<SystemInfoT>()}, p_impl{std::make_unique<System_model<SystemInfoT>>(std::move(system_info))}
    : impl{std::make_unique<model<SystemDescription>>(std::move(description), std::move(observer))}
  {}

  void update()
  {
    impl->update();
  }

//   const std::vector<entt::type_info>& read_type_info() const
//   {
//     return p_impl->read_types_info();
//   }
  
//   const std::vector<entt::type_info>& write_type_info() const
//   {
//     return p_impl->write_types_info();
//   }

//   entt::type_info type_info;

private:
  std::unique_ptr<concept_> impl;
};

namespace detail
{

template<typename Comp>
constexpr auto& get_storage(auto&& system)
{
    return std::get<storage_type_t<Comp>>(system.storages);
}

}

// We disallow getting a storage to const components, forcing the use of a view.
template<typename T>
constexpr auto& get_storage(auto&& system)
{
    static_assert(std::negation_v<std::is_const<T>>);
    return detail::get_storage<T>(system);
}

// We disallow getting a view to mutable components, forcing the use of a storage
// and encouraging modifying exclusively through `patch`, `emplace`, etc.
template<typename... Ts>
constexpr auto get_view(auto&& system)
{
    using entt::literals::operator""_hs;

    static_assert(std::conjunction_v<std::is_const<Ts>...>);

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::cout << entt::resolve<decltype(system)>().prop("name"_hs).value().template cast<std::string>() << std::endl;
    ((std::cout << entt::resolve<Ts>().prop("name"_hs).value().template cast<std::string>() << ", "), ...) << std::endl;

    return entt::basic_view{detail::get_storage<Ts>(system)...};
}

}
