// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/TdrParse.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/protocol/common/TdrParse.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrTypeUtil.h"
#include "src/protocol/common/TdrBuf.h"
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

// line 1649
tsf4g_tdr::TdrError::ErrorType parseIP(tdr_ip_t *dest, unsigned int count, const char *src, unsigned int *parsed, const char *_default) {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: unsigned int i;
}

// line 1603
tsf4g_tdr::TdrError::ErrorType parseIP(tdr_ip_t &dest, const char *src, unsigned int *parsed, const char *_default) {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
}

// line 1546
tsf4g_tdr::TdrError::ErrorType parseTime(tdr_time_t *dest, unsigned int count, const char *src, unsigned int *parsed, const char *_default) {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: unsigned int i;
}

// line 1500
tsf4g_tdr::TdrError::ErrorType parseTime(tdr_time_t &dest, const char *src, unsigned int *parsed, const char *_default) {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
}

// line 1443
tsf4g_tdr::TdrError::ErrorType parseDate(tdr_date_t *dest, unsigned int count, const char *src, unsigned int *parsed, const char *_default) {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: unsigned int i;
}

// line 1397
tsf4g_tdr::TdrError::ErrorType parseDate(tdr_date_t &dest, const char *src, unsigned int *parsed, const char *_default) {
    // local: tsf4g_tdr::TdrError::ErrorType ret;
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
}

// line 1356
tsf4g_tdr::TdrError::ErrorType parseDouble(double *dest, unsigned int count, const char *src, unsigned int *parsed, double _default) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
}

// line 952
tsf4g_tdr::TdrError::ErrorType parseInt32(int32_t *dest, unsigned int count, const char *src, unsigned int *parsed, int32_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
    // local: int64_t int64bit;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 1072
tsf4g_tdr::TdrError::ErrorType parseUInt64(uint64_t *dest, unsigned int count, const char *src, unsigned int *parsed, uint64_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
    // local: int32_t int32bit;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 79
tsf4g_tdr::TdrError::ErrorType parseUInt8(uint8_t *dest, unsigned int count, const char *src, unsigned int *parsed, uint8_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 1018
tsf4g_tdr::TdrError::ErrorType parseUInt64(uint64_t &dest, const char *src, unsigned int *parsed, uint64_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
    // local: int32_t int32bit;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 1322
tsf4g_tdr::TdrError::ErrorType parseDouble(double &dest, const char *src, unsigned int *parsed, double _default) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
}

// line 1281
tsf4g_tdr::TdrError::ErrorType parseFloat(float *dest, unsigned int count, const char *src, unsigned int *parsed, float _default) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
}

// line 1247
tsf4g_tdr::TdrError::ErrorType parseFloat(float &dest, const char *src, unsigned int *parsed, float _default) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
}

// line 759
tsf4g_tdr::TdrError::ErrorType parseUInt32(uint32_t &dest, const char *src, unsigned int *parsed, uint32_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
    // local: int64_t int64bit;
    // local: int32_t int32bit;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 890
tsf4g_tdr::TdrError::ErrorType parseInt32(int32_t &dest, const char *src, unsigned int *parsed, int32_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
    // local: int64_t int64bit;
    // local: int32_t int32bit;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 821
tsf4g_tdr::TdrError::ErrorType parseUInt32(uint32_t *dest, unsigned int count, const char *src, unsigned int *parsed, uint32_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
    // local: int64_t int64bit;
    // local: int32_t int32bit;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 1187
tsf4g_tdr::TdrError::ErrorType parseInt64(int64_t *dest, unsigned int count, const char *src, unsigned int *parsed, int64_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
    // local: int32_t int32bit;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 1133
tsf4g_tdr::TdrError::ErrorType parseInt64(int64_t &dest, const char *src, unsigned int *parsed, int64_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: char *temp;
    // local: int32_t int32bit;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 634
tsf4g_tdr::TdrError::ErrorType parseInt16(int16_t &dest, const char *src, unsigned int *parsed, int16_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 568
tsf4g_tdr::TdrError::ErrorType parseUInt16(uint16_t *dest, unsigned int count, const char *src, unsigned int *parsed, uint16_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 693
tsf4g_tdr::TdrError::ErrorType parseInt16(int16_t *dest, unsigned int count, const char *src, unsigned int *parsed, int16_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 20
tsf4g_tdr::TdrError::ErrorType parseUInt8(uint8_t &dest, const char *src, unsigned int *parsed, uint8_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 509
tsf4g_tdr::TdrError::ErrorType parseUInt16(uint16_t &dest, const char *src, unsigned int *parsed, uint16_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 446
tsf4g_tdr::TdrError::ErrorType parseChar(char *dest, unsigned int count, const char *src, unsigned int *parsed, char _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 387
tsf4g_tdr::TdrError::ErrorType parseChar(char &dest, const char *src, unsigned int *parsed, char _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 323
tsf4g_tdr::TdrError::ErrorType parseUChar(unsigned char *dest, unsigned int count, const char *src, unsigned int *parsed, unsigned char _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 264
tsf4g_tdr::TdrError::ErrorType parseUChar(unsigned char &dest, const char *src, unsigned int *parsed, unsigned char _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 201
tsf4g_tdr::TdrError::ErrorType parseInt8(int8_t *dest, unsigned int count, const char *src, unsigned int *parsed, int8_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

// line 142
tsf4g_tdr::TdrError::ErrorType parseInt8(int8_t &dest, const char *src, unsigned int *parsed, int8_t _default, tsf4g_tdr::TdrError::ErrorType (**macro)(int32_t &arg0, const char *arg1, const char *arg2), const char *groupName) {
    // local: unsigned int parsedCount;
    // local: char *context;
    // local: char *segment;
    // local: int32_t int32bit;
    // local: char *temp;
    // local: tsf4g_tdr::TdrError::ErrorType ret;
}

