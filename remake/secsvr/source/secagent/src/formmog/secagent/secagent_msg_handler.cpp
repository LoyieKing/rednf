// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/formmog/secagent/secagent_msg_handler.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/formmog/secagent/secagent_predefine.h"
#include "output/commlib/zenlib/release/include/zen_predefine.h"
#include "import/include/opensource/rapidxml/rapidxml/rapidxml.hpp"
#include "import/include/opensource/rapidxml/rapidxml/rapidxml_utils.hpp"
#include "import/include/opensource/rapidxml/rapidxml/rapidxml_print.hpp"
#include "import/include/opensource/mysqlclient/mysql.h"
#include "import/include/opensource/mysqlclient/mysql_version.h"
#include "import/include/opensource/mysqlclient/mysql_com.h"
#include "import/include/opensource/mysqlclient/mysql_time.h"
#include "import/include/opensource/mysqlclient/typelib.h"
#include "import/include/opensource/mysqlclient/my_alloc.h"
#include "import/include/opensource/mysqlclient/my_list.h"
#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_shm_hash_expire.h"
#include "output/commlib/zenlib/release/include/zen_shm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_posix.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include "output/commlib/zenlib/release/include/zen_crc32.h"
#include "output/commlib/zenlib/release/include/zen_digit_list.h"
#include "output/commlib/zenlib/release/include/zen_key_value_config.h"
#include "output/commlib/zenlib/release/include/zen_data_with_version.h"
#include "output/commlib/zenlib/release/include/zen_code_engine.h"
#include "output/commlib/framework/release/include/comm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_id_to_string.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_time.h"
#include "output/commlib/zenlib/release/include/zen_mysql_connect.h"
#include "output/commlib/zenlib/release/include/zen_mysql_predefine.h"
#include "output/commlib/zenlib/release/include/zen_mysql_command.h"
#include "output/commlib/zenlib/release/include/zen_mysql_result.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_string.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_debug.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_mutex.h"
#include "output/commlib/zenlib/release/include/zen_lock_base.h"
#include "output/commlib/zenlib/release/include/zen_lock_guard.h"
#include "output/commlib/zenlib/release/include/zen_mysql_field.h"
#include "output/commlib/zenlib/release/include/zen_mysql_process.h"
#include "output/commlib/zenlib/release/include/zen_shm_cache_chunk.h"
#include "output/commlib/zenlib/release/include/zen_shm_lockfree_deque.h"
#include "output/commlib/zenlib/release/include/zen_server_toolkit.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_process.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_sysinfo.h"
#include "output/commlib/zenlib/release/include/zen_shm_hash_table.h"
#include "output/commlib/zenlib/release/include/zen_boost_lord_rings.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_thread.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_socket.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_error.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_getopt.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_file.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_dirent.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include "output/commlib/zenlib/release/include/zen_lock_null_lock.h"
#include "output/commlib/zenlib/release/include/zen_config_property_tree.h"
#include "output/commlib/zenlib/release/include/zen_config_ini_implement.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_sema.h"
#include "output/commlib/zenlib/release/include/zen_lock_synch_traits.h"
#include "output/commlib/zenlib/release/include/zen_lock_process_mutex.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_rw_mutex.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_semaphore.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_condi.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_condi.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_template.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_nonlock.h"
#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_base.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_wheel.h"
#include "output/commlib/zenlib/release/include/zen_bus_two_way.h"
#include "output/commlib/zenlib/release/include/zen_bus_mmap_pipe.h"
#include "output/commlib/zenlib/release/include/zen_thread_task.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_base.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include "output/commlib/zenlib/release/include/zen_socket_base.h"
#include "output/commlib/zenlib/release/include/zen_socket_stream.h"
#include "output/commlib/zenlib/release/include/zen_socket_datagram.h"
#include "output/commlib/zenlib/release/include/zen_socket_connector.h"
#include "output/commlib/zenlib/release/include/zen_event_handle_base.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_base.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_select.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_epoll.h"
#include "output/commlib/zenlib/release/include/zen_thread_wait_mgr.h"
#include "output/commlib/zenlib/release/include/zen_config_getopt.h"
#include "output/commlib/zenlib/release/include/zen_string_util.h"
#include "output/commlib/zenlib/release/include/zen_server_status.h"
#include "output/commlib/zenlib/release/include/zen_shm_vector.h"
#include "output/commlib/zenlib/release/include/zen_thread_bus_pipe.h"
#include "output/protocol/common/release/include/comm_proto_cfgsvr.h"
#include "output/protocol/common/release/include/TdrBuf.h"
#include "output/protocol/common/release/include/TdrPal.h"
#include "output/protocol/common/release/include/TdrError.h"
#include "output/protocol/common/release/include/TdrBufUtil.h"
#include "output/protocol/common/release/include/TdrTypeUtil.h"
#include "output/protocol/common/release/include/TdrTime.h"
#include "output/protocol/common/release/include/comm_proto_public_head.h"
#include "output/protocol/common/release/include/comm_proto_public_cmd.h"
#include "output/protocol/common/release/include/comm_proto_public_cfgsvr.h"
#include "output/protocol/common/release/include/comm_proto_logsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_logsvr.h"
#include "output/protocol/common/release/include/comm_proto_monitorsvr.h"
#include "output/protocol/common/release/include/comm_proto_public_monitorsvr.h"
#include "output/protocol/common/release/include/comm_conf_framework.h"
#include "output/protocol/common/release/include/TdrXml.h"
#include "output/protocol/common/release/include/TdrIO.h"
#include "output/protocol/common/release/include/comm_conf_svcid.h"
#include "output/protocol/common/release/include/comm_conf_zerg.h"
#include "output/protocol/common/release/include/comm_conf_cfgsdk.h"
#include "import/include/tencore/tencrypt/TenHash.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_external.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_types.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_define.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_ctypes_info.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_error.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_metalib_init.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_XMLtags.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_XMLMetaLib.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_metalib_to_hpp.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_data_io.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_operate_data.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_data_sort.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_net.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_metalib_manage.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_sql.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tdr/tdr_meta_entries_index.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tbus/tbus.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tbus/tbus_macros.h"
#include "import/include/tencore/tsf4g/tsf4g_base/tbus/tbus_error.h"
#include "output/commlib/framework/release/include/comm_svrd_app_non_ctrl.h"
#include "output/commlib/framework/release/include/comm_svrd_application.h"
#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/commlib/framework/release/include/comm_enum_define.h"
#include "output/commlib/framework/release/include/comm_error_code.h"
#include "output/commlib/framework/release/include/comm_transaction_manager.h"
#include "output/commlib/framework/release/include/comm_frame_malloc.h"
#include "output/commlib/framework/release/include/comm_app_frame.h"
#include "output/commlib/framework/release/include/comm_frame_command.h"
#include "output/commlib/framework/release/include/comm_zerg_mmappipe.h"
#include "output/commlib/framework/release/include/comm_stat_monitor.h"
#include "output/commlib/framework/release/include/comm_stat_define.h"
#include "output/commlib/framework/release/include/comm_svrd_config.h"
#include "output/commlib/framework/release/include/comm_bill_record.h"
#include "output/commlib/framework/release/include/comm_time_provider.h"
#include "output/commlib/framework/release/include/comm_cfgsvr_sdk.h"
#include "output/formmog/appcomm/release/include/app_punish_mode.h"
#include "output/formmog/appcomm/release/include/app_svr_type.h"
#include "output/formmog/appcomm/release/include/app_error_code.h"
#include "output/formmog/appcomm/release/include/app_anti_err_code.h"
#include "import/include/tencore/tencrypt/TenCrypt.h"
#include "output/protocol/formmog/release/include/formmog_proto_antibot_client.h"
#include "output/protocol/formmog/release/include/formmog_proto_public_secsvr.h"
#include "output/protocol/formmog/release/include/formmog_proto_public_cmd.h"
#include "output/protocol/formmog/release/include/formmog_proto_public_tsssdk.h"
#include "output/protocol/formmog/release/include/formmog_proto_tss_sdk_client.h"
#include "output/protocol/formmog/release/include/formmog_conf_secagent.h"
#include "src/formmog/secagent/secagent_msg_handler.h"
#include "src/formmog/secagent/secagent_antibot_msg_crypter.h"
#include "src/formmog/secagent/secagent_restrict.h"
#include "src/formmog/secagent/secagent_gamesvr_channel.h"
#include "src/formmog/secagent/secagent_antibot_msg_handler.h"
#include "src/formmog/secagent/secagent_antibot_crypt_data.h"
#include "src/formmog/secagent/secagent_static_config_dec.h"
#include "src/formmog/secagent/secagent_user_mgr.h"
#include "src/formmog/secagent/secagent_user_info.h"
#include "src/formmog/secagent/secagent_bill.h"
#include "src/formmog/secagent/secagent_antibot_sanlixdata.h"
#include "src/formmog/secagent/secagent_app.h"
#include "src/formmog/secagent/secagent_dpsdk_info_mgr.h"
#include "src/formmog/secagent/secagent_game_exe.h"
#include "src/formmog/secagent/<built-in>"
#include <_G_config.h>
#include <algorithm>
#include <alloca.h>
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm/errno.h>
#include <asm/sigcontext.h>
#include <asm/socket.h>
#include <asm/sockios.h>
#include <assert.h>
#include <bits/allocator.h>
#include <bits/atomicity.h>
#include <bits/basic_ios.h>
#include <bits/basic_ios.tcc>
#include <bits/basic_string.h>
#include <bits/basic_string.tcc>
#include <bits/byteswap.h>
#include <bits/char_traits.h>
#include <bits/codecvt.h>
#include <bits/concept_check.h>
#include <bits/confname.h>
#include <bits/cpp_type_traits.h>
#include <bits/deque.tcc>
#include <bits/dirent.h>
#include <bits/dlfcn.h>
#include <bits/endian.h>
#include <bits/environments.h>
#include <bits/errno.h>
#include <bits/fcntl.h>
#include <bits/fstream.tcc>
#include <bits/functexcept.h>
#include <bits/huge_val.h>
#include <bits/huge_valf.h>
#include <bits/huge_vall.h>
#include <bits/in.h>
#include <bits/inf.h>
#include <bits/ios_base.h>
#include <bits/ipc.h>
#include <bits/ipctypes.h>
#include <bits/istream.tcc>
#include <bits/list.tcc>
#include <bits/local_lim.h>
#include <bits/locale.h>
#include <bits/locale_classes.h>
#include <bits/locale_facets.h>
#include <bits/locale_facets.tcc>
#include <bits/localefwd.h>
#include <bits/mathcalls.h>
#include <bits/mathdef.h>
#include <bits/mathinline.h>
#include <bits/mman.h>
#include <bits/nan.h>
#include <bits/netdb.h>
#include <bits/ostream.tcc>
#include <bits/posix1_lim.h>
#include <bits/posix2_lim.h>
#include <bits/posix_opt.h>
#include <bits/postypes.h>
#include <bits/pthreadtypes.h>
#include <bits/resource.h>
#include <bits/sched.h>
#include <bits/select.h>
#include <bits/semaphore.h>
#include <bits/setjmp.h>
#include <bits/shm.h>
#include <bits/sigaction.h>
#include <bits/sigcontext.h>
#include <bits/siginfo.h>
#include <bits/signum.h>
#include <bits/sigset.h>
#include <bits/sigstack.h>
#include <bits/sigthread.h>
#include <bits/sockaddr.h>
#include <bits/socket.h>
#include <bits/sstream.tcc>
#include <bits/stat.h>
#include <bits/stdio.h>
#include <bits/stdio_lim.h>
#include <bits/stl_algo.h>
#include <bits/stl_algobase.h>
#include <bits/stl_bvector.h>
#include <bits/stl_construct.h>
#include <bits/stl_deque.h>
#include <bits/stl_function.h>
#include <bits/stl_heap.h>
#include <bits/stl_iterator.h>
#include <bits/stl_iterator_base_funcs.h>
#include <bits/stl_iterator_base_types.h>
#include <bits/stl_list.h>
#include <bits/stl_map.h>
#include <bits/stl_multimap.h>
#include <bits/stl_multiset.h>
#include <bits/stl_pair.h>
#include <bits/stl_queue.h>
#include <bits/stl_raw_storage_iter.h>
#include <bits/stl_relops.h>
#include <bits/stl_set.h>
#include <bits/stl_tempbuf.h>
#include <bits/stl_tree.h>
#include <bits/stl_uninitialized.h>
#include <bits/stl_vector.h>
#include <bits/stream_iterator.h>
#include <bits/streambuf.tcc>
#include <bits/streambuf_iterator.h>
#include <bits/stringfwd.h>
#include <bits/sys_errlist.h>
#include <bits/time.h>
#include <bits/types.h>
#include <bits/typesizes.h>
#include <bits/uio.h>
#include <bits/vector.tcc>
#include <bits/waitflags.h>
#include <bits/waitstatus.h>
#include <bits/wchar.h>
#include <bits/wordsize.h>
#include <bits/xopen_lim.h>
#include <cassert>
#include <cctype>
#include <climits>
#include <clocale>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <ctype.h>
#include <cwchar>
#include <cwctype>
#include <debug/debug.h>
#include <deque>
#include <dirent.h>
#include <dlfcn.h>
#include <endian.h>
#include <errno.h>
#include <exception>
#include <exception_defines.h>
#include <execinfo.h>
#include <ext/hash_fun.h>
#include <ext/hash_map>
#include <ext/hash_set>
#include <ext/hashtable.h>
#include <ext/new_allocator.h>
#include <fcntl.h>
#include <features.h>
#include <fstream>
#include <functional>
#include <gconv.h>
#include <getopt.h>
#include <gnu/stubs-32.h>
#include <gnu/stubs.h>
#include <i586-suse-linux/bits/atomic_word.h>
#include <i586-suse-linux/bits/basic_file.h>
#include <i586-suse-linux/bits/c++allocator.h>
#include <i586-suse-linux/bits/c++config.h>
#include <i586-suse-linux/bits/c++io.h>
#include <i586-suse-linux/bits/c++locale.h>
#include <i586-suse-linux/bits/cpu_defines.h>
#include <i586-suse-linux/bits/ctype_base.h>
#include <i586-suse-linux/bits/ctype_inline.h>
#include <i586-suse-linux/bits/gthr-default.h>
#include <i586-suse-linux/bits/gthr.h>
#include <i586-suse-linux/bits/messages_members.h>
#include <i586-suse-linux/bits/os_defines.h>
#include <i586-suse-linux/bits/time_members.h>
#include <iconv.h>
#include <inttypes.h>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <langinfo.h>
#include <libintl.h>
#include <libio.h>
#include <limits.h>
#include <limits>
#include <linux/compiler.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/limits.h>
#include <list>
#include <locale.h>
#include <locale>
#include <map>
#include <math.h>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <new>
#include <nl_types.h>
#include <ostream>
#include <pthread.h>
#include <queue>
#include <rpc/netdb.h>
#include <sched.h>
#include <semaphore.h>
#include <set>
#include <signal.h>
#include <sstream>
#include <stdarg.h>
#include <stddef.h>
#include <stdexcept>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <streambuf>
#include <string.h>
#include <string>
#include <sys/cdefs.h>
#include <sys/epoll.h>
#include <sys/file.h>
#include <sys/io.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <sys/uio.h>
#include <syslimits.h>
#include <time.h>
#include <typeinfo>
#include <unistd.h>
#include <utility>
#include <vector>
#include <wchar.h>
#include <wctype.h>
#include <xlocale.h>

int Comm_App_Frame::appdata_encode<sec_proto::ReportHeartbeatReq>(unsigned int szframe_appdata, const /*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) {
    // local: size_t use_len;
    // local: int ret;
}

int Comm_App_Frame::appdata_decode<sec_proto::SecsvrNotifyIsChangeSvrRsp>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) {
    // local: size_t use_len;
    // local: int ret;
}

// line 533
int SecAgentMsgHandler::proc_notify_is_change_svr(const Comm_App_Frame *recv_frame) {
    // local: /*anon struct*/ int notify_pkg;
    // local: int ret;
    // local: SecAgentUserInfo *user_info;
    // local: SecAgentDPsdkInfoMgr::LPCONFIG conf;
}

int Comm_App_Frame::appdata_decode<sec_proto::SecsvrNotifyKeyOffsetRsp>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) {
    // local: size_t use_len;
    // local: int ret;
}

int Comm_App_Frame::appdata_encode<sec_proto::ConfirmRpcodeInfo>(unsigned int szframe_appdata, const /*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) {
    // local: size_t use_len;
    // local: int ret;
}

int Comm_App_Frame::appdata_encode<sec_proto::PunishClientReq>(unsigned int szframe_appdata, const /*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) {
    // local: size_t use_len;
    // local: int ret;
}

int SecAgentGamesvrChannel::send<sec_proto::PunishClientReq>(unsigned int uin, unsigned int cmd, unsigned int appid, unsigned int channel_id, const /*anon struct*/ int &pkg) {
    // local: size_t len;
    // local: Comm_App_Frame *frame;
}

// line 375
int SecAgentMsgHandler::punish_user(unsigned int uin, unsigned int game_id, short unsigned int channel_id, int punish_mode) {
    // local: /*anon struct*/ int punish_req;
}

// line 366
int SecAgentMsgHandler::reload(SecAgentDPsdkInfoMgr::LPCONFIG conf) {
}

// line 360
int SecAgentMsgHandler::init(SecAgentDPsdkInfoMgr::LPCONFIG conf) {
}

// line 354
int SecAgentMsgHandler::proc_sc_sdk_antibot_pkg(Comm_App_Frame *recv_frame, /*anon struct*/ int *sdk_pkg) {
}

int Comm_App_Frame::appdata_decode<sec_proto::SendToSDK>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) {
    // local: size_t use_len;
    // local: int ret;
}

// line 310
int SecAgentMsgHandler::proc_sc_sdk_pkg(Comm_App_Frame *recv_frame) {
    // local: /*anon struct*/ int sdk_pkg;
    // local: int ret;
}

// line 297
int SecAgentMsgHandler::proc_cs_sdk_antibot_pkg(Comm_App_Frame *recv_frame, /*anon struct*/ int *sdk_pkg) {
}

int Comm_App_Frame::appdata_decode<sec_proto::RecvDataFromSDKReq>(/*anon struct*/ int &info, unsigned int data_start, size_t *sz_code) {
    // local: size_t use_len;
    // local: int ret;
}

// line 262
int SecAgentMsgHandler::proc_cs_sdk_pkg(Comm_App_Frame *recv_frame) {
    // local: /*anon struct*/ int sdk_pkg;
    // local: int ret;
}

// line 223
int SecAgentMsgHandler::proc_del_user(unsigned int uin, unsigned int game_id) {
    // local: int ret;
}

// line 184
int SecAgentMsgHandler::proc_add_user(unsigned int uin, unsigned int game_id, short unsigned int channel_id) {
    // local: int ret;
}

// line 589
int SecAgentMsgHandler::heart_beat_to_secsvr() {
    // local: /*anon struct*/ int heart_beat_pkg;
    // local: SERVICES_ID recv_info;
    // local: SERVICES_ID proxy_info;
    // local: char send_buf[];
    // local: Comm_App_Frame *send_frame;
    // local: int ret;
}

// line 20
int SecAgentMsgHandler::proc_client_msg(Comm_App_Frame *recv_frame) {
    // local: int ret;
    // local: char *recv_data;
    // local: short unsigned int channel_id;
    // local: short unsigned int __v;
    // local: short unsigned int __x;
    // local: short unsigned int channel_id;
    // local: short unsigned int __v;
    // local: short unsigned int __x;
}

int SecAgentMsgHandler::send_pkg_to_secsvr<sec_proto::ConfirmRpcodeInfo>(unsigned int cmd, const SecAgentUserInfo *user_info, const /*anon struct*/ int &pkg) {
    // local: SERVICES_ID recv_info;
    // local: SERVICES_ID proxy_info;
    // local: char send_buf[];
    // local: Comm_App_Frame *send_frame;
    // local: int ret;
}

// line 395
int SecAgentMsgHandler::proc_rpcode_info_pkg(Comm_App_Frame *recv_frame) {
    // local: /*anon struct*/ int rpcode_info_pkg;
    // local: int ret;
    // local: SecAgentUserInfo *user_info;
    // local: const SecagentGameExe *game_exe;
    // local: /*anon struct*/ int rsp_pkg;
}

// line 116
int SecAgentMsgHandler::proc_sever_msg(Comm_App_Frame *recv_frame) {
    // local: int ret;
}

