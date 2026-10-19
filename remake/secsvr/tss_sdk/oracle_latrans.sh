#!/bin/bash
# Build and run the reconstructed-vs-ORIG differential oracle for
# formmog_proto_public_latransadapt.cpp (82 symbols) and the 45 supplemental
# formmog_proto_public_secsvr symbols (formmog_proto_public_secsvr_extra.cpp).
# Mirrors oracle_uic.sh.
cd "$(dirname "$0")"
set -e
sh c5build.sh recon/src/origsrc/formmog_proto_public_latransadapt.cpp \
  /tmp/latrans_proto.o -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/formmog_proto_public_secsvr_extra.cpp \
  /tmp/latrans_svx.o -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/test_latrans_oracle.cpp \
  /tmp/latrans_oracle.o -I recon/src/origsrc
# -Bsymbolic is required: without it the recon library's internal calls
# (pack(char*) -> pack(TdrWriteBuf&), visualize -> printVariable/printArray)
# would be preempted by the ORIG copies resident in the global scope, which
# would make the oracle vacuous for the core overloads.  Undefined runtime
# symbols (TdrBufUtil::*) still bind to ORIG at dlopen time.
g++ -m32 -shared -Wl,--allow-multiple-definition -Wl,-Bsymbolic \
  -o /tmp/liblatrans_recon.so /tmp/latrans_proto.o /tmp/latrans_svx.o
g++ -m32 /tmp/latrans_oracle.o -ldl -o /tmp/latrans_oracle
/tmp/latrans_oracle
