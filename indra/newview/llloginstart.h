// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_LLLOGINSTART_H
#define LL_LLLOGINSTART_H

#include "llslurl.h"

// A one-session login can bypass the login panel with no spatial SLURL. The
// inherited login request uses "last" in that case. Use the same destination
// kind when evaluating the server reply, so a valid arrival is not reported as
// a fallback. Explicit home/URL requests still require their matching reply.
inline const char* llLoginStartDestination(LLSLURL::SLURL_TYPE type)
{
    switch (type)
    {
    case LLSLURL::LOCATION: return "url";
    case LLSLURL::HOME_LOCATION: return "home";
    default: return "last";
    }
}

#endif
