int Inter_NpcLimitBuyItemInfo::checkError(CUser* user, LimitNpcBuyItemInfo* info)
{
    if (user == 0)
        return 3;
    if (user->get_state() <= 2)
        return 3;
    if (user->get_charac_no(-1) !=
        *reinterpret_cast<int*>(reinterpret_cast<char*>(info) + 0x12))
        return 3;

    m_error = *reinterpret_cast<int*>(reinterpret_cast<char*>(info) + 0x16);
    if (m_error == 0)
        return 0x11;

    m_itemRef =
        reinterpret_cast<int>(G_CDataManager()->find_item(m_error));
    if (m_itemRef == 0)
        return 0x11;

    const CItem* mat = reinterpret_cast<CItem*>(m_itemRef)->GetNeedMaterial();
    m_needItem.first  = mat->m_index;    // GetNeedMaterial().first（CItem* 首 int）
    m_needItem.second = mat->m_grade;    // GetNeedMaterial().second
    if (m_needItem.first == 0 || m_needItem.first == -1)
        return 0x17;

    const CInventory* invenR = user->getCurCharacInvenRefR();
    m_param2 = invenR->GetInvenData(m_needItem.first, m_invenItem2);
    if (m_param2 < 0)
        return 0x15;
    if (m_needItem.second < 1 ||
        m_invenItem2.get_add_info() < m_needItem.second)
        return 0x16;
    return 0;
