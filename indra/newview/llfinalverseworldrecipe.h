// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FINALVERSE_WORLD_RECIPE_H
#define LL_FINALVERSE_WORLD_RECIPE_H
#include "llsd.h"
#include <string>

// Authoring data only. The gateway checks pins and MutSea authorizes execution.
bool llfinalverseValidateRecipe(const LLSD& recipe, std::string& error);
LLSD llfinalverseReadRecipe(const std::string& filename, std::string& error);
bool llfinalverseSaveRecipe(const LLSD& recipe, const std::string& filename, std::string& error);
#endif
