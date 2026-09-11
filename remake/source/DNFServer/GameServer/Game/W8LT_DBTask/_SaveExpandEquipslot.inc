// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 1} 中转 mov（单段直落/同型化待验）；ORIG 0843ff48
bool DB_SaveExpandEquipslot::_SaveExpandEquipslot(SIG_SAVE_EXPAND_EQUIPSLOT* sig)
{
    char buf[0xa5c] = {0};
    unsigned int bufLen = 0xa5c;
    if (!compress_zip(buf, bufLen, (const char*)sig, 0x898))
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    // ORIG 0x0843ff48：characNo 无具名局部——首处 `mov (%eax),%ebx` 直落
    // callee-saved 跨 blob_to_str 保活、次处重新装载（P0/P183 表达式内联）。
    db->set_query(
        "upDate charac_inven_expand set expand_equipslot='%s' where charac_no=%u",
        db->blob_to_str(0, buf, (int)bufLen), sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_inven_expand(charac_no, expand_equipslot) values(%u, '%s')",
            sig->m_characNo, db->blob_to_str(0, buf, (int)bufLen));
        if (!db->exec(true))
            return 0;
    }
    return 1;
}