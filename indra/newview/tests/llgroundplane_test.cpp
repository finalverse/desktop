// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "../llgroundplane.h"
#include <array>
#include <cstdlib>
#include <iostream>
#include <limits>

void require(bool ok, const char* reason)
{
    if (!ok) { std::cerr << reason << '\n'; std::exit(1); }
}

int main()
{
    const std::array<float, 4> no_contact{0, 0, 0, 1}, cleared{0, 0, 0, 0};
    const std::array<float, 4> floor{0, 0, 1, 25}, slope{.6f, 0, .8f, 20};
    require(!llFootPlaneHasSurface(no_contact.data()), "UnitW must fall back to terrain");
    require(!llFootPlaneHasSurface(cleared.data()), "Cleared plane must fall back to terrain");
    require(llFootPlaneHasSurface(floor.data()), "A horizontal contact must remain usable");
    require(llFootPlaneHasSurface(slope.data()), "A sloped contact must remain usable");
    for (int i = 0; i != 4; ++i)
    {
        for (float invalid : {std::numeric_limits<float>::infinity(),
                              std::numeric_limits<float>::quiet_NaN()})
        {
            auto bad = floor; bad[i] = invalid;
            require(!llFootPlaneHasSurface(bad.data()), "Non-finite contact must be ignored");
        }
    }
    // Reproduce the inherited step-height comparison with terrain at 25 m.
    // UnitW used to win at distance zero and project an ankle at 25 m to 26 m.
    auto height = [](const std::array<float, 4>& plane) {
        float land = 25.f, start = 26.f, length = 2.f;
        if (llFootPlaneHasSurface(plane.data()))
        {
            float distance = (start * plane[2] - plane[3] + .05f) / length;
            distance = std::fmax(0.f, std::fmin(1.f, distance));
            if (distance < (start - land) / length) return start - distance * length;
        }
        return land;
    };
    require(height(no_contact) == 25.f, "No contact must not lift feet above terrain");
    const std::array<float, 4> raised_platform{0, 0, 1, 25.5f};
    require(std::fabs(height(raised_platform) - 25.45f) < 1e-4f,
            "Real platform contact must retain inherited Havok tolerance");
    std::cout << "Ground-plane regressions passed\n";
}
