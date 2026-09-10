bool CUser::LoadEffectItemInfo(CONTINUOUS_ITEM_INFO* info)
{
    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<ContinuousItemInfo>* list = GetCurCharacEffectItemList();
    list->clear();
    for (int i = 0; i < static_cast<int>(static_cast<signed char>(info->m_count)); ++i)
    {
        // b28d-21：ORIG 0x086590f0 每次字段访问都重发
        // `mov i,%edx; mov info,%eax; mov 0x8(%eax,%edx,8),%eax` 全链
        //（P183 地址重物化铁律）⇒ 不得缓存 `const CONT_ENTRY& entry`。
        // 守卫形态：ORIG 第 1 条件 jbe 直跳循环尾（无岛），第 2/3 条件各落独立
        // nop 中继岛（0x8659097/0x865909a）⇒ P223 多岛形：三条独立失败 goto +
        // 连续尾标签（标签序=守卫序）。
        if (info->m_entries[i].m_expireTime > curSec)
        {
            if (static_cast<unsigned int>(info->m_entries[i].m_itemIdx) > 0x7ffffffe)
                goto L2;
            if (info->m_entries[i].m_expireTime > 0x77359400)
                goto L3;
            {
                ContinuousItemInfo ci;
                ci.m_itemIdx = info->m_entries[i].m_itemIdx;
                ci.m_expireTime = info->m_entries[i].m_expireTime;
                _processSpeicalContinuousTimeItem(static_cast<unsigned int>(ci.m_itemIdx));
                list->push_back(ci);
            }
            goto NEXT;
        L2:;
        L3:;
        NEXT:;
        }
    }
    ClearUselessEffectItem(curSec);
    // b28d-21：ORIG 0x086590f0 尾 0x86590da `xor $1,%eax; test; je 0xe8(mov$1)`
    // = 4.1 bool 返回物化；4.4 恒折 `cmp $1; sete`（setcc-fold 族）⇒ N 封顶。
    if (SendEffectItemInfo(curSec) != 1)
        return false;
    return true;
