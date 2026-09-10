bool CDungeon::isTournamentDungeon() const
{
    // ORIG 0x0836520a：m_tournamentFlag 非 0 且 (signed char)<=1
    //（1=普通锦标赛，2=特殊，负值视为普通）
    if (m_tournamentFlag != 0 && (char)m_tournamentFlag <= 1)
        return true;
    return false;
