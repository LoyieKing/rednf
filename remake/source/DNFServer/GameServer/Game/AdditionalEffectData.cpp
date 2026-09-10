// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AdditionalEffectData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// AdditionalEffectData 实现 —— ORIG W 符号转写（报告缺失，objdump 定址实证）。
// 布局与字段见 AdditionalEffectData.h；均为非虚成员（无 vptr 操作）。
#include "AdditionalEffectData.h"

// ORIG C1 0x0891b05e：vector 默认构造 → m_id=-1 → vector clear →
// m_mode=2 → m_count=0 → m_max=-1 → m_min=-1 → m_valueA/B/C=0 → m_flag=0。
#include "AdditionalEffectData/AdditionalEffectData.inc"
    : m_strings()
{
    m_id = -1;
    m_strings.clear();
    m_mode = 2;
    m_count = 0;
    m_max = -1;
    m_min = -1;
    m_valueA = 0;
    m_valueB = 0;
    m_valueC = 0;
    m_flag = 0;
}

// ORIG C1ERKSs 0x08929074：初始化列表 m_id 先行、vector 拷贝构造次之
// （ORIG 汇编 m_id@+0 拷贝在 vector C1ERKS1 之前），其余标量函数体逐句赋值。
#include "AdditionalEffectData/AdditionalEffectData_1.inc"
    : m_id(other.m_id), m_strings(other.m_strings)
{
    m_mode = other.m_mode;
    m_count = other.m_count;
    m_min = other.m_min;
    m_max = other.m_max;
    m_valueA = other.m_valueA;
    m_valueB = other.m_valueB;
    m_valueC = other.m_valueC;
    m_flag = other.m_flag;
}

// ORIG D1 0x0891c73e：非虚用户析构（无 vptr 重置），仅析构 vector 成员。
#include "AdditionalEffectData/dtor_AdditionalEffectData.inc"
{
}

// ORIG 0x0891c754：逐字段赋值，返回 *this（ORIG 尾部 mov this,%eax）。
AdditionalEffectData& AdditionalEffectData::operator=(AdditionalEffectData const& other)
{
    m_id = other.m_id;
    m_strings = other.m_strings;
    m_mode = other.m_mode;
    m_count = other.m_count;
    m_min = other.m_min;
    m_max = other.m_max;
    m_valueA = other.m_valueA;
    m_valueB = other.m_valueB;
    m_valueC = other.m_valueC;
    m_flag = other.m_flag;
    return *this;
}
