void CharacAdvanceAltarManager::refillTicketFree()
{
    for (int i = 0; i < 1; ++i)
    {
        // [b28d-solo-advancealtar_DB] P183：ORIG 0x081315aa 段每次访问都重发
        //   `mov i,%eax; mov this,%edx; imul $0xd0,%eax,%eax; lea (%edx,%eax,1),%eax`
        //   全链（三处 m_data[i] 各自展开），常量下标 m_data[0] 会被折成单 disp。
        if (isValidRidableId(static_cast<advancealtar::RidableId::T>(m_data[i].m_ridableId)))
        {
            m_data[i].m_ticket = G_CDataManager()->m_altarConfig.m_0;
            m_data[i].m_update = 1;
        }
    }
