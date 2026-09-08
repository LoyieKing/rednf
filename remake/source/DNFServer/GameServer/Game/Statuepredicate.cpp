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
Statuepredicate::Statuepredicate(pieceQuestRewardStatus status)
    : std::unary_function<pieceQuestRewardStatus, bool>(),
      m_appendageType(status.m_appendageType),
      m_valueType(status.m_valueType),
      m_value(status.m_value)
{
}

bool Statuepredicate::operator()(pieceQuestRewardStatus const& status) const
{
    if (m_appendageType == status.m_appendageType &&
        m_valueType == status.m_valueType)
        return true;
    return false;
}
