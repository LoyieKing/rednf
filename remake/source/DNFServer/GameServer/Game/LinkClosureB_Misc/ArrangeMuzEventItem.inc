void EventMuz::ArrangeMuzEventItem(unsigned int mid, unsigned int serverId)
{
    // [b28d-solo-LinkClosureB_Misc] ORIG 函数头 `movl $0xa,-0x1c` 为未被读的
    // dead int 局部（P232/P31 帧槽对齐）；kFunc 局部已删（P118/P139 用
    // __PRETTY_FUNCTION__ 生成 _ZZ...E19__PRETTY_FUNCTION__ 符号）。
    int limit = 10;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,item_server_id,item_charac_no,item_no,item_check,item_cnt "
        "from event_muz_entry where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // P227：全表达式临时链（构造即调用）令 NumberToString 在 ctor 前求值
        // 并以 %ebx 跨 ctor 保活（ORIG `call NumberToString; mov %eax,%ebx`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1ce, 5)(
            "%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__,
            NumberToString(mid, 0));
        return;
    }

    unsigned int rows[10][4];
    int count = 0;
    // P283/P164：ORIG 为 for 底部条件布局（fetch 在体内、get_n_rows 在尾条件），
    // 且 scanned 为 unsigned（尾测 seta）。
    for (unsigned int scanned = 0;
         scanned < static_cast<unsigned int>(db->get_n_rows()); ++scanned)
    {
        if (!db->fetch())
            break;
        unsigned int rowId = 0; int itemServerId = 0; int itemCharacNo = 0;
        int itemNo = 0; int itemCheck = 0; int itemCnt = 0;
        db->get_int(0, rowId);
        db->get_int(1, itemServerId);
        db->get_int(2, itemCharacNo);
        db->get_int(3, itemNo);
        db->get_int(4, itemCheck);
        db->get_int(5, itemCnt);

        if (itemCheck == 0 &&
            static_cast<int>(itemServerId) == static_cast<int>(serverId))
        {
            // ORIG 存储序 [0],[2],[3],[1]（-0x30/-0x3c/-0x44/-0x38 实证）。
            rows[count][0] = static_cast<unsigned int>(rowId);
            rows[count][2] = static_cast<unsigned int>(itemNo);
            rows[count][3] = static_cast<unsigned int>(itemCnt);
            rows[count][1] = static_cast<unsigned int>(itemCharacNo);
            ++count;
        }
    }

    for (int i = 0; i < count; ++i)
    {
        UpdateEventMuzInfo(rows[i][0], static_cast<int>(rows[i][2]));
        EventCommon::InsertEventItem2EventTable(rows[i][1], rows[i][2],
                                                rows[i][3], 5);
    }
