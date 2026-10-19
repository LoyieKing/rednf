#!/bin/bash
# Build and run the reconstructed-vs-ORIG differential oracle for the captcha
# and safe-mode-adapt CUs:
#   formmog_proto_captcha_client.cpp
#   formmog_proto_captcha_client_old.cpp
#   formmog_proto_public_safemodetransadapt.cpp
# (pack/unpack/visualize over all shipped types, all recovered union ids).
cd "$(dirname "$0")"
set -e
sh c5build.sh recon/src/origsrc/formmog_proto_captcha_client.cpp \
  /tmp/captcha_new_proto.o -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/formmog_proto_captcha_client_old.cpp \
  /tmp/captcha_old_proto.o -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/formmog_proto_public_safemodetransadapt.cpp \
  /tmp/captcha_safe_proto.o -I recon/src/origsrc
# -Bsymbolic keeps the reconstructed objects' internal cross-function calls
# bound to their own definitions: with the ORIG dlopen()ed RTLD_GLOBAL, the
# global scope is searched first and would otherwise preempt them (making the
# oracle vacuous for everything behind a wrapper).  Undefined runtime symbols
# still bind to the ORIG (RTLD_GLOBAL) / libc / libstdc++.
g++ -m32 -shared -Wl,--allow-multiple-definition -Wl,-Bsymbolic \
  -o /tmp/libcaptcha_recon.so \
  /tmp/captcha_new_proto.o /tmp/captcha_old_proto.o /tmp/captcha_safe_proto.o
g++ -m32 /tmp/captcha_oracle.o -ldl -o /tmp/captcha_oracle
/tmp/captcha_oracle
