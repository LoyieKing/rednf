// ============================================================================
// DB_UpdateOneDayLetheSkill::ProcessInitLetheSkill @0x084345a4
// 与 ProcessUseLethe 同构但 lethe_flag 用 USE_LETHE_STATE 常量，尾部
// cMyTrace(0x7d49, "ONE_DAY_LETHE : DB_INIT_LETHE_SKILL")。
// ============================================================================
bool DB_UpdateOneDayLetheSkill::ProcessInitLetheSkill(
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    char* blob = 0;
    if (!put_compressed_blob_data(
            db, 0, (char*)((unsigned long)sig + 0xdUL), 0x198, &blob))
    {
        return false;
    }
    if (sig->m_treeKind == 0xff || sig->m_treeKind == 0)
    {
        db->set_query("upDate %s set skill_slot_lethe='%s', lethe_flag=%u where charac_no=%u",
                      get_skill_table_name(false), blob,
                      (int)CPremiumLetheManager::USE_LETHE_STATE,
                      sig->m_characNo);
    }
    else
    {
        db->set_query("upDate %s set skill_slot_lethe_2nd='%s', lethe_flag_2nd=%u where charac_no=%u",
                      get_skill_table_name(false), blob,
                      (int)CPremiumLetheManager::USE_LETHE_STATE,
                      sig->m_characNo);
    }
    if (!db->exec(true))
    {
        return false;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x7d49, 0)(
        "ONE_DAY_LETHE : DB_INIT_LETHE_SKILL");
    return true;
}