bool DB_SaveCharac::_SaveCharacManageInfo(SIG_SAVE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("inSert into charac_manage_info(charac_no, tag_charac_no, "
                  "striker_skill_index, max_equip_level) values(%u,%u,%u,%u)",
                  sig->m_characNo, sig->m_tagCharacNo,
                  (unsigned int)sig->m_strikerSkillIndex,
                  (unsigned int)sig->m_maxEquipLevel);
    bool r = db->exec(true);
    if (r != 1)
    {
        if (db->getDBError() == 0x426)
        {
            db->set_query("upDate charac_manage_info set tag_charac_no=%u, "
                          "striker_skill_index=%u, max_equip_level=%u "
                          "where charac_no=%u",
                          sig->m_tagCharacNo,
                          (unsigned int)sig->m_strikerSkillIndex,
                          (unsigned int)sig->m_maxEquipLevel, sig->m_characNo);
            r = db->exec(true);
            if (r != 1)
                return false;
        }
        else
        {
            return false;
        }
    }
    return true;
