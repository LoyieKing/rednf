#!/bin/bash
# Dump the eight antibot callback bodies.
O=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
dump() {
  echo "######## $1 ($2+$3)"
  objdump -d -C --start-address=$2 --stop-address=$(( $2 + $3 )) "$O" \
    | awk 'f||/>:/{f=1;print}'
}
dump process_safemode_mba_set_verify_status_cb_ 0x88770 0x40
dump process_safemode_mba_set_verify_options_cb_ 0x887b0 0x48
dump process_safemode_mba_set_safe_status_cb_ 0x88800 0x39
dump process_safemode_set_limit_status_cb_ 0x88840 0x4e
dump process_safemode_msg_cb_ 0x88890 0x55
dump process_safemode_cb_ 0x888f0 0x82
dump send_buffer_to_client_cb_ 0x89120 0x140
dump punish_user_cb_ 0x89260 0x1eb
