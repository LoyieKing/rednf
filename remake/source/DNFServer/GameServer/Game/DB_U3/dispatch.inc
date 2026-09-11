// ---- DB_InsertCeraItemHistory::dispatch ORIG @ 0x8445032 ----
// 读流 itemIdx(int)/sender(uint)/receiver(uint)/flag(char 符号扩展)，DB_HANDLE_3，
// set_query("inSert into user_items_change_history(ui_id,from_charac_no,
// to_charac_no,occ_date,change_type) values(%u,%u,%u,now(),%u)", itemIdx,
// sender, receiver, (int)flag) → exec(true) → if(!ok) false else true。
bool DB_InsertCeraItemHistory::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int senderCharacNo = 0;
    unsigned int receiverCharacNo = 0;
    int itemIdx = 0;
    char flag = 0;
    *stream >> itemIdx;
    *stream >> senderCharacNo;
    *stream >> receiverCharacNo;
    *stream >> flag;

    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into user_items_change_history(ui_id,from_charac_no,to_charac_no,"
        "occ_date,change_type) values(%u,%u,%u,now(),%u)",
        itemIdx, senderCharacNo, receiverCharacNo, (int)flag);
    if (!db->exec(true))
        return false;
    else
        return true;
}