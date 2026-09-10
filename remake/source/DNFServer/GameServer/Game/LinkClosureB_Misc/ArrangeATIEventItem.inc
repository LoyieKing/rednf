void EventATI::ArrangeATIEventItem(unsigned int mid, unsigned int serverId)
{
    // ORIG 函数头 `movl $0xa,-0x1c` 为未被读的 dead int 局部（P232）。
    int limit = 10;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect id,cera,cera_check,item_no,item_server_id,item_charac_no,item_check,"
        "avata_no1,avata_no2,avata_server_id,avata_charac_no,avata_check1,avata_check2 "
        "from event_ati_info where m_id=%s and occ_check!=0 limit %d",
        NumberToString(mid, 0), 10);
    if (!db->exec(true))
    {
        // P227 全表达式临时链：NumberToString 跨 ctor 以 %ebx 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0xa1, 5)(
            "%s, exec ERROR m_id=%s", __PRETTY_FUNCTION__,
            NumberToString(mid, 0));
        return;
    }

    unsigned int rows[10][4];      // [dbId, stackCount?, itemId, flag]
    int count = 0;
    // P283/P164：ORIG for 底部条件布局 + unsigned 尾测（seta）。
    for (unsigned int scanned = 0;
         scanned < static_cast<unsigned int>(db->get_n_rows()); ++scanned)
    {
        if (!db->fetch())
            break;
        unsigned int rowId = 0; int cera = 0; int ceraCheck = 0; int itemNo = 0;
        int itemServerId = 0; int itemCharacNo = 0; int itemCheck = 0;
        int avataNo1 = 0; int avataNo2 = 0; int avataServerId = 0;
        int avataCharacNo = 0; int avataCheck1 = 0; int avataCheck2 = 0;
        db->get_int(0, rowId);
        db->get_int(1, cera);
        db->get_int(2, ceraCheck);
        db->get_int(3, itemNo);
        db->get_int(4, itemServerId);
        db->get_int(5, itemCharacNo);
        db->get_int(6, itemCheck);
        db->get_int(7, avataNo1);
        db->get_int(8, avataNo2);
        db->get_int(9, avataServerId);
        db->get_int(0xa, avataCharacNo);
        db->get_int(0xb, avataCheck1);
        db->get_int(0xc, avataCheck2);

        // P171：ORIG 无 count<10 防御（照抄越界语义），存储序 [0],[2],[3],[1]。
        if (itemCheck == 0 && itemServerId == static_cast<int>(serverId))
        {
            rows[count][0] = static_cast<unsigned int>(rowId);
            rows[count][2] = static_cast<unsigned int>(itemNo);
            rows[count][3] = 1;
            rows[count][1] = static_cast<unsigned int>(itemCharacNo);
            ++count;
        }
        if (avataServerId == static_cast<int>(serverId))
        {
            if (avataCheck1 == 0)
            {
                rows[count][0] = static_cast<unsigned int>(rowId);
                rows[count][2] = static_cast<unsigned int>(avataNo1);
                rows[count][3] = 1;
                rows[count][1] = static_cast<unsigned int>(avataCharacNo);
                ++count;
            }
            if (avataCheck2 == 0)
            {
                rows[count][0] = static_cast<unsigned int>(rowId);
                rows[count][2] = static_cast<unsigned int>(avataNo2);
                rows[count][3] = 1;
                rows[count][1] = static_cast<unsigned int>(avataCharacNo);
                ++count;
            }
        }
    }

    for (int i = 0; i < count; ++i)
    {
        UpdateEventATIInfo(rows[i][0], static_cast<int>(rows[i][2]));
        EventCommon::InsertEventItem2EventTable(rows[i][1], rows[i][2],
                                                rows[i][3], 2);
    }
