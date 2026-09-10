void CUserCharacInfo::GetContinuousItemInfo(SIG_LOAD_ETC* p) const
{
    p->m_cooltimeCharacNo = p->m_characNo;
    p->m_effectCharacNo = p->m_characNo;
    const std::vector<ContinuousItemInfo>* cool = GetCurCharacCoolTimeItemListR();
    p->m_cooltimeItemCount = (unsigned char)cool->size();
    int index = 0;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        ((LoadEtcCoolItemWindow*)p)->m_entries[index + 0x460].m_itemIdx =
            (*cool)[i].m_itemIdx;
        ((LoadEtcCoolItemWindow*)p)->m_entries[index + 0x460].m_expireTime =
            (*cool)[i].m_expireTime;
        ++index;
    }
    index = 0;
    const std::vector<ContinuousItemInfo>* effect = GetCurCharacEffectItemListR();
    p->m_effectItemCount = (unsigned char)effect->size();
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        ((LoadEtcEffectItemWindow*)p)->m_entries[index + 0x494].m_itemIdx =
            (*effect)[i].m_itemIdx;
        ((LoadEtcEffectItemWindow*)p)->m_entries[index + 0x494].m_expireTime =
            (*effect)[i].m_expireTime;
        ++index;
    }
