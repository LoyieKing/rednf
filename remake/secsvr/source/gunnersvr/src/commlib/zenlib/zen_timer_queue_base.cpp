// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_timer_queue_base.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/commlib/zenlib/zen_predefine.h"
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
#include "src/commlib/zenlib/zen_os_adapt_time.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_trace_log_debug.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_shm_predefine.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_lock_base.h"
#include "src/commlib/zenlib/zen_lock_guard.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_timer_handler_base.h"
#include "src/commlib/zenlib/zen_timer_queue_base.h"
#include "src/commlib/zenlib/<built-in>"
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

ZEN_Timer_Queue * ZEN_Timer_Queue::instance() {
}

void ZEN_Timer_Queue::clean_instance() {
}

void ZEN_Timer_Queue::instance(ZEN_Timer_Queue *pinstatnce) {
}

void std::fill<__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >, int>(/*anon struct*/ int __first, /*anon struct*/ int __last, const int &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_aux<ZEN_Timer_Queue::ZEN_TIMER_NODE*, unsigned int, ZEN_Timer_Queue::ZEN_TIMER_NODE>(ZEN_Timer_Queue::ZEN_TIMER_NODE *__first, unsigned int __n, const ZEN_Timer_Queue::ZEN_TIMER_NODE &__x, __false_type arg3) {
    // local: ZEN_Timer_Queue::ZEN_TIMER_NODE *__cur;
}

void std::__uninitialized_fill_n_a<ZEN_Timer_Queue::ZEN_TIMER_NODE*, unsigned int, ZEN_Timer_Queue::ZEN_TIMER_NODE, ZEN_Timer_Queue::ZEN_TIMER_NODE>(ZEN_Timer_Queue::ZEN_TIMER_NODE *__first, unsigned int __n, const ZEN_Timer_Queue::ZEN_TIMER_NODE &__x, /*anon struct*/ int arg3) {
}

void std::__uninitialized_fill_n_aux<__gnu_cxx::__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*, std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > >, unsigned int, ZEN_Timer_Queue::ZEN_TIMER_NODE>(/*anon struct*/ int __first, unsigned int __n, const ZEN_Timer_Queue::ZEN_TIMER_NODE &__x, __false_type arg3) {
    // local: /*anon struct*/ int __cur;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*, std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > >, unsigned int, ZEN_Timer_Queue::ZEN_TIMER_NODE, ZEN_Timer_Queue::ZEN_TIMER_NODE>(/*anon struct*/ int __first, unsigned int __n, const ZEN_Timer_Queue::ZEN_TIMER_NODE &__x, /*anon struct*/ int arg3) {
}

int * std::fill_n<int*, unsigned int, int>(int *__first, unsigned int __n, const int &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<int*, unsigned int, int, int>(int *__first, unsigned int __n, const int &__x, /*anon struct*/ int arg3) {
}

/*anon struct*/ int std::fill_n<__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >, unsigned int, int>(/*anon struct*/ int __first, unsigned int __n, const int &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >, unsigned int, int, int>(/*anon struct*/ int __first, unsigned int __n, const int &__x, /*anon struct*/ int arg3) {
}

__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > erase(__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > __first, __normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > __last) {
    // local: /*anon struct*/ int __i;
}

int ZEN_Timer_Queue::get_first_timeout(ZEN_Time_Value *first_timeout) {
    // local: int ret;
    // local: int time_node_id;
    // local: ZEN_Time_Value now_time;
    // local: uint64_t now_trigger_msec;
    // local: uint64_t first_timeout_msec;
    // local: ZEN_Time_Value cpu_tick;
}

void ZEN_Timer_Queue::calc_next_trigger(int time_node_id, uint64_t now_trigger_msec, bool &continue_trigger) {
    // local: ZEN_Timer_Queue::ZEN_TIMER_NODE *prc_time_node;
}

int ZEN_Timer_Queue::cancel_timer(const ZEN_Timer_Handler *timer_hdl) {
    // local: bool cancel_time;
    // local: size_t i;
    // local: uint32_t time_id;
}

int ZEN_Timer_Queue::close() {
    // local: size_t i;
    // local: ZEN_Timer_Handler *time_hdl;
}

int ZEN_Timer_Queue::free_timernode(int time_node_id) {
}

int ZEN_Timer_Queue::cancel_timer(int timer_id) {
}

int ZEN_Timer_Queue::alloc_timernode(ZEN_Timer_Handler *timer_hdl, const void *action, const ZEN_Time_Value &delay_time, const ZEN_Time_Value &interval_time, int &time_node_id, ZEN_Timer_Queue::ZEN_TIMER_NODE *&alloc_time_node) {
    // local: int ret;
    // local: size_t old_num_node;
}

__normal_iterator<int*,std::vector<int, std::allocator<int> > > erase(__normal_iterator<int*,std::vector<int, std::allocator<int> > > __first, __normal_iterator<int*,std::vector<int, std::allocator<int> > > __last) {
    // local: /*anon struct*/ int __i;
}

int ZEN_Timer_Queue::initialize(size_t num_timer_node, unsigned int timer_precision_mesc, ZEN_Timer_Queue::TRIGGER_MODE trigger_mode, bool dynamic_expand_node) {
    // local: size_t old_num_node;
    // local: ZEN_Time_Value cpu_tick;
    // local: ZEN_Time_Value now_time;
}

size_t ZEN_Timer_Queue::expire() {
    // local: ZEN_Time_Value now_time;
    // local: uint64_t now_trigger_msec;
    // local: ZEN_Time_Value cpu_tick;
}

void std::fill<__gnu_cxx::__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*, std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > >, ZEN_Timer_Queue::ZEN_TIMER_NODE>(/*anon struct*/ int __first, /*anon struct*/ int __last, const ZEN_Timer_Queue::ZEN_TIMER_NODE &__value) {
    // local: const bool __scalar;
}

void _M_fill_insert(__normal_iterator<ZEN_Timer_Queue::ZEN_TIMER_NODE*,std::vector<ZEN_Timer_Queue::ZEN_TIMER_NODE, std::allocator<ZEN_Timer_Queue::ZEN_TIMER_NODE> > > __position, unsigned int __n, const ZEN_Timer_Queue::ZEN_TIMER_NODE &__x) {
    // local: ZEN_Timer_Queue::ZEN_TIMER_NODE __x_copy;
    // local: const size_t __elems_after;
    // local: /*anon struct*/ int __old_finish;
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
}

void _M_fill_insert(__normal_iterator<int*,std::vector<int, std::allocator<int> > > __position, unsigned int __n, const int &__x) {
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
    // local: int __x_copy;
    // local: const size_t __elems_after;
    // local: /*anon struct*/ int __old_finish;
}

int ZEN_Timer_Queue::extend_node(size_t num_timer_node, size_t &old_num_node) {
    // local: size_t i;
    // local: size_t i;
}

