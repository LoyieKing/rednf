#!/usr/bin/env python3
"""Build the server SDK from source; reject unresolved application symbols."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
import os
from pathlib import Path
import re
import shlex
import subprocess

ROOT = Path(__file__).resolve().parent
SECSVR = ROOT.parent
C5_CANDIDATES = (
    SECSVR.parent / 'dnf_remake/toolchains/c5root',  # dnf_workspace
    SECSVR.parent.parent / 'toolchains/c5root',      # rednf publish tree
)
C5 = next((p for p in C5_CANDIDATES if p.exists()), C5_CANDIDATES[0])

def resolve_include_root(root):
    """Map recorded dnf_workspace absolute roots onto the current layout."""
    path = Path(root)
    if path.exists():
        return path
    prefixes = (
        ('/dnf_workspace/dnf_secsvr/', SECSVR),
        ('/dnf_workspace/dnf_remake/toolchains/', C5.parent),
    )
    for prefix, base in prefixes:
        if root.startswith(prefix):
            candidate = base / root[len(prefix):]
            if candidate.exists():
                return candidate
    return path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=8)
    parser.add_argument('--output-dir', type=Path, default=ROOT / 'build-server')
    parser.add_argument('--debug', action='store_true',
                        help='add -g (DWARF) for layout/signature audits')
    args = parser.parse_args()
    out = args.output_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)
    records = []
    jobs = []
    debug_flags = ['-g'] if args.debug else []
    host_flags = ['-m32', '-std=gnu++98', '-O2', '-fPIC',
                  '-fno-implement-inlines', '-fno-stack-protector',
                  '-D_GLIBCXX_USE_CXX11_ABI=0', *debug_flags,
                  '-I', str(ROOT / 'recon'),
                  '-I', str(ROOT / 'recon/include'),
                  '-I', str(SECSVR / 'source/secagent')]
    vendor_flags = ['-m32', '-std=gnu++98', '-O2', '-fPIC',
                    '-fno-stack-protector', *debug_flags]
    vendor_env = dict(os.environ)
    vendor_env['PATH'] = str(C5 / 'usr/bin') + ':' + vendor_env.get('PATH', '')
    vendor_env['LD_LIBRARY_PATH'] = ':'.join([str(C5 / 'usr/lib'), str(C5 / 'lib')])

    def add(kind, src, compiler, flags, env=None):
        src = src.resolve()
        obj = out / (kind + '__' + str(src.relative_to(SECSVR)).replace('/', '__') + '.o')
        jobs.append((kind, src, obj, [compiler, *flags, '-c', str(src), '-o', str(obj)], env))

    script = (ROOT / 'recon/link_check.sh').read_text()
    match = re.search(r'^for f in \\\n(.*?); do', script, re.M | re.S)
    if not match:
        raise RuntimeError('Cannot find reconstructed source manifest')
    for name in match.group(1).replace('\\\n', ' ').split():
        flags = list(host_flags)
        if Path(name).name in ('bill_cf_hackers_agent.cpp', 'hacker_base_agent.cpp'):
            flags.append('-fkeep-inline-functions')
        add('recon', ROOT / 'recon' / name, 'g++', flags)

    overrides = [x.strip() for x in (ROOT / 'reuse/OVERRIDES.txt').read_text().splitlines()
                 if x.strip() and not x.lstrip().startswith('#')]
    for record in sorted((ROOT / 'reuse/log').glob('*.res')):
        row = dict(line.split('=', 1) for line in record.read_text().splitlines() if '=' in line)
        if row.get('compiled') != '1':
            # Failed candidates are not part of the accepted reuse selection.
            continue
        if any(key in record.stem for key in overrides):
            continue
        roots_file = ROOT / 'reuse/log' / ('_' + ('min' if row['roots'] == 'minimized' else 'full')
                                          + '_' + row['tree'].replace('/', '_') + '.roots')
        includes = []
        for line in roots_file.read_text().splitlines():
            line = line.strip()
            if line and not line.startswith('#'):
                includes.extend(['-I', str(resolve_include_root(line))])
        add('reuse', SECSVR / row['candidate'], str(C5 / 'usr/bin/g++'),
            [*vendor_flags, *shlex.split(row.get('flags', '')), *includes], vendor_env)

    for line in (ROOT / 'recon/vendor_only_tus.txt').read_text().splitlines():
        name = line.strip()
        if name and not name.startswith('#'):
            add('vendor', ROOT / 'recon' / name, str(C5 / 'usr/bin/g++'),
                [*vendor_flags, '-fno-implement-inlines', '-I', str(ROOT / 'recon')], vendor_env)
    for src in sorted((ROOT / 'recon/src/origsrc').glob('*.cpp')):
        if not src.name.startswith('test_'):
            add('generated', src, str(C5 / 'usr/bin/g++'),
                [*vendor_flags, '-fno-implement-inlines', '-I', str(ROOT / 'recon'),
                 '-I', str(src.parent)], vendor_env)
    for src in sorted((ROOT / 'recon/src/closing').glob('*.cpp')):
        if not src.name.startswith('test_'):
            add('closing', src, 'g++', host_flags)

    def compile_one(job):
        kind, src, obj, cmd, env = job
        result = subprocess.run(cmd, cwd=ROOT / 'recon', env=env,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        log = obj.with_suffix('.log')
        log.write_text(result.stdout)
        return {'kind': kind, 'source': str(src), 'object': str(obj),
                'command': cmd, 'returncode': result.returncode, 'log': str(log)}

    with ThreadPoolExecutor(max_workers=max(1, args.jobs)) as pool:
        records = list(pool.map(compile_one, jobs))
    (out / 'compile_commands_record.json').write_text(json.dumps(records, indent=2) + '\n')
    failed = [r for r in records if r['returncode']]
    print('compiled=%d failed=%d' % (len(records) - len(failed), len(failed)))
    if failed:
        for row in failed:
            print('FAILED', row['source'], 'log=' + row['log'])
        return 1
    library = out / 'libtss_sdk.so.1.0.394'
    cmd = ['g++', '-m32', '-shared', '-Wl,-z,defs',
           '-Wl,-soname,libtss_sdk.so.1.0.394', '-o', str(library),
           *[r['object'] for r in records], '-pthread', '-lrt', '-ldl']
    result = subprocess.run(cmd, cwd=ROOT, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, text=True)
    (out / 'link.log').write_text(result.stdout)
    (out / 'link_command.json').write_text(json.dumps(cmd, indent=2) + '\n')
    print('link_returncode=%d log=%s' % (result.returncode, out / 'link.log'))
    if result.returncode:
        print(result.stdout)
        return result.returncode
    print('library=' + str(library))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
