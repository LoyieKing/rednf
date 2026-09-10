void EventEverland::UpdateEventEverlandInfo(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    // ORIG T 0x083face3：GetItemType 结果 `cmp $0x1,%eax; jne`（直比无 bool
    // 物化）。4.4 对 `if (f()==1)` 恒产 `cmp $1; sete %al; test; je`，switch
    // case 1 直比才复现 jne（P17：jump-table/线性 cmp 定 switch，本例单 case
    // 即 cmp $1/jne 直落）。
    switch (CWebEvent::GetItemType(4, itemNo))
    {
    case 1:
        db->set_query(
            "upDate event_everland_info set coin_check=unix_timestamp(now()) where id=%s",
            NumberToString(id, 0));
        break;
    default:
        break;
    }
    db->exec(true);
