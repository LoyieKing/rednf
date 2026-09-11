// ============================================================================
// DB_UpdateOneDayLetheSkill::ProcessInitLethe @0x0843410c
// GetDBHandle(3,0) → treeKind∈{0xff,0} 分支 lethe_flag/lethe_flag_2nd 更新
// （skill 表名 get_skill_table_name(false)）→ exec(true) 成败映射 bool。
// ============================================================================
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
}