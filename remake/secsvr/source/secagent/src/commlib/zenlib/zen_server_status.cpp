// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/zenlib/zen_server_status.cpp
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
#include "src/commlib/zenlib/zen_trace_log_debug.h"
#include "src/commlib/zenlib/zen_trace_log_msg.h"
#include "src/commlib/zenlib/zen_trace_log_basic.h"
#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_lock_base.h"
#include "src/commlib/zenlib/zen_lock_guard.h"
#include "src/commlib/zenlib/zen_time_value.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_os_adapt_time.h"
#include "src/commlib/zenlib/zen_server_status.h"
#include "src/commlib/zenlib/zen_share_mem_posix.h"
#include "src/commlib/zenlib/zen_shm_vector.h"
#include "src/commlib/zenlib/zen_shm_predefine.h"
#include "src/commlib/zenlib/zen_lock_null_lock.h"
#include "src/commlib/zenlib/zen_lock_ptr_guard.h"
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

// line 35
bool ZEN_STATUS_ITEM_ID::operator==(const ZEN_STATUS_ITEM_ID &others) {
}

uint32_t ZEN_Server_Status::get_copy_time() {
}

void ZEN_Server_Status::get_stat_head(ZEN_STATUS_HEAD *stat_head) {
}

void ZEN_Server_Status::report_monitor_time(uint32_t report_time) {
}

void ZEN_Server_Status::clean_instance() {
}

void ZEN_Server_Status::instance(ZEN_Server_Status *pinstatnce) {
}

const long unsigned int * std::lower_bound<const long unsigned int*, long unsigned int>(const long unsigned int *__first, const long unsigned int *__last, const long unsigned int &__val) {
    // local: ptrdiff_t __len;
    // local: ptrdiff_t __half;
    // local: const long unsigned int *__middle;
}

void std::fill<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > >, __gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*>(/*anon struct*/ int __first, /*anon struct*/ int __last, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_aux<ZEN_STATUS_ITEM_WITHNAME*, unsigned int, ZEN_STATUS_ITEM_WITHNAME>(ZEN_STATUS_ITEM_WITHNAME *__first, unsigned int __n, const ZEN_STATUS_ITEM_WITHNAME &__x, __false_type arg3) {
    // local: ZEN_STATUS_ITEM_WITHNAME *__cur;
}

void std::__uninitialized_fill_n_a<ZEN_STATUS_ITEM_WITHNAME*, unsigned int, ZEN_STATUS_ITEM_WITHNAME, ZEN_STATUS_ITEM_WITHNAME>(ZEN_STATUS_ITEM_WITHNAME *__first, unsigned int __n, const ZEN_STATUS_ITEM_WITHNAME &__x, /*anon struct*/ int arg3) {
}

void std::__uninitialized_fill_n_aux<__gnu_cxx::__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > >, unsigned int, ZEN_STATUS_ITEM_WITHNAME>(/*anon struct*/ int __first, unsigned int __n, const ZEN_STATUS_ITEM_WITHNAME &__x, __false_type arg3) {
    // local: /*anon struct*/ int __cur;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > >, unsigned int, ZEN_STATUS_ITEM_WITHNAME, ZEN_STATUS_ITEM_WITHNAME>(/*anon struct*/ int __first, unsigned int __n, const ZEN_STATUS_ITEM_WITHNAME &__x, /*anon struct*/ int arg3) {
}

_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > ** std::fill_n<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*>(_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > **__first, unsigned int __n, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, __gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*>(_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > **__first, unsigned int __n, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &__x, /*anon struct*/ int arg3) {
}

/*anon struct*/ int std::fill_n<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*>(/*anon struct*/ int __first, unsigned int __n, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &__value) {
    // local: const bool __scalar;
}

void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, __gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*>(/*anon struct*/ int __first, unsigned int __n, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &__x, /*anon struct*/ int arg3) {
}

void std::fill<__gnu_cxx::__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > >, ZEN_STATUS_ITEM_WITHNAME>(/*anon struct*/ int __first, /*anon struct*/ int __last, const ZEN_STATUS_ITEM_WITHNAME &__value) {
    // local: const bool __scalar;
}

void ZEN_Server_Status::modify_multi_thread_guard(bool multi_thread) {
}

void ZEN_Server_Status::copy_stat_counter() {
    // local: size_t num_of_counter;
    // local: ZEN_STATUS_ITEM *stat_sandy_begin;
    // local: ZEN_STATUS_ITEM *stat_mandy_begin;
}

size_t ZEN_Server_Status::num_of_counter() {
}

__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > erase(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > __first, __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > __last) {
    // local: /*anon struct*/ int __i;
}

void reserve(unsigned int __n) {
    // local: const size_t __old_size;
    // local: _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > **__tmp;
}

void clear() {
    // local: size_t __i;
    // local: _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *__cur;
    // local: _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *__next;
}

int ZEN_Server_Status::initialize(const char *stat_filename, bool restore_mmap, bool multi_thread) {
    // local: int ret;
    // local: size_t size_alloc;
    // local: int open_mode;
    // local: char *stat_ptr;
    // local: char *copy_ptr;
}

int ZEN_Server_Status::initialize(const char *stat_filename) {
    // local: int ret;
}

void _M_fill_insert(__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >**,std::vector<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> >*> > > __position, unsigned int __n, _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *const &__x) {
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
    // local: _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *__x_copy;
    // local: const size_t __elems_after;
    // local: /*anon struct*/ int __old_finish;
}

void _M_fill_insert(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > __position, unsigned int __n, const ZEN_STATUS_ITEM_WITHNAME &__x) {
    // local: ZEN_STATUS_ITEM_WITHNAME __x_copy;
    // local: const size_t __elems_after;
    // local: /*anon struct*/ int __old_finish;
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
}

void _M_insert_aux(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > __position, const ZEN_STATUS_ITEM_WITHNAME &__x) {
    // local: ZEN_STATUS_ITEM_WITHNAME __x_copy;
    // local: const size_t __old_size;
    // local: size_t __len;
    // local: /*anon struct*/ int __new_start;
    // local: /*anon struct*/ int __new_finish;
}

void reserve(unsigned int __n) {
    // local: const size_t __old_size;
    // local: ZEN_STATUS_ITEM_WITHNAME *__tmp;
}

void ZEN_Server_Status::add_status_item(size_t num_add_stat_item, const ZEN_STATUS_ITEM_WITHNAME *item_ary) {
    // local: size_t num_old_stat_item;
    // local: size_t i;
}

void ZEN_Server_Status::dump_status_info(ostringstream &strstream, bool dump_copy) {
    // local: size_t num_of_counter;
    // local: size_t cfg_size;
    // local: ZEN_STATUS_ITEM *stat_process_iter;
    // local: char statics_item_name[];
    // local: ZEN_Lock_Ptr_Guard guard;
    // local: size_t i;
    // local: size_t j;
}

void ZEN_Server_Status::dump_all(/*anon struct*/ int &array_status, bool dump_copy) {
    // local: ZEN_Lock_Ptr_Guard guard;
    // local: size_t cfg_size;
    // local: size_t num_of_counter;
    // local: size_t i;
    // local: size_t j;
}

void ZEN_Server_Status::dump_status_info(bool dump_copy) {
    // local: size_t num_of_counter;
    // local: ZEN_STATUS_ITEM *stat_process_iter;
    // local: size_t cfg_size;
    // local: char statics_item_name[];
    // local: ZEN_Lock_Ptr_Guard guard;
    // local: size_t i;
    // local: size_t j;
}

void ZEN_Server_Status::check_overtime(time_t now_time) {
    // local: int clear_type;
    // local: size_t num_of_counter;
    // local: ZEN_Lock_Ptr_Guard guard;
    // local: size_t i;
    // local: ZEN_STATUS_ITEM *cur_item;
}

uint64_t ZEN_Server_Status::get_counter(unsigned int statics_id, unsigned int app_id, unsigned int classify_id) {
    // local: ZEN_STATUS_ITEM_ID stat_item_id;
    // local: /*anon struct*/ int iter_tmp;
    // local: ZEN_Lock_Ptr_Guard guard;
    // local: size_t index;
}

void resize(unsigned int __num_elements_hint) {
    // local: const size_t __old_n;
    // local: const size_t __n;
    // local: /*anon struct*/ int __tmp;
    // local: size_t __bucket;
    // local: _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *__first;
    // local: size_t __new_bucket;
}

/*anon struct*/ int & find_or_insert(const /*anon struct*/ int &__obj) {
    // local: size_t __n;
    // local: _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *__first;
    // local: _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *__tmp;
    // local: _Hashtable_node<std::pair<const ZEN_STATUS_ITEM_ID, unsigned int> > *__cur;
}

int ZEN_Server_Status::find_insert_idx(unsigned int statics_id, unsigned int app_id, unsigned int classify_id, size_t *sandy_idx) {
    // local: ZEN_STATUS_ITEM_ID stat_item_id;
    // local: /*anon struct*/ int iter_tmp;
    // local: size_t v;
    // local: size_t idx;
    // local: ZEN_STATUS_ITEM status_item;
}

int ZEN_Server_Status::set_by_statid(unsigned int statics_id, unsigned int app_id, unsigned int classify_id, uint64_t set_value) {
    // local: int ret;
    // local: size_t sandy_idx;
    // local: ZEN_Lock_Ptr_Guard guard;
}

int ZEN_Server_Status::increase_by_statid(unsigned int statics_id, unsigned int app_id, unsigned int classify_id, int64_t incre_value) {
    // local: int ret;
    // local: size_t sandy_idx;
    // local: ZEN_Lock_Ptr_Guard guard;
}

int ZEN_Server_Status::initialize(const char *stat_filename, size_t num_stat_item, const ZEN_STATUS_ITEM_WITHNAME *item_ary, bool multi_thread) {
    // local: int ret;
}

ZEN_Server_Status * ZEN_Server_Status::instance() {
}

