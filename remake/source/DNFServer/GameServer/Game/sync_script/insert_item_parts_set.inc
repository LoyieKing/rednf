bool CSyncScript::insert_item_parts_set(int index, STEquipmentPartSetPiece& piece)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into item_part_set(part_set_index, part_name, part_type, part_grade, part_rarity) values(%d, '%s',%d,%d,%d)",
                  index, piece.m_name.c_str(), piece.m_field4, piece.m_field8,
                  piece.m_fieldc);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
