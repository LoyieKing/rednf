// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 AvatarRechargeServer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG C1：SCRIPT 子对象构造 + 清 +0x54 与字节 +0x58（+0x00 上限字段不初始化，忠实 ORIG）。
#include "AvatarRechargeServer/AvatarRechargeServer.inc"
}

// ============================================================================
// SendDurability —— ORIG T 0x08190416 _ZN...14SendDurabilityEP5CUsercsci
// ============================================================================
#include "AvatarRechargeServer/SendDurability.inc"
}

// ============================================================================
// CalcDurability —— ORIG T 0x0819049a _ZN...14CalcDurabilityEP5CUseri
// [b28d-solo-AvatarRechargeServer] AE（86=86）：①槽 0..9 循环体
// `if (!CalcDurabilityDetail(...)) continue;` 复现 ORIG 0x081904f5
// `xor $1,%eax; test %al,%al`（je 落循环增量处被 4.1 jump-thread）；
// ②rates 遍历缓存 size（P13）：`for(i=0,n=rates.size(); i<n; ++i)`。
// ============================================================================
#include "AvatarRechargeServer/CalcDurability.inc"
}

// CalcDurabilityChangeEquip —— ORIG T 0x081905b4：定义 UserCoreDepsB2.cpp（去重保留）。

// ============================================================================
// CheckConditionUseRecharge —— ORIG T 0x081906e2
// 充值石(区1)/头像(区2)存在性、IsEmblemRechargeStone、CheckItemLock×2 校验；
// 通过时 sig->maxEndurance = m_rechargeLimit × socket 数，返 true。
// ============================================================================
#include "AvatarRechargeServer/CheckConditionUseRecharge.inc"
}

// ============================================================================
// Recharge —— ORIG T 0x08190a70
// 删充值石 → 按 [emblem recharge stone info][itemId] 加耐久（clamp 上限）
// → 回传 addEndurance/enduranceAfter → DB 保存流。
// [b28d-solo-AvatarRechargeServer] NEAR（231=231，仅 4 行槽号差）：
// ①P59/P168a 负极性 guard（delete_item 失败块前置，ORIG 0x08190b5a `xor$1;test;je`）；
// ②P227 两处 cMyTrace 全表达式临时链 + P139 __PRETTY_FUNCTION__；
// ③P171 照抄 ORIG 只判 avatar->isEmpty()（无判空比较）；
// ④map 键具名物化槽（ORIG 0x08190cae `mov %eax,-0x3c`）。
// 残余 = 4.1 map-key 临时槽 -0x3c / trace1 -0x4c vs 4.4 -0x4c/-0x48（N82/N93
// 槽分配方向）；P167 callsite 25=25 CLEAR。
// ============================================================================
#include "AvatarRechargeServer/Recharge.inc"
}

// ============================================================================
// UseEmblem —— ORIG T 0x08190ed8
// 徽章合成加耐久：累加 grades 在 [emblem recharge] 的数值并 clamp。
// [b28d-solo-AvatarRechargeServer] AE（181=181）：真语义 bug 修正 —— ORIG
// 0x08190f39 `lea 0x20(%eax),%edx` = Server+0x20 = m_emblemRechargeMap
// （旧源码误用 m_useEmblemGradeRateMap this+0x38）；另 P52 链式去 invenR +
// P227 trace 全表达式临时链 + P13 end 缓存 / P50 迭代器先声明后赋值。
// ============================================================================
#include "AvatarRechargeServer/UseEmblem.inc"
}

// ============================================================================
// GmReduceEndurance —— ORIG T 0x081911b6 _ZN...17GmReduceEnduranceEP5CUserss
// ============================================================================
#include "AvatarRechargeServer/GmReduceEndurance.inc"
}

// ============================================================================
// GmChangeEnduranceReduceRate —— ORIG T 0x08191318
// GM 前置校验后转发 GmSetEnduranceReduceRate（ORIG 经 Singleton::Get 取 this，
// Meyers 单例同一对象，语义一致）。
// ============================================================================
#include "AvatarRechargeServer/GmChangeEnduranceReduceRate.inc"
}

// ============================================================================
// GmSetEnduranceReduceRate —— ORIG T 0x08191364
// enable 写 +0x58；rate<0→0 / ≤100→×10 / >100→1000 写 +0x54；系统提示。
// ============================================================================
#include "AvatarRechargeServer/GmSetEnduranceReduceRate.inc"
}

// ============================================================================
// UpdateAvatarEndurance —— ORIG T 0x081913f6
// 组 0x220 包：uid 低 16 位 / slot / item_idx / 耐久计数，SendPacket 上报。
// [b28d-solo-AvatarRechargeServer] AE：P48 去 invenW 具名缓存
// （ORIG 0x0819142a `mov %eax,(%esp); call GetInvenRef` 结果右值直传实参槽）。
// ============================================================================
#include "AvatarRechargeServer/UpdateAvatarEndurance.inc"
}

// ============================================================================
// AvatarRechargeServer —— ORIG C1 0x08190200 / D1 0x08190246 /
// Load 0x0819026c / Destroy 0x081902de / Test 0x08190266
// ============================================================================

#include "AvatarRechargeServer/dtor_AvatarRechargeServer.inc"
}

// ORIG 0x0819026c：cMyTrace(__PRETTY_FUNCTION__,0x22,9,true,false) +
// "\t- Loading Avatar Recharge script - "；loadScript()==0 即成功。
#include "AvatarRechargeServer/Load.inc"
}

// ORIG 0x081902de：恒返回 1（接口 void 化同 CerashopAddRestrict::Manager 先例）。
#include "AvatarRechargeServer/Destroy.inc"
}

// ORIG 0x08190266：纯 ret。
#include "AvatarRechargeServer/Test.inc"
}

// ORIG 0x0819114c：非静态成员，find 于 this+0x38（= m_useEmblemGradeRateMap）。
#include "AvatarRechargeServer/GetRateFromEmblemGrade.inc"
}

// ORIG 0x08190dea（静态）：Acquire 流 → make_internal_stream_jpn(0x31, -1) → put_binary 8 字节
// → DB_Q。不触碰 this，与 ORIG 一致保持静态。
#include "AvatarRechargeServer/SaveAvatarEnduranceUnit.inc"
}

// ORIG 0x08190eae（静态）：SigAradAvatarEnduranceSave ctor（清 +0/+4）后
// 写 uiId/endurance 两字段，走引用重载。
#include "AvatarRechargeServer/SaveAvatarEnduranceUnit_1.inc"
}

// ============================================================================
// ARAD::SCRIPT::AvatarRecharge —— ORIG C1 0x088b0dd0 / D1 0x088b0e46 /
// loadScript 0x088b0eba / clearScript 0x088b12d8 / 访问器 0x88b1314..0x88b13a6
// ============================================================================

// ORIG C1：三个 map 默认构造 + 清 +0x4c 已装载标志（+0x00 上限不初始化）。
#include "AvatarRechargeServer/AvatarRecharge.inc"
}

#include "AvatarRechargeServer/dtor_AvatarRecharge.inc"
}

// ORIG 0x088b12d8：上限清零 + 三 map clear。
#include "AvatarRechargeServer/clearScript.inc"
}

#include "AvatarRechargeServer/getAvatarRecharageLimit.inc"
}

#include "AvatarRechargeServer/IsEmblemRechargeStone.inc"
}

#include "AvatarRechargeServer/getEmblemRechargeStoneInfo.inc"
}

#include "AvatarRechargeServer/getEmblemRecharge.inc"
}

// ORIG 0x088b0eba loadScript。忠实转写：
//   已装载→0；打开失败→-1；重复键 printf（ORIG 无换行符）后分别 -2/-3/-4 且不卸载脚本；
//   主循环 ScanType 失败即 unloadRDARScriptFile + 置已装载 + 返回 0。
// [recharge use emblem info] 段 grade>=8 的行不消费数值直接退出内层（ORIG 同）。
#include "AvatarRechargeServer/loadScript.inc"
}
