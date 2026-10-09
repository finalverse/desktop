#!/usr/bin/env python3
# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
from check_harbor_benchmark_process import identified_process


class ProcessSafety(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name).resolve()
        self.app = self.root / 'Fixture.app'
        self.session = self.root / 'session.xml'
        self.spec = self.root / 'spec.xml'
        self.executable = str((self.app / 'Contents/MacOS/Finalverse Test').resolve())
        self.rows = '123 ' + self.executable
        self.command = ('--grid 127.0.0.1:18094 --sessionsettings ' + str(self.session) +
                        ' --set FinalverseVisualBenchmarkFile ' + str(self.spec))
        self.fixture = {'output_dir': str(self.root / 'evidence')}
        self.result = {'status': 'complete', 'spec': self.fixture}

    def tearDown(self): self.temp.cleanup()

    def identify(self, rows=None, command=None):
        with patch('check_harbor_benchmark_process.document', side_effect=[self.fixture, self.result]):
            return identified_process(self.app, self.session, self.spec,
                                      rows or self.rows, lambda _: command or self.command)

    def test_identifies_only_selected_completed_process(self):
        self.assertEqual(self.identify(), 123)

    def test_refuses_cached_launch_or_different_session(self):
        for command in ['cached launch', self.command.replace('session.xml', 'other.xml'),
                        self.command.replace('127.0.0.1:18094', 'Agni'),
                        self.command.replace('127.0.0.1:18094', '127.0.0.1:180940')]:
            with self.assertRaises(ValueError): self.identify(command=command)

    def test_refuses_ambiguous_process_and_incomplete_evidence(self):
        with self.assertRaises(ValueError): self.identify(rows=self.rows + '\n456 ' + self.executable)
        self.result['status'] = 'running'
        with self.assertRaises(ValueError): self.identify()
        self.result = {'status': 'complete', 'spec': {'output_dir': 'other'}}
        with self.assertRaises(ValueError): self.identify()


if __name__ == '__main__': unittest.main()
