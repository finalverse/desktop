#!/usr/bin/env python3
"""Read CPU/RSS for the named local executable while a native capture runs."""
# Copyright (c) 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
import argparse
import csv
import datetime
from pathlib import Path
import subprocess
import time
import xml.etree.ElementTree as ET
from harbor_benchmark import decode

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--results',type=Path,required=True)
    parser.add_argument('--file',type=Path,required=True)
    parser.add_argument('--executable',type=Path,required=True)
    args=parser.parse_args()
    with args.file.open('x') as stream:
        args.file.chmod(0o600)
        writer=csv.writer(stream);writer.writerow(['utc','pid','cpu_percent','rss_kb'])
        for _ in range(240):
            procs=subprocess.check_output(['ps','-axo','pid,comm'],text=True).splitlines()
            for line in procs:
                fields=line.strip().split(None,1)
                if len(fields)!=2 or fields[1]!=str(args.executable):continue
                result=subprocess.run(['ps','-p',fields[0],'-o','%cpu=,rss='],text=True,capture_output=True)
                values=result.stdout.split()
                if result.returncode==0 and len(values)==2:
                    writer.writerow([datetime.datetime.now(datetime.timezone.utc).isoformat(),fields[0],*values])
            stream.flush()
            try:
                if decode(ET.parse(args.results).getroot()[0]).get('status') in ('complete','failed'):break
            except (FileNotFoundError,ET.ParseError):pass
            time.sleep(5)
    print('Completed local process sampling; CPU includes all viewer threads, RSS excludes separate helpers.')

if __name__=='__main__':main()
