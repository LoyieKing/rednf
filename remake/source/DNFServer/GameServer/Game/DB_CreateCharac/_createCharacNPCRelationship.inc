// ===========================================================================
// _createCharacNPCRelationship — 报告 @0x0840103e
// seLect/inSert charac_npc 一条；尾部 exec 返回值即调用方判定值。
// ===========================================================================
bool DB_CreateCharac::_createCharacNPCRelationship(unsigned int characNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into charac_npc(charac_no,npc_cnt,npc_data) values(%d,0,'')",
        characNo);
    return db->exec(true);
}