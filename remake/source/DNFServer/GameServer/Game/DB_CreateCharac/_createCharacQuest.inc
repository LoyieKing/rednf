// ===========================================================================
// _createCharacQuest — 报告 @0x08401094
// ===========================================================================
bool DB_CreateCharac::_createCharacQuest(unsigned int characNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("inSert into new_charac_quest(charac_no, clear_quest) values(%u,'')",
                  characNo);
    // P168a①：callee 返回 bool 一律裸形——`if (!db->exec(true)) return false;`
    // 直产 ORIG 尾岛 call;xor$1;test;je（N96 全域禁 register 绑定，旧形删除）。
    if (!db->exec(true))
        return false;
    return true;
}