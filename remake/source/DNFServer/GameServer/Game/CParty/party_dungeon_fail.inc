// ============================================================================
// CParty::party_dungeon_fail —— ORIG 0x85b3670：全灭败北处理（计时差记录、
// 逐成员退场与惩罚、状态复位或解散；锦标赛路径改发奖励）。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x6c/0x8c + P167 EXTRA new/delete landing（P90/P83）；463 vs 444
// [轴 B 自校 N] tsV 两行（0x85b39c5/0x85b3be8）系**序号错配假阳性**：我方循环尾
// 实为 `addl $1,-0x2c; cmpl $0x3,-0x2c; setle` = ORIG 的 setle（槽号 -0x2c vs
// ORIG -0x28 致错配）。未改源码。
void CParty::party_dungeon_fail()
{
    // b28d-49 判词：N 封顶 + 缺口。P167 EXTRA new/delete 2 项（P90/P83 landing 差）。@0x85b3670
    // [G7-sub] 语义等价形修正：ORIG 三条**独立早退**，各自跳不同尾标
    // （0x085b3685 m_tournamentVictory!=0 → +0x611；0x085b369b !check_allmember_die()
    // → +0x614；0x085b36ac m_pDungeon==0 → +0x617），无 bool 物化；我方原写复合
    // `!(a && b && c)` 被 4.4 折成三段 `mov $1/jmp/mov $0/test` 物化（+9 条来源）
    // ⇒ 拆回三条独立 if-return。
    if (m_tournamentVictory != 0)
        return;
    if (!check_allmember_die())
        return;
    if (m_battleField.m_pDungeon == 0)
        return;

    // P183：ORIG 本函数对 bf/bd 每个使用点各自重取址（无引用缓存槽）⇒ 宏直用。
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
#define bd (m_battleData)
    char skipPanalty = 0;                               // -0x31
    CDungeon* dungeon = m_battleField.m_pDungeon;                      // -0x30
    if (dungeon != 0)
    {
        // ORIG party_dungeon_fail.md 085b36cb `movzbl 0x89f(%eax)` =
        // CDungeon::m_bloodDungeonType（unsigned char，宽度修正：原 char→movsbl 异形）
        skipPanalty = (dungeon->m_bloodDungeonType > 0) ? 1 : 0;
        if (dungeon->isTowerOfDespairDungeon())
            skipPanalty = 1;
    }
    // ORIG 085b36f0 `movzwl 0xc4c(%eax)` = m_villageMonsterId（非 0 判）
    if (reinterpret_cast<CBattle_Field*>(&m_battleField)->m_villageMonsterId != 0)
        skipPanalty = 1;
    if (dungeon != 0 &&
        (dungeon->get_dimension_possible() >= 1 ||
         // ORIG 085b3718 `movzbl 0x89c(%eax)` = CDungeon::m_89c（unsigned char，
         // 宽度修正：原 (char*)→movsbl 异形）
         dungeon->m_89c != 0))
        skipPanalty = 1;

    if (dungeon->isTournamentDungeon())
    {
        bf.m_tournamentReward.ResetReward();
        MakeTournamentReward(bf.m_tournamentReward);
        return;
    }

    unsigned int now =
        (unsigned int)GlobalData::s_systemTime_.getCurTickCount();
    unsigned int elapsedLow = now - (unsigned int)bd.m_field20;
    bd.m_field0 = (int)(elapsedLow - (unsigned int)bd.m_field2c);
    bd.m_field4 =
        (int)((0u - (unsigned int)(now < (unsigned int)bd.m_field20)
               - (unsigned int)bd.m_field24 - (unsigned int)bd.m_field30)
              - (unsigned int)(elapsedLow < (unsigned int)bd.m_field2c));
    SaveCoin((eCoinSubReason)2);
    CheckHackAverageHitCount();
    m_secu.dungeonFail();

    PacketGuard packet;
    for (int slot = 0; slot < 4; ++slot)                // -0x2c
    {
        if (_checkValidUser(slot))
        {
            CUser* user = m_member[slot].m_pUser;
            OnFailedDungeonClear(user);
            G_GameWorld()->out_from_dungeon(user);
            if (user->getMoveSpace() != (UserSpace::T)1)
            {
                GetInstanceDungeonEntranceLog()->DecrementDungeonEntrance(
                    dungeon->get_index() != 0, true);
            }
            if (user->getMoveSpace() == (UserSpace::T)1)
            {
                BlueMarble* marble =
                    reinterpret_cast<BlueMarble*>(user->getBlueMarble());
                if (marble != 0)
                    marble->giveupDungeon(user);
            }
            if (m_battleField.m_bloodState != 1)
            {
                // ORIG 085b3952 `movzbl 0x85c` = m_entryRestrictFlag；
                //      085b396e `movzbl 0x87a` = m_villageAttackEventFlag
                //（两处 unsigned 读，与 CParty.cpp:2303/2319 既有
                //  `(unsigned char)` 消费形态同口径）
                char has85c =
                    (dungeon != 0 &&
                     (unsigned char)dungeon->m_entryRestrictFlag != 0) ? 1 : 0;
                char has87a =
                    (dungeon != 0 &&
                     (unsigned char)dungeon->m_villageAttackEventFlag != 0) ? 1 : 0;
                if (skipPanalty != 1 && has85c != 1 && has87a != 1)
                    user->giveup_panalty();
            }
        }
    }

    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short((short)get_member_count());
    if (m_autoCreated == 0)                             // +0x13c
    {
        for (int i = 0; i < 4; ++i)
        {
            if (_checkValidUser(i))
                m_member[i].m_pUser->make_basic_info((char*)&packet, 0);
        }
        set_state(1);
        battle_data_reset();
        packet.finalize(true);
        if (!IsExistInvisible())
            G_GameWorld()->send_all(packet);
        else
            send_to_party(packet);
    }
    else
    {
        short mgrIndex = m_manager->GetPartyIndex();
        int mgrState = m_manager->get_state();
        unsigned int accId = m_manager->get_acc_id();
        char* idStr = NumberToString(accId, 0);
        // [P227/N83] 删 placement-new（ORIG 无 _ZnwjPv/_ZdlPvS_）⇒ 具名对象直构。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3541, 0);
        trace(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            idStr, mgrState, (int)mgrIndex);
        m_manager->SetPartyIndex(-1);
        if (getManager()->getMoveSpace() != (UserSpace::T)1)
            m_manager->set_state((ch_state)3);
        m_manager->make_basic_info((char*)&packet, 0);
        destroy();
        packet.finalize(true);
        if (getManager()->getMoveSpace() == (UserSpace::T)1)
            send_to_party(packet);
        else
            // [R5BigTURest 偏移轴·接收者错 2026-09-11] ORIG 0x085b3b59-5c 逐字节：
            // `8b 45 08` (mov 0x8(%ebp),%eax = this) + `8b 58 74` (mov 0x74(%eax),%ebx)
            // ⇒ 实参 = **m_manager**（+0x74），非 m_host（+0x70）。三重旁证：ORIG
            // party_dungeon_fail 内 0x74 出现 7 次 / 0x70 出现 0 次；两侧
            // CParty::getManager() 均读 0x74（m_manager 布局一致）；本分支其余
            // 6 处（mgrIndex/mgrState/accId/SetPartyIndex/set_state/make_basic_info）
            // 全用 m_manager。我方原写 m_host ⇒ **send_all 第 2 参 CUser* 取错对象**。
            G_GameWorld()->send_all(packet, m_manager);
    }
}