#!/bin/bash
# Verify every origsrc CU against its ORIG CU symbol set.
cd "$(dirname "$0")"
check() {
  echo "######## $1"
  o="/tmp/v_$(basename "$2" .cpp).o"
  if sh c5build.sh "$2" "$o" -I recon/src/origsrc >/tmp/v.log 2>&1; then
    bash cu_match.sh "$1" "$o" | tail -3
  else
    echo "COMPILE FAIL"; grep -m2 "error:" /tmp/v.log
  fi
}
check formmog_proto_tss_sdk_uic.cpp recon/src/origsrc/formmog_proto_tss_sdk_uic.cpp
check formmog_proto_tss_sdk_client.cpp recon/src/origsrc/formmog_proto_tss_sdk_client.cpp
check formmog_proto_tss_sdk_qt.cpp recon/src/origsrc/formmog_proto_tss_sdk_qt.cpp
check formmog_proto_public_uictransadapt.cpp recon/src/origsrc/formmog_proto_public_uictransadapt.cpp
check formmog_proto_public_latransadapt.cpp recon/src/origsrc/formmog_proto_public_latransadapt.cpp
check formmog_proto_public_safemodetransadapt.cpp recon/src/origsrc/formmog_proto_public_safemodetransadapt.cpp
check formmog_proto_captcha_client.cpp recon/src/origsrc/formmog_proto_captcha_client.cpp
check formmog_proto_captcha_client_old.cpp recon/src/origsrc/formmog_proto_captcha_client_old.cpp
check formmog_conf_tsssdk_conf.cpp recon/src/origsrc/formmog_conf_tsssdk_conf.cpp
check formmog_conf_tsssdk_dp.cpp recon/src/origsrc/formmog_conf_tsssdk_dp.cpp
check formmog_conf_tsssdk_conf_path.cpp recon/src/origsrc/formmog_conf_tsssdk_conf_path.cpp
check formmog_proto_public_secsvr.cpp recon/src/origsrc/formmog_proto_public_secsvr_extra.cpp
