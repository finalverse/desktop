#!/usr/bin/env python3
# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import copy
import unittest
from harbor_benchmark import compare_runs

class EvidenceComparison(unittest.TestCase):
    def setUp(self):
        self.baseline = dict(status='complete', viewer='fixture build', spec=dict(views=[dict(name='CAM_Lumi')], output_dir='baseline'), views=[dict(camera=dict(name='CAM_Lumi'), sky={}, water={}, environment_selection=1, width_raw=1280, height_raw=738, cached_objects=10, graphics=dict(RenderShadowDetail=0, RenderDeferredSSAO=False), valid=True, screenshot_saved=True, frame_csv_saved=True, frames=100, sample_seconds=2., fps=50., p95_ms=24.)])
        self.candidate = copy.deepcopy(self.baseline)
        self.candidate['spec']['output_dir'] = 'candidate'
        self.candidate['views'][0]['graphics']['RenderShadowDetail'] = 1
    def compare(self): return compare_runs(self.baseline, self.candidate, ['RenderShadowDetail'])
    def test_matched_declared_change(self):
        rows, before, after = self.compare()
        self.assertEqual((len(rows), before, after), (1, 50., 50.))
    def test_reject_unfinished_and_missing_evidence(self):
        self.candidate['status'] = 'running'
        with self.assertRaises(ValueError): self.compare()
        self.candidate['status'] = 'complete'; self.candidate['views'][0]['screenshot_saved'] = False
        with self.assertRaises(ValueError): self.compare()
    def test_reject_changed_scene_camera_build_and_environment(self):
        original = copy.deepcopy(self.candidate)
        for key, value in [('camera',dict(name='CAM_Garden')), ('cached_objects',11), ('width_raw',900), ('sky',dict(ambient=2))]:
            self.candidate = copy.deepcopy(original); self.candidate['views'][0][key] = value
            with self.assertRaises(ValueError): self.compare()
        self.candidate = copy.deepcopy(original); self.candidate['viewer'] = 'another build'
        with self.assertRaises(ValueError): self.compare()
    def test_reject_undeclared_graphics_change(self):
        self.candidate['views'][0]['graphics']['RenderDeferredSSAO'] = True
        with self.assertRaises(ValueError): self.compare()

if __name__ == '__main__': unittest.main()
