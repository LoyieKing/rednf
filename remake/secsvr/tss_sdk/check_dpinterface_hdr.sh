#!/bin/bash
# Syntax-check every TU that includes the generated dpinterface.h.
cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon/recon
for f in \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_anticheck_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_bill.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_buffer_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_core.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_init.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_lifecycle_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_onrecv_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_reload_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_comm_svc_agent.cpp; do
  printf '%-85s ' "$f"
  if g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
      -D_GLIBCXX_USE_CXX11_ABI=0 -I . -I include -fsyntax-only "$f" 2>/tmp/hdr.err; then
    echo OK
  else
    echo FAIL
    grep -m2 "error:" /tmp/hdr.err
  fi
done
