void CEquipItem::getAvatarSocket(int slot, stAvatarEmblemInfo_t& emblem) const
{
    size_t index = 0;
    std::vector<unsigned short>::const_iterator it = m_vec180.begin();
    std::vector<unsigned short>::const_iterator e = m_vec180.end();
    for (; it != e; ++it)
    {
        emblem.stEmblemSocket[index].emblemSocketType_ = *it;
        index++;
    }

    if (slot < -1 || slot >= (int)m_avatarSelect.size())
        return;
    if (m_avatarSelect.empty())
        return;

    int selIndex = slot;
    if (slot == -1)
    {
        // 找第一个 m_field10 > 0 的选项
        std::vector<stAvatarTypeSelect_t>::const_iterator it2 =
            m_avatarSelect.begin();
        std::vector<stAvatarTypeSelect_t>::const_iterator e2 =
            m_avatarSelect.end();
        while (it2 != e2)
        {
            ++selIndex;
            if (it2->m_field10 > 0)
                break;
            ++it2;
        }
        if (it2 == e2)
            return;
    }

    const stAvatarTypeSelect_t* p = &m_avatarSelect.at(selIndex);
    if (index + (size_t)p->m_field10 > 4)
        return;

    // ORIG 增量段序：先 index++ 再 i++（`addl $0x1,-0x18` 在 `addl $0x1,-0xc` 之前）。
    for (int i = 0; i < p->m_field10; ++index, ++i)
        emblem.stEmblemSocket[index].emblemSocketType_ = p->m_socketType[i];
