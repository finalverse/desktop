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
    const bool quick = spec.has("quick_mode") && spec["quick_mode"].isBoolean() && spec["quick_mode"].asBoolean();
    const bool staging = spec.has("staging_review") && spec["staging_review"].isBoolean() && spec["staging_review"].asBoolean();
    const bool hero = spec.has("hero_review") && spec["hero_review"].isBoolean() && spec["hero_review"].asBoolean();
    const LLUUID harbor("a9d44538-26ca-4586-a8a9-e551722ffa15");
    const bool fixture = spec["region_id"].isUUID() && (hero ?
        spec["grid_login_uri"].asString() == "http://127.0.0.1:18099/" &&
            spec["region_id"].asUUID() == LLUUID("38bfc868-034e-4a29-acd6-5ff203e96e57") : staging ?
        spec["grid_login_uri"].asString() == "http://127.0.0.1:18095/" &&
            !spec["region_id"].asUUID().isNull() && spec["region_id"].asUUID() != harbor :
        spec["grid_login_uri"].asString() == "http://127.0.0.1:18094/" && spec["region_id"].asUUID() == harbor);
    if (!spec.isMap() || !spec["schema"].isInteger() || spec["schema"].asInteger() != 1 ||
        !fixture || (hero && staging) || (spec.has("hero_review") && !spec["hero_review"].isBoolean()) ||
        (spec.has("staging_review") && !spec["staging_review"].isBoolean()) ||
        !spec["output_dir"].isString() || spec["output_dir"].asString().empty() ||
        !std::filesystem::path(spec["output_dir"].asString()).is_absolute() ||
        !number(spec["warmup_seconds"], quick ? 2. : 10., quick ? 10. : 60.) ||
        !number(spec["sample_seconds"], quick ? 3. : 10., quick ? 10. : 60.) ||
        !spec["views"].isArray() || spec["views"].size() < 1 || spec["views"].size() > ((staging || hero) ? 3 : 8))
    {
        error = "Expected schema 1, the loopback Phase 4B fixture, region UUID, absolute output path, 10–60 second windows and one to eight canonical cameras.";
        return false;
    }
    std::set<std::string> seen;
    // Staging is one bounded passive review, not a second interaction queue.
    // The launcher pins this UUID to its private account; the runner also
    // verifies the actual grid and live region before touching the camera.
    if ((staging || hero) && (quick || spec.has("interaction")))
    {
        error = "Staging review requires a normal warm start and at most three passive cameras.";
        return false;
    }
    if (spec.has("quick_mode") && !spec["quick_mode"].isBoolean())
    {
        error = "quick_mode must be an explicit boolean.";
        return false;
    }
    if (quick && (spec.has("environment") || spec.has("capture_size") ||
        (spec.has("quit_after_capture") && spec["quit_after_capture"].asBoolean())))
    {
        error = "Quick checks preserve the existing environment, window and login session.";
        return false;
    }
    if (spec.has("interaction"))
    {
        const LLSD& action = spec["interaction"];
        if (!quick || spec["views"].size() != 1 || !action.isMap() ||
            action["kind"].asString() != "bench_sit_stand" ||
            !action["seat_id"].isUUID() || action["seat_id"].asUUID().isNull() ||
            !position(action["expected_position"]))
        {
            error = "Interaction requires one quick view and a bounded, owned fixture bench.";
            return false;
        }
    }
    if (spec.has("quit_after_capture") && !spec["quit_after_capture"].isBoolean())
    {
        error = "quit_after_capture must be an explicit boolean in the isolated fixture.";
        return false;
    }
    if (spec.has("capture_size") && (!spec["capture_size"].isArray() || spec["capture_size"].size() != 2 ||
        !spec["capture_size"][0].isInteger() || !spec["capture_size"][1].isInteger() ||
        !number(spec["capture_size"][0], 640., 2560.) || !number(spec["capture_size"][1], 480., 1600.)))
    {
        error = "capture_size requires bounded integer pixel dimensions.";
        return false;
    }
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
