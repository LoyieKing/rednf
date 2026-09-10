void TeamInfo::refresh()    // ORIG T 0x0855c9ae
{
    for (int i = 0; i <= 3; ++i) {
        if (m_players[i].m_flag != 0) {
            m_players[i].m_accId = m_players[i].m_user->get_acc_id();
            m_players[i].m_rating = m_players[i].m_user->GetRating();
            m_players[i].m_schoolNo = m_players[i].m_user->GetSchoolNo();
            m_players[i].m_user->GetDailyBadge(m_players[i].m_badges);
        }
    }
