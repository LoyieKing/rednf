// df_game_r 还原 —— QuickParty::IRewardType 基类 ctor 独立 TU。
// ORIG 0x0826dbee（W）：vptr + 引用存储，逐指令同 CQuickPartyRewardManager.cpp 原实现。
// 归属实证：基类 ctor 与派生类 ctor（IBasicReward 0x826dc06 等）同 TU 时，
// gcc4.4 unit-at-a-time 将派生 ctor outgoing 帧由 sub $0x18 塌缩为 sub $0x8，
// 与 ORIG 4.1 恒发 $0x18 相悖（NL4 p6/p7 探针族 + NL7 三处同类实证）；
// 定义落独立 TU（头声明不变）后派生 ctor 恢复 0x18 帧。

#include "CQuickPartyRewardManager.h"

namespace QuickParty
{

IRewardType::IRewardType(QuickPartyData& data)
    : m_data(data)
{
}

}  // namespace QuickParty
