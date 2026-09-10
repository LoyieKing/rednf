bool DB_SaveRank::SaveRankAll(SIG_SAVE_RANK* sig)
{
    // 声明序 db→i 与 ORIG 栈槽对应（-0x20/-0x1c）；无 row/sv 局部，
    // 每字段按 ORIG 视图基址重算（charac_no/count 直读参数）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    int i;
    for (i = 0; i < ((DB_SaveRankSigView*)sig)->m_count; ++i)
    {
        db->set_query(
            "inSert into charac_best_record (charac_no,dungeon_no,difficulty,style,technic,attacked,rank) values(%d,%d,%d,%d,%d,%d,%d)",
            ((DB_SaveRankSigView*)sig)->m_characNo,
            ((DB_SaveRankRowView*)((char*)sig + i * 0x50))->m_dungeonNo,
            ((DB_SaveRankRowView*)((char*)sig + i * 0x50))->m_difficulty,
            ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_style,
            ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_technic,
            ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_attacked,
            ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_rank);
        if (!db->exec(true))
        {
            db->set_query(
                "upDate charac_best_record set style=%d, technic=%d ,attacked=%d, rank=%d where charac_no=%d and dungeon_no=%d and difficulty=%d",
                ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_style,
                ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_technic,
                ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_attacked,
                ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_rank,
                ((DB_SaveRankSigView*)sig)->m_characNo,
                ((DB_SaveRankRowView*)((char*)sig + i * 0x50))->m_dungeonNo,
                ((DB_SaveRankRowView*)((char*)sig + i * 0x50))->m_difficulty);
            if (!db->exec(true))
                return 0;
        }
    }
    return 1;
