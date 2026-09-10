bool DB_LoadTicket::LoadTicket(SIG_LOAD_TICKET* sig)
{
    static const int handleIdx = 3;  // ORIG _ZZN13DB_LoadTicket10LoadTicket…E9handleIdx
    // 声明序 i→ok→db 与 ORIG 栈槽对应（-0x14/-0xd/-0xc；gcc O0 按声明序分配）。
    int i;
    bool ok;
    MySQL* db;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("seLect village from village_ticket where charac_no=%u",
                  sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
        return 0;
    i = 0;
    while (i <= 0x63)
    {
        ok = db->fetch();
        if (!ok)
            break;
        ok = db->get_short(0, sig->m_tickets[i]);
        if (!ok)
            return 0;
        ++i;
    }
    sig->m_count = i;
    return 1;
