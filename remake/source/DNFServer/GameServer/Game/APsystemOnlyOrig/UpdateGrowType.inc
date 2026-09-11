// ORIG 0x84181ea：grow_type(+0x04 movsbl 符号扩展) where charac_no(+0x00)；
// exec 失败返 0。
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
}