int EventATI::CheckAlreadyGetATIItem(unsigned int uid, int itemNo)
{
    // ORIG 直调 DBMgr::GetDBHandle(1,0)（禁 helper 包装遮蔽该 call，P167 判据）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    switch (CWebEvent::GetItemType(2, itemNo))
    {
    case 1:
        db->set_query("seLect avata_check1 from event_ati_info where id=%s",
                      NumberToString(uid, 0));
        break;
    case 2:
        db->set_query("seLect avata_check2 from event_ati_info where id=%s",
                      NumberToString(uid, 0));
        break;
    case 3:
        db->set_query("seLect item_check from event_ati_info where id=%s",
                      NumberToString(uid, 0));
        break;
    case 4:
        db->set_query("seLect cera_check from event_ati_info where id=%s",
                      NumberToString(uid, 0));
        break;
    default:
        break;  // ORIG default 落入 exec 段（Ghidra C 证实无 return 0）
    }
    if (!db->exec(true))
        return 0;
    int check = 0;
    if (!db->get_int(0, check) || check == 0)
        return 0;
    return 1;
