// ---------------------------------------------------------------------------
// CParty::OnTimeoutBossDie —— ORIG T 0x085aa5d4（报告 OnTimeoutBossDie.md）：
//   超时击杀当前 Boss（BF.m_apcAllKilledSec）：遍历怪物→击杀→失败按队伍类型收尾。
// [b28d-solo-CPartyDepsD2] gap -1→0（369=369），p167 真缺口归零（仅余 C1/C2、
// D1/D2 同址别名，P93）。本轮真修：①[真缺口] 两处 InsertTimer 的 index 实参由
// `m_partyIndex` 直读改 out-of-line `GetPartyIndex()`（ORIG 0x085aa90d/0x085aaa49
// call）；②[真缺口] 两循环增量 `++it`→`it++` 复现 ORIG ppEi（P28）。
// 形态（P24/P171/P227）：两处 key 内联实参、trace 改全表达式临时链、
// 删两处 `m_pDungeon != 0` 过度防御（ORIG 0x085aa9d7/0x085aaa9b 直接解引用
// 0xcac，无判空）。残余：map_monster 0x34c 临时 vs dieBuf 0xa3f 槽复用方向差
// （ORIG 帧 0xac0；4.4 不复用 ⇒ 0xe10，N82/N86）。语义/调用集齐，byte 层封顶。
// ---------------------------------------------------------------------------
bool CParty::OnTimeoutBossDie(int monsterId, memberIdentificationNumber_t characNo)
{
    if (bf(this).m_apcAllKilledSec != monsterId)
        return 0;
    CUser* user = GetMemberSlotNo(characNo);
    if (user == 0)
        return 0;
    if (m_battleField.m_pDungeon != 0 && m_battleField.m_pDungeon->IsEnterEachMap())
        bf(this).SetCurSeatNo(get_party_seatno(user));
    // [b28d-solo-CPartyDepsD2] ORIG 0x085aa5d4 语义照抄（Ghidra C 逐调用还原）：
    // ①第一循环：GetMonsterMap()->begin()/end()（每轮重调），map_monster 临时
    //   赋值 it->second，m_dropCnt(+0x2c)!=0 时 push_back(m_instId(+0x04))；
    // ②第二循环：vector 迭代器（非索引），每轮 memset(dieBuf,0,0xa3f)；
    //   空体 if 内真语义 = mit->second.m_active.getHpMax() 死写 -0x8e（P171）；
    // ③dieFlag(-0x9) 头部预清、成功后置 1（死变量照抄）。
    std::vector<int> mobIds;
    for (std::map<int, map_monster>::iterator it = bf(this).GetMonsterMap()->begin();
         it != bf(this).GetMonsterMap()->end(); it++)   // ORIG ppEi（P28 后置增量）
    {
        map_monster mob;
        mob = it->second;
        if (mob.m_dropCnt != 0)
            mobIds.push_back(mob.m_instId);
    }
    char dieFlag = 0;
    for (std::vector<int>::iterator it = mobIds.begin(); it != mobIds.end(); it++)
    {
        // [g21E 试形] dieBuf 移入循环体：与第一循环的 map_monster mob 作用域互斥
        // ⇒ 复现 ORIG 的槽复用帧 0xac0（4.4 基线不复用 ⇒ 0xe10）。
        unsigned int dieBufAlignPad;
        (void)sizeof(dieBufAlignPad);
        char dieBuf[0xa3f];
        memset(dieBuf, 0, sizeof(dieBuf));
        std::map<int, map_monster>::iterator mit = bf(this).GetMonsterMap()->find(*it);
        if (mit != bf(this).GetMonsterMap()->end())
        {
            const int hpMax = mit->second.m_active.getHpMax();
            (void)hpMax;
        }
        if (kill_monster(user, *it, 0xffff,
                         *reinterpret_cast<const MSG_MONSTER_DIE*>(dieBuf), 0) != 1)
        {
            // P227：ORIG 0x085aa886 先算 *it 存 %ebx，再 ctor（0x085aa8b1）。
            cMyTrace(__PRETTY_FUNCTION__, 0x22ce, 0)(
                "KILLBOSS2 server - monster id(%d) failed", *it);
            if (m_battleField.m_bloodState == 1)
            {
                SetEPLPState(2);
                // P24：ORIG 0x085aa905 key→%esi、0x085aa912 index→%ebx 跨 G_TimerQueue()。
                G_TimerQueue()->InsertTimer(
                    (TimerEntry::OBJ_TYPE)1, GetPartyIndex(), (TIMER_MESSAGE)0x15,
                    0x3c, (int)(unsigned int)gen_timer_key((TIMER_MESSAGE)0x15), 0);
                set_state(1);
                PacketGuard packet;
                packet.put_header(0, 0xfe);
                packet.finalize(true);
                send_to_party(packet);
            }
            else if (m_battleField.m_pDungeon->m_bloodDungeonType < 1)
            {
                PacketGuard packet;
                packet.put_header(0, 0x1f);
                packet.finalize(true);
                send_to_party(packet);
            }
            else
            {
                bf(this).m_bloodReward.onFinishBloodRound(
                    true, (short)bf(this).m_bloodRound.getBloodRoundOrder(),
                    this, m_battleField.m_pDungeon);
                G_TimerQueue()->InsertTimer(
                    (TimerEntry::OBJ_TYPE)1, GetPartyIndex(), (TIMER_MESSAGE)0x22,
                    5, (int)(unsigned int)gen_timer_key((TIMER_MESSAGE)0x22), 0x22);
            }
            break;
        }
        dieFlag = 1;
    }
    (void)dieFlag;
    return 1;
}