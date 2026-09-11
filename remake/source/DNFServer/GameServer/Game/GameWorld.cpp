// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GameWorld/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "GameWorld.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"   // 权威 CInventory/PacketGuard（本文件原本地 PacketGuard 已删除）
#include "InterSelectCreateDnfEventInfo.h"   // Reward 权威定义（本地局部类已删除，§4.4 合并）
#include "CItemList.h"
#include "Inven_Item.h"
#include "CDataManager.h"
#include "CraneMinigameManager.h"  // CraneMinigameManager::updateCraneItemNeedMaterial
#include "CUserProc.h"       // APSystem::CUserProc::SetTodayActionAndCheckMedalReward
#include "QuickParty.h"      // QuickParty::CQuickPartySystemManager（CDungeon.h 本地最小声明在前置）
#include "CDungeon.h"        // CDungeon::get_min_level（getDungeonMinimumRequiredLevel）
#include "CEnvironment.h"
#include "PvP_Room.h"
#include "CGameManager.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "CGM_Manager.h"  // 权威 CGM_Manager::IsGm（s_GM_Manager 指针解引用）
#include "CNetwork.h"
#include "CSystemTime.h"
#include "CParty.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "Stream.h"
#include "TaiwanInternal.h"
#include "LogManager.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"   // CServerProxyMgr<CStatisticServerProxy> 完整类型
#include "CHackLog_InvalidAreaMove.h"
#include "Arad_EventPeriodDataManager.h"
#include "Packet_MiniCraneSeed.h"
#include "STQuestScript.h"      // initTownScript / importTownScript / TownScript
#include "CPrivateStoreMgr.h"   // private_store::GetInstancePrivateStoreMgr
#include "CSchoolMgr.h"         // g_schoolMgr
#include "CGuildAgitManager.h"  // g_guildAgitMgr
#include "CPowerManager.h"      // GlobalData::s_power_manager 方法
#include "CAssaultMgr.h"        // pvp_assault::GetInstanceAssaultMgr
#include "CSyncSlangFilter.h"   // CSyncSlangFilter::GetInstance / Filter
#include "CTitleBook.h"         // CTitleBook::sendListOtherUser
#include "CMyFileLog.h"         // CMyFileLog
#include "CFatigueBatteryHandle.h"
#include "CBoosterGage.h"
#include "CEventScriptMng.h"    // EventClassify::CEventScriptMng
#include "CTimeGate.h"
#include "CPartyTelePort.h"     // CPartyTelePort::get_teleport_state
#include "Arad_DataManager.h"   // ARAD::Arad_DataManager / Singleton / notifyOpenMessageDialog
#include "HeroMissionEvent.h"   // ARAD::Singleton<HeroMissionEvent>
#include "AradServerStateMessage.h"
#include "InterSelectPcroomDailyReward.h"
#include "PacketTypes.h"         // MSG_ITEM_HYPER_LINK_MESSAGE 权威定义（原 TU 局部 struct 已删除）
#include "DB_AccountCargoSync.h" // DB_PassPadUpdateFailCnt / APSystem::DB_UpdateActionPoint
#include "CEventStayTime.h"      // CEventStayTime::RewardGoGoFighter 权威（原局部视图 ODR 清理）
#include "Packet_Monitor_Guild_Chat_Hyper_Link.h"    // case 6 超链接公会消息
#include "Packet_Monitor_Member_Chat_Hyper_Link.h"   // case 8 超链接会员消息
#include "Packet_Monitor_MegaPhone_Hyper_Link.h"     // case 0xb/0xc/0xe/0x23 超链接喇叭消息（set 声明，定义已报 Main）
#include "Packet_Monitor_Other_Channel_Chat_Hyper_Link.h"  // case 1 超链接跨服消息
#include "Packet_Monitor_Other_Channel_Chat.h"  // case 1 跨服私聊监控包（ORIG 0x86ca0b9 布局）
#include "../../../shared/packet/include/Packet_Monitor_Guild_Chat.h"  // case 6 公会聊天监控包（ORIG 0x86ca6ac；Game/ 空壳头同名，必须用 shared 完整版）
#include "../../../shared/packet/include/Packet_Monitor_Member_Chat.h"  // case 8 会员聊天监控包（ORIG 0x86ca7dc）
#include "../../../shared/packet/include/Packet_Monitor_MegaPhone.h"  // case 0xb/0xc/0xe 喇叭监控包（ORIG 0x86ca8b8）

// ============================================================================
// CStreamGuard::GetInBuffer 模板实参（全局类型：C++98 不允许局部类型作为模板
// 实参；全局同名类型使 guard.GetInBuffer<SIG_*>() 自然产生 ORIG 模板实例符号
// _ZN12CStreamGuard11GetInBufferI24SIG_UPDATE_STATISTIC4LEVEEPT_v 与
// _ZN12CStreamGuard11GetInBufferI17SIG_NAT_TYPE_USEREEPT_v）

// ============================================================================
// sync_script::CSyncScript 最小声明（不 include sync_script.h：其 ST* 镜像
// 声明与本 TU 经 CUser.h/CDataManager.h 引入的权威 STEquipmentScript/
// EquipmentParameterInfo 冲突；本 TU 仅需 GetInstanceSyncScript +
// truncate/insert_town_info，声明与 sync_script.h 同签名（ORIG mangled 一致））
// ============================================================================
namespace sync_script
{
class CSyncScript
{
public:
    bool insert_town_info_to_db(const TownScript& script);  // _ZN11sync_script11CSyncScript22insert_town_info_to_dbERK10TownScript
    bool truncate_town_info_table();                        // _ZN11sync_script11CSyncScript24truncate_town_info_tableEv
};
CSyncScript* GetInstanceSyncScript();   // _ZN11sync_script21GetInstanceSyncScriptEv
}

// ============================================================================
// 跨类方法（原 extern "C" asm 桥已删除；均改用权威头真实调用：
//   CInventory::insertItemIntoInventory — CInventory.h
//   CUser::GetServerGroup / getPowerSide / GetEventCreateDnfReward — CUser.h
//   CGM_Manager::IsGm — CGM_Manager.h（s_GM_Manager 为指针，按 ORIG 解引用））
// ============================================================================


// ServerParameterScript 方法现由真实 ServerParameterScript.h 提供（m_serverParameter 成员），
// 旧 23 错拼长度 asm 桥接已随调用迁移删除。

// ---- Taiwan 事件流（CEventStayTime::RewardGoGoFighter 依赖）----

// ============================================================================
// 全局数据（ORIG：MAX_VILLAGE_NUM 0948b2b0 / g_townScriptFileList 09500ea0）
// ============================================================================
int MAX_VILLAGE_NUM;
std::map<int, std::string> g_townScriptFileList;

#include "GameWorld/get_gate_area.inc"


// ============================================================================
// 单例
// ============================================================================
// 判据㉓（符号绑定，Main 已批准单点修法）：ORIG `_Z11G_GameWorldv` 0x080da3a7 为
// W 弱绑定，我方原为 T 强绑定 ⇒ 定义加 weak（只改本 .cpp，零头文件风险）。
__attribute__((weak)) GameWorld* G_GameWorld()
{
    return GlobalInstance<GameWorld>::inst_ptr();
}

#include "GameWorld/CREATE_GameWorld.inc"


#include "GameWorld/DESTROY_GameWorld.inc"


#include "GameWorld/GameWorld.inc"


#include "GameWorld/dtor_GameWorld.inc"


#include "GameWorld/destroy.inc"


#include "GameWorld/init.inc"


#include "GameWorld/InsertChannel.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x11a0),
                       true);
    **guard << 0x2d;
    **guard << -1;

    struct __attribute__((packed)) stChannelInsertInfo
    {
        int m_gcNo;                 // +0x00
        unsigned short m_zero;      // +0x04
        char m_serverIP[0x10];      // +0x06
        unsigned short m_tcpPort;   // +0x16
        unsigned short m_maxUserNum;// +0x18
        char m_serverGroup;         // +0x1a
        char m_channelName[0x1e];   // +0x1b
        unsigned short m_channelNo; // +0x39
        char m_channelInfo[0x19];   // +0x3b
        int m_channelType;          // +0x54 ← channel_info_t::m_channelType
        int m_group;                // +0x58
    };
    stChannelInsertInfo buf;
    memset(&buf, 0, 0x5c);
    buf.m_gcNo = gcNo;
    buf.m_group = (int)group;
    buf.m_zero = 0;

    memcpy(buf.m_serverIP, G_CEnvironment()->m_serverEnvir.m_serverIP,
           strlen(G_CEnvironment()->m_serverEnvir.m_serverIP));
    buf.m_tcpPort = (unsigned short)G_CEnvironment()->m_serverEnvir.m_tcpPort;
    buf.m_maxUserNum =
        (unsigned short)G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    buf.m_serverGroup = (char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    memcpy(buf.m_channelName, G_CEnvironment()->m_serverEnvir.m_channelName,
           strlen(G_CEnvironment()->m_serverEnvir.m_channelName));
    buf.m_channelNo =
        (unsigned short)G_CEnvironment()->m_serverEnvir.m_channelNo;

    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    channel_info_t* channelInfo = channelScript->getChannelInfo(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        G_CEnvironment()->get_channel_no());
    if (channelInfo != NULL)
    {
        // ORIG（0x086cb88a +0x18b）：`add $0x8,%eax` 后 call std::string::c_str，
        // 即取 m_dungeonName 的 C 字符串（非对象裸内存 +8）。
        strncpy(buf.m_channelInfo, channelInfo->m_dungeonName.c_str(), 0x19);
    }
    if (channelInfo != NULL)   // ORIG +0x1b3 第二次独立 cmpl $0（P48 双臂不合并）
    {
        buf.m_channelType = channelInfo->m_channelType;   // +0x1bc mov 0x4(%eax),%eax
    }

    guard->put_binary(&buf, 0x5c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "GameWorld/DeleteChannel.inc"
{
    // N82/P112 封顶：指令序/call 序/EH 清理段逐条全等（78/78），残差全为
    // guard/buf/trace 槽序（4.1 声明正序 buf→guard→trace；4.4 实测声明逆序
    // 分配，trace 构造时机不可前移，buf 提前声明已试无收益）。帧 0x40/0x30
    // 同源。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x11bc),
                       true);
    **guard << 0x2f;
    **guard << -1;

    struct stChannelDeleteInfo
    {
        int m_gcNo;     // +0x00
        int m_group;    // +0x04
        int m_reserved; // +0x08  ORIG sizeof=0xc（memset 0xc / put_binary 0xc 证据），
                        //        该字段仅由 memset 清零，ORIG 无独立写入
    };
    stChannelDeleteInfo buf;
    memset(&buf, 0, 0xc);
    buf.m_gcNo = gcNo;
    buf.m_group = (int)group;

    guard->put_binary(&buf, 0xc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    // b28c-lane3：P221 全表达式临时链——具名 trace 使 4.4 把 trace 放最深槽
    // (-0x28)，临时链后 4.4 复现 ORIG 槽序 trace -0x18 / guard -0x20 / buf -0x2c。
    cMyTrace(__PRETTY_FUNCTION__, 0x11c7, 0)(
        "put db message to db thread DeleteChannel()");
}

#include "GameWorld/UpdateChannelUser.inc"


#include "GameWorld/updateDetailChannelServiceInfo.inc"

#include "GameWorld/UpdateStatistic4ChannelUserLev.inc"


#include "GameWorld/GetChannelUserCount.inc"


#include "GameWorld/_GetChannelType.inc"


#include "GameWorld/GetChannelType.inc"


#include "GameWorld/IsIntegratedPvPBaseChannel.inc"


#include "GameWorld/IsPVPChannel.inc"


#include "GameWorld/IsFreePvPChannel.inc"


#include "GameWorld/IsWinPointPvPChannel.inc"


#include "GameWorld/IsSameTeamChannelJoin.inc"


#include "GameWorld/IsSchoolPvPChannel.inc"


#include "GameWorld/IsPvPSkilTreeChannel.inc"


#include "GameWorld/IsEquipSlotSwitchChannel.inc"


#include "GameWorld/IsPvPVillageMapChannel.inc"


#include "GameWorld/IsCharacterPvPExpRevisionChannel.inc"


#include "GameWorld/IsCharacterLevelRevisionChannel.inc"


#include "GameWorld/IsCharacterStatRevisionChannel.inc"


#include "GameWorld/is_dungeon_tag_matching_channel.inc"


#include "GameWorld/InsertLoginUser.inc"


#include "GameWorld/EraseLoginUser.inc"


#include "GameWorld/InsertUserToIDMap.inc"


#include "GameWorld/DeleteUserInIDMap.inc"


#include "GameWorld/GetIDUserInIDMap.inc"


#include "GameWorld/reach_game_world.inc"


#include "GameWorld/leave_game_world.inc"


#include "GameWorld/make_unique_id.inc"


#include "GameWorld/find_session.inc"


#include "GameWorld/find_from_world.inc"


#include "GameWorld/find_user_from_world_byaccid.inc"


#include "GameWorld/find_user_from_world_byaccid_loop.inc"


 CUser* GameWorld::find_user_by_charac_name(const char* name)
 {
     // ORIG 0x86c9… 族：for-init 声明（begin sret 直落槽）；四个子条件各自
     // 独立物化（setg / test;setne;test / sete;test）⇒ 嵌套单层 if 形，
     // 单层 && 链在 4.4 会被折叠成直跳
     for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
          it != m_UserInWorld.end(); it++)
     {
         CUser* user = it->second;
         if (user != NULL)
         {
             if (user->get_state() > 2)
             {
                 if (user->getCurCharacR() != NULL)
                 {
                     if (strcmp(user->getCurCharacName(), name) == 0)
                     {
                         return user;
                     }
                 }
             }
         }
     }
     return NULL;
 }

#include "GameWorld/arrange_users.inc"


#include "GameWorld/CheckUserCount.inc"


#include "GameWorld/GetUserCount.inc"


#include "GameWorld/get_UserCount_InWorld.inc"


#include "GameWorld/UpdateNatTypeUser.inc"


#include "GameWorld/send_all.inc"


#include "GameWorld/send_all_1.inc"


#include "GameWorld/send_all_excepted_user.inc"


#include "GameWorld/send_all_with_state.inc"


#include "GameWorld/send_all_dungeon_inout_message.inc"


#include "GameWorld/send_all_dungeon_inout_message_1.inc"


#include "GameWorld/send_user_dungeon_inout_message.inc"


#include "GameWorld/send_to_area.inc"


#include "GameWorld/send_to_area_1.inc"


#include "GameWorld/send_to_area_2.inc"


#include "GameWorld/send_to_user.inc"


#include "GameWorld/send_to_vill.inc"


#include "GameWorld/send_group.inc"


#include "GameWorld/send_server_group.inc"


#include "GameWorld/send_party_info_to_all.inc"


#include "GameWorld/_makeBasicInfoHeader.inc"


#include "GameWorld/send_AllBasicInfo.inc"


#include "GameWorld/send_buddy_conn_list.inc"


#include "GameWorld/send_power_war_hp_of_vill.inc"


#include "GameWorld/GetVillage.inc"


#include "GameWorld/getUserVillage.inc"


#include "GameWorld/getUserVillage_1.inc"


#include "GameWorld/get_gate_area_1.inc"


#include "GameWorld/get_area_center_point.inc"


#include "GameWorld/is_available_point.inc"


#include "GameWorld/check_valid_area.inc"


#include "GameWorld/isDungeonEntranceArea.inc"


#include "GameWorld/getDungeonMinimumRequiredLevel.inc"


#include "GameWorld/GetWorldMapIndex.inc"


#include "GameWorld/get_user_id_list.inc"


#include "GameWorld/get_disposition.inc"
                                pvp_assault::CDisposition* disposition)
{
    // ORIG 0x086c4xxx：正向 test;je 早跳 + 尾部 return false（! 形态会多出 xor 物化）
    if (check_valid_area(vill, area))
    {
        return (*(bool (**)(void*, void*))*(void**)disposition)(
            disposition, (void*)&m_pVill[vill].getArea(area)->m_mapAreas);   // Area +0x7c
    }
    return false;
}

// ORIG CUserModifier 最小镜像（df_game_r modify_user 0x86c9…：`mov (%eax),%eax;
// mov (%eax),%edx … call *%edx` vtable 槽0=Modify(CUser*)，thiscall 栈传 this+user；
// 无虚析构 ⇒ slot0。与 InterDispatcher.cpp:16732 同口径镜像。）
class CUserModifier
{
public:
    virtual void Modify(CUser* user) = 0;
};

#include "GameWorld/modify_user.inc"


#include "GameWorld/check_restrictive_commercial_transaction_zone.inc"


#include "GameWorld/move_position.inc"


#include "GameWorld/check_move_area.inc"


#include "GameWorld/move_area.inc"


#include "GameWorld/goto_dungeon.inc"


#include "GameWorld/out_from_dungeon.inc"


#include "GameWorld/goto_pvp.inc"


#include "GameWorld/out_from_pvp.inc"


#include "GameWorld/goto_warroom.inc"


#include "GameWorld/out_from_warroom.inc"


#include "GameWorld/gotoBlueMarble.inc"


#include "GameWorld/outFromBlueMarble.inc"


#include "GameWorld/fishing.inc"


#include "GameWorld/IsPowerWarSafeZone.inc"


#include "GameWorld/IsPowerWarSafeZone_1.inc"


// ============================================================================
// 禁言 / 禁移动族（ORIG 086cd908..086ced1d；逻辑来自 Ghidra C + 反汇编）
// ============================================================================

#include "GameWorld/IsForbiddenChat.inc"


#include "GameWorld/IsForbiddenMove.inc"


#include "GameWorld/WhenIsTheEndOfForbiddenChat.inc"


#include "GameWorld/WhenIsTheEndOfForbiddenMove.inc"


#include "GameWorld/GetLeftTimeOfForbiddenChat.inc"


#include "GameWorld/GetLeftTimeOfForbiddenMove.inc"


// ============================================================================
// 每日重置族（ORIG 086cc340..086cd15f；逻辑来自 Ghidra C + 反汇编）
// ============================================================================

#include "GameWorld/EnableChat.inc"


#include "GameWorld/EnableMove.inc"


#include "GameWorld/DisableChat.inc"


#include "GameWorld/DisableMove.inc"



#include "GameWorld/ResetMidnight.inc"


#include "GameWorld/resetDailyData.inc"


#include "GameWorld/ResetAllCoins.inc"


#include "GameWorld/ResetAllCleadpadPoint.inc"


#include "GameWorld/ResetAllPassPadFailCount.inc"


#include "GameWorld/ResetAllEventData.inc"


#include "GameWorld/ResetAllFatigue.inc"


// ============================================================================
// 缓存 / 商店 / 任务重置族（ORIG 086cf684..086d13ba）
// ============================================================================

#include "GameWorld/ResetCacheCharactorMemory.inc"


#include "GameWorld/ResetCacheCharactorMemory_1.inc"


#include "GameWorld/ResetEntireReliablePerson.inc"


#include "GameWorld/ResetOneADayItemShop.inc"


#include "GameWorld/testResetOneADayItemShop.inc"


#include "GameWorld/SetWeekendBounsEvent.inc"
{
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    localtime_r(&t, &local);
    // ORIG 86d12dc/86d12ec/86d12f3: mov -0x24(%ebp) = local+0x18 = tm_wday
    // b28d-lane11 新试形（P223 定案① 负条件 goto 共享尾标签串）：ORIG 尾
    // 0x086d1358/0x086d13ac 两 jmp + 0x086d13ae/0x086d13b1/0x086d13b4 三 nop 岛
    // + 0x086d13b7 fall nop；标签序 L0(0x3ac)/L3(0x3ae)/L1(0x3b1)/L2(0x3b4)。
    if ((local.tm_wday == 6 && local.tm_hour > 5) || local.tm_wday == 0 ||
        (local.tm_wday == 1 && local.tm_hour < 6))
    {
        if (!GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0))
        {
            Word_Param param;
            param.m_duration = 0;
            param.m_field2 = 0;
            GlobalData::s_event_manager->TriggerEventStart(0x57, param);
        }
        else
        {
            goto L3;
        }
        goto END;
    }
    else
    {
        if (local.tm_wday != 1)
        {
            goto L1;
        }
        if (local.tm_hour <= 5)
        {
            goto L2;
        }
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0))
        {
            GlobalData::s_event_manager->TriggerEventEnd(0x57);
            goto L0;
        }
        return;
    }
    // b28d-lane11 判词（0x086d1358/0x086d13ac/0x086d13ae/0x086d13b1/0x086d13b4
    // /0x086d13b7 实证）：if/else 保留 then 块物理前置 + 尾标签串造岛后 76=76，
    // 唯 L3(0x3ae) 岛的 jmp 被 4.4 thread 到相邻同目标 L1 岛的 jmp（P223 边界：
    // 单源 je 进入的尾岛恒合并）⇒ ndiff 封 1（N223）。
L0:;
L3:;
L1:;
L2:;
END:;
}

#include "GameWorld/DeleteDailyItem.inc"


#include "GameWorld/AddDailyItem.inc"


#include "GameWorld/ReselectDailyQuest.inc"


#include "GameWorld/DailyEventModify.inc"


#include "GameWorld/ResetCraneItemNeedMaterial.inc"


#include "GameWorld/deleteSpecificItems.inc"
                        const std::vector<std::pair<int, int> >* list);

#include "GameWorld/deleteSpecificItems_1.inc"


#include "GameWorld/UpdateServerSnapShot.inc"


#include "GameWorld/UpdateMiniCraneSeed.inc"


// ============================================================================
// 频道判定 / 统计访问器 / 力战族（ORIG 82343ba..86cf56c）
// ============================================================================

#include "GameWorld/InitChannelType.inc"



#include "GameWorld/IsEnchantRevisionChannel.inc"


#include "GameWorld/IsCheckUnusableEquipmentChannel.inc"


#include "GameWorld/getStatisticsPvP.inc"


#include "GameWorld/GetStatisticsNatType.inc"


#include "GameWorld/getStatisticsShusiaService.inc"


#include "GameWorld/getStatisticsToObserveCrash.inc"


#include "GameWorld/CancelPowerUp.inc"


#include "GameWorld/GetPowerUpDomain.inc"


#include "GameWorld/SendPowerUpList.inc"


#include "GameWorld/OnEndPowerWar.inc"


#include "GameWorld/OnRefreshPowerWarProcessInfo.inc"


#include "GameWorld/OnBanAbuserFromPowerWar.inc"


#include "GameWorld/CleanupPowerWarVillToStart.inc"


#include "GameWorld/get_user_info.inc"


#include "GameWorld/make_packet_chat_msg.inc"


#include "GameWorld/make_packet_chat_msg_hyper_link.inc"


#include "GameWorld/WriteLogChatMsg.inc"


#include "GameWorld/send_peerTypemsg_sameWorldUser.inc"


#include "GameWorld/check_peerTypemsg_sameWorldUser.inc"


#include "GameWorld/send_chat_msg.inc"


#include "GameWorld/send_chat_msg_hyper_link.inc"


// ============================================================================
// ProcessAllUsers 模板（ORIG 实例 0863d3bc / 0816270c）
// ============================================================================

// CEventStayTime::RewardGoGoFighter 权威声明已提升至 CEventStayTime.h
// （此前的局部遮蔽视图按 ODR 清理移除）。

#include "GameWorld/operator.inc"


template <typename T>
#include "GameWorld/ProcessAllUsers.inc"
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (!(user->get_state() > 2))
            continue;
        functor(user);
    }
}

template void GameWorld::ProcessAllUsers<CEventStayTime::RewardGoGoFighter>(
    CEventStayTime::RewardGoGoFighter);
template void GameWorld::ProcessAllUsers<InterSelectCreateDnfEventInfo::Reward>(
    InterSelectCreateDnfEventInfo::Reward);
