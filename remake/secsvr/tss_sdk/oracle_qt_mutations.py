#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Negative control for oracle_qt.sh.

Applies one deliberate mutation at a time to *copies* of the reconstruction
(never the production files), rebuilds /tmp/libqt_recon.so, runs the oracle and
prints cases/failures plus the first FAIL labels.  A mutation that the oracle
does not catch would mean the harness (or that path) is not actually compared.

Usage: negctl.py
"""
import os
import shutil
import subprocess
import sys

ROOT = "/mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon"
WORK = "/tmp/negctl"
QT = "formmog_proto_tss_sdk_qt.cpp"
UICTA = "formmog_proto_public_uictransadapt.cpp"

MUTATIONS = [
    (
        "body-arm8-construct (union selector 8 -> arm 9)",
        QT,
        "        ret = role_action.construct();",
        "        ret = room_master.construct();",
        "Body",
    ),
    (
        "uicta-cmdbuf-bound (32768 -> 32767, REFER_SURPASS boundary)",
        UICTA,
        "if (cmd_buf_len_ > 32768) {",
        "if (cmd_buf_len_ > 32767) {",
        "UICTransAdapt",
    ),
    (
        "login-vis-format (\"[uin]\" -> \"[uinX]\")",
        QT,
        '"[uin]", "%u"',
        '"[uinX]", "%u"',
        "Login",
    ),
    (
        "changeposition-pack-order (role_position_/role_type_ swapped)",
        QT,
        "    ret = destBuf.writeUInt8(role_position_);\n"
        "    if (ret != TdrError::TDR_NO_ERROR) return ret;\n"
        "    ret = destBuf.writeUInt8(role_type_);",
        "    ret = destBuf.writeUInt8(role_type_);\n"
        "    if (ret != TdrError::TDR_NO_ERROR) return ret;\n"
        "    ret = destBuf.writeUInt8(role_position_);",
        "ChangePosition",
    ),
    (
        "login-unpack-order (uin_/server_ip_ swapped)",
        QT,
        "    ret = srcBuf.readUInt32(uin_);\n"
        "    if (ret != TdrError::TDR_NO_ERROR) return ret;\n"
        "    ret = srcBuf.readUInt32(server_ip_);",
        "    ret = srcBuf.readUInt32(server_ip_);\n"
        "    if (ret != TdrError::TDR_NO_ERROR) return ret;\n"
        "    ret = srcBuf.readUInt32(uin_);",
        "Login",
    ),
    (
        "roleaction-vis-format (\"%u\" -> \"%d\" for revert_)",
        QT,
        '"[revert]",\n                                    "%u"',
        '"[revert]",\n                                    "%d"',
        "RoleAction",
    ),
]


def run(cmd, **kw):
    p = subprocess.run(cmd, capture_output=True, cwd=ROOT, **kw)
    return p.returncode, p.stdout.decode("utf-8", "replace") + \
        p.stderr.decode("utf-8", "replace")


def prepare():
    if os.path.isdir(WORK):
        shutil.rmtree(WORK)
    os.makedirs(WORK)
    for f in (QT, UICTA, "formmog_proto_tss_sdk_qt.h",
              "formmog_proto_public_uictransadapt.h", "tsf4g_tdr_compat.h"):
        shutil.copy(os.path.join(ROOT, "recon/src/origsrc", f), WORK)


def build_mutated(name, path, old, new):
    src = open(os.path.join(WORK, path)).read()
    if src.count(old) < 1:
        print("  !! mutation %s: anchor not found" % name)
        return False
    open(os.path.join(WORK, path), "w").write(src.replace(old, new))
    return True


def build_and_run():
    rc, out = run(["sh", "c5build.sh", WORK + "/" + QT, "/tmp/neg_qt.o",
                   "-I", WORK])
    if rc:
        return rc, out
    rc, out = run(["sh", "c5build.sh", WORK + "/" + UICTA, "/tmp/neg_uicta.o",
                   "-I", WORK])
    if rc:
        return rc, out
    rc, out = run(["g++", "-m32", "-shared", "-Wl,-Bsymbolic",
                   "-Wl,--allow-multiple-definition",
                   "-o", "/tmp/libqt_recon.so", "/tmp/neg_qt.o",
                   "/tmp/neg_uicta.o"])
    if rc:
        return rc, out
    return run(["/tmp/qt_oracle"])


def main():
    prepare()
    # Baseline: unmutated copies must still pass.
    rc, out = build_and_run()
    line = [l for l in out.splitlines() if l.startswith("cases=")]
    print("baseline (unmutated copies): %s rc=%d" % (line[-1] if line else "?", rc))
    if not line or not line[-1].endswith("failures=0"):
        print(out[-2000:])
        return 1
    bad = 0
    for name, path, old, new, want_label in MUTATIONS:
        prepare()
        if not build_mutated(name, path, old, new):
            bad += 1
            continue
        rc, out = build_and_run()
        line = [l for l in out.splitlines() if l.startswith("cases=")]
        fails = [l for l in out.splitlines() if l.startswith("FAIL")]
        print("%-62s %s rc=%d" % (name, line[-1] if line else "NO SUMMARY", rc))
        if not line or line[-1].endswith("failures=0"):
            print("  !! NOT CAUGHT")
            bad += 1
        elif not any(want_label in f for f in fails):
            print("      !! caught, but no FAIL line names %s" % want_label)
            bad += 1
        else:
            print("      first fail: %s" % (fails[0][:120] if fails else "?"))
    print("mutations_not_caught=%d of %d" % (bad, len(MUTATIONS)))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
