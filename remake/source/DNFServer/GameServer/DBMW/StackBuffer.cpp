// df_dbmw_r - StackBuffer (ORIG StackBuffer.cpp)
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

StackBufferContext::StackBufferContext() {}
StackBufferContext::~StackBufferContext() {}

// .tbss 布局与 ORIG 一致（GS-0x4）：eh_globals.o 显式链接在 DNFFunctionLib.o
// 与 StackBuffer.o 之间（见 cmake/dbmw/CMakeLists.txt），使 libstdc++ 的
// eh_global 排在 g_stackBufferContext 之前。必须是 static（LOCAL）才能生成
// 直接 GS 寻址 mov %gs:0xfffffffc,%eax。
static __thread StackBufferContext* g_stackBufferContext;

static bool allocStackBuffer(unsigned int size, unsigned char** buf, int* end)
{
    if (!g_stackBufferContext)
    {
        g_stackBufferContext = new StackBufferContext;
        g_stackBufferContext->m_buffers.reserve(0x20);
        g_stackBufferContext->m_blocks.reserve(8);
        g_stackBufferContext->m_blocks.push_back(
            new unsigned char[0x4000]);
        g_stackBufferContext->m_blockIndex = 0;
        g_stackBufferContext->m_offset = 0;
    }
    StackBufferContext::Buffer b;
    b.m_blockIndex = 0;
    b.m_offset = 0;
    b.m_size = 0;
    g_stackBufferContext->m_buffers.push_back(
        (StackBufferContext::Buffer&&)b);
    StackBufferContext::Buffer& cur = g_stackBufferContext->m_buffers.back();
    cur.m_blockIndex = 0;
    cur.m_offset = 0;
    cur.m_size = size;
    if (size > 0x4000)
    {
        *buf = new unsigned char[size];
        *end = -1;
        return *buf != 0;
    }
    if (g_stackBufferContext->m_offset + size > 0x4000)
    {
        register int idx = g_stackBufferContext->m_blockIndex + 1;
        if (idx == (int)g_stackBufferContext->m_blocks.size())
        {
            g_stackBufferContext->m_blocks.push_back(
                new unsigned char[0x4000]);
        }
        cur.m_blockIndex = g_stackBufferContext->m_blockIndex + 1;
        cur.m_offset = 0;
        g_stackBufferContext->m_blockIndex =
            g_stackBufferContext->m_blockIndex + 1;
        g_stackBufferContext->m_offset = size;
    }
    else
    {
        cur.m_blockIndex = g_stackBufferContext->m_blockIndex;
        cur.m_offset = g_stackBufferContext->m_offset;
        g_stackBufferContext->m_offset += size;
    }
    *buf = g_stackBufferContext->m_blocks[cur.m_blockIndex] + cur.m_offset;
    *end = g_stackBufferContext->m_buffers.size() - 1;
    return 1;
}
static void freeStackBuffer(unsigned char* buf, int end)
{
    if (end == -1)
    {
        delete[] buf;
        return;
    }
    g_stackBufferContext->m_buffers.erase(
        g_stackBufferContext->m_buffers.begin() + end);
    if (g_stackBufferContext->m_buffers.empty())
    {
        g_stackBufferContext->m_blockIndex = 0;
        g_stackBufferContext->m_offset = 0;
    }
    else
    {
        StackBufferContext::Buffer& b =
            g_stackBufferContext->m_buffers.back();
        g_stackBufferContext->m_blockIndex = b.m_blockIndex;
        g_stackBufferContext->m_offset = b.m_offset + b.m_size;
    }
}
static void freeAllStackBuffers()
{
    if (!g_stackBufferContext)
        return;
    std::vector<unsigned char*>::iterator it =
        g_stackBufferContext->m_blocks.begin();
    std::vector<unsigned char*>::iterator e =
        g_stackBufferContext->m_blocks.end();
    for (; it != e; it++)
    {
        unsigned char* p = *it;
        if (p)
            delete[] p;
    }
    delete g_stackBufferContext;
    g_stackBufferContext = 0;
}
StackBuffer::StackBuffer() : m_buf(0), m_end(0) {}
StackBuffer::StackBuffer(const StackBuffer& other)
{
    m_buf = other.m_buf;
    m_end = other.m_end;
    const_cast<StackBuffer&>(other).m_buf = 0;
    const_cast<StackBuffer&>(other).m_end = 0;
}
StackBuffer::~StackBuffer()
{
    if (m_buf)
        freeStackBuffer((unsigned char*)m_buf, m_end);
}
StackBuffer& StackBuffer::operator=(const StackBuffer& other)
{
    if (this == &other)
        return *this;
    if (m_buf)
        freeStackBuffer((unsigned char*)m_buf, m_end);
    m_buf = other.m_buf;
    m_end = other.m_end;
    const_cast<StackBuffer&>(other).m_buf = 0;
    const_cast<StackBuffer&>(other).m_end = 0;
    return *this;
}
StackBuffer StackBuffer::alloc(unsigned int size)
{
    StackBuffer sb;
    allocStackBuffer(size, (unsigned char**)&sb.m_buf, &sb.m_end);
    return sb;
}
void StackBuffer::freeAll()
{
    freeAllStackBuffers();
}
char* StackBuffer::getBuffer()
{
    return m_buf;
}
StackBuffer_char StackBuffer_char::alloc(unsigned int size)
{
    StackBuffer_char sb;
    allocStackBuffer(size, (unsigned char**)&sb.m_buf, &sb.m_end);
    return sb;
}
StackBuffer_char::operator char*()
{
    return getBuffer();
}

StackBuffer_wchar StackBuffer_wchar::alloc(unsigned int size)
{
    StackBuffer_wchar sb;
    allocStackBuffer(size, (unsigned char**)&sb.m_buf, &sb.m_end);
    return sb;
}
StackBuffer_wchar::operator wchar_t*()
{
    return (wchar_t*)getBuffer();
}
