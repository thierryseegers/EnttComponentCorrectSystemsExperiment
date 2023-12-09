#include "C1.h"
#include "C2.h"
#include "make_system.h"
#include "S1.h"

#include <entt/entt.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

void some_callback1(entt::registry const&)
{
  std::cout << ">>> a C1 was constructed or updated" << std::endl;
}

void some_callback2(entt::registry const &)
{
  std::cout << ">>> a C2 was constructed or updated" << std::endl;
}

#define SET_ENTT_META_NAME_PROP(type) entt::meta<type>().prop("name"_hs, std::string(#type))

int main() {
  using entt::literals::operator""_hs;
  using namespace std::string_literals;

  SET_ENTT_META_NAME_PROP(systems::s1);
  SET_ENTT_META_NAME_PROP(C1);
  SET_ENTT_META_NAME_PROP(C2);
  SET_ENTT_META_NAME_PROP(C3);
  // entt::meta<systems::s1>().prop("name"_hs, "systems::s1"s);
  // entt::meta<C1>().prop("name"_hs, "C1"s);
  // entt::meta<C2>().prop("name"_hs, "C2"s);
  // entt::meta<C3>().prop("name"_hs, "C3"s);

  entt::registry r;

  const auto e = r.create();

  r.on_construct<C1>().connect<some_callback1>();
  r.on_update<C1>().connect<some_callback1>();

  r.on_construct<C2>().connect<some_callback2>();
  r.on_update<C2>().connect<some_callback2>();

  systems::system s1 = systems::make_system<systems::s1>(r);

  r.emplace<C1>(e, 0.0f);
  // r.emplace<C2>(e);
  r.emplace<C3>(e, 22.0f);


  int i{0};

  while(i<2)
  {
    std::cout << "-- iteration " << i++ << "--" << std::endl;
    // update(s1);
    s1.update();
    r.replace<C1>(e, 10);
  }

  for(auto [e, c1, c2] : r.view<C1, C2>().each())
  {
    std::cout << "e " << entt::to_integral(e) << " c1 " << c1.value << " c2 " << c2 << std::endl;
  }
}
