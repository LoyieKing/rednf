float CInventory::GetExpertJobSelfDisjointResultVariation(ENUM_EXPERT_JOB_TYPE type,
                                                          int param) const
{
    float total = 0.0f;
    if (type == 0)
    {
        return total;
    }
    for (int i = 10; i <= 0x15; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item((int)m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                total = total + pItem->GetExpertJobSelfDisjointResultVariation();
            }
        }
    }
    return total;
}