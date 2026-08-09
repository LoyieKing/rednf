// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/TdrTypeUtil.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrError.h"
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

// line 249
size_t wstrlen(const tdr_wchar_t *wstr) {
    // local: size_t len;
}

// line 137
tsf4g_tdr::TdrError::ErrorType str2TdrDate(tdr_date_t &dest, const char *pszDate) {
    // local: tm stTm;
    // local: /*anon struct*/ int tdrDate;
}

// line 114
tsf4g_tdr::TdrError::ErrorType str2TdrTime(tdr_time_t &dest, const char *pszTime) {
    // local: tm stTm;
    // local: /*anon struct*/ int tdrTime;
}

// line 89
tsf4g_tdr::TdrError::ErrorType str2TdrIP(tdr_ip_t &dest, const char *pszIP) {
    // local: in_addr addr;
}

// line 60
tsf4g_tdr::TdrError::ErrorType UTC2TdrDateTime(tdr_datetime_t &dest, time_t utcTime) {
    // local: tm *pstTm;
    // local: tm stTm;
    // local: /*anon struct*/ int tdrDateTime;
}

// line 18
tsf4g_tdr::TdrError::ErrorType tdrIp2Str(char *dest, size_t size, tdr_ip_t src) {
    // local: in_addr addr;
}

tsf4g_tdr::TdrError::ErrorType textize(const char *format) {
    // local: va_list ap;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 39
tsf4g_tdr::TdrError::ErrorType tdrIp2Str(/*anon struct*/ int &dest, tdr_ip_t src) {
    // local: in_addr addr;
    // local: char dotIP[];
}

// line 221
int compareTdrDate(tdr_date_t left, tdr_date_t right) {
}

// line 207
int compareTdrTime(tdr_time_t left, tdr_time_t right) {
}

// line 183
tsf4g_tdr::TdrError::ErrorType tdrDateTime2UTC(time_t &dest, tdr_datetime_t src) {
    // local: tm stTm;
    // local: /*anon struct*/ int tdrDateTime;
    // local: time_t utcTime;
}

// line 160
tsf4g_tdr::TdrError::ErrorType str2TdrDateTime(tdr_datetime_t &dest, const char *pszDateTime) {
    // local: tm stTm;
    // local: /*anon struct*/ int tdrDateTime;
}

// line 235
int compareTdrDateTime(tdr_datetime_t left, tdr_datetime_t right) {
}

