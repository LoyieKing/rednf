CraneItemInfo* CraneMinigameScript::getItem(short idx)
{
    if (static_cast<unsigned int>(idx) < m_items.size())
    {
        return m_items.at(idx);   // ORIG 用 at（含界检）
    }
    return 0;
