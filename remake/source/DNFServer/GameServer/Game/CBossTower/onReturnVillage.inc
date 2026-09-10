int CBossTower::onReturnVillage()
{
    if (m_party != 0 && m_party->GetEPLPState() == 2)
    {
        char memberNames[255] = {0};           // ebp-0x120（ORIG 0x08144fd9 单段
                                               // rep stos 0x3f dwords）

        const char partyTypeByte =
            static_cast<char>(m_party->m_battleField.m_bloodState);   // 低字节 movsbl
        m_party->getMemberNames(memberNames);

        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = m_party->get_user(i);
            // ORIG 0x08145094..0x081450a0：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
            if (member == 0 || m_party->checkValidUser(i) != 1)
                continue;
            member->m_historyLog.LeaveDungeon(m_dungeon->GetDungeonName(),
                                              m_difficultyE, memberNames,
                                              static_cast<int>(partyTypeByte));
        }
        m_party->ReturnToVillage();
        _destroy();
    }
    return 1;
