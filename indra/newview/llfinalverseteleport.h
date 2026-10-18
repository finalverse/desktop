// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FINALVERSE_TELEPORT_H
#define LL_FINALVERSE_TELEPORT_H

// MutSea can close the source circuit before the destination movement-complete
// packet arrives. Preserve the source view only during this bounded handoff.
// Genuine current-region shutdowns and system-grid behavior remain immediate.
inline bool llfinalverseDeferSourceDisable(bool system_grid, bool world_capability,
    bool current_source, bool moving, double elapsed_seconds)
{
    return !system_grid && world_capability && current_source && moving &&
        elapsed_seconds >= 0. && elapsed_seconds < 15.;
}
#endif
