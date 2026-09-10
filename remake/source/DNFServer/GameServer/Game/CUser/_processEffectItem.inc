void CUser::_processEffectItem(CStackableItem* item, unsigned int itemIdx,
                               unsigned int curSec)
{
    // X2 字段形（判据②③：ORIG 0x0865ec1d `movzbl 0x2e0; xor $1; test al; je 消费者`
    // ——`!= 1` 直写 4.4 产 `cmp $1,%al`，uchar 绑 eax + ^=1 复现 xor 段）。
    unsigned char tag =
        (unsigned char)item->m_stat2dc.m_flagLo;
    tag ^= 1;
    if (! tag)
    {
        cMyTrace("void CUser::_processEffectItem(CStackableItem*, "
                 "unsigned int, unsigned int)", 0x391e, 0)
            ("ERROR : Mismatch tag [stat change duration] and "
             "[effect maintenance]. There is no duration value.");
        return;
    }
    unsigned int durationSec = (unsigned int)item->m_stat2dc.sec / 1000;
    unsigned int expireSec = curSec + durationSec;
    CStackableItem::StatChangeInfo stat = item->GetStatChangeInfo();
    if (stat.m_flagLo != 0 || stat.m_flagHi != 0)
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, GetUID(),
                                    (TIMER_MESSAGE)0x76, (int)durationSec,
                                    (int)itemIdx, (unsigned int)getCurCharacNo());
    }
    std::vector<ContinuousItemInfo>* effect =
        GetCurCharacEffectItemList();
    bool found = false;
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        if ((*effect)[i].m_itemIdx == (int)itemIdx)
        {
            if ((*effect)[i].m_expireTime < expireSec)
                (*effect)[i].m_expireTime = expireSec;
            found = true;
        }
    }
    if (! found)   // bool 值域聚合位，X2 免约束（判据①，ORIG 尾 testb %al 直判）
    {
        ContinuousItemInfo info;
        info.m_itemIdx = (int)itemIdx;
        info.m_expireTime = expireSec;
        effect->push_back(info);
    }
    UpdateEffectItemInfo(curSec);
