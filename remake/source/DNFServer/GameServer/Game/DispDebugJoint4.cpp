// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DispDebugJoint4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>

#include "DisPatcherCore.h"     // DisPatcher_DebugCommand（含 MSG_BASE/PacketBuf 域）
#include "CUser.h"              // CUser/CUserCharacInfo/UserQuest/eSendTarget/ENUM_ERROR
#include "CDataManager.h"       // G_CDataManager/find_item/get_level_exp/m_disJoint
#include "DisJoint.h"           // DisJoint::GetResult/DisJointResult
#include "CInventory.h"         // CInventory/Store/G_Store/CValueStatistic/
                                // GetInstanceValueStatistic/GetInvenTypeFromItemSpace
#include "Inven_Item.h"
#include "CItem.h"              // make_item/is_stackable
#include "CExpertJob.h"         // expert_job::CExpertJob/CDisjointer
#include "GameWorld.h"          // G_GameWorld/IsPVPChannel
#include "CCubeStatistic.h"     // GetInstanceCubeStatistic/collectCubeStatistics
#include "CUserProc.h"          // APSystem::CUserProc::ClearActionAndSendtoUser
#include "HistoryLog.h"         // HistoryLog::WriteDisJoint
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "PacketGuard.h"

// b22 桥清零（§8 协议，Model22）：原 extern"C" 五桥全清——
//   CDisjointer::{OnStartMission,get_disjoint_result,complete_disjoint} 三桥
//   声明在本 TU 无调用点（真实 _disjoint 逻辑在 DisPatcherCore.cpp:6320/6358/
//   6495 已是成员真调用，CExpertJob.h 权威声明），按 §8-4 删无语用桥声明；
//   CUser::CalcurateUserMaxLevel / CUser::SetMercenaryInfoPacket 二桥转真成员
//   调用（CUser.h:2633-2634 已声明，零新声明）。

// [OurOnly33 符号名轴 2026-09-11] 原手写串 static（g_disjointPrettyFunction /
// g_disjointSourceFile，ORIG 全 ELF 无该 base 名，我方 `_ZL24…`/`_ZL20…` r 段）
// 在本 TU **无任何引用点**（grep 全 source 仅命中此定义；GCC 默认
// -fkeep-static-consts 故其符号此前仍被发射）⇒ 删除。
// ORIG 权威：`_ZZN23DisPatcher_DisJointItem9_disjointEP5CUserii14ENUM_CMDPACKETS1_tE19__PRETTY_FUNCTION__`
// @0x08bcfd20 (r,0x6c=108B) —— 该 prett 符号由 DisPatcherCore/_disjoint.inc 的
// __PRETTY_FUNCTION__ 使用点发射，与这两个未引用手写串无关。

// ===========================================================================
// 1. DisPatcher_DebugCommand::_debugCommandSetLevel
//    ORIG 0x0858efde..0x0858f3b9
// ===========================================================================
#include "DispDebugJoint4/_debugCommandSetLevel.inc"
{
    // b28d-37 [N18a 封顶，ORIG 0x0858efde]：`mov $I,%eax` vs `mov $I,%ebx`
    // callee-saved 分配差（264/266），语义闭合。
    if (!user->isGMUser())                       // +0x00e isGMUser; !=1 → ret 0
        return 0;
    if (level > 0x46)                            // +0x024 cmpl $0x46; jle 继续
        return 0;

    bool changed = true;                         // -0x19 local_1d
    int delta = level - user->get_charac_level();  // -0x18 local_1c

    if (delta == 0)
    {
        changed = false;
    }
    else if (delta < 1)
    {
        // +0x0bb 降级环：for(i=0; delta<i; --i) SetCharacLevel(min(200, lv-1))
        for (int i = 0; delta < i; --i)
        {
            int next = std::min(200, user->get_charac_level() - 1);
            user->SetCharacLevel(next);
        }
    }
    else
    {
        // +0x064 升级环：for(i=0; i<delta; ++i) SetCharacLevel(min(lv+1, 200))
        for (int i = 0; i < delta; ++i)
        {
            int next = std::min(user->get_charac_level() + 1, 200);
            user->SetCharacLevel(next);
        }
    }

    user->_onLevelUp();                          // +0x110
    // +0x11b setCurCharacExp(G_CDataManager()->get_level_exp(get_charac_level()))
    int curLevel = user->get_charac_level();
    user->setCurCharacExp(G_CDataManager()->get_level_exp(curLevel));

    if (!changed)                                // +0x14e test local_1d
        return 0;

    user->CalcurateUserMaxLevel();               // +0x158

    // +0x163 雇佣兵信息包：header(1,0x1ba)/byte(1)/SetMercenaryInfoPacket/
    // finalize(true)/Send
    PacketGuard mercenaryPacket;                 // -0x38 local_3c
    mercenaryPacket.put_header(1, 0x1ba);
    mercenaryPacket.put_byte(1);
    user->SetMercenaryInfoPacket(mercenaryPacket);
    mercenaryPacket.finalize(true);
    user->Send(mercenaryPacket);

    // +0x1d3 满装备等级追平角色等级则清零
    bool resetMaxEquip = false;
    int maxEquipLevel = user->GetCurCharacMaxEquipLevel();
    if (maxEquipLevel != 0 &&
        maxEquipLevel == user->get_charac_level())
    {
        resetMaxEquip = true;
    }
    if (resetMaxEquip)
        user->SetCurCharacMaxEquipLevel(0);      // +0x21c
    // ORIG 两分支各持独立 PacketGuard 槽（-0x44/-0x50）；C++ 分支内构造无法
    // 复刻帧布局，提升为函数级共享槽（本地缓冲，无行为差异）。
    PacketGuard questPacket;

    int state = user->get_state();
    if (state == 3)
    {
        // +0x23f 地下城内：0x25 提示 + 双段任务信息刷新
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_NOTIPACKET>(0x25), 0);
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(0),
                             static_cast<ENUM_NOTIPACKET>(2), 0);
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_NOTIPACKET>(2), 1);
        UserQuest* quest = user->getCurCharacQuestR();
        quest->get_quest_info(reinterpret_cast<char*>(&questPacket));
        user->Send(questPacket);
    }
    else if (state == 5)
    {
        // +0x309 战场内：0x25 提示 + 任务信息刷新
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_NOTIPACKET>(0x25), 0);
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                             static_cast<ENUM_NOTIPACKET>(2), 1);
        UserQuest* quest = user->getCurCharacQuestR();
        quest->get_quest_info(reinterpret_cast<char*>(&questPacket));
        user->Send(questPacket);
    }
    return changed ? 1 : 0;                      // +0x3d0 movzbl local_1d
}
