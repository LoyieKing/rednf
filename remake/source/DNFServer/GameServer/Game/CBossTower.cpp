// df_game_r Game/ WongWork::CBossTower 实现。
// getIdx 0x082aa728 读 +0xaf0；setIdx 0x082aa716 写 +0xaf0。
// 虚函数（FixBossVtables.TowerVirtuals）：handleDieUser 0x08143a62、
// handleLeaveUser 0x08143ae2、handleMoveMap 0x08143996、handleDropItem
// 0x08143e5e、handleKillMonster 0x08143ebc、handlePickupItem(char 按值)
// 0x08143d2c。ORIG 虚表 _ZTVN8WongWork10CBossTowerE@0x08b70260 共 11 槽，
// 与 CBossTower.h 声明顺序一致。
// onFinishStage/_onPrepareFinishTower/_destroy 见文件末尾
// （FixBossTowerDeps：ORIG 0x081440b2 / 0x08142d24 / 0x08142b36）。
#include "CBossTower.h"

#include "CParty.h"     // get_user/get_member_count/getMemberNames/IsRoutingState
#include "CUser.h"      // SendCmdErrorPacket / m_historyLog(cUserHistoryLog@+0x79700)
#include "QuickParty.h" // 先于 CDungeon.h：QUICKPARTY_H 已定义则跳过其镜像块
#include "CDungeon.h"   // GetDungeonName

#include <cstring>

#include "GlobalData.h"      // s_systemTime_
#include "CSystemTime.h"     // getCurTickCount
#include "TimerQueue.h"      // G_TimerQueue / TimerEntry::OBJ_TYPE / TIMER_MESSAGE
#include "CGameManager.h"    // G_CGameManager()->returnBossTower
#include "cHistoryTrace.h"   // 权威声明（原局部最小声明已删除）
#include "PacketGuard.h"        // PacketGuard（handleStartGame/handleFinishLoading 回包）
#include "CDungeonEntranceLog.h" // GetInstanceDungeonEntranceLog / IncrementDungeonEntrance
#include "CDataManager.h"       // G_CDataManager()->find_dungeon（InitBossTower）
#include "GameTypes.h"          // cMyTrace（checkStartGameCondition）


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
}

// ============================================================================
// 虚函数实现（WongWork::CBossTower）
// ============================================================================
namespace WongWork
{

// ORIG 0x08143a62：
//   user==0 → 0；CBossPlay::handleDieUser 非 1 → 0；
//   虚调 _checkMemberDie()（vptr+8 → 槽[4]，ORIG call *(vptr+8)）为真时
//   onFinishStage()；+0xafc 字节 != 1 时 _onPrepareFinishTower(false)；返回 1。
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

// ORIG 0x08143ae2：
//   user==0 → 0；CBossPlay::handleLeaveUser 为假 → 0；
//   记 LeaveDungeon 历史（dungeon=+0x9f0，stageIdx=+0xae8，成员名表，
//   party+0xcd8 低字节）；trace "member_count : %d"；
//   无成员 → _destroy 并返回 0；（全员死或无成员检查）→ onFinishStage +
//   _onPrepareFinishTower(false)；否则 +0xae8 = member_count-1（1..4）；返回 1。
int CBossTower::handleLeaveUser(CUser* user)
{
    if (user == 0)
        return 0;
    if (!CBossPlay::handleLeaveUser(user))
        return 0;

    CParty* party = getParty();

    char memberNames[255];                    // ebp-0x11d 起 0xff 字节清零
    memset(memberNames, 0, sizeof(memberNames));

    char partyField = static_cast<char>(
        *reinterpret_cast<int*>(reinterpret_cast<char*>(party) + 0xcd8));
    party->getMemberNames(memberNames);

    int stageIdx = *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0xae8);
    CDungeon* dungeon =
        *reinterpret_cast<CDungeon**>(reinterpret_cast<char*>(this) + 0x9f0);

    reinterpret_cast<cUserHistoryLog*>(reinterpret_cast<char*>(user) + 0x79700)
        ->LeaveDungeon(dungeon->GetDungeonName(), stageIdx, memberNames,
                       static_cast<int>(partyField));

    int memberCount = party->get_member_count();
    cHistoryTrace histTrace;
    histTrace("member_count : %d", memberCount);

    if (party == 0 || party->get_member_count() < 1)
    {
        _destroy();
        return 0;
    }

    bool finishNow = false;
    if (party == 0)
        finishNow = true;
    else if (party->get_member_count() > 0 && _checkMemberDie())
        finishNow = true;

    if (finishNow)
    {
        onFinishStage();
        _onPrepareFinishTower(false);
    }
    else
    {
        int count = party->get_member_count();
        if (count >= 1 && count <= 4)
            *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0xae8) = count - 1;
    }
    return 1;
}

// ORIG 0x08143996：
//   CBossStage::handleMoveMap 为假 → 0；否则对 4 个槽位中存在且存活的成员
//   调 m_dungeonLog(+0xb00).IncrementBossStageStart(user,
//   (uchar)*(int*)(this+0xae0), (uchar)*(int*)(this+0xae8))；返回 1。
int CBossTower::handleMoveMap(unsigned short* mapIndex)
{
    if (!CBossStage::handleMoveMap(mapIndex))
        return 0;
    for (int i = 0; i < 4; ++i)
    {
        CUser* member = getParty()->get_user(i);
        if (member != 0 && isMemberAlive(i))
        {
            m_dungeonLog.IncrementBossStageStart(
                *member,
                static_cast<unsigned char>(*reinterpret_cast<unsigned int*>(
                    reinterpret_cast<char*>(this) + 0xae0)),
                static_cast<unsigned char>(*reinterpret_cast<unsigned int*>(
                    reinterpret_cast<char*>(this) + 0xae8)));
        }
    }
    return 1;
}

// ORIG 0x08143e5e：转发 CBossStage::handleDropItem，结果 ==1 → 1 否则 0。
int CBossTower::handleDropItem(CUser* user, int a, int b, char c, int d, int e)
{
    return CBossStage::handleDropItem(user, a, b, c, d, e) ? 1 : 0;
}

// ORIG 0x08143ebc：
//   CBossStage::handleKillMonster 为假 → 0；怪物表 std::map<int,map_monster>
//   @+0xa00 为空（_Rb_tree::_M_node_count@map+0x14 ==0，ORIG thunk
//   0x08145bb2 读 map 基址+0x14）时 onFinishStage()；且
//   *(uint*)(this+0xae4) <= *(uint*)(this+0xae0) 时
//   _onPrepareFinishTower(true)；返回 1。
int CBossTower::handleKillMonster(CUser* user, unsigned short a, unsigned short b,
                                  const MSG_MONSTER_DIE& die, int* d)
{
    if (!CBossStage::handleKillMonster(user, a, b, die, d))
        return 0;
    if (*reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 0xa14) == 0)
    {
        onFinishStage();
        if (*reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 0xae4) <=
            *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 0xae0))
        {
            _onPrepareFinishTower(true);
        }
    }
    return 1;
}

// ORIG 0x08143d2c（新增虚槽：char 按值版，与 CBossStage 的 char& 版并存）：
//   party 非空且 IsRoutingState() → SendCmdErrorPacket(0x2e,0xd8)，返回 0；
//   经由 CBossStage::handlePickupItem(char& out_rarity) 拾取失败 → 0；
//   server_rarity != client 参数时 trace（ORIG __PRETTY_FUNCTION__ 文本，
//   600,5）；user 非空且 rarity==3 时 m_dungeonLog.IncrementBossPickUpItem
//   (*user, (uchar)*(int*)(this+0xae0), (uchar)*(int*)(this+0xae8))；返回 1。
int CBossTower::handlePickupItem(CUser* user, int a, bool b, char c)
{
    bool routingBlocked = true;
    if (getParty() != 0)
        routingBlocked = getParty()->IsRoutingState();
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
            static_cast<unsigned char>(*reinterpret_cast<unsigned int*>(
                reinterpret_cast<char*>(this) + 0xae0)),
            static_cast<unsigned char>(*reinterpret_cast<unsigned int*>(
                reinterpret_cast<char*>(this) + 0xae8)));
    }
    return 1;
}

// ============================================================================
// 生命周期方法（FixBossTowerDeps）
// ============================================================================

// ORIG 0x081440b2：
//   +0x9ec = CSystemTime::getCurTickCount(s_systemTime_)；
//   playSec = (int)(+0x9ec - +0x9e8) / 1000（ORIG imul 0x10624dd3 有符号除法）；
//   对槽位 0..3：user=get_user(i)，user!=0 且 checkValidUser(i)==1 且
//   isMemberAlive(i) 时 m_dungeonLog.IncrementBossStageEnd(
//   *user, (uchar)+0xae0, playSec, (uchar)+0xae8)；恒返 1。
int CBossTower::onFinishStage()
{
    unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 0x9ec) = now;

    int startTick =
        static_cast<int>(*reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 0x9e8));
    int playSec = (static_cast<int>(now) - startTick) / 1000;

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = getParty()->get_user(i);
        bool invalid = (member == 0) ||
                       (getParty()->checkValidUser(i) != 1);
        if (!invalid && isMemberAlive(i))
        {
            m_dungeonLog.IncrementBossStageEnd(
                *member,
                static_cast<unsigned char>(*reinterpret_cast<unsigned int*>(
                    reinterpret_cast<char*>(this) + 0xae0)),
                static_cast<unsigned int>(playSec),
                static_cast<unsigned char>(*reinterpret_cast<unsigned int*>(
                    reinterpret_cast<char*>(this) + 0xae8)));
        }
    }
    return 1;
}

// ORIG 0x08142d24：
//   m_prepareFinishFlag(+0xafc) = flag；+0xaf8 = getCurTickCount()；
//   playSec = (int)(+0xaf8 - +0xaf4) / 1000；成员名表 0xff 清零后
//   party->getMemberNames(names)；槽位 0..3 有效成员：
//     成员加载标记(+0x9d0+i*4)==0 时 hack addServerHackCnt(0x13a,1,0,0)；
//     cUserHistoryLog@user+0x79700 DungeonClearInfo(flag, playSec)。
//   循环后 G_TimerQueue()->InsertTimer(OBJ_TYPE 7, m_idx, msg 0xa3, 5s,
//   param2 0xa3, param3 0xa3)。
void CBossTower::_onPrepareFinishTower(bool flag)
{
    m_prepareFinishFlag = flag ? 1 : 0;

    unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 0xaf8) = now;

    int startTick =
        static_cast<int>(*reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 0xaf4));
    int playSec = (static_cast<int>(now) - startTick) / 1000;

    char memberNames[255];                    // ebp-0x113 起 0xff 字节清零
    memset(memberNames, 0, sizeof(memberNames));
    getParty()->getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = getParty()->get_user(i);
        bool invalid = (member == 0) ||
                       (getParty()->checkValidUser(i) != 1);
        if (invalid)
            continue;

        // ORIG *(int*)(this + (i + 0x274)*4 + 8)：成员完成加载标记数组 @+0x9d0。
        if (*reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x9d0 + i * 4) == 0)
        {
            member->getHackAnalyzer()->addServerHackCnt(
                member, (WongWork::ENUM_HACKTYPE)0x13a, 1, 0, 0);
        }

        reinterpret_cast<cUserHistoryLog*>(reinterpret_cast<char*>(member) + 0x79700)
            ->DungeonClearInfo(flag ? 1 : 0, playSec);
    }

    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, getIdx(),
                                (TIMER_MESSAGE)0xa3, 5, 0xa3, 0xa3);
}

// ORIG 0x08142b36：
//   m_dungeonLog.BossTowerLogDBUpdate() → m_dungeonLog.reset() →
//   G_CGameManager()->returnBossTower(this)。
void CBossTower::_destroy()
{
    m_dungeonLog.BossTowerLogDBUpdate();
    m_dungeonLog.reset();
    G_CGameManager()->returnBossTower(this);
}

// ============================================================================
// GlobalBossAndAltar 批次（2026-08-24）：InitBossTower / checkStartGameCondition /
// handleStartGame / handleUseItem / handleFinishLoading
// （docs/class_func_reports/WongWork__CBossTower/ 对应 md 全文）。
// ============================================================================

// ----------------------------------------------------------------------------
// ORIG T 0x08142f32 _ZN8WongWork10CBossTower13InitBossTowerEP6CPartyi：
// m_dungeon(+0x9f0) = find_dungeon(dungeonIdx)；party/dungeon 判空 → 0；
// getParty() = party；m_stageMax(+0xae4) = getRandomTowerMaxStage()；
// m_prepareFinishFlag(+0xafc) = 0；返回 1。
// ----------------------------------------------------------------------------
int CBossTower::InitBossTower(CParty* party, int dungeonIdx)
{
    CDungeon*& dungeon =
        *reinterpret_cast<CDungeon**>(reinterpret_cast<char*>(this) + 0x9f0);
    dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (party == 0 || dungeon == 0)
        return 0;

    m_party = party;   // ORIG *(int*)(this+4) = party
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0xae4) =
        dungeon->getRandomTowerMaxStage();
    m_prepareFinishFlag = 0;
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08142fa6 _ZN8WongWork10CBossTower23checkStartGameConditionERhb：
//   dungeon 空/未开放 → trace "boss Tower dungeon is not open"(0xdb,5) +
//     包(1,0x10)/byte 0/int 0x15 → send_to_party → 0；
//   逐槽位：有效成员 charac_level < get_min_level → trace(0xf0,5) +
//     包 int 0xe → send → 0；
//   partyType==1 → 1；否则 CheckEnterAncientDungeon 失败 → 包 int 0x11 +
//     byte(flag) → send → 0；useItems 时 UseAncientDungeonItems 失败 →
//     包 int 1 → send → 0；通过 → 1。
// ----------------------------------------------------------------------------
int CBossTower::checkStartGameCondition(unsigned char& flag, bool useItems)
{
    Inven_Item items[4];                       // ebp-0x168 起 4*0x3d，整体清零 0xf4
    memset(items, 0, 0xf4);
    int itemParams[4];                         // ebp-0x44 起 0x10 字节清零
    memset(itemParams, 0, sizeof(itemParams));

    CDungeon* dungeon =
        *reinterpret_cast<CDungeon**>(reinterpret_cast<char*>(this) + 0x9f0);
    CParty* party = getParty();

    if (dungeon == 0 || !dungeon->IsOpenScheduleDungeon())
    {
        cMyTrace trace("bool WongWork::CBossTower::checkStartGameCondition("
                       "unsigned char&, bool)", 0xdb, 5);
        trace("boss Tower dungeon is not open");
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x15);
        packet.finalize(true);
        party->send_to_party(packet);
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = party->get_user(i);
        if (member == 0 || !party->checkValidUser(i))
            continue;
        if (member->get_charac_level() < dungeon->get_min_level())
        {
            cMyTrace trace("bool WongWork::CBossTower::checkStartGameCondition("
                           "unsigned char&, bool)", 0xf0, 5);
            trace("dungeon level check charac_level : %d, dungeon_min_level : %d",
                  member->get_charac_level(), dungeon->get_min_level());
            PacketGuard packet;
            packet.put_header(1, 0x10);
            packet.put_byte(0);
            packet.put_int(0xe);
            packet.finalize(true);
            party->send_to_party(packet);
            return 0;
        }
    }

    if (*reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(party) + 0xcd8) == 1)
    {
        return 1;
    }

    if (!party->CheckEnterAncientDungeon(dungeon, flag, items, itemParams))
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x11);
        packet.put_byte(static_cast<unsigned char>(flag));
        packet.finalize(true);
        party->send_to_party(packet);
        return 0;
    }

    if (useItems &&
        !party->UseAncientDungeonItems(dungeon, items, itemParams))
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(1);
        packet.finalize(true);
        party->send_to_party(packet);
        return 0;
    }
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081434f4 _ZN8WongWork10CBossTower15handleStartGameEP6CPartyic17ENUM_DUNGEON_TYPE：
// dungeonType==1 或 partyType==1 → 0；expParamA(+0xaec)=dungeonType、
// difficulty(+0xae8)=(char)b；reset 链 + m_dungeonLog.reset + stageCur=1；
// InitBossTower / checkStartGameCondition 失败 → 0；GM 管理者字节替换广播字节；
// RandomConsistMap 失败 → 0；InitPlayUser(idx 参数, m_idx)；startTick(+0xaf4)=
// getCurTickCount、endTick(+0xaf8)=0；成员名表清零后逐槽位 IncrementBossStageStart
// + IncrementDungeonEntrance + EnterDungeon 日志；SetEPLPState(1)、gen_timer_key(0x2d)、
// SetSelectedEPLPCmd(-1)；包(0,0x1e1){towerIdx, stageMax(short), partyType(byte),
// announceByte(byte)} 广播；SendMapInfoPacket；InsertTimer(OBJ 7, m_idx, msg 0xa5,
// 单人 90s 否则 120s, key 0xa5, 0xa5)；返回 1。
// ----------------------------------------------------------------------------
char CBossTower::handleStartGame(CParty* party, int towerIdx, char b,
                                 ENUM_DUNGEON_TYPE dungeonType)
{
    if (dungeonType == static_cast<ENUM_DUNGEON_TYPE>(1))
        return 0;
    if (*reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(party) + 0xcd8) == 1)
        return 0;

    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0xaec) =
        static_cast<int>(dungeonType);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0xae8) =
        static_cast<int>(b);                   // movsbl

    CBossPlay::reset();
    CBossStage::reset();
    m_dungeonLog.reset();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0xae0) = 1;

    if (!InitBossTower(party, towerIdx))
        return 0;

    unsigned char gmFlag = 0;
    if (!checkStartGameCondition(gmFlag, true))
        return 0;

    CUser* manager = party->getManager();
    char announceByte = b;
    if (manager->isGMUser() && manager->m_field796cc != 0)
    {
        announceByte = manager->m_field796cc;  // GM 观战标记替换广播字节并清位
        manager->m_field796cc = 0;
    }

    if (!RandomConsistMap(*manager))
        return 0;

    InitPlayUser(towerIdx, getIdx());
    unsigned int now = GlobalData::s_systemTime_.getCurTickCount();
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 0xaf4) = now;
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(this) + 0xaf8) = 0;

    char memberNames[256];                     // ebp-0x138 起 0x100 字节清零
    memset(memberNames, 0, sizeof(memberNames));
    getParty()->_getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = getParty()->get_user(i);
        bool valid = (member != 0) && (getParty()->checkValidUser(i) != 0);
        if (!valid)
            continue;

        m_dungeonLog.IncrementBossStageStart(
            *member,
            static_cast<unsigned char>(
                *reinterpret_cast<const int*>(reinterpret_cast<const char*>(this) + 0xae0)),
            static_cast<unsigned char>(
                *reinterpret_cast<const int*>(reinterpret_cast<const char*>(this) + 0xae8)));

        const int partyType = *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(getParty()) + 0xcd8);
        GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
            towerIdx, partyType == 1);

        CDungeon* dungeon =
            *reinterpret_cast<CDungeon**>(reinterpret_cast<char*>(this) + 0x9f0);
        reinterpret_cast<cUserHistoryLog*>(reinterpret_cast<char*>(member) + 0x79700)
            ->EnterDungeon(dungeon->GetDungeonName(),
                           *reinterpret_cast<const int*>(
                               reinterpret_cast<const char*>(this) + 0xae8),
                           memberNames, partyType);
    }

    getParty()->SetEPLPState(1);
    getParty()->gen_timer_key((TIMER_MESSAGE)0x2d);
    getParty()->SetSelectedEPLPCmd(-1);

    PacketGuard packet;
    packet.put_header(0, 0x1e1);
    packet.put_int(towerIdx);
    packet.put_short(*reinterpret_cast<const int*>(
        reinterpret_cast<const char*>(this) + 0xae4));
    packet.put_byte(*reinterpret_cast<const int*>(
        reinterpret_cast<const char*>(getParty()) + 0xcd8));
    packet.put_byte(static_cast<int>(announceByte));
    packet.finalize(true);
    getParty()->send_to_party(packet);

    SendMapInfoPacket();

    int timeoutSec = (getParty()->get_member_count() == 1) ? 0x5a : 0x78;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, getIdx(),
                                (TIMER_MESSAGE)0xa5, timeoutSec, 0xa5, 0xa5);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081454b8 _ZN8WongWork10CBossTower13handleUseItemEP5CUsermh：
// GetMemberSlotNo==-1 → 0；itemId==0xbdd → m_useItemCount[slot]++（ORIG
// this+(slot+0x270)*4+8 = +0x9c8 区）；param==2 → m_memberField[slot]++（+0x9d8）；
// 返回 1。
// ----------------------------------------------------------------------------
int CBossTower::handleUseItem(CUser* user, unsigned long itemId, unsigned char param)
{
    const int slot = static_cast<int>(getParty()->GetMemberSlotNo(user));
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
// SendCmdOkPacket(0x28)；checkAllMemberReady(get_member_count()) 时广播
// 包(0,0x1e1→0,0x1e)/int 0/finalize；返回 1。
// ----------------------------------------------------------------------------
int CBossTower::handleFinishLoading(CUser* user)
{
    CParty* party = getParty();
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
}  // namespace WongWork
