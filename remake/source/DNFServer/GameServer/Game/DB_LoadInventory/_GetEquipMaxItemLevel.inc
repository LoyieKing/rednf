// ============================================================================
// DB_LoadInventory::_GetEquipMaxItemLevel @0x0840aac8
// charac_manage_info.max_equip_level → sig+0x15448（m_maxEquipLevel）。
// exec 失败/fetch 失败/get_int 失败 → 0；无行（getAffectedRowCount==0）→ 1。
// ============================================================================
bool DB_LoadInventory::_GetEquipMaxItemLevel(SIG_LOAD_INVENTORY* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect max_equip_level from charac_manage_info"
                  " where charac_no = %d", sig->m_characNo);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        return 1;
    }
    if (!db->fetch())
    {
        return 0;
    }
    // ORIG 0840ab62..：col 命名槽（-0xc 浅，后声明→浅）+ col++ 实参形；
    // get_int 失败（==0）返回 0，否则返回 1（勿直接返回 get_int）。
    int col = 0;
    if (!db->get_int(col++, *reinterpret_cast<int*>(&sig->m_maxEquipLevel)))
    {
        return 0;
    }
    return 1;
}