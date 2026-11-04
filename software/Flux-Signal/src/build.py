#!/usr/bin/env python
"""
build.py  —  Flux ATtiny85 build script
Run from the sketch folder (e.g. software/Flux-Signal/).
Compiles with arduino-cli, then copies the .hex to:
  <repo>/releases/<sketch-name>.hex
  <org-root>/OS-software-tool/<sketch-name>.hex
"""

import os
import sys
import shutil
import time
from pathlib import Path

BOARD = 'ATTinyCore:avr:attinyx5:chip=85,clock=8internal'


def fail(msg):
    print(f'[FAIL] {msg}')
    sys.exit(1)

def info(msg):
    print(f'[INFO] {msg}')

def success(msg):
    print(f'[OK] {msg}')


def main():
    sketch_dir  = Path.cwd()                                        # …/OS-Flux/software/Flux-Signal
    sketch_name = sketch_dir.name                                   # Flux-Signal
    repo_root   = sketch_dir.parent.parent                         # …/OS-Flux
    org_root    = repo_root.parent                                  # …/Open-Source-Model-Railway-Electronics

    hex_src  = sketch_dir / 'build' / 'ATTinyCore.avr.attinyx5' / f'{sketch_name}.ino.hex'
    releases = repo_root  / 'releases'
    sw_tool  = org_root   / 'OS-software-tool'

    info(f'Building {sketch_name} ...')
    if os.system(f'arduino-cli compile -b {BOARD} -e') != 0:
        fail('Build failed!')

    if not hex_src.exists():
        fail(f'.hex not found: {hex_src}')

    releases.mkdir(exist_ok=True)
    dest = releases / f'{sketch_name}.hex'
    shutil.copy2(hex_src, dest)
    success(f'Copied → {dest}')

    if sw_tool.is_dir():
        shutil.copy2(hex_src, sw_tool / f'{sketch_name}.hex')
        success(f'Copied → {sw_tool / (sketch_name + ".hex")}')
    else:
        info('OS-software-tool folder not found, skipped.')

    time.sleep(1)   # let Windows release file locks before rmtree
    try:
        shutil.rmtree(sketch_dir / 'build')
        info('Build folder removed.')
    except Exception as e:
        info(f'Could not remove build folder: {e}')

    success('Done!')


if __name__ == '__main__':
    main()
