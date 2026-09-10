// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Stream/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— Stream / SmallStream / BigStream（S1 组）
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN6StreamC2Ev`（ORIG 0x861c999）；ctor 已是首强符号。
#include <iostream>

#include "Stream.h"

#include <string.h>

#include "Stream/Stream.inc"
}

#include "Stream/Stream_1.inc"
}

#include "Stream/Stream_2.inc"
}

#include "Stream/Stream_3.inc"
}

#include "Stream/dtor_Stream.inc"
}

#include "Stream/size.inc"
}

#include "Stream/length.inc"
}

#include "Stream/in_ptr.inc"
}

#include "Stream/in_ptr_1.inc"
}

#include "Stream/out_ptr.inc"
}

#include "Stream/out_ptr_1.inc"
}

#include "Stream/reset.inc"
}

#include "Stream/set_big_stream.inc"
}

#include "Stream/is_big_stream.inc"
}

#include "Stream/resize.inc"
}

#include "Stream/enable_in.inc"
}

#include "Stream/enable_out.inc"
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
}

#include "Stream/get_buffer.inc"
}

#include "Stream/get_binary.inc"
}

#include "Stream/SmallStream.inc"
}

#include "Stream/dtor_SmallStream.inc"
}

#include "Stream/BigStream.inc"
}

#include "Stream/dtor_BigStream.inc"
}
