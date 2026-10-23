#!/usr/bin/env python3
"""Check explicit source license notices without assigning new licenses.

Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
"""
import re
import sys
from pathlib import Path

LEGACY = re.compile(r"\$LicenseInfo:[^$]*\blicense=(lgpl|viewerlgpl|bsd|mit)(?=[&$])", re.I)
SPDX = re.compile(r"SPDX-License-Identifier:\s*([^\r\n]+)")
ALLOWED = {"LGPL-2.1-only", "LGPL-2.1-or-later", "MIT", "Apache-2.0", "MIT OR Apache-2.0"}


def has_license(text):
    if LEGACY.search(text):
        return True
    for match in SPDX.finditer(text):
        if match[1].strip().removesuffix("*/").strip() in ALLOWED:
            return True
    # The ABI header is byte-for-byte vendored from FinalStorm and carries its
    # original dual-license notice. Do not silently relicense copied interfaces.
    return bool(re.search(r"/\* Copyright 2026 Finalverse contributors\. MIT OR Apache-2\.0\. \*/", text))


def main(paths):
    missing = [p for p in paths if not has_license(Path(p).read_text())]
    for path in missing:
        print(f"{path}: No recognized opensource license notice", file=sys.stderr)
    return bool(missing)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
