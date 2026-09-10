void EventSigong::UpdateEventSigongInfo(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    // P17：ORIG 单读双判线性 cmp/je 链（body 后置）= 两 case switch 形。
    switch (CWebEvent::GetItemType(3, itemNo))
    {
        case 1:
            db->set_query(
                "upDate event_sigong_info set item1_check=unix_timestamp(now()) where id=%s",
                NumberToString(id, 0));
            break;
        case 2:
            db->set_query(
                "upDate event_sigong_info set item2_check=unix_timestamp(now()) where id=%s",
                NumberToString(id, 0));
            break;
    }
    db->exec(true);
