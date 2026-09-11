unsigned int DB_Login::_isPunishedHackUser(unsigned int mid)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect occ_time,period,now_flag from member_punish_hack where m_id=%s",
                  NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    int occTime = 0;
    int period = 0;
    int nowFlag = 0;
    if (!db->get_int(0, occTime)) return 0;
    if (!db->get_int(1, period)) return 0;
    if (!db->get_int(2, nowFlag)) return 0;
    if (period != 0)
    {
        // [b28d-solo-DB_Login] ORIG 0x08412c0d..0x08412c1c 为 32 位 imul/lea
        // （`imul $0xe10,%eax,%edx; lea (%edx,%eax,1),%eax; mov %eax,-0xc`）⇒ int。
        int expireSec = period * 0xe10 + occTime;
        if (expireSec < GlobalData::s_systemTime_.getCurSec())
        {
            return 0;
        }
    }
    // P47/P56：ORIG `test; jne <mov$1 尾块>; mov$0` ⇒ if(==0) return 0; return 1;
    if (nowFlag == 0)
    {
        return 0;
    }
    return 1;
}