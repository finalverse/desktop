#!/usr/bin/env python3
"""Identify a completed development benchmark before binding the native UI."""
# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import argparse
from pathlib import Path
import re
import subprocess
import xml.etree.ElementTree as ET
from launch_harbor_benchmark import document, require, GRID_KEY


def identified_process(app, session, spec, process_rows, command_for_pid):
    executable = str((app / 'Contents/MacOS/Finalverse Test').resolve())
    matches = []
    for row in process_rows.splitlines():
        fields = row.strip().split(None, 1)
        if len(fields) == 2 and fields[1] == executable:
            matches.append(int(fields[0]))
    require(len(matches) == 1, 'Expected exactly one active test executable.')
    pid = matches[0]
    # Inspect privately; never print a process argument string. A UI tool can
    # reopen the bundle without these flags after Quit, using a cached profile.
    command = command_for_pid(pid)
    for fragment in ['--grid ' + GRID_KEY,
                     '--sessionsettings ' + str(session.resolve()),
                     'FinalverseVisualBenchmarkFile ' + str(spec.resolve())]:
        require(re.search(r'(?:^|\s)' + re.escape(fragment) + r'(?:\s|$)', command) is not None,
                'Active process does not match the guarded test launch.')
    fixture = document(spec)
    result = document(Path(fixture['output_dir']) / 'results.xml')
    require(result.get('status') == 'complete' and result.get('spec') == fixture,
            'Keep an incomplete or mismatched benchmark running.')
    return pid


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for flag in ['app', 'session', 'spec']:
        parser.add_argument('--' + flag, type=Path, required=True)
    args = parser.parse_args()
    try:
        rows = subprocess.check_output(['ps', '-axo', 'pid=,comm='], text=True)
        def command(pid):
            return subprocess.check_output(['ps', '-p', str(pid), '-o', 'command='], text=True)
        identified_process(args.app, args.session, args.spec, rows, command)
        print('Completed guarded test process verified; use native Quit for orderly grid logout.')
    except (ValueError, OSError, KeyError, TypeError, ET.ParseError, subprocess.SubprocessError):
        parser.exit(1, 'Refused UI handoff: active process or completed evidence does not match. No private arguments printed.\n')


if __name__ == '__main__': main()
