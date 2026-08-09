// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/framework/comm_transaction_manager.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/commlib/framework/comm_predefine.h"
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
#include "output/commlib/zenlib/release/include/zen_boost_non_copyable.h"
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
#include "output/commlib/zenlib/release/include/zen_shm_predefine.h"
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
#include "output/commlib/zenlib/release/include/zen_share_mem_posix.h"
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
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_enum_define.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_frame_command.h"
#include "src/commlib/framework/comm_error_code.h"
#include "src/commlib/framework/comm_transaction_base.h"
#include "src/commlib/framework/comm_transaction_manager.h"
#include "src/commlib/framework/comm_frame_malloc.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_stat_define.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/commlib/framework/comm_security_check.h"
#include "src/commlib/framework/<built-in>"
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

void Transaction_Manager::enable_trans_statistics(const ZEN_Time_Value *stat_clock) {
}

void Transaction_Manager::get_manager_load_foctor2(unsigned int &load_max, unsigned int &load_cur) {
    // local: const unsigned int ONE_CYCLE_GENERATE_TRANS;
}

void Transaction_Manager::clean_instance() {
}

void Transaction_Manager::instance(Transaction_Manager *pinstatnce) {
}

const long unsigned int * std::lower_bound<const long unsigned int*, long unsigned int>(const long unsigned int *__first, const long unsigned int *__last, const long unsigned int &__val) {
    // local: ptrdiff_t __len;
    // local: ptrdiff_t __half;
    // local: const long unsigned int *__middle;
}

void std::fill<__gnu_cxx::__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*, std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > >, ZEN_LIB::lordrings<Comm_App_Frame*> >(/*anon struct*/ int __first, /*anon struct*/ int __last, const lordrings<Comm_App_Frame*> &__value) {
    // local: const bool __scalar;
}

void std::fill<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > >, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*>(/*anon struct*/ int __first, /*anon struct*/ int __last, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &__value) {
    // local: const bool __scalar;
}

void std::fill<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > >, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*>(/*anon struct*/ int __first, /*anon struct*/ int __last, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &__value) {
    // local: const bool __scalar;
}

void std::fill<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**, std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > >, __gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*>(/*anon struct*/ int __first, /*anon struct*/ int __last, _Hashtable_node<TRANS_LOCK_RECORD> *const &__value) {
    // local: const bool __scalar;
}

_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > ** std::fill_n<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*>(_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > **__first, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*>(_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > **__first, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &__x, /*anon struct*/ int arg3) {
}

_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > ** std::fill_n<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*>(_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > **__first, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*>(_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > **__first, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &__x, /*anon struct*/ int arg3) {
}

void std::__uninitialized_fill_n_aux<ZEN_LIB::lordrings<Comm_App_Frame*>*, unsigned int, ZEN_LIB::lordrings<Comm_App_Frame*> >(lordrings<Comm_App_Frame*> *__first, unsigned int __n, const lordrings<Comm_App_Frame*> &__x, __false_type arg3) {
    // local: lordrings<Comm_App_Frame*> *__cur;
}

void std::__uninitialized_fill_n_a<ZEN_LIB::lordrings<Comm_App_Frame*>*, unsigned int, ZEN_LIB::lordrings<Comm_App_Frame*>, ZEN_LIB::lordrings<Comm_App_Frame*> >(lordrings<Comm_App_Frame*> *__first, unsigned int __n, const lordrings<Comm_App_Frame*> &__x, /*anon struct*/ int arg3) {
}

void std::__uninitialized_fill_n_aux<__gnu_cxx::__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*, std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > >, unsigned int, ZEN_LIB::lordrings<Comm_App_Frame*> >(/*anon struct*/ int __first, unsigned int __n, const lordrings<Comm_App_Frame*> &__x, __false_type arg3) {
    // local: /*anon struct*/ int __cur;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*, std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > >, unsigned int, ZEN_LIB::lordrings<Comm_App_Frame*>, ZEN_LIB::lordrings<Comm_App_Frame*> >(/*anon struct*/ int __first, unsigned int __n, const lordrings<Comm_App_Frame*> &__x, /*anon struct*/ int arg3) {
}

_Hashtable_node<TRANS_LOCK_RECORD> ** std::fill_n<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**, unsigned int, __gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*>(_Hashtable_node<TRANS_LOCK_RECORD> **__first, unsigned int __n, _Hashtable_node<TRANS_LOCK_RECORD> *const &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**, unsigned int, __gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, __gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*>(_Hashtable_node<TRANS_LOCK_RECORD> **__first, unsigned int __n, _Hashtable_node<TRANS_LOCK_RECORD> *const &__x, /*anon struct*/ int arg3) {
}

/*anon struct*/ int std::fill_n<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*>(/*anon struct*/ int __first, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*>(/*anon struct*/ int __first, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &__x, /*anon struct*/ int arg3) {
}

/*anon struct*/ int std::fill_n<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*>(/*anon struct*/ int __first, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, __gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*>(/*anon struct*/ int __first, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &__x, /*anon struct*/ int arg3) {
}

/*anon struct*/ int std::fill_n<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**, std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > >, unsigned int, __gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*>(/*anon struct*/ int __first, unsigned int __n, _Hashtable_node<TRANS_LOCK_RECORD> *const &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**, std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > >, unsigned int, __gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, __gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*>(/*anon struct*/ int __first, unsigned int __n, _Hashtable_node<TRANS_LOCK_RECORD> *const &__x, /*anon struct*/ int arg3) {
}

int Transaction_Manager::mgr_sendbuf_to_service(unsigned int cmd, unsigned int qquin, unsigned int trans_id, unsigned int backfill_trans_id, const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc, const SERVICES_ID &sndsvc, const unsigned char *buf, size_t buf_len, unsigned int app_id, unsigned int option) {
}

void _M_destroy_nodes(Comm_App_Frame ***__nstart, Comm_App_Frame ***__nfinish) {
    // local: Comm_App_Frame ***__n;
}

void AppFrame_Mallocor_Mgr<ZEN_Null_Mutex>::ExtendFrameListCapacity(unsigned int list_no, unsigned int extend_num) {
    // local: size_t old_capacity;
    // local: size_t j;
    // local: Comm_App_Frame *proc_frame;
}

void Transaction_Manager::get_manager_load_foctor(unsigned int &load_max, unsigned int &load_cur) {
}

int Transaction_Manager::push_back_sendpipe(Comm_App_Frame *proc_frame) {
}

int Transaction_Manager::mgr_sendmsghead_to_service(unsigned int cmd, unsigned int qquin, const SERVICES_ID &rcvsvc, const SERVICES_ID &proxysvc, unsigned int backfill_trans_id, unsigned int app_id, unsigned int option) {
    // local: Comm_App_Frame *rsp_msg;
}

void _M_reallocate_map(unsigned int __nodes_to_add, bool __add_at_front) {
    // local: const size_t __old_num_nodes;
    // local: const size_t __new_num_nodes;
    // local: Comm_App_Frame ***__new_nstart;
    // local: size_t __new_map_size;
    // local: Comm_App_Frame ***__new_map;
}

void _M_push_back_aux(Comm_App_Frame *const &__t) {
    // local: Comm_App_Frame *__t_copy;
}

pair<__gnu_cxx::_Hashtable_iterator<TRANS_LOCK_RECORD, TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, std::_Identity<TRANS_LOCK_RECORD>, EQUAL_OF_TRANS_LOCK, std::allocator<TRANS_LOCK_RECORD> >,bool> insert_unique_noresize(const TRANS_LOCK_RECORD &__obj) {
    // local: const size_t __n;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__first;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__tmp;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__cur;
}

void _M_pop_front_aux() {
}

Comm_App_Frame * AppFrame_Mallocor_Mgr<ZEN_Null_Mutex>::alloc_appframe(unsigned int frame_len) {
    // local: ZEN_Lock_Guard<ZEN_Null_Mutex> tmp_guard;
    // local: size_t hk;
    // local: Comm_App_Frame *new_frame;
}

void AppFrame_Mallocor_Mgr<ZEN_Null_Mutex>::clone_appframe(const Comm_App_Frame *model_freame, Comm_App_Frame *&cloned_frame) {
    // local: size_t frame_len;
}

void _M_create_nodes(Comm_App_Frame ***__nstart, Comm_App_Frame ***__nfinish) {
    // local: Comm_App_Frame ***__cur;
}

void _M_initialize_map(unsigned int __num_elements) {
    // local: const size_t __num_nodes;
    // local: Comm_App_Frame ***__nstart;
    // local: Comm_App_Frame ***__nfinish;
}

void clear() {
    // local: size_t __i;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__cur;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__next;
}

void reserve(unsigned int __n) {
    // local: const size_t __old_size;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> **__tmp;
}

void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>**,std::vector<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*, std::allocator<__gnu_cxx::_Hashtable_node<TRANS_LOCK_RECORD>*> > > __position, unsigned int __n, _Hashtable_node<TRANS_LOCK_RECORD> *const &__x) {
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__x_copy;
    // local: const size_t __elems_after;
    // local: /*anon struct*/ int __old_finish;
}

void _M_fill_insert(__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > __position, unsigned int __n, const lordrings<Comm_App_Frame*> &__x) {
    // local: /*anon struct*/ int __x_copy;
    // local: const size_t __elems_after;
    // local: /*anon struct*/ int __old_finish;
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
}

void AppFrame_Mallocor_Mgr<ZEN_Null_Mutex>::free_appframe(Comm_App_Frame *proc_frame) {
    // local: ZEN_Lock_Guard<ZEN_Null_Mutex> tmp_guard;
    // local: size_t hk;
}

int Transaction_Manager::mgr_postframe_to_msgqueue(Comm_App_Frame *post_frame) {
    // local: int ret;
    // local: Comm_App_Frame *tmp_frame;
}

void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, Transaction_Base*> >*> > > __position, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *const &__x) {
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__x_copy;
    // local: const size_t __elems_after;
    // local: /*anon struct*/ int __old_finish;
}

void reserve(unsigned int __n) {
    // local: const size_t __old_size;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > **__tmp;
}

int Transaction_Manager::get_handler_by_transid(unsigned int transid, unsigned int trans_cmd, Transaction_Base *&ptxbase) {
    // local: /*anon struct*/ int mapiter;
}

void clear() {
    // local: size_t __i;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__cur;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__next;
}

__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > erase(__normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > __first, __normal_iterator<ZEN_LIB::lordrings<Comm_App_Frame*>*,std::vector<ZEN_LIB::lordrings<Comm_App_Frame*>, std::allocator<ZEN_LIB::lordrings<Comm_App_Frame*> > > > __last) {
    // local: /*anon struct*/ int __i;
}

void AppFrame_Mallocor_Mgr<ZEN_Null_Mutex>::initialize(unsigned int init_num, unsigned int max_frame_len) {
    // local: size_t sz_frame;
    // local: size_t i;
    // local: size_t i;
}

void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> >*> > > __position, unsigned int __n, _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *const &__x) {
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
    // local: _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *__x_copy;
    // local: const size_t __elems_after;
    // local: /*anon struct*/ int __old_finish;
}

void reserve(unsigned int __n) {
    // local: const size_t __old_size;
    // local: _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > **__tmp;
}

void clear() {
    // local: size_t __i;
    // local: _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *__cur;
    // local: _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *__next;
}

size_t erase(const TRANS_LOCK_RECORD &__key) {
    // local: const size_t __n;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__first;
    // local: size_t __erased;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__cur;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__next;
}

void Transaction_Manager::unlock_qquin_trans_cmd(unsigned int qq_uin, unsigned int lock_trnas_id) {
    // local: TRANS_LOCK_RECORD lock_rec;
}

size_t erase(const unsigned int &__key) {
    // local: const size_t __n;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__first;
    // local: size_t __erased;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__cur;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__next;
}

/*anon struct*/ int & operator++() {
    // local: const _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__old;
    // local: size_t __bucket;
}

void Transaction_Manager::dump_all_trans_info() {
    // local: /*anon struct*/ int iter_tmp;
    // local: /*anon struct*/ int iter_end;
    // local: unsigned int i;
    // local: Transaction_Base *pbase;
}

/*anon struct*/ int & operator++() {
    // local: const _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *__old;
    // local: size_t __bucket;
}

void Transaction_Manager::dump_statistics_info() {
    // local: /*anon struct*/ int iter_tmp;
    // local: /*anon struct*/ int iter_end;
    // local: unsigned int i;
}

void Transaction_Manager::dump_trans_pool_info() {
    // local: /*anon struct*/ int iter_tmp;
    // local: /*anon struct*/ int iter_end;
    // local: unsigned int i;
    // local: unsigned int frame_command;
    // local: const POOL_OF_REGISTERTRANS &const pool_regtrans;
}

void Transaction_Manager::dump_all_debug_info() {
}

/*anon struct*/ int & operator++() {
    // local: const _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *__old;
    // local: size_t __bucket;
}

void Transaction_Manager::recycle_clone_from_pool() {
    // local: /*anon struct*/ int iter_tmp;
    // local: /*anon struct*/ int iter_end;
    // local: unsigned int i;
    // local: unsigned int frame_command;
    // local: POOL_OF_REGISTERTRANS &const pool_regtrans;
    // local: const size_t USE_TRANS_THRESHOLD;
    // local: const size_t RECYCLE_TRANS_NUM;
    // local: size_t pool_capacity;
    // local: size_t i;
    // local: Transaction_Base *recycle_txbase;
}

void resize(unsigned int __num_elements_hint) {
    // local: const size_t __old_n;
    // local: const size_t __n;
    // local: /*anon struct*/ int __tmp;
    // local: size_t __bucket;
    // local: _Hashtable_node<TRANS_LOCK_RECORD> *__first;
    // local: size_t __new_bucket;
}

int Transaction_Manager::lock_qquin_trnas_cmd(unsigned int qq_uin, unsigned int trnas_lock_id, unsigned int frame_cmd) {
    // local: TRANS_LOCK_RECORD lock_rec;
    // local: /*anon struct*/ int iter_tmp;
}

void resize(unsigned int __num_elements_hint) {
    // local: const size_t __old_n;
    // local: const size_t __n;
    // local: /*anon struct*/ int __tmp;
    // local: size_t __bucket;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__first;
    // local: size_t __new_bucket;
}

/*anon struct*/ int & find_or_insert(const /*anon struct*/ int &__obj) {
    // local: size_t __n;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__first;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__tmp;
    // local: _Hashtable_node<std::pair<const unsigned int, Transaction_Base*> > *__cur;
}

void resize(unsigned int __num_elements_hint) {
    // local: const size_t __old_n;
    // local: const size_t __n;
    // local: /*anon struct*/ int __tmp;
    // local: size_t __bucket;
    // local: _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *__first;
    // local: size_t __new_bucket;
}

/*anon struct*/ int & find_or_insert(const /*anon struct*/ int &__obj) {
    // local: size_t __n;
    // local: _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *__first;
    // local: _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *__tmp;
    // local: _Hashtable_node<std::pair<const unsigned int, CREATE_TRANS_RECORD> > *__cur;
}

void Transaction_Manager::initialize(size_t szregtrans, size_t sztransmap, SERVICES_ID selfsvr, ZEN_Timer_Queue *timer_queue, Zerg_MMAP_BusPipe *zerg_mmap_pipe, unsigned int max_frame_len, bool init_inner_queue, bool init_lock_pool) {
}

Transaction_Manager * Transaction_Manager::instance() {
}

int Transaction_Manager::return_clone_to_pool(unsigned int frame_cmd, Transaction_Base *&rt_txbase) {
    // local: CREATE_TRANS_RECORD &const reg_ctr_trans;
}

int Transaction_Manager::get_clone_from_pool(unsigned int frame_cmd, unsigned int qq_uin, Transaction_Base *&crt_trans) {
    // local: int ret;
    // local: /*anon struct*/ int mapiter;
    // local: CREATE_TRANS_RECORD &const reg_ctr_trans;
    // local: Transaction_Base *model_trans;
    // local: size_t capacity_of_pool;
    // local: size_t i;
    // local: Transaction_Base *cloned_txbase;
}

int Transaction_Manager::unregiester_trans_id(unsigned int transid, unsigned int trans_cmd, int run_state, time_t trans_start) {
    // local: /*anon struct*/ int mapiter;
    // local: Transaction_Base *rt_tsbase;
}

int Transaction_Manager::regiester_trans_id(unsigned int transid, unsigned int trans_cmd, Transaction_Base *ptxbase) {
}

int Transaction_Manager::process_appframe(Comm_App_Frame *ppetappframe, bool &bcrttx) {
    // local: int ret;
    // local: Transaction_Base *crt_trans;
    // local: Transaction_Base *run_tans;
}

int Transaction_Manager::process_queue_frame(size_t &proc_frame, size_t &create_trans) {
    // local: int ret;
    // local: Comm_App_Frame *tmp_frame;
    // local: bool bcrtcx;
}

int Transaction_Manager::process_pipe_frame(size_t &proc_frame, size_t &create_trans) {
    // local: int ret;
    // local: Comm_App_Frame *tmp_frame;
    // local: bool bcrtcx;
}

int Transaction_Manager::register_trans_cmd(unsigned int cmd, Transaction_Base *ptxbase, bool if_auto_lock_trans, unsigned int lock_trans_cmd) {
    // local: CREATE_TRANS_RECORD ctr_trans_rec;
    // local: size_t i;
    // local: Transaction_Base *tmp_txbase;
}

