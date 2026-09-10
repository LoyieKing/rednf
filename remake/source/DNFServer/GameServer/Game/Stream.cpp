// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Stream/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— Stream / SmallStream / BigStream（S1 组）
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN6StreamC2Ev`（ORIG 0x861c999）；ctor 已是首强符号。
#include <iostream>

#include "Stream.h"

#include <string.h>

#include "Stream/Stream.inc"
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

#include "Stream/Stream_1.inc"
{
    m_buffer = 0;
    m_size = 0;
    m_inPtr = 0;
    m_outPtr = 0;
    set_big_stream(false);
    resize(size);
    m_poolType = 0;
}

#include "Stream/Stream_2.inc"
{
    Stream();
    *this << str;
}

#include "Stream/Stream_3.inc"
{
    Stream();
    put_binary(other.get_buffer(), other.length());
}

#include "Stream/dtor_Stream.inc"
{
    delete[] m_buffer;
}

#include "Stream/size.inc"
{
    return m_size;
}

#include "Stream/length.inc"
{
    return in_ptr() - out_ptr();
}

#include "Stream/in_ptr.inc"
{
    return m_inPtr;
}

#include "Stream/in_ptr_1.inc"
{
    m_inPtr += n;
    return m_inPtr;
}

#include "Stream/out_ptr.inc"
{
    return m_outPtr;
}

#include "Stream/out_ptr_1.inc"
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

#include "Stream/reset.inc"
{
    m_inPtr = 0;
    m_outPtr = 0;
    set_big_stream(false);
    m_poolType = 0;
}

#include "Stream/set_big_stream.inc"
{
    m_bigStream = b;
}

#include "Stream/is_big_stream.inc"
{
    return m_bigStream;
}

#include "Stream/resize.inc"
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

#include "Stream/enable_in.inc"
{
    if (in_ptr() + n > size())
    {
        return false;
    }
    return true;
}

#include "Stream/enable_out.inc"
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

#include "Stream/put_binary.inc"
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

#include "Stream/str_len.inc"
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

#include "Stream/get_buffer.inc"
{
    return m_buffer + out_ptr();
}

#include "Stream/get_binary.inc"
{
    if (!enable_out(len))
    {
        return 0;
    }
    memcpy(dst, m_buffer + out_ptr(), len);
    out_ptr(len);
    return 1;
}

#include "Stream/SmallStream.inc"
    : Stream(0x1000)
{
}

#include "Stream/dtor_SmallStream.inc"
{
}

#include "Stream/BigStream.inc"
    : Stream(0x40000)
{
}

#include "Stream/dtor_BigStream.inc"
{
}
