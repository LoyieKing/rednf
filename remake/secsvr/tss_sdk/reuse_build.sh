#!/bin/bash
# Trial-compile one reused secsvr CU with the c5root toolchain.
# Usage: reuse_build.sh <relative path under dnf_secsvr> <out.o> [extra -I/-D...]
R=/home/loyieking/dnf_workspace/dnf_secsvr
C5=/home/loyieking/dnf_workspace/dnf_remake/toolchains/c5root/usr/bin
src="$R/$1"; out="$2"; shift 2
exec "$C5/g++" -m32 -O2 -fPIC -fno-stack-protector -std=gnu++98 \
  -I "$R/source/secagent" \
  -I "$R/source/gunnersvr" \
  -I "$R/source/zergsvr" \
  -I "$R/source/secagent/output/commlib/zenlib/release/include" \
  -I "$R/source/secagent/output/commlib/framework/release/include" \
  -I "$R/source/secagent/output/protocol/common/release/include" \
  -I "$R/source/secagent/output/protocol/formmog/release/include" \
  -I "$R/source/secagent/output/formmog/appcomm/release/include" \
  -I "$R/source/secagent/include" \
  -I "$R/tencrypt_new/include" \
  -I "$R/third_party/import/include/opensource" \
  -I "$R/third_party/import/include/tencore" \
  "$@" -c "$src" -o "$out"
