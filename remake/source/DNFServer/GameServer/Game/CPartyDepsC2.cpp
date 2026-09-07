// ============================================================================
// CPartyDepsC2.cpp —— CParty 第三组独占恢复（2026-08-24，PartyDepsC2）
//
// 覆盖 11 个 ORIG 符号（全部经 docs/class_func_reports/CParty/ 对应报告
// ## 汇编 + ## 反编译 C 完整读取后转写；未命名 thunk 均以 nm 定位为真实 W 符号）：
//   increase_ancient_dungeon_ticket  T 0x0859bc9a（increase_ancient_dungeon_ticket.md）
//   isDungeonDifficultyInfromNotice  T 0x085be8a0（isDungeonDifficultyInfromNotice.md）
//   isGuildParty                     T 0x085b49d0（isGuildParty.md）
//   isPremiumGoldCardReward          T 0x085b11ee（isPremiumGoldCardReward.md）
//   make_blood_dungeon_info          T 0x0859e0d2（make_blood_dungeon_info.md）
//   make_blood_dungeon_info_blank    T 0x0859e480（make_blood_dungeon_info_blank.md）
//   move_map                         T 0x085a8f4e（move_map.md）
//   send_loadingcheck_msg            T 0x085b1fe0（send_loadingcheck_msg.md）
//   set_random_reward                T 0x0859b8fa（set_random_reward.md）
//   useItem                          T 0x085a7800（useItem.md）
//   use_skill                        T 0x085a77d8（use_skill.md）
// ============================================================================

#include <string.h>
#include <vector>

#include "CParty.h"
                 // CGameManager.h 先于 CDungeon.h：其 QuickParty.h 完整定义
                 // 需先于 CDungeon.h 的 CQuickPartySystemManager 兜底声明
#include "CGameManager.h"
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "Secu_HackLogCheckByParty.h"
#include "PacketTypes.h"         // MSG_MOVE_MAP 完整类型
#include "CCommonStatisticsMgr.h" // GetInstanceCommonStatisticsMgr/IncreaseQuantity
#include "CDungeon.h"
#include "CDataManager.h"
#include "CInventory.h"         // insertItemIntoInventory 完整类型
#include "CWorldMap.h"
#include "GameWorld.h"
#include "CBloodDungeonRewardFirstEvent.h"   // GetGold W 0x085bff1a（真实类，本批次闭环）
#include "CBloodDungeonRewardSecondEvent.h"  // GetGold W 0x085bff26
 #include "CGuildServerProxy.h"   // CEventManager / CEventBase（IsEventing vptr+0x34 槽13）
 #include "GlobalData.h"
 #include "CMailBoxHelper.h"
 #include "RDARScriptStringManager.h"
 #include "CBattle_Field.h"

namespace
{

// ---- CCharacBloodDungeon 只读视图（类未建模，同 CUser.cpp 本地视图惯例）----
// 报告缺失标记：docs/class_func_reports/CCharacBloodDungeon/ 仅 ResetDaily/
// _reset/_saveData/getData/loadData 五函数，无两 getter 报告；按 nm 定位后
// objdump 转写（各 2 条指令）：GetEnterCount W 0x085bfe88 = *(int*)(this+0x18)、
// GetWeeklyEnterCount W 0x085bfe94 = *(int*)(this+0x0c)。loadData 报告证实
// +0x08..+0x28 为连续 int 计数区，两偏移均落其中。
struct BloodDungeonCountsView
{
    char m_pad0[0xc];
    int m_weeklyEnterCount;   // +0x0c（GetWeeklyEnterCount 读取）
    char m_pad10[0x8];        // +0x10..+0x17
    int m_enterCount;         // +0x18（GetEnterCount 读取）
};

}  // namespace

// ---------------------------------------------------------------------------
// increase_ancient_dungeon_ticket —— ORIG T 0x0859bc9a
// 报告：docs/class_func_reports/CParty/increase_ancient_dungeon_ticket.md
// ---------------------------------------------------------------------------
void CParty::increase_ancient_dungeon_ticket()
{
    if (m_battleField.m_pDungeon == 0)
        return;
    // movzbl 0x89c：要求副本 +0x89c 字节 == 1 才继续
    if (m_battleField.m_pDungeon->m_89c != 1)
        return;

    // dungeon+0x7f4 = RequiredItem.m_0（道具 idx）；+0x7f8 = RequiredItem.m_4
    const int itemId = m_battleField.m_pDungeon->m_entryRequiredItem.m_itemIdx;
    const int createParam = m_battleField.m_pDungeon->m_entryRequiredItem.m_itemCount;

    // find_item 结果存槽 local，比较用 cmpl $0（ORIG -0x14）；G_CDataManager()
    // 每次内联重取（P0：ORIG 无 dataMgr 局部，寄存器直传）。
    CItem* found = G_CDataManager()->find_item(itemId);  // find_item(int) const 0x0835fa32
    if (found == 0)
        return;

    Inven_Item item;                        // Inven_Item::Inven_Item W 0x080cb854
    item.m_addInfo = itemId;                // uStack_70/uStack_6e 两 short 写 = +0x02 int
    G_CDataManager()->m_itemList->create_item(  // CItemList::create_item T 0x08511fb8
        (ENUM_ITEM_CREATE_TYPE)0, item, createParam);

    // create 后有效性判定：m_addInfo(+0x02) 与 get_add_info()(+0x07) 任一非零
    // ORIG：bVar1 在寄存器物化（mov$1/jmp/mov$0 后 test%al），条件为
    // (m_addInfo != 0) || (get_add_info() != 0)
    if (item.m_addInfo != 0 || item.get_add_info() != 0)
    {
        for (int i = 0; i < 4; ++i)         // cmpl $3/setle：i<=3 共 4 槽
        {
            if (!checkValidUser(i))         // checkValidUser(int) W 0x08145868
                continue;
            int slot = -1;                  // movl $0xffffffff,-0xc
            // insertItemIntoInventory(Inven_Item, eItemAddReason, bool, bool)
            // 实参 (item, 0x38, 1, 1)；返回槽号符号扩展判负
            slot = m_member[i].m_pUser->getCurCharacInvenW()
                       ->insertItemIntoInventory(
                           item, (eItemAddReason)0x38, true, true);
            if (slot < 0)
            {
                // 入包失败 → 系统邮件补发门票后重置栈上物品
                char title[0x15];           // local_35[21]
                strncpy(title,
                        g_scriptStringManager_.findString(4, "game_server_msg_53", 0),
                        0x14);
                char body[0x100];           // local_172[256]
                strncpy(body,
                        g_scriptStringManager_.findString(4, "game_server_msg_154", 0),
                        0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, 0,
                    (unsigned int)m_member[i].m_pUser->getCurCharacNo(),
                    body, (int)strlen(body), 0,
                    m_member[i].m_pUser->GetServerGroup(), false, false);
                item.reset();               // Inven_Item::reset W 0x080cb7d8
            }
            else
            {
                m_member[i].m_pUser->SendUpdateItem((CUser::eSendTarget)1,
                                                    (ENUM_ITEMSPACE)0, slot);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// isDungeonDifficultyInfromNotice —— ORIG T 0x085be8a0
// 报告：docs/class_func_reports/CParty/isDungeonDifficultyInfromNotice.md
// ---------------------------------------------------------------------------
bool CParty::isDungeonDifficultyInfromNotice(bool flag)
{
    // this+0x74 = m_manager（CUserCharacInfo 基类方法直呼）
    if (m_manager->getMemberDungeonFlag() != 0)      // W 0x085bfaec
        return false;
    if (m_manager->getCurCharacTutorialFlag() == 0)  // W 0x085bfb44：新手教程
        return false;                                // 未完成（==0）不提示
    if (flag)                                // param_1 为真直接不提示
        return false;
    // this+0xc4c = CBattle_Field(+0xb24)+0x128 = m_villageMonsterId(short)。
    // 直接成员访问，编译器以 this 基址直寻 0xc4c（ORIG `movzwl 0xc4c(%eax)`）。
    if (m_battleField.m_villageMonsterId != 0)
        return false;
    if (getManager()->getMoveSpace() == (UserSpace::T)1)   // getMoveSpace 0x08690eca
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// isGuildParty —— ORIG T 0x085b49d0
// 报告：docs/class_func_reports/CParty/isGuildParty.md
// ---------------------------------------------------------------------------
bool CParty::isGuildParty()
{
    bool result = false;     // local_19
    int guildKey = -1;       // local_14
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))            // _ZN6CParty15_checkValidUserEi 0x85b4d12
            continue;
        int key = m_member[i].m_pUser->get_charac_guildkey();  // W 0x0822f46c
        if (key == 0)
            return false;
        if (key != guildKey && guildKey != -1)
            return false;
        result = true;
        guildKey = key;
    }
    return result;
}

// ---------------------------------------------------------------------------
// isPremiumGoldCardReward —— ORIG T 0x085b11ee
// 报告：docs/class_func_reports/CParty/isPremiumGoldCardReward.md
//   数据经 m_serverParameter(+0x68) m_premiumGoldCard4894(+0x4894) m_conds[i]
//   （+0x08 起，步长 0x10）：count=m_conditionCount（short @+0x04，0x4900），
//   元素 min=max=prob=reward 与原 ORIG 平铺 int 偏移 0x4904/+0x4910 一致。
// ---------------------------------------------------------------------------
bool CParty::isPremiumGoldCardReward(CDungeon const* dungeon, unsigned int& param)
{
    // ORIG 0x085b11ee：dungeon->get_standard_level() 结果存槽（local -0x28）；
    // G_CDataManager() 每次取字段时重新调用（-O0 无 CSE，ORIG 每轮 loop 4 次重取），
    // 无命名 mgr/ev 局部（P0 槽律）。字段经 m_serverParameter(+0x68)
    // m_premiumGoldCard4894(+0x4894) m_conds[i]（+0x08 起，步长 0x10）。
    const int standardLevel = dungeon->get_standard_level();   // W 0x080f9810

    // movzwl 0x4900 + cwtl：count = m_conditionCount（short @ +0x04）扩为 int 上限
    const int condCount =
        (int)G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conditionCount;

    for (int i = 0; i < condCount; ++i)
    {
        const int minLevel =
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_field0;
        const int maxLevel =
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_field4;
        const int prob =
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_field8;
        const unsigned int rewardItem = (unsigned int)
            G_CDataManager()->m_serverParameter.m_premiumGoldCard4894.m_conds[i].m_fieldc;

        if (minLevel < 1)
            return false;
        if (maxLevel < 1)
            return false;
        if (prob < 1)
            return false;
        if (standardLevel >= minLevel && standardLevel <= maxLevel)
        {
            const int dice = get_rand_int(1000);   // get_rand_int T 0x086b1b87
            if (dice <= prob)
            {
                param = rewardItem;
                return true;
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// make_blood_dungeon_info —— ORIG T 0x0859e0d2
// 报告：docs/class_func_reports/CParty/make_blood_dungeon_info.md
// ---------------------------------------------------------------------------
bool CParty::make_blood_dungeon_info(CUser* user, PacketGuard& packet)
{
    if (m_hellWorldMap == 0)
    {
        // G_GameWorld W 0x080da3a7；GetWorldMapIndex T 0x086cebe0
        int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
        if (worldIdx > 0)
            m_hellWorldMap =   // CWorldMapList::find_world_map T 0x083647a2
                G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
    }
    if (m_hellWorldMap == 0)
        return false;
    if (!m_hellWorldMap->hasDeathTower())      // hasDeathTower W 0x0822b520
        return false;

    packet.put_byte(get_member_count());

    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        CUser* memberUser = m_member[i].m_pUser;

        // put_short W 0x080d9ea4；uid 取低 16 位
        packet.put_short(memberUser->get_unique_id() & 0xffff);

        // 计数占位字节回填：get_index W 0x08110b4c / put_byte(int&,int) W 0x08110b28
        int countIdx = packet.get_index();
        int dungeonCount = 0;
        packet.put_byte(0);                    // put_byte W 0x080cb920

        // CWorldMap::GetDungeonList W 0x085beda4 = &m_dungeonList(+0x10)
        std::vector<int>& dungeonList = m_hellWorldMap->m_dungeonList;
        for (std::vector<int>::iterator it = dungeonList.begin();
             it != dungeonList.end(); ++it)
        {
            CDungeon* dungeon = G_CDataManager()->find_dungeon(*it);  // 0x0835f9f8
            if (dungeon == 0 || (char)dungeon->m_bloodDungeonType != 1)
                continue;

            packet.put_short(*it);
            packet.put_int(get_blood_dungeon_admission_fee(memberUser));
            BloodDungeonCountsView* blood =
                reinterpret_cast<BloodDungeonCountsView*>(
                    memberUser->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_1));
            packet.put_int(blood->m_enterCount);        // GetEnterCount W 0x085bfe88
            packet.put_int(blood->m_weeklyEnterCount);  // GetWeeklyEnterCount W 0x085bfe94
            ++dungeonCount;
        }
        packet.put_byte(countIdx, dungeonCount);
    }

    // 金币奖励：repeat 0x2e(SecondEvent) → 0x2d(FirstEvent) → Jackpot
    // IsEventing 经 vptr+0x34（槽13）虚调用，实参 0
    CEventBase* secondEv = GlobalData::s_event_manager->GetRepeatEvent(0x2e);
    if (secondEv->IsEventing(0))
    {
        packet.put_int(
            static_cast<CBloodDungeonRewardSecondEvent*>(secondEv)->GetGold());
    }
    else
    {
        CEventBase* firstEv = GlobalData::s_event_manager->GetRepeatEvent(0x2d);
        if (firstEv->IsEventing(0))
        {
            packet.put_int(
                static_cast<CBloodDungeonRewardFirstEvent*>(firstEv)->GetGold());
        }
        else
        {
            packet.put_int(G_CDataManager()->m_serverParameter.GetJackpot());
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// make_blood_dungeon_info_blank —— ORIG T 0x0859e480
// 报告：docs/class_func_reports/CParty/make_blood_dungeon_info_blank.md
// ---------------------------------------------------------------------------
void CParty::make_blood_dungeon_info_blank(PacketGuard& packet)
{
    packet.put_byte(get_member_count());
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            packet.put_short(m_member[i].m_pUser->get_unique_id() & 0xffff);
            packet.put_byte(0);
        }
    }
    packet.put_int(0);
}

// ---------------------------------------------------------------------------
// move_map —— ORIG T 0x085a8f4e
// 报告：docs/class_func_reports/CParty/move_map.md
// ---------------------------------------------------------------------------
bool CParty::move_map(MSG_MOVE_MAP& info)
{
    PacketGuard packet;   // ORIG 在状态检查前构造，所有出口析构

    if (m_state != 2)
    {   // 非地图移动状态 → 0x30 号 cmd 包错误码 0x13
        send_to_party_cmd_error((ENUM_CMDPACKET)0x30, 0x13);
        return false;
    }

    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_battleField);

    if (m_battleField.m_pDungeon != 0 && m_battleField.m_pDungeon->IsEnterEachMap())
    {   // 逐张进入型副本：直接改写当前座位号
        bf.SetCurSeatNo((int)(char)info.m_39);          // SetCurSeatNo W 0x082a43cc
    }
    else if (bf.IsClearDungeon())                       // IsClearDungeon W 0x0822d08c
    {
        send_to_party_cmd_error((ENUM_CMDPACKET)0x30, 0x13);
        return false;
    }

    const int curPosX = bf.GetCurPosX();                // W 0x085bf32c
    const int curPosY = bf.GetCurPosY();                // W 0x085bf382
    const int fromMapIdx = bf.get_map_index(curPosX, curPosY);  // W 0x085bf260

    if (info.m_field17 == 1)
        bf.checkLayeredMap((int)info.m_x, (int)info.m_y);   // T 0x0830e32e

    // ORIG 第二次 get_map_index 结果存栈后未再消费，保留调用形态
    const int toMapIdx = bf.get_map_index((int)info.m_x, (int)info.m_y);
    (void)toMapIdx;

    // 反作弊路径校验（隐式 this=&m_padSecu 即 Secu 子对象 +0x1abc）
    reinterpret_cast<Secu_HackLogCheckByParty*>(m_padSecu)
        ->CheckMoveMapFitInPathHackCnt(m_host, this, info.m_fieldF,
                                       info.m_field13, fromMapIdx);

    bool firstVisit = false;
    const int ret = bf.MoveMap((int)info.m_x, (int)info.m_y, firstVisit);  // T 0x08308ae6
    if (ret > 0)
    {   // MoveMap 失败码 > 0：错误码原样回发
        send_to_party_cmd_error((ENUM_CMDPACKET)0x30, (unsigned char)(ret & 0xff));
        return false;
    }

    checkMoveMapHackCnt(info);

    if ((char)m_battleField.m_pDungeon->m_bloodDungeonType < 1)
        bf.MakePacket(0x1d, &packet);       // MakePacket T 0x083090a4
    else
        bf.MakePacket(0xd3, &packet);
    send_to_party(packet);

    check_passive_skill();
    send_loadingcheck_msg();
    resetFinishLoadingVar();
    changePartyMemberInfoOnMoveMap(info, firstVisit);

    // 队伍统计：人数>1 时按等级差分桶计数
    const int levelGap = _getMemberLevelGap();
    if (get_member_count() > 1)
    {
        if (levelGap < 7)
            GetInstanceCommonStatisticsMgr()->IncreaseQuantity(
                (CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)0, 3);
        else
            GetInstanceCommonStatisticsMgr()->IncreaseQuantity(
                (CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)0, GetPartyType());
    }

    changeBattleDataOnMoveMap(firstVisit);

    if (getDungeonDropTestFlag() && !firstVisit)
    {   // GM 掉落测试：记录途经地图（+0x1af4 vector<MapInfo>，push_back W 0x084b472c）
        std::vector<MapInfo>& passedMaps =
            *reinterpret_cast<std::vector<MapInfo>*>(&m_padPassedMap);
        passedMaps.push_back(*bf.GetCurrentMapInfo());   // GetCurrentMapInfo W 0x0822cbcc
    }
    return true;
}

// ---------------------------------------------------------------------------
// send_loadingcheck_msg —— ORIG T 0x085b1fe0
// 报告：docs/class_func_reports/CParty/send_loadingcheck_msg.md
// 常量沿用 ORIG 局部静态实证值（同 source/Community/CommonConstants.h）：
//   CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT=0x78、CONST_PARTY_...=0x5a
// ---------------------------------------------------------------------------
void CParty::send_loadingcheck_msg()
{
    m_pad119 = 0;   // this+0x119 字节清零（装载检查标志）

    int timeout = 0x5a;   // CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT
    if (get_member_count() == 1)
        timeout = 0x78;   // CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT

    const int key = gen_timer_key((TIMER_MESSAGE)0xb);
    // InsertTimer(objType=1, param1=m_partyIndex(+0x64), msg=0xb, sec=timeout,
    //             param2=key, param3=0)
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_partyIndex,
                                (TIMER_MESSAGE)0xb, timeout, key, 0);
}

// ---------------------------------------------------------------------------
// set_random_reward —— ORIG T 0x0859b8fa
// 报告：docs/class_func_reports/CParty/set_random_reward.md
// ---------------------------------------------------------------------------
void CParty::set_random_reward(QuickParty::RandomBuffType type,
                               QuickParty::RewardData_Param param,
                               QuickParty::RewardData_Result& result)
{
    if (!is_quick_party())                  // is_quick_party W 0x0822d952
        return;
    if (m_field1ae8 == 0xb)                 // BASIC_REWARD_CARD_GOLD 占用时跳过
        return;
    if (m_field1ae8 != (int)type)
        return;

    QuickParty::IRewardType* obj =
        G_CGameManager()->GetQuickPartyRewardManager()->get_reward_type_object(
            (QuickParty::ENUM_REWARD_TYPE)1, (unsigned int)type);
    if (obj == 0)
        return;
    obj->process(param, result);            // vtable 槽1（vptr+0x04）
}

// ---------------------------------------------------------------------------
// useItem —— ORIG T 0x085a7800
// 报告：docs/class_func_reports/CParty/useItem.md
// ---------------------------------------------------------------------------
void CParty::useItem(CUser* user, Inven_Item const& item)
{
    reinterpret_cast<CBattle_Field*>(&m_battleField)->onUseItem(user, item);
}

// ---------------------------------------------------------------------------
// use_skill —— ORIG T 0x085a77d8
// 报告：docs/class_func_reports/CParty/use_skill.md
// ---------------------------------------------------------------------------
void CParty::use_skill(CUser* user, int skillIdx, int param)
{
    OnUsedSkill(user, skillIdx, param);     // OnUsedSkill T 0x085b5f8e
}
