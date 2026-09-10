bool CEnchanter::IsRecipeRight()
{
    if (reinterpret_cast<CItem*>(m_field40) == 0)
        return 0;
    if (!reinterpret_cast<CItem*>(m_field40)->is_stackable())
        return 0;
    if (reinterpret_cast<CItem*>(m_field40)->GetItemType() != 3)
        return 0;
    return 1;
