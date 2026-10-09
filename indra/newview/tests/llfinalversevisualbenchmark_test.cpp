// Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
#include "../llviewerprecompiledheaders.h"
#include "../llfinalversevisualbenchmark.h"
#include "../test/lltut.h"
#include <limits>
namespace tut
{
struct visual_benchmark_data
{
    LLSD spec;
    visual_benchmark_data()
    {
        spec["schema"] = 1;
        spec["grid_login_uri"] = "http://127.0.0.1:18094/";
        spec["region_id"] = LLUUID("a9d44538-26ca-4586-a8a9-e551722ffa15");
        spec["output_dir"] = "/tmp/new-finalverse-benchmark";
        spec["warmup_seconds"] = 15.; spec["sample_seconds"] = 15.;
        for (const char* name : {"CAM_Arrival", "CAM_Harbor", "CAM_Lumi", "CAM_LumiHome",
            "CAM_Garden", "CAM_Pavilion", "CAM_Waterfront", "CAM_Sunset"})
        {
            LLSD view;
            view["name"] = name; view["fov"] = 1.;
            for (double x : {128., 120., 30.}) view["position"].append(x);
            for (double x : {128., 128., 25.}) view["focus"].append(x);
            spec["views"].append(view);
        }
    }
};
typedef test_group<visual_benchmark_data> group_t;
typedef group_t::object object_t;
group_t visual_benchmark_group("finalverse_visual_benchmark");
template<> template<> void object_t::test<1>()
{
    std::string error;
    ensure("bounded valid fixture", llfinalverseValidateVisualBenchmark(spec, error));
    spec["grid_login_uri"] = "https://grid.mutsea.com/";
    ensure("cannot run on public grid", !llfinalverseValidateVisualBenchmark(spec, error));
}
template<> template<> void object_t::test<2>()
{
    std::string error;
    spec["views"][0]["name"] = "../../private";
    ensure("camera cannot inject output path", !llfinalverseValidateVisualBenchmark(spec, error));
    spec["views"][0]["name"] = "CAM_Harbor";
    ensure("duplicate view cannot overwrite evidence", !llfinalverseValidateVisualBenchmark(spec, error));
}
template<> template<> void object_t::test<3>()
{
    std::string error;
    spec["views"][0]["position"][0] = std::numeric_limits<double>::quiet_NaN();
    ensure("NaN cannot reach camera", !llfinalverseValidateVisualBenchmark(spec, error));
    spec["views"][0]["position"][0] = 257.;
    ensure("no out-of-region camera", !llfinalverseValidateVisualBenchmark(spec, error));
    spec["views"][0]["position"] = spec["views"][0]["focus"];
    ensure("no degenerate camera", !llfinalverseValidateVisualBenchmark(spec, error));
}
template<> template<> void object_t::test<4>()
{
    std::string error;
    spec["sample_seconds"] = 0.;
    ensure("no instant misleading sample", !llfinalverseValidateVisualBenchmark(spec, error));
    spec["sample_seconds"] = 15.; spec["output_dir"] = "relative";
    ensure("explicit absolute destination", !llfinalverseValidateVisualBenchmark(spec, error));
}
template<> template<> void object_t::test<5>()
{
    const auto result = llfinalverseFrameSummary({.01, .02, .03, .04});
    ensure("valid distribution", result["valid"].asBoolean());
    ensure_equals("FPS is total frames / total time", result["fps"].asReal(), 40.);
    ensure_equals("mean ms", result["mean_ms"].asReal(), 25.);
    ensure_equals("even median ms", result["median_ms"].asReal(), 25.);
    ensure_equals("nearest-rank p95", result["p95_ms"].asReal(), 40.);
}
template<> template<> void object_t::test<6>()
{
    ensure("empty samples unavailable", !llfinalverseFrameSummary({})["valid"].asBoolean());
    ensure("invalid frames cannot inflate FPS", !llfinalverseFrameSummary({.01, 0.})["valid"].asBoolean());
    ensure("infinite frames unavailable", !llfinalverseFrameSummary({std::numeric_limits<double>::infinity()})["valid"].asBoolean());
}
template<> template<> void object_t::test<7>()
{
    std::string error;
    spec["environment"]["sky"] = "script is not a sky";
    ensure("no arbitrary environment payload", !llfinalverseValidateVisualBenchmark(spec, error));
}
template<> template<> void object_t::test<8>()
{
    std::string error;
    const LLSD first = spec["views"][0];
    spec["views"] = LLSD::emptyArray();
    ensure("empty run cannot masquerade as complete", !llfinalverseValidateVisualBenchmark(spec, error));
    spec["views"].append(first);
    ensure("canonical subset supports feature A/B without a new harness", llfinalverseValidateVisualBenchmark(spec, error));
    for (S32 i = 0; i < 8; ++i) spec["views"].append(first);
    ensure("no unbounded repeated camera run", !llfinalverseValidateVisualBenchmark(spec, error));
}
template<> template<> void object_t::test<9>()
{
    std::string error;
    spec["quit_after_capture"] = true;
    ensure("native logout is opt-in for the bounded fixture", llfinalverseValidateVisualBenchmark(spec, error));
    spec["quit_after_capture"] = "true";
    ensure("string cannot enable an automatic quit", !llfinalverseValidateVisualBenchmark(spec, error));
    spec["quit_after_capture"] = true;
    spec["grid_login_uri"] = "https://login.agni.lindenlab.com/cgi-bin/login.cgi";
    ensure("automatic quit cannot run on a cached public grid", !llfinalverseValidateVisualBenchmark(spec, error));
}
template<> template<> void object_t::test<10>()
{
    std::string error;
    spec["capture_size"] = LLSD::emptyArray();
    spec["capture_size"].append(1280); spec["capture_size"].append(738);
    ensure("bounded native pixel request", llfinalverseValidateVisualBenchmark(spec, error));
    spec["capture_size"][0] = 100000;
    ensure("refuse unbounded allocation", !llfinalverseValidateVisualBenchmark(spec, error));
    spec["capture_size"][0] = 1280.;
    ensure("dimensions must be integers", !llfinalverseValidateVisualBenchmark(spec, error));
}
}
