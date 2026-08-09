// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/protocol/common/comm_proto_public_logsvr.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/protocol/common/comm_proto_public_logsvr.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrBufUtil.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrTime.h"
#include "src/protocol/common/<built-in>"
#include <_G_config.h>
#include <alloca.h>
#include <arpa/inet.h>
#include <asm/socket.h>
#include <asm/sockios.h>
#include <assert.h>
#include <bits/byteswap.h>
#include <bits/confname.h>
#include <bits/endian.h>
#include <bits/environments.h>
#include <bits/in.h>
#include <bits/local_lim.h>
#include <bits/posix1_lim.h>
#include <bits/posix2_lim.h>
#include <bits/posix_opt.h>
#include <bits/pthreadtypes.h>
#include <bits/select.h>
#include <bits/sigset.h>
#include <bits/sockaddr.h>
#include <bits/socket.h>
#include <bits/stdio.h>
#include <bits/stdio_lim.h>
#include <bits/sys_errlist.h>
#include <bits/time.h>
#include <bits/types.h>
#include <bits/typesizes.h>
#include <bits/uio.h>
#include <bits/waitflags.h>
#include <bits/waitstatus.h>
#include <bits/wchar.h>
#include <bits/wordsize.h>
#include <bits/xopen_lim.h>
#include <cassert>
#include <cctype>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <ctype.h>
#include <endian.h>
#include <exception>
#include <features.h>
#include <gconv.h>
#include <getopt.h>
#include <gnu/stubs-32.h>
#include <gnu/stubs.h>
#include <i586-suse-linux/bits/c++config.h>
#include <i586-suse-linux/bits/cpu_defines.h>
#include <i586-suse-linux/bits/os_defines.h>
#include <inttypes.h>
#include <libio.h>
#include <limits.h>
#include <linux/limits.h>
#include <netinet/in.h>
#include <new>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <syslimits.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>
#include <xlocale.h>

// line 227
tsf4g_tdr::TdrError::ErrorType visualize(/*anon struct*/ int &destBuf, int indent, char separator) {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 133
tsf4g_tdr::TdrError::ErrorType unpack(/*anon struct*/ int &srcBuf, unsigned int cutVer) {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
    // local: uint32_t size4string_buffer_;
    // local: size_t realSize4string_buffer_;
}

// line 114
tsf4g_tdr::TdrError::ErrorType unpack(const char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) {
    // local: /*anon struct*/ int srcBuf;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 25
tsf4g_tdr::TdrError::ErrorType construct() {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 54
tsf4g_tdr::TdrError::ErrorType pack(/*anon struct*/ int &destBuf, unsigned int cutVer) {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
    // local: const size_t sizePos4string_buffer_;
    // local: const size_t beginPos4string_buffer_;
    // local: size_t realSize4string_buffer_;
    // local: const size_t size4string_buffer_;
}

// line 270
const char * visualize_ex(/*anon struct*/ int &destBuf, int indent, char separator) {
    // local: size_t targetPos;
    // local: const size_t totalSize;
}

// line 35
tsf4g_tdr::TdrError::ErrorType pack(char *buffer, size_t size, size_t *usedSize, unsigned int cutVer) {
    // local: /*anon struct*/ int destBuf;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 243
const char * visualize_ex(char *buffer, size_t size, size_t *usedSize, int indent, char separator) {
    // local: /*anon struct*/ int destBuf;
    // local: size_t targetPos;
    // local: const size_t totalSize;
}

// line 208
tsf4g_tdr::TdrError::ErrorType visualize(char *buffer, size_t size, size_t *usedSize, int indent, char separator) {
    // local: /*anon struct*/ int destBuf;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

