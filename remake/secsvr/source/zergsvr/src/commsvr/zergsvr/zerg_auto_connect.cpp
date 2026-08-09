// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commsvr/zergsvr/zerg_auto_connect.cpp
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
#include "src/commsvr/zergsvr/zerg_auto_connect.h"
#include "src/commsvr/zergsvr/zerg_application.h"
#include "src/commsvr/zergsvr/zerg_tcp_ctrl_handler.h"
#include "src/commsvr/zergsvr/zerg_buf_storage.h"
#include "src/commsvr/zergsvr/zerg_service_info_set.h"
#include "src/commsvr/zergsvr/zerg_app_handler.h"
#include "src/commsvr/zergsvr/zerg_configure.h"
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

// line 374
size_t Zerg_Auto_Connector::numsvr_connect() {
}

_Rb_tree_iterator<std::pair<const short unsigned int, unsigned int> > find(const short unsigned int &__k) {
    // local: _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__x;
    // local: _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__y;
    // local: /*anon struct*/ int __j;
}

// line 423
bool Zerg_Auto_Connector::is_connected(const SERVICES_ID &svrinfo) {
    // local: TCP_Svc_Handler *svchandle;
    // local: int ret;
}

void _M_erase(_Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__x) {
    // local: _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__y;
}

// line 301
int Zerg_Auto_Connector::get_backupsvcinfo(const SERVICES_ID &main_svrinfo, bool &backroute_valid, SERVICES_ID &backroute_svrinfo) {
    // local: size_t i;
    // local: ZEN_Sockaddr_In inetaddr;
}

_Rb_tree_node<std::pair<const short unsigned int, unsigned int> > * _M_copy(const _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__x, _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__p) {
    // local: _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__top;
    // local: _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__y;
}

/*anon struct*/ int & operator=(const /*anon struct*/ int &__x) {
}

_Rb_tree_iterator<std::pair<const short unsigned int, unsigned int> > _M_insert(/*anon struct*/ int *__x, /*anon struct*/ int *__p, const pair<const short unsigned int,unsigned int> &__v) {
    // local: bool __insert_left;
    // local: _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__z;
}

pair<std::_Rb_tree_iterator<std::pair<const short unsigned int, unsigned int> >,bool> insert_unique(const pair<const short unsigned int,unsigned int> &__v) {
    // local: _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__x;
    // local: _Rb_tree_node<std::pair<const short unsigned int, unsigned int> > *__y;
    // local: bool __comp;
    // local: /*anon struct*/ int __j;
}

// line 322
int Zerg_Auto_Connector::connect_server_bysvcid(const SERVICES_ID &svrinfo, const ZEN_Sockaddr_In &inetaddr) {
    // local: ZEN_Socket_Stream tcpscoket;
    // local: const socklen_t opval;
    // local: socklen_t opvallen;
    // local: int ret;
    // local: TCP_Svc_Handler *p_handler;
}

// line 280
int Zerg_Auto_Connector::reconnect_server(const SERVICES_ID &reconnect_svrinfo) {
    // local: size_t i;
    // local: ZEN_Sockaddr_In inetaddr;
}

// line 189
void Zerg_Auto_Connector::reconnect_allserver(size_t &szvalid, size_t &szsucc, size_t &szfail) {
    // local: int ret;
    // local: size_t i;
    // local: TCP_Svc_Handler *svchandle;
}

// line 380
int Zerg_Auto_Connector::get_server(short unsigned int svr_type, SERVICES_ID *svrinfo) {
    // local: /*anon struct*/ int iter;
    // local: unsigned int index;
    // local: unsigned int id_num;
    // local: unsigned int i;
    // local: unsigned int rand_num;
}

// line 551
bool Zerg_Auto_Connector::is_current_auto_connect(const SERVICES_ID &service, bool is_main_service) {
    // local: /*anon struct*/ int iter;
    // local: vector<unsigned int,std::allocator<unsigned int> > *list_id;
    // local: unsigned int i;
    // local: TCP_Svc_Handler *svchandle;
    // local: int ret;
    // local: ZEN_Sockaddr_In svr_ip_addr;
}

void _M_insert_aux(__normal_iterator<unsigned int*,std::vector<unsigned int, std::allocator<unsigned int> > > __position, const unsigned int &__x) {
    // local: unsigned int __x_copy;
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
}

vector<unsigned int,std::allocator<unsigned int> > & operator=(const vector<unsigned int,std::allocator<unsigned int> > &__x) {
    // local: const size_t __xlen;
    // local: /*anon struct*/ int __i;
    // local: unsigned int *__tmp;
}

void _M_insert_aux(__normal_iterator<SERVICES_ROUTE_INFO*,std::vector<SERVICES_ROUTE_INFO, std::allocator<SERVICES_ROUTE_INFO> > > __position, const SERVICES_ROUTE_INFO &__x) {
    // local: SERVICES_ROUTE_INFO __x_copy;
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
}

void _M_insert_aux(__normal_iterator<std::vector<unsigned int, std::allocator<unsigned int> >*,std::vector<std::vector<unsigned int, std::allocator<unsigned int> >, std::allocator<std::vector<unsigned int, std::allocator<unsigned int> > > > > __position, const vector<unsigned int,std::allocator<unsigned int> > &__x) {
    // local: /*anon struct*/ int __x_copy;
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
}

/*anon struct*/ int & operator=(const /*anon struct*/ int &__x) {
    // local: const size_t __xlen;
    // local: /*anon struct*/ int __i;
    // local: vector<unsigned int,std::allocator<unsigned int> > *__tmp;
}

// line 452
void Zerg_Auto_Connector::add_auto_connect_info(const SERVICES_ID &main_svrinfo, const SERVICES_ID &back_svrinfo) {
    // local: /*anon struct*/ int iter;
    // local: /*anon struct*/ int main_id_list;
    // local: /*anon struct*/ int back_id_list;
    // local: unsigned int index;
    // local: /*anon struct*/ int main_id_list;
    // local: /*anon struct*/ int back_id_list;
}

// line 62
int Zerg_Auto_Connector::get_autoconnect_cfg(const Zerg_MMAP_BusPipe::ZERG_CONFIG *config) {
    // local: int ret;
    // local: SERVICES_ROUTE_INFO svc_route;
    // local: int numsvr;
    // local: char tmp[];
    // local: int i;
    // local: short unsigned int svrtype;
    // local: unsigned int svrid;
    // local: ZEN_Sockaddr_In svr_ip_addr;
    // local: ZEN_Sockaddr_In back_addr;
    // local: const size_t TMP_ADDR_LEN;
    // local: char mainroute_addr[];
    // local: char backroute_addr[];
    // local: short unsigned int backtype;
    // local: unsigned int backid;
}

// line 488
int Zerg_Auto_Connector::reload_cfg(const Zerg_MMAP_BusPipe::ZERG_CONFIG *config) {
    // local: RouteType2Index old_want_connect_type_2_index;
    // local: ListOfMainRouteId old_list_of_want_connect_main_id;
    // local: ListOfBackRouteId old_list_of_want_connect_back_id;
    // local: int ret;
    // local: /*anon struct*/ int iter;
    // local: vector<unsigned int,std::allocator<unsigned int> > &const old_main_id_list;
    // local: vector<unsigned int,std::allocator<unsigned int> > &const old_back_id_list;
    // local: unsigned int i;
    // local: SERVICES_ID service;
    // local: unsigned int i;
    // local: SERVICES_ID service;
}

