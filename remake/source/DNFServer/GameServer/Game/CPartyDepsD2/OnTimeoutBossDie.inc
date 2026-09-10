int CParty::OnTimeoutBossDie(int monsterId, unsigned int characNo)
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
    char dieBuf[0xa3f];
    char dieFlag = 0;
    for (std::vector<int>::iterator it = mobIds.begin(); it != mobIds.end(); it++)
    {
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
