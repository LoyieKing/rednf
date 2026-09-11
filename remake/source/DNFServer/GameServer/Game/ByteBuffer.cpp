// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ByteBuffer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "nexon_cash.h"

// ORIG 该 TU 含 file-scope 动态初始化（0x081ace9d
// __static_initialization_and_destruction_0 → ios_base::Init::C1Ev +
// __cxa_atexit(D1)）；include <iostream> 是 GI 的发射前提（纯 POD 全局 +
// 只有函数的 TU 不发 _GLOBAL__I_，P327）。
#include <iostream>

#include <limits>
#include <stdexcept>
#include <string.h>
#include <wchar.h>

namespace nexon {
namespace cash {

// 内联通用形态：cap 校验通过则逐字节大端写入，推进 write 游标；
// 空间不足时静默丢弃（游标不动），ORIG 无返回错误通道。

// ORIG 0x081acba0：strlen → 超 ushort::max 抛 invalid_argument("string length
// too long: must shorter than max(unsigned short)")；length()+len 不超容量时
// 写 ushort 长度前缀（大端）+ strncpy(len 字节，不含 NUL）+ 游标前进 strlen。
ByteBuffer& ByteBuffer::operator<<(const char* value)
{
    // [b27L8] ORIG 0x081acc8d 为 setb（无符号比较）⇒ len 无符号；int 形 4.4 出 setl。
    unsigned int len = strlen(value);
    if (len > (unsigned)std::numeric_limits<unsigned short>::max())
        throw std::invalid_argument("string length too long: must shorter than max(unsigned short)");
    if (length() + len <= m_capacity)
    {
        *this << (unsigned short)len;
        strncpy(m_writePtr, value, len);
        m_writePtr += strlen(value);
    }
    return *this;
}

#include "ByteBuffer/wrap.inc"

#include "ByteBuffer/ByteBuffer.inc"


// ORIG 0x081ae614：if(length()+1 < m_capacity){ *wp=v; ++wp; }
ByteBuffer& ByteBuffer::operator<<(unsigned char value)
{
    if (static_cast<unsigned int>(length() + 1) < m_capacity)
    {
        *m_writePtr = static_cast<char>(value);
        ++m_writePtr;
    }
    return *this;
}

// ORIG 各 lsEx W body 为手写字节反序拷贝（-O0 不展开循环，逐对 movzbl）：
// 具名 src 指针槽 + m_writePtr[i] = src[n-1-i] 逐条赋值。

// ORIG 0x081ae65e（W）：if(length()+2 <= cap) 写 2B 大端；src 槽 -0xc、value 副本 -0x1c。
ByteBuffer& ByteBuffer::operator<<(short value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 2) <= m_capacity)
    {
        m_writePtr[0] = src[1];
        m_writePtr[1] = src[0];
        m_writePtr += 2;
    }
    return *this;
}

// ORIG 0x081acefa（W）：u16 版本，同形。
ByteBuffer& ByteBuffer::operator<<(unsigned short value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 2) <= m_capacity)
    {
        m_writePtr[0] = src[1];
        m_writePtr[1] = src[0];
        m_writePtr += 2;
    }
    return *this;
}

// ORIG 0x081ae6c4（W）：int 大端 4B 逐条；incoming 地址直取（lea 0xc）。
ByteBuffer& ByteBuffer::operator<<(int value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 4) <= m_capacity)
    {
        m_writePtr[0] = src[3];
        m_writePtr[1] = src[2];
        m_writePtr[2] = src[1];
        m_writePtr[3] = src[0];
        m_writePtr += 4;
    }
    return *this;
}

// ORIG 0x081ae74c（W）：u32 版本，同形。
ByteBuffer& ByteBuffer::operator<<(unsigned int value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 4) <= m_capacity)
    {
        m_writePtr[0] = src[3];
        m_writePtr[1] = src[2];
        m_writePtr[2] = src[1];
        m_writePtr[3] = src[0];
        m_writePtr += 4;
    }
    return *this;
}

// ORIG 0x081ae7d4（W）：u64 大端 8B 逐条；value 副本槽 -0x20/-0x1c。
ByteBuffer& ByteBuffer::operator<<(unsigned long long value)
{
    const char* src = reinterpret_cast<const char*>(&value);   // 槽位早于 length() 调用（ORIG lea 先于 call）
    if (static_cast<unsigned int>(length() + 8) <= m_capacity)
    {
        m_writePtr[0] = src[7];
        m_writePtr[1] = src[6];
        m_writePtr[2] = src[5];
        m_writePtr[3] = src[4];
        m_writePtr[4] = src[3];
        m_writePtr[5] = src[2];
        m_writePtr[6] = src[1];
        m_writePtr[7] = src[0];
        m_writePtr += 8;
    }
    return *this;
}

// ORIG 0x081ae8bc：从 read 游标读 1B 并推进。
ByteBuffer& ByteBuffer::operator>>(unsigned char& value)
{
    value = static_cast<unsigned char>(*m_readPtr);
    ++m_readPtr;
    return *this;
}

// ORIG 0x081ae8e4（W）：dst 指针槽 -0x4 + 逐字节 m_readPtr[3-i] 直读（无 tmp）。
ByteBuffer& ByteBuffer::operator>>(int& value)
{
    char* dst = reinterpret_cast<char*>(&value);
    dst[0] = m_readPtr[3];
    dst[1] = m_readPtr[2];
    dst[2] = m_readPtr[1];
    dst[3] = m_readPtr[0];
    m_readPtr += 4;
    return *this;
}

// ORIG 0x081ae94e（W）：u32 版本，同形。
ByteBuffer& ByteBuffer::operator>>(unsigned int& value)
{
    char* dst = reinterpret_cast<char*>(&value);
    dst[0] = m_readPtr[3];
    dst[1] = m_readPtr[2];
    dst[2] = m_readPtr[1];
    dst[3] = m_readPtr[0];
    m_readPtr += 4;
    return *this;
}

#include "ByteBuffer/length.inc"

}  // namespace cash
}  // namespace nexon
