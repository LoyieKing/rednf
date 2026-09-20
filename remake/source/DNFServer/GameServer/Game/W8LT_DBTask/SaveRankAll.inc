// ORIG 0x8417510：循环 i < sig->m_count（+0x04），每行 0x50 步长：
// inSert charac_best_record(charac_no,dungeon_no,difficulty,style,technic,
// attacked,rank)；inSert exec 失败改 upDate 同键；upDate 也失败返 0。
// [hc 收] (char*)sig + i*0x50 视图族：sig 为 SIG_SAVE_RANK 载荷线格式缓冲，
// 每行 0x50 步长字段经 DB_SaveRankRowView/StatsView 视图类型 cast 直读
// （§5.3 序列化缓冲豁免；ORIG 基址重算每字段同形）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08417510
bool DB_SaveRank::SaveRankAll(SIG_SAVE_RANK* sig)
{
    // 声明序 db→i 与 ORIG 栈槽对应（-0x20/-0x1c）；无 row/sv 局部，
    // 每字段按 ORIG 视图基址重算（charac_no/count 直读参数）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    int i;
    for (i = 0; i < sig->m_rows[0].m_count; ++i)
    {
        db->set_query(
            "inSert into charac_best_record (charac_no,dungeon_no,difficulty,style,technic,attacked,rank) values(%d,%d,%d,%d,%d,%d,%d)",
            sig->m_rows[0].m_characNo,
            sig->m_rows[i].m_dungeonNo,
            sig->m_rows[i].m_difficulty,
            sig->m_rows[i].m_style,
            sig->m_rows[i].m_technic,
            sig->m_rows[i].m_attacked,
            ((i < 99) ? sig->m_rows[i + 1].m_rankOfPrev : sig->m_rankOfPrev));
        if (!db->exec(true))
        {
            db->set_query(
                "upDate charac_best_record set style=%d, technic=%d ,attacked=%d, rank=%d where charac_no=%d and dungeon_no=%d and difficulty=%d",
                sig->m_rows[i].m_style,
                sig->m_rows[i].m_technic,
                sig->m_rows[i].m_attacked,
                ((i < 99) ? sig->m_rows[i + 1].m_rankOfPrev : sig->m_rankOfPrev),
                sig->m_rows[0].m_characNo,
                sig->m_rows[i].m_dungeonNo,
                sig->m_rows[i].m_difficulty);
            if (!db->exec(true))
                return 0;
        }
    }
    return 1;
}