#!/bin/bash
# Build and run the reconstructed-vs-ORIG differential oracle for
# formmog_proto_tss_sdk_uic.cpp (pack/unpack/visualize over all shipped types).
cd "$(dirname "$0")"
set -e
sh c5build.sh recon/src/origsrc/formmog_proto_tss_sdk_uic.cpp /tmp/uic_proto.o \
  -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/test_uic_oracle.cpp /tmp/uic_oracle.o \
  -I recon/src/origsrc
g++ -m32 -shared -Wl,-Bsymbolic -Wl,--allow-multiple-definition -o /tmp/libuic_recon.so \
  /tmp/uic_proto.o
g++ -m32 /tmp/uic_oracle.o -ldl -o /tmp/uic_oracle
/tmp/uic_oracle
