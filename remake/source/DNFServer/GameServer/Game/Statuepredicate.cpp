// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Statuepredicate/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// Statuepredicate.cpp — ORIG 真实实现（独立 TU，未入 CMake，待 Main 登记）。
// ORIG 证据（df_game_r elf32-i386 objdump）：
//   C1 @0x08a6c186：call unary_function::C1（0x8a6c192）→ 以 eax 缓存 this，
//     将按值实参 0xc(%ebp)/0x10(%ebp)/0x14(%ebp) 三字分别 mov 至 this+0x0/+0x4/+0x8
//     （P29 初始化列表形态）。
//   operator() @0x08a6c1ae：先比 this->m_appendageType==arg.m_appendageType，
//     再比 this->m_valueType==arg.m_valueType，任一不等 → mov $0；全等 → mov $1
//     （P56 分支形态，勿直 return 表达式）。不比较 m_value。
#include "Statuepredicate.h"
#include "CDataManager.h"   // pieceQuestRewardStatus 完整定义

// [b22判词·N族] ORIG C1 0x8a6c186 在置 3 成员前先 call
// _ZNSt14unary_functionI22pieceQuestRewardStatusbEC1Ev（0x8a6c180，trivial 空基类
// ctor）并以 sub $0x18 预留其调用帧；我方 GCC 视 std::unary_function 为空基类、
// ctor 平凡 → 省略该 call 与对应帧（P0：函数体无额外局部）。三成员 mov 与 ORIG
// 完全同址同序（+0x0/+0x4/+0x8）。空基类 ctor 无副作用，语义等价，不可复现
// （需以非平凡基类才能强制发 call，会改类语义）。
// [b28c-27] 复核同上：4.4 对 std::unary_function（空基类 trivial ctor）恒省 call
// 与对应帧，ORIG 4.1 O0 发射；不可翻（同 P215 边界）。
// [b28d-lane15] 复核维持 N：ORIG 15 insn / 我方 13（TSV ndiff=11）；唯一残差即
// unary_function::C1 call（ORIG 0x08a6c192）与随之的 sub $0x18 帧（4.4 恒省）。
// 三成员 mov 与 ORIG 同址同序（+0x0/+0x4/+0x8）。封顶。
// [A24TailBatch2 2026-09-10 四次复核·N 维持] 二次 nm 双侧核对：
//   ORIG `08a6c180 00000005 W _ZNSt14unary_functionI22pieceQuestRewardStatusbEC1Ev`
//   （另有同址 C2）；我方 /tmp/tu_Statuepredicate.o **无该符号** ⇒ 4.4 判
//   std::unary_function（libstdc++ 空类、无用户 ctor）trivial，C1 恒省 call + 无帧
//   （P71/P215 边界：要复现需改库头使其非平凡=禁改，或造非平凡基类=改语义）。
//   同族差：ORIG 0x08a6c197 载入 eax 一次并复用供 3 次 store（4.1 的 dest 缓存形），
//   我方每字段重读 0x8(%ebp)（4.4 恒重读）⇒ 该 2 条亦跨编译器寄存器分配差（N18a 族）。
//   语义等价（空基类 ctor 零副作用）。判定：不可翻，封 N。
// [P2-S4 2026-09-11 五次复核·N 维持] 依指派核查 call 目标：ORIG 0x08a6c192 `call
//   0x8a6c180` = `_ZNSt14unary_functionI22pieceQuestRewardStatusbEC1Ev`，该目标体
//   `push %ebp; mov %esp,%ebp; pop %ebp; ret`（0x8a6c180..0x8a6c184，共 5 字节，空函数）。
//   我方 TU 侧 nm 无同名符号（4.4 视空类无用户 ctor 为 trivial，恒省 call 与 `sub $0x18`
//   帧；初始化列表已显式写 `std::unary_function<...>()` 仍被省略）。要发该 call 只能把
//   基类改成非平凡 ⇒ 改类语义（禁）或改 .h（禁）。维持不可达：ORIG 15 / 我方 14。
#include "Statuepredicate/Statuepredicate.inc"
    : std::unary_function<pieceQuestRewardStatus, bool>(),
      m_appendageType(status.m_appendageType),
      m_valueType(status.m_valueType),
      m_value(status.m_value)
{
}

#include "Statuepredicate/operator.inc"
{
    if (m_appendageType == status.m_appendageType &&
        m_valueType == status.m_valueType)
        return true;
    return false;
}
