// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commsvr/zergsvr/zerg_tcp_ctrl_handler.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/commsvr/zergsvr/zerg_predefine.h"
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
#include "output/commlib/zenlib/release/include/zen_id_to_string.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_predefine.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_time.h"
#include "output/commlib/zenlib/release/include/zen_mysql_connect.h"
#include "output/commlib/zenlib/release/include/zen_mysql_predefine.h"
#include "output/commlib/zenlib/release/include/zen_mysql_command.h"
#include "output/commlib/zenlib/release/include/zen_mysql_result.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_string.h"
#include "output/commlib/zenlib/release/include/zen_mysql_field.h"
#include "output/commlib/zenlib/release/include/zen_mysql_process.h"
#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
#include "output/commlib/zenlib/release/include/zen_shm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_shm_cache_chunk.h"
#include "output/commlib/zenlib/release/include/zen_shm_lockfree_deque.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_spin.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_thread.h"
#include "output/commlib/zenlib/release/include/zen_server_toolkit.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_process.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_sysinfo.h"
#include "output/commlib/zenlib/release/include/zen_shm_hash_table.h"
#include "output/commlib/zenlib/release/include/zen_boost_lord_rings.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_error.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_file.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_socket.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_getopt.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_debug.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_mutex.h"
#include "output/commlib/zenlib/release/include/zen_lock_base.h"
#include "output/commlib/zenlib/release/include/zen_lock_guard.h"
#include "output/commlib/zenlib/release/include/zen_lock_null_lock.h"
#include "output/commlib/zenlib/release/include/zen_config_property_tree.h"
#include "output/commlib/zenlib/release/include/zen_config_ini_implement.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_sema.h"
#include "output/commlib/zenlib/release/include/zen_lock_synch_traits.h"
#include "output/commlib/zenlib/release/include/zen_lock_process_mutex.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_posix.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_rw_mutex.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_semaphore.h"
#include "output/commlib/zenlib/release/include/zen_lock_thread_condi.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_condi.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_template.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_nonlock.h"
#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_wheel.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_base.h"
#include "output/commlib/zenlib/release/include/zen_share_mem_mmap.h"
#include "output/commlib/zenlib/release/include/zen_bus_mmap_pipe.h"
#include "output/commlib/zenlib/release/include/zen_bus_two_way.h"
#include "output/commlib/zenlib/release/include/zen_thread_task.h"
#include "output/commlib/zenlib/release/include/zen_event_handle_base.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_base.h"
#include "output/commlib/zenlib/release/include/zen_config_getopt.h"
#include "output/commlib/zenlib/release/include/zen_socket_base.h"
#include "output/commlib/zenlib/release/include/zen_socket_acceptor.h"
#include "output/commlib/zenlib/release/include/zen_socket_connector.h"
#include "output/commlib/zenlib/release/include/zen_socket_stream.h"
#include "output/commlib/framework/release/include/comm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_os_adapt_dirent.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_base.h"
#include "output/commlib/zenlib/release/include/zen_socket_addr_in.h"
#include "output/commlib/zenlib/release/include/zen_socket_datagram.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_select.h"
#include "output/commlib/zenlib/release/include/zen_event_reactor_epoll.h"
#include "output/commlib/zenlib/release/include/zen_thread_wait_mgr.h"
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
#include "output/commlib/framework/release/include/comm_error_code.h"
#include "output/commlib/framework/release/include/comm_random_number.h"
#include "output/commlib/framework/release/include/comm_service_config.h"
#include "output/commlib/framework/release/include/comm_service_info.h"
#include "output/commlib/framework/release/include/comm_enum_define.h"
#include "output/commlib/framework/release/include/comm_app_frame.h"
#include "output/commlib/framework/release/include/comm_frame_command.h"
#include "output/commlib/framework/release/include/comm_zerg_mmappipe.h"
#include "output/commlib/framework/release/include/comm_stat_monitor.h"
#include "output/commlib/framework/release/include/comm_stat_define.h"
#include "output/commlib/framework/release/include/comm_svrd_config.h"
#include "output/commlib/framework/release/include/comm_mml_command.h"
#include "output/commlib/framework/release/include/comm_mml_console_handler.h"
#include "output/commlib/framework/release/include/comm_cfgsvr_sdk.h"
#include "output/commlib/framework/release/include/comm_encrypt_arithmetic.h"
#include "output/commlib/framework/release/include/comm_cmd_statistic.h"
#include "output/commlib/framework/release/include/comm_svrd_application.h"
#include "output/commlib/framework/release/include/comm_svrd_app_main.h"
#include "output/commlib/framework/release/include/comm_bill_record.h"
#include "output/commlib/framework/release/include/comm_time_provider.h"
#include "output/commlib/framework/release/include/comm_xml_config.h"
#include "src/commsvr/zergsvr/zerg_tcp_ctrl_handler.h"
#include "src/commsvr/zergsvr/zerg_buf_storage.h"
#include "src/commsvr/zergsvr/zerg_auto_connect.h"
#include "src/commsvr/zergsvr/zerg_service_info_set.h"
#include "src/commsvr/zergsvr/zerg_comm_manager.h"
#include "src/commsvr/zergsvr/zerg_configure.h"
#include "src/commsvr/zergsvr/zerg_stat_define.h"
#include "src/commsvr/zergsvr/zerg_app_handler.h"
#include "src/commsvr/zergsvr/<built-in>"
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

unsigned int TCP_Svc_Handler::get_handle_id() {
}

TCP_Svc_Handler::PEER_STATUS TCP_Svc_Handler::get_peer_status() {
}

void TCP_Svc_Handler::get_max_peer_num(size_t &maxaccept, size_t &maxconnect) {
}

const ZEN_Sockaddr_In & TCP_Svc_Handler::get_peer_sockaddr() {
}

int TCP_Svc_Handler::find_services_peer(const SERVICES_ID &svr_info, TCP_Svc_Handler *&svchanle) {
    // local: int ret;
}

int TCP_Svc_Handler::close_services_peer(const SERVICES_ID &svr_info) {
    // local: int ret;
    // local: TCP_Svc_Handler *svchanle;
}

int TCP_Svc_Handler::uninit_all_staticdata() {
}

void TCP_Svc_Handler::auto_connect_allserver() {
    // local: size_t szsucc;
    // local: size_t szfail;
    // local: size_t szvalid;
}

ZEN_SOCKET TCP_Svc_Handler::get_handle() {
}

int TCP_Svc_Handler::get_tcpctrl_conf(const Zerg_MMAP_BusPipe::ZERG_CONFIG *config) {
    // local: int ret;
}

short unsigned int TCP_Svc_Handler::get_peer_port() {
}

void TCP_Svc_Handler::unite_frame_sendlist() {
    // local: size_t sz_deque;
}

TCP_Svc_Handler * TCP_Svc_Handler::AllocSvcHandlerFromPool(TCP_Svc_Handler::HANDLER_MODE handler_mode) {
    // local: TCP_Svc_Handler *p_handler;
    // local: TCP_Svc_Handler *p_handler;
}

void TCP_Svc_Handler::dump_svcpeer_info(ostringstream &ostr_stream, size_t startno, size_t numquery) {
}

void TCP_Svc_Handler::dump_status_staticinfo(ostringstream &ostr_stream) {
}

void TCP_Svc_Handler::dump_status_info(ostringstream &ostr_stream) {
}

const char * TCP_Svc_Handler::get_peer_address() {
}

int TCP_Svc_Handler::process_send_error(ZByteBuffer *tmpbuf, bool frame_encode) {
    // local: size_t use_start;
    // local: Comm_App_Frame *proc_frame;
}

int TCP_Svc_Handler::write_data_to_peer(size_t &szsend, bool &bfull) {
    // local: ZByteBuffer *sndbuffer;
    // local: ssize_t sendret;
}

int TCP_Svc_Handler::write_all_data_to_peer() {
    // local: int ret;
    // local: int handle_mask;
    // local: size_t szsend;
    // local: bool bfull;
    // local: int ret;
}

int TCP_Svc_Handler::put_frame_to_sendlist(ZByteBuffer *tmpbuf) {
    // local: int ret;
    // local: Comm_App_Frame *proc_frame;
    // local: bool bret;
}

int TCP_Svc_Handler::send_simple_zerg_cmd(unsigned int cmd, const SERVICES_ID &recv_services_info, unsigned int option) {
    // local: ZByteBuffer *tmpbuf;
    // local: Comm_App_Frame *proc_frame;
}

int TCP_Svc_Handler::send_zergheatbeat_reg() {
}

int TCP_Svc_Handler::process_send_data(ZByteBuffer *tmpbuf) {
    // local: Comm_App_Frame *proc_frame;
    // local: SERVICES_ID *p_sendto_svrinfo;
    // local: int ret;
    // local: TCP_Svc_Handler *svchanle;
    // local: bool backroute_valid;
    // local: SERVICES_ID backroute_svcinfo;
    // local: SERVICES_ID svrinfo;
    // local: int ret;
    // local: TCP_Svc_Handler *backroute_svchanle;
}

int TCP_Svc_Handler::check_recv_full_frame(bool &bfull, unsigned int &whole_frame_len) {
    // local: unsigned int __v;
    // local: unsigned int __x;
}

int TCP_Svc_Handler::read_data_from_peer(size_t &szrevc) {
    // local: ssize_t recvret;
}

int TCP_Svc_Handler::process_connect_register() {
    // local: ZEN_Sockaddr_In peeraddr;
}

int TCP_Svc_Handler::handle_output() {
    // local: int ret;
}

int TCP_Svc_Handler::preprocess_recvframe(Comm_App_Frame *proc_frame) {
    // local: int ret;
    // local: TCP_Svc_Handler *old_hdl;
}

int TCP_Svc_Handler::handle_close() {
    // local: size_t sz_of_deque;
    // local: size_t i;
    // local: Comm_App_Frame appframe;
}

int TCP_Svc_Handler::handle_timeout(const ZEN_Time_Value &now_time, const void *arg) {
    // local: const int timeid;
}

void TCP_Svc_Handler::init_tcpsvr_handler(const SERVICES_ID &my_svcinfo, const ZEN_Socket_Stream &sockstream, const ZEN_Sockaddr_In &socketaddr, bool sessionkey_verify) {
    // local: int ret;
    // local: ZEN_Time_Value delay;
    // local: ZEN_Time_Value interval;
    // local: int keep_alive;
    // local: socklen_t opvallen;
    // local: const size_t WARNNING_TIMES;
}

int TCP_Svc_Handler::push_frame_to_comm_mgr() {
    // local: int ret;
    // local: unsigned int whole_frame_len;
    // local: bool bfull;
    // local: Comm_App_Frame *proc_frame;
}

int TCP_Svc_Handler::handle_input() {
    // local: size_t szrecv;
    // local: int ret;
}

void TCP_Svc_Handler::init_tcpsvr_handler(const SERVICES_ID &my_svcinfo, const SERVICES_ID &peer_svrinfo, const ZEN_Socket_Stream &sockstream, const ZEN_Sockaddr_In &socketaddr) {
    // local: int ret;
    // local: ZEN_Time_Value delay;
    // local: ZEN_Time_Value interval;
    // local: int keep_alive;
    // local: socklen_t opvallen;
    // local: int NODELAY;
}

int TCP_Svc_Handler::init_all_static_data() {
    // local: size_t szsucc;
    // local: size_t szfail;
    // local: size_t szvalid;
    // local: size_t i;
    // local: TCP_Svc_Handler *p_handler;
    // local: size_t i;
    // local: TCP_Svc_Handler *p_handler;
}

