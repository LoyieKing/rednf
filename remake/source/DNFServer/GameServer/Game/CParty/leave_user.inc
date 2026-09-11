// b28d-solo-CParty: N 封顶 —— N82 帧 0x16c/0x19c；518 vs 473
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
    // [待 .h 属主·只报告] ORIG 0x0859c1b2 前为 `call IsOpen; test %al,%al; je`
    // （P292 档① 单字节直判，无 setne 归一化）⇒ `SECRET_SHOP_DATA::IsOpen()` 应返
    // **bool**；我方现因 int 返值多 2 条（`test %eax,%eax; setne %al; test %al,%al`）
    // —— 本函数剩余 gap −2 全部来源。改 .h 后预计 474/474。
    if (m_secretShop.IsOpen())
    {
        // [P-L11] ORIG 0x0859c18a..0x0859c1ad：GetUserPosInParty 结果**无槽**，
        // 直进 SetBuying 的 0x4(%esp)；旧码 `int pos` 多 1 store + 1 load。
        m_secretShop.SetBuying(user->GetUserPosInParty(), false);
    }
    if (m_battleField.m_pDungeon != 0)
    {
        // [2026-09-10 试形 P312/P78] ORIG 该点无 memset call（p167 EXTRA memset）；
        // `char[0x100] = {0}` 令 4.4 内联 rep stos（OnFightVillageMonster 同族实证）。
        char names[0x100] = {0};
        // [P-L11] ORIG 0x0859c1d9..0x0859c23a：四实参**全内联**（edi=partyType、
        // esi=memberNames、ebx=diff、eax=dname 寄存器保活，无槽），求值序右→左；
        // 旧码四个具名局部多 4 store + 4 load。
        user->m_historyLog.LeaveDungeon(
            m_battleField.m_pDungeon->GetDungeonName(),
            reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_diff(),
            _getMemberNames(names),
            m_battleField.m_bloodState);
        // [负结论·2026-09-11 本轮实测] 全函数仅剩 1 条（我方 473 vs ORIG 474）：
        // ORIG 0x0859c1d9..0x0859c1e2 `mov 0xcd8(%eax),%eax; mov %eax,%edi`
        // （partyType 经 eax 中转到实参寄存器）vs 我方 `mov 0xcd8(%eax),%edi` 一步
        // ⇒ 4.1 实参寄存器分配差，源码层无对应形，判 N。
        // [P0/P329] 同 ReturnToVillage：短路直跳，无 bool 槽（ORIG 两 test 直判）。
        if (IsSinglePlay() && user->isDungeonClearHackCheck())
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
        // [P227/N83] ORIG 无 placement-new 且三实参在 ctor 前右→左求值、跨 ctor 保活
        //（无具名槽）⇒ 全表达式临时链（无具名 trace 对象）。
        cMyTrace(__PRETTY_FUNCTION__, 0x5b8, 0)(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            NumberToString(user->get_acc_id(), 0), user->get_state(),
            (int)user->GetPartyIndex());
        if (count < 1)
        {
            // [P-L11] ORIG 0x0859c3c5..0x0859c3e5：cState/pIndex/dIdx 三实参**全内联**
            // （movzbl 0x6a+movsbl / mov 0x74 / movzwl 0x13e 两级零扩展），无具名槽；
            // 旧码三个具名局部多 3 store + 3 load（我方帧 -0x28 系槽）。
            LogManager::logFormat(1, "party.cpp",
                __PRETTY_FUNCTION__, 0x5c5,
                "[PARTYPOOL] member_count : %d, m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d",
                count, m_manager, (unsigned int)(unsigned short)m_dungIndex,
                m_partyIndex, (int)(char)m_state);
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
}