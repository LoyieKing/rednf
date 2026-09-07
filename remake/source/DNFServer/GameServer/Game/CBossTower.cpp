// df_game_r Game/ WongWork::CBossTower 实现（CBossTowerRetry 批次）。
// 全部函数按 docs/class_func_reports/WongWork__CBossTower/ 对应 md 的
// 汇编 + 反编译 C 转写。裸偏移已字段化：
//   基类段（CBossStage，protected）：+0x9e8→m_startTick、+0x9ec→m_endTick、
//     +0x9f0→m_dungeon、+0xa00/+0xa14→m_stageData.m_monsterMap(_M_node_count)、
//     +0xae0→m_stageCur、+0xae4→m_stageMax、+0xae8→m_difficulty；
//   CBossTower 自有段：+0xaf4→m_towerStartTick、+0xaf8→m_towerEndTick、
//     +0xafc→m_prepareFinishFlag、+0xb00→m_dungeonLog；
//   跨类：party+0xcd8→CParty::m_battleField.m_bloodState、user+0x79700→CUser::m_historyLog、
//     manager+0x796cc→CUser::m_gmBossDifficulty。
// getIdx 0x082aa728 读 +0xaf0；setIdx 0x082aa716 写 +0xaf0。
// 虚表 _ZTVN8WongWork10CBossTowerE@0x08b70260 共 11 槽（vptr 相对偏移：
// +0x08=_checkMemberDie、+0x0c=checkAllMemberReady、+0x14=handleLeaveUser）。
#include "CBossTower.h"

#include <algorithm>   // std::min（generateRewardItem 上限链）
#include <cstring>

#include "CParty.h"     // get_user/get_member_count/getMemberNames/m_battleField.m_bloodState(+0xcd8)
#include "CUser.h"      // SendCmdErrorPacket/m_historyLog(+0x79700)/m_gmBossDifficulty
#include "QuickParty.h" // 先于 CDungeon.h：QUICKPARTY_H 已定义则跳过其镜像块
#include "CDungeon.h"   // GetDungeonName/getRandomTowerMaxStage/get_min_level 等
#include "CInventory.h" // gain_money/insertItemIntoInventory/MakeItemPacket/
                        // GetInvenTypeFromItemSpace/eMoneyAddReason/eItemAddReason
#include "CEquipItem.h" // get_endurance（InsertRewardItems 邮件分支）
#include "CGenUniqueNo.h"            // WongWork::CGenUniqueNo::genIPGNo
#include "CMailBoxHelper.h"          // ReqDBSendNewSystemMail/ReqDBSendNewAvatarMail
#include "RDARScriptStringManager.h" // g_scriptStringManager_（邮件文案）

#include "GlobalData.h"      // s_systemTime_
#include "CSystemTime.h"     // getCurTickCount / getCurSec
#include "TimerQueue.h"      // G_TimerQueue / TimerEntry::OBJ_TYPE / TIMER_MESSAGE
#include "CGameManager.h"    // G_CGameManager()->returnBossTower
#include "cHistoryTrace.h"   // 权威声明（原局部最小声明已删除）
#include "PacketGuard.h"        // PacketGuard（回包构造）
#include "CDungeonEntranceLog.h" // GetInstanceDungeonEntranceLog / IncrementDungeonEntrance
#include "CDataManager.h"       // find_dungeon/find_item/get_mob_reward/GetTowerScript
#include "GameTypes.h"          // cMyTrace


namespace WongWork
{

int CBossTower::getIdx()
{
    return m_idx;   // ORIG 0x082aa728：return *(int*)(this+0xaf0)
}

void CBossTower::setIdx(int idx)
{
    m_idx = idx;    // ORIG 0x082aa716：*(int*)(this+0xaf0)=idx
}

// ============================================================================
// 虚函数实现（WongWork::CBossTower）
// ============================================================================

// ORIG T 0x08143a62：
//   user==0 → 0；CBossPlay::handleDieUser 非 1 → 0；虚调 _checkMemberDie()
//   （ORIG call *(vptr+8)）为真时 onFinishStage()；m_prepareFinishFlag(+0xafc)
//   != 1 时 _onPrepareFinishTower(false)；返回 1。
int CBossTower::handleDieUser(CUser* user)
{
    if (user == 0)
        return 0;
    if (!CBossPlay::handleDieUser(user))
        return 0;
    if (_checkMemberDie())
    {
        onFinishStage();
        if (m_prepareFinishFlag != 1)
            _onPrepareFinishTower(false);
    }
    return 1;
}

// ORIG T 0x08143ae2：
//   user==0 → 0；CBossPlay::handleLeaveUser 为假 → 0；
//   记 LeaveDungeon 历史（dungeon=m_dungeon(+0x9f0)、stage=m_difficulty(+0xae8)、
//   成员名表、partyType 低字节）；trace "member_count : %d"；
//   无成员 → _destroy 并返回 0；（party 空或全员死）→ onFinishStage +
//   _onPrepareFinishTower(false)；否则按人数 switch 写 m_difficulty(+0xae8)
//   （1..4 → 0..3）；返回 1。
int CBossTower::handleLeaveUser(CUser* user)
{
    if (user == 0)
        return 0;
    if (!CBossPlay::handleLeaveUser(user))
        return 0;

    char memberNames[255] = "";               // ebp-0x11d（内联展开清零 0xff）

    const char partyTypeByte =
        static_cast<char>(m_party->m_battleField.m_bloodState);   // ORIG 读 +0xcd8 低字节
    m_party->getMemberNames(memberNames);

    user->m_historyLog.LeaveDungeon(m_dungeon->GetDungeonName(), m_difficulty,
                                    memberNames,
                                    static_cast<int>(partyTypeByte));

    cHistoryTrace histTrace;
    histTrace("member_count : %d", m_party->get_member_count());

    if (m_party == 0 || m_party->get_member_count() < 1)
    {
        _destroy();
        return 0;
    }

    bool finishNow = false;
    if (m_party == 0)
        finishNow = true;
    else if (m_party->get_member_count() > 0 && _checkMemberDie())
        finishNow = true;

    if (finishNow)
    {
        onFinishStage();
        _onPrepareFinishTower(false);
    }
    else
    {
        switch (m_party->get_member_count())
        {
        case 1:
            m_difficulty = 0;
            break;
        case 2:
            m_difficulty = 1;
            break;
        case 3:
            m_difficulty = 2;
            break;
        case 4:
            m_difficulty = 3;
            break;
        default:
            break;
        }
    }
    return 1;
}

// ORIG T 0x08143996：
//   CBossStage::handleMoveMap 为假 → 0；否则对 4 个槽位中存在且存活的成员
//   调 m_dungeonLog(+0xb00).IncrementBossStageStart(user, (uchar)m_stageCur,
//   (uchar)m_difficulty)；返回 1。
int CBossTower::handleMoveMap(unsigned short* mapIndex)
{
    // X1 形（XOR_FAMILY.tsv）+ 落点岛（技巧 24）：ORIG @0x…handleMoveMapEPt 是
    // `call CBossStage::handleMoveMap; xor $0x1,%eax; test %al,%al; je BODY;
    //  mov $0x0,%eax; jmp end; BODY: …` —— **return 0 走 fall-through、
    //  成功路径跳走**。被调返回 int，4.4 对 `if (!intcall())` 恒产两段
    //  `test;sete`，必须用绑 %eax 的具名 unsigned char + 显式 `^= 1`。
    register unsigned char v asm("eax") = (unsigned char)CBossStage::handleMoveMap(mapIndex);
    v ^= 1;
    if (!v) goto BODY;
    return 0;
BODY:
    for (int i = 0; i < 4; ++i)
    {
        CUser* member = m_party->get_user(i);
        if (member != 0 && isMemberAlive(i))
        {
            m_dungeonLog.IncrementBossStageStart(
                *member,
                static_cast<unsigned char>(m_stageCur),
                static_cast<unsigned char>(m_difficulty));
        }
    }
    return 1;
}

// ORIG T 0x08143e5e：转发 CBossStage::handleDropItem，结果 ==1 → 1 否则 0。
int CBossTower::handleDropItem(CUser* user, int a, int b, char c, int d, int e)
{
    return CBossStage::handleDropItem(user, a, b, c, d, e) ? 1 : 0;
}

// ORIG T 0x08143ebc：
//   CBossStage::handleKillMonster 为假 → 0；怪物清空检查：ORIG 对 this+0xa00
//   的 std::map<int,map_monster> 取 empty()（_M_node_count@map+0x14 即
//   this+0xa14），为空时 onFinishStage()；且 (uint)m_stageMax(+0xae4) <=
//   (uint)m_stageCur(+0xae0) 时 _onPrepareFinishTower(true)；返回 1。
int CBossTower::handleKillMonster(CUser* user, unsigned short a, unsigned short b,
                                  const MSG_MONSTER_DIE& die, int* d)
{
    if (!CBossStage::handleKillMonster(user, a, b, die, d))
        return 0;
    if (m_stageData.m_monsterMap.empty())
    {
        onFinishStage();
        if (static_cast<unsigned int>(m_stageCur) >=
            static_cast<unsigned int>(m_stageMax))
        {
            _onPrepareFinishTower(true);
        }
    }
    return 1;
}

// ORIG T 0x08143d2c（新增虚槽：char 按值版，与 CBossStage 的 char& 版并存）：
//   party 非空且 IsRoutingState() → SendCmdErrorPacket(0x2e,0xd8)，返回 0；
//   经由 CBossStage::handlePickupItem(char& out_rarity) 拾取失败 → 0；
//   server_rarity != client 参数时 trace（ORIG __PRETTY_FUNCTION__ 文本，
//   600,5）；user 非空且 rarity==3 时 m_dungeonLog.IncrementBossPickUpItem
//   (*user, (uchar)m_stageCur, (uchar)m_difficulty)；返回 1。
int CBossTower::handlePickupItem(CUser* user, int a, bool b, char c)
{
    bool routingBlocked = true;
    if (m_party != 0)
        routingBlocked = m_party->IsRoutingState();
    if (routingBlocked)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x2e), 0xd8);
        return 0;
    }

    char serverRarity = 0;   // ebp-0x1d
    if (!CBossStage::handlePickupItem(user, a, b, serverRarity))
        return 0;

    if (serverRarity != c)
    {
        cMyTrace trace("virtual bool WongWork::CBossTower::handlePickupItem(CUser*, "
                       "int, bool, char)", 600, 5);
        trace("server_item_rarity != client_item_rarity");
    }

    if (user != 0 && c == 3)
    {
        m_dungeonLog.IncrementBossPickUpItem(
            *user,
            static_cast<unsigned char>(m_stageCur),
            static_cast<unsigned char>(m_difficulty));
    }
    return 1;
}

// ============================================================================
// 生命周期方法
// ============================================================================

// ORIG T 0x081440b2：
//   m_endTick(+0x9ec) = CSystemTime::getCurTickCount(s_systemTime_)；
//   playSec = (m_endTick - m_startTick(+0x9e8)) / 1000（ORIG imul 0x10624dd3
//   有符号除法）；对槽位 0..3：user=get_user(i)，user!=0 且 checkValidUser(i)==1
//   且 isMemberAlive(i) 时 m_dungeonLog.IncrementBossStageEnd(*user,
//   (uchar)m_stageCur, playSec, (uchar)m_difficulty)；恒返 1。
int CBossTower::onFinishStage()
{
    int playSec = 0;
    m_endTick = GlobalData::s_systemTime_.getCurTickCount();
    playSec = (m_endTick - m_startTick) / 1000;

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        bool invalid = (member == 0) ||
                       (m_party->checkValidUser(i) != 1);
        if (!invalid && isMemberAlive(i))
        {
            m_dungeonLog.IncrementBossStageEnd(
                *member,
                static_cast<unsigned char>(m_stageCur),
                static_cast<unsigned int>(playSec),
                static_cast<unsigned char>(m_difficulty));
        }
    }
    return 1;
}

// ORIG T 0x08142d24：
//   m_prepareFinishFlag(+0xafc)=flag；m_towerEndTick(+0xaf8)=getCurTickCount()；
//   playSec=(m_towerEndTick-m_towerStartTick(+0xaf4))/1000；成员名表 0xff 清零后
//   party->getMemberNames(names)；槽位 0..3 有效成员：
//     未完成加载（m_memberField[i]==0，ORIG this+(i+0x274)*4+8=+0x9d8 区）时
//     hack addServerHackCnt(0x13a,1,0,0)；
//     m_historyLog.DungeonClearInfo(flag, playSec)。
//   循环后 G_TimerQueue()->InsertTimer(OBJ_TYPE 7, m_idx, msg 0xa3, 5s,
//   key 0xa3, 0xa3)。
void CBossTower::_onPrepareFinishTower(bool flag)
{
    m_prepareFinishFlag = flag;

    int playSec = 0;
    m_towerEndTick = GlobalData::s_systemTime_.getCurTickCount();
    playSec = (m_towerEndTick - m_towerStartTick) / 1000;

    char memberNames[255] = "";               // ebp-0x113（内联展开清零 0xff）
    m_party->getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        bool invalid = (member == 0) ||
                       (m_party->checkValidUser(i) != 1);
        if (invalid)
            continue;

        if (m_memberField[i] == 0)
        {
            member->getHackAnalyzer()->addServerHackCnt(
                member, (WongWork::ENUM_HACKTYPE)0x13a, 1, 0, 0);
        }

        member->m_historyLog.DungeonClearInfo(flag, playSec);
    }

    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa3, 5, 0xa3, 0xa3);
}

// ORIG T 0x08142b36：
//   m_dungeonLog.BossTowerLogDBUpdate() → m_dungeonLog.reset() →
//   G_CGameManager()->returnBossTower(this)。
void CBossTower::_destroy()
{
    m_dungeonLog.BossTowerLogDBUpdate();
    m_dungeonLog.reset();
    G_CGameManager()->returnBossTower(this);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08142f32 _ZN8WongWork10CBossTower13InitBossTowerEP6CPartyi：
// m_dungeon(+0x9f0) = find_dungeon(dungeonIdx)；party/dungeon 判空 → 0；
// m_party(+0x04) = party；m_stageMax(+0xae4) = getRandomTowerMaxStage()；
// m_prepareFinishFlag(+0xafc) = 0；返回 1。
// [batch-19 N18a 封顶 ndiff=5] 唯一结构残差 = ORIG +0x57 三段冗余 regmove
// （`call`→`mov %eax,%edx`→`mov 0x8(%ebp),%eax`→`mov %edx,0xae4(%eax)`）；
// 同函数 +0x1a 的 m_dungeon 落槽处 ORIG 只 3 条（`mov this,%edx;mov %eax,`
// `0x9f0(%edx)`）⇒ 4.1 在同一 store 模板上两种分配，源码侧无法定向。
// 实测否决：① `register int` 无约束 → ndiff 11（4.4 选 %ebx ⇒ 多
// push/pop + `add $0x14,%esp` 帧形全变）；② `register int asm("edx")` →
// ndiff 5（复现 `mov %eax,%edx` 但反出 `mov %edx,%eax; mov this,%edx`
// 三步，净 0 且违 P78）。维持直赋值。
// ----------------------------------------------------------------------------
int CBossTower::InitBossTower(CParty* party, int dungeonIdx)
{
    m_dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (party == 0 || m_dungeon == 0)
        return 0;

    m_party = party;
    m_stageMax = m_dungeon->getRandomTowerMaxStage();
    m_prepareFinishFlag = 0;
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08142fa6 _ZN8WongWork10CBossTower23checkStartGameConditionERhb：
//   dungeon 空/未开放 → trace "boss Tower dungeon is not open"(0xdb,5) +
//     包(1,0x10)/byte 0/int 0x15 → send_to_party → 0；
//   逐槽位：有效成员 charac_level < get_min_level → trace(0xf0,5) +
//     包 int 0xe → send → 0；
//   partyType(+0xcd8)==1 → 1；否则 CheckEnterAncientDungeon 失败 → 包 int 0x11 +
//     byte(flag) → send → 0；useItems 时 UseAncientDungeonItems 失败 →
//     包 int 1 → send → 0；通过 → 1。
// ----------------------------------------------------------------------------
int CBossTower::checkStartGameCondition(unsigned char& flag, bool useItems)
{
    Inven_Item items[4];                       // ebp-0x168 起 4*0x3d，整体清零 0xf4
    memset(items, 0, 0xf4);
    int itemParams[4];                         // ebp-0x44 起 0x10 字节清零
    memset(itemParams, 0, sizeof(itemParams));

    if (m_dungeon == 0 || !m_dungeon->IsOpenScheduleDungeon())
    {
        cMyTrace trace("bool WongWork::CBossTower::checkStartGameCondition("
                       "unsigned char&, bool)", 0xdb, 5);
        trace("boss Tower dungeon is not open");
        PacketGuard packet;
        packet.clear();              // ORIG 0x0814309a（本分支 C1 后显式 clear）
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x15);
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        if (member == 0 || !m_party->checkValidUser(i))
            continue;
        if (member->get_charac_level() < m_dungeon->get_min_level())
        {
            cMyTrace trace("bool WongWork::CBossTower::checkStartGameCondition("
                           "unsigned char&, bool)", 0xf0, 5);
            trace("dungeon level check charac_level : %d, dungeon_min_level : %d",
                  member->get_charac_level(), m_dungeon->get_min_level());
            PacketGuard packet;
            packet.clear();          // ORIG 0x08143229（本分支 C1 后显式 clear）
            packet.put_header(1, 0x10);
            packet.put_byte(0);
            packet.put_int(0xe);
            packet.finalize(true);
            m_party->send_to_party(packet);
            return 0;
        }
    }

    if (m_party->m_battleField.m_bloodState == 1)
    {
        return 1;
    }

    if (!m_party->CheckEnterAncientDungeon(m_dungeon, flag, items, itemParams))
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x11);
        packet.put_byte(static_cast<unsigned char>(flag));
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 0;
    }

    if (useItems &&
        !m_party->UseAncientDungeonItems(m_dungeon, items, itemParams))
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(1);
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 0;
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081434f4 _ZN8WongWork10CBossTower15handleStartGameEP6CPartyic17ENUM_DUNGEON_TYPE：
// dungeonType==1 或 partyType(+0xcd8)==1 → 0；m_expParamA(+0xaec)=dungeonType、
// m_difficulty(+0xae8)=(char)b；reset 链 + m_dungeonLog.reset + m_stageCur=1；
// InitBossTower / checkStartGameCondition 失败 → 0；GM 管理者字节替换广播字节；
// RandomConsistMap 失败 → 0；InitPlayUser(idx 参数, m_idx)；
// m_towerStartTick(+0xaf4)=getCurTickCount、m_towerEndTick(+0xaf8)=0；成员名表
// 清零后逐槽位 IncrementBossStageStart + IncrementDungeonEntrance +
// EnterDungeon 日志；SetEPLPState(1)、gen_timer_key(0x2d)、
// SetSelectedEPLPCmd(-1)；包(0,0x1e1){towerIdx, m_stageMax(short), partyType(byte),
// announceByte(byte)} 广播；SendMapInfoPacket；InsertTimer(OBJ 7, m_idx, msg 0xa5,
// 单人 90s 否则 120s, key 0xa5, 0xa5)；返回 1。
// ----------------------------------------------------------------------------
char CBossTower::handleStartGame(CParty* party, int towerIdx, char b,
                                 ENUM_DUNGEON_TYPE dungeonType)
{
    if (dungeonType == static_cast<ENUM_DUNGEON_TYPE>(1))
        return 0;
    if (party->m_battleField.m_bloodState == 1)
        return 0;

    m_expParamA = static_cast<int>(dungeonType);
    m_difficulty = static_cast<int>(b);        // movsbl

    CBossPlay::reset();
    CBossStage::reset();
    m_dungeonLog.reset();
    m_stageCur = 1;

    // X1 形（XOR_FAMILY + 落点岛）：ORIG `call InitBossTower; xor $0x1,%eax;
    // test %al,%al; je BODY; mov $0,%ebx; jmp end` — return 0 走落点岛。
    register unsigned char ibt asm("eax") =
        (unsigned char)InitBossTower(party, towerIdx);
    ibt ^= 1;
    if (!ibt) goto CHECK;
    return 0;
CHECK:
    unsigned char gmFlag = 0;
    // X1 形：ORIG `call checkStartGameCondition; xor $0x1,%eax; test; je GM`.
    register unsigned char csc asm("eax") =
        (unsigned char)checkStartGameCondition(gmFlag, true);
    csc ^= 1;
    if (!csc) goto GMBLOCK;
    return 0;
GMBLOCK:
    // ORIG：manager 存 -0x28 复用；GM 字节直写回 b 的字节槽（-0x13c）。
    // 嵌套 if（非 &&）以复现 ORIG 双 je 落点形，避免 4.4 物化布尔。
    CUser* manager = party->getManager();
    if (manager->isGMUser())
    {
        if (manager->m_gmBossDifficulty != 0)
        {
            b = manager->m_gmBossDifficulty;   // GM 观战标记替换广播字节
            manager->m_gmBossDifficulty = 0;
        }
    }
    // X1 形：RandomConsistMap 假 → 0。
    register unsigned char rcm asm("eax") =
        (unsigned char)RandomConsistMap(*manager);
    rcm ^= 1;
    if (!rcm) goto DOPLAY;
    return 0;
DOPLAY:
    InitPlayUser(towerIdx, m_idx);
    m_towerStartTick = GlobalData::s_systemTime_.getCurTickCount();
    m_towerEndTick = 0;

    char memberNames[256] = {0};             // ebp-0x138（内联展开清零 0x100）
    m_party->_getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        bool invalid = (member == 0) ||
                       (m_party->checkValidUser(i) != 1);
        if (invalid)
            continue;

        m_dungeonLog.IncrementBossStageStart(
            *member,
            static_cast<unsigned char>(m_stageCur),
            static_cast<unsigned char>(m_difficulty));

        // ORIG：IncrementDungeonEntrance 在循环内，每个有效成员调用。
        if (m_party->m_battleField.m_bloodState == 1)
            GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(towerIdx, 1);
        else
            GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(towerIdx, 0);

        member->m_historyLog.EnterDungeon(m_dungeon->GetDungeonName(),
                                          m_difficulty, memberNames,
                                          m_party->m_battleField.m_bloodState);
    }

    m_party->SetEPLPState(1);
    m_party->gen_timer_key((TIMER_MESSAGE)0x2d);
    m_party->SetSelectedEPLPCmd(-1);

    PacketGuard packet;
    packet.put_header(0, 0x1e1);
    packet.put_int(towerIdx);
    packet.put_short(m_stageMax);
    packet.put_byte(m_party->m_battleField.m_bloodState);
    packet.put_byte(static_cast<int>(b));    // announceByte 即 b 槽（GM 可改写）
    packet.finalize(true);
    m_party->send_to_party(packet);

    SendMapInfoPacket();

    int timeoutSec = (m_party->get_member_count() == 1) ? 0x5a : 0x78;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa5, timeoutSec, 0xa5, 0xa5);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081454b8 _ZN8WongWork10CBossTower13handleUseItemEP5CUsermh：
// GetMemberSlotNo==-1 → 0；itemId==0xbdd → m_useItemCount[slot]++（+0x9c8 区）；
// param==2 → m_memberField[slot]++（+0x9d8）；返回 1。
// ----------------------------------------------------------------------------
int CBossTower::handleUseItem(CUser* user, unsigned long itemId, unsigned char param)
{
    const int slot = static_cast<int>(m_party->GetMemberSlotNo(user));
    if (slot == -1)
        return 0;

    if (itemId == 0xbdd)
    {
        ++m_useItemCount[slot];
    }
    if (param == 2)
    {
        ++m_memberField[slot];
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08143f6a _ZN8WongWork10CBossTower19handleFinishLoadingEP5CUser：
// slot 无效或 checkValidUser 为假 → 0；setMemberReady(slot,true) +
// SendCmdOkPacket(0x28)；虚调 checkAllMemberReady(get_member_count())（vptr+0xc）
// 为真时广播包(0,0x1e)/int 0/finalize/send；返回 1。
// ----------------------------------------------------------------------------
int CBossTower::handleFinishLoading(CUser* user)
{
    CParty* party = m_party;
    const int slot = static_cast<int>(party->GetMemberSlotNo(user));
    bool ready = false;
    if (slot != -1 && party->checkValidUser(slot) != 0)
        ready = true;

    if (ready)
    {
        setMemberReady(slot, true);
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);
        if (checkAllMemberReady(party->get_member_count()))
        {
            PacketGuard packet;
            packet.put_header(0, 0x1e);
            packet.put_int(0);
            packet.finalize(true);
            party->send_to_party(packet);
        }
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08142cde _ZN8WongWork10CBossTower15_onTimerProcessE13TIMER_MESSAGEj：
// msg 0xa3 → onClearBossTower()；0xa4 → onReturnVillage()；0xa5 →
// onReadyTimerCheck()；其余忽略。第二参数未使用。
// [batch-19 N 封顶 ndiff=3] 残余 = 末级比较极性：ORIG `cmp $0xa3,%eax;`
// `jne exit` + 体 fall-through，4.4 恒 `je body; jmp exit`（switch 尾 case
// 与空 default 汇入同一出口时 4.4 不 reverse 最后比较）。实测否决三形：
// ① 去 default → 3（同）；② if-else 链 → 11（4.4 折 `cmpl $imm,mem` 丢
// `mov 0xc(%ebp),%eax` 三级 + jne 正向）；③ `register` 形参 + if-else →
// 20（cmp 用寄存器但 body 全 fall-through，块序反）。switch+default 形最优。
// ----------------------------------------------------------------------------
void CBossTower::_onTimerProcess(TIMER_MESSAGE msg, unsigned int a)
{
    (void)a;   // ORIG 忽略第二参数
    switch (msg)
    {
    case (TIMER_MESSAGE)0xa4:
        onReturnVillage();
        break;
    case (TIMER_MESSAGE)0xa5:
        onReadyTimerCheck();
        break;
    case (TIMER_MESSAGE)0xa3:
        onClearBossTower();
        break;
    default:
        break;
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08142b72 _ZN8WongWork10CBossTower17handleEPLPCommandEi：
// bool 门（party 非空且 GetEPLPState()==2 才放行）；cmd(unsigned)<=2 时成员名表
// 清零、逐有效成员记 LeaveDungeon(level 固定 0) 并 ReturnToVillage；随后
// _destroy，返回 1；门未放行返回 0。
// ----------------------------------------------------------------------------
int CBossTower::handleEPLPCommand(int cmd)
{
    bool skip = true;
    if (m_party != 0)
    {
        if (m_party->GetEPLPState() == 2)
            skip = false;
    }

    if (!skip)
    {
        if ((unsigned int)cmd <= 2)
        {
            char memberNames[255] = "";           // ebp-0x110（内联清零 0xff）

            const char partyTypeByte =
                static_cast<char>(m_party->m_battleField.m_bloodState);   // 低字节 movsbl
            m_party->getMemberNames(memberNames);

            for (int i = 0; i <= 3; ++i)
            {
                CUser* member = m_party->get_user(i);
                bool invalid = (member == 0) ||
                               (m_party->checkValidUser(i) != 1);
                if (!invalid)
                {
                    member->m_historyLog.LeaveDungeon(
                        m_dungeon->GetDungeonName(), 0, memberNames,
                        static_cast<int>(partyTypeByte));
                }
            }
            m_party->ReturnToVillage();
        }

        _destroy();
        return 1;
    }
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0814432a _ZN8WongWork10CBossTower16SendRewardPacketEv：
// 逐有效成员发奖励预告包(0,0x1e3)：put_int(m_rewardExp[i])；对全部 4 个奖励块
// （无论有效性）：put_byte(m_memberReward[j].m_count) 后逐件
// put_int(item.m_addInfo)+put_int(item.get_add_info())；finalize 后 Send。
// 恒返 1。
// ----------------------------------------------------------------------------
int CBossTower::SendRewardPacket()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        bool invalid = (member == 0) ||
                       (m_party->checkValidUser(i) != 1);
        if (invalid)
            continue;

        PacketGuard packet;
        packet.put_header(0, 0x1e3);
        packet.put_int(m_rewardExp[i]);              // this+(i+4)*4+8 = +0x18
        for (int j = 0; j <= 3; ++j)
        {
            packet.put_byte(m_memberReward[j].m_count);   // +j*0x268+0x28
            for (int k = 0; k < m_memberReward[j].m_count; ++k)
            {
                Inven_Item& reward = m_memberReward[j].m_items[k];
                packet.put_int(reward.m_addInfo);         // 块内 +0x2e = item+0x02
                packet.put_int(reward.get_add_info());    // item+0x07
            }
        }
        packet.finalize(true);
        member->Send(packet);
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081441d4 _ZN8WongWork10CBossTower14SendELPLPacketEv：
// 非 partyType==1 时逐有效成员 checkStartGameCondition(flag,false)，任一失败
// ok=false 提前跳出；随后无条件发包(0,0x1e4){ok}给全队；恒返 1。
// ----------------------------------------------------------------------------
int CBossTower::SendELPLPacket()
{
    CParty* party = m_party;
    bool ok = true;                            // ebp-0x11
    if (party->m_battleField.m_bloodState != 1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = party->get_user(i);
            bool invalid = (member == 0) ||
                           (party->checkValidUser(i) != 1);
            if (!invalid)
            {
                unsigned char requireFlag = 0;       // ebp-0x21
                if (checkStartGameCondition(requireFlag, false) != 1)
                {
                    ok = false;
                    break;
                }
            }
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0x1e4);
    packet.put_byte(ok ? 1 : 0);
    packet.finalize(true);
    party->send_to_party(packet);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081446a2 _ZN8WongWork10CBossTower15InsertRewardExpEv：
// 逐有效成员：bonusExp/extraFlag 局部清零后 gain_exp_sp(m_rewardExp[i], ...,
// eExpAddReason 2, 0, false)；成功时 SendNotiPacket(eSendTarget 2,
// ENUM_NOTIPACKET 2, 1)。恒返 1。
// [batch-19 AE] 4→0：两处 P0 形订正——
//  ① ORIG +0x53 起 `xor$1;test%al,%al;jne +0xe2` 直测 %eax（choose 无槽）⇒
//    成员有效性判定必为 `if (member==0 || checkValidUser(i)!=1) continue;`
//    内联形；写 `bool invalid = ...;` 具名 bool 局部 ⇒ 4.4 多
//    `mov %al,-0xa; cmpb $0,-0xa` 两段（+2/−1）。
//  ② ORIG +0x60 `movb $0x0,-0x9(%ebp)` 在 gain_exp_sp call **之前** ⇒ 原码
//    `bool gained = false;` 后接 `gained = member->gain_exp_sp(...)` 两句；
//    合写成 `bool gained = call();` 丢预清行（−1）。
// ----------------------------------------------------------------------------
int CBossTower::InsertRewardExp()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        if ((member == 0) || (m_party->checkValidUser(i) != 1))
            continue;

        int bonusExp = 0;                        // ebp-0x18
        int extraFlag = 0;                       // ebp-0x1c
        bool gained = false;
        gained = member->gain_exp_sp(
            m_rewardExp[i], bonusExp, extraFlag,
            (CUser::eExpAddReason)2, 0, false);
        if (gained)
        {
            member->SendNotiPacket((CUser::eSendTarget)2,
                                   (ENUM_NOTIPACKET)2, 1);
        }
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081447a0 _ZN8WongWork10CBossTower17InsertRewardItemsEv：
// 逐有效成员、逐件 m_memberReward[i].m_items[j]（上限 m_count）：
//   空（m_addInfo==-1）跳过；m_addInfo==0 → 金币：get_add_info() 金额经
//   gain_money(reason 5,true,0) 入账，不足部分 SendMoneyFullReason(0,金额,实得)，
//   slotResult=0；否则 insertItemIntoInventory(item, reason 0xb, true, true)。
//   成功（>=0）发包(0,0xe){itemSpace,short 1,MakeItemPacket}+Send；
//   失败（<0）邮件补发：uniqueNo[0x18]/sender[0x1e]/title[0x100] 清零，
//   find_item(m_addInfo) 后取脚本文案 chn_game_server_msg_21(sender)/22(title)，
//   IsAvatarItem()（vptr+0x10）为真 → 计算 usable period 到期秒
//   （0 或 0x16d 不限）、genIPGNo(TYPE 3,characNo) 代理名、get_endurance，
//   ReqDBSendNewAvatarMail；否则 ReqDBSendNewSystemMail(0xf, 本服组)。
// 恒返 1。
// ----------------------------------------------------------------------------
int CBossTower::InsertRewardItems()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        bool invalid = (member == 0) ||
                       (m_party->checkValidUser(i) != 1);
        if (invalid)
            continue;

        for (int j = 0; j < m_memberReward[i].m_count; ++j)
        {
            Inven_Item& reward = m_memberReward[i].m_items[j];
            if (reward.m_addInfo == -1)          // 空槽（ORIG 读 +0x2e 的 int）
                continue;

            int slotResult = -1;
            if (reward.m_addInfo == 0)
            {
                // 金币奖励：m_addInfo2 为金额（ORIG 三次 get_add_info 各用点重取，P48）
                const int gained = member->getCurCharacInvenW()->gain_money(
                    reward.get_add_info(), (eMoneyAddReason)5, true, 0);
                if (gained < reward.get_add_info())
                {
                    member->SendMoneyFullReason(
                        (ENUM_MONEY_FULL_REASON)0,
                        static_cast<unsigned long>(reward.get_add_info()),
                        static_cast<unsigned long>(gained));
                }
                slotResult = 0;
            }
            else
            {
                // ORIG else 分支重取 getCurCharacInvenW（与金币分支各一次，P48）
                slotResult = member->getCurCharacInvenW()->insertItemIntoInventory(
                    reward, (eItemAddReason)0xb, true, true);
            }

            if (slotResult >= 0)
            {
                PacketGuard packet;
                packet.put_header(0, 0xe);
                packet.put_byte(reward.GetItemSpace());
                packet.put_short(1);
                member->getCurCharacInvenR()->MakeItemPacket(
                    (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)reward.GetItemSpace()),
                    slotResult, packet);
                packet.finalize(true);
                member->Send(packet);
            }
            else
            {
                // 背包满 → 邮件补发
                char uniqueNo[0x18] = {0};         // ebp-0x64（ORIG .rodata 清零模板）

                CItem* itemData =
                    G_CDataManager()->find_item(reward.m_addInfo);
                if (itemData == 0)
                    continue;

                char title[256];                   // ebp-0x182 起 0x100 清零
                memset(title, 0, sizeof(title));
                char senderName[30];               // ebp-0x82 起 0x1e 清零
                memset(senderName, 0, sizeof(senderName));

                strncpy(senderName,
                        g_scriptStringManager_.findString(
                            4, "chn_game_server_msg_21", 0),
                        0x1d);
                strncpy(title,
                        g_scriptStringManager_.findString(
                            4, "chn_game_server_msg_22", 0),
                        0xff);

                if (itemData->IsAvatarItem())
                {
                    // 有效期换算到期秒（0 或 0x16d=365 视为无限）
                    const int period = itemData->getUsablePeriod();
                    int expireSec = 0;             // ebp-0x24
                    if (period != 0 && period != 0x16d)
                    {
                        expireSec =
                            GlobalData::s_systemTime_.getCurSec() +
                            period * 0x15180;      // 天 → 秒
                    }

                    char* agency = WongWork::CGenUniqueNo::genIPGNo(
                        WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_3,
                        static_cast<unsigned int>(member->getCurCharacNo()),
                        uniqueNo);
                    const int endurance =
                        static_cast<CEquipItem*>(itemData)->get_endurance();
                    WongWork::CMailBoxHelper::ReqDBSendNewAvatarMail(
                        member, member->getCurCharacNo(), reward.m_addInfo,
                        expireSec, static_cast<char>(endurance), 0, agency,
                        title, static_cast<int>(strlen(title)));
                }
                else
                {
                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        senderName, reward, 0,
                        static_cast<unsigned int>(member->getCurCharacNo()),
                        title, static_cast<int>(strlen(title)), 0xf,
                        member->GetServerGroup(), false, false);
                }
            }
        }
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08144508 _ZN8WongWork10CBossTower16onClearBossTowerEv：
// partyType==1 → 仅发包(0,0xfe)；否则 generateRewardExp/generateRewardItem/
// SendRewardPacket/InsertRewardExp/InsertRewardItems 任一非 1 → 0；
// SendELPLPacket()==1 时 SetEPLPState(2) 并 InsertTimer(OBJ 7, m_idx, msg 0xa4,
// 20s, key 0xa4, 0xa4)。返回结果标志。
// ----------------------------------------------------------------------------
int CBossTower::onClearBossTower()
{
    CParty* party = m_party;
    if (party->m_battleField.m_bloodState == 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0xfe);
        packet.finalize(true);
        party->send_to_party(packet);
    }
    else
    {
        if (generateRewardExp() != 1)
            return 0;
        if (generateRewardItem() != 1)
            return 0;
        if (SendRewardPacket() != 1)
            return 0;
        if (InsertRewardExp() != 1)
            return 0;
        if (InsertRewardItems() != 1)
            return 0;
    }

    if (SendELPLPacket() != 1)
        return 0;

    party->SetEPLPState(2);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa4, 0x14, 0xa4, 0xa4);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08144d54 _ZN8WongWork10CBossTower17generateRewardExpEv：
// 逐有效成员：get_mob_reward(charac_level) 得基础奖励经验后直调基类
// CBossPlay::generateRewardExp(i, m_stageCur, mobReward, m_dungeon)（ORIG
// call 08149e20 非虚）。恒返 1。
// ----------------------------------------------------------------------------
int CBossTower::generateRewardExp()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        bool invalid = (member == 0) ||
                       (m_party->checkValidUser(i) != 1);
        if (invalid)
            continue;

        unsigned int mobReward = 0;              // ebp-0x14
        G_CDataManager()->get_mob_reward(member->get_charac_level(), mobReward);
        CBossPlay::generateRewardExp(
            i, static_cast<unsigned int>(m_stageCur), mobReward, m_dungeon);
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08144e36 _ZN8WongWork10CBossTower18generateRewardItemEv：
// 逐有效成员：层数奖励上限 = min(10, min(GetTowerScript 槽值,
// getTowerMaxClearRewardItemNum()))，其中 GetTowerScript 返回 it->second+4
// （&stDeathTower_t::m_rateA[0]），ORIG 读 ret+4+(stage+200)*4（stage>=1 落在
// m_cntB 区）；层数参数 = min(10, m_stageCur)；直调基类
// CBossPlay::generateRewardItem(i, itemCount, level, difficulty, 层数参数,
// m_dungeon)（ORIG call 08149914 非虚）。恒返 1。
// ----------------------------------------------------------------------------
int CBossTower::generateRewardItem()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        bool invalid = (member == 0) ||
                       (m_party->checkValidUser(i) != 1);
        if (invalid)
            continue;

        stDeathTower_t* towerScript = G_CDataManager()->GetTowerScript(
            static_cast<unsigned int>(m_dungeon->get_index()));
        // ORIG 0x08144e36 +0x09d..+0x0a3：`add $0xc8,%edx`（0xc8*4=0x320）后
        // `mov 0x4(%eax,%edx,4),%eax` ⇒ 绝对槽 = towerScript + 4 + 0x320 + 4*stage
        // = stDeathTower_t::m_rewardCardNum[stage]（该数组 +0x324，int 读宽一致）。
        const int scriptLimit = towerScript->m_rewardCardNum[m_stageCur];

        int itemCount = std::min(m_dungeon->getTowerMaxClearRewardItemNum(),
                                 scriptLimit);
        itemCount = std::min(10, itemCount);
        const int stageParam = std::min(10, m_stageCur);

        CBossPlay::generateRewardItem(
            i, itemCount,
            static_cast<unsigned int>(member->get_charac_level()),
            static_cast<ENUM_DUNGEON_DIFFICULTY>(m_difficulty),
            static_cast<unsigned int>(stageParam),
            m_dungeon);
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0814530c _ZN8WongWork10CBossTower19onChargeRequireItemEP5CUser：
// user==0 → 0；partyType==1 → 1；m_dungeon->m_entryRequiredItem.m_enableFlag(+0x7fc) 非零时：
// find_item(m_entryRequiredItem.m_itemIdx(+0x7f4)) 失败 → 0；make_item 构造物品后按 ORIG
// 用 dungeon 字段覆写 item.m_addInfo(+0x7f4)/m_addInfo2(+0x7f8)，入包
// （reason 4,true,true），slot>=0 时 SendUpdateItemList(target 1, space 0,
// slot)。末尾恒返 1。
// ----------------------------------------------------------------------------
int CBossTower::onChargeRequireItem(CUser* user)
{
    if (user == 0)
        return 0;
    if (m_party->m_battleField.m_bloodState == 1)
        return 1;

    if (m_dungeon->m_entryRequiredItem.m_enableFlag != 0)
    {
        Inven_Item item;                               // ebp-0x4d

        CItem* itemData = G_CDataManager()->find_item(
            m_dungeon->m_entryRequiredItem.m_itemIdx);
        if (itemData == 0)
            return 0;

        itemData->make_item(item);                     // 虚槽 vptr+0x08
        item.m_addInfo = m_dungeon->m_entryRequiredItem.m_itemIdx;   // item+0x02
        item.m_addInfo2 = m_dungeon->m_entryRequiredItem.m_itemCount;  // item+0x07

        const int slot =
            user->getCurCharacInvenW()->insertItemIntoInventory(
                item, (eItemAddReason)4, true, true);
        if (slot >= 0)
        {
            user->SendUpdateItemList(CUser::eSendTarget_1,
                                     (ENUM_ITEMSPACE)0, slot);
        }
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x0814512a _ZN8WongWork10CBossTower17onReadyTimerCheckEv：
// 虚调 checkAllMemberReady(get_member_count())（vptr+0x0c）为真 → 0；
// party 空或无成员 → _destroy 返回 0；逐槽位有效且未就绪
// （m_memberReady[i]!=1，基类 +0x11 数组）的成员：onChargeRequireItem +
// 虚调 handleLeaveUser(member)（vptr+0x14）；仍有成员 → 广播包(0,0x1e)/int 0，
// 否则 _destroy；返回 1。
// ----------------------------------------------------------------------------
int CBossTower::onReadyTimerCheck()
{
    if (checkAllMemberReady(m_party->get_member_count()))
        return 0;

    if (m_party == 0 || m_party->get_member_count() < 1)
    {
        _destroy();
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        bool invalid = (member == 0) ||
                       (m_party->checkValidUser(i) != 1);
        if (!invalid && m_memberReady[i] != 1)
        {
            onChargeRequireItem(member);
            handleLeaveUser(member);
        }
    }

    if (m_party->get_member_count() > 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x1e);
        packet.put_int(0);
        packet.finalize(true);
        m_party->send_to_party(packet);
    }
    else
    {
        _destroy();
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08144fca _ZN8WongWork10CBossTower15onReturnVillageEv：
// party 存在且 GetEPLPState()==2 时：成员名表 0xff 清零、逐有效成员记
// LeaveDungeon(dungeon, m_difficulty, names, partyType 低字节)，ReturnToVillage
// 后 _destroy。恒返 1。
// ----------------------------------------------------------------------------
int CBossTower::onReturnVillage()
{
    if (m_party != 0 && m_party->GetEPLPState() == 2)
    {
        char memberNames[255] = "";            // ebp-0x120（内联展开清零 0xff）

        const char partyTypeByte =
            static_cast<char>(m_party->m_battleField.m_bloodState);   // 低字节 movsbl
        m_party->getMemberNames(memberNames);

        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = m_party->get_user(i);
            bool invalid = (member == 0) ||
                           (m_party->checkValidUser(i) != 1);
            if (!invalid)
            {
                member->m_historyLog.LeaveDungeon(m_dungeon->GetDungeonName(),
                                                  m_difficulty, memberNames,
                                                  static_cast<int>(partyTypeByte));
            }
        }
        m_party->ReturnToVillage();
        _destroy();
    }
    return 1;
}

}  // namespace WongWork
