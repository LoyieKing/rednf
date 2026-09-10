void CUser::_processEffectItem(CStackableItem* item, unsigned int itemIdx,
                               unsigned int curSec)
{
    // P0/P183（b28d-G3C 复核 2026-09-10）：删具名 uchar tag 槽。ORIG 0x0865ec1d
    //   `movzbl 0x2e0(%eax),%eax; xor $0x1,%eax; test %al,%al; je <+0x58>`
    //   —— **无栈槽往返**（xor 在 %eax 全宽 = int 提升域，test 落 %al = 截断后判零）。
    //   我方原形多 3 条：`mov %al,-0x2d; xorb $1,-0x2d; cmpb $0,-0x2d`（独有 -0x2d 槽，
    //   并连带 StatChangeInfo 对象被顶到 -0x48，ORIG 在 -0x38）。改为内联表达式。
    if (!(unsigned char)(item->m_stat2dc.m_flagLo ^ 1))
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
    // 语义轴（b28d-G3C 2026-09-10）：ORIG 0x0865eca5 只读**一个**字节
    //   `movzbl -0x33(%ebp),%eax`（对象基址 -0x38 ⇒ 即 +0x5 = m_flagHi），
    //   我方原写 `m_flagLo != 0 || m_flagHi != 0` 多出 `movzbl -0x44; test; jne`
    //   两条（+0x4 的 m_flagLo）——ORIG 无此判断，删。
    if (stat.m_flagHi != 0)
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
