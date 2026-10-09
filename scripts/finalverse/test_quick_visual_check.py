# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import hashlib
import plistlib
from pathlib import Path
import tempfile
import unittest
import uuid
from harbor_benchmark import decode
from quick_visual_check import guarded_process, specification, submit
import xml.etree.ElementTree as ET


class QuickChecks(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(); self.root = Path(self.temp.name).resolve(); self.root.chmod(0o700)
    def tearDown(self): self.temp.cleanup()
    def test_one_trio_full_and_roundtrip(self):
        for mode, size in [('single', 1), ('trio', 3), ('full', 8)]:
            spec = specification(mode, 'CAM_Waterfront', self.root)
            self.assertEqual(len(spec['views']), size)
            submit(self.root, spec)
            self.assertEqual(decode(ET.parse(self.root / 'request.xml').getroot()[0]), spec)
            (self.root / 'request.xml').unlink()
        self.assertEqual(len(specification('trio', 'CAM_Pavilion', self.root)['views']), 3)
    def test_no_pending_overwrite_or_symlink(self):
        spec = specification('single', 'CAM_Lumi', self.root); submit(self.root, spec)
        original = (self.root / 'request.xml').read_bytes()
        with self.assertRaises(ValueError): submit(self.root, spec)
        self.assertEqual((self.root / 'request.xml').read_bytes(), original)
        (self.root / 'request.xml').unlink(); (self.root / 'active.xml').symlink_to(self.root / 'missing')
        with self.assertRaises(ValueError): submit(self.root, spec)
    def test_private_root_and_fresh_evidence(self):
        spec = specification('single', 'CAM_Lumi', self.root)
        self.root.chmod(0o755)
        with self.assertRaises(ValueError): submit(self.root, spec)
        self.root.chmod(0o700); Path(spec['output_dir']).mkdir()
        with self.assertRaises(ValueError): submit(self.root, spec)
    def test_finite_camera_and_bounded_time(self):
        for pos, focus in [([float('nan'), 120, 27], [176, 120, 26]), ([257, 120, 27], [176, 120, 26]), ([176, 120, 27], [176, 120, 26])]:
            with self.assertRaises(ValueError): specification('single', 'CAM_Waterfront', self.root, position=pos, focus=focus)
        with self.assertRaises(ValueError): specification('single', 'CAM_Lumi', self.root, sample=0.)
    def test_seat_is_explicit_single_fixture(self):
        uid = uuid.uuid4()
        spec = specification('single', 'CAM_Waterfront', self.root, seat=uid, seat_position=[176, 119.775, 25.48])
        self.assertEqual(spec['interaction']['seat_id'], uid)
        for mode, seat, pos in [('trio', uid, [176, 120, 25]), ('single', uuid.UUID(int=0), [176, 120, 25]), ('single', uid, [-1, 120, 25])]:
            with self.assertRaises(ValueError): specification(mode, 'CAM_Waterfront', self.root, seat=seat, seat_position=pos)
    def test_active_process_exact_fixture_and_build(self):
        app = self.root / 'Test.app'; (app / 'Contents/MacOS').mkdir(parents=True)
        (app / 'Contents/Info.plist').write_bytes(plistlib.dumps(dict(CFBundleIdentifier='com.finalverse.viewer.visualtests', CFBundleExecutable='Finalverse Test')))
        exe = app / 'Contents/MacOS/Finalverse Test'; exe.write_bytes(b'fixture')
        session = self.root / 'session.xml'; session.write_text('<llsd><map/></llsd>'); session.chmod(0o600)
        digest = hashlib.sha256(b'fixture').hexdigest(); rows = '42 ' + str(exe)
        cmd = f'{exe} --grid 127.0.0.1:18094 --sessionsettings {session} --set FinalverseQuickVisualDirectory {self.root}'
        self.assertEqual(guarded_process(app, session, self.root, digest, rows, lambda pid: cmd), 42)
        for wrong in [cmd.replace('127.0.0.1:18094', 'Agni'), cmd.replace(str(session), '/private/wrong.xml'), cmd.replace('FinalverseQuickVisualDirectory ' + str(self.root), 'FinalverseQuickVisualDirectory /wrong'), cmd.replace('FinalverseQuickVisualDirectory', 'OtherControl')]:
            with self.assertRaises(ValueError): guarded_process(app, session, self.root, digest, rows, lambda pid: wrong)
        with self.assertRaises(ValueError): guarded_process(app, session, self.root, '0'*64, rows, lambda pid: cmd)


if __name__ == '__main__': unittest.main()
