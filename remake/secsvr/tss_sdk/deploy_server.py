#!/usr/bin/env python3
"""Replace only the chroot TSS SDK after strict relocation checks; support rollback."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

HERE = Path(__file__).resolve().parent
CHROOT = Path('/opt/dnf1031')
TARGET = CHROOT / 'home/template/neople/game/libtss_sdk.so.1.0.394'
ORIGINAL = HERE / 'reference/libtss_sdk.so.1.0.394.orig'
ORIGINAL_SHA256 = '8f6263dae6fcd1eb19baece6f7c9408d2161abd1a05bd492d398d5cb7783d2a7'
SERVICES = ('game_siroco11', 'game_siroco52')
S6 = '/package/admin/s6/command/'


def run(cmd, check=True):
    return subprocess.run(cmd, text=True, stdout=subprocess.PIPE,
                          stderr=subprocess.STDOUT, check=check)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def service(flag, name):
    return run(['chroot', str(CHROOT), S6 + 's6-svc', flag, '/run/service/' + name])


def snapshot():
    result = {'time_ns': time.time_ns(), 'library_sha256': digest(TARGET), 'processes': []}
    for proc in Path('/proc').iterdir():
        if not proc.name.isdigit():
            continue
        try:
            cmd = (proc / 'cmdline').read_bytes().replace(b'\0', b' ').decode(errors='replace')
            if 'df_game_r ' not in cmd:
                continue
            maps = (proc / 'maps').read_text()
            result['processes'].append({'pid': int(proc.name), 'command': cmd,
                                        'executable': os.readlink(proc / 'exe'),
                                        'sdk_maps': [line for line in maps.splitlines()
                                                     if 'libtss_' in line]})
        except (FileNotFoundError, PermissionError, ProcessLookupError):
            continue
    result['services'] = {name: run(['chroot', str(CHROOT), S6 + 's6-svstat',
                                     '/run/service/' + name], check=False).stdout
                          for name in SERVICES}
    return result


def install_image(source):
    temp = TARGET.with_name(TARGET.name + '.source-rebuild-new')
    with temp.open('xb') as handle:
        handle.write(source.read_bytes())
        handle.flush()
        os.fsync(handle.fileno())
    temp.chmod(TARGET.stat().st_mode & 0o777)
    os.replace(temp, TARGET)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rollback', action='store_true')
    parser.add_argument('--library', type=Path, default=HERE / 'build-server/libtss_sdk.so.1.0.394')
    parser.add_argument('--record-dir', type=Path, required=True)
    args = parser.parse_args()
    if os.geteuid() != 0:
        parser.error('Run as WSL root')
    if digest(ORIGINAL) != ORIGINAL_SHA256:
        raise RuntimeError('Original backup identity mismatch')
    source = ORIGINAL if args.rollback else args.library.resolve()
    if not source.is_file():
        raise RuntimeError('Missing library: ' + str(source))
    if not args.rollback:
        relocation = run(['ldd', '-r', str(source)], check=False)
        if relocation.returncode or 'undefined symbol:' in relocation.stdout or 'not found' in relocation.stdout:
            raise RuntimeError('Independent relocation failed:\n' + relocation.stdout)
        dynamic = run(['readelf', '-d', str(source)]).stdout
        if '[libtss_sdk.so.1.0.394]' not in dynamic:
            raise RuntimeError('SDK SONAME mismatch')
    args.record_dir.mkdir(parents=True, exist_ok=False)
    (args.record_dir / 'before.json').write_text(json.dumps(snapshot(), indent=2) + '\n')
    prior = args.record_dir / 'previous-library.so'
    prior.write_bytes(TARGET.read_bytes())
    prior.chmod(TARGET.stat().st_mode & 0o777)
    stopped = []
    try:
        for name in SERVICES:
            service('-d', name)
            stopped.append(name)
        deadline = time.monotonic() + 30
        while True:
            states = [run(['chroot', str(CHROOT), S6 + 's6-svstat',
                           '/run/service/' + name]).stdout for name in SERVICES]
            if all(state.startswith('down ') for state in states):
                break
            if time.monotonic() >= deadline:
                raise RuntimeError('Channels did not stop: ' + repr(states))
            time.sleep(0.25)
        # Check every process, since overwriting a shared library used elsewhere
        # must not silently leave additional consumers running the old image.
        for proc in Path('/proc').iterdir():
            if not proc.name.isdigit():
                continue
            try:
                if str(TARGET) in (proc / 'maps').read_text():
                    raise RuntimeError('SDK still mapped by pid ' + proc.name)
            except (FileNotFoundError, PermissionError, ProcessLookupError):
                continue
        install_image(source)
        try:
            # Validate against the deployed chroot runtime before starting either
            # game process; the host linker alone cannot establish this.
            check = run(['chroot', str(CHROOT), '/usr/bin/ldd', '-r',
                         '/home/template/neople/game/libtss_sdk.so.1.0.394'], check=False)
            (args.record_dir / 'chroot-relocations.log').write_text(check.stdout)
            if check.returncode or 'undefined symbol:' in check.stdout or 'not found' in check.stdout:
                raise RuntimeError('Chroot relocation failed: ' + check.stdout)
            (args.record_dir / 'installed.json').write_text(json.dumps({
                'source': str(source), 'target': str(TARGET), 'sha256': digest(TARGET),
                'rollback': args.rollback}, indent=2) + '\n')
        except BaseException:
            install_image(prior)
            raise
    finally:
        for name in stopped:
            service('-u', name)
    (args.record_dir / 'after_start.json').write_text(json.dumps(snapshot(), indent=2) + '\n')
    print('Installed sha256=' + digest(TARGET))
    print('Channels restarted; verify startup logs and actual process mappings before client acceptance.')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
