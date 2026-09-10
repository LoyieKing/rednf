bool DB_UpdateOneDayLetheSkill::ProcessInitLethe(
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    if (sig->m_treeKind == 0xff || sig->m_treeKind == 0)
    {
        db->set_query("upDate %s set lethe_flag=%u where charac_no=%u",
                      get_skill_table_name(false),
                      (int)(unsigned char)sig->m_state2, sig->m_characNo);
    }
    else
    {
        db->set_query("upDate %s set lethe_flag_2nd=%u where charac_no=%u",
                      get_skill_table_name(false),
                      (int)(unsigned char)sig->m_state2, sig->m_characNo);
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
