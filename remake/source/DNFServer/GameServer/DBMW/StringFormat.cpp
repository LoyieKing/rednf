// df_dbmw_r - StringFormat (ORIG StringFormat.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

StackBuffer_char sformat(const char* fmt, ...)
{
    char buf[0x200];
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buf, 0x200, fmt, ap);
    if (len >= 0 && (unsigned int)len <= 0x1ff)
    {
        StackBuffer_char tmp = StackBuffer_char::alloc(len + 1);
        memcpy(tmp.getBuffer(), buf, len + 1);
        va_end(ap);
        return tmp;
    }
    else
    {
    len = vsnprintf(0, 0, fmt, ap);
    StackBuffer_char tmp = StackBuffer_char::alloc(len + 1);
    vsnprintf(tmp.getBuffer(), len + 1, fmt, ap);
    va_end(ap);
    return tmp;
    }
}
StackBuffer_wchar wformat(const wchar_t* fmt, ...)
{
    wchar_t buf[0x200];
    va_list ap;
    va_start(ap, fmt);
    int len = vswprintf(buf, 0x200, fmt, ap);
    if (len >= 0 && (unsigned int)len <= 0x1ff)
    {
        StackBuffer_wchar tmp = StackBuffer_wchar::alloc((len + 1) * 4);
        memcpy(tmp.getBuffer(), buf, (len + 1) * 4);
        va_end(ap);
        return tmp;
    }
    else
    {
    len = vswprintf((wchar_t*)0, 0, fmt, ap);
    StackBuffer_wchar tmp = StackBuffer_wchar::alloc((len + 1) * 4);
    vswprintf((wchar_t*)tmp.getBuffer(), len + 1, fmt, ap);
    va_end(ap);
    return tmp;
    }
}
