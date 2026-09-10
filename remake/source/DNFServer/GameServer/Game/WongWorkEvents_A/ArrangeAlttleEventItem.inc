void EventAlttle::ArrangeAlttleEventItem(unsigned int mid, unsigned int serverId)
{
    const unsigned int MAX_ENTRIES = 10;   // local_24 = 10
    // [b28d-solo-WongWorkEvents_A] P311 反向：ORIG 每列两步寻址；struct 行
    // 数组避免 4.4 对 [1]/[2] 的 scaled 折叠（探针 arr.cpp f2）。
    struct AlttleEntry
    {
        unsigned int m_id;        // [0] mid
        int          m_characNo;  // [1]
        int          m_itemNo;    // [2]
        int          m_itemCnt;   // [3]
    };
    AlttleEntry entries[MAX_ENTRIES];

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect item_server_id,item_charac_no,item_no,item_check,item_cnt from "
        "event_alttle_info where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P227/P139：ORIG 0x083fb612 先
        // NumberToString 存 %ebx 跨 ctor 保活 + __PRETTY_FUNCTION__ 同址双用。
        cMyTrace(__PRETTY_FUNCTION__, 0x21b, 5)
            ("%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__, NumberToString(mid, 0));
        return;
    }

    int outCount = 0;                       // -0x18（写满区计数）
    unsigned int row = 0;                   // -0x14（fetch 游标）
    // [b28d-solo-WongWorkEvents_A] ORIG 0x083fb67c 尾测 while + 体首
    // `call fetch; xor $1; test; jne exit`（P225 setcc 形）。
    while (row < (unsigned int)db->get_n_rows())
    {
        if (!db->fetch())
            break;
        // [b28d-solo-WongWorkEvents_A] ORIG 0x083fb697 `movl $0,-0x10(%ebp)`
        // 为循环体内声明、每轮清零且从不使用的 dead 局部（P171/P232 照抄；
        // 它把 db/outCount/row 与列局部整体下移 4 槽）。
        unsigned int unused = 0;
        (void)unused;
        unsigned int server_id = 0;         // col0
        int charac_no = 0;                  // col1
        unsigned int item_no = 0;           // col2
        int item_check = 0;                 // col3
        unsigned int item_cnt = 0;          // col4
        db->get_int(0, (int&)server_id);    // ORIG 全走 get_int(col,&int)
        db->get_int(1, charac_no);
        db->get_int(2, (int&)item_no);
        db->get_int(3, item_check);
        db->get_int(4, (int&)item_cnt);

        // ORIG 无 outCount<MAX 界检（查询 LIMIT 10 保证 ≤10 行，auStack_e8[40] 不外溢）
        if (item_check == 0 && server_id == serverId)
        {
            entries[outCount].m_id = mid;
            entries[outCount].m_itemNo = item_no;
            entries[outCount].m_itemCnt = item_cnt;
            entries[outCount].m_characNo = charac_no;
            ++outCount;
        }
        ++row;
    }

    for (int i = 0; i < outCount; ++i)
    {
        UpdateEventAlttleInfo(entries[i].m_id, entries[i].m_itemNo);
        EventCommon::InsertEventItem2EventTable(entries[i].m_characNo, entries[i].m_itemNo,
                                                entries[i].m_itemCnt, 6);
    }
