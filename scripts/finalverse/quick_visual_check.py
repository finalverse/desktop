#!/usr/bin/env python3
"""Capture a bounded camera check from the already logged-in loopback test viewer."""
# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import plistlib
import re
import stat
import subprocess
import time
import uuid
import xml.etree.ElementTree as ET
from harbor_benchmark import VIEWS, encode
from launch_harbor_benchmark import document, require, GRID_KEY, GRID_URI, REGION_ID, TEST_BUNDLES


def guarded_process(app, session, root, digest, rows, command_for_pid):
    info = plistlib.loads((app / 'Contents/Info.plist').read_bytes())
    require(info.get('CFBundleIdentifier') in TEST_BUNDLES and info.get('CFBundleExecutable') == 'Finalverse Test',
            'Expected the isolated test app.')
    exe = (app / 'Contents/MacOS/Finalverse Test').resolve()
    require(hashlib.sha256(exe.read_bytes()).hexdigest() == digest, 'Selected build hash must match.')
    document(session, private=True)  # Ownership/mode check; never print its contents.
    matches = []
    for row in rows.splitlines():
        fields = row.strip().split(None, 1)
        if len(fields) == 2 and fields[1] == str(exe): matches.append(int(fields[0]))
    require(len(matches) == 1, 'Expected one running guarded test process.')
    command = command_for_pid(matches[0])
    for fragment in ['--grid ' + GRID_KEY, '--sessionsettings ' + str(session.resolve()),
                     '--set FinalverseQuickVisualDirectory ' + str(root)]:
        require(re.search(r'(?:^|\s)' + re.escape(fragment) + r'(?:\s|$)', command) is not None,
                'Running viewer must match the guarded fixture session and command directory.')
    return matches[0]


def specification(mode, camera, root, warmup=5., sample=5., position=None, focus=None, seat=None, seat_position=None):
    require(mode in ('single', 'trio', 'full'), 'Unknown mode.')
    require(2 <= warmup <= 10 and 3 <= sample <= 10, 'Quick windows must be 2–10 / 3–10 seconds.')
    require(camera in {v[0] for v in VIEWS}, 'Use a canonical target name.')
    names = [camera] if mode == 'single' else list(dict.fromkeys(['CAM_Arrival', camera, 'CAM_Pavilion'])) if mode == 'trio' else [v[0] for v in VIEWS]
    # Keep trio at three views even when its target is Arrival or Pavilion.
    if mode == 'trio' and len(names) < 3: names.append('CAM_Harbor')
    def coordinates(value):
        return len(value) == 3 and all(math.isfinite(v) and 0 <= v <= (128 if i == 2 else 256) for i, v in enumerate(value))
    require((position is None) == (focus is None), 'Override both position and focus together.')
    views = []
    for name in names:
        _, pos, target = next(v for v in VIEWS if v[0] == name)
        if name == camera and position is not None:
            require(coordinates(position) and coordinates(focus), 'Target camera must stay in fixture bounds.')
            require(sum((position[i] - focus[i])**2 for i in range(2)) >= .01, 'Camera cannot point straight up or be degenerate.')
            pos, target = position, focus
        views.append(dict(name=name, position=list(pos), focus=list(target), fov=1.))
    result = dict(schema=1, quick_mode=True, grid_login_uri=GRID_URI, region_id=uuid.UUID(REGION_ID),
                  output_dir=str(root / ('capture-' + str(uuid.uuid4()))), warmup_seconds=warmup,
                  sample_seconds=sample, views=views)
    if seat is not None:
        require(mode == 'single' and seat.int != 0 and seat_position is not None and coordinates(seat_position),
                'Bench interaction requires one view, a fixture UUID and expected position.')
        result['interaction'] = dict(kind='bench_sit_stand', seat_id=seat, expected_position=seat_position)
    return result


def submit(root, fixture):
    info = root.lstat()
    require(root.is_absolute() and stat.S_ISDIR(info.st_mode) and info.st_uid == os.getuid() and
            stat.S_IMODE(info.st_mode) == 0o700, 'Use an owned mode-0700 command directory without symlinks.')
    require(not any((root / n).exists() or (root / n).is_symlink() for n in ['request.xml', 'active.xml']),
            'A previous request is pending; do not overwrite it.')
    require(Path(fixture['output_dir']).parent == root and not Path(fixture['output_dir']).exists(), 'Evidence must be new and inside the command directory.')
    xml = ET.Element('llsd'); xml.append(encode(fixture)); ET.indent(xml)
    temporary = root / ('pending-' + str(uuid.uuid4()) + '.xml')
    descriptor = os.open(temporary, os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o600)
    with os.fdopen(descriptor, 'wb') as out: out.write(ET.tostring(xml, encoding='utf-8', xml_declaration=True))
    # Hard link is atomic and refuses a concurrent request; unlike replace it
    # cannot silently overwrite work submitted by another command.
    try: os.link(temporary, root / 'request.xml')
    finally: temporary.unlink()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for key in ['app', 'session', 'directory']: parser.add_argument('--' + key, type=Path, required=True)
    parser.add_argument('--binary-sha256', required=True)
    parser.add_argument('--mode', choices=['single', 'trio', 'full'], default='single')
    parser.add_argument('--camera', '--target', choices=[v[0] for v in VIEWS], default='CAM_Waterfront')
    parser.add_argument('--position', nargs=3, type=float)
    parser.add_argument('--focus', nargs=3, type=float)
    parser.add_argument('--warmup', type=float, default=5.)
    parser.add_argument('--sample', type=float, default=5.)
    parser.add_argument('--seat-id', type=uuid.UUID, help='Explicit opt-in native owned-bench approach/sit/stand')
    parser.add_argument('--seat-position', nargs=3, type=float)
    args = parser.parse_args()
    try:
        rows = subprocess.check_output(['ps', '-axo', 'pid=,comm='], text=True)
        guarded_process(args.app, args.session, args.directory, args.binary_sha256, rows,
                        lambda pid: subprocess.check_output(['ps', '-p', str(pid), '-o', 'command='], text=True))
        fixture = specification(args.mode, args.camera, args.directory, args.warmup, args.sample,
                                args.position, args.focus, args.seat_id, args.seat_position)
        started = time.monotonic(); submit(args.directory, fixture)
        evidence = Path(fixture['output_dir']); results = evidence / 'results.xml'
        while time.monotonic() - started < 190:
            try: result = document(results, private=True)
            except (OSError, ET.ParseError): result = {}
            if result.get('spec') == fixture and result.get('status') in ('complete', 'failed'):
                summary = dict(mode=args.mode, wall_seconds=round(time.monotonic() - started, 3),
                               purpose='feature visual check; not a controlled release benchmark', **result)
                descriptor = os.open(evidence / 'summary.json', os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o600)
                with os.fdopen(descriptor, 'w') as out: json.dump(summary, out, indent=2, default=str)
                print(json.dumps(dict(status=result['status'], wall_seconds=summary['wall_seconds'],
                                      camera_restored=result.get('camera_restored', False), evidence=str(evidence))))
                if result['status'] != 'complete': parser.exit(1, 'Native quick check failed; inspect private evidence.\n')
                return
            time.sleep(.25)
        parser.exit(1, 'Quick check timed out; request/evidence preserved for diagnosis.\n')
    except (ValueError, OSError, KeyError, TypeError, ET.ParseError, subprocess.SubprocessError):
        parser.exit(1, 'Refused quick check: process, request or private fixture did not match. No credentials printed.\n')


if __name__ == '__main__': main()
