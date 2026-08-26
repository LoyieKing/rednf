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

// ---------------------------------------------------------------------------
// 本 TU 内直连的 ORIG 外部符号（等价自由函数声明，i386 成员/静态 ABI 一致）。
// 注意：以下符号当前在重建源码中尚无定义方（见批次报告 blocked 清单），
// 链接期需相应 producer 批次落地。
// ---------------------------------------------------------------------------

// ORIG T 0x081f8a7c _Z30_check_disjointable_user_stateP5CUser14ENUM_CMDPACKETcs
// 定义 ExpertJobLink4.cpp（C++ mangled）；asm 标签对齐调用点字面名。
bool _Z30_check_disjointable_user_state(CUser* user, ENUM_CMDPACKET cmd,
                                        char spaceChar, short slotShort)
    asm("_Z30_check_disjointable_user_stateP5CUser14ENUM_CMDPACKETcs");

void _Z30_check_disjointable_item_state(CUser* user, int slot, int invenType,
                                        Inven_Item& item, CItem** pItem,
                                        CUser* targetUser, unsigned short region)
    asm("_Z30_check_disjointable_item_stateP5CUseriiR10Inven_ItemPP5CItemS0_t");

void _Z25_check_disjoint_need_slot(CUser* user, CItem* item,
                                   Inven_Item& invenItem,
                                   expert_job::CDisjointer* disjointer)
    asm("_Z25_check_disjoint_need_slotP5CUserP5CItemR10Inven_ItemPN10expert_job11CDisjointerE");

// ORIG 为 vptr[0] 虚槽调用：ORIG CDisjointer vtable 0x8cc0828 slot0 =
// OnStartMission(CUser*, CUser*)（nm 实证）。本仓库 CExpertJob.h 虚表布局未按
// ORIG 接口序重排（重排共享虚表超本批次范围），故按 ORIG mangled 直连非虚调
// 用，语义一致。实参序 = ORIG 虚调用 (job, targetUser, requestUser)。
extern "C" void _ZN10expert_job11CDisjointer14OnStartMissionEP5CUserS2_(
    expert_job::CExpertJob* job, CUser* target, CUser* request);

// ORIG W 0x082347ec _ZN10expert_job11CDisjointer19get_disjoint_resultEP5CItemP10Inven_ItemR14DisJointResultP5CUser
extern "C" int _ZN10expert_job11CDisjointer19get_disjoint_resultEP5CItemP10Inven_ItemR14DisJointResultP5CUser(
    expert_job::CDisjointer* job, CItem* item, Inven_Item* invenItem,
    DisJointResult& result, CUser* requestUser);

// ORIG T 0x085d31a0 _ZN10expert_job11CDisjointer17complete_disjointEP5CUserR11PacketGuard
extern "C" void _ZN10expert_job11CDisjointer17complete_disjointEP5CUserR11PacketGuard(
    expert_job::CDisjointer* job, CUser* requestUser, PacketGuard& packet);

// ORIG T 0x0868ff04 _ZN5CUser21CalcurateUserMaxLevelEv（CUser.h 未声明，禁改）
extern "C" void _ZN5CUser21CalcurateUserMaxLevelEv(CUser* user);

// ORIG T 0x0868e704 _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard（同上）
extern "C" void _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard(
    CUser* user, PacketGuard& packet);

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

    _ZN5CUser21CalcurateUserMaxLevelEv(user);    // +0x158（extern 直连）

    // +0x163 雇佣兵信息包：header(1,0x1ba)/byte(1)/SetMercenaryInfoPacket/
    // finalize(true)/Send
    PacketGuard mercenaryPacket;                 // -0x38 local_3c
    mercenaryPacket.put_header(1, 0x1ba);
    mercenaryPacket.put_byte(1);
    _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard(user, mercenaryPacket);
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

// ===========================================================================
// 2. DisPatcher_DisJointItem_disjoint —— ORIG 静态成员 _disjoint 的自由函数桥
//    ORIG 0x081f92ca..0x081f9fb7
// ===========================================================================
void DisPatcher_DisJointItem_disjoint(CUser* requestUser, int reason,
                                      int unk0, ENUM_CMDPACKET cmd,
                                      CUser* targetUser,
                                      unsigned short region)
{
    // +0x037 用户状态门（失败静默返回）
    if (!_Z30_check_disjointable_user_state(requestUser, cmd,
                                            static_cast<char>(unk0),
                                            static_cast<short>(reason)))
    {
        return;
    }

    PacketGuard packet;                          // -0x70 local_74
    packet.put_header(1, cmd);                   // +0x06d

    int invenType = GetInvenTypeFromItemSpace(unk0);  // +0x078 → -0x44

    expert_job::CExpertJob* expertJob = 0;       // -0x40 local_40
    if (targetUser != 0)
    {
        expertJob = targetUser->GetCurCharacExpertJob();  // +0x097
        if (expertJob == 0)
        {
            LogManager::logFormat(1, g_disjointSourceFile,
                                  g_disjointPrettyFunction, 0x87b7,
                                  "expert job null point");
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (expertJob->GetType() != 3)
        {
            LogManager::logFormat(1, g_disjointSourceFile,
                                  g_disjointPrettyFunction, 0x87bd,
                                  "expert job permission error");
            throw static_cast<ENUM_ERROR>(0x13);
        }
        // ORIG +0x185 vptr[0] 虚槽 = OnStartMission(target, request)
        _ZN10expert_job11CDisjointer14OnStartMissionEP5CUserS2_(
            expertJob, targetUser, requestUser);
    }

    if (requestUser->CheckItemLock(invenType, reason))    // +0x19b
        throw static_cast<ENUM_ERROR>(0xd5);

    Inven_Item invenItem;                        // -0xf9 local_fd
    CItem* item = 0;                             // -0x74 local_78

    if (region != 0xffff)
    {
        // +0x1ef PVP 通道禁拆
        if (G_GameWorld()->IsPVPChannel())
            throw static_cast<ENUM_ERROR>(0x60);
    }

    // +0x263 拆解物状态校验（写入 invenItem/item）
    _Z30_check_disjointable_item_state(requestUser, reason, invenType,
                                       invenItem, &item, targetUser, region);
    // +0x286 结果槽位校验
    _Z25_check_disjoint_need_slot(
        requestUser, item, invenItem,
        reinterpret_cast<expert_job::CDisjointer*>(expertJob));

    DisJointResult result;                       // -0xc0 local_c0（10×int）
    int err;
    if (targetUser == 0)
    {
        // +0x2f6 无专家职业：DisJoint 表结果（m_addInfo=物品 idx @+0x02）
        err = G_CDataManager()->m_disJoint->GetResult(invenItem.m_addInfo,
                                                      result);
    }
    else
    {
        // +0x2a0 专家职业拆解器结果
        err = _ZN10expert_job11CDisjointer19get_disjoint_resultEP5CItemP10Inven_ItemR14DisJointResultP5CUser(
            reinterpret_cast<expert_job::CDisjointer*>(expertJob), item,
            &invenItem, result, requestUser);
    }
    if (err > 0)
        throw static_cast<ENUM_ERROR>(err);

    if (region != 0xffff)
    {
        // +0x341 扣除拆解券（slot=region,count=1,invenType=1）
        CInventory* inventory = requestUser->getCurCharacInvenW();
        if (inventory->use_item(region, 1, 1) != 0)
            throw static_cast<ENUM_ERROR>(0x16);
        requestUser->SendUpdateItem(static_cast<CUser::eSendTarget>(1),
                                    static_cast<ENUM_ITEMSPACE>(0), region);
    }

    // +0x3ca 结果物品数组（ORIG 逆序 i=4..0 构造 Inven_Item[5]，语义等价）
    CItem* resultItems[5] = {0, 0, 0, 0, 0};     // -0x88 local_90[1..5]
    Inven_Item outItems[5];                      // -0x267 local_26b（步长 0x3d）

    // +0x41a 第一轮：按 result.m_f[5+i] 取模板生成产物并试插背包
    for (int i = 0; i < 5; ++i)
    {
        if (result.m_f[5 + i] == 0)              // aiStack_bc[i+4]
            continue;

        resultItems[i] = G_CDataManager()->find_item(result.m_f[5 + i]);
        if (resultItems[i] == 0)
            continue;

        outItems[i].m_addInfo = result.m_f[5 + i];        // +0x494 写 +0x02
        outItems[i].set_add_info(result.m_f[i]);          // +0x4b7 写 add_info
        resultItems[i]->make_item(outItems[i]);           // +0x4cb vtbl+0x8

        int slot = requestUser->getCurCharacInvenR()->tryInsertItemIntoInventory(
            outItems[i]);                                 // +0x55c
        if (slot == -1)
            throw static_cast<ENUM_ERROR>(4);
    }

    // +0x58c 删除源物品（invenType, reason, 1, eItemDelReason 9, log=true）
    CInventory* inventory = requestUser->getCurCharacInvenW();
    bool deleted = inventory->delete_item(
        static_cast<INVEN_TYPE>(invenType), reason, 1,
        static_cast<eItemDelReason>(9), true) != 0;
    if (!deleted)
        throw static_cast<ENUM_ERROR>(err);      // +0x98e2 抛 err（-0x3c）

    // +0x990d 源物品卖价统计（field 0x13）
    int sellPrice = 0;                           // -0x8c
    {
        short count = static_cast<short>(invenItem.m_addInfo2);  // +0x07 低16位
        Store* store = G_Store();
        store->GetSellItemPrice(invenItem, item, count, false, sellPrice);
        GetInstanceValueStatistic()->AddValueStatistic(
            static_cast<VALUE_STATISTIC_FIELD>(0x13), requestUser, sellPrice);
    }

    // +0x997c 动作点清理：无目标→请求方 0x07；同角色→0x20；异角色→目标 0x2d
    if (targetUser == 0)
    {
        APSystem::_ActionGroupIndex group;
        group.m_value = 7;
        APSystem::CUserProc::ClearActionAndSendtoUser(
            requestUser, group, 0, static_cast<ENUM_PACKETCLASS>(0));
    }
    else if (requestUser != 0)
    {
        int selfNo = requestUser->get_charac_no(-1);
        int targetNo = targetUser->get_charac_no(-1);
        APSystem::_ActionGroupIndex group;
        if (selfNo == targetNo)
        {
            group.m_value = 0x20;
            APSystem::CUserProc::ClearActionAndSendtoUser(
                requestUser, group, 0, static_cast<ENUM_PACKETCLASS>(0));
        }
        else
        {
            group.m_value = 0x2d;
            APSystem::CUserProc::ClearActionAndSendtoUser(
                targetUser, group, 0, static_cast<ENUM_PACKETCLASS>(0));
        }
    }

    Inven_Item unusedItem;                       // -0x136 local_13a（ORIG 构造后未用，残留）

    // +0x9a2f 应答包头：byte(1)/short(reason)/byte(unk0)/idx 占位 byte(0)
    packet.put_byte(1);
    packet.put_short(reason);
    packet.put_byte(unk0);
    int countIndex = packet.get_index();         // -0x90
    packet.put_byte(0);
    int outCount = 0;                            // -0x30 local_34

    // +0x9aab 第二轮：入包产物、统计、逐项写应答
    for (int i = 0; i < 5; ++i)
    {
        if (resultItems[i] == 0)
            continue;

        if (resultItems[i]->is_stackable() && outItems[i].get_add_info() > 999)
        {
            cMyTrace trace(g_disjointPrettyFunction, 0x8867, 5);
            trace("DisPatcher_DisJointItem::dispatch_sig, out_item , ITEM #%d, %d",
                  outItems[i].m_addInfo, outItems[i].get_add_info());
        }

        int slot = requestUser->getCurCharacInvenW()->insertItemIntoInventory(
            outItems[i], static_cast<eItemAddReason>(10), true, false);
        if (slot < 0)
            continue;

        // +0x9c20 槽内容一致性校验（仅日志）
        const Inven_Item* ref =
            requestUser->getCurCharacInvenR()->GetInvenRef(1, slot);
        if (ref != 0 && ref->m_addInfo != outItems[i].m_addInfo)
        {
            cMyTrace trace(g_disjointPrettyFunction, 0x888a, 0);
            trace("ENUM_CMDPACKET_DISJOINT_ITEM error item_id mismatch %d, %d",
                  ref->m_addInfo, outItems[i].m_addInfo);
        }

        packet.put_short(slot);
        packet.put_int(outItems[i].m_addInfo);
        packet.put_int(outItems[i].m_addInfo2);

        // +0x9d19 方块统计：无目标 field 0；有专家职业 field 2
        if (targetUser == 0)
        {
            GetInstanceCubeStatistic()->collectCubeStatistics(
                outItems[i].m_addInfo, outItems[i].m_addInfo2, requestUser,
                static_cast<CUBE_STATISTIC_FIELD>(0));
        }
        else if (targetUser->GetCurCharacExpertJob() != 0)
        {
            GetInstanceCubeStatistic()->collectCubeStatistics(
                outItems[i].m_addInfo, outItems[i].m_addInfo2, requestUser,
                static_cast<CUBE_STATISTIC_FIELD>(2));
        }

        // +0x9ddc 产物卖价统计（field 0x8）
        int itemPrice = 0;                       // -0x94
        {
            short count = static_cast<short>(outItems[i].m_addInfo2);
            Store* store = G_Store();
            store->GetSellItemPrice(outItems[i], resultItems[i], count, false,
                                    itemPrice);
            GetInstanceValueStatistic()->AddValueStatistic(
                static_cast<VALUE_STATISTIC_FIELD>(8), requestUser, itemPrice);
        }
        ++outCount;
    }

    // +0x9e86 回填产物计数
    packet.put_byte(countIndex, outCount);

    if (targetUser != 0)
    {
        // +0x9ea8 专家拆解收尾
        _ZN10expert_job11CDisjointer17complete_disjointEP5CUserR11PacketGuard(
            reinterpret_cast<expert_job::CDisjointer*>(expertJob), requestUser,
            packet);
    }

    packet.finalize(true);
    requestUser->Send(packet);

    // +0x9ed? 分解历史日志（history FILE* @ CUser+0x796f8）
    HistoryLog::WriteDisJoint(
        reinterpret_cast<FILE*>(requestUser->m_field796f8),
        invenItem.m_addInfo);
}
