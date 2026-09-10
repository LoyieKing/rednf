bool DB_RequestCharacSkillInfo::_GetSkillTreeIndex(unsigned int characNo, char* outIndex)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect skill_tree_index from charac_info where charac_no=%u", characNo);
    bool ok = db->exec(true);
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb19a,
                              "DB_RequestCharacSkillInfo::_GetSkillTreeIndex exec() failed, charac_no(%d)", characNo);
        return 0;
    }
    ok = db->fetch();
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1a3,
                              "DB_RequestCharacSkillInfo::_GetSkillTreeIndex fetch() failed, charac_no(%d)", characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1aa,
                              "DB_RequestCharacSkillInfo::_GetSkillTreeIndex no result, charac_no(%d)", characNo);
        return 0;
    }
    int i = 0;
    if (!db->get_byte(i++, *outIndex))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0xb1b2,
                              "DB_RequestCharacSkillInfo::_GetSkillTreeIndex get_byte() failed, charac_no(%d)", characNo);
        return 0;
    }
    return 1;
