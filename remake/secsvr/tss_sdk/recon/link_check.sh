#!/bin/sh
# Compile every reconstructed TSS SDK TU and combine them with ld -r.
# This is a symbol/link-shape check, not a full libtss_sdk link: external
# ZEN/plugin/tencrypt symbols intentionally remain undefined.
set -e
cd "$(dirname "$0")"
OUT=${1:-/tmp/tss_recon_all.o}
OBJS=""
for f in \
  src/tdirty/oidirty.cpp \
  src/tdirty/oidirtyinterface.cpp \
  src/tdirty/tdirtyapi.cpp \
  src/tdirty/charactermap.cpp \
  src/tdirty/convert.c \
  src/tdirty/md5.c \
  src/tdirty/tagentapi.c \
  src/VMCore/cryptdata.cpp \
  src/VMCore/common_proc.cpp \
  src/VMCore/codeengine.cpp \
  src/VMCore/vm.cpp \
  src/VMCore/upvm.cpp \
  src/VMCore/server_crypt_code_mgr.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_ctor_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_init.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_bill.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_core.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_lifecycle_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_buffer_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_anticheck_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_onrecv_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dpinterface_reload_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_core_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_buffer_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_send_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_msg_handler_recv_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_msg_packer_core_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_msg_packer_templates_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_vmbase_engine_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_bill_monitor_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_code_mgr_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_comm_svc_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_comm_svc_templates_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_hack_cmp_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/dp_hack_mgr_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_plugin_mgr_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/user_mgr_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_framework_lifecycle_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_framework_dispatch_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/captcha_plugin_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/safemode_plugin_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/datatrans_plugin_agent.cpp \
  src/formmog/tss_sdk/plugins/channel_plugin_agent.cpp \
  src/formmog/tss_sdk/plugins/usercontrol_plugin_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc_plugin_agent.cpp \
  src/formmog/tss_sdk/plugins/uic_plugin_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/hackers_a_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/hackers_b_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/hackers_c_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_exports_timer_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/bill_cf_hackers_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/property_trans_impl_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/channel_misc_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/hacker_base_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/uic_manager_agent.cpp \
  src/formmog/tss_sdk/plugins/proto_proc/tss_sdk_export_api_agent.cpp \
  src/commlib/framework/comm_socket_channel.cpp \
  src/commlib/framework/comm_socket_channel_tcp_handler.cpp \
  src/commlib/framework/comm_socket_channel_bus.cpp \
  src/commlib/framework/comm_socket_channel_info.cpp \
  src/emission/crypto_static_data.cpp \
  src/emission/comm_app_frame_instances.cpp \
  src/emission/zen_hashtable_prime_instances.cpp \
  src/emission/zen_string_util_hex.cpp \
  src/commlib/comm_buddy_alloc.cpp \
  src/commlib/framework/comm_cfgsvr_sdk.cpp \
  src/commlib/framework/comm_socket_channel_base_handler.cpp \
  src/commlib/zenlib/zen_os_adapt_rwlock.cpp \
  src/commlib/zenlib/zen_lock_file_lock.cpp \
  src/commlib/zenlib/zen_lock_thread_rw_mutex.cpp \
  src/commlib/zenlib/zen_os_adapt_dirent.cpp \
  src/commlib/zenlib/zen_server_status.cpp \
  src/commlib/zenlib/zen_thread_error_info.cpp \
  src/commlib/zenlib/zen_base64.cpp \
  src/commlib/zenlib/zen_event_reactor_base.cpp \
  src/commlib/zenlib/zen_event_reactor_epoll.cpp \
  src/appcomm/app_bus_channel_id_mgr.cpp \
  src/formmog/tss_sdk/adapter/antibot_callbacks_agent.cpp \
  src/formmog/tss_sdk/adapter/antibotinterface_trivial.cpp \
  src/formmog/tss_sdk/adapter/antibotinterface_ctor_agent.cpp \
  src/formmog/tss_sdk/adapter/antibot_client_agent.cpp \
  src/formmog/tss_sdk/adapter/antibot_packets_agent.cpp \
  src/formmog/tss_sdk/busi_interf/busi_impl/tss_sdk_busi_impl.cpp \
  src/formmog/tss_sdk/framework/tss_sdk_predefine.cpp \
  src/formmog/tss_sdk/adapter/antibot_policy_agent.cpp; do
  o="/tmp/recon_$(echo "$f" | tr '/' '_').o"
  extra=""
  case "$f" in
    # GCC >= 4.6 no longer emits out-of-line copies of inline functions that
    # are only reached through inlined calls, and it leaves the destructor
    # slots of abstract-class vtables empty, so the weak copies that GCC 4.1
    # emitted for the ORIG (`tss_dpsdk::IDPHacker::{init,on_logout,proc,
    # on_special_control,~IDPHacker}`, `IDPHackerSafe::~IDPHackerSafe`,
    # `DPPkgDelayHacker::~DPPkgDelayHacker`) are missing.  These two TUs carry
    # the original header bodies (dp_hacker_if.h / dp_hacker_safe_if.h /
    # dp_pkg_delay_hacker.h) and the CF hackers whose vtables reference them,
    # so they are built with -fkeep-inline-functions to reproduce the ORIG's
    # weak copies.  See AGENT_REPORT_weak_slots.md.
    *bill_cf_hackers_agent.cpp|*hacker_base_agent.cpp)
      extra="-fkeep-inline-functions" ;;
  esac
  g++ -m32 -std=gnu++98 -O2 -fPIC -fno-implement-inlines -fno-stack-protector \
      -D_GLIBCXX_USE_CXX11_ABI=0 -I . -I include \
      -I /home/loyieking/dnf_workspace/dnf_secsvr/source/secagent \
      $extra -c "$f" -o "$o"
  OBJS="$OBJS $o"
done
g++ -m32 -r $OBJS -o "$OUT"
echo "wrote $OUT"
nm -C --defined-only "$OUT" | grep -E ' (T|W) (.*::)?(DPInterface|AntibotInterface|DPMsgHandler|DPMsgPacker|DPVMBaseEngine|DPBillAdapter|DPMonitorReporter|DPCodeMgr|DPHackMgr|TssSdkCommSvc|TssSdkPluginMgr|UserMgr|TssSdkFramework|CCryptData)::' | sort
