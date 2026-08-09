// Auto-generated stub from DWARF info
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/protocol/common/TdrXml.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "src/protocol/common/TdrXml.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrError.h"
#include "src/protocol/common/TdrIO.h"
#include "src/protocol/common/TdrBuf.h"
#include "import/include/opensource/rapidxml/rapidxml/rapidxml.hpp"
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

const char * what() {
}

// line 129
void stepOut(const char *name) {
}

tsf4g_tdr::TdrError::ErrorType textize(const char *format) {
    // local: va_list ap;
}

// line 455
tsf4g_tdr::TdrError::ErrorType textizeDateTime(tdr_datetime_t src, bool space) {
    // local: char temp[];
}

// line 255
tsf4g_tdr::TdrError::ErrorType closeSimple(const char *name) {
}

// line 395
tsf4g_tdr::TdrError::ErrorType textizeIP(tdr_ip_t src, bool space) {
    // local: char temp[];
}

// line 322
tsf4g_tdr::TdrError::ErrorType textizeStr(const char *src, bool space) {
    // local: const char *head;
    // local: const char *tail;
    // local: size_t length;
    // local: const char *replace;
}

char * tsf4g_tdr::strip(char *head, char *tail) {
}

// line 276
tsf4g_tdr::TdrError::ErrorType openComplex(const char *name) {
}

// line 220
tsf4g_tdr::TdrError::ErrorType openSimple(const char *name) {
}

// line 300
tsf4g_tdr::TdrError::ErrorType closeComplex(const char *name) {
}

// line 435
tsf4g_tdr::TdrError::ErrorType textizeTime(tdr_time_t src, bool space) {
    // local: char temp[];
}

// line 415
tsf4g_tdr::TdrError::ErrorType textizeDate(tdr_date_t src, bool space) {
    // local: char temp[];
}

// line 178
const char * getAttrValue(const char *name) {
    // local: xml_attribute<char> *attribute;
    // local: char *value;
    // local: char *tail;
}

// line 103
tsf4g_tdr::TdrXmlReader::WalkState stepIn(const char *name) {
    // local: xml_node<char> *tempNode;
}

// line 154
const char * getNodeValue(const char *name) {
    // local: tsf4g_tdr::TdrXmlReader::WalkState state;
    // local: char *value;
    // local: char *tail;
}

// line 140
const char * getEntryValue(const char *name) {
}

xml_node<char> * parse_element<0>(char *&text) {
    // local: xml_node<char> *element;
    // local: char *name;
}

xml_node<char> * parse_node<0>(char *&text) {
}

// line 78
tsf4g_tdr::TdrError::ErrorType _parse(char *string) {
    // local: /*anon struct*/ int &const error;
}

