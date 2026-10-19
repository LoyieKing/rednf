#!/bin/bash
# Count DWARF-attributed function symbols per remaining TSS CU.
cd "$(dirname "$0")"
count() {
  n=$(python3 cu_symbols.py /opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394 "$1" | grep -c '^0x')
  printf '%-45s %s\n' "$1" "$n"
}
count formmog_proto_tss_sdk_uic.cpp
count formmog_proto_tss_sdk_client.cpp
count formmog_proto_tss_sdk_qt.cpp
count formmog_proto_captcha_client.cpp
count formmog_proto_captcha_client_old.cpp
count formmog_proto_public_latransadapt.cpp
count formmog_proto_public_safemodetransadapt.cpp
count formmog_proto_public_uictransadapt.cpp
count formmog_conf_tsssdk_conf.cpp
count formmog_conf_tsssdk_conf_path.cpp
count formmog_conf_tsssdk_dp.cpp
count formmog_proto_antibot_client.cpp
count formmog_proto_public_secsvr.cpp
count formmog_proto_public_tsssdk.cpp
count formmog_conf_secagent.cpp
