void EventFCCombo::ArrangeFCComboEventItem(unsigned int mid, unsigned int serverId)
{
    const unsigned int MAX_ENTRIES = 10;
    // [b28d-solo-WongWorkEvents_A] P311 反向：struct 行数组复现 ORIG 每列
    // 两步寻址（`shl $4; lea -0x8(%ebp); lea; sub $K`）。
    struct FCComboEntry
    {
        unsigned int m_id;        // [0] id
        int          m_characNo;  // [1]
        int          m_itemNo;    // [2]
        int          m_itemCnt;   // [3]
    };
    FCComboEntry entries[MAX_ENTRIES];

    MySQL* db = GetDBHandle();
    db->set_query(
        "seLect id,item_server_id,item_charac_no,item_no,item_check,item_cnt from "
        "event_combo_entry where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P227/P139：ORIG 0x083fb9db 先
        // NumberToString 存 %ebx 跨 ctor 保活 + __PRETTY_FUNCTION__ 同址双用。
        cMyTrace(__PRETTY_FUNCTION__, 0x27a, 5)
            ("%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__, NumberToString(mid, 0));
        return;
    }

    int outCount = 0;
    unsigned int row = 0;
    // [b28d-solo-WongWorkEvents_A] ORIG 0x083fba45 尾测 while + 体首
    // `call fetch; xor $1; test; jne exit`（P225 setcc 形）。
    while (row < (unsigned int)db->get_n_rows())
    {
        if (!db->fetch())
            break;
        unsigned int id = 0;               // col0
        int srv = 0;                    // col1
        int charac_no = 0;                 // col2
        int item_no = 0;                   // col3
        int item_check = 0;                // col4
        int item_cnt = 0;                  // col5
        db->get_int(0, id);
        db->get_int(1, srv);
        db->get_int(2, charac_no);
        db->get_int(3, item_no);
        db->get_int(4, item_check);
        db->get_int(5, item_cnt);

        // [b28d-solo-WongWorkEvents_A] P140：ORIG 无 outCount<MAX_ENTRIES 界检。
        if (item_check == 0 && srv == serverId)
        {
            entries[outCount].m_id = id;
            entries[outCount].m_itemNo = item_no;
            entries[outCount].m_itemCnt = item_cnt;
            entries[outCount].m_characNo = charac_no;
            ++outCount;
        }
        ++row;
    }

    for (int i = 0; i < outCount; ++i)
    {
        UpdateFCComboEventInfo(entries[i].m_id, entries[i].m_itemNo);
        EventCommon::InsertEventItem2EventTable(entries[i].m_characNo, entries[i].m_itemNo,
                                                entries[i].m_itemCnt, 7);
    }
