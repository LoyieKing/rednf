// ============================================================================
// df_game_r 还原 —— CStreamBuffer 实现（ORIG 0x086269be 域）
// 逐函数对应 docs/class_func_reports 缺失（无 CStreamBuffer 目录），按指南
// §3.2 以 nm 定位 + objdump 转写，报告路径：无（见头文件证据注释）。
// 非静态成员：AppendString/Insert 均带 this（参数槽 = this, 显式参数...）。
// AppendString 源码形态（声明 slen/n 后先赋 n 再赋 slen）为实测对齐 ORIG
// 指令序/栈槽（探针验证 gcc4.4 -m32 -O0 同形）。
// ============================================================================

#include "CStreamBuffer.h"

#include <algorithm>   // std::min<unsigned int>
#include <string.h>

CStreamBuffer::CStreamBuffer()                    // 0x086269be
{
    memset(m_buf, 0, sizeof(m_buf));
}

char* CStreamBuffer::c_str()                      // 0x086269e2
{
    return m_buf;
}

void CStreamBuffer::AppendString(char* dst, int maxlen, char* src)  // 0x086269ea
{
    unsigned int slen;
    unsigned int n;
    n = maxlen - strlen(dst) - 1;
    slen = strlen(src);
    strncat(dst, src, std::min(slen, n));
}

CStreamBuffer& CStreamBuffer::operator<<(const char* v)   // 0x08626a4a "%s"
{
    Insert<const char*>("%s", v);
    return *this;
}

CStreamBuffer& CStreamBuffer::operator<<(int v)           // 0x08626a70 "%d"
{
    Insert<int>("%d", v);
    return *this;
}

CStreamBuffer& CStreamBuffer::operator<<(unsigned int v)  // 0x08626a96 "%u"
{
    Insert<unsigned int>("%u", v);
    return *this;
}

CStreamBuffer& CStreamBuffer::operator<<(float v)         // 0x08626abc "%f"
{
    Insert<float>("%f", v);
    return *this;
}
