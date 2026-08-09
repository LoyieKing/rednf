// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/zenlib/zen_shm_lockfree_deque.cpp
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
#include "src/commlib/zenlib/zen_shm_lockfree_deque.h"
#include "src/commlib/zenlib/zen_shm_predefine.h"
#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_os_adapt_thread.h"
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

size_t getallocsize(size_t szdeque) {
}

void snap_getpoint(size_t &pstart, size_t &pend) {
}

size_t freesize() {
    // local: size_t pstart;
    // local: size_t pend;
    // local: size_t szfree;
}

bool empty() {
}

bool full() {
}

bool push_end(const /*anon struct*/ int *node) {
    // local: char *pend;
    // local: size_t first;
    // local: size_t second;
}

void clear() {
}

// line 20
void * operator new(size_t arg0, size_t nodelen) {
    // local: void *ptr;
}

/*anon struct*/ int * initialize(size_t size_of_deque, size_t max_len_node, char *pmmap, bool if_restore, bool if_check_pthread) {
    // local: /*anon struct*/ int *dequechunk_head;
    // local: /*anon struct*/ int *dequechunk;
}

bool read_front(/*anon struct*/ int *node) {
    // local: char *pbegin;
    // local: size_t tmplen;
    // local: size_t first;
    // local: size_t second;
}

bool read_front_new(/*anon struct*/ int *&new_node) {
    // local: size_t tmplen;
}

bool pop_front(/*anon struct*/ int *node) {
    // local: char *pbegin;
    // local: size_t tmplen;
    // local: size_t first;
    // local: size_t second;
}

bool pop_front_new(/*anon struct*/ int *&new_node) {
    // local: size_t tmplen;
}

bool discard_frond() {
    // local: char *pbegin;
    // local: size_t tmplen;
    // local: size_t first;
    // local: size_t second;
}

bool read_front_ptr(const /*anon struct*/ int *&node_ptr) {
    // local: char *pbegin;
    // local: size_t tmplen;
    // local: size_t first;
    // local: size_t second;
}

