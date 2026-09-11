bool CSyncScript::truncate_charac_advance_altar_item_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return advancealtar::SyncScript::truncateItemDescTable();
}