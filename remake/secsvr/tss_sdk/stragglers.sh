#!/bin/bash
# Show the missing-symbol examples for a fixed set of single-straggler CUs.
cd "$(dirname "$0")"
show() {
  echo "### $1"
  python3 missing_by_cu.py /opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394 \
    /tmp/fl_missing.txt --detail "$1" 2>&1 | grep -A6 "examples for"
}
show proto_proc_plugin.cpp
show uic_user_input_info.cpp
show user_mgr.cpp
show comm_bill_record.cpp
