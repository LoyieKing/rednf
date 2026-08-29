#ifndef GAME_CSTREAMBUFFER_H_
#define GAME_CSTREAMBUFFER_H_

// ============================================================================
// df_game_r 还原 —— CStreamBuffer 权威声明（ORIG 0x086269be 域）
// ORIG 符号（nm -S --defined-only df_game_r）：
//   _ZN13CStreamBufferC1Ev / C2Ev            0x086269be (0x23) ctor：memset(this,0,0xa000)
//   _ZN13CStreamBuffer5c_strEv               0x086269e2 (0x08) 返回 m_buf（非 const，mangled 无 K）
//   _ZN13CStreamBuffer12AppendStringEPciS0_  0x086269ea (0x5f) 非静态成员(char*,int,char*)
//        —— this@+0x8 未用，dst@+0xc/maxlen@+0x10/src@+0x14；strncat 容量截断追加
//   _ZN13CStreamBufferlsEPKc                 0x08626a4a (0x25) operator<<(const char*) "%s"
//   _ZN13CStreamBufferlsEi                   0x08626a70 (0x25) operator<<(int) "%d"
//   _ZN13CStreamBufferlsEj                   0x08626a96 (0x25) operator<<(unsigned int) "%u"
//   _ZN13CStreamBufferlsEf                   0x08626abc (0x25) operator<<(float) "%f"
//   _ZN13CStreamBuffer6InsertIPKcEEvPcT_     0x08627cbc (0x8b) 非静态成员模板 Insert<const char*>
//   _ZN13CStreamBuffer6InsertIiEEvPcT_       0x08627d48       Insert<int>
//   _ZN13CStreamBuffer6InsertIjEEvPcT_       0x08627dd4       Insert<unsigned int>
//   _ZN13CStreamBuffer6InsertIfEEvPcT_       0x08627e60       Insert<float>
// Insert 为成员模板，显式参数 2 个：(char* fmt, T val)（mangled 无 this；
// 实参槽 = this, fmt, val；ls 调用 Insert<T>("%s", v)）。
// 布局：单 char m_buf[0xa000]（对象尺寸 0xa000，无 vptr；this == &m_buf[0]）。
// 证据：docs/class_func_reports 无 CStreamBuffer 目录（报告缺失），按指南 §3.2
// 以 nm 定位 + objdump 逐指令转写；rodata 0x8ce2de0 实测 "%s\0%d\0%u\0%f\0"。
// ============================================================================

#include <string.h>

namespace OS_API   // CInventory.h 同款最小声明（snprintf wrapper，mangled _ZN6OS_API8snprintfEPcjPKcz）
{
int snprintf(char* buf, unsigned int n, const char* fmt, ...);
}

class CStreamBuffer
{
public:
    CStreamBuffer();                              // 0x086269be
    CStreamBuffer& operator<<(const char* v);     // 0x08626a4a "%s"
    CStreamBuffer& operator<<(int v);             // 0x08626a70 "%d"
    CStreamBuffer& operator<<(unsigned int v);    // 0x08626a96 "%u"
    CStreamBuffer& operator<<(float v);           // 0x08626abc "%f"
    char* c_str();                                // 0x086269e2

    void AppendString(char* dst, int maxlen, char* src);  // 0x086269ea

    // ORIG：局部 0xa000 缓冲 snprintf 格式化后 AppendString 截断追加到 m_buf。
    // 函数体含 ORIG 同款 strlen(m_buf)/strlen(tmp) 调用（snprintf 后，结果丢弃）。
    template <typename T>
    void Insert(char* fmt, T val);                          // 0x08627cbc..e60

    char m_buf[0xa000];   // +0x00
};

template <typename T>
void CStreamBuffer::Insert(char* fmt, T val)
{
    char tmp[0xa000];
    memset(tmp, 0, sizeof(tmp));
    OS_API::snprintf(tmp, 0xa000, fmt, val);
    strlen(m_buf);
    strlen(tmp);
    AppendString(m_buf, 0xa000, tmp);
}

#endif  // GAME_CSTREAMBUFFER_H_
