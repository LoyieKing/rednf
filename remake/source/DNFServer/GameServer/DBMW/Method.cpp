// df_dbmw_r - Method (ORIG Method.cpp)
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

// ---- CSHA：SHA-256 ----
IMethod::IMethod()
{
    m_initialized = 0;
    m_blockSize = 0;
    m_keyLength = 0;
    m_mode = 0;
    m_padding = 0;
}
IMethod::~IMethod() {}
int IMethod::Xor(char* a, char const* b)
{
    if (m_initialized == 1)
        return 0x70000005;
    for (int i = 0; i < m_blockSize; i++)
        a[i] ^= b[i];
    return 0x6fffffff;
}
int IMethod::SetMode(int mode)
{
    if (m_initialized == 1)
        return 0x70000005;
    if (mode < 0 || mode > 2)
        return 0x70000003;
    m_mode = mode;
    return 0x6fffffff;
}
int IMethod::SetPadding(int padding)
{
    if (m_initialized == 1)
        return 0x70000005;
    if (padding < 0 || padding > 2)
        return 0x70000004;
    m_padding = padding;
    return 0x6fffffff;
}
int IMethod::GetKeyLength(unsigned int* out)
{
    if (m_initialized == 1)
    {
        if (out)
            *out = 0x70000005;
        return 0;
    }
    return m_keyLength;
}
int IMethod::GetBlockSize(unsigned int* out)
{
    if (m_initialized == 1)
    {
        if (out)
            *out = 0x70000005;
        return 0;
    }
    return m_blockSize;
}
int IMethod::GetMode(unsigned int* out)
{
    if (m_initialized == 1)
    {
        if (out)
            *out = 0x70000005;
        return 0;
    }
    return m_mode;
}
int IMethod::GetPadding(unsigned int* out)
{
    if (m_initialized == 1)
    {
        if (out)
            *out = 0x70000005;
        return 0;
    }
    return m_padding;
}
int IMethod::Pad(char* data, int len, unsigned int* out)
{
    if (m_initialized == 1)
    {
        if (out)
            *out = 0x70000005;
        return 0;
    }
    int rem = len % m_blockSize;
    if (rem == 0)
        return (int)data;
    int padLen = m_blockSize - rem;
    char* dst = data + len;
    switch (m_padding)
    {
    case 0:
        for (int i = 0; i < padLen; i++)
            dst[i] = 0;
        break;
    case 1:
        for (int i = 0; i < padLen; i++)
            dst[i] = 0x20;
        break;
    case 2:
        for (int i = 0; i < padLen; i++)
            dst[i] = (char)padLen;
        break;
    default:
        break;
    }
    return (int)(data + padLen);
}
void IMethod::BytesToWord(unsigned char const* src, unsigned int& dst)
{
    dst = 0;
    dst |= (unsigned int)src[0] << 24;
    dst |= (unsigned int)src[1] << 16;
    dst |= (unsigned int)src[2] << 8;
    dst |= (unsigned int)src[3];
}
void IMethod::WordToBytes(unsigned int src, unsigned char* dst)
{
    dst[0] = (unsigned char)(src >> 24);
    dst[1] = (unsigned char)(src >> 16);
    dst[2] = (unsigned char)(src >> 8);
    dst[3] = (unsigned char)src;
}
