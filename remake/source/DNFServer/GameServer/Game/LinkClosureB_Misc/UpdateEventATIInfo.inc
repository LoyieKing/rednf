// UpdateEventATIInfo —— ORIG T 0x083fa0ad (size 0x126)
// [b28d-D9] 修复：P46 叶块序：case1 写于 case2 前（交换后 .o AE 70/70）（ORIG 0x083fa0ad sz 0x00000126）
void EventATI::UpdateEventATIInfo(unsigned int uid, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    switch (CWebEvent::GetItemType(2, itemNo))
    {
    case 1:
        db->set_query("upDate event_ati_info set avata_check1=unix_timestamp(now()) where id=%s",
                      NumberToString(uid, 0));
        break;
    case 2:
        db->set_query("upDate event_ati_info set avata_check2=unix_timestamp(now()) where id=%s",
                      NumberToString(uid, 0));
        break;
    case 3:
        db->set_query("upDate event_ati_info set item_check=unix_timestamp(now()) where id=%s",
                      NumberToString(uid, 0));
        break;
    case 4:
        db->set_query("upDate event_ati_info set cera_check=unix_timestamp(now()) where id=%s",
                      NumberToString(uid, 0));
        break;
    default:
        break;
    }
    db->exec(true);
}