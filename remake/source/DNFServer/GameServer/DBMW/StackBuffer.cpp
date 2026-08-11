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

static __thread StackBufferContext* g_stackBufferContext;

static void allocStackBuffer(unsigned int size, unsigned char** buf, int* end)
{
    StackBufferContext* ctx = g_stackBufferContext;
    if (!ctx)
    {
        ctx = new StackBufferContext;
        g_stackBufferContext = ctx;
        ctx->m_buffers.reserve(0x20);
        ctx->m_blocks.reserve(8);
        unsigned char* block = new unsigned char[0x4000];
        ctx->m_blocks.push_back(block);
        ctx->m_blockIndex = 0;
        ctx->m_offset = 0;
        StackBufferContext::Buffer b;
        b.m_blockIndex = 0;
        b.m_offset = 0;
        b.m_size = 0;
        ctx->m_buffers.push_back(std::move(b));
    }
    if (size > 0x4000)
    {
        *buf = new unsigned char[size];
        *end = -1;
        return;
    }
    if (ctx->m_offset + (int)size > 0x4000)
    {
        int idx = ctx->m_blockIndex + 1;
        if (idx == (int)ctx->m_blocks.size())
        {
            unsigned char* nb = new unsigned char[0x4000];
            ctx->m_blocks.push_back(nb);
        }
        ctx->m_buffers.back().m_blockIndex = idx;
        ctx->m_buffers.back().m_offset = 0;
        ctx->m_blockIndex = idx;
        ctx->m_offset = 0;
    }
    *buf = ctx->m_blocks[ctx->m_blockIndex] + ctx->m_offset;
    ctx->m_offset += size;
    *end = ctx->m_blockIndex;
}
static void freeStackBuffer(unsigned char* buf, int end)
{
    if (end == -1)
    {
        if (buf)
            delete[] buf;
        return;
    }
    StackBufferContext* ctx = g_stackBufferContext;
    ctx->m_buffers.erase(ctx->m_buffers.begin() + end);
    if (ctx->m_buffers.empty())
    {
        ctx->m_blockIndex = 0;
        ctx->m_offset = 0;
    }
    else
    {
        ctx->m_blockIndex = ctx->m_buffers.back().m_blockIndex;
        ctx->m_offset = ctx->m_buffers.back().m_offset + ctx->m_buffers.back().m_size;
    }
}
static void freeAllStackBuffers()
{
    StackBufferContext* ctx = g_stackBufferContext;
    if (!ctx)
        return;
    for (std::vector<unsigned char*>::iterator it = ctx->m_blocks.begin();
         it != ctx->m_blocks.end(); ++it)
    {
        if (*it)
            delete[] *it;
    }
    delete ctx;
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
    if (this != &other)
    {
        if (m_buf)
            freeStackBuffer((unsigned char*)m_buf, m_end);
        m_buf = other.m_buf;
        m_end = other.m_end;
        const_cast<StackBuffer&>(other).m_buf = 0;
        const_cast<StackBuffer&>(other).m_end = 0;
    }
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
