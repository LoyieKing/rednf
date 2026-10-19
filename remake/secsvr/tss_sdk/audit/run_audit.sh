#!/bin/bash
# Rebuild the DWARF variant and regenerate every audit report.
# Usage: bash audit/run_audit.sh [--no-build]
set -e
cd "$(dirname "$0")/.."
if [ "$1" != "--no-build" ]; then
  python3 build_server.py --debug --output-dir build-server-dbg --jobs 8
fi
python3 audit/dwarf_model.py reference/libtss_sdk.so.1.0.394.orig \
  /tmp/orig_model.json
python3 audit/dwarf_model.py build-server-dbg/libtss_sdk.so.1.0.394 \
  /tmp/our_model.json
python3 audit/diff_model.py /tmp/orig_model.json /tmp/our_model.json \
  audit/report/diff.json audit/report/diff.tsv
python3 audit/static_init.py reference/libtss_sdk.so.1.0.394.orig \
  audit/report/init_orig.json /tmp/orig_model.json >/tmp/init_orig.txt
python3 audit/static_init.py build-server-dbg/libtss_sdk.so.1.0.394 \
  audit/report/init_ours.json /tmp/our_model.json >/tmp/init_ours.txt
python3 audit/var_diff.py /tmp/orig_model.json /tmp/our_model.json \
  audit/report/diff.json audit/report/var_diff.tsv
python3 audit/summarize.py audit/report
