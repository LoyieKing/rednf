int CAvatarItemMgr::RegistItem(int key, int expireDate, int registTime, const char* ipgAgency,
                               const stAvatarEmblemInfo_t& emblem,
                               const stAvatarExpansionInfo_t& expansion, bool b)
{
    Avatar_Item* found = _FindItem(key);
    // [b28d-38] ORIG -0x10=_FindItem 结果、-0x2c=Acquire 结果（两个独立局部）；
    // 早返形（`cmpl $0,-0x10; je <主流程>` 由 4.1 早返布局产生）。
    if (found != 0)
        return 0;
    Avatar_Item* item = m_AvatarItemPool.Acquire();
    int expireDate2 = 0;
    if (b)
    {
        if (expireDate != 0 && expireDate != 0x16d)
            expireDate2 = expireDate * 0x15180 + registTime;
    }
    else
    {
        expireDate2 = expireDate;
    }
    item->SetData(expireDate2, registTime, ipgAgency);
    item->setEmblemSocketData(emblem);
    item->setExpansionInfo(expansion);
    m_registeredItems.insert(std::make_pair(key, item));
    AddEmblemEndurance(key, item);
    return 1;
}