bool DB_SaveConditionEventInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_CONDITION_EVENT* sig = stream->GetOutBuffer<SIG_SAVE_CONDITION_EVENT>();

    db->set_query(
        "upDate event_conditionable_info set current_step=%d, reward_step=%d, update_time=now() where charac_no=%u",
        (int)sig->m_a2, (int)sig->m_a3, sig->m_value);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_conditionable_info(charac_no, current_step, reward_step, update_time) values(%u, %d, %d, now())",
            sig->m_value, (int)sig->m_a2, (int)sig->m_a3);
        if (!db->exec(true))
        {
            return 0;
        }
    }
    return 1;
