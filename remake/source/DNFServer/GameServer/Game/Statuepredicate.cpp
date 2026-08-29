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
