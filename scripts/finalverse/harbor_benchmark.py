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

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    spec = commands.add_parser('spec')
    spec.add_argument('--file', type=Path, required=True)
    spec.add_argument('--output-dir', type=Path, required=True)
    spec.add_argument('--region-id', type=uuid.UUID, default=uuid.UUID('a9d44538-26ca-4586-a8a9-e551722ffa15'))
    spec.add_argument('--warmup', type=float, default=15.)
    spec.add_argument('--sample', type=float, default=15.)
    report = commands.add_parser('report')
    report.add_argument('results', type=Path)
    args = parser.parse_args()
    if args.command == 'spec':
        if not 10 <= args.warmup <= 60 or not 10 <= args.sample <= 60:
            parser.error('Sample and warm-up must be 10–60 seconds.')
        document = ET.Element('llsd')
        document.append(encode(dict(schema=1, grid_login_uri='http://127.0.0.1:18094/',
            region_id=args.region_id, output_dir=str(args.output_dir.absolute()),
            warmup_seconds=args.warmup, sample_seconds=args.sample,
            views=[dict(name=n, position=p, focus=f, fov=1.) for n,p,f in VIEWS])))
        ET.indent(document)
        with args.file.open('xb') as output:
            output.write(ET.tostring(document, encoding='utf-8', xml_declaration=True))
        args.file.chmod(0o600)
        print(f'Created camera specification: {args.file}')
    else:
        data = decode(ET.parse(args.results).getroot()[0])
        print('Status:', data['status'])
        print('| Camera | FPS | Mean ms | Median ms | P95 ms | Cached objects | Raw pixels | Screenshot |')
        print('|---|---:|---:|---:|---:|---:|---|---|')
        for view in data.get('views', []):
            print(f"| {view['camera']['name']} | {view['fps']:.1f} | {view['mean_ms']:.2f} | {view['median_ms']:.2f} | {view['p95_ms']:.2f} | {view['cached_objects']} | {view['width_raw']} × {view['height_raw']} | {view['screenshot_saved']} |")

if __name__ == '__main__': main()
