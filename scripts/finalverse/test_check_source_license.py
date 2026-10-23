#!/usr/bin/env python3
"""Exercise accepted notices and refusal of absent or unrecognized licenses.

Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
"""
import unittest
from check_source_license import has_license


class SourceLicenseTests(unittest.TestCase):
    def test_upstream_notices(self):
        for name in ("lgpl", "viewerlgpl", "bsd", "mit"):
            self.assertTrue(has_license(f"$LicenseInfo:firstyear=2001&license={name}$"))

    def test_spdx_and_vendored_notice(self):
        for name in ("LGPL-2.1-only", "LGPL-2.1-or-later", "MIT", "Apache-2.0", "MIT OR Apache-2.0"):
            self.assertTrue(has_license(f"/* SPDX-License-Identifier: {name} */"))
        self.assertTrue(has_license("/* Copyright 2026 Finalverse contributors. MIT OR Apache-2.0. */"))

    def test_missing_unknown_or_partial_notice(self):
        for text in ("", "Copyright 2026", "MIT", "SPDX-License-Identifier: Proprietary",
                     "SPDX-License-Identifier: MIT AND Proprietary", "$LicenseInfo:license=mit-proprietary$"):
            self.assertFalse(has_license(text))


if __name__ == "__main__":
    unittest.main()
