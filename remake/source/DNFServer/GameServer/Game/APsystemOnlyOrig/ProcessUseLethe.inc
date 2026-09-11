// ============================================================================
// DB_UpdateOneDayLetheSkill::ProcessUseLethe @0x084341fc
// put_compressed_blob_data(sig+0xd, 0x198, &blob) → treeKind 分支
// skill_slot_lethe(_2nd)='blob', lethe_flag(_2nd)=m_state2 → exec。
// ============================================================================
bool DB_UpdateOneDayLetheSkill::ProcessUseLethe(
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    char* blob = 0;
    // ORIG 0x08434223 `add $0xd,%eax` 覆写基址 vs gcc4.4 decay `lea
    // 0xd(%eax),%edx` 新寄存器——add-vs-lea 固有差（N4/P30），保语义具名成员。
    if (!put_compressed_blob_data(
            db, 0, (char*)((unsigned long)sig + 0xdUL), 0x198, &blob))
    {
        return false;
    }
    if (sig->m_treeKind == 0xff || sig->m_treeKind == 0)
    {
        db->set_query("upDate %s set skill_slot_lethe='%s', lethe_flag=%u where charac_no=%u",
                      get_skill_table_name(false), blob,
                      (int)(unsigned char)sig->m_state2, sig->m_characNo);
    }
    else
    {
        db->set_query("upDate %s set skill_slot_lethe_2nd='%s', lethe_flag_2nd=%u where charac_no=%u",
                      get_skill_table_name(false), blob,
                      (int)(unsigned char)sig->m_state2, sig->m_characNo);
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}