#include "C1.h"
#include "C2.h"
#include "make_system.h"
#include "S1.h"

#include <entt/entt.hpp>

#include <functional>
#include <iostream>
#include <string_view>
#include <utility>

void some_callback1(entt::registry &r)
{
  std::cout << ">>> a C1 was constructed or updated" << std::endl;
}

void some_callback2(entt::registry &r)
{
  std::cout << ">>> a C2 was constructed or updated" << std::endl;
}

int main() {
  entt::registry r;

  const auto e = r.create();

  r.on_construct<C1>().connect<some_callback1>();
  r.on_update<C1>().connect<some_callback1>();

  r.on_construct<C2>().connect<some_callback2>();
  r.on_update<C2>().connect<some_callback2>();

  r.emplace<C1>(e, 0.0f);
  // r.emplace<C2>(e);
  r.emplace<C3>(e, 22.0f);

  auto s1 = make_system<S1>(r);

  int i{0};

  while(i<2)
  {
    std::cout << "-- iteration " << i++ << "--" << std::endl;
    update(s1);
  }

  for(auto [e, c1, c2] : r.view<C1, C2>().each())
  {
    std::cout << "e " << entt::to_integral(e) << " c1 " << c1.value << " c2 " << c2 << std::endl;
  }
}
