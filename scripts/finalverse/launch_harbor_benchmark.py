#!/usr/bin/env python3
"""Fail-closed launcher for the isolated loopback MutSea visual fixture on macOS."""
# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import argparse
import hashlib
import os
from pathlib import Path
import plistlib
import stat
import subprocess
import sys
import xml.etree.ElementTree as ET
from harbor_benchmark import decode, VIEWS

GRID_KEY = '127.0.0.1:18094'
GRID_URI = 'http://127.0.0.1:18094/'
REGION_ID = 'a9d44538-26ca-4586-a8a9-e551722ffa15'
TEST_BUNDLES = {'com.finalverse.viewer.experience', 'com.finalverse.viewer.visualtests'}
ALLOWED_SESSION = {
    'FinalverseUXMetrics', 'FirstLoginThisInstall', 'FirstRunThisInstall',
    'UserLoginInfo', 'RememberPassword', 'RememberUser', 'AutoLogin',
    'FinalverseExperienceEnabled', 'FinalverseVisualBenchmarkFile',
    'RenderFarClip', 'RenderShadowDetail', 'RenderShadowResolutionScale',
    'RenderDeferredSSAO', 'RenderFSAAType', 'RenderFSAASamples',
    'RenderAnisotropic', 'RenderAnisotropicLevel', 'RenderReflectionProbeDetail',
    'RenderReflectionsEnabled', 'RenderTonemapType', 'RenderExposure',
    'RenderAvatarMaxNonImpostors',
}

def require(condition, message):
    if not condition: raise ValueError(message)

def document(path, private=False):
    info = path.lstat()
    require(stat.S_ISREG(info.st_mode) and info.st_uid == os.getuid(), 'Expected an owned regular fixture file.')
    if private: require(stat.S_IMODE(info.st_mode) == 0o600, 'Private session file must have mode 0600.')
    require(0 < info.st_size <= 1024 * 1024, 'Fixture file size is invalid.')
    return decode(ET.parse(path).getroot()[0])

def preflight(app, profile, session, spec, expected_hash, quick_directory=None):
    require(sys.platform == 'darwin', 'This launcher is for the macOS test fixture.')
    require(profile.is_dir() and profile.resolve() != Path.home().resolve(), 'An existing isolated profile is required.')
    info = plistlib.loads((app / 'Contents/Info.plist').read_bytes())
    require(info.get('CFBundleIdentifier') in TEST_BUNDLES and
            info.get('CFBundleExecutable') == 'Finalverse Test', 'Expected the isolated Finalverse test bundle.')
    executable = app / 'Contents/MacOS/Finalverse Test'
    require(hashlib.sha256(executable.read_bytes()).hexdigest() == expected_hash, 'Test executable hash does not match the selected milestone.')
    procs = subprocess.check_output(['ps', '-axo', 'comm='], text=True).splitlines()
    # LaunchServices can reuse a different checkout with the same bundle ID and
    # discard its new arguments. Do not open another test package concurrently.
    require(not any(p.strip().endswith('/Contents/MacOS/Finalverse Test') for p in procs),
            'Quit the existing Finalverse test viewer before launching a new session.')
    grids = document(profile / 'Library/Application Support/Finalverse/user_settings/grids.xml')
    require(isinstance(grids, dict) and grids.get(GRID_KEY, {}).get('login_uri') == [GRID_URI], 'Isolated profile must map the explicit fixture grid to loopback only.')
    settings = document(session, private=True)
    require(isinstance(settings, dict) and not set(settings) - ALLOWED_SESSION, 'Session contains unsupported controls; it cannot redirect the test profile or login endpoint.')
    def value(key): return settings.get(key, {}).get('Value')
    require(all(value(k) is False for k in ['AutoLogin', 'RememberUser', 'RememberPassword']), 'Use one-session login; cached autologin and credential saving are forbidden.')
    login = value('UserLoginInfo')
    require(isinstance(login, list) and len(login) == 3 and all(isinstance(x, str) and x for x in login), 'An authorized private one-session fixture login is required.')
    fixture = document(spec)
    if 'capture_size' in fixture:
        size = fixture['capture_size']
        require(isinstance(size, list) and len(size) == 2 and all(type(v) is int for v in size) and
                640 <= size[0] <= 2560 and 480 <= size[1] <= 1600, 'Capture pixels must be bounded integers.')
    require('quit_after_capture' not in fixture or type(fixture['quit_after_capture']) is bool, 'Automatic native logout must be an explicit boolean.')
    require(fixture.get('schema') == 1 and fixture.get('grid_login_uri') == GRID_URI and str(fixture.get('region_id')) == REGION_ID, 'Benchmark must target the designated loopback grid and region.')
    views = fixture.get('views', [])
    names = [v.get('name') for v in views]
    require(1 <= len(names) <= 8 and len(set(names)) == len(names) and set(names) <= {v[0] for v in VIEWS}, 'Expected a unique canonical camera subset.')
    output = Path(fixture['output_dir'])
    require(output.is_absolute() and not output.exists() and not output.is_symlink() and output.parent.is_dir(), 'Choose a new absolute evidence directory with an existing parent.')
    # No startup URLs or additional options are accepted. Credentials remain in
    # the owned session file, never argv or preflight output.
    command = ['open', '--env', 'CFFIXED_USER_HOME=' + str(profile.resolve()), str(app.resolve()),
            '--args', '--grid', GRID_KEY, '--sessionsettings', str(session.resolve()),
            '--set', 'FinalverseVisualBenchmarkFile', str(spec.resolve()),
            '--set', 'AutoLogin', 'false', '--set', 'RememberPassword', 'false', '--set', 'RememberUser', 'false']
    if quick_directory is not None:
        info = quick_directory.lstat()
        require(quick_directory.is_absolute() and stat.S_ISDIR(info.st_mode) and
                info.st_uid == os.getuid() and stat.S_IMODE(info.st_mode) == 0o700,
                'Quick-check directory must be owned, absolute, mode 0700 and not a symlink.')
        require(not any((quick_directory / n).exists() or (quick_directory / n).is_symlink()
                        for n in ['request.xml', 'active.xml']), 'Quick-check directory contains a pending request.')
        command += ['--set', 'FinalverseQuickVisualDirectory', str(quick_directory)]
    return command

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for flag in ['app', 'profile', 'session', 'spec']: parser.add_argument('--' + flag, type=Path, required=True)
    parser.add_argument('--binary-sha256', required=True)
    parser.add_argument('--check-only', action='store_true')
    parser.add_argument('--quick-directory', type=Path, help='Private directory for later captures in this same session')
    args = parser.parse_args()
    try:
        command = preflight(args.app, args.profile, args.session, args.spec, args.binary_sha256, args.quick_directory)
        if not args.check_only: subprocess.run(command, check=True, capture_output=True)
    except (ValueError, OSError, KeyError, TypeError, ET.ParseError, subprocess.SubprocessError):
        # Never interpolate XML parser errors or subprocess output: those may
        # contain a private payload. Detailed failures can be tested offline.
        parser.exit(1, 'Refused launch: fixture/profile/session/executable validation failed. No credentials printed.\n')
    print('Validated isolated MutSea test launch.' if args.check_only else 'Launched isolated MutSea test viewer; keep it foreground for capture.')

if __name__ == '__main__': main()
