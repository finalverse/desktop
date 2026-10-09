#!/usr/bin/env python3
"""Generate the bounded local camera fixture and read native viewer measurements."""
# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import argparse
from pathlib import Path
import uuid
import xml.etree.ElementTree as ET

VIEWS = [
    ('CAM_Arrival', (128, 112, 30), (128, 140, 26)),
    ('CAM_Harbor', (111, 109, 43), (153, 158, 25)),
    ('CAM_Lumi', (172, 146, 27.1), (172, 150, 26.25)),
    ('CAM_LumiHome', (164, 144, 29), (172, 154, 26.4)),
    ('CAM_Garden', (178, 144, 28.5), (171, 149, 25.7)),
    ('CAM_Pavilion', (110, 176, 31), (128, 192, 28)),
    ('CAM_Waterfront', (169, 117, 30), (184, 132, 25.8)),
    ('CAM_Sunset', (193, 149, 29), (215, 149, 22)),
]

def encode(value):
    if isinstance(value, dict):
        node = ET.Element('map')
        for key, val in value.items():
            ET.SubElement(node, 'key').text = key
            node.append(encode(val))
    elif isinstance(value, (list, tuple)):
        node = ET.Element('array')
        node.extend(encode(x) for x in value)
    else:
        tag = 'uuid' if isinstance(value, uuid.UUID) else 'boolean' if isinstance(value, bool) else 'integer' if isinstance(value, int) else 'real' if isinstance(value, float) else 'string'
        node = ET.Element(tag); node.text = ('1' if value else '0') if isinstance(value, bool) else str(value)
    return node

def decode(node):
    if node.tag == 'map':
        children = list(node)
        return {children[i].text: decode(children[i+1]) for i in range(0, len(children), 2)}
    if node.tag == 'array': return [decode(child) for child in node]
    if node.tag == 'integer': return int(node.text or 0)
    if node.tag == 'real': return float(node.text or 0)
    if node.tag == 'boolean': return node.text in ('1', 'true')
    if node.tag == 'uuid': return uuid.UUID(node.text or '00000000-0000-0000-0000-000000000000')
    return node.text or ''

def compare_runs(baseline, candidate, changes):
    """Refuse a performance comparison when the native fixtures differ."""
    def require(condition):
        if not condition: raise ValueError('Completed native runs must match build, fixture, cameras, environment, pixels and cached counts; only declared graphics changes are allowed.')
    require(baseline['status'] == candidate['status'] == 'complete')
    require(baseline['viewer'] == candidate['viewer'])
    fixture = lambda run: {k:v for k,v in run['spec'].items() if k != 'output_dir'}
    require(fixture(baseline) == fixture(candidate))
    before, after = baseline['views'], candidate['views']
    require(len(before) == len(after) == len(baseline['spec']['views']) and len(before) > 0)
    rows = []
    for left, right in zip(before, after):
        require(all(left[k] == right[k] for k in ('camera', 'sky', 'water', 'environment_selection', 'width_raw', 'height_raw', 'cached_objects')))
        require(left['graphics'].keys() == right['graphics'].keys())
        differences = {k for k,v in left['graphics'].items() if v != right['graphics'][k]}
        require(differences <= set(changes))
        for view in (left, right):
            require(view['valid'] and view['screenshot_saved'] and view['frame_csv_saved'] and view['frames'] > 0 and view['sample_seconds'] > 0)
        rows.append((left['camera']['name'], left['fps'], right['fps'], (right['fps']/left['fps'] - 1)*100, left['p95_ms'], right['p95_ms']))
    weighted = lambda views: sum(v['frames'] for v in views)/sum(v['sample_seconds'] for v in views)
    return rows, weighted(before), weighted(after)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    spec = commands.add_parser('spec')
    spec.add_argument('--file', type=Path, required=True)
    spec.add_argument('--output-dir', type=Path, required=True)
    spec.add_argument('--region-id', type=uuid.UUID, default=uuid.UUID('a9d44538-26ca-4586-a8a9-e551722ffa15'))
    spec.add_argument('--warmup', type=float, default=15.)
    spec.add_argument('--sample', type=float, default=15.)
    spec.add_argument('--views', nargs='+', choices=[v[0] for v in VIEWS], help='Canonical subset for feature A/B; omitted means all eight')
    report = commands.add_parser('report')
    report.add_argument('results', type=Path)
    comparison = commands.add_parser('compare', help='One matched native comparison; no GUI interaction')
    comparison.add_argument('baseline', type=Path)
    comparison.add_argument('candidate', type=Path)
    comparison.add_argument('--changes', nargs='+', required=True, help='Only these recorded graphics controls may differ')
    args = parser.parse_args()
    if args.command == 'spec':
        if not 10 <= args.warmup <= 60 or not 10 <= args.sample <= 60:
            parser.error('Sample and warm-up must be 10–60 seconds.')
        if args.views and len(set(args.views)) != len(args.views):
            parser.error('Camera names must be unique.')
        document = ET.Element('llsd')
        document.append(encode(dict(schema=1, grid_login_uri='http://127.0.0.1:18094/',
            region_id=args.region_id, output_dir=str(args.output_dir.absolute()),
            warmup_seconds=args.warmup, sample_seconds=args.sample,
            views=[dict(name=n, position=p, focus=f, fov=1.) for n,p,f in VIEWS if not args.views or n in args.views])))
        ET.indent(document)
        with args.file.open('xb') as output:
            output.write(ET.tostring(document, encoding='utf-8', xml_declaration=True))
        args.file.chmod(0o600)
        print(f'Created camera specification: {args.file}')
    elif args.command == 'compare':
        try:
            rows, before, after = compare_runs(decode(ET.parse(args.baseline).getroot()[0]), decode(ET.parse(args.candidate).getroot()[0]), args.changes)
        except (ValueError, KeyError, TypeError, ZeroDivisionError, OSError, ET.ParseError):
            parser.exit(1, 'Refused comparison: incomplete, invalid or mismatched native evidence.\n')
        print('| Camera | Baseline FPS | Candidate FPS | Change | Baseline p95 ms | Candidate p95 ms |')
        print('|---|---:|---:|---:|---:|---:|')
        for name, left, right, change, lp95, rp95 in rows:
            print(f'| {name} | {left:.1f} | {right:.1f} | {change:+.2f}% | {lp95:.2f} | {rp95:.2f} |')
        print(f'Weighted FPS: {before:.2f} → {after:.2f} ({(after/before-1)*100:+.2f}%). Single runs; not a statistical confidence interval or isolated GPU cost.')
    else:
        data = decode(ET.parse(args.results).getroot()[0])
        print('Status:', data['status'])
        print('| Camera | FPS | Mean ms | Median ms | P95 ms | Cached objects | Raw pixels | Screenshot |')
        print('|---|---:|---:|---:|---:|---:|---|---|')
        for view in data.get('views', []):
            print(f"| {view['camera']['name']} | {view['fps']:.1f} | {view['mean_ms']:.2f} | {view['median_ms']:.2f} | {view['p95_ms']:.2f} | {view['cached_objects']} | {view['width_raw']} × {view['height_raw']} | {view['screenshot_saved']} |")

if __name__ == '__main__': main()
