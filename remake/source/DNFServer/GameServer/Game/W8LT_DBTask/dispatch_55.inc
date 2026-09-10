bool DB_HackIPSave::dispatch(int userIdx, int slot, Stream* stream)
{
    DB_HackIPSaveView* sig =
        (DB_HackIPSaveView*)stream->GetOutBuffer<SIG_HACK_IP_SAVE>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate auto_punish_hack_ip set cnt = cnt+1 where occ_date = CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR) and hack_type = %d and hack_sub_type = %d and c_class_ip = '%s'",
        sig->m_hackType, sig->m_hackSubType, sig->m_classIp);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into auto_punish_hack_ip (occ_date, hack_type, hack_sub_type, c_class_ip, cnt) values (now(), %d, %d, '%s', 1)",
            sig->m_hackType, sig->m_hackSubType, sig->m_classIp);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x829d,
                "DB_HackIPSave DB ERROR");
            return 0;
        }
    }
    db->set_query(
        "upDate auto_punish_hack_full_ip set cnt = cnt+1 where occ_date = CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR) and hack_type = %d and hack_sub_type = %d and full_ip = '%s'",
        sig->m_hackType, sig->m_hackSubType, sig->m_fullIp);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into auto_punish_hack_full_ip (occ_date, hack_type, hack_sub_type, full_ip, cnt) values (now(), %d, %d, '%s', 1)",
            sig->m_hackType, sig->m_hackSubType, sig->m_fullIp);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x82af,
                "DB_HackIPSave FullIP DB ERROR");
            return 0;
        }
    }
    return 1;
