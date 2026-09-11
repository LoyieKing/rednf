// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AvatarRechargeServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ AvatarRechargeServer 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 依据 docs/class_func_reports/AvatarRechargeServer/、ARAD__SCRIPT__AvatarRecharge/
// 与 ORIG 反汇编转写。
#include "AvatarRechargeServer.h"
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn

#include <cstdio>
#include <string>
#include <string.h>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameTypes.h"        // cMyTrace
#include "MoreTypes.h"        // arad::SigArad*
#include "DNFLexWrapper.h"    // loadRDARScriptFile / unloadRDARScriptFile / Scan*
#include "CUser.h"            // CUser（含 eSendTarget/get_state/SendPacket 等）
#include "CInventory.h"       // CInventory::GetInvenRef/delete_item/GetAvatarItemMgrR
#include "CAvatarItemMgr.h"   // WongWork::CAvatarItemMgr::getJewelSocketData
#include "CParty.h"           // CParty::IsUseFatigueDungeon/CheckMapClear
#include "Inven_Item.h"       // Inven_Item::isEmpty/m_addInfo/m_addInfo2/m_fieldSplit
#include "GamePODTypes.h"     // stAvatarEmblemInfo_t
#include "PacketGuard.h"      // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "LogManager.h"       // LogManager::logFormat

// make_internal_stream_jpn 定义在 Arad_InternalDispatcher.cpp（ORIG 0x081977b6，
// 签名 (CStreamGuard&, ENUM_INTERNALPACKET_JPN, int)）。

// ============================================================================
// getSocketCount —— ORIG l 0x08190699 _ZL14getSocketCountPK20stAvatarEmblemInfo_t
// （文件局部静态）。遍历 5 个徽章槽（每槽 6 字节，取首 ushort emblemSocketType_），
// 非零计数。ORIG 不判空（调用方经 getJewelSocketData 保证非空），忠实转写。
// [b24F] DONE：改 break 结构复现 ORIG 双出口循环（指令序列一致）。
// ============================================================================
static int getSocketCount(const stAvatarEmblemInfo_t* emblem)
{
    int count = 0;
    for (int i = 0; i <= 4; ++i)
    {
        if (emblem->stEmblemSocket[i].emblemSocketType_ == 0)
            break;
        ++count;
    }
    return count;
}

#include "AvatarRechargeServer/AvatarRechargeServer.inc"


#include "AvatarRechargeServer/SendDurability.inc"


#include "AvatarRechargeServer/CalcDurability.inc"


// CalcDurabilityChangeEquip —— ORIG T 0x081905b4：定义 UserCoreDepsB2.cpp（去重保留）。

#include "AvatarRechargeServer/CheckConditionUseRecharge.inc"


#include "AvatarRechargeServer/Recharge.inc"


#include "AvatarRechargeServer/UseEmblem.inc"


#include "AvatarRechargeServer/GmReduceEndurance.inc"


#include "AvatarRechargeServer/GmChangeEnduranceReduceRate.inc"


#include "AvatarRechargeServer/GmSetEnduranceReduceRate.inc"


#include "AvatarRechargeServer/UpdateAvatarEndurance.inc"


// ============================================================================
// AvatarRechargeServer —— ORIG C1 0x08190200 / D1 0x08190246 /
// Load 0x0819026c / Destroy 0x081902de / Test 0x08190266
// ============================================================================

#include "AvatarRechargeServer/dtor_AvatarRechargeServer.inc"


#include "AvatarRechargeServer/Load.inc"


#include "AvatarRechargeServer/Destroy.inc"


#include "AvatarRechargeServer/Test.inc"


#include "AvatarRechargeServer/GetRateFromEmblemGrade.inc"


#include "AvatarRechargeServer/SaveAvatarEnduranceUnit.inc"


#include "AvatarRechargeServer/SaveAvatarEnduranceUnit_1.inc"


// ============================================================================
// ARAD::SCRIPT::AvatarRecharge —— ORIG C1 0x088b0dd0 / D1 0x088b0e46 /
// loadScript 0x088b0eba / clearScript 0x088b12d8 / 访问器 0x88b1314..0x88b13a6
// ============================================================================

#include "AvatarRechargeServer/AvatarRecharge.inc"


#include "AvatarRechargeServer/dtor_AvatarRecharge.inc"


#include "AvatarRechargeServer/clearScript.inc"


#include "AvatarRechargeServer/getAvatarRecharageLimit.inc"


#include "AvatarRechargeServer/IsEmblemRechargeStone.inc"


#include "AvatarRechargeServer/getEmblemRechargeStoneInfo.inc"


#include "AvatarRechargeServer/getEmblemRecharge.inc"


#include "AvatarRechargeServer/loadScript.inc"

