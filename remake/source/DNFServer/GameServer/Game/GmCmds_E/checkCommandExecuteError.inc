const char* CMonsterDropTest::checkCommandExecuteError(CParty* party)
{
    if (party == 0)
    {
        return "Party가 없습니다";
    }
    if (m_mode == 1)
    {
        return "user가 지나간 맵을 저장합니다";
    }
    if (m_mode == 2)
    {
        if (10000 < static_cast<unsigned int>(m_count))
        {
            return "passed map 통계는 0 ~ 10000번까지만 지원됩니다";
        }
    }
    else
    {
        if (100000 < static_cast<unsigned int>(m_count))
        {
            return "0 ~ 100000번 통계까지만 지원됩니다";
        }
    }

    // P115：ORIG `mov %eax,-0xc(%ebp); cmpl $0,-0xc(%ebp)` —— find_dungeon
    // 结果有具名局部（无 sete 物化）。
    CDungeon* dungeon = G_CDataManager()->find_dungeon(
        party->m_battleField.get_dungeon_index());
    if (dungeon == 0)
    {
        return "Dungeon에서 사용가능하지 않습니다";
    }
    if (party->m_battleField.m_bloodState == 1)
    {
        return "연습모드 입니다";
    }
    return 0;
