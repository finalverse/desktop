// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_GROUND_PLANE_H
#define LL_GROUND_PLANE_H

#include <cmath>

// A contact plane needs a finite, nonzero normal. OpenSim/MutSea's UnitW
// (0, 0, 0, 1) means no contact, not a surface one metre above the feet.
inline bool llFootPlaneHasSurface(const float* plane)
{
    return std::isfinite(plane[0]) && std::isfinite(plane[1]) &&
           std::isfinite(plane[2]) && std::isfinite(plane[3]) &&
           (plane[0] != 0.f || plane[1] != 0.f || plane[2] != 0.f);
}

#endif
