bool DB_Login::GetBadgeCount(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10, E_SERVER_GROUP_NONE);
    for (int i = 0; i < 3; ++i)
    {
        sig->m_dailyBadge[i] = 0;                  // +0x399c green/red/blue 清零
    }
    int initTime = getDailyInitializeTime(6, 0, 0);
    db->set_query(
        "seLect daily_green_badge, daily_red_badge, daily_blue_badge from school_member where school_id=%d and m_id = %s and  last_play_time >= from_unixtime(%d)",
        sig->m_schoolNo,
        NumberToString(sig->m_accId, 0), initTime);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    // [b28d-solo-DB_Login] P0/P244：ORIG 无 pretty/green/red/blue 具名局部——
    // get_short 直接传 sig->m_dailyBadge[i]（`add $0x399c,%eax`），trace 用
    // 全表达式临时链（sig 入 %ebx 跨 ctor，P87/P227）。
    if (!db->get_short(0, sig->m_dailyBadge[0]))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3b60, 5)(
            "DB_Login::GetBadgeCount, get(0) ERROR m_id=%s",
            reinterpret_cast<const char*>(sig));
        return 0;
    }
    if (!db->get_short(1, sig->m_dailyBadge[1]))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3b66, 5)(
            "DB_Login::GetBadgeCount, get(1) ERROR m_id=%s",
            reinterpret_cast<const char*>(sig));
        return 0;
    }
    if (!db->get_short(2, sig->m_dailyBadge[2]))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3b6b, 5)(
            "DB_Login::GetBadgeCount, get(2) ERROR m_id=%s",
            reinterpret_cast<const char*>(sig));
        return 0;
    }
    return 1;
}