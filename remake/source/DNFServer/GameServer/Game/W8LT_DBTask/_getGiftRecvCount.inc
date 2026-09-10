int DB_ReqQueryCharacInfo::_getGiftRecvCount(unsigned int mId)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    int lastGiftTime = 0;
    int giftCnt = -1;
    char table[0x14];
    memset(table, 0, 0x14);
    memcpy(table, "member_login", 0xd);
    db->set_query("seLect last_gift_time,gift_cnt from %s where m_id=%s",
                  table, NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    // ORIG 0x8427013-0x842701a：`test %eax,%eax; setne %al; test %al,%al;
    // je <共享尾 mov -0x14,%eax>` —— rows!=0 正向大块（技巧 19 int!=0 物化）
    // + giftCnt!=0 同尾 je(0x8427075)；tm 槽 tmLast=-0x5c > tmNow=-0x88 ⇒
    // tmLast 先声明（4.4 后声明占更低槽）。
    if (db->get_n_rows() != 0)
    {
        if (!db->fetch())
            return -1;
        db->get_int(0, lastGiftTime);
        db->get_int(1, giftCnt);
        if (giftCnt != 0)
        {
            time_t now = GlobalData::s_systemTime_.getCurSec();
            struct tm tmLast;
            struct tm tmNow;
            time_t t = lastGiftTime;
            localtime_r(&t, &tmLast);
            localtime_r(&now, &tmNow);
            if (tmLast.tm_mon != tmNow.tm_mon)
            {
                db->set_query(
                    "upDate %s set last_gift_time=unix_timestamp(now()),gift_cnt=0 where m_id=%s",
                    table, NumberToString(mId, 0));
                db->exec(true);
                giftCnt = 0;
            }
        }
    }
    return giftCnt;
