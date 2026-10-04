// ORIG 0x08436dec：GetOutBuffer<SIG_HACK_IP_SAVE> → GetDBHandle(6,0)。
// 每表先 upDate 计数，exec 失败或无影响行时 inSert 新行；insert exec 失败
// 分别 logFormat(0x829d/"DB_HackIPSave DB ERROR"、0x82af/FullIP) 返 0；
// 全部成功返 1。
bool DB_HackIPSave::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_HACK_IP_SAVE* sig =
        stream->GetOutBuffer<SIG_HACK_IP_SAVE>();
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
}