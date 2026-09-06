// df_game_r Game/ WongWork::CBossStage 实现（批3：从 GameStubs.cpp 迁移；
// FixCBossPlay：加 ctor/dtor，CBossStage : CBossPlay）。
// StageVirtuals：补 4 个新增虚函数 handleMoveMap/handlePickupItem/
// handleDropItem/handleKillMonster（ORIG 0x0814f66a/0x0814fa76/0x0814ff4a/
// 0x08150388），使本 TU 发出 _ZTVN8WongWork10CBossStageE。
#include "CBossStage.h"
#include "WarField.h"   // MapInfo::Clear / map_item / map_monster
#include "GameRand.h"   // get_rand_int

#include <cstring>

#include "CUser.h"                  // CUser/CHackAnalyzer/cUserHistoryLog/ENUM_ERROR
#include "QuickParty.h"         // 先于 CDungeon.h：QUICKPARTY_H 守卫去重
#include "CDungeon.h"               // CDungeon/get_standard_level/RandomList
#include "CMap.h"                   // CMap/_mapMonster/_mapItem 完整类型
#include "CItemGenerator.h"         // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CGenUniqueNo.h"           // WongWork::CGenUniqueNo::genIPGNo
#include "CParty.h"                 // get_user/checkValidUser/getManager/m_battleField.m_bloodState
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"           // find_map/find_item/G_CDataManager
#include "CInventory.h"             // gain_money/delete_item/insertItemIntoInventory
#include "MonsterIndependentDrop.h" // generateIndependentItem/CAICharacter 权威镜像
#include "CMonster.h"               // CMonster::generateIndependentItem
#include "GlobalData.h"             // GlobalData::s_systemTime_
#include "CSystemTime.h"            // getCurTickCount
#include "CSpecialItemRoutingManager.h"  // StartRouting（RoutingPickupItem）
#include "CGameManager.h"               // G_CGameManager
#include "Secu_HackLogCheckByParty.h"   // MSG_MONSTER_DIE 完整定义

namespace WongWork
{

// Ctor（ORIG 0x0814bd0c）：CBossPlay 基类、置 vtable（编译器自动）、构造
// MapInfo、Clear、get_rand_int(60000) 写 m_assignItemIdx/m_dropItemCnt，
// 函数体内依次 m_stageCur=1、m_dungeon=0、m_difficulty=0。
CBossStage::CBossStage()
{
    m_stageCur = 1;
    m_dungeon = 0;
    m_difficulty = 0;
    m_stageData.Clear();
    m_stageData.m_assignItemIdx = get_rand_int(60000);   // ORIG +0xa30
    m_stageData.m_dropItemCnt = get_rand_int(60000);     // ORIG +0xa34
}

// Dtor（ORIG 0x0814bdd6）：置 vtable（编译器自动）、m_stageCur=1、
//   m_dungeon=0、m_difficulty=0、Clear、rand×2、析构 MapInfo、基类（编译器自动）。
CBossStage::~CBossStage()
{
    m_stageCur = 1;
    m_dungeon = 0;
    m_difficulty = 0;
    m_stageData.Clear();
    m_stageData.m_assignItemIdx = get_rand_int(60000);
    m_stageData.m_dropItemCnt = get_rand_int(60000);
}

// ORIG 0x0814bea2：m_startTick=0、m_endTick=0、MapInfo::Clear、
//   memset(m_useItemCount, 0, 0x10)（CBossPlay::m_useItemCount[4]，ORIG this+0x9c8）。
void CBossStage::reset()
{
    m_startTick = 0;
    m_endTick = 0;
    m_stageData.Clear();
    memset(m_useItemCount, 0, sizeof(m_useItemCount));
}


// ============================================================================
// 新增虚函数（虚表槽 [8]..[11]，紧随 CBossPlay 6 槽之后）
// ============================================================================


// ---- handleMoveMap（ORIG 0x0814f66a）----
// 换图入口：_checkMemberDie 短路 → 逐成员反外挂计数 + MapClear 日志 →
// moveNextStage → randomMapIndex →（GM 强制图/难度）→ find_map → reset →
// consistMap（失败 throw ENUM_ERROR(1)，catch 后广播 0x30 包并返回 0）→
// 记 startTick → SendMapInfoPacket。
int CBossStage::handleMoveMap(unsigned short* mapIndex)
{
    if (_checkMemberDie())
        return 0;

    CParty* party = getParty();
    // 已进行时长（秒）：(m_endTick - m_startTick) / 1000
    int playSec = (m_endTick - m_startTick) / 1000;

    for (int i = 0; i < 4; ++i)
    {
        CUser* user = party->get_user(i);
        if (user == 0 || !party->checkValidUser(i))
            continue;

        CHackAnalyzer* analyzer = user->getHackAnalyzer();
        if (playSec <= 4)
            analyzer->addServerHackCnt(user, (ENUM_HACKTYPE)0x19f, 1, 0, 0);
        if (mapIndex[i] == 0)
            analyzer->addServerHackCnt(user, (ENUM_HACKTYPE)0x19b, 1, 0, 0);
        // ORIG this+0x9c8+i*4 逐 int 判零（CBossPlay::m_useItemCount[i]）
        if (m_useItemCount[i] == 0)
            analyzer->addServerHackCnt(user, (ENUM_HACKTYPE)0xdd, 1, 0, 0);

        // cUserHistoryLog 为 CUser 嵌套成员 m_historyLog（ORIG user+0x79700）
        user->m_historyLog.MapClear(mapIndex[i], playSec, 0);
    }

    if (!moveNextStage())
        return 0;

    int mapIdx = 0;
    if (!randomMapIndex(mapIdx))
        return 0;

    try
    {
        CUser* manager = party->getManager();
        if (manager != 0 && manager->isGMUser())
        {
            // CUser GM 调试字段（"bossmap"/"bossdifficult" 指令写入，取走即清零）
            if (manager->m_gmBossMapIndex != 0)
            {
                mapIdx = manager->m_gmBossMapIndex;
                manager->m_gmBossMapIndex = 0;
            }
            if (manager->m_gmBossDifficulty != 0)
            {
                m_difficulty = (int)(signed char)manager->m_gmBossDifficulty;
                manager->m_gmBossDifficulty = 0;
            }
        }

        const CMap* map =
            (const CMap*)G_CDataManager()->find_map(mapIdx);
        if (map == 0)
            return 0;

        reset();
        if (!consistMap(map))
            throw (ENUM_ERROR)1;

        m_startTick = (int)GlobalData::s_systemTime_.getCurTickCount();
        SendMapInfoPacket();
        return 1;
    }
    catch (const ENUM_ERROR& e)
    {
        PacketGuard packet;
        packet.put_header(1, 0x30);
        packet.put_byte(0);
        packet.put_byte((int)e);
        packet.finalize(true);
        party->send_to_party(packet);
        return 0;
    }
}

// ---- handlePickupItem（ORIG 0x0814fa76）----
// 拾取分发：stageData_.item（MapInfo@+0x9f4 的 m_itemMap@+0xa18）按 itemUID
// 查找；金币（m_item.m_addInfo==0）走 GoldPickupItem；否则 find_item 后按
// 队伍人数/存活数/rarity 分发 Routing/Random/Normal。
int CBossStage::handlePickupItem(CUser* user, int itemUID, bool flag,
                                 char& outRarity)
{
    (void)flag;   // ORIG 未读取第 3 参（仅按值传递占位）
    static const char kPretty[] =
        "virtual bool WongWork::CBossStage::handlePickupItem(CUser*, int, bool, char&)";

    MapInfo& stage = m_stageData;
    std::map<int, map_item>::iterator it = stage.m_itemMap.find(itemUID);
    if (it == stage.m_itemMap.end())
    {
        cMyTrace trace(kPretty, 0x56e, 5);
        trace("stageData_.item not find itemUID : %d", itemUID);
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    map_item item = it->second;
    if (item.m_item.m_addInfo == 0)
    {
        // 金币
        if (!GoldPickupItem(user, (unsigned int)itemUID, item))
            return 0;
        return 1;
    }

    CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
    {
        cMyTrace trace(kPretty, 0x57f, 5);
        trace("item not find - item_id : %d, itemUID : %d",
              item.m_item.m_addInfo, itemUID);
        return 0;
    }

    // ORIG 逐虚槽分类（结果仅副作用语义，后续未读取，忠实保留）：
    //   可堆叠 && GetItemType()==9 → 7；否则生物物品 → 7；头像物品 → 1。
    int routeKind = 0;
    if (script->is_stackable())
    {
        if (script->GetItemType() == 9)
            routeKind = 7;
    }
    else
    {
        if (script->IsCreatureItem())
            routeKind = 7;
        else if (script->IsAvatarItem())
            routeKind = 1;
    }
    CParty* party = getParty();
    bool canRoute = party->get_member_count() > 1 &&
                    getLiveMemberCount() > 1 &&
                    item.m_count != 0 &&
                    (script->get_rarity() == 4 ||
                     script->IsRoutingItem());
    if (canRoute)
    {
        if (!RoutingPickupItem(user, (unsigned int)itemUID, item))
            return 0;
        return 1;
    }

    outRarity = (char)script->get_rarity();
    bool canRandom = party->get_member_count() > 1 &&
                     getLiveMemberCount() > 1 &&
                     item.m_count != 0 &&
                     (script->get_rarity() == 1 ||
                      script->get_rarity() == 2 ||
                      script->get_rarity() == 3);
    if (canRandom)
    {
        if (!RandomPickupItem(user, (unsigned int)itemUID, item))
        {
            cMyTrace trace(kPretty, 0x5ac, 5);
            trace("RandomPickupItem false");
            return 0;
        }
    }
    else
    {
        if (!NormalPickupItem(user, (unsigned int)itemUID, item))
        {
            cMyTrace trace(kPretty, 0x5b4, 5);
            trace("NormalPickupItem false");
            return 0;
        }
    }
    return 1;
}

// ---- handleDropItem（ORIG 0x0814ff4a）----
// 丢弃：dropItem 成功 → 取 +0xa34 序号（-1 回执失败包）→ MapInfo::Add_Item →
// 0x28 广播包 → 0x32 成功回执给丢弃者；失败/越限 → 0x32 失败回执。
int CBossStage::handleDropItem(CUser* user, int a, int b, char c, int d, int e)
{
    if (e < 0)
        return 0;

    PacketGuard packet;
    packet.clear();
    map_item item;
    if (!dropItem(user, c, d, e, item))
    {
        packet.put_header(1, 0x32);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.put_byte((int)(signed char)c);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    int seq = m_stageData.m_dropItemCnt;   // ORIG +0xa34（物品序号）
    m_stageData.m_dropItemCnt = seq + 1;
    if (seq == -1)
    {
        packet.put_header(1, 0x32);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.put_byte((int)(signed char)c);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    m_stageData.Add_Item(item);

    packet.put_header(0, 0x28);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(a);
    packet.put_short(b);
    packet.put_short(seq);
    packet.put_int(item.m_item.m_addInfo);
    packet.put_byte((int)item.m_item.GetItemAttr());
    packet.put_int(item.m_item.get_add_info());
    packet.put_short((int)item.m_item.m_fieldb);
    packet.put_int(item.m_item.m_fieldd);
    packet.put_byte((int)item.m_item.m_amp.getAbilityType() & 0xff);
    packet.put_short((int)item.m_item.m_amp.getAbilityValue() & 0xffff);
    packet.put_packet(item.m_item);
    packet.put_byte(item.m_count == 0 ? 1 : 0);
    packet.finalize(true);
    getParty()->send_to_party(packet);

    packet.clear();
    packet.put_header(1, 0x32);
    packet.put_byte(1);
    packet.put_byte((int)(signed char)c);
    packet.put_short(d);
    packet.put_int(e);
    packet.finalize(true);
    user->Send(packet);
    return 1;
}

// ---- handleKillMonster（ORIG 0x08150388）----
// 击杀：killMonsterErase → partyType==1（PowerWar）时 0x26 广播即返回 →
// KillMonsterGetExp → KillMonster 日志 → killMonsterDropItem。
int CBossStage::handleKillMonster(CUser* user, unsigned short a,
                                  unsigned short b, const MSG_MONSTER_DIE& die,
                                  int* out)
{
    (void)out;   // ORIG 未使用第 5 参
    map_monster mob;
    int eraseCnt = 0;
    if (!killMonsterErase(a, mob, eraseCnt))
        return 0;

    CParty* party = getParty();
    if (party->get_party_type() == 1)   // CParty::m_battleField.m_bloodState（PowerWar）
    {
        PacketGuard packet;
        packet.put_header(0, 0x26);
        packet.put_short((int)a);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.finalize(true);
        party->send_to_party(packet);
        return 1;
    }

    unsigned int exp = 0;
    if (!KillMonsterGetExp(mob, exp))
        return 0;

    int curExp = user->getCurCharacExp();
    char roleType = mob.m_roleType;
    // cUserHistoryLog 为 CUser 嵌套成员 m_historyLog（ORIG user+0x79700）
    user->m_historyLog.KillMonster(mob.m_mobIndex,
                      die.m_killValue1,   // +0xa1e（耗时类）
                      die.m_killValue2,   // +0xa1a（经验类）
                      die.m_killPairA24,
                      die.m_killPairA26,
                      die, roleType, roleType == 5, curExp, (int)exp,
                      (eExpAddReason)0xb);

    if (!killMonsterDropItem(mob, eraseCnt, a, b))
        return 0;
    return 1;
}

// ============================================================================
// 非虚方法（逐函数按 docs/class_func_reports/WongWork__CBossStage/ 报告转写）
// 字段速查（均已具名，偏移仅作对照）：
//   m_dungeon(+0x9f0)  m_stageData(MapInfo@+0x9f4；m_mapIndex+0x9f8、
//     m_assignItemIdx+0xa30、m_dropItemCnt+0xa34、m_mapBasisLevel+0xa3c)
//   m_stageCur(+0xae0)  m_stageMax(+0xae4)  m_difficulty(+0xae8)
//   m_expParamA(+0xaec)  基类 CBossPlay::m_useItemCount[4](+0x9c8)
// ============================================================================

// ---- moveNextStage（ORIG 0x0814d1b6）----
// 怪物清空（monsterMap.empty()）且 m_stageCur < m_stageMax（无符号比较）
// 时 m_stageCur+=1 进入下一层。
bool CBossStage::moveNextStage()
{
    if (!m_stageData.m_monsterMap.empty())
        return false;
    if ((unsigned int)m_stageCur < (unsigned int)m_stageMax)
    {
        m_stageCur += 1;
        return true;
    }
    return false;
}

// ---- randomMapIndex（ORIG 0x0814f4fc）----
// acc/roll 先置零；栈上 vector 拷贝后遍历；getRandomTowerMapIndexes 在
// find 与 end 处各调用一次（ORIG 两次外呼）。
bool CBossStage::randomMapIndex(int& index)
{
    int acc = 0;
    int roll = 0;
    std::map<int, std::vector<RandomList> >::const_iterator it;
    std::vector<RandomList> vec;
    int stageCur = m_stageCur;                        // +0xae0
    const std::map<int, std::vector<RandomList> >* towerMaps =
        m_dungeon->getRandomTowerMapIndexes();
    it = towerMaps->find(stageCur);
    const std::map<int, std::vector<RandomList> >* endMaps =
        m_dungeon->getRandomTowerMapIndexes();
    if (it == endMaps->end())
        return false;
    vec = it->second;
    roll = get_rand_int(10000);
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        acc += vec[i].m_weight;
        if (roll < acc)
        {
            index = vec[i].m_mapIndex;
            break;
        }
    }
    return true;
}

// ---- killMonsterErase（ORIG 0x0814d214）----
// find(uid) → const_iterator 判 end() → mob=it->second、outCount=(uchar)+0x14、
// erase(by key) → true；未找到 false。
bool CBossStage::killMonsterErase(unsigned short uidParam, map_monster& mob,
                                  int& outCount)
{
    int uid = uidParam;                           // ORIG 以 int 局部承接（无 ushort 槽）
    std::map<int, map_monster>::const_iterator it =
        m_stageData.m_monsterMap.find(uid);
    if (it != m_stageData.m_monsterMap.end())
    {
        mob = it->second;                             // map_monster::operator=
        outCount = (unsigned char)it->second.m_level;   // ORIG pair+0x14 = mob+0x10
        m_stageData.m_monsterMap.erase(uid);          // by-key erase
        return true;
    }
    return false;
}

// ---- KillMonsterGetExp（ORIG 0x0814d300）----
// 对每名有效队员：get_mob_reward → getMonsterTotalExp → 死亡之塔两层费率 →
// sumGainedEachExp → calcHelpAbuseRatio → gain_exp_sp(+Noti) → CheckQuestMonster。
int CBossStage::KillMonsterGetExp(const map_monster& mob, unsigned int& exp)
{
    CParty* party = getParty();
    CDungeon* dungeon = m_dungeon;

    party->getMemberLevelGap();      // ORIG 副作用调用（填充 m_memberLevelGap 缓存）

    for (int i = 0; i < 4; ++i)
    {
        CUser* user = party->get_user(i);
        if (user == 0 || !party->checkValidUser(i))
            continue;
        if (party->get_party_type() == 1)   // m_battleField.m_bloodState==1（PowerWar）
            continue;

        G_CDataManager()->get_mob_reward(user->get_charac_level(), exp);
        char roleType = mob.m_roleType;           // +0x08 byte
        unsigned int total = party->getMonsterTotalExp(
            exp, roleType, m_expParamA, 1.0f, 1.0f, false);

        // 死亡之塔费率（ORIG x87 fistpll 就近舍入；经验恒正，
        // (double)x+0.5 截断等价）。APC（roleType==5）额外乘 [apc exp weight]
        // （stDeathTower_t+0x00）；所有怪乘 [monster exp weight]（+0xc84）。
        float expF = (float)total;
        stDeathTower_t* tower = G_CDataManager()->GetTowerScript(
            dungeon->get_index());
        if (roleType == 5)
            total = (unsigned int)((double)tower->m_apcExpWeight * expF + 0.5);
        tower = G_CDataManager()->GetTowerScript(dungeon->get_index());
        total = (unsigned int)(
            (double)tower->m_monsterExpWeight * (float)total + 0.5);

        STGainedEachExpData data;                 // ORIG ctor W 0x08151a56
        data.m_exp = total;
        exp = party->sumGainedEachExp(user, data);
        user->calcHelpAbuseRatio(exp, false);

        int reason = (roleType == 5) ? 0xc : 0xb;
        int sp = 0;
        int sfp = 0;
        if (user->gain_exp_sp((int)exp, sp, sfp, (CUser::eExpAddReason)reason,
                              mob.m_mobIndex, false))
            user->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);

        ENUM_QUEST_ENEMY_TYPE questType =
            (roleType > 4 && roleType <= 8) ? (ENUM_QUEST_ENEMY_TYPE)2
                                            : (ENUM_QUEST_ENEMY_TYPE)1;
        user->CheckQuestMonster(dungeon->get_index(), mob.m_mobIndex,
                                questType);
    }
    return 1;
}

// ---- SendMapInfoPacket（ORIG 0x0814ce12）----
int CBossStage::SendMapInfoPacket()
{
    PacketGuard packet;
    packet.put_header(0, 0x1e2);
    packet.put_short(m_stageCur);                          // 当前层
    packet.put_int(m_stageData.m_mapBasisLevel);           // seed 值（ORIG +0xa3c）
    packet.put_short(m_stageData.m_mapIndex);              // 当前图索引

    MapInfo& stage = m_stageData;
    packet.put_byte((int)stage.m_monsterMap.size());
    for (std::map<int, map_monster>::const_iterator it =
             stage.m_monsterMap.begin();
         it != stage.m_monsterMap.end(); ++it)
    {
        map_monster mob(it->second);               // ORIG 栈拷贝后逐字段写包
        packet.put_int(mob.m_mobId);               // +0x00
        packet.put_short((int)(short)mob.m_instId);       // +0x04
        packet.put_short((int)(short)mob.m_mobIndex);     // +0x0c
        packet.put_byte((int)(unsigned char)mob.m_level); // +0x10
        packet.put_byte((int)(signed char)mob.m_roleType);// +0x08
        packet.put_byte((int)(unsigned char)mob.m_field13);
        packet.put_byte((int)(unsigned char)mob.m_field14);
    }

    packet.put_byte((int)stage.m_itemMap.size());
    for (std::map<int, map_item>::const_iterator it = stage.m_itemMap.begin();
         it != stage.m_itemMap.end(); ++it)
    {
        const map_item& item = it->second;
        // +0x01 等级字节（ORIG movzbl+movsbl 后 put_byte：0814d05a）
        packet.put_byte((int)(signed char)item.m_standardLevel);
        packet.put_int(item.m_itemIndex);                  // +0x04
        packet.put_short((int)item.m_item.m_addInfo & 0xffff);  // +0x12 word
        packet.put_int(item.m_item.get_add_info());        // Inven_Item+7
        packet.put_byte((int)item.m_item.m_amp.getAbilityType() & 0xff);
        packet.put_short((int)item.m_item.m_amp.getAbilityValue() & 0xffff);
    }
    packet.finalize(true);
    getParty()->send_to_party(packet);
    return 1;
}


// ---- GoldPickupItem（ORIG 0x0814f0e6）----
// 金币按存活队员平分（拾取者拿余数），逐槽写 0x27 包并 erase 掉落物。
bool CBossStage::GoldPickupItem(CUser* user, unsigned int uid,
                                const map_item& item)
{
    int liveCnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (isMemberAlive(i))
            ++liveCnt;
    }

    PacketGuard packet;
    packet.clear();
    if (liveCnt == 0)
    {
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }

    int gold = item.m_item.get_add_info();        // map_item+0x10 Inven_Item
    int share = gold / liveCnt;
    int rem = gold % liveCnt;
    int bonus = share + rem;                      // 拾取者份额

    packet.put_header(0, 0x27);
    packet.put_short((int)uid);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    CParty* party = getParty();
    for (int i = 0; i < 4; ++i)
    {
        CUser* member = party->get_user(i);
        bool valid = (member != 0) && party->checkValidUser(i);
        if (valid)
        {
            packet.put_byte(1);
            int amount = (member == user) ? bonus : share;
            int gained = member->getCurCharacInvenW()->gain_money(
                amount, (eMoneyAddReason)4, true, 0);
            if (gained < amount)
            {
                if (gained < 0)
                    gained = 0;
                member->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0,
                                            (unsigned int)amount,
                                            (unsigned int)gained);
            }
            packet.put_int(gained);
            packet.put_byte(1);
            packet.put_int(0);
            packet.put_int(0);
        }
        else
        {
            packet.put_byte(0);
            packet.put_int(0);
        }
    }
    m_stageData.m_itemMap.erase((int)uid);
    packet.finalize(true);
    party->send_to_party(packet);
    return true;
}

// ---- RoutingPickupItem（ORIG 0x0814e57a）----
bool CBossStage::RoutingPickupItem(CUser* user, unsigned int uid,
                                   map_item& item)
{
    CSpecialItemRoutingManager* mgr =
        G_CGameManager()->GetSpecialItemRoutingManager();

    int ret = 0;
    ret = mgr->StartRouting(user, item.m_item, uid);
    bool failed = (ret != 0);
    if (failed)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }
    return true;
}


namespace
{

// Random/Normal 共用的展示分类（ORIG 逐虚槽判定，仅写入包字段）：
// 可堆叠 && GetItemType()==9 → 7；生物物品 → 7；头像物品 → 1。
int pickupRouteKind(const CItem* script)
{
    if (script->is_stackable())
    {
        if (script->GetItemType() == 9)
            return 7;
    }
    else
    {
        if (script->IsCreatureItem())
            return 7;
        else if (script->IsAvatarItem())
            return 1;
    }
    return 0;
}

}  // namespace

// ---- NormalPickupItem（ORIG 0x0814ecec）----
bool CBossStage::NormalPickupItem(CUser* user, unsigned int uid,
                                  const map_item& item)
{
    if (user == 0)
        return false;
    const CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
        return false;

    int routeKind = pickupRouteKind(script);
    int slot = -1;
    if (!item.m_item.isAvatarItemType())
    {
        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item.m_item, (eItemAddReason)4, true, true);
    }
    else
    {
        char agency[24];
        memset(agency, 0, sizeof(agency));
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
            (unsigned int)user->getCurCharacNo(), agency);
        // 第三参取 map_item+0x1b = m_item.m_fieldb 低字节（ORIG movzwl+movsbl）
        char periodByte = (char)(unsigned char)item.m_item.m_fieldb;
        slot = user->addAvatarItem((unsigned long)item.m_item.m_addInfo, 0,
                                   periodByte, agency,
                                   (eAvatarItemAddReason)0x10);
    }
    if (slot == -1)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }

    m_stageData.m_itemMap.erase((int)uid);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x27);
    packet.put_short((int)uid);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    for (int i = 0; i < 4; ++i)
        packet.put_byte(0);                        // 无掷点
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    packet.put_short(slot);
    packet.put_byte(routeKind);
    packet.finalize(true);
    getParty()->send_to_party(packet);
    return true;
}

// ---- RandomPickupItem（ORIG 0x0814e684）----
// 存活且背包可容纳的成员各掷 1..100，最高者获得物品；广播含四人掷点。
bool CBossStage::RandomPickupItem(CUser* user, unsigned int uid,
                                  const map_item& item)
{
    char rolls[4] = { 0, 0, 0, 0 };
    int bestRoll = 0;
    CUser* winner = 0;
    CParty* party = getParty();
    for (int i = 0; i < 4; ++i)
    {
        CUser* member = party->get_user(i);
        bool valid = (member != 0) && party->checkValidUser(i);
        if (!valid)
            continue;
        int trySlot =
            member->getCurCharacInvenR()->tryInsertItemIntoInventory(item.m_item);
        if (trySlot != -1 && isMemberAlive(i))
        {
            rolls[i] = (char)(get_rand_int(100) + 1);
            if (bestRoll < (int)rolls[i])
            {
                bestRoll = (int)rolls[i];
                winner = member;
            }
        }
        else
        {
            rolls[i] = 0;
        }
    }

    if (winner == 0)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }

    static const char kPretty[] =
        "bool WongWork::CBossStage::RandomPickupItem(CUser*, unsigned int, const map_item&)";
    const CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
    {
        cMyTrace trace(kPretty, 0x39c, 5);
        trace("item not find : %d", item.m_item.m_addInfo);
        return false;
    }

    int routeKind = pickupRouteKind(script);
    int slot = -1;
    if (!item.m_item.isAvatarItemType())
    {
        slot = winner->getCurCharacInvenW()->insertItemIntoInventory(
            item.m_item, (eItemAddReason)4, true, true);
    }
    else
    {
        char agency[24];
        memset(agency, 0, sizeof(agency));
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
            (unsigned int)winner->getCurCharacNo(), agency);
        char periodByte = (char)(unsigned char)item.m_item.m_fieldb;
        slot = winner->addAvatarItem((unsigned long)item.m_item.m_addInfo, 0,
                                     periodByte, agency,
                                     (eAvatarItemAddReason)0x10);
    }
    if (slot == -1)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x2e);
        packet.put_byte(0);
        packet.put_byte(4);
        packet.finalize(true);
        winner->Send(packet);
        return false;
    }

    m_stageData.m_itemMap.erase((int)uid);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x27);
    packet.put_short((int)uid);
    packet.put_short((int)(user->get_unique_id() & 0xffff));
    for (int i = 0; i < 4; ++i)
        packet.put_byte((int)rolls[i]);
    packet.put_short((int)(winner->get_unique_id() & 0xffff));
    packet.put_short(slot);
    packet.put_byte(routeKind);
    packet.finalize(true);
    party->send_to_party(packet);
    return true;
}

// ---- dropItem（ORIG 0x0814de1a）----
// 按 invenType 取出物品构建掉落 map_item，再从原容器删除并记流通统计。
// type：0=背包/金币、2=仓库、3=装备栏、7=造物。
bool CBossStage::dropItem(CUser* user, char type, int slot, int count,
                          map_item& out)
{
    Inven_Item tmp;                               // ORIG 栈 Inven_Item
    if (type == 2)
    {
        tmp = user->getCurCharacCargoR()->get_cargo_slot(slot);
    }
    else if (type == 0)
    {
        if (slot != 0)
            tmp = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, slot);
        // slot==0：金币丢弃——输出置空模板后直接走删除段
    }
    else if (type == 3)
    {
        tmp = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)0, slot);
    }
    else if (type == 7)
    {
        tmp = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)3, slot);
        if ((unsigned char)tmp.m_field1 == 5)     // ORIG +1 字节==5 拒绝
            return false;
    }
    else
    {
        return false;
    }

    int sellPrice = 0;                            // ORIG local_20
    bool moneyOnly = false;                       // 金币输出分支
    if (type == 0 && slot == 0)
    {
        moneyOnly = true;
    }
    else if (*(int*)(&tmp.m_addInfo) == 0)        // dword@tmp+2 == 0 → 纯金币
    {
        moneyOnly = true;
    }

    if (!moneyOnly)
    {
        const CItem* script =
            G_CDataManager()->find_item(tmp.m_addInfo);
        if (script == 0)
            return false;

        int attachType = script->GetAttachType();
        if (attachType == 1 || attachType == 2 || attachType == 5)
            return false;
        if (attachType == 3 && *(int*)(&tmp.m_addInfo) != 1)
            return false;

        if (tmp.isEquipableItemType())
        {
            // ORIG 读装备对象 +0x234 int = CEquipItem::m_itemType（CEquipItem.h）
            const CEquipItem* equip =
                static_cast<const CEquipItem*>(script);
            if (equip->m_itemType == 0xb || equip->m_itemType < 10)
                return false;
        }

        // 构建 out（逐字段按 asm 宽度；out 由调用方默认构造已清零）
        memset(&out, 0, sizeof(out));
        out.m_item.m_field0 = 0;                          // byte +0x00 = 0
        out.m_item.m_field1 = tmp.m_field1;               // byte +0x11
        out.m_item.m_addInfo = tmp.m_addInfo;             // dword +0x12
        out.m_item.m_fieldd = tmp.m_fieldd;               // dword +0x1d
        out.m_item.m_amp = tmp.m_amp;                     // +0x21..+0x2e 整体拷贝（14 字节 POD）
        if (!tmp.isEquipableItemType())
            out.m_item.set_add_info(count);               // 非装备用丢弃数量
        else
            out.m_item.set_add_info(tmp.m_addInfo2);      // 装备沿用原 add_info2
        out.m_item.SetItemAttr(tmp.GetItemAttr());
        out.m_item.m_fieldb = tmp.m_fieldb;               // word +0x1b
        out.m_item.m_field0 = tmp.m_field0;               // byte +0x10
        G_Store()->GetSellItemPrice(
            const_cast<Inven_Item&>(tmp), script, (short)count, false,
            sellPrice);
    }
    else
    {
        memset(&out, 0, sizeof(out));
        out.m_item.m_field0 = 0;                          // byte +0x00 = 0
        out.m_item.m_field1 = 2;                          // byte +0x11 = 2
        out.m_item.m_addInfo = 0;                         // dword +0x12 = 0
        out.m_item.m_fieldb = 0;                          // word +0x1b = 0
        out.m_item.set_add_info(count);
        out.m_item.ResetItemAttr();
    }

    // 删除段
    char deleted = 0;
    if (type == 2)
    {
        deleted = (char)(user->getCurCharacCargoW()->delete_item(
            slot, count, (eItemDelReason)0x10) ? 1 : 0);
    }
    else if (type == 0)
    {
        CInventory* invenW = user->getCurCharacInvenW();
        if (slot == 0)
            deleted = (char)(invenW->use_money(
                count, (eMoneySubReason)6, true) ? 1 : 0);
        else
            deleted = (char)(invenW->delete_item(
                (INVEN_TYPE)1, slot, count, (eItemDelReason)6, true) ? 1 : 0);
    }
    else if (type == 3)
    {
        deleted = (char)(user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)0, slot, 1, (eItemDelReason)6, true) ? 1 : 0);
    }
    else if (type == 7)
    {
        deleted = (char)(user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)3, slot, 1, (eItemDelReason)6, true) ? 1 : 0);
    }
    else
    {
        return false;
    }

    if (deleted == 1)
    {
        if (type == 0 && slot == 0)
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x17, user, (unsigned int)count);
        else
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        return true;
    }
    return false;
}

// ---- killMonsterDropItem（ORIG 0x0814d628）----
// 怪物/APC 掉落生成 → 逐件构建 map_item（count=1、dropIndex=b、
// createTick=当前秒）入 MapInfo → 0x26 包广播。
bool CBossStage::killMonsterDropItem(const map_monster& mob, int eraseCnt,
                                     unsigned short a, unsigned short b)
{
    std::vector<unsigned long> specifyDrops;      // 塔指定掉落（ORIG 收集后未再读取）
    specifyDrops.clear();
    std::list<map_item> newItems;
    newItems.clear();
    stGenerateResult_t result;
    result.m_dropItems.clear();

    CDungeon* dungeon = m_dungeon;
    CDataManager* dm = G_CDataManager();
    if ((unsigned char)mob.m_roleType <= 4)
    {
        CMonster* mon =
            (CMonster*)dm->find_monster(mob.m_mobIndex);
        if (mon != 0)
        {
            stGenerateRefData_t ref;
            ref.m_field34 = (char)eraseCnt;        // +0x34（DepB 具名化）
            ref.m_rateSlot = (char)m_difficulty;
            ref.m_goldFactor = (unsigned short)((ref.m_goldFactor & 0xff00) | (getParty()->get_member_count() & 0xff));  // +0x3e 字节写（保 +0x3f 等级上界）
            mon->generateIndependentItem(ref, result.m_dropItems);
        }
    }
    else
    {
        CAICharacterList* aiList = dm->m_aiCharacterList;
        CAICharacter* ai = aiList->get(mob.m_mobIndex);
        if (ai != 0)
        {
            if (dungeon->hasTowerItemDrop())
                ai->deathTowerSpecifyItemDrop(specifyDrops);
            stGenerateRefData_t ref;
            ref.m_field34 = (char)eraseCnt;
            ref.m_rateSlot = (char)m_difficulty;
            ref.m_goldFactor = (unsigned short)((ref.m_goldFactor & 0xff00) | (getParty()->get_member_count() & 0xff));  // +0x3e 字节写（保 +0x3f 等级上界）
            ai->generateIndependentItem(ref, result.m_dropItems);
        }
    }

    // 生成结果逐件入图（ORIG：byte count=1、word +8=b、dword +0xc=当前秒）
    for (std::vector<Inven_Item>::const_iterator it =
             result.m_dropItems.begin();
         it != result.m_dropItems.end(); ++it)
    {
        map_item mi;
        mi.m_count = 1;                    // ORIG 仅写低字节，ctor 已清零等价
        mi.m_dropIndex = b;
        mi.m_createTick =
            (int)GlobalData::s_systemTime_.getCurSec();
        mi.m_itemIndex = m_stageData.m_dropItemCnt;
        m_stageData.m_dropItemCnt += 1;
        memcpy(&mi.m_item, &*it, sizeof(Inven_Item));
        m_stageData.Add_Item(mi);
    }

    PacketGuard packet;
    packet.put_header(0, 0x26);
    packet.put_short((int)a);
    int countIdx = packet.get_index();     // 物品计数回填位置
    int itemCount = 0;
    packet.put_byte(0);
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    for (std::list<map_item>::const_iterator it = newItems.begin();
         it != newItems.end(); ++it)
    {
        const map_item& item = *it;
        packet.put_short(item.m_itemIndex);              // +0x04
        packet.put_int(item.m_item.m_addInfo);           // +0x12 dword
        packet.put_byte((int)item.m_item.GetItemAttr() & 0xff);
        if (!item.m_item.isAvatarItemType())
        {
            packet.put_int(item.m_item.get_add_info());
        }
        else
        {
            CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(
                item.m_item.m_addInfo);
            if (equip == 0)
            {
                packet.put_int(item.m_item.get_add_info());
            }
            else
            {
                int period = equip->getAvatarPeriod(
                    (unsigned char)item.m_item.m_fieldb);
                packet.put_int(period * 0x15180);
                emblem.init();
                equip->getAvatarSocket((int)item.m_item.m_fieldb, emblem);
            }
        }
        packet.put_short((int)item.m_item.m_fieldb);     // word @+0x1b
        packet.put_byte((int)item.m_item.m_amp.getAbilityType() & 0xff);
        packet.put_short((int)item.m_item.m_amp.getAbilityValue() & 0xffff);
        packet.put_packet(item.m_item);
        if (!item.m_item.isAvatarItemType())
        {
            packet.put_byte(0);
        }
        else
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            packet.put_binary(reinterpret_cast<char*>(&emblem), 0x1e);
        }
        packet.put_short((int)item.m_dropIndex);         // word @+0x08
        ++itemCount;
    }
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(countIdx, itemCount);   // 回填物品数
    packet.finalize(true);
    getParty()->send_to_party(packet);
    return true;
}

// ---- consistMap（ORIG 0x0814bef4）----
// 按源图布怪：AI 布点 → AP 随机怪（预算扣减）→ 脚本怪列表 → 特殊被动物品。
// 任一 AI 查询失败返回 false。map_monster 具名字段映射：
//   m_field11 半字 = ORIG +0x11/+0x12 两字节写、m_field13/m_field14 同名、
//   m_dropCnt(+0x2c) 收布点/脚本怪掉落数。
bool CBossStage::consistMap(const CMap* map)
{
    m_stageData.m_mapIndex = map->get_index();

    CDataManager* dm = G_CDataManager();
    std::vector<unsigned int> usedAIs;
    // ORIG：std::map::operator= 从 CAICharacterList::getAPCount() 结果拷贝
    std::map<int, int> apStat(dm->m_aiCharacterList->getAPCount());
    int counter = 0;

    // ---- 第一段：图上 AI 布点（STAICharacterMapArrangeData 向量）----
    const std::vector<STAICharacterMapArrangeData>* arranges =
        map->getAICharacters();
    if (!arranges->empty())
    {
        counter = 0;
        for (std::vector<STAICharacterMapArrangeData>::const_iterator it =
                 arranges->begin();
             it != arranges->end(); ++it)
        {
            unsigned int aiType = (unsigned int)it->m_type;
            CAICharacter* ai =
                dm->m_aiCharacterList->get(aiType);
            if (ai == 0)
                return false;

            usedAIs.push_back((unsigned int)ai->getIdx());
            int ap = ai->getAppearancePoint();
            if (ap != 0)
            {
                std::map<int, int>::iterator st = apStat.find(ap);
                if (st != apStat.end())
                    st->second = std::max(st->second - 1, 0);
            }

            map_monster mob;
            memset(&mob, 0, sizeof(mob));
            mob.m_mobId = counter;                            // +0x00 序号
            mob.m_mobIndex = (int)aiType;                     // +0x0c
            mob.m_instId = m_stageData.m_assignItemIdx;       // +0x04 seq++
            m_stageData.m_assignItemIdx += 1;
            mob.m_roleType = 5;                               // APC
            mob.m_level = (char)(unsigned char)ai->getLevel();
            mob.m_dropCnt = it->m_camp;                       // +0x10 dword（STMapScript 批次命名 m_camp；
                                                              //   本调用面按等级消费，语义双证待裁决）
            m_stageData.Add_Mob(mob);                         // ctor 已清 +0x11/+0x12
            ++counter;
        }
    }

    // ---- 第二段：AP 随机怪（剩余点数预算扣减）----
    int remainAP = map->getAPCRandomPoint();
    if (remainAP != 0)
    {
        counter = 0x40;
        while (true)
        {
            if (remainAP < dm->m_aiCharacterList->getMinAP())
                break;
            CAICharacter* ai = dm->m_aiCharacterList->getByAP(
                (unsigned int)(get_rand_int(remainAP) + 1));
            if (ai == 0)
                return false;

            int aiIdx = ai->getIdx();
            bool alreadyUsed =
                std::find(usedAIs.begin(), usedAIs.end(),
                          (unsigned int)aiIdx) != usedAIs.end();
            if (!alreadyUsed)
            {
                usedAIs.push_back((unsigned int)aiIdx);
                int ap = ai->getAppearancePoint();
                std::map<int, int>::iterator st = apStat.find(ap);
                if (st != apStat.end())
                    st->second = std::max(st->second - 1, 0);
                remainAP -= ap;

                map_monster mob;
                memset(&mob, 0, sizeof(mob));
                mob.m_mobId = counter;
                mob.m_mobIndex = aiIdx;
                mob.m_instId = m_stageData.m_assignItemIdx;
                m_stageData.m_assignItemIdx += 1;
                mob.m_roleType = 5;
                mob.m_level = (char)(unsigned char)ai->getLevel();
                mob.m_dropCnt = 100;                          // ORIG 立即数 100
                m_stageData.Add_Mob(mob);
                ++counter;
            }
            else
            {
                // 已用过的 AI：重算 ≤ 剩余点数的可用 AP 总量，为 0 则退出
                int avail = 0;
                for (std::map<int, int>::const_iterator st = apStat.begin();
                     st != apStat.upper_bound(remainAP); ++st)
                    avail += st->second;
                if (avail == 0)
                    break;
            }
        }
    }

    // seed 值（ORIG get_rand_int(0) 后清 bit 0x40000）
    m_stageData.m_mapBasisLevel = get_rand_int(0);
    *(unsigned int*)&m_stageData.m_mapBasisLevel &= 0xfffbffffu;

    std::vector<int> singleSpawnSeqs;   // 非 named 单人刷怪序号表

    // ---- 第三段：脚本怪列表 ----
    counter = 0;
    const std::list<_mapMonster> mobs = map->get_monster_list();
    for (std::list<_mapMonster>::const_iterator it = mobs.begin();
         it != mobs.end(); ++it)
    {
        const _mapMonster& em = *it;
        map_monster mob;
        memset(&mob, 0, sizeof(mob));
        mob.m_mobId = counter;

        // 等级：em.m_flag04==0 直接取 em.m_field08；否则 标准等级+m_field08，
        // 字节域夹取 [1..99]
        unsigned char lv;
        if ((char)em.m_flag04 == 0)
            lv = (unsigned char)(char)em.m_field08;
        else
        {
            lv = (unsigned char)(m_dungeon->get_standard_level() +
                 (int)(signed char)em.m_field08);
            if (lv == 0)
                lv = 1;
            else if (lv > 99)
                lv = 99;
        }
        mob.m_level = (char)lv;

        if (em.m_mobId == -1)
            mob.m_mobIndex =
                dm->m_monsterManager->get_levelrandom_mob((int)lv);
        else
            mob.m_mobIndex = em.m_mobId;

        // ORIG 分别写 +0x11/+0x12 两字节 = m_field11 半字（小端拼装）
        mob.m_field11 = (short)(
            ((unsigned short)(unsigned char)(char)em.m_levelAdd) |
            ((unsigned short)(unsigned char)(char)em.m_field14 << 8));
        mob.m_roleType = (char)(unsigned char)em.m_roleType;
        mob.m_instId = m_stageData.m_assignItemIdx;
        m_stageData.m_assignItemIdx += 1;

        if ((signed char)mob.m_roleType == 0 && !mob.isNamedMonster())
            singleSpawnSeqs.push_back(mob.m_instId);

        mob.m_field13 = (char)(unsigned char)em.m_champion;   // em +0x1c
        // em +0x20 低字节（_mapMonster::m_posX 首字节；CBattle_Field 同型转写
        // BattleMonsterDepsC2.cpp mob.m_field14 ← ref.m_posX）
        mob.m_field14 = (char)(unsigned char)em.m_posX;
        mob.m_dropCnt = em.m_hpPct;                           // em +0x28
        m_stageData.Add_Mob(mob);
        ++counter;
    }

    // ---- 第四段：特殊被动物品生成 ----
    std::vector<stDungeonAssignItem_t>* specialItems =
        m_dungeon->getSpecialPassiveObjectItems();
    // ORIG 同时取出 getObjectDropItems() 但未使用（忠实保留调用）
    m_dungeon->getObjectDropItems();
    const std::list<_mapItem> items = map->get_item_list();

    stGenerateRefData_t refGen1;
    stGenerateRefData_t refGen4;
    std::list<_mapItem>::const_iterator it = items.begin();
    while (it != items.end())
    {
        const _mapItem& mi = *it;
        const stDungeonAssignItem_t& assign =
            specialItems->at((unsigned int)mi.m_objectIdx);

        unsigned char genLevel;
        if (assign.m_assignItemIdx != -1)
        {
            genLevel = (unsigned char)(char)assign.m_assignItemIdx;
        }
        else
        {
            int sl = m_dungeon->get_standard_level();
            if (sl <= 0 || sl > 200)
            {
                // ORIG 该路径不推进迭代器直接回到循环判定——标准等级恒在域内
                // 时不可达；为防重建服务死循环改为跳过并注释差异。
                ++it;
                continue;
            }
            genLevel = (unsigned char)(char)sl;
        }
        (void)genLevel;   // ORIG 计算后未再读取（忠实保留）

        stGenerateResult_t result;
        // ORIG consistMap（0x0814bef4 +0xb54..+0xb5a）此处把 mi.m_field04 低字节
        // 写入独立栈标量 -0xb2f(%ebp)——位于 -0xb20(%ebp) 的 result 对象【之外】，
        // 写后全函数无任何读取（Ghidra local_b33 死存储；stGenerateResult_t 布局
        // vector(0xc)+CLuckPoint(0x24)=0x30 已完整覆盖，不存在 +0x0f 成员；
        // +0x0f 落在 CLuckPoint::m_users._M_start 指针内部，不可字段化）。
        // 忠实转写 = 删除该死存储；不得向 result+0x0f 写入（会破坏 vector 头）。

        WongWork::CItemGeneratorMgr* genMgr =
            G_CDataManager()->getItemGenerator();
        genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateSpecificItem(refGen1, result);
        genMgr->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
            ->generateSpecificItem(refGen4, result);

        for (std::vector<Inven_Item>::iterator d = result.m_dropItems.begin();
             d != result.m_dropItems.end(); ++d)
        {
            map_item drop;
            drop.m_count = 1;              // ORIG movb $1（0830b5ab 同型）
            // ORIG 写 map_item+0x01 等级字节：add_item_curr_map 08302f84 /
            // consistMap 0814c9da 同型（来源 _mapItem.m_field0c 或标准等级）
            drop.m_standardLevel = (char)mi.m_field0c;
            drop.m_itemIndex = m_stageData.m_dropItemCnt;
            m_stageData.m_dropItemCnt += 1;
            memcpy(&drop.m_item, &*d, sizeof(Inven_Item));
            m_stageData.Add_Item(drop);
        }
        ++it;
    }
    return true;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0814f426（docs/class_func_reports/WongWork__CBossStage/
// RandomConsistMap.md 全文，GlobalBossAndAltar 2026-08-24）：
// randomMapIndex 失败 → false；GM 且 m_gmBossMapIndex(+0x796d0) 非空时强制该图并清零；
// find_map 空/consistMap 失败 → false；成功记 m_startTick(+0x9e8) 并返回 true。
// ----------------------------------------------------------------------------
bool CBossStage::RandomConsistMap(CUser& user)
{
    int mapIdx = 0;
    if (randomMapIndex(mapIdx) != true)
        return false;


    if (user.isGMUser() && user.m_gmBossMapIndex != 0)
    {
        mapIdx = user.m_gmBossMapIndex;
        user.m_gmBossMapIndex = 0;
    }

    CMap* map = static_cast<CMap*>(G_CDataManager()->find_map(mapIdx));
    const bool failed = (map == 0) || !consistMap(map);
    if (failed)
        return false;
    // ORIG this+0x9e8 = m_startTick
    m_startTick = (int)GlobalData::s_systemTime_.getCurTickCount();
    return true;
}

}  // namespace WongWork