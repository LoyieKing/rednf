#!/bin/bash
# Match reused secsvr CUs against ORIG TSS CU symbol sets.
cd "$(dirname "$0")"
check() {
  echo "######## $1  <->  $2"
  if sh reuse_build.sh "$2" /tmp/cu.o 2>/tmp/cu.err; then
    bash cu_match.sh "$1" /tmp/cu.o | tail -n 20
  else
    echo "COMPILE FAIL"; grep -m2 "error:" /tmp/cu.err
  fi
}
check formmog_proto_antibot_client.cpp source/secagent/src/protocol/formmog/formmog_proto_antibot_client.cpp
check formmog_proto_public_secsvr.cpp source/secagent/src/protocol/formmog/formmog_proto_public_secsvr.cpp
check TdrIO.cpp source/secagent/src/protocol/common/TdrIO.cpp
check TdrXml.cpp source/secagent/src/protocol/common/TdrXml.cpp
check zen_time_value.cpp source/secagent/src/commlib/zenlib/zen_time_value.cpp
check comm_app_frame.cpp source/secagent/src/commlib/framework/comm_app_frame.cpp
