// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#ifndef LL_FINALVERSE_VISUAL_BENCHMARK_H
#define LL_FINALVERSE_VISUAL_BENCHMARK_H
#include "llsd.h"
#include <string>
#include <vector>

// Local developer fixture only. Optional owned-bench interaction uses inherited
// walking/sit/stand; no content writes or AI commands.
bool llfinalverseValidateVisualBenchmark(const LLSD& spec, std::string& error);
LLSD llfinalverseFrameSummary(const std::vector<double>& seconds);
void llfinalverseVisualBenchmarkIdle(double frame_seconds, bool quitting);
#endif
