bool DB_AvatarExtendPeriod::ExtendPeriod(SIG_AVATAR_RENEW& renew)
{
    // ORIG 栈槽：ok(-0xd) db(-0xc)——bool 先声明（低地址），db 后。
    bool ok;      // -0xd（exec 结果落槽，P102）
    MySQL* db;    // -0xc
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    // ORIG 0x83ffbb4：set_query("upDate user_items set expire_date =
    // adddate(expire_date, interval %d day) where charac_no=%u and slot=%d",
    // days, charac_no, slot) 后 exec(true)
    db->set_query(
        "upDate user_items set expire_date = adddate(expire_date, "
        "interval %d day) where charac_no=%u and slot=%d",
        renew.m_days, renew.m_characNo, renew.m_slot);
    ok = db->exec(true);
    if (ok != 1)
        return 0;
    return 1;
