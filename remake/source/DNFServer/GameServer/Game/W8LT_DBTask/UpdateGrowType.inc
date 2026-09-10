bool DB_UpdateGrowType::UpdateGrowType(SIG_UPDATE_GROWTYPE* sig)
{
    // 声明序 r→db 与 ORIG 栈槽对应（-0xd/-0xc）；字段经视图强转直读参数。
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("upDate charac_info set grow_type=%d where charac_no=%u",
                  ((DB_UpdateGrowTypeView*)sig)->m_growType,
                  ((DB_UpdateGrowTypeView*)sig)->m_characNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
