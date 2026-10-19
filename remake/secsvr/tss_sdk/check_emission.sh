#!/bin/bash
# Host-GCC syntax check of the emission TUs.
cd /mnt/d/Docs/dnf_client_test/dnf_client_remake/tools/tss_sdk_recon/recon
for f in \
  src/emission/crypto_static_data.cpp \
  src/emission/comm_app_frame_instances.cpp \
  src/emission/zen_hashtable_prime_instances.cpp \
  src/emission/zen_string_util_hex.cpp; do
  printf '%-55s ' "$f"
  if g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
      -D_GLIBCXX_USE_CXX11_ABI=0 -I . -I include -I src/emission \
      -fsyntax-only "$f" 2>/tmp/em.err; then
    echo OK
  else
    echo FAIL
    grep -m1 "error:" /tmp/em.err
  fi
done
