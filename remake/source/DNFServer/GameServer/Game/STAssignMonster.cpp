// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STAssignMonster/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ STAssignMonster 专属 TU（从 GameStubs.cpp 拆出）。
// 逐方法对照 ORIG df_game_r 反汇编实现（证据见各函数注释）。
#include "STAssignMonster.h"

// ORIG 0x836c5e0：默认构造 +0x28/+0x40/+0x5c 三个 vector（编译器按成员自动完成），
// 随后调用 Clear() 置初值。
#include "STAssignMonster/STAssignMonster.inc"
{
    Clear();
}

// ORIG 0x8377696：标量逐字段复制 + m_pair18/m_pair38 整块拷贝 + 三个 vector 拷贝构造。
// [b28d-solo-STAssignMonster] C1ERKS_：union 具名成员整体拷贝复现 ORIG 4.1
// 「dest 缓存 %ecx + 高低半读交错」8 字节块拷贝（P246，0x83776e5/0x8377833）。
#include "STAssignMonster/STAssignMonster_1.inc"
    : m_0(other.m_0), m_4(other.m_4), m_8(other.m_8), m_c(other.m_c),
      m_10(other.m_10), m_14(other.m_14), m_pair18(other.m_pair18),
      m_20(other.m_20), m_24(other.m_24), m_vec28(other.m_vec28),
      m_34(other.m_34), m_pair38(other.m_pair38),
      m_vec40(other.m_vec40), m_4c(other.m_4c), m_50(other.m_50),
      m_54(other.m_54), m_55(other.m_55), m_58(other.m_58),
      m_vec5c(other.m_vec5c)
{
}

// ORIG 0x8377622：三个 vector 按声明逆序自动析构（ORIG 显式调用 ~vector）。
#include "STAssignMonster/dtor_STAssignMonster.inc"
{
}

// ORIG 0x83777f2：标量逐字段赋值 + m_pair18/m_pair38 整块拷贝 + 三个 vector operator=。
// [b28d-solo-STAssignMonster] aSERKS_：同 C1ERKS_，P246 union 整体赋值。
STAssignMonster& STAssignMonster::operator=(const STAssignMonster& other)
{
    // ORIG 0x083777f2 无自赋值守卫，直接逐字段赋值。
    m_0 = other.m_0;
    m_4 = other.m_4;
    m_8 = other.m_8;
    m_c = other.m_c;
    m_10 = other.m_10;
    m_14 = other.m_14;
    m_pair18 = other.m_pair18;
    m_20 = other.m_20;
    m_24 = other.m_24;
    m_vec28 = other.m_vec28;
    m_34 = other.m_34;
    m_pair38 = other.m_pair38;
    m_vec40 = other.m_vec40;
    m_4c = other.m_4c;
    m_50 = other.m_50;
    m_54 = other.m_54;
    m_55 = other.m_55;
    m_58 = other.m_58;
    m_vec5c = other.m_vec5c;
    return *this;
}

// ORIG 0x836c4fa：字段复位初值；仅 m_vec28/m_vec5c 清空（m_vec40 不动）。
#include "STAssignMonster/Clear.inc"
{
    m_0 = -1;
    m_4 = 1;
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
    m_14 = 0;
    m_pair18.m_pair.m_hi = 1;
    m_pair18.m_pair.m_lo = m_pair18.m_pair.m_hi;
    m_20 = 0;
    m_24 = 0;
    m_vec28.clear();
    m_50 = 0;
    m_4c = 100;
    m_54 = 0;
    m_55 = 0;
    m_58 = -1;
    for (int i = 0; i <= 1; ++i)   // ORIG 0x836c5a5 scaled 循环（setle 实证 i<=1）
        m_pair38.m_arr[i] = 0;
    m_34 = 0;
    m_vec5c.clear();
}