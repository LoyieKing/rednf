int CParty::leave_user(CUser* user, ENUM_PARTY_INFO_TYPE type)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 EXTRA GetSecretShopData/memset/new/delete/_Unwind_Resume
    // 5 项（P90 landing + GetSecretShopData 取址形）。@0x859c114
    if (user == 0)
    {
        LogManager::logFormat(1, "party.cpp",
            __PRETTY_FUNCTION__, 0x56d,
            "CParty::leave_user pUser is NULL");
        return 1;
    }
    ((CPartyTelePort*)GetPartyTelePort())->process_leave_user_at_teleport(user);
    // ORIG 0859c14x `mov 0x8(%ebp),%eax; add $0x2a4,%eax` ⇒ 直接取 &m_padShop
    //（P167 EXTRA GetSecretShopData 已消：原 `GetSecretShopData()` 形多发一次 call）。
    if (m_secretShop.IsOpen())
    {
        int pos = user->GetUserPosInParty();
        m_secretShop.SetBuying(pos, false);
    }
    if (m_battleField.m_pDungeon != 0)
    {
        // [2026-09-10 试形 P312/P78] ORIG 该点无 memset call（p167 EXTRA memset）；
        // `char[0x100] = {0}` 令 4.4 内联 rep stos（OnFightVillageMonster 同族实证）。
        char names[0x100] = {0};
        int partyType = m_battleField.m_bloodState;
        char* memberNames = _getMemberNames(names);
        int diff = reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_diff();
        const char* dname = m_battleField.m_pDungeon->GetDungeonName();
        user->m_historyLog.LeaveDungeon(dname, diff, memberNames, partyType);
        bool b;
        if (!IsSinglePlay() || !user->isDungeonClearHackCheck())
        {
            b = false;
        }
        else
        {
            b = true;
        }
        if (b)
        {
            user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x19c, 1, 0, 0);
        }
    }
    if (get_quick_party_index() != -1)
    {
        G_CGameManager()->CheckOutQuickParty(this, false);
    }
    if (is_quick_party())
    {
        send_quick_party_matching_result(false, user);
        if (get_member_count() == 2)
        {
            set_quick_party(false);
        }
    }
    int count = get_member_count();
    if (count < 2)
    {
        short partyIdx = user->GetPartyIndex();
        int userState = user->get_state();
        unsigned int accId = user->get_acc_id();
        char* accIdStr = NumberToString(accId, 0);
        // [P227/N83] ORIG 无 placement-new（p167 EXTRA _ZnwjPv/_ZdlPvS_/_Unwind_Resume
        //   即旧码 `new (log) cMyTrace(…)` 残留）⇒ 改真实具名对象（lea 取址构造 + op()）。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x5b8, 0);
        trace(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            accIdStr, userState, (int)partyIdx);
        if (count < 1)
        {
            int cState = (char)m_state;
            int pIndex = m_partyIndex;
            unsigned short dIdx = m_dungIndex;
            LogManager::logFormat(1, "party.cpp",
                __PRETTY_FUNCTION__, 0x5c5,
                "[PARTYPOOL] member_count : %d, m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d",
                count, m_manager, (unsigned int)dIdx, pIndex, cState);
            for (int i = 0; i <= 3; ++i)
            {
                if (m_member[i].m_pUser == 0)
                {
                    LogManager::logFormat(1, "party.cpp",
                        __PRETTY_FUNCTION__, 0x5cd,
                        "User[%d] NULL, ch_state NONE, uid : %d, charac_no : 0",
                        i, m_member[i].m_field8);
                }
                else
                {
                    LogManager::logFormat(1, "party.cpp",
                        __PRETTY_FUNCTION__, 0x5d8,
                        "User[%d] %x, ch_state %d, uid : %d / %d, charac_no : %u",
                        i, m_member[i].m_pUser, m_member[i].m_pUser->get_state(),
                        m_member[i].m_pUser->GetUID(), m_member[i].m_field8,
                        m_member[i].m_pUser->getCurCharacNo());
                }
            }
        }
        user->SetPartyIndex(-1);
        user->set_charac_party_bonus_exp(0);
        user->set_charac_member_bonus_exp(0);
        user->set_charac_fatigue_buf_bonus_exp(0);
        user->set_charac_seria_buf_bonus_exp(0);
        if (user->getMoveSpace() != (UserSpace::T)1)
        {
            user->set_state(ch_state3);
        }
        destroy();
        return 0;
    }
    else
    {
        if (user != 0)
        {
            user->getHackAnalyzer()->reportHackInfo();
            user->getHackAnalyzer()->resetHackInfo();
        }
        if (m_manager == user)
        {
            change_manager();
            G_GameWorld()->send_party_info_to_all(this, (int)type);
            send_party_realtime_info();
            sendInoutConditionDungeon((RetryFailReason::T)checkInoutConditionDungeon());
            return 0;
        }
        else
        {
            for (int i = 0; i <= 3; ++i)
            {
                if (_checkValidUser(i) != 1)
                    continue;
                if (m_member[i].m_pUser == user)
                {
                    m_member[i].m_pUser = 0;
                    m_member[i].m_field8 = 0;
                    user->SetPartyIndex(-1);
                    user->set_charac_party_bonus_exp(0);
                    user->set_charac_member_bonus_exp(0);
                    user->set_charac_fatigue_buf_bonus_exp(0);
                    user->set_charac_seria_buf_bonus_exp(0);
                    if (user->getMoveSpace() != (UserSpace::T)1)
// b28d-solo-CParty: AE —— P226 语句表达式消 bool 槽 + reward 直型 EventNewCharacterReward* 消 edx 中转；81=81
                    {
                        user->set_state(ch_state3);
                    }
                    G_GameWorld()->send_party_info_to_all(this, (int)type);
                    send_party_realtime_info();
                    sendInoutConditionDungeon((RetryFailReason::T)checkInoutConditionDungeon());
                    return 0;
                }
            }
            return 1;
        }
    }
