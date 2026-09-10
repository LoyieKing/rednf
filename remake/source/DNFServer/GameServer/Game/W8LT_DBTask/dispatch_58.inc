bool DB_HadesPunish::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序照 ORIG 栈槽（低→高）：reason(-0x1b7) endTime(-0xb8) tm(-0x54)
    // year(-0x22) curSec(-0x28)...（ORIG 清零在 +0x1ec、getCurSec 在 +0x207）；
    // curSec 与 year 均声明于函数中部使用点以复现槽序。
    char reason[0xff];
    char endTime[0x64];
    struct tm tmVal;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    DB_HadesPunishView* sig =
        (DB_HadesPunishView*)stream->GetOutBuffer<SIG_HADES_PUNISH>();
    if (sig->m_days == 0)
    {
        memcpy(endTime, "'9999-12-31 23:59:59'", 0x16);
    }
    else
    {
        sprintf(endTime, "adddate(now(),interval %d day)", sig->m_days);
    }
    int applyFlag = (sig->m_applyFlag != 0) ? 2 : 1;
    int punishValue = 0;
    if (sig->m_punishType == 1)
        punishValue = 3;
    else
        punishValue = 0xc;
    db->set_query(
        "upDate member_punish_info set occ_time=now(), start_time=now(), end_time=%s, apply_flag=%d where m_id=%s and punish_type=%d",
        endTime, applyFlag, NumberToString(sig->m_mId, 0), punishValue);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into member_punish_info (m_id, punish_type, occ_time, start_time, end_time, punish_value, apply_flag) values(%s, %d, now(), now(), %s, 0, %d)",
            NumberToString(sig->m_mId, 0), punishValue, endTime, applyFlag);
        if (!db->exec(true))
            return 0;
    }
    if (sig->m_punishType == 1)
    {
        memcpy(reason,
               "|*|HADES \xC1\xA6\xC0\xE7 / \xBA\xD2\xB9\xFD\xC7\xC1\xB7\xCE\xB1\xD7\xB7\xA5 \xB9\xD7 \xC0\xAF\xC7\xD8 \xC7\xC1\xB7\xCE\xB1\xD7\xB7\xA5\xC0\xBB \xBB\xE7\xBF\xEB\xC7\xCF\xBF\xA9 \xC0\xCC\xB5\xE6\xC0\xBB \xC3\xEB\xC7\xCF\xB0\xC5\xB3\xAA, \xBA\xD2\xB9\xFD \xC0\xE7\xC8\xAD \xBB\xFD\xBC\xBA \xB9\xD7 \xC0\xAF\xC5\xEB\xC7\xD1 \xB0\xE6\xBF\xEC(\xC6\xDB\xBD\xBA\xC6\xAE\xBC\xAD\xB9\xF6 \xC6\xF7\xC7\xD4) / 1\xC2\xF7 : \xC7\xD8\xC1\xF6\xBA\xD2\xB0\xA1 / \xB0\xE8\xC1\xA4\xC7\xD8\xC5\xB7 \xC1\xD6\xC0\xE5 \xBD\xC3 \xC8\xAE\xC0\xCE\xBF\xE4\xC3\xBB",
               0xa6);
    }
    else
    {
        memcpy(reason,
               "|*|HADES \xC1\xA6\xC0\xE7 / \xBA\xD2\xB9\xFD\xC7\xC1\xB7\xCE\xB1\xD7\xB7\xA5\xBF\xA1 \xC0\xC7\xC7\xD1 \xC3\xB3\xB9\xFA / \xB0\xE8\xC1\xA4\xBA\xB8\xC8\xA3\xC1\xA6\xB5\xB5 \xC0\xFB\xBF\xEB",
               0x3d);
    }
    char year[10] = { '\0' };
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    localtime_r((const time_t*)&curSec, &tmVal);
    strftime(year, 10, "%Y", &tmVal);
    db->set_query(
        "inSert into member_punish_info_history_%s (m_id, punish_type, occ_time, punish_value, start_time, end_time, admin_id, apply_flag, reason) values(%s, %d, now(), 6, now(), %s, 'HADES', %d, '%s') ",
        year, NumberToString(sig->m_mId, 0), punishValue, endTime, applyFlag, reason);
    if (!db->exec(true))
        return 0;
    if (punishValue == 3 || (punishValue == 0xc && applyFlag == 1))
    {
        db->set_query(
            "inSert into member_punish_info_ars_mail (m_id, punish_type, apply_flag, occ_time) values(%s, %d, %d, now())",
            NumberToString(sig->m_mId, 0), punishValue, applyFlag);
        if (!db->exec(true))
            return 0;
    }
    return 1;
