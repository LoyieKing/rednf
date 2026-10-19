#!/bin/bash
# Build and run the reconstructed-vs-ORIG differential oracle for
# formmog_proto_tss_sdk_client.cpp (pack/unpack/visualize over all 29 classes,
# every DPInterData / TClientPkg / TClientFrame union arm).
#
# ORIG is dlopen()ed RTLD_GLOBAL (provides the tsf4g_tdr runtime and the
# reference methods); the reconstructed object is linked into a .so and
# dlopen()ed RTLD_LOCAL, so both implementations can be called side by side.
cd "$(dirname "$0")"
set -e
sh c5build.sh recon/src/origsrc/formmog_proto_tss_sdk_client.cpp /tmp/client_proto.o \
  -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/test_client_oracle.cpp /tmp/client_oracle.o \
  -I recon/src/origsrc
# -Bsymbolic is required for a *meaningful* comparison: the ORIG is loaded
# RTLD_GLOBAL, so without it the reconstructed .so's internal calls (e.g.
# ClientPkgHead::pack -> TdrWriteBuf&) would be preempted by the ORIG's copies
# and the oracle would compare the ORIG against itself.  Symbols the .so does
# not define (TdrBufUtil, memmove, ...) still resolve to the ORIG runtime.
# Negative control: mutating one pack body makes this oracle report 368
# failures; without -Bsymbolic the same mutant reports 0.
g++ -m32 -shared -Wl,--allow-multiple-definition -Wl,-Bsymbolic \
  -o /tmp/libclient_recon.so /tmp/client_proto.o
g++ -m32 /tmp/client_oracle.o -ldl -o /tmp/client_oracle
/tmp/client_oracle
