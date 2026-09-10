void EventEverland::ArrangeEverlandEventItem(unsigned int mid, unsigned int serverId)
{
    const unsigned int MAX_ENTRIES = 10;
    // [b28d-solo-WongWorkEvents_A] P311 反向：ORIG 0x083faf16 对每列均发
    // 两步寻址（`shl $4; lea -0x8(%ebp); lea; sub $K`）；`unsigned[10][4]`
    // 在 4.4 下 [1]/[2] 被折成 scaled（`shl $2; add $1; -0xe0(%ebp,%edx,4)`）。
    // 改 struct 行数组（成员访问恒两步形，探针 /tmp/b28d_ww_probe/arr.cpp f2）。
    struct EverlandEntry
    {
        unsigned int m_id;        // [0]
        int          m_characNo;  // [1]
        int          m_itemNo;    // [2] 恒 1
        int          m_coin;      // [3]
    };
    EverlandEntry entries[MAX_ENTRIES];

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,server_id,charac_no,coin,coin_check from event_everland_info "
        "where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P227/P139：ORIG 0x083faddd 先
        // NumberToString 存 %ebx 跨 cMyTrace ctor 保活（push %ebx/sub 0xf4），
        // 两处 %s 实参同址 `_ZZ…E19__PRETTY_FUNCTION__` ⇒ 全表达式临时链 +
        // __PRETTY_FUNCTION__ 宏。
        cMyTrace(__PRETTY_FUNCTION__, 0x182, 5)
            ("%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__, NumberToString(mid, 0));
        return;
    }

    int outCount = 0;
    unsigned int row = 0;
    // [b23S3C] ORIG 0x083fad60 循环形 = 尾测 while：进入前 `movl $0x0,-0x10(%ebp);
    // jmp <tail>`，体首 `mov -0x18(%ebp),%eax; call fetch;
    // xor $0x1,%eax; test %al,%al; jne <exit>`（P194 xor 直测），尾 `call get_n_rows;
    // cmp -0x10(%ebp),%eax; seta %al; test %al,%al; jne <body>` 后才回体 ⇒ fetch 先于
    // 行数界判定（原 `while (row < n && fetch())` 短路序相反，丢体首 call）。
    while (row < (unsigned int)db->get_n_rows())
    {
        if (!db->fetch())
            break;
        unsigned int id = 0;               // col0
        int srv = 0;                       // col1
        int charac_no = 0;                 // col2
        int coin = 0;                      // col3
        int coin_check = 0;                // col4
        db->get_int(0, id);
        db->get_int(1, srv);
        db->get_int(2, charac_no);
        db->get_int(3, coin);
        db->get_int(4, coin_check);

        // [b28d-solo-WongWorkEvents_A] P140：ORIG 无 outCount<MAX_ENTRIES 界检
        // （limit 10 保障），删多余守卫。
        if (coin_check == 0 && srv == serverId)
        {
            entries[outCount].m_id = id;
            entries[outCount].m_itemNo = 1;      // ORIG 常量 item_no
            entries[outCount].m_coin = coin;
            entries[outCount].m_characNo = charac_no;
            ++outCount;
        }
        ++row;
    }

    for (int i = 0; i < outCount; ++i)
    {
        UpdateEventEverlandInfo(entries[i].m_id, entries[i].m_itemNo);
        EventCommon::InsertEventItem2EventTable(entries[i].m_characNo, entries[i].m_itemNo,
                                                entries[i].m_coin, 3);
    }
