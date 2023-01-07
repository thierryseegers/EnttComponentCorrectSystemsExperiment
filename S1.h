#pragma once

#include "C1.h"
#include "C2.h"
#include "C3.h"
#include "system.h"

#include <iostream>

using S1 = System<comp_list<C1, C3>, comp_list<C2>>;

void update(S1 &system)
{
    std::cout << ">>> update(S1)" << std::endl;
    auto view_C1 = get_view<const C1>(system);

    for (const auto [e, c1] : view_C1.each())
    {
        // c1.value = 100.0f; // As C1 is in read only, it is not possible to be modified!
    }

    for (const auto e : view_C1)
    {
        // auto view_C2 = get_view<C2>(system);
        auto& storage_C2{get_storage<C2>(system)};

        if (storage_C2.contains(e))
        {
            storage_C2.patch(e, [](auto& c2)
                {
                    std::cout << ">>> before patching C2= " << c2 << std::endl;
                    c2 += 1.0f;
                });
            std::cout << ">>> after patching C2=" << storage_C2.get(e) << std::endl;
        }
        else
        {
            std::cout << ">>> emplace a new C2 " << std::endl;
            storage_C2.emplace(e, 10.0f);
        }
    }

    // auto view_C1_C2 = get_view<const C1, C2>(system);
    // for (auto&& [e, c1, c2] : view_C1_C2.each())
    // {
    //     std::cout << ">>> C1=" << c1.value << " C2=" << c2 << std::endl;
    // }

    // for (auto&& [e, c1, c2, c3] : get_view<const C1, C2, const C3>(system).each())
    // {
    //     // c3 = 22.f;
    //     // c2 = 44.f;
    // }
}
