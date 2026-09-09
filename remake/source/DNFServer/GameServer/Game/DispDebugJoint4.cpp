// ============================================================================
// df_game_r 还原 —— LinkMisc4 批次：Dispatcher 域 2 符号（DispDebugJoint4.cpp）
//
//   1. DisPatcher_DebugCommand::_debugCommandSetLevel(CUser*, int)
//      ORIG _ZN23DisPatcher_DebugCommand21_debugCommandSetLevelEP5CUseri
//      T 0x0858efde sz 0x3db
//      报告 docs/class_func_reports/DisPatcher_DebugCommand/_debugCommandSetLevel.md
//      声明为 DisPatcherCore.h 类内纯追加（本批次）。
//
//   2. 自由函数 DisPatcher_DisJointItem_disjoint —— 桥接转写。
//      ORIG 真实对应静态成员
//      DisPatcher_DisJointItem::_disjoint(CUser*, int, int, ENUM_CMDPACKET,
//                                         CUser*, unsigned short)
//      ORIG _ZN23DisPatcher_DisJointItem9_disjointEP5CUserii14ENUM_CMDPACKETS1_t
//      T 0x081f92ca sz 0xcee
//      报告 docs/class_func_reports/DisPatcher_DisJointItem/_disjoint.md
//      桥接原因：调用方 GmCmds_F.cpp:58 已按自由函数名
//      DisPatcher_DisJointItem_disjoint 声明并禁改；ORIG 静态成员无 this、
//      i386 下与同参数表自由函数 ABI 一致，故在本 TU 以该自由函数承载完整
//      ORIG 逻辑（逐段对照报告汇编/反编译 C 转写）。
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

// ORIG _disjoint 内 logFormat/__PRETTY_FUNCTION__ 原文（报告 0x87b7/0x87bd 行）
static const char g_disjointPrettyFunction[] =
    "static int DisPatcher_DisJointItem::_disjoint(CUser*, int, int, "
    "ENUM_CMDPACKET, CUser*, short unsigned int)";
static const char g_disjointSourceFile[] = "PacketDispatcher_Impl_1.cpp";

// ===========================================================================
// 1. DisPatcher_DebugCommand::_debugCommandSetLevel
//    ORIG 0x0858efde..0x0858f3b9
// ===========================================================================
char DisPatcher_DebugCommand::_debugCommandSetLevel(CUser* user, int level)
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
