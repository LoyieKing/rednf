// ORIG 0x08658ff6：DB 载入 cooltime 表 → 过滤 → 清理无效 → 下发 0xac 包。
// b28d-solo-W8_CU_G: AE — 同 LoadEffectItemInfo：P183 去缓存引用 + P223 多岛形
//（ORIG 0x08659097/0x0865909a 两岛）+ SendCoolTimeItemInfo bool 返回旋钮
//（P248 档①，Main 代落 CUser.h:3574）⇒ `xor $1; test; je` 复现 0x086590da。
// 76/76 指令（ORIG 尾多 1 条对齐 nop，§6.1 豁免），compare_tu 判 AE。
bool CUser::LoadCoolTimeItemInfo(CONTINUOUS_ITEM_INFO* info)
{
    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<ContinuousItemInfo>* list = GetCurCharacCoolTimeItemList();
    list->clear();
    for (int i = 0; i < static_cast<int>(static_cast<signed char>(info->m_count)); ++i)
    {
        // b28d-21：同 LoadEffectItemInfo，ORIG 0x08658ff6 逐字段重发完整
        // imul/lea 寻址（P183）⇒ 不缓存 entry 引用。
        // b28d-solo-W8_CU_G：守卫「1 直落 + 2 岛」（ORIG 0x08659097/0x0865909a）
        // ⇒ 正向 if 包裹（第一守卫 jbe 直跳循环尾）+ 第二/三守卫独立 goto 尾标签。
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
                list->push_back(ci);
            }
            goto NEXT;
        L2:;
        L3:;
        NEXT:;
        }
    }
    ClearUselessCooltimeItem(curSec);
    // b28d-21：ORIG 0x08658ff6 尾 0x86590da `xor $1,%eax; test; je`（setcc-fold）⇒ N。
    if (SendCoolTimeItemInfo(curSec) != 1)
        return false;
    return true;
}