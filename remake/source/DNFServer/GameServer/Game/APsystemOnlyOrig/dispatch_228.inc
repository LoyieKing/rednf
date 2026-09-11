// ============================================================================
// CLocalChina_DB_LastDay_BreakAway::dispatch —— ORIG 0x0812801a
// 读 accId、flag、day；flag==0 → quest_time 回退 day 天；flag==1 →
// last_reward_time 回退 day 天。失败 log 返 0。
// ============================================================================
bool CLocalChina_DB_LastDay_BreakAway::dispatch(int userIdx, int slot,
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x0812801a 帧：db(-0xc) id(-0x10) day(-0x14) flag(-0x18)；
    // 本 TU 实测（DeleteChannel/此函数双探针）4.4 对「声明即初始化」
    // 局部按声明序近→远落槽 ⇒ 声明序 id,day,flag（技巧21 按实证方向）。
    unsigned int m_id = 0;
    int m_day = 0;
    int m_flag = 0;
    *stream >> m_id;
    *stream >> m_flag;
    *stream >> m_day;
    // ORIG 0x08128097-0x08128097 switch(int) 两 case 链形：`mov flag,%eax;
    // test; je A; cmp $1,%eax; je B; jmp END`（4.1 线性 case 下探）。4.4 的
    // if/else-if 产 test+jne+二次装载形 ⇒ 直接写 switch。
    switch (m_flag)
    {
    case 0:
        db->set_query(
            "UPDATE churn_member_info \t\t\t\t  SET quest_time = UNIX_TIMESTAMP(SUBDATE(FROM_UNIXTIME(quest_time), INTERVAL %d DAY)) \t\t\t\t   WHERE m_id=%s",
            m_day, NumberToString(m_id, 0));
        break;
    case 1:
        db->set_query(
            "UPDATE churn_member_info \t\t\t\t  SET last_reward_time = UNIX_TIMESTAMP(SUBDATE(FROM_UNIXTIME(last_reward_time), INTERVAL %d DAY)) \t\t\t\t   WHERE m_id=%s",
            m_day, NumberToString(m_id, 0));
        break;
    }
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localchina/localchina_DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x42f, "[GM]CLocalChina_DB_LastDay_BreakAway Fail!!");
        return 0;
    }
    return 1;
}