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
// [b28d-54 N + P0 清绑] ORIG 0x081434f4：本波清除 3 处 `register asm("eax")` 绑定
//   （N96 违禁）→ 自然 `if(!f()) return 0;`；残余帧 0x15c→0x16c + 全帧槽平移（N82）
//   + int callee 调用点 sete 物化（P168a 定论，N18b）。调用序列全等（301 vs 302）。
// [b28d-solo-CBossTower] AE（302:302，本波翻正）：①IncrementDungeonEntrance 块序
//   反写（ORIG je→(towerIdx,1)）+ timeoutSec 两段式（P198③）+ 调用点 `!f()`
//   （本类 bool 化）；②CBossStage.h m_expParamA 匿名 union 双视角（P235/P211：
//   ORIG 0x08143531 LHS-first ⇒ 字段与 ENUM_DUNGEON_TYPE 同型）+ 赋值走
//   m_expParamAE；③m_difficulty/m_bloodState 同法（CBattle_Field.h:473 union，
//   EnterDungeon 实参走 m_bloodStateE ⇒ 两段 `mov 0xcd8(%eax),%eax; mov %eax,%esi`）。
//   CBossStage.h 补 ENUM_DUNGEON_TYPE guard 同体声明（CParty.h 约定）。
char CBossTower::handleStartGame(CParty* party, int towerIdx, char b,
                                 ENUM_DUNGEON_TYPE dungeonType)
{
    if (dungeonType == static_cast<ENUM_DUNGEON_TYPE>(1))
        return 0;
    if (party->m_battleField.m_bloodState == 1)
        return 0;

    // P211 档③：ORIG 0x08143531 为 LHS-first ⇒ 字段与 RHS 同型（enum 视角
    // m_expParamAE，与形参 ENUM_DUNGEON_TYPE 同型；int 视角 + cast 两向实测
    // 均 RHS-first，4.4 看穿 cast）。
    m_expParamAE = dungeonType;
    m_difficulty = static_cast<int>(b);        // movsbl

    CBossPlay::reset();
    CBossStage::reset();
    m_dungeonLog.reset();
    m_stageCur = 1;

    // [b28d-54 P0 清绑] 原 X1 形用 `register asm("eax")` 绑定复现 ORIG 的
    // `call; xor $1; test; je`——违 N96/P0（全域禁 register asm）。按 P168a
    // 定论改自然 `if(!f()) return 0;`（int callee 调用点无法产 xor，属 N 封顶）。
    if (!InitBossTower(party, towerIdx))
        return 0;
    unsigned char gmFlag = 0;
    if (!checkStartGameCondition(gmFlag, true))
        return 0;
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
    if (!RandomConsistMap(*manager))
        return 0;
    InitPlayUser(towerIdx, m_idx);
    m_towerStartTick = GlobalData::s_systemTime_.getCurTickCount();
    m_towerEndTick = 0;

    char memberNames[256] = {0};             // ebp-0x138（内联展开清零 0x100）
    m_party->_getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081436f8..0x08143704：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        m_dungeonLog.IncrementBossStageStart(
            *member,
            static_cast<unsigned char>(m_stageCur),
            static_cast<unsigned char>(m_difficulty));

        // ORIG 0x0814376b..0x0814378f：`je <towerIdx,1 块>` + fall-through
        // (towerIdx,0) ⇒ 反写条件（!=1 → 0；else → 1）复现块序。
        if (m_party->m_battleField.m_bloodState != 1)
            GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(towerIdx, 0);
        else
            GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(towerIdx, 1);

        // ORIG 0x0814377x 段 m_bloodState 读为两段（`mov 0xcd8(%eax),%eax;
        // mov %eax,%esi`）⇒ enum 视角 → int 形参（P254/P265 反向）。
        member->m_historyLog.EnterDungeon(m_dungeon->GetDungeonName(),
                                          m_difficultyE, memberNames,
                                          m_party->m_battleField.m_bloodStateE);
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

    // ORIG 0x08143909..0x0814391b：movl $0,-0x24 预清 + sete/test/je 双赋值
    // ⇒ 声明=0 + if/else 两赋值（P198③），勿用三目（4.4 产 eax 中转）。
    int timeoutSec = 0;
    if (m_party->get_member_count() == 1)
        timeoutSec = 0x5a;
    else
        timeoutSec = 0x78;
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa5, timeoutSec, 0xa5, 0xa5);
    return 1;
}