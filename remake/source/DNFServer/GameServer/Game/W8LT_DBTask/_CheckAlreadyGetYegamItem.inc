bool DB_SaveEventItem::_CheckAlreadyGetYegamItem(int id, int type)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    if (type == 1)
    {
        db->set_query("seLect coin_check from event_yegam_info where id=%d", id);
    }
    else
    {
        db->set_query("seLect item_check from event_yegam_info where id=%d", id);
    }
    if (!db->exec(true))
    {
        return 0;
    }
    int rowId = 0;
    if ((db->get_int(0, rowId) && rowId != 0) ? false : true)
    {
        return 0;
    }
    else
    {
        return 1;
    }
