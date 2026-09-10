void CParty::TournamentDungeonClear(bool flag)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85bd236
    // ORIG 0x085bd24x：`mov 0xcac(%eax),%eax; mov %eax,-0x1c(%ebp);
    // cmpl $0x0,-0x1c(%ebp); jne` + 后续 `mov -0x1c(%ebp),%eax` 复用 ⇒ 原码有
    // 具名 CDungeon* 局部（P0 槽往返），非表达式直读。
    CDungeon* dungeon = m_battleField.m_pDungeon;                      // +0xcac
    if (dungeon == 0)
    {
        LogManager::logFormat(
            1, "party.cpp", __PRETTY_FUNCTION__, 0x45bc,
            "Tournament Dungeon is NULL");
        return;
    }
    // ORIG 0x085bd3aa `call isTournamentDungeon; xor $0x1,%eax; test %al,%al; je <主体>`
    // ⇒ 原码是**否定守卫早退**（`!= 1` → xor 物化 + 错误块直落 fall-through），
    // 不是 if/else 正极性（正极性产 `test; je <错误块>` 且块序相反）。
    if (dungeon->isTournamentDungeon() != 1)
    {
        // P0：ORIG 085bd3aa 对 get_dungeon_index 重新 add $0xb24（不缓存 bf 指针）。
        LogManager::logFormat(
            1, "party.cpp", __PRETTY_FUNCTION__, 0x45c2,
            "Dungeon(%d) is can not call function",
            m_battleField.get_dungeon_index());
        return;
    }
    // ORIG：两句式 if/else 各一次 SaveCoin（两个独立 call 岛），非三元表达式。
    if (flag)
        SaveCoin((eCoinSubReason)1);
    else
        SaveCoin((eCoinSubReason)2);
        PacketGuard packet;
    for (int slot = 0; slot < 4; ++slot)                // -0x18
    {
        if (_checkValidUser(slot))
        {
            Inven_Item items[2];                        // -0xa6
            // ORIG 085bd3xx `call; mov %al,-0xd(%ebp); cmpb $0x0,-0xd(%ebp)` =
            // 具名 **char** 局部（截断直存、无 setne 归一化）⇒ 声明 char 非 bool。
            char got = (char)m_battleField.m_tournamentReward.GetItemFromCard(
                ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, slot, items, 2);
            if (got != 0)
            {
                _putItemIntoUser(m_member[slot].m_pUser, items, packet);
            }
            if (flag)
            {
                for (int i = 0; i < 2; ++i)
                    items[i].reset();
                char got2 = (char)m_battleField.m_tournamentReward.GetItemFromCard(
                    ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, slot, items, 2);
                if (got2 != 0)
                {
                    _putItemIntoUser(m_member[slot].m_pUser, items, packet);
                }
                HistoryLog::WriteDunClear(
                    (FILE*)m_member[slot].m_pUser->m_field796f8);
                OnSuccessedDungeonClear(m_member[slot].m_pUser);
                if (m_member[slot].m_pUser != 0 &&
                    m_member[slot].m_pUser->IsProperLevelDungeonUser())
                {
                    m_member[slot].m_pUser->ProcProperLevelDungeonClear();
                }
            }
            else
            {
                OnFailedDungeonClear(m_member[slot].m_pUser);
            }
        }
    }

    SetEPLPState(2);
    int sec = 0x3c;                                     // -0x1c
    if (m_secretShop.IsOpen())
        // ORIG TournamentDungeonClear.md 085bd5b9 `mov 0xa784(%eax)` =
        // CDataManager::m_secretShop(+0xa700)+0x84 → SecretShopScript::m_int84
        //（CDataManager.h 布局注释：clear 0x8372020 写 +0x80/+0x84 两 int）
        sec = G_CDataManager()->m_secretShop.m_int84 * 0x3c;
    int key = gen_timer_key((TIMER_MESSAGE)0x15);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                (TIMER_MESSAGE)0x15, sec, key, 0);
    set_state(1);
    sendInoutConditionDungeon((RetryFailReason::T)checkInoutConditionDungeon());
    CheckHackAverageHitCount();
    ClearMapHitCount();
    m_secu.dungeonClear();
