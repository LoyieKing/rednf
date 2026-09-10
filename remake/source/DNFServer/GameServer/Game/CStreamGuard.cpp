// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CStreamGuard/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CStreamGuard 全量（S1 组）
#include "CStreamGuard.h"

// GlobalData 静态池指针（属 GlobalData 组还原范围，此处仅声明引用）
class GlobalData
{
public:
    static StreamPool* s_stream_pool;        // 0x0940bd6c
    static BigStreamPool* s_big_stream_pool; // 0x0940bda4
    static SmallStreamPool* s_small_stream_pool;  // 0x0940bda8
};

#include "CStreamGuard/CStreamGuard.inc"
{
    m_bOwn = bOwn;
}

#include "CStreamGuard/CStreamGuard_1.inc"
{
    m_stream = stream;
    m_bOwn = bOwn;
}

#include "CStreamGuard/dtor_CStreamGuard.inc"
{
    if (m_stream && m_bOwn && m_stream->m_inUse)
    {
        // ORIG cmp 级联顺序：1→Small、2→Big、default→Stream
        switch (m_stream->m_poolType)
        {
        case 1:
            GlobalData::s_small_stream_pool->Free(m_stream);
            break;
        case 0:
            GlobalData::s_stream_pool->Free(m_stream);
            break;
        case 2:
            GlobalData::s_big_stream_pool->Free(m_stream);
            break;
        }
    }
}

#include "CStreamGuard/Set.inc"
{
    m_stream = stream;
}

#include "CStreamGuard/Get.inc"
{
    return m_stream;
}

CStreamGuard* CStreamGuard::operator->()
{
    return this;
}

CStreamGuard* CStreamGuard::operator*()
{
    return this;
}

Stream& CStreamGuard::operator<<(bool b)
{
    return *m_stream << b;
}

Stream& CStreamGuard::operator<<(char c)
{
    return *m_stream << c;
}

Stream& CStreamGuard::operator<<(short s)
{
    return *m_stream << s;
}

Stream& CStreamGuard::operator<<(int v)
{
    return *m_stream << v;
}

Stream& CStreamGuard::operator<<(long v)
{
    return *m_stream << v;
}

Stream& CStreamGuard::operator<<(unsigned char c)
{
    return *m_stream << c;
}

Stream& CStreamGuard::operator<<(unsigned short s)
{
    return *m_stream << s;
}

Stream& CStreamGuard::operator<<(unsigned int v)
{
    return *m_stream << v;
}

Stream& CStreamGuard::operator<<(unsigned long v)
{
    return *m_stream << v;
}

Stream& CStreamGuard::operator<<(char* s)
{
    return *m_stream << s;
}

#include "CStreamGuard/put_binary.inc"
{
    return m_stream->put_binary(src, len);
}
