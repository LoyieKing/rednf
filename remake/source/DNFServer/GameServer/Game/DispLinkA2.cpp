// ============================================================================
// DispLinkA2.cpp —— 链接核销批次 A2（LinkHelper.DispLinkA2 独占 TU）。
//
// 14 个 Dispatcher 类符号（调用方 Dispatchers.cpp，本 TU 不改调用方）：
//   1  Dispatcher_DisJointAvatar::_selectJewelTypeIndex(stAvatarEmblemInfo_t const*) const
//      ORIG T 0x08218598 _ZNK25Dispatcher_DisJointAvatar21_selectJewelTypeIndexEPK20stAvatarEmblemInfo_t
//   2  Dispatcher_EPLPCommand::_BroadCastPacket(CParty*, MSG_EPLP_COMMAND&)
//      ORIG T 0x081fee14 _ZN22Dispatcher_EPLPCommand16_BroadCastPacketEP6CPartyR16MSG_EPLP_COMMAND
//   3  Dispatcher_EPLPCommand::_SendEPLPError(CParty*, unsigned char, unsigned char)
//      ORIG T 0x081fed4c _ZN22Dispatcher_EPLPCommand14_SendEPLPErrorEP6CPartyhh
//   4  Dispatcher_EPLPCommand::excludeEplpState(CUser*)
//      ORIG T 0x0825f760 _ZN22Dispatcher_EPLPCommand16excludeEplpStateEP5CUser
//   5  Dispatcher_EPLPCommand::isReTurnToVillage(CUser*)
//      ORIG T 0x0825f7f4 _ZN22Dispatcher_EPLPCommand17isReTurnToVillageEP5CUser
//   6  Dispatcher_EnchantByBead::check_error(CUser*, MSG_BASE&, ParamBase&)
//      ORIG T 0x081d233a _ZN24Dispatcher_EnchantByBead11check_errorEP5CUserR8MSG_BASER9ParamBase
//   7  Dispatcher_EnterDisjointStore::check_error(...)
//      ORIG T 0x081d00d8 _ZN29Dispatcher_EnterDisjointStore11check_errorEP5CUserR8MSG_BASER9ParamBase
//   8  Dispatcher_GuildCargo::check_error(...)
//      ORIG T 0x081d0a6a _ZN21Dispatcher_GuildCargo11check_errorEP5CUserR8MSG_BASER9ParamBase
//   9  Dispatcher_GuildCargoHistory::check_error(...)
//      ORIG T 0x081d0c3c _ZN28Dispatcher_GuildCargoHistory11check_errorEP5CUserR8MSG_BASER9ParamBase
//   10 Dispatcher_ImageCommunicationEquipmentUse::read(PacketBuf&, MSG_BASE&)
//      ORIG T 0x081e21cc _ZN41Dispatcher_ImageCommunicationEquipmentUse4readER9PacketBufR8MSG_BASE
//      （ORIG 忠实转写：函数体即 ret 0，合法空实现）
//   11 Dispatcher_New_Gmdebug_Command::oldGmRoutine(CUser*, PacketBuf&, std::vector<std::string>&)
//      ORIG T 0x0820bf2c _ZN30Dispatcher_New_Gmdebug_Command12oldGmRoutineEP5CUserR9PacketBufRSt6vectorISsSaISsEE
//   12 Dispatcher_PrecheckSoloTeleport::check_error(...)
//      ORIG T 0x081e1a9e _ZN31Dispatcher_PrecheckSoloTeleport11check_errorEP5CUserR8MSG_BASER9ParamBase
//   13 Dispatcher_RequestDisjointItem::check_error(...)
//      ORIG T 0x081cfc04 _ZN30Dispatcher_RequestDisjointItem11check_errorEP5CUserR8MSG_BASER9ParamBase
//   14 Dispatcher_Request_Seria_Buff::check_error(...)
//      ORIG T 0x081dbcd8 _ZN29Dispatcher_Request_Seria_Buff11check_errorEP5CUserR8MSG_BASER9ParamBase
//
// 证据：docs/class_func_reports/Dispatcher_<类>/<方法>.md（汇编 + 反编译 C 全文）。
// ============================================================================

#include "Dispatchers.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "CParty.h"
#include "PvP_Room.h"
#include "CBattle_Field.h"
#include "GlobalScriptTutorial.h"
#include "CExpertJob.h"
#include "Teleport.h"
#include "ServiceRestrictManager.h"
#include "CEventTable.h"          // CGuildServerProxy.h 经此可见 CEventBase/CEventManager
#include "CGuildServerProxy.h"    // CEventManager::GetRepeatEvent / CServerProxyMgr
#include "CServerProxyMgr.h"

#include <string>
#include <vector>
#include <utility>
#include <set>

// ---- 跨 TU 声明（ORIG 真实符号、尚无 owner 定义；本 TU 只调用不定义） ----

// ORIG T 0x0820ba90 _Z19DoUserDefineCommandP5CUseriPKc（GM 自定义命令解释器，
// 归 GmCmds/GameMaster 域批次）
int DoUserDefineCommand(CUser* user, int cmd, const char* arg);

// ORIG T 0x0820bb68 _Z28SendVillageAttackedGMCommandP5CUseri（村庄攻击 GM 通知，
// 归 Village/GmCmds 域批次）
void SendVillageAttackedGMCommand(CUser* user, int state);

// ORIG T 0x08545d24 _ZN16CItemMakingSkill21SetItemMakingSkillPointEiiP5CUser
// （CItemMakingSkill 类当前无权威头；引用方仅 oldGmRoutine "itemskill"。
//  GetCharacExpandData(user,10) 返回该扩展数据指针。）
class CItemMakingSkill
{
public:
    void SetItemMakingSkillPoint(int a, int b, CUser* user);
};
// ORIG T 0x08644272 _ZNK17TowerOfDespairMgr31IsTowerOfDespairDungeon_ByIndexEi：
// m_layerSet(+0x328, set<int>).find(idx) != end()。
// 已由 TowerOfDespairMgr.cpp（TOD 域 owner）按报告定义真实成员，
// 本 TU 只调用不定义。
#include "TowerOfDespairMgr.h"

// ============================================================================
// 局部辅助
// ============================================================================

// ORIG 自由符号 _Z22getAvatarEmblemTypeIdxt（W 0x0822a98c，定义在本 TU 尾部）
int getAvatarEmblemTypeIdx(unsigned short type);

// MSG_EPLP_COMMAND：ORIG 无独立导出符号，仅经 _BroadCastPacket 以引用访问。
// 报告反编译只读 +0x0d/+0x0e 两个命令字节（Dispatchers.cpp 定义同布局视图；
// 前 0xd 字节为包头区【推断】，不设 static_assert）。
struct MSG_EPLP_COMMAND_A2
{
    char m_pad00[0xd];        // 包头区 +0x00..+0x0c
    unsigned char m_cmd;      // +0x0d：命令字节
    unsigned char m_subCmd;   // +0x0e：子命令字节
};

// GM 消息包（oldGmRoutine 内联展开模式，ORIG 多处重复同一序列）：
// put_header(0,0xc) / put_byte(0) / put_short(0) / put_byte(0) / put_int(len) /
// put_str(msg,len) / finalize(true) / Send。
static void A2_SendGmMessage(CUser* user, const char* msg)
{
    PacketGuard guard;
    guard.put_header(0, 0xc);
    guard.put_byte(0);
    guard.put_short(0);
    guard.put_byte(0);
    int len = static_cast<int>(strlen(msg));
    guard.put_int(len);
    guard.put_str(msg, len);
    guard.finalize(true);
    user->Send(guard);
}

static inline unsigned int A2_dungeonIndexOf(CParty* party)
{
    // CBattle_Field 位于 CParty+0xb24（m_padBattleField1 占位），dungeon index 取自
    // CBattle_Field::get_dungeon_index（ORIG W 0x0822d0d8，定义 CBattle_Field.cpp）。
    return static_cast<unsigned int>(
        reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1)->get_dungeon_index());
}

// ============================================================================
// 1. Dispatcher_DisJointAvatar::_selectJewelTypeIndex
//    报告 docs/class_func_reports/Dispatcher_DisJointAvatar/_selectJewelTypeIndex.md
// ============================================================================
int Dispatcher_DisJointAvatar::_selectJewelTypeIndex(
    stAvatarEmblemInfo_t const* jewelSocketData) const
{
    int typeCounts[5] = {0, 0, 0, 0, 0};   // -0x3c..-0x2c：各纹章类型计数
    int cumul[5]      = {0, 0, 0, 0, 0};   // -0x50..-0x40：累计概率分布
    int total = 0;                         // -0x28
    int totalWeight = 0;                   // -0x24
    int picked;                            // -0x20
    int i;                                 // -0x1c
    int idx;                               // -0x18
    int j;                                 // -0x14
    int weight;                            // -0x10
    int k;                                 // -0xc

    for (i = 0; i < 5; ++i)
    {
        // stAvatarEmblemInfo_t pack(1)：5×{ushort emblemSocketType_@+i*6, int emblemItemIdx_}
        if (jewelSocketData->stEmblemSocket[i].emblemSocketType_ == 0)
            continue;
        if (jewelSocketData->stEmblemSocket[i].emblemSocketType_ == 0xffef)
            continue;
        if (jewelSocketData->stEmblemSocket[i].emblemSocketType_ == 0xffff)
            continue;
        // ORIG：idx = _getJewelTypeIndex(type)（getAvatarEmblemTypeIdx），-1 → continue
        idx = _getJewelTypeIndex(
            jewelSocketData->stEmblemSocket[i].emblemSocketType_);
        if (idx == -1)
            continue;
        typeCounts[idx] = typeCounts[idx] + 1;
        total = total + 1;
    }

    if (total == 0)
        return -1;

    for (j = 0; j < 5; ++j)
    {
        weight = typeCounts[j] * (10000 / total);
        cumul[j] = weight + totalWeight;
        totalWeight = totalWeight + weight;
    }

    picked = get_rand_int(totalWeight);         // ORIG 0x086b1b87 get_rand_int(int)

    for (k = 0; k < 5; ++k)
    {
        if (picked < cumul[k])
            return k;
    }
    return -1;
}

// ORIG 汇编（0x0821857c sz 0x1b）：形参 spill 后 movzwl 直传尾调，不读返回值。
int Dispatcher_DisJointAvatar::_getJewelTypeIndex(unsigned short type) const
{
    return getAvatarEmblemTypeIdx(type);
}

// 自由函数 getAvatarEmblemTypeIdx（ORIG W 0x0822a98c，定义见下）：
// cmpw $0x10; jbe → type>0x10 返 -1；自 4 位向 0 位 sar 扫描首个 (type>>i)==1
// 的位序号，未命中返回 -1。
int getAvatarEmblemTypeIdx(unsigned short type)
{
    if (type > 0x10)
        return -1;
    for (int i = 4; i >= 0; --i)
    {
        if ((type >> i) == 1)
            return i;
    }
    return -1;
}
// ============================================================================
// 2/3. Dispatcher_EPLPCommand::_SendEPLPError / _BroadCastPacket
//     报告 docs/class_func_reports/Dispatcher_EPLPCommand/{_SendEPLPError,_BroadCastPacket}.md
// ============================================================================
void Dispatcher_EPLPCommand::_SendEPLPError(CParty* party, unsigned char subCmd,
                                            unsigned char errCode)
{
    PacketGuard guard;                       // 0x0858dd4c
    guard.put_header(1, 0x4b);
    guard.put_byte(0);                       // 类型 0：错误应答
    guard.put_byte(static_cast<int>(errCode));   // -0x20 参数（先写）
    guard.put_byte(static_cast<int>(subCmd));    // -0x1c 参数（后写）
    guard.finalize(true);
    party->send_to_party(guard);             // 0x0859d14e
}

void Dispatcher_EPLPCommand::_BroadCastPacket(CParty* party, MSG_EPLP_COMMAND& msg)
{
    MSG_EPLP_COMMAND_A2& m = reinterpret_cast<MSG_EPLP_COMMAND_A2&>(msg);
    PacketGuard guard;
    guard.put_header(1, 0x4b);
    guard.put_byte(1);                       // 类型 1：广播
    guard.put_byte(static_cast<int>(m.m_cmd));     // msg+0x0d
    guard.put_byte(static_cast<int>(m.m_subCmd));  // msg+0x0e
    guard.finalize(true);
    party->send_to_party(guard);
}

// ============================================================================
// 4. Dispatcher_EPLPCommand::excludeEplpState
//    报告 docs/class_func_reports/Dispatcher_EPLPCommand/excludeEplpState.md
// ============================================================================
bool Dispatcher_EPLPCommand::excludeEplpState(CUser* user)
{
    CParty* party = static_cast<CParty*>(user->GetParty());        // 0x0865514c
    if (party == 0)
        return false;

        // ORIG: TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex(
    //           GlobalData::s_pTowerOfDespairMgr, bf(party).get_dungeon_index())
    // （TowerOfDespairMgr.cpp 按报告定义的 const 成员，0x08644272）
    if (static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr)
            ->IsTowerOfDespairDungeon_ByIndex(
                static_cast<int>(reinterpret_cast<CBattle_Field*>(
                    &party->m_padBattleField1)->get_dungeon_index())))
        return false;

    if (user->getMoveSpace() == (UserSpace::T)1)   // 0x08690eca
        return false;

    // ORIG: GlobalScriptTutorial::IsGlobalTutorialDungeon(idx)（0x08ab976a，静态成员）
    if (GlobalScriptTutorial::IsGlobalTutorialDungeon(
            reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1)
                ->get_dungeon_index()))
        return false;
    return true;
}

// ============================================================================
// 5. Dispatcher_EPLPCommand::isReTurnToVillage
//    报告 docs/class_func_reports/Dispatcher_EPLPCommand/isReTurnToVillage.md
// ============================================================================
bool Dispatcher_EPLPCommand::isReTurnToVillage(CUser* user)
{
    if (user == 0)
        return false;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
        return false;
    if (user->getMoveSpace() == (UserSpace::T)1)
        return true;
    if (GlobalScriptTutorial::IsGlobalTutorialDungeon(A2_dungeonIndexOf(party)))
        return true;
    return false;
}

// ============================================================================
// 6. Dispatcher_EnchantByBead::check_error
//    报告 docs/class_func_reports/Dispatcher_EnchantByBead/check_error.md
// ============================================================================
static inline short A2_paramShort(ParamBase& param, int off)
{
    return *reinterpret_cast<short*>(reinterpret_cast<char*>(&param) + off);
}

int Dispatcher_EnchantByBead::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() <= 2)
        return 0x7fffffff;

    // CSecu_ProtectionField::Check(CUser*, SECURITY_PROTCTION)（ORIG 0x08288a02）
    int err = GlobalData::s_pSecuProtectionField->Check(
        user, static_cast<SECURITY_PROTCTION>(0x2b));
    if (err != 0)
        return err;

    short slot1 = A2_paramShort(param, 0xe);
    CInventory* inven = user->getCurCharacInvenW();     // 0x080da28e
    Inven_Item* item1 = inven->GetInvenRef(1, slot1);   // 0x084fc2ae
    if (item1 == 0)
        return 0x11;

    short slot2 = A2_paramShort(param, 0x12);
    Inven_Item* item2 = inven->GetInvenRef(1, slot2);
    if (item2 == 0)
        return 0x11;

    CDataManager* dm = G_CDataManager();
    CItem* pItem = dm->find_item(item1->m_addInfo);     // Inven_Item+0x02
    if (pItem == 0)
        return 0x11;

    // CItem 派生视图 +0x3d8 的可装配珠子类型集合（CStackableItem::m_set3d8）。
    std::set<int>& acceptSet = static_cast<CStackableItem*>(pItem)->m_set3d8;
    if (!acceptSet.empty())
    {
        if (acceptSet.find(item2->m_addInfo) == acceptSet.end())
            return 0x13;
    }
    return 0;
}

// ============================================================================
// 7. Dispatcher_EnterDisjointStore::check_error
//    报告 docs/class_func_reports/Dispatcher_EnterDisjointStore/check_error.md
// ============================================================================
int Dispatcher_EnterDisjointStore::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return -1;

    unsigned short uid = *reinterpret_cast<unsigned short*>(
        reinterpret_cast<char*>(&param) + 0xe);
    CUser* target = G_GameWorld()->find_from_world(uid);   // 0x086c4b9c
    if (target == 0)
        return 0x15;

    if (target->GetCurCharacExpertJobType() != 3)          // 0x0822f894
        return 0x13;
    expert_job::CExpertJob* job = target->GetCurCharacExpertJob();  // 0x0822f8d4
    if (job == 0)
        return 0x13;
    if (job->GetType() != 3)                               // 0x08234796
        return 0x13;
    return 0;
}

// ============================================================================
// 8. Dispatcher_GuildCargo::check_error
//    报告 docs/class_func_reports/Dispatcher_GuildCargo/check_error.md
// ============================================================================
int Dispatcher_GuildCargo::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() != 3 || user->getCurCharacR() != 0)
        return -1;

    if (user->get_charac_guildkey() == 0)                  // 0x0822f46c
        return 100;
    if (user->IsExistGuildAgit())                          // 0x082301d8
        return 0;
    return 0xb5;
}

// ============================================================================
// 9. Dispatcher_GuildCargoHistory::check_error
//    报告 docs/class_func_reports/Dispatcher_GuildCargoHistory/check_error.md
// ============================================================================
int Dispatcher_GuildCargoHistory::check_error(CUser* user, MSG_BASE& msg,
                                              ParamBase& param)
{
    if (user->get_state() != 3 || user->getCurCharacR() != 0)
        return -1;

    if (user->get_charac_guildkey() == 0)
        return 100;
    if (user->IsExistGuildAgit())
        return 0;
    return 0xb5;
}

// ============================================================================
// 10. Dispatcher_ImageCommunicationEquipmentUse::read
//     报告 docs/class_func_reports/Dispatcher_ImageCommunicationEquipmentUse/read.md
//     ORIG 忠实转写：函数体为 ret 0（合法空实现，非桩）。
// ============================================================================
int Dispatcher_ImageCommunicationEquipmentUse::read(PacketBuf& packet, MSG_BASE& msg)
{
    (void)packet;
    (void)msg;
    return 0;
}

// ============================================================================
// 12. Dispatcher_PrecheckSoloTeleport::check_error
//     报告 docs/class_func_reports/Dispatcher_PrecheckSoloTeleport/check_error.md
// ============================================================================
int Dispatcher_PrecheckSoloTeleport::check_error(CUser* user, MSG_BASE& msg,
                                                 ParamBase& param)
{
    if (user == 0)
        return -1;
    int result = 0;                       // ORIG：先 0 初始化再赋 check_rule 结果（栈槽 -0xc）
    result = Teleport::check_rule(user);
    return result;
}

// ============================================================================
// 13. Dispatcher_RequestDisjointItem::check_error
//     报告 docs/class_func_reports/Dispatcher_RequestDisjointItem/check_error.md
// ============================================================================
int Dispatcher_RequestDisjointItem::check_error(CUser* user, MSG_BASE& msg,
                                                ParamBase& param)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return -1;

    ServiceRestrictManager* mgr =
        ARAD::Singleton<ServiceRestrictManager>::Get();    // 0x081625e6
    if (mgr->isRestricted(user, RestrictCategory::Enum(1), 0x13))
        return 0xd1;

    unsigned short uid = *reinterpret_cast<unsigned short*>(
        reinterpret_cast<char*>(&param) + 0xe);
    CUser* target = G_GameWorld()->find_from_world(uid);
    if (target == 0)
        return 3;
    return 0;
}

// ============================================================================
// 14. Dispatcher_Request_Seria_Buff::check_error
//     报告 docs/class_func_reports/Dispatcher_Request_Seria_Buff/check_error.md
// ============================================================================
int Dispatcher_Request_Seria_Buff::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if (user->getCurCharacR() == 0)
        return -1;

    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x5d);  // 0x08115998
    if (event->IsEventing(0))                        // vptr+0x34（slot 13）
    {
        if (!G_CDataManager()->m_seriaBlessing.is_possible_lv(user->get_charac_level()))
            return -1;
    }
    else
    {
        if (G_CDataManager()->m_growthPower.get_idx_break_away_section(
                user->get_charac_level()) < 0)
            return -1;
    }
    return 0;
}

// ============================================================================
// 11. Dispatcher_New_Gmdebug_Command::oldGmRoutine
//     报告 docs/class_func_reports/Dispatcher_New_Gmdebug_Command/oldGmRoutine.md
//     （ORIG 0x0820bf2c..0x082134a7，约 0x757b 字节；旧式 GM 调试命令解释器，
//      空格分隔命令字 → 分支分发。Dispatchers.cpp:11927 调用。）
// ============================================================================

#include "GameMasterCmd.h"              // DisPatcher_DebugCommand 前向域依赖
#include "DisPatcherCore.h"             // DisPatcher_DebugCommand / MSG_DEBUG_COMMAND
#include "CServerProxyMgr.h"            // CServerProxyMgr<T>::GetServerProxy
#include "CFairPvPScore.h"              // fair_pvp::CFairPvPScore
#include "CUserProc.h"                  // APSystem::CUserProc::GM_Set/GM_Reset
#include "CDailyScheduleManager.h"      // GM_Open*/GM_Close*
#include "SkillSlot.h"                  // CSkillChanger / SkillSlot 访问
#include "CRandomOptionItemHandleTest.h"
#include "CCirculationStatistic.h"
#include "RefPvpGrade.h"
#include "AvatarCoin.h"
#include "AvatarRechargeServer.h"       // GmReduceEndurance / GmChangeEnduranceReduceRate
#include "CMailBoxHelper.h"             // WongWork::CMailBoxHelper MultiMail
#include "StatisticsGmCmdPacket.h"
#include "Packet_CollectItemsGm.h"
#include "Packet_Game_Monitor_GM_Village_Attacked.h"
#include "DB_UpdateInventoryCapacity.h"
#include "BreakAwayPreventSystem.h"

#include "Arad_DataManager.h"           // ARAD::Singleton / findGameScript
#include "Arad_EventPeriodDataManager.h"
#include "Arad_ServerStateManager.h"    // deleteState / insertState
#include "AradServerStateMessage.h"
#include "Arad_InternalDispatcher.h"    // make_internal_stream_jpn
#include "MoreTypes.h"                  // arad 域 Sig POD（SigSetCharacCreateTime）
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "TimerQueue.h"
#include "PacketDispatcher.h"
#include "CTimeGate.h"
#include "DataManagerInitDepends.h"   // TOD_UserState（wjfakd）
#include "CStatisticServerProxy.h"    // 完整类型（CServerProxyMgr<T> 实例化）
#include "CMonitorServerProxy.h"      // 完整类型 / SendTcpPacket
// ORIG _Z14StringToNumberPKc（T 0x081090fa，定义 DNFFunctionLib.cpp）
int StringToNumber(const char* str);

// ORIG _ZN4ARAD10uint32TimeEPs（T 0x081979c0，返回 long long；ARAD 域 TU 定义）
namespace ARAD
{
long long uint32Time(short* fields);
}

#include "BattleData.h"                // hellvalue（SetHellPartyValueTotal）
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

// CLocalChina_DB_LastDay_BreakAway（ORIG T 0x08128152）：CLocalChina 域类，
// 当前无权威头；最小成员声明（不产生定义，链接归 CLocalChina 域批次）。
class CLocalChina_DB_LastDay_BreakAway
{
public:
    // ORIG 0x08128152 静态（调用点只压 3 个参数，无 this；
    // 见 Dispatcher_New_Gmdebug_Command/oldGmRoutine.md +0x7414..+0x7428）。
    static void makeRequest(unsigned int accId, int day, int flag);
};

// AradServerStateMessage::AradServerStateMessage（ORIG W 0x08186652）已由
// AradServerStateMessage.h 声明（定义归 ARAD 域 TU）。


// GM 文本消息包（ORIG show lp / max lp / ulti max / dimen max 内联展开序列）：
// put_header(0,0xc) / put_byte(0) / put_short(0) / put_byte(0) / put_int(len) /
// put_str(text,len) / finalize(true) / Send。
static void A2_SendGmText(CUser* user, const char* text)
{
    PacketGuard g;
    g.put_header(0, 0xc);
    g.put_byte(0);
    g.put_short(0);
    g.put_byte(0);
    int len = static_cast<int>(std::strlen(text));
    g.put_int(len);
    g.put_str(text, len);
    g.finalize(true);
    user->Send(g);
}

// GM 消息缓冲（ORIG local_66c，栈上 0x100 字节，先清零再 sprintf）。
struct A2_GmBuf
{
    char buf[0x100];
    A2_GmBuf() { std::memset(buf, 0, sizeof(buf)); }
    operator char*() { return buf; }
};

int Dispatcher_New_Gmdebug_Command::oldGmRoutine(CUser* user, PacketBuf& packet,
                                                 std::vector<std::string>& args)
{
    (void)packet;                        // ORIG 仅持引用，函数体不消费
    DisPatcher_DebugCommand dbgCmd;      // local_254（ORIG 栈构造）
    PacketGuard guard;                   // local_260（函数级持有，末尾析构）
    unsigned int ret = 0;

    std::string cmd = args.size() > 0 ? args[0] : std::string();
    std::string arg1 = args.size() > 1 ? args[1] : std::string();
    std::string arg2 = args.size() > 2 ? args[2] : std::string();
    std::string arg3 = args.size() > 3 ? args[3] : std::string();

    // ---- lawless：SetCurCharacChaosPoint（0 < v < 10000）----
    if (cmd == "lawless")
    {
        int v = std::atoi(arg1.c_str());
        if (0 < v && v < 10000)
        {
            user->SetCurCharacChaosPoint(v);
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(100);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
    }
    // ---- bal：help abuse ratio（0 < v < 10000，×100）----
    else if (cmd == "bal")
    {
        int v = std::atoi(arg1.c_str());
        if (0 < v && v < 10000)
        {
            user->setCurCharacHelpAbuseComputedRatio(v * 100);
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(0x65);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
    }
    // ---- questclear2 / q：GM 任务旗标开关/执行 ----
    else if (cmd == "questclear2" || cmd == "q")
    {
        if (arg1 != "on" && arg1 != "off")
        {
            int v = std::atoi(arg1.c_str());
            if (0 < v && v < 100000)
            {
                user->setGmQuestFlag(true);
                user->quest_action(0x23, v, 0, 0);
                guard.put_header(1, 0xb3);
                guard.put_byte(1);
                guard.put_byte(0x66);
                guard.finalize(true);
                user->Send(guard);
                ret = 0;
                return 0;
            }
        }
    }
    // ---- guild exp ----
    else if (cmd == "guild")
    {
        if (arg1 == "exp")
        {
            int v = std::atoi(arg2.c_str());
            if (0 < v && v < 10000)
            {
                user->set_guild_exp(v);
                user->m_field796d9 = 1;                  // +0x796d9
                user->WorkPerFiveMin();
                guard.put_header(1, 0xb3);
                guard.put_byte(1);
                guard.put_byte(0x67);
                guard.finalize(true);
                user->Send(guard);
                ret = 0;
                return 0;
            }
        }
    }
    // ---- troublemaker on/off ----
    else if (cmd == "troublemaker")
    {
        if (arg1 == "on" || arg1 == "off")
        {
            user->setCurrCharacBlackCount(arg1 == "on" ? 100 : 0);
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(0x68);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
    }
    // ---- fatigue set / fatigue <v> ----
    else if (cmd == "fatigue")
    {
        if (arg1 == "set")
        {
            int v = std::atoi(arg2.c_str());
            if (-1 < v && v < 10000)
            {
                unsigned int maxFatigue = user->getCurCharacMaxFatigue();
                user->setCurCharacFatigue(static_cast<int>(maxFatigue & 0xffff) - v);
                user->SendFatigue();
                guard.put_header(1, 0xb3);
                guard.put_byte(1);
                guard.put_byte(0x69);
                guard.finalize(true);
                user->Send(guard);
                ret = 0;
                return 0;
            }
        }
        else
        {
            int v = std::atoi(arg1.c_str());
            if (-1 < v && v < 10000 && 0 < v)
            {
                user->FatigueUp(v);
                user->SendFatigue();
            }
        }
    }
    // ---- show lp：查询幸运点 ----
    else if (cmd == "show" && arg1 == "lp")
    {
        int luck = user->GetCurCharacLuckPoint();
        A2_GmBuf b;
        std::sprintf(b.buf, "(%d) ", luck);
        {
            std::string text = "remained luck point : ";
            text += b.buf;
            A2_SendGmText(user, text.c_str());
        }
        return 0;
    }
    // ---- max lp：+100000 幸运点 ----
    else if (cmd == "max" && arg1 == "lp")
    {
        user->IncCurCharacLuckPoint(100000);
        user->enableSaveCharacStat();
        {
            A2_SendGmText(user, "100000 luck point!!");
        }
        return 0;
    }
    // ---- item：GM 发物 ----
    else if (cmd == "item")
    {
        unsigned long v = static_cast<unsigned long>(std::atoi(arg1.c_str()));
        dbgCmd._debugCommandAddItem(user, v, 100);
        return 0;
    }
    // ---- show / hide（顶级可见性）----
    else if (cmd == "show")
    {
        MSG_DEBUG_COMMAND dbg;
        dbgCmd._debugCommandVisible(user, dbg);
        return 0;
    }
    else if (cmd == "hide")
    {
        MSG_DEBUG_COMMAND dbg;
        dbgCmd._debugCommandInvisible(user, dbg);
        return 0;
    }
    // ---- dirty ----
    else if (cmd == "dirty")
    {
        DoUserDefineCommand(user, 0x1f, arg1.c_str());
    }
    // ---- avatar：GM 发头像 ----
    else if (cmd == "avatar")
    {
        MSG_DEBUG_COMMAND dbg;
        dbg.m_arg = std::atoi(arg1.c_str());
        dbg.payload.avatar.periodKind = static_cast<unsigned char>(std::atoi(arg2.c_str()));
        dbg.payload.avatar.slotByte = static_cast<signed char>(std::atoi(arg3.c_str()));
        dbgCmd._debugCommandAddAvatar(user, dbg);
        return 0;
    }
    // ---- combo：无操作（ORIG 匹配后无动作）----
    else if (cmd == "combo")
    {
        // ORIG 空分支
    }
    // ---- coin：扣/清金币 ----
    else if (cmd == "coin")
    {
        int v = std::atoi(arg1.c_str());
        unsigned int coin = user->getCurCharacInvenR()->GetCoin();
        unsigned int left = coin - v;
        if (static_cast<int>(left) < 0)
            left = 0;
        user->getCurCharacInvenW()->SetCoin(left);
        if (v == 0)
        {
            user->getCurCharacInvenW()->SetCoin(0);
            user->getCurCharacInvenW()->SetPayCoin(0);
            user->getCurCharacInvenW()->SetEventCoin(0);
        }
        user->SendCoin();
    }
    // ---- use item：重复 increase_status ----
    else if (cmd == "use")
    {
        if (arg1 == "item")
        {
            int kind = std::atoi(arg2.c_str());
            int count = std::atoi(arg3.c_str());
            if (kind < 1 || 6 < kind)
            {
                ret = 0;
                return 0;
            }
            kind = kind + 2;
            for (int i = 0; i < count; ++i)
                user->increase_status(static_cast<short>(kind));
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, kind);
        }
    }
    // ---- npcfav / favterm / pickup / refund / durable / bluemarble：双参透传 ----
    else if (cmd == "npcfav")
    {
        A2_GmBuf b;
        std::sprintf(b.buf, "%s %s", arg1.c_str(), arg2.c_str());
        DoUserDefineCommand(user, 0x24, b.buf);
    }
    else if (cmd == "favterm")
    {
        A2_GmBuf b;
        std::sprintf(b.buf, "%s %s", arg1.c_str(), arg2.c_str());
        DoUserDefineCommand(user, 0x23, b.buf);
    }
    // ---- sumhell / sdale ----
    else if (cmd == "sumhell")
    {
        DoUserDefineCommand(user, 0x15, arg1.c_str());
    }
    else if (cmd == "sdale")
    {
        DoUserDefineCommand(user, 0x1c, arg1.c_str());
    }
    // ---- mdt / monsterdroptest / startmdt / endmdt / goldmdt ----
    else if (cmd == "mdt" || cmd == "monsterdroptest" || cmd == "startmdt" ||
             cmd == "endmdt" || cmd == "goldmdt")
    {
        A2_GmBuf b;
        if (cmd == "startmdt")
            std::sprintf(b.buf, "%d %s", 1, arg1.c_str());
        else if (cmd == "endmdt")
            std::sprintf(b.buf, "%d %s", 2, arg1.c_str());
        else if (cmd == "goldmdt")
            std::sprintf(b.buf, "%d %s", 3, arg1.c_str());
        else
            std::sprintf(b.buf, "%d %s", 0, arg1.c_str());
        DoUserDefineCommand(user, 0x4b, b.buf);
    }
    // ---- cli / checklottoitem ----
    else if (cmd == "cli" || cmd == "checklottoitem")
    {
        A2_GmBuf b;
        if (arg1 == "ven")
        {
            std::sprintf(b.buf, "%d %s", 1, arg2.c_str());
            DoUserDefineCommand(user, 0x4d, b.buf);
        }
        else
        {
            std::sprintf(b.buf, "%s %s", arg1.c_str(), arg2.c_str());
            DoUserDefineCommand(user, 0x4c, b.buf);
        }
    }
    // ---- cbind / checkbind ----
    else if (cmd == "cbind" || cmd == "checkbind")
    {
        DoUserDefineCommand(user, 0x4e, arg1.c_str());
    }
    // ---- oneadayitem / craneDay / cinven ----
    else if (cmd == "oneadayitem")
    {
        DoUserDefineCommand(user, 0x52, arg1.c_str());
    }
    else if (cmd == "craneDay")
    {
        DoUserDefineCommand(user, 0x79, arg1.c_str());
    }
    else if (cmd == "cinven")
    {
        DoUserDefineCommand(user, 0x46, arg1.c_str());
    }
    // ---- pickup / refund ----
    else if (cmd == "pickup")
    {
        A2_GmBuf b;
        std::sprintf(b.buf, "%s %s", arg1.c_str(), arg2.c_str());
        DoUserDefineCommand(user, 0x47, b.buf);
    }
    else if (cmd == "refund")
    {
        A2_GmBuf b;
        std::sprintf(b.buf, "%s %s", arg1.c_str(), arg2.c_str());
        DoUserDefineCommand(user, 0x48, b.buf);
    }
    // ---- tb reset/adjust/no ----
    else if (cmd == "tb")
    {
        if (arg1 == "reset")
            DoUserDefineCommand(user, 0x5d, arg1.c_str());
        else if (arg1 == "adjust")
            DoUserDefineCommand(user, 0x5e, arg1.c_str());
        else if (arg1 == "no")
            DoUserDefineCommand(user, 0x5f, arg1.c_str());
    }
    // ---- achieve/ac reset/report/clear/all/no ----
    else if (cmd == "achieve" || cmd == "ac")
    {
        if (arg1 == "reset")
            DoUserDefineCommand(user, 0x60, arg1.c_str());
        else if (arg1 == "report")
            DoUserDefineCommand(user, 0x61, arg1.c_str());
        else if (arg1 == "clear")
            DoUserDefineCommand(user, 0x62, arg2.c_str());
        else if (arg1 == "all")
            DoUserDefineCommand(user, 99, arg1.c_str());
        else if (arg1 == "no")
            DoUserDefineCommand(user, 100, arg1.c_str());
    }
    // ---- power userpoint/guildpoint/win ----
    else if (cmd == "power")
    {
        if (arg1 == "userpoint")
        {
            MSG_DEBUG_COMMAND dbg;
            dbg.m_arg = std::atoi(arg2.c_str());
            dbgCmd._debugCommandUserPowerwarPointUp(user, dbg);
            return 0;
        }
        else if (arg1 == "guildpoint")
        {
            int v = std::atoi(arg2.c_str());
            unsigned int characNo = user->getCurCharacNo();
            unsigned int guildKey = user->get_charac_guildkey();
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
            proxy->SendIncreaseDecreasePowerWarPoint(guildKey, characNo, v);
        }
        else if (arg1 == "win")
        {
            DoUserDefineCommand(user, 0x2b, arg2.c_str());
        }
    }
    // ---- townatk success/fail/end/其它 ----
    else if (cmd == "townatk")
    {
        if (arg1 == "success")
            SendVillageAttackedGMCommand(user, 1);
        else if (arg1 == "fail")
            SendVillageAttackedGMCommand(user, 2);
        else if (arg1 == "end")
            SendVillageAttackedGMCommand(user, 3);
        else
            DoUserDefineCommand(user, 0x1e, arg1.c_str());
    }
    // ---- rec start/stop/run ----
    else if (cmd == "rec")
    {
        if (arg1 == "start")
            DoUserDefineCommand(user, 0x12, arg1.c_str());
        else if (arg1 == "stop")
            DoUserDefineCommand(user, 0x13, arg1.c_str());
        else if (arg1 == "run")
            DoUserDefineCommand(user, 0x14, arg1.c_str());
    }
    // ---- spd / durable ----
    else if (cmd == "spd")
    {
        DoUserDefineCommand(user, 9, arg1.c_str());
    }
    else if (cmd == "durable")
    {
        A2_GmBuf b;
        std::sprintf(b.buf, "%s %s", arg1.c_str(), arg2.c_str());
        DoUserDefineCommand(user, 0x29, b.buf);
    }
    // ---- sep upgrade/up success|s|fail|f ----
    else if (cmd == "sep" && (arg1 == "upgrade" || arg1 == "up"))
    {
        A2_GmBuf b;
        if (arg2 == "success" || arg2 == "s")
        {
            if (user->getUpgradeSeparateTrueFlag() == 0)
            {
                char ok[4];
                ok[0] = 'o'; ok[1] = 'k'; ok[2] = '\0'; ok[3] = '\0';
                user->setUpgradeSeparateTrueFlag(1);
                user->setUpgradeSeparateFalseFlag(0);
                std::sprintf(b.buf, "sep upgrade success - %s", ok);
            }
            else
            {
                char end[5];
                end[0] = 'e'; end[1] = 'n'; end[2] = 'd'; end[3] = '\0'; end[4] = '\0';
                user->setUpgradeSeparateTrueFlag(0);
                std::sprintf(b.buf, "sep upgrade success - %s", end);
            }
        }
        else if (arg2 == "fail" || arg2 == "f")
        {
            if (user->getUpgradeSeparateFlaseFlag() == 0)
            {
                char ok[4];
                ok[0] = 'o'; ok[1] = 'k'; ok[2] = '\0'; ok[3] = '\0';
                user->setUpgradeSeparateTrueFlag(0);
                user->setUpgradeSeparateFalseFlag(1);
                std::sprintf(b.buf, "sep upgrade fail - %s", ok);
            }
            else
            {
                char end[5];
                end[0] = 'e'; end[1] = 'n'; end[2] = 'd'; end[3] = '\0'; end[4] = '\0';
                user->setUpgradeSeparateFalseFlag(0);
                std::sprintf(b.buf, "sep upgrade fail - %s", end);
            }
        }
        {
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            int len = static_cast<int>(std::strlen(b.buf));
            g.put_int(len);
            g.put_str(b.buf, len);
            g.finalize(true);
            user->Send(g);
        }
        return 0;
    }
    // ---- initstat / guilddebug / delguild / changename ----
    else if (cmd == "initstat")
    {
        DoUserDefineCommand(user, 0x3f, arg1.c_str());
    }
    else if (cmd == "guilddebug")
    {
        DoUserDefineCommand(user, 0x42, arg1.c_str());
    }
    else if (cmd == "delguild")
    {
        DoUserDefineCommand(user, 0x2c, arg1.c_str());
    }
    else if (cmd == "changename")
    {
        DoUserDefineCommand(user, 0x2d, arg1.c_str());
    }
    // ---- startevent（三参）/ endevent ----
    else if (cmd == "startevent")
    {
        A2_GmBuf b;
        std::sprintf(b.buf, "%s %s %s", arg1.c_str(), arg2.c_str(), arg3.c_str());
        DoUserDefineCommand(user, 0x2e, b.buf);
    }
    else if (cmd == "endevent")
    {
        DoUserDefineCommand(user, 0x2f, arg1.c_str());
    }
    // ---- shopcheck / shopnpc ----
    else if (cmd == "shopcheck")
    {
        DoUserDefineCommand(user, 0x30, arg1.c_str());
    }
    else if (cmd == "shopnpc")
    {
        DoUserDefineCommand(user, 0x31, arg1.c_str());
    }
    // ---- cargo delete ----
    else if (cmd == "cargo")
    {
        if (arg1 == "delete")
            DoUserDefineCommand(user, 0x36, arg1.c_str());
    }
    // ---- guildfund / dropitem ----
    else if (cmd == "guildfund")
    {
        DoUserDefineCommand(user, 0x41, arg1.c_str());
    }
    else if (cmd == "dropitem")
    {
        DoUserDefineCommand(user, 0x34, arg1.c_str());
    }
    // ---- premiumdelete/pd / mailitem ----
    else if (cmd == "premiumdelete" || cmd == "pd")
    {
        DoUserDefineCommand(user, 0x3c, arg1.c_str());
    }
    else if (cmd == "mailitem")
    {
        DoUserDefineCommand(user, 0x3d, arg1.c_str());
    }
    // ---- ci true/false ----
    else if (cmd == "ci")
    {
        if (arg1 == "true")
            DoUserDefineCommand(user, 0x6d, arg2.c_str());
        else if (arg1 == "false")
            DoUserDefineCommand(user, 0x6d, arg1.c_str());
    }
    // ---- ritemclear / difficulty ----
    else if (cmd == "ritemclear")
    {
        DoUserDefineCommand(user, 0x76, arg1.c_str());
    }
    else if (cmd == "difficulty")
    {
        DoUserDefineCommand(user, 0x78, arg1.c_str());
    }
    // ---- bluemarble ----
    else if (cmd == "bluemarble")
    {
        A2_GmBuf b;
        std::sprintf(b.buf, "%s %s", arg1.c_str(), arg2.c_str());
        DoUserDefineCommand(user, 0x7d, b.buf);
    }
    // ---- ulti max ----
    else if (cmd == "ulti" && arg1 == "max")
    {
        int v = G_CDataManager()->get_limit_inout_count((ENUM_BLOOD_TYPE)2);
        user->setUltimateInoutValue(2, v);
        {
            A2_SendGmText(user, "Add ultimate inout max count!!");
        }
    }
    // ---- dimen max ----
    else if (cmd == "dimen" && arg1 == "max")
    {
        for (int i = 0; i < 6; ++i)
        {
            int v = G_CDataManager()->get_dimensionInout(i);
            user->setDemensionInoutValue(static_cast<char>(i), v);
        }
        {
            A2_SendGmText(user, "Add dimension inout max count!!");
        }
    }
    // ---- dontsend on/off ----
    else if (cmd == "dontsend")
    {
        if (arg1 == "on")
            user->SetDebugCommand(1);
        else if (arg1 == "off")
            user->SetDebugCommand(0);
    }
    // ---- pvpskilllevel ----
    else if (cmd == "pvpskilllevel")
    {
        int v = std::atoi(arg1.c_str());
        if (0x25 < v)
        {
            return 0;
        }
        int nextPoint = GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(v);
        PvpResultType* pvpW = user->getPVPResultW();
        pvpW->m_expPoint = nextPoint;                    // +0x08
        pvpW->m_pvpGrade = v;                            // +0x14
        G_GameWorld()->send_AllBasicInfo(user);
        user->givePvPSkillTree(0, false, 3);
        user->send_skill_info();
    }
    // ---- lethe：清技能 ----
    else if (cmd == "lethe")
    {
        SkillSlot* skill = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
        skill->clear_all_skills_both();
        skill->set_parent(user);
        {
            WongWork::CSkillChanger changer;
            changer.SkillInitialize(user, 0, false);
            user->givePvPSkillTree(0, false, 3);
            user->send_skill_info();
        }
    }
    // ---- rematch ----
    else if (cmd == "rematch")
    {
        PvP_Room* pvpRoom = static_cast<PvP_Room*>(user->GetPVPRoom());
        if (pvpRoom != 0)
        {
            void* matchObj = reinterpret_cast<void*>(pvpRoom->m_matching);  // +0x6e4 IMatch* 槽
            if (matchObj != 0)
            {
                typedef int (*RematchFn)(void*, CUser*, int);
                RematchFn fn = *reinterpret_cast<RematchFn*>(
                    *reinterpret_cast<void**>(matchObj) + 0x68);   // vptr+0x34 域
                fn(matchObj, user, std::atoi(arg1.c_str()));
            }
        }
    }
    // ---- wjfakd / wjfakd2 ----
    else if (cmd == "wjfakd")
    {
        TOD_Layer layer(static_cast<unsigned short>(std::atoi(arg1.c_str())));
        TOD_UserState* state =
            reinterpret_cast<TOD_UserState*>(user->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
        state->setEnterLayer(layer);
        TOD_Layer today = state->getTodayEnterLayer();
        A2_GmBuf b;
        std::sprintf(b.buf, "set Enter Layer : %d",
                     static_cast<int>(today.GetLayer()) & 0xffff);
        TowerOfDespairMgr::SendMessageToCUser(user, b.buf);
    }
    else if (cmd == "wjfakd2")
    {
        if (arg1 == "reload")
        {
            Stream* s = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xb719);
            CStreamGuard g(s, true);
            g << 0x277;
            g << -1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), g);
        }
        else if (arg1 == "randomselect")
        {
            Stream* s = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xb720);
            CStreamGuard g(s, true);
            g << 0x275;
            g << -1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), g);
        }
        // layerinfo：ORIG 空分支（比较结果未用）
    }
    // ---- mission 系列 ----
    else if (cmd == "mission")
    {
        int v2 = std::atoi(arg2.c_str());
        int v1 = std::atoi(arg1.c_str());
        user->SetMissionParameter(v1, v2);
        user->send_MissionList();
    }
    else if (cmd == "newdailymission")
    {
        user->ResetDailyMission();
        user->send_MissionList();
    }
    else if (cmd == "clearmission")
    {
        user->ClearMission(static_cast<short>(std::atoi(arg1.c_str())));
        user->send_MissionList();
        user->send_pvp_record();
    }
    else if (cmd == "clearmissionkind")
    {
        user->ClearMissionKind(static_cast<short>(std::atoi(arg1.c_str())));
        user->send_MissionList();
        user->send_pvp_record();
    }
    else if (cmd == "resetmissionlist")
    {
        user->ResetMissionList();
        user->send_MissionList();
    }
    // ---- resetdaily ----
    else if (cmd == "resetdaily")
    {
        G_GameWorld()->ResetAllFatigue();
        G_GameWorld()->ReselectDailyQuest();
        G_GameWorld()->AddDailyItem();
    }
    // ---- combomission ----
    else if (cmd == "combomission")
    {
        short v = static_cast<short>(std::atoi(arg1.c_str()));
        short kind = user->getCurCharacGrowType() == 0 ? 0x1f : 0x20;
        user->Update_ComboMission(kind, v);
    }
    // ---- roiregen：ORIG 空分支（args[0] 匹配后直接 return 0）----
    else if (cmd == "roiregen")
    {
        // ORIG 空分支
    }
    // ---- qp ----
    else if (cmd == "qp")
    {
        user->setQuestPoint(std::atoi(arg1.c_str()));
    }
    // ---- registauction / burningstage ----
    else if (cmd == "registauction")
    {
        random_option::CRandomOptionItemHandleTest test;
        test.test_regist_auction(user);
    }
    else if (cmd == "burningstage")
    {
        int v = std::atoi(arg1.c_str());
        fair_pvp::CFairPvPScore* score =
            reinterpret_cast<fair_pvp::CFairPvPScore*>(user->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)));
        score->SetRemainBonusCount(v);
        int daily = static_cast<int>(
            G_CDataManager()->m_serverParameter.m_schoolMatchDailyPlayLimit) - v;
        score->SetDailyPlayCount(daily);
    }
    // ---- randitem ----
    else if (cmd == "randitem")
    {
        if (arg1 == "")
        {
            return 0;
        }
        int itemIdx = std::atoi(arg1.c_str());
        Inven_Item item;                       // local_4d5（0x3d 字节）
        item.m_addInfo = itemIdx;              // +0x02
        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            return 0;
        }
        pItem->make_item(item);                // vtbl+0x08 槽 make_item
        // +0x29 起 9 字节 3 槽选项区（ChronicleRandomOptionInfo::m_random(RandomOption)
        // 的 m_field3.m_field1 起连续区）
        unsigned char* raw = &item.m_random.m_random.m_field3.m_field1;
        int idx = 2;
        for (int i = 0; i < 3; ++i)
        {
            if (args.size() <= static_cast<unsigned int>(idx) ||
                args[idx] == "")
                break;
            if (args.size() <= static_cast<unsigned int>(idx + 1) ||
                args[idx + 1] == "")
                break;
            // ORIG 0x82100d0/0x821011d/0x8210166：三次 atoi 写 +1/+2/+3
            raw[0x29 + i * 3 + 0] = static_cast<unsigned char>(std::atoi(args[idx].c_str()));
            raw[0x29 + i * 3 + 1] = static_cast<unsigned char>(std::atoi(args[idx + 1].c_str()));
            raw[0x29 + i * 3 + 2] = static_cast<unsigned char>(std::atoi(args[idx + 1].c_str()));
            idx += 2;
        }
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)6, true, true);
        if (slot >= 0)
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }
    // ---- insertitem（拼串 find_item → AddItem）----
    else if (cmd == "insertitem")
    {
        std::string name;
        for (int i = 1; i < 10; ++i)
        {
            if (args.size() <= static_cast<unsigned int>(i))
                break;
            if (args[i].length() == 0)
                break;
            if (name.length() != 0)
                name += " ";
            name += args[i];
        }
        CItem* pItem = G_CDataManager()->find_item(name.c_str());
        if (pItem == 0)
        {
            A2_GmBuf b;
            std::sprintf(b.buf, "cannot find item (%s) ", name.c_str());
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            int len = static_cast<int>(std::strlen(b.buf));
            g.put_int(len);
            g.put_str(b.buf, len);
            g.finalize(true);
            user->Send(g);
            return 0;
        }
        {
            A2_GmBuf b;
            std::sprintf(b.buf, "item_index(%d) ", pItem->get_index());
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            int len = static_cast<int>(std::strlen(b.buf));
            g.put_int(len);
            g.put_str(b.buf, len);
            g.finalize(true);
            user->Send(g);
        }
        int itemIdx = pItem->get_index();
        int count = 100;
        if (pItem->is_stackable())
        {
            int limit = static_cast<CStackableItem*>(pItem)->getStackableLimit();
            if (limit < count)
                count = limit;
        }
        ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
        int slot = user->AddItem(itemIdx, count, (eItemAddReason)6, space, 0);
        if (slot < 0)
        {
            return 0;
        }
        {
            char* accIdStr = NumberToString(user->get_acc_id(), 0);
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xb916, 0);
            trace("%s:\'%s\' GM Mode Add Item(%d).", accIdStr,
                  user->getCurCharacName(), slot);
        }
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }
    // ---- itemskill ----
    else if (cmd == "itemskill")
    {
        int a = std::atoi(arg1.c_str());
        int b = std::atoi(arg2.c_str());
        CItemMakingSkill* skill = reinterpret_cast<CItemMakingSkill*>(
            user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(10)));
        skill->SetItemMakingSkillPoint(a, b, user);
    }
    // ---- multimail stack/equip ----
    else if (cmd == "multimail")
    {
        if (arg1 == "stack")
        {
            std::vector<std::pair<int, int> > items;
            items.clear();
            items.push_back(std::make_pair(5, 0xbdd));
            items.push_back(std::make_pair(4, 0xbda));
            items.push_back(std::make_pair(3, 0xbd9));
            items.push_back(std::make_pair(2, 0xbdb));
            items.push_back(std::make_pair(1, 0xbdc));
            Inven_Item base[10];
            for (int i = 9; i >= 0; --i)
                base[i] = Inven_Item();
            WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, base, 10);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                "multimail test", base, 5, 1000, user->getCurCharacNo(),
                "stackable item test",
                static_cast<int>(std::strlen("stackable item test")),
                0, (ENUM_SERVER_GROUP)99, false);
        }
        else if (arg1 == "equip")
        {
            std::vector<std::pair<int, int> > items;
            items.clear();
            items.push_back(std::make_pair(5, 10000));
            items.push_back(std::make_pair(5, 0x2711));
            items.push_back(std::make_pair(5, 0x2712));
            items.push_back(std::make_pair(5, 0x2713));
            items.push_back(std::make_pair(5, 0x2714));
            items.push_back(std::make_pair(5, 0x2715));
            items.push_back(std::make_pair(5, 0x2716));
            items.push_back(std::make_pair(5, 0x2717));
            items.push_back(std::make_pair(5, 0x2718));
            items.push_back(std::make_pair(5, 0x2719));
            Inven_Item base[10];
            for (int i = 9; i >= 0; --i)
                base[i] = Inven_Item();
            WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, base, 10);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                "multimail test", base, 10, 9999, user->getCurCharacNo(),
                "equip item test",
                static_cast<int>(std::strlen("equip item test")),
                0, (ENUM_SERVER_GROUP)99, false);
        }
    }
    // ---- statistic gold ----
    else if (cmd == "statistic")
    {
        if (arg1 == "gold")
        {
            GetInstanceCirculationStatistic()->InsertCirculationStatistic();
        }
    }
    // ---- ingameday/day ----
    else if (cmd == "ingameday" || cmd == "day")
    {
        DoUserDefineCommand(user, 0x66, arg1.c_str());
    }
    // ---- setinven ----
    else if (cmd == "setinven")
    {
        int cap = std::atoi(arg1.c_str());
        int oldCap = user->getCurCharacInvenR()->get_inventory_capacity();
        user->m_historyLog.ExpandInventory(oldCap, cap, 2);
        user->getCurCharacInvenW()->set_inventory_capacity(cap);
        int newCap = user->getCurCharacInvenR()->get_inventory_capacity();
        DB_UpdateInventoryCapacity::makeRequest(user->GetServerGroup(),
                                                static_cast<int>(user->GetUID()),
                                                user->getCurCharacNo(), newCap);
        guard.clear();
        guard.put_header(0, 0x42);
        guard.put_short(0xb);
        guard.put_short(newCap);
        guard.finalize(true);
        user->Send(guard);
    }
    // ---- itemseed ----
    else if (cmd == "itemseed")
    {
        int itemIdx = std::atoi(arg1.c_str());
        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            A2_GmBuf b;
            std::sprintf(b.buf, "cannot find item (%s) ", arg1.c_str());
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            int len = static_cast<int>(std::strlen(b.buf));
            g.put_int(len);
            g.put_str(b.buf, len);
            g.finalize(true);
            user->Send(g);
            return 0;
        }
        int idx = pItem->get_index();
        int count = 100;
        if (pItem->is_stackable())
        {
            int limit = static_cast<CStackableItem*>(pItem)->getStackableLimit();
            if (limit < count)
                count = limit;
        }
        ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
        int slot = user->AddItem(idx, count, (eItemAddReason)6, space, 0);
        if (slot < 0)
        {
            return 0;
        }
        Inven_Item* ref = user->getCurCharacInvenW()->GetInvenRef(1, slot);
        if (ref != 0)
            ref->m_addInfo2 = std::atoi(arg2.c_str());   // +0x07 add_info
        {
            char* accIdStr = NumberToString(user->get_acc_id(), 0);
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xb9fb, 0);
            trace("%s:\'%s\' GM Mode Add Item(%d).", accIdStr,
                  user->getCurCharacName(), slot);
        }
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }
    // ---- moveavatar/mva ----
    else if (cmd == "moveavatar" || cmd == "mva")
    {
        PacketDispatcher* disp = G_PacketDispatcher();
        IDispatcher* d13 = disp->getDispatcher(0x13);
        if (d13 == 0)
        {
            return 0;
        }
        {
            PacketGuard g;
            g.put_header(1, 0x13);
            g.put_byte(1);
            g.put_short(0);
            g.put_int(0);
            g.put_int(1);
            g.put_byte(3);
            g.put_short(1);
            g.put_int(0x9b07);
            g.put_int(0);
            g.put_byte(0);
            g.put_byte(0);
            g.put_byte(0);
            g.finalize(false);
            g.set_index(0xf);
            // ORIG：vtbl[0]（dispatch_template）以 (obj, user, PacketBuf*) 调用。
            d13->dispatch_template(user, *g.get());
        }
    }
    // ---- quickon/qon ----
    else if (cmd == "quickon" || cmd == "qon")
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0 && party->get_member_count() >= 2)
        {
            party->send_quick_party_matching_result(true, 0);
            party->set_quick_party(true);
        }
        return 0;
    }
    // ---- quickoff/qoff ----
    else if (cmd == "quickoff" || cmd == "qoff")
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0 && party->get_member_count() >= 2)
        {
            party->send_quick_party_matching_result(false, 0);
            party->set_quick_party(false);
        }
        return 0;
    }
    // ---- quickreward/qr ----
    else if (cmd == "quickreward" || cmd == "qr")
    {
        int v = std::atoi(arg1.c_str());
        if (v < 0 || 10 < v)
        {
            return 0;
        }
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0 && party->get_member_count() >= 2)
            {
                party->set_gm_random_buff_type(static_cast<QuickParty::RandomBuffType>(v));
                A2_GmBuf b;
                std::sprintf(b.buf, "GM QUICK PARTY RANDOM BUFF (%d) TYPE!!", v);
                PacketGuard g;
                g.put_header(0, 0xc);
                g.put_byte(0);
                g.put_short(0);
                g.put_byte(0);
                int len = static_cast<int>(std::strlen(b.buf));
                g.put_int(len);
                g.put_str(b.buf, len);
                g.finalize(true);
                party->send_to_party(g);
            }
        }
        return 0;
    }
    // ---- hellvalue/hv ----
    else if (cmd == "hellvalue" || cmd == "hv")
    {
        int v = std::atoi(arg1.c_str());
        if (v < 0 || 100 < v)
        {
            return 0;
        }
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0 && party->get_member_count() >= 2)
            {
                BattleData* bd =
                    reinterpret_cast<BattleData*>(party->m_padBattleData);  // +0x32c
                bd->SetHellPartyValueTotal(v);
                party->send_hellParty_value();
                A2_GmBuf b;
                std::sprintf(b.buf, "HELL PARTY VALUE (%d)",
                             bd->GetHellPartyValueTotal());
                PacketGuard g;
                g.put_header(0, 0xc);
                g.put_byte(0);
                g.put_short(0);
                g.put_byte(0);
                int len = static_cast<int>(std::strlen(b.buf));
                g.put_int(len);
                g.put_str(b.buf, len);
                g.finalize(true);
                party->send_to_party(g);
            }
        }
        return 0;
    }
    // ---- statisticlog/sl ----
    else if (cmd == "statisticlog" || cmd == "sl")
    {
        StatisticsGmCmdPacket pkt;
        pkt.m_characNo = user->getCurCharacNo();      // +0x0a（ORIG local_34c）
        CStatisticServerProxy* proxy =
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0xe);
    }
    // ---- itemdrop / upcard ----
    else if (cmd == "itemdrop")
    {
        DoUserDefineCommand(user, 0x6f, arg1.c_str());
    }
    else if (cmd == "upcard")
    {
        DoUserDefineCommand(user, 0x70, arg1.c_str());
    }
    // ---- inde item drop ----
    else if (cmd == "inde" && arg1 == "item" && arg2 == "drop")
    {
        std::string st;
        st = (user->getForceDropFlag() == 1) ? "off" : "on";
        DoUserDefineCommand(user, 0x77, st.c_str());
    }
    // ---- timegate/tg ----
    else if (cmd == "timegate" || cmd == "tg")
    {
        Packet_CollectItemsGm pkt;
        pkt.m_a = std::atoi(arg1.c_str());            // +0x0b（ORIG local_368）
        pkt.m_b = std::atoi(arg2.c_str());            // +0x0f（ORIG local_364）
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x17);
    }
    // ---- bossdifficult / bossmap ----
    else if (cmd == "bossdifficult")
    {
        char v = static_cast<char>(StringToNumber(arg1.c_str()));
        if (v > '\0' && v < '\x05')
            user->m_gmBossDifficulty = v;              // +0x796cc
    }
    else if (cmd == "bossmap")
    {
        unsigned int v = static_cast<unsigned int>(StringToNumber(arg1.c_str()));
        user->m_gmBossMapIndex = v;                    // +0x796d0
    }
    // ---- reset fatigue ----
    else if (cmd == "reset" && arg1 == "fatigue")
    {
        G_TimerQueue()->InsertTimer(
            static_cast<TimerEntry::OBJ_TYPE>(2), 0,
            static_cast<TIMER_MESSAGE>(0x50), 3, 0, 0);
        return 0;
    }
    // ---- apply_effect ----
    else if (cmd == "apply_effect")
    {
        int code = StringToNumber(arg1.c_str());
        int value = StringToNumber(arg2.c_str());
        if (code < 0)
        {
            ret = 0;
        }
        else
        {
            AradServerStateMessage msg;
            msg.m_active = static_cast<short>(value != 0);
            msg.m_endSec = static_cast<unsigned int>(value);
            GlobalData::s_serverStateManager_->deleteState(
                static_cast<ENUM_GLOBAL_EFFECT_ARAD>(code));
            GlobalData::s_serverStateManager_->insertState(
                static_cast<ENUM_GLOBAL_EFFECT_ARAD>(code), msg);
            ret = 0;
        }
        return 0;
    }
    // ---- open sd/boss/aa ----
    else if (cmd == "open")
    {
        if (arg1 == "sd")
            GlobalData::s_DailyScheduleManager->GM_OpenSouthernDale();
        else if (arg1 == "boss")
            GlobalData::s_DailyScheduleManager->GM_OpenBossTower();
        else if (arg1 == "aa")
            GlobalData::s_DailyScheduleManager->GM_OpenAdvanceAltar();
    }
    // ---- close sd/boss/aa ----
    else if (cmd == "close")
    {
        if (arg1 == "sd")
            GlobalData::s_DailyScheduleManager->GM_CloseSouthernDale();
        else if (arg1 == "boss")
            GlobalData::s_DailyScheduleManager->GM_CloseBossTower();
        else if (arg1 == "aa")
            GlobalData::s_DailyScheduleManager->GM_CloseAdvanceAltar();
    }
    // ---- ap set/reset/gmmode/realmode/week ----
    else if (cmd == "ap")
    {
        if (arg1 == "set")
        {
            APSystem::CUserProc::GM_Set(user, static_cast<unsigned int>(std::atoi(arg2.c_str())));
        }
        else if (arg1 == "reset")
        {
            APSystem::CUserProc::GM_Reset(user);
        }
        else if (arg1 == "gmmode")
        {
            APSystem::CActionPointManager::_IsGMOn = 1;
        }
        else if (arg1 == "realmode")
        {
            APSystem::CActionPointManager::_IsGMOn = 0;
            APSystem::CActionPointManager::_gmWeekday.m_weekday = 0;
        }
        else if (arg1 == "week")
        {
            int v = std::atoi(arg2.c_str());
            int weekday = 7;
            switch (v)
            {
                case 1: weekday = 1; break;
                case 2: weekday = 2; break;
                case 3: weekday = 3; break;
                case 4: weekday = 4; break;
                case 5: weekday = 5; break;
                case 6: weekday = 6; break;
                case 7: weekday = 0; break;
                default:
                    ret = 0;
                    return 0;
            }
            APSystem::CActionPointManager::_gmWeekday.m_gmOn = 1;
            APSystem::CActionPointManager::_gmWeekday.m_weekday = weekday;
        }
    }
    // ---- pcroom ----
    else if (cmd == "pcroom")
    {
        Taiwan::Packet_Pcroom_Response pkt;
        pkt.setResult(1);                             // +0x12（ORIG local_38c=1）
        std::time_t now = std::time(0);
        pkt.setPatchTime(static_cast<unsigned int>(now) + 0x93a80);  // +0x16（ORIG local_388）
        pkt.setUid(user->GetUID());                   // +0x0a（ORIG local_394）
        pkt.setAccId(user->get_acc_id());             // +0x0e（ORIG local_390）
        pkt.setMobileAuth(1);                         // +0x1a（ORIG local_384=1）
        pkt.setGcaType(0xb);                          // +0x1b（ORIG local_383=0xb）
        {
            Stream* s = GlobalData::s_stream_pool->Acquire(
                "PacketDispatcher_Impl_1.cpp", 0xbc0a);
            CStreamGuard g(s, true);
            g << 0x17e;
            g << static_cast<int>(user->GetUID());
            g->put_binary(reinterpret_cast<char*>(&pkt),
                          static_cast<int>(pkt.getLen()));   // ORIG len=local_39c
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), g);
        }
    }
    // ---- avatarcoin ----
    else if (cmd == "avatarcoin")
    {
        unsigned int v = static_cast<unsigned int>(std::atoi(arg1.c_str()));
        AvatarCoin* coinR = &user->getCurCharacInvenW()->m_avatarCoin;  // +0x658
        AvatarCoin* coinW = &user->getCurCharacInvenW()->m_avatarCoin;  // +0x658
        unsigned int total = v + coinR->Get();
        coinW->Set(total);
        AvatarCoin::SaveToDB(user);
        AvatarCoin::SendSyncPacket(user);
    }
    // ---- avr sorneh/ghkrfbf ----
    else if (cmd == "avr")
    {
        if (arg1 == "sorneh")
        {
            short a = static_cast<short>(std::atoi(arg2.c_str()));
            short b = static_cast<short>(std::atoi(arg3.c_str()));
            ARAD::Singleton<AvatarRechargeServer>::Get()->GmReduceEndurance(user, a, b);
        }
        else if (arg1 == "ghkrfbf")
        {
            if (arg2 == "on")
                ARAD::Singleton<AvatarRechargeServer>::Get()->GmChangeEnduranceReduceRate(
                    user, true, std::atoi(arg3.c_str()));
            else if (arg2 == "off")
                ARAD::Singleton<AvatarRechargeServer>::Get()->GmChangeEnduranceReduceRate(user, false, 0);
        }
    }
    // ---- startva ----
    else if (cmd == "startva")
    {
        if (user != 0)
        {
            Packet_Game_Monitor_GM_Village_Attacked pkt;
            pkt.m_flag = 1;                           // +0x0a（ORIG local_3b5）
            pkt.m_a = std::atoi(arg1.c_str());        // +0x0c（ORIG local_3b4）
            pkt.m_b = std::atoi(arg2.c_str());        // +0x10（ORIG local_3b0）
            pkt.m_c = std::atoi(arg3.c_str());        // +0x14（ORIG local_3ac）
            CMonitorServerProxy* proxy =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
            proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x17);
        }
    }
    // ---- SetAvengerReady ----
    else if (cmd == "SetAvengerReady")
    {
        int v = std::atoi(arg1.c_str());
        std::string name = "Arad_EventPeriodDataManager";
        ARAD::Arad_DataManager* dm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        ARAD::Arad_EventPeriodDataManager* period =
            static_cast<ARAD::Arad_EventPeriodDataManager*>(
                dm->findGameScript(name));
        period->setCreatePeriod_GM(v);
    }
    // ---- SetCreateTime ----
    else if (cmd == "SetCreateTime")
    {
        if (args.size() < 5)
        {
            return 0;
        }
        {
            short fields[4] = {0, 0, 0, 0};
            fields[0] = static_cast<short>(std::atoi(args[1].c_str()));
            fields[1] = static_cast<short>(std::atoi(args[2].c_str()));
            fields[2] = static_cast<short>(std::atoi(args[3].c_str()));
            fields[3] = static_cast<short>(std::atoi(args[4].c_str()));
            long long t = ARAD::uint32Time(fields);
            user->set_charac_createtime(t);
            arad::SigSetCharacCreateTime sig;
            unsigned int createTime = user->getCurCharacCreateTime();
            unsigned int characNo = user->get_charac_no(-1);
            sig.set(characNo, createTime);
            Stream* s = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xbc83);
            CStreamGuard g(s, true);
            unsigned int uid = user->GetUID();
            ARAD::DISPATCHER::make_internal_stream_jpn(
                g, ARAD::ENUM_INTERNALPACKET_JPN(0x24), static_cast<int>(uid));
            g->put_binary(reinterpret_cast<char*>(&sig), 8);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), g);
        }
    }
    // ---- aa time/star/ticket/reset ----
    else if (cmd == "aa")
    {
        advancealtar::CharacAdvanceAltarManager* mgr =
            &user->m_advanceAltar;                     // +0x8df60
        if (arg1 == "time")
        {
            int v = std::atoi(arg2.c_str());
            if (0 < v && v < 0x33)
                advancealtar::Manager::gmTimeSpeed_ = v;
        }
        else if (arg1 == "star")
        {
            int v = std::atoi(arg2.c_str());
            if (-1 < v && v < 1000)
                mgr->gmSetStar(v);
        }
        else if (arg1 == "ticket")
        {
            int v = std::atoi(arg2.c_str());
            if (-1 < v && v < 100)
                mgr->gmSetTicket(v);
        }
        else if (arg1 == "reset" && arg2 == "star")
        {
            int v = std::atoi(arg3.c_str());
            if (-1 < v && v < 2)
                mgr->resetStar(user, static_cast<CMDPacketStruct::ResetStarType::T>(v));
        }
    }
    // ---- SetRewardLast / SetQuestLast / SetBreak ----
    else if (cmd == "SetRewardLast")
    {
        int v = StringToNumber(arg1.c_str());
        CLocalChina_DB_LastDay_BreakAway::makeRequest(user->get_acc_id(), v, 1);
    }
    else if (cmd == "SetQuestLast")
    {
        int v = StringToNumber(arg1.c_str());
        CLocalChina_DB_LastDay_BreakAway::makeRequest(user->get_acc_id(), v, 0);
    }
    else if (cmd == "SetBreak")
    {
        int v = StringToNumber(arg1.c_str());
        GlobalData::s_BreakAwaySys->GM_UpdateUvList(
            user->get_acc_id(), static_cast<unsigned int>(v));
    }

    return 0;
}
