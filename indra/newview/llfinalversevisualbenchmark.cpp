// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "llviewerprecompiledheaders.h"
#include "llfinalversevisualbenchmark.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <numeric>
#include <set>

namespace
{
bool number(const LLSD& value, double low, double high)
{
    return (value.isReal() || value.isInteger()) && std::isfinite(value.asReal()) &&
           value.asReal() >= low && value.asReal() <= high;
}
bool position(const LLSD& value)
{
    return value.isArray() && value.size() == 3 &&
           number(value[0], 0., 256.) && number(value[1], 0., 256.) && number(value[2], 0., 128.);
}
}

bool llfinalverseValidateVisualBenchmark(const LLSD& spec, std::string& error)
{
    const std::set<std::string> names = {"CAM_Arrival", "CAM_Harbor", "CAM_Lumi", "CAM_LumiHome",
        "CAM_Garden", "CAM_Pavilion", "CAM_Waterfront", "CAM_Sunset"};
    if (!spec.isMap() || !spec["schema"].isInteger() || spec["schema"].asInteger() != 1 ||
        spec["grid_login_uri"].asString() != "http://127.0.0.1:18094/" ||
        !spec["region_id"].isUUID() || spec["region_id"].asUUID().isNull() ||
        !spec["output_dir"].isString() || spec["output_dir"].asString().empty() ||
        !std::filesystem::path(spec["output_dir"].asString()).is_absolute() ||
        !number(spec["warmup_seconds"], 10., 60.) || !number(spec["sample_seconds"], 10., 60.) ||
        !spec["views"].isArray() || spec["views"].size() < 1 || spec["views"].size() > 8)
    {
        error = "Expected schema 1, the loopback Phase 4B fixture, region UUID, absolute output path, 10–60 second windows and one to eight canonical cameras.";
        return false;
    }
    std::set<std::string> seen;
    for (S32 index = 0; index < spec["views"].size(); ++index)
    {
        const LLSD& view = spec["views"][index];
        const auto name = view["name"].asString();
        if (!names.count(name) || !seen.insert(name).second || !position(view["position"]) ||
            !position(view["focus"]) || !number(view["fov"], .3, 1.5))
        {
            error = "Invalid/duplicate camera name, out-of-fixture coordinates or FOV.";
            return false;
        }
        const double dx = view["position"][0].asReal() - view["focus"][0].asReal();
        const double dy = view["position"][1].asReal() - view["focus"][1].asReal();
        if (dx * dx + dy * dy < .01)
        {
            error = "Camera direction must have a horizontal component.";
            return false;
        }
    }
    if (spec.has("environment") && (!spec["environment"].isMap() ||
        !spec["environment"]["sky"].isMap() || !spec["environment"]["water"].isMap()))
    {
        error = "Local environment preview requires both native sky and water maps.";
        return false;
    }
    error.clear();
    return true;
}

LLSD llfinalverseFrameSummary(const std::vector<double>& seconds)
{
    LLSD result;
    if (seconds.empty() || std::any_of(seconds.begin(), seconds.end(), [](double x) { return !std::isfinite(x) || x <= 0.; }))
    {
        result["valid"] = false;
        return result;
    }
    std::vector<double> sorted(seconds);
    std::sort(sorted.begin(), sorted.end());
    const double total = std::accumulate(sorted.begin(), sorted.end(), 0.);
    const size_t n = sorted.size();
    const double median = n % 2 ? sorted[n / 2] : (sorted[n / 2 - 1] + sorted[n / 2]) * .5;
    result["valid"] = true;
    result["frames"] = static_cast<S32>(n);
    result["sample_seconds"] = total;
    result["fps"] = n / total;
    result["mean_ms"] = total * 1000. / n;
    result["median_ms"] = median * 1000.;
    result["p95_ms"] = sorted[static_cast<size_t>(std::ceil(.95 * n)) - 1] * 1000.;
    return result;
}
