void TeamInfoSecond::refresh()
{
    CUser* last = 0;
    for (int i = 0; i < 4; ++i)
    {
        /* ORIG @0855fd33 mov last 后 @0855fd36 jmp 出循环 = break【Ghidra
         * goto LAB_0855fd47 实证】；双 test;je 直指 addl 块 = && 短路 */
        if (m_seats[i].m_flag != 0 && m_seats[i].m_user != 0)
        {
            last = m_seats[i].m_user;
            break;
        }
    }
    if (last == 0)
        return;

    m_teamId = last->GetCurCharacOnlinePreliminaryTeamId();  // T 0x8645de2
    if ((char)online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam() == 1)
        m_rating = reinterpret_cast<online_preliminary::COnlinePreliminaryMember*>(
                       last->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))
                       ->GetRating();
    else if (last->GetCurCharacOnlinePreliminaryTeam() != 0)
        m_rating = last->GetCurCharacOnlinePreliminaryTeam()->GetRating();
