bool DB_LoadEtc::LoadConditionEventInfo(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect current_step, reward_step from event_conditionable_info"
                  " where charac_no=%u", sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    int nRows = db->get_n_rows();   // 后声明→浅槽 -0xc（ORIG 形）
    if (nRows != 1)
    {
        sig->m_curConditionEventStep = 0;            // +0x40c8
        sig->m_curConditionEventRewardStep = 0;      // +0x40ca
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_short(0,
                       *reinterpret_cast<short*>(&sig->m_curConditionEventStep)))
    {
        return false;
    }
    if (!db->get_short(1,
                       *reinterpret_cast<short*>(&sig->m_curConditionEventRewardStep)))
    {
        return false;
    }
    return true;
