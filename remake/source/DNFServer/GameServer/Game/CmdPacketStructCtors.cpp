// ============================================================================
// df_game_r CMDPacketStruct 生命周期符号实现（OnlyOrig_Exchange 批）
//
// worklist /tmp/only_orig/CMDPacketStruct.txt 全部 77 个符号由本 TU + 头文件
// 内联基类产生：
//   - C1/C2：本 TU 显式 out-of-line 定义（一对 C1/C2 由同一函数体定义生成）。
//   - D0/D1/D2：STBaseResponse 虚析构链使派生 Res 类自动生成弱析构
//     （ORIG 同为 W；D0=D1+operator delete，D1=复位 vptr+ParamBase::D1，
//     与 ORIG 逐字节形态一致，见 /tmp/es_disasm.txt）。
// memset 常量为 ORIG ctor 反汇编字面量（证据见 CMDPacketStruct.h 注释）。
// ============================================================================

#include "CMDPacketStruct.h"

namespace CMDPacketStruct
{

// ---- Pause ----
_ReqAdvanceAltarPause::_ReqAdvanceAltarPause()
{
    memset(this, 0, 0xe);
}
_ResAdvanceAltarPause::_ResAdvanceAltarPause()
{
    memset(this, 0, 0xc);
}

// ---- BuyItem ----
_ReqAdvanceAltarBuyItem::_ReqAdvanceAltarBuyItem()
{
    memset(this, 0, 0x20);
}
_ResAdvanceAltarBuyItem::_ResAdvanceAltarBuyItem()
{
    memset(this, 0, 0x1c);
}

// ---- SetSlot ----
_ReqAdvanceAltarSetSlot::_ReqAdvanceAltarSetSlot()
{
    memset(this, 0, 0x24);
}
_ResAdvanceAltarSetSlot::_ResAdvanceAltarSetSlot()
{
    memset(this, 0, 0x1c);
}

// ---- ResetStar ----
_ReqAdvanceAltarResetStar::_ReqAdvanceAltarResetStar()
{
    memset(this, 0, 0x14);
}
_ResAdvanceAltarResetStar::_ResAdvanceAltarResetStar()
{
    memset(this, 0, 0xc);
}

// ---- SummonUnit ----
_ReqAdvanceAltarSummonUnit::_ReqAdvanceAltarSummonUnit()
{
    memset(this, 0, 0x18);
}
_ResAdvanceAltarSummonUnit::_ResAdvanceAltarSummonUnit()
{
    memset(this, 0, 0x14);
}

// ---- UpgradeGage ----
_ReqAdvanceAltarUpgradeGage::_ReqAdvanceAltarUpgradeGage()
{
    memset(this, 0, 0x18);
}
_ResAdvanceAltarUpgradeGage::_ResAdvanceAltarUpgradeGage()
{
    memset(this, 0, 0x14);
}

// ---- ExchangeSlot ----
_ReqAdvanceAltarExchangeSlot::_ReqAdvanceAltarExchangeSlot()
{
    memset(this, 0, 0x30);
}
_ResAdvanceAltarExchangeSlot::_ResAdvanceAltarExchangeSlot()
{
    memset(this, 0, 0x24);
}

// ---- GetAchievementReward ----
_ReqAdvanceAltarGetAchievementReward::_ReqAdvanceAltarGetAchievementReward()
{
    memset(this, 0, 0x14);
}
_ResAdvanceAltarGetAchievementReward::_ResAdvanceAltarGetAchievementReward()
{
    memset(this, 0, 0xc);
}

// ---- EventDungeon ClearRoom / DestoryObject ----
_STReqEventDungeonClearRoom::_STReqEventDungeonClearRoom()
{
    memset(this, 0, 0x14);
}
_STResEventDungeonClearRoom::_STResEventDungeonClearRoom()
{
    memset(this, 0, 0xc);
}
_STReqEventDungeonDestoryObject::_STReqEventDungeonDestoryObject()
{
    memset(this, 0, 0x18);
}
_STResEventDungeonDestoryObject::_STResEventDungeonDestoryObject()
{
    memset(this, 0, 6);   // ORIG 字面量（< sizeof(8)，忠实复刻）
}

}  // namespace CMDPacketStruct
