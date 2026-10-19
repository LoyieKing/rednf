#!/bin/bash
# Build and run the reconstructed-vs-ORIG differential oracle for the two
# GenProto CUs of this task:
#   formmog_proto_tss_sdk_qt.cpp              (106 functions)
#   formmog_proto_public_uictransadapt.cpp    (9 functions)
#
# Both reconstructed objects are linked into one RTLD_LOCAL shared object
# (--allow-multiple-definition covers the weak tsf4g_tdr::TdrWriteBuf::textize
# copy each of them emits, exactly as the ORIG's own linker folds them); the
# ORIG is dlopen()ed RTLD_GLOBAL first so the reconstructed object's TdrBufUtil
# references bind to the ORIG runtime.
#
# -Bsymbolic is required: without it the reconstructed object's *internal*
# calls (pack(char*) -> pack(TdrWriteBuf&), Dp2QtMsg::pack -> body_.pack, ...)
# are preempted by the ORIG's RTLD_GLOBAL copies, which would make those
# wrapped paths vacuous.  Undefined runtime symbols (TdrBufUtil::print*,
# TdrWriteBuf::vtextize) still resolve to the ORIG.
cd "$(dirname "$0")"
set -e
sh c5build.sh recon/src/origsrc/formmog_proto_tss_sdk_qt.cpp /tmp/qt_proto.o \
  -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/formmog_proto_public_uictransadapt.cpp \
  /tmp/uicta_proto.o -I recon/src/origsrc
sh c5build.sh recon/src/origsrc/test_qt_oracle.cpp /tmp/qt_oracle.o \
  -I recon/src/origsrc
g++ -m32 -shared -Wl,-Bsymbolic -Wl,--allow-multiple-definition \
  -o /tmp/libqt_recon.so /tmp/qt_proto.o /tmp/uicta_proto.o
g++ -m32 /tmp/qt_oracle.o -ldl -o /tmp/qt_oracle
/tmp/qt_oracle
