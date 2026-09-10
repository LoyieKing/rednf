void EventSigong::ArrangeSigongEventItem(unsigned int mid, unsigned int serverId)
{
    const unsigned int MAX_ENTRIES = 10;
    // [b28d-solo-WongWorkEvents_A] P311 反向：struct 行数组复现 ORIG 每列
    // 两步寻址（`shl $4; lea -0x8(%ebp); lea; sub $K`）。
    struct SigongEntry
    {
        unsigned int m_id;        // [0] id
        int          m_characNo;  // [1] itemN_charac_no
        int          m_itemNo;    // [2] itemN_no
        int          m_itemCnt;   // [3] 常量 1/200
    };
    SigongEntry entries[MAX_ENTRIES];

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,item1_no,item1_server_id,item1_charac_no,item1_check,"
        "item2_no,item2_server_id,item2_charac_no,item2_check from "
        "event_sigong_info where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P227/P139：ORIG 0x083fa8d2 先
        // NumberToString 存 %ebx 跨 ctor 保活 + __PRETTY_FUNCTION__ 同址双用。
        cMyTrace(__PRETTY_FUNCTION__, 0x123, 5)
            ("%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__, NumberToString(mid, 0));
        return;
    }

    int outCount = 0;
    unsigned int row = 0;
    // [b28d-solo-WongWorkEvents_A] ORIG 0x083fa93c 尾测 while + 体首
    // `call fetch; xor $1; test; jne exit`（P225 setcc 形）。
    while (row < (unsigned int)db->get_n_rows())
    {
        if (!db->fetch())
            break;
        // [b28d-solo-WongWorkEvents_A] P311：ORIG 0x083fa957 对 rowBuf 的
        // 访问是 `lea -0x34(%ebp)` / `mov -0x54(%ebp,%eax,4)` 形（常量偏移
        // 折进 disp、变量索引留 scaled）。裸 `unsigned[11]`/局部 struct 视图
        // 在 4.4 下恒发 `lea -0x5c(%ebp); add $K` / `add $K,%eax`（+1 条/处）
        // ⇒ 改「每栏独立 2 元素数组 + 独立 id」形（声明序按 4.4 先声明浅，
        // 使 id 落 -0x34、cnts 落 -0x5c，与 ORIG 槽位一致）。
        unsigned int id;        // [10] -0x34
        int items[2];           // [8],[9]
        int servers[2];         // [6],[7]
        int characs[2];         // [4],[5]
        int checks[2];          // [2],[3]
        int cnts[2];            // [0],[1]
        // ORIG 初始化发射序 = id,[8],[9],[6],[7],[4],[5],[2],[3],[0],[1]。
        id = 0;
        items[0] = 0;             // item1_no
        items[1] = 0;             // item2_no
        servers[0] = 0;           // item1_server_id
        servers[1] = 0;           // item2_server_id
        characs[0] = 0;           // item1_charac_no
        characs[1] = 0;           // item2_charac_no
        checks[0] = 0;            // item1_check
        checks[1] = 0;            // item2_check
        cnts[0] = 1;              // ORIG: item1 常量数量
        cnts[1] = 200;            // ORIG: item2 常量数量

        db->get_int(0, id);            // id（ORIG 选 unsigned 重载）
        db->get_int(1, items[0]);      // item1_no
        db->get_int(2, servers[0]);    // item1_server_id
        db->get_int(3, characs[0]);    // item1_charac_no
        db->get_int(4, checks[0]);     // item1_check
        db->get_int(5, items[1]);      // item2_no
        db->get_int(6, servers[1]);    // item2_server_id
        db->get_int(7, characs[1]);    // item2_charac_no
        db->get_int(8, checks[1]);     // item2_check

        for (int i = 0; i <= 1; ++i)       // ORIG cmpl $0x1 setle
        {
            // [b28d-solo-WongWorkEvents_A] P140：ORIG 无 outCount<MAX 界检。
            if (checks[i] == 0 && servers[i] == serverId)
            {
                entries[outCount].m_id = id;
                entries[outCount].m_itemNo = items[i];
                entries[outCount].m_itemCnt = cnts[i];
                entries[outCount].m_characNo = characs[i];
                ++outCount;
            }
        }
        ++row;
    }

    for (int k = 0; k < outCount; ++k)
    {
        UpdateEventSigongInfo(entries[k].m_id, entries[k].m_itemNo);
        EventCommon::InsertEventItem2EventTable(entries[k].m_characNo, entries[k].m_itemNo,
                                                entries[k].m_itemCnt, 3);
    }
