// df_game_r 还原 —— Stream / SmallStream / BigStream（S1 组）
#include "Stream.h"

#include <string.h>

Stream::Stream()
{
    // ORIG 存储顺序：+8(m_buffer) → +4(m_size) → +0xc → +0x10
    m_buffer = 0;
    m_size = 0;
    m_inPtr = 0;
    m_outPtr = 0;
    set_big_stream(false);
    resize(0x4000);
    m_poolType = 0;
}

Stream::Stream(int size)
{
    m_buffer = 0;
    m_size = 0;
    m_inPtr = 0;
    m_outPtr = 0;
    set_big_stream(false);
    resize(size);
    m_poolType = 0;
}

Stream::Stream(char* str)
{
    Stream();
    *this << str;
}

Stream::Stream(Stream& other)
{
    Stream();
    put_binary(other.get_buffer(), other.length());
}

Stream::~Stream()
{
    delete[] m_buffer;
}

int Stream::size()
{
    return m_size;
}

int Stream::length()
{
    return in_ptr() - out_ptr();
}

int Stream::in_ptr()
{
    return m_inPtr;
}

int Stream::in_ptr(int n)
{
    m_inPtr += n;
    return m_inPtr;
}

int Stream::out_ptr()
{
    return m_outPtr;
}

int Stream::out_ptr(int n)
{
    if (m_outPtr + n > m_inPtr)
    {
        return -1;
    }
    m_outPtr += n;
    if (m_outPtr == m_inPtr)
    {
        reset();
    }
    return m_outPtr;
}

void Stream::reset()
{
    m_inPtr = 0;
    m_outPtr = 0;
    set_big_stream(false);
    m_poolType = 0;
}

void Stream::set_big_stream(bool b)
{
    m_bigStream = b;
}

bool Stream::is_big_stream()
{
    return m_bigStream;
}

int Stream::resize(int n)
{
    char* buf = new char[n];
    if (!buf)
    {
        return 0;
    }
    memcpy(buf, m_buffer + out_ptr(), length());
    if (m_buffer)
    {
        delete[] m_buffer;
    }
    m_outPtr = 0;
    m_inPtr = length();
    m_buffer = buf;
    m_size = n;
    return 1;
}

bool Stream::enable_in(int n)
{
    if (in_ptr() + n > size())
    {
        return false;
    }
    return true;
}

bool Stream::enable_out(int n)
{
    if (out_ptr() + n > in_ptr())
    {
        return false;
    }
    return true;
}

Stream& Stream::operator<<(bool b)
{
    while (!enable_in(1))
    {
        resize(size() * 2);
    }
    if (b)
    {
        m_buffer[in_ptr()] = 1;
    }
    else
    {
        m_buffer[in_ptr()] = 0;
    }
    in_ptr(1);
    return *this;
}

Stream& Stream::operator<<(char c)
{
    while (!enable_in(1))
    {
        resize(size() * 2);
    }
    m_buffer[in_ptr()] = c;
    in_ptr(1);
    return *this;
}

Stream& Stream::operator<<(short s)
{
    while (!enable_in(2))
    {
        resize(size() * 2);
    }
    memcpy(m_buffer + in_ptr(), &s, 2);
    in_ptr(2);
    return *this;
}

Stream& Stream::operator<<(int v)
{
    while (!enable_in(4))
    {
        resize(size() * 2);
    }
    memcpy(m_buffer + in_ptr(), &v, 4);
    in_ptr(4);
    return *this;
}

Stream& Stream::operator<<(long v)
{
    while (!enable_in(4))
    {
        resize(size() * 2);
    }
    memcpy(m_buffer + in_ptr(), &v, 4);
    in_ptr(4);
    return *this;
}

Stream& Stream::operator<<(char* s)
{
    int len = str_len(s);
    while (!enable_in(len))
    {
        resize(size() * 2);
    }
    memcpy(m_buffer + in_ptr(), s, len + 1);
    in_ptr(len + 1);
    return *this;
}

Stream& Stream::operator<<(unsigned char c)
{
    operator<<((char)c);
    return *this;
}

Stream& Stream::operator<<(unsigned short s)
{
    operator<<((short)s);
    return *this;
}

Stream& Stream::operator<<(unsigned int v)
{
    operator<<((int)v);
    return *this;
}

Stream& Stream::operator<<(unsigned long v)
{
    operator<<((long)v);
    return *this;
}

int Stream::put_binary(void* src, int len)
{
    while (!enable_in(len))
    {
        resize(size() * 2);
    }
    memcpy(m_buffer + in_ptr(), src, len);
    in_ptr(len);
    return 1;
}

bool Stream::operator>>(bool& b)
{
    if (!enable_out(1))
    {
        return 0;
    }
    char c = m_buffer[out_ptr()];
    if (c == 1)
    {
        b = true;
    }
    else
    {
        b = false;
    }
    out_ptr(1);
    return 1;
}

bool Stream::operator>>(char& c)
{
    if (!enable_out(1))
    {
        return 0;
    }
    c = m_buffer[out_ptr()];
    out_ptr(1);
    return 1;
}

bool Stream::operator>>(short& s)
{
    if (!enable_out(2))
    {
        return 0;
    }
    memcpy(&s, m_buffer + out_ptr(), 2);
    out_ptr(2);
    return 1;
}

bool Stream::operator>>(int& v)
{
    if (!enable_out(4))
    {
        return 0;
    }
    memcpy(&v, m_buffer + out_ptr(), 4);
    out_ptr(4);
    return 1;
}

bool Stream::operator>>(long& v)
{
    if (!enable_out(4))
    {
        return 0;
    }
    memcpy(&v, m_buffer + out_ptr(), 4);
    out_ptr(4);
    return 1;
}

bool Stream::operator>>(char* s)
{
    if (!length())
    {
        return 0;
    }
    char* p = m_buffer + out_ptr();
    int len = str_len(p);
    memcpy(s, p, len + 1);
    out_ptr(len + 1);
    return 1;
}

bool Stream::operator>>(unsigned char& c)
{
    char tmp = 0;
    bool ok = operator>>(tmp);
    c = (unsigned char)tmp;
    return ok;
}

bool Stream::operator>>(unsigned short& s)
{
    short tmp = 0;
    bool ok = operator>>(tmp);
    s = (unsigned short)tmp;
    return ok;
}

bool Stream::operator>>(unsigned int& v)
{
    int tmp = 0;
    bool ok = operator>>(tmp);
    v = (unsigned int)tmp;
    return ok;
}

bool Stream::operator>>(unsigned long& v)
{
    long tmp = 0;
    bool ok = operator>>(tmp);
    v = (unsigned long)tmp;
    return ok;
}

int Stream::str_len(char* s)
{
    int len = 0;
    int i = 0;
    while (i <= 9999999)
    {
        if (s[i] == 0)
        {
            return len;
        }
        ++len;
        ++i;
    }
    return 0;
}

char* Stream::get_buffer()
{
    return m_buffer + out_ptr();
}

bool Stream::get_binary(void* dst, int len)
{
    if (!enable_out(len))
    {
        return 0;
    }
    memcpy(dst, m_buffer + out_ptr(), len);
    out_ptr(len);
    return 1;
}

SmallStream::SmallStream()
    : Stream(0x1000)
{
}

SmallStream::~SmallStream()
{
}

BigStream::BigStream()
    : Stream(0x40000)
{
}

BigStream::~BigStream()
{
}
