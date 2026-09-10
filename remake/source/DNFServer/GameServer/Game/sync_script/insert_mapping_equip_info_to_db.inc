bool CSyncScript::insert_mapping_equip_info_to_db(std::map<int, int>& mapping)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    /* f3 源形（begin/end 头部预求值 + jmp 进尾判 = ORIG 形）+ it++ 后置
     * （ORIG ppEi 含临时槽，探针 /tmp/l5_pp.cpp f3）。 */
    std::map<int, int>::iterator it = mapping.begin();
    std::map<int, int>::iterator map_end = mapping.end();
    while (it != map_end)
    {
        if (it->second != 0)
        {
            db->set_query("inSert into equip_mapping_info(equip_idx, mapping_idx) values(%d, %d)",
                          it->second, it->first);
            if (!db->exec(true))
            {
                return false;
            }
        }
        it++;
    }
    return true;
