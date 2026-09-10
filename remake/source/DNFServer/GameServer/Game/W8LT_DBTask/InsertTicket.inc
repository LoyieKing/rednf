bool DB_InsertTicket::InsertTicket(SIG_INSERT_TICKET* sig)
{
    // 声明序 r→db 对应 ORIG 栈槽 -0xd/-0xc（gcc O0 首局部占最低槽）。
    bool r;
    static const int handleIdx = 3;  // ORIG _ZZN15DB_InsertTicket12InsertTicket…E9handleIdx
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("inSert village_ticket values(%u,%d)",
                  ((DB_InsertTicketView*)sig)->m_village,
                  ((DB_InsertTicketView*)sig)->m_field4);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
