bool CUserCharacInfo::isExistItem(int itemIdx)
{
    if (!m_selected)
        return false;
    if (m_selected->m_cargo.check_item_exist(itemIdx) != -1)
        return true;
    if (m_selected->m_curCharacInven.check_item_exist(itemIdx) != -1)
        return true;
    return false;
}
