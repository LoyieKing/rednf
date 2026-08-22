// df_game_r Game/ 迁移 TU —— MSG_STAT_RANKRES 构造/析构（2026-08-21）。
// 从 GameStubs.cpp 拆出。权威布局见 Statistics.cpp / SigTypes.h：
//   +0x00..+0x18 七个整型 + +0x1c GameResultType m_result（0x50 字节），总尺寸 0x6c。
// ORIG：
//   MSG_STAT_RANKRES::MSG_STAT_RANKRES()  0x08610798 —— 仅构造 +0x1c 的 m_result
//                                                 （call _ZN14GameResultTypeC1Ev）
//   MSG_STAT_RANKRES::~MSG_STAT_RANKRES() 0x086107ae —— 仅析构 +0x1c 的 m_result
//                                                 （call _ZN14GameResultTypeD1Ev）
// 前置字段（m_a..m_g）由使用方（Statistics::RankResult）在 SendSignal 前填充，ctor 不 init。
#include "GameResultType.h"
#include <new>

// C1/C2、D1/D2 以 asm-label 别名提供强 T 符号（与 ORIG mangled 一致）。
extern "C" void sub_MSG_STAT_RANKRES_C1(void* self)
    asm("_ZN16MSG_STAT_RANKRESC1Ev");
extern "C" void sub_MSG_STAT_RANKRES_C2(void* self)
    asm("_ZN16MSG_STAT_RANKRESC2Ev");
extern "C" void sub_MSG_STAT_RANKRES_D1(void* self)
    asm("_ZN16MSG_STAT_RANKRESD1Ev");
extern "C" void sub_MSG_STAT_RANKRES_D2(void* self)
    asm("_ZN16MSG_STAT_RANKRESD2Ev");

void sub_MSG_STAT_RANKRES_C1(void* self)
{
    new (reinterpret_cast<GameResultType*>(
        reinterpret_cast<char*>(self) + 0x1c)) GameResultType();
}
void sub_MSG_STAT_RANKRES_C2(void* self)
{
    new (reinterpret_cast<GameResultType*>(
        reinterpret_cast<char*>(self) + 0x1c)) GameResultType();
}
void sub_MSG_STAT_RANKRES_D1(void* self)
{
    reinterpret_cast<GameResultType*>(
        reinterpret_cast<char*>(self) + 0x1c)->~GameResultType();
}
void sub_MSG_STAT_RANKRES_D2(void* self)
{
    reinterpret_cast<GameResultType*>(
        reinterpret_cast<char*>(self) + 0x1c)->~GameResultType();
}