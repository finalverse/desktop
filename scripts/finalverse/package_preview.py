#!/usr/bin/env python3
"""Package a clean Mac developer preview, without operator configuration.

Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import plistlib
import platform
import sys
from importlib.metadata import version
import re
import shutil
import subprocess
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[2]


def run(*args, **kwargs):
    return subprocess.run(args, check=True, capture_output=True, **kwargs)


def require(value, message):
    if not value:
        raise RuntimeError(message)


def sha(path):
    with path.open("rb") as source:
        return hashlib.file_digest(source, "sha256").hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bundle", type=Path, required=True)
    parser.add_argument("--packages", type=Path, required=True)
    parser.add_argument("--build-variables", type=Path, required=True)
    parser.add_argument("--third-party-source", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--version", default="0.4.0-preview.1")
    args = parser.parse_args()
    require(re.fullmatch(r"\d+\.\d+\.\d+-preview\.\d+", args.version), "Preview version required")
    require(not run("git", "status", "--porcelain", cwd=ROOT, text=True).stdout.strip(),
            "Package only a clean committed tree")
    require(not args.output.exists(), "Use a fresh output directory; preserve prior artifacts")
    require(args.third_party_source.is_file(), "Pinned LGPL/MPL dependency sources required")
    info = plistlib.loads((args.bundle / "Contents/Info.plist").read_bytes())
    require(info["CFBundleIdentifier"] == "com.finalverse.viewer.preview", "Wrong bundle identity")
    commit = run("git", "rev-parse", "HEAD", cwd=ROOT, text=True).stdout.strip()
    args.output.mkdir(parents=True)
    stage = args.output / "disk-image"
    stage.mkdir()
    app = stage / "Finalverse Preview.app"
    run("ditto", "--norsrc", str(args.bundle), str(app))
    resources = app / "Contents/Resources"
    # This first distributable preview omits the unclosed libVLC codec source
    # inventory. Keep the implementation/full development build unchanged.
    plugins = resources / "SLPlugin.app/Contents/Frameworks"
    for path in list(plugins.glob("libvlc*.dylib*")) + [plugins / "media_plugin_libvlc.dylib", plugins / "plugins"]:
        if path.is_symlink() or path.is_file():
            path.unlink()
        elif path.is_dir():
            shutil.rmtree(path)
    # Voice is retained in source/binaries but is not an accepted preview gate.
    settings = resources / "app_settings/settings.xml"
    tree = ET.parse(settings)
    values = list(tree.getroot()[0])
    for index, item in enumerate(values[:-1]):
        if item.tag == "key" and item.text == "EnableVoiceChat":
            controls = list(values[index + 1])
            for j, control in enumerate(controls[:-1]):
                if control.tag == "key" and control.text == "Value":
                    controls[j + 1].text = "0"
    tree.write(settings, encoding="utf-8", xml_declaration=True)
    import llsd
    controls = llsd.parse_xml(settings.read_bytes())
    require(not controls.get("UserLoginInfo", {}).get("Value"), "Saved login found in app defaults")
    require(not controls.get("AutoLogin", {}).get("Value"), "Automatic login must be off")
    legal = resources / "FinalverseDistribution"
    legal.mkdir(exist_ok=False)
    for source in [ROOT / "LICENSE", ROOT / "doc/LICENSE-source.txt", ROOT / "doc/LICENSE-logos.txt"]:
        shutil.copyfile(source, legal / source.name)
    shutil.copytree(args.packages / "licenses", legal / "third-party-notices")
    shutil.copyfile(args.build_variables, legal / "build-variables")
    shutil.copyfile(args.build_variables, args.output / f"Finalverse-{args.version}-build-variables.txt")
    installed = llsd.parse_xml((args.packages / "installed-packages.xml").read_bytes())
    dependencies = []
    for name, package in installed["dependencies"].items():
        description = package.get("package_description", {})
        dependencies.append({"name": name, **{k: description[k] for k in
            ("version", "license", "license_file", "copyright", "canonical_repo", "vcs_revision", "vcs_branch") if k in description},
            "archive": package.get("archive"),
            "scope": "build dependency; not every installed package is shipped"})
    inventory = args.output / f"Finalverse-{args.version}-dependencies.json"
    inventory.write_text(json.dumps(dependencies, indent=2) + "\n")
    shutil.copyfile(inventory, legal / inventory.name)
    provenance = {"release": "v" + args.version, "repository": "https://github.com/finalverse/final_viewer",
        "commit": commit, "bundle_id": info["CFBundleIdentifier"],
        "engine_version": info.get("CFBundleShortVersionString"), "engine_build": info.get("CFBundleVersion"),
        "signing": "ad hoc; no Developer ID or notarization", "runtime_verified": "Apple Silicon; Intel hardware unverified",
        "source": f"Finalverse-{args.version}-source.tar.gz",
        "third_party_source": args.third_party_source.name, "third_party_source_sha256": sha(args.third_party_source),
        "build_variables_sha256": sha(args.build_variables),
        "configuration": {"architectures": ["arm64", "x86_64"], "proprietary": False, "bugsplat": False,
            "updater": False, "voice_default": False, "libvlc_distribution": False},
        "limits": ["invited world access", "one configured hosted Lumi controller", "experimental developer preview"],
        "xcode": run("xcodebuild", "-version", text=True).stdout.strip(),
        "cmake": run("cmake", "--version", text=True).stdout.splitlines()[0],
        "python": sys.version.split()[0], "autobuild": version("autobuild"),
        "sdk": run("xcrun", "--show-sdk-version", text=True).stdout.strip(),
        "runtime_verified_macos": platform.mac_ver()[0]}
    source_note = (f"Finalverse v{args.version}\nSource commit: {commit}\n"
        "Inherited viewer source is LGPL-2.1-only; preserve upstream notices.\n"
        f"Corresponding viewer source and dependency sources: https://github.com/finalverse/final_viewer/releases/tag/v{args.version}\n"
        "Build instructions: docs/finalverse/building-preview.md in the source archive.\n"
        "Optional libVLC/codec playback is excluded from this developer preview pending its source inventory.\n"
        "Voice defaults off and is not preview-certified. This does not change the full source implementation.\n")
    (legal / "SOURCE.txt").write_text(source_note)
    for path in app.rglob("*"):
        if path.is_symlink():
            require(path.resolve().is_relative_to(app.resolve()), "Bundle symlink escapes application")
            continue
        require(path.name not in {"account.json", "session.xml", "session-settings.xml", ".env", "grids.xml"}
                and path.suffix != ".log" and not path.name.endswith(".dSYM"), "Private/debug file in bundle")
        if path.is_dir():
            path.chmod(0o755)
        else:
            executable = bool(path.stat().st_mode & 0o111)
            path.chmod(0o755 if executable else 0o644)
    executable = app / "Contents/MacOS" / info["CFBundleExecutable"]
    architectures = run("lipo", "-archs", str(executable), text=True).stdout.split()
    require(set(architectures) == {"arm64", "x86_64"}, "Expected universal Mac executable")
    for path in app.rglob("*"):
        if not path.is_file() or path.is_symlink():
            continue
        magic = path.open("rb").read(4)
        if magic not in (b"\xca\xfe\xba\xbe", b"\xcf\xfa\xed\xfe", b"\xce\xfa\xed\xfe"):
            continue
        linked = "\n".join(line for line in run("otool", "-L", str(path), text=True).stdout.splitlines()
                           if line.startswith("\t"))
        require(not any(prefix in linked for prefix in ("/Users/", "/home/", "/opt/homebrew/", "/usr/local/")),
                "Bundle links a nonportable developer library")
    run("codesign", "--force", "--deep", "--sign", "-", str(app))
    run("codesign", "--verify", "--deep", "--strict", str(app))
    provenance["executable_sha256"] = sha(executable)
    provenance["bundle_symlinks_checked"] = True
    provenance["portable_dynamic_links_checked"] = True
    (args.output / f"Finalverse-{args.version}-provenance.json").write_text(json.dumps(provenance, indent=2) + "\n")
    (stage / "START-HERE.txt").write_text(source_note + "\nDrag Finalverse Preview.app to Applications.\n"
        "Read https://finalverse.com/get-started for checksum/signing details and world invitation.\n"
        "No public account or password is bundled. Do not disable Gatekeeper globally.\n")
    (stage / "Applications").symlink_to("/Applications", target_is_directory=True)
    run("git", "archive", "--format=tar.gz", f"--prefix=Finalverse-{args.version}-source/", "HEAD",
        "-o", str(args.output / f"Finalverse-{args.version}-source.tar.gz"), cwd=ROOT)
    shutil.copyfile(args.third_party_source, args.output / args.third_party_source.name)
    dmg = args.output / f"Finalverse-{args.version}-mac-universal.dmg"
    run("hdiutil", "create", "-srcfolder", str(stage), "-volname", "Finalverse Preview", "-format", "UDZO", str(dmg))
    run("hdiutil", "verify", str(dmg))
    print(json.dumps({"commit": commit, "dmg": dmg.name, "sha256": sha(dmg),
                      "signing": provenance["signing"], "private_files_checked": True}, indent=2))


if __name__ == "__main__":
    main()
