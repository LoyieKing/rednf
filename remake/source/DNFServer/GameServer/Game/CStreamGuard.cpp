// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CStreamGuard/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

#include "CStreamGuard/CStreamGuard_1.inc"
}

#include "CStreamGuard/dtor_CStreamGuard.inc"
}

#include "CStreamGuard/Set.inc"
}

#include "CStreamGuard/Get.inc"
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
}
