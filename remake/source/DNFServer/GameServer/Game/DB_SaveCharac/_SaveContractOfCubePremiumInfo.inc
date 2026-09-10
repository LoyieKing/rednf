bool DB_SaveCharac::_SaveContractOfCubePremiumInfo(SIG_SAVE_CHARAC const& sig)
{
    if (sig.m_bSaveCubePremiumInfo != 1)
        return true;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("upDate cube_premium set selected=%u, cube_type=%u "
                  "where charac_no=%u",
                  (unsigned int)sig.m_cubeSelected, (unsigned int)sig.m_cubeType,
                  sig.m_characNo);
    if (!db->exec(true))
        return false;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into cube_premium(charac_no, selected, cube_type) "
                      "values(%u,%u,%u)",
                      sig.m_characNo, (unsigned int)sig.m_cubeSelected,
                      (unsigned int)sig.m_cubeType);
        if (!db->exec(true))
            return false;
    }
    return true;
