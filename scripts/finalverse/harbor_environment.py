#!/usr/bin/env python3
"""Generate local preview specs from a completed native baseline; no world writes."""
# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import argparse
import copy
import json
import math
from pathlib import Path
import xml.etree.ElementTree as ET
from harbor_benchmark import decode, encode

def direction_quaternion(azimuth, altitude):
    azimuth, altitude = map(math.radians, (azimuth, altitude))
    x = math.cos(azimuth) * math.cos(altitude)
    y = math.sin(azimuth) * math.cos(altitude)
    z = math.sin(altitude)
    axis_length = math.hypot(y, z)
    if axis_length < 1e-8: return [0., 0., 0., 1.] if x > 0 else [0., 0., 1., 0.]
    half_angle = .5 * math.acos(max(-1., min(1., x)))
    factor = math.sin(half_angle) / axis_length
    return [0., -z * factor, y * factor, math.cos(half_angle)]

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--library', type=Path, required=True)
    parser.add_argument('--variant', choices=('matched', 'golden-hour', 'day', 'night'), required=True)
    parser.add_argument('--file', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    baseline = decode(ET.parse(args.baseline).getroot()[0])
    if baseline['status'] != 'complete' or len(baseline.get('views', [])) != 8:
        parser.error('Complete all eight native baseline cameras first.')
    spec = copy.deepcopy(baseline['spec'])
    spec['output_dir'] = str(args.output_dir.absolute())
    environment = {key: copy.deepcopy(baseline['views'][0][key]) for key in ('sky', 'water')}
    if args.variant != 'matched':
        library = json.loads(args.library.read_text())
        if library['schema_version'] != 1 or library['library_id'] != 'mutsea.starter.harbor-environments':
            parser.error('Expected declared Harbor numerical art-direction library.')
        preset = library['presets'][args.variant]
        sky = environment['sky']; water = environment['water']
        sky['sun_rotation'] = direction_quaternion(preset['sun_azimuth_degrees'], preset['sun_altitude_degrees'])
        sky['moon_rotation'] = direction_quaternion(preset['moon_azimuth_degrees'], preset['moon_altitude_degrees'])
        for key in ('name', 'sunlight_color', 'cloud_shadow', 'cloud_scroll_rate', 'star_brightness', 'moon_brightness'):
            sky[key] = preset[key]
        for key in ('ambient', 'haze_density', 'haze_horizon'): sky['legacy_haze'][key] = preset[key]
        sky['gamma'] = 1.0
        water.update(library['water']); water['name'] = 'Finalverse Harbor Water'
    spec['environment'] = environment
    root = ET.Element('llsd');root.append(encode(spec));ET.indent(root)
    with args.file.open('xb') as output:output.write(ET.tostring(root,encoding='utf-8',xml_declaration=True))
    args.file.chmod(0o600)
    print('Created local preview spec:',args.variant)

if __name__ == '__main__':main()
