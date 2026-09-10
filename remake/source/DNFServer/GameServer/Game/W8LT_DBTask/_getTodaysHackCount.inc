int DB_SaveClientHackLog::_getTodaysHackCount(MySQL* db, unsigned int mid,
                                              unsigned short hackType)
{
    int count = 0;
    db->set_query("seLect cnt from dnf_hack_log where m_id=%s and hack_type=%d "
                  "and occ_date=CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR)",
                  NumberToString(mid, 0), (int)hackType);
    if (!db->exec(true) || db->get_n_rows() != 1)
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_int(0, count) || count == 0)
        return 0;
    return count;
