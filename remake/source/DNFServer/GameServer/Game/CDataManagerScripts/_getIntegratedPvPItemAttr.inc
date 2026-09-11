// ============================================================================
// _getIntegratedPvPItemAttr（ORIG T 0x84fc541，G3 PvP 物品属性）
// ORIG 反汇编（84fc541）：
//   it = G_CDataManager()->find_item(item.m_addInfo)；it==0 → 0；
//   虚表 +0xc（GetItemType）== 0xa → item.GetReSealCount() << 5；
//   0xb < GetItemType() <= 0x15 → item.GetReSealCount() << 5；
//   其余 → item.GetItemAttr()。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（5/5）。P9 —— GetReSealCount/GetItemAttr 返回 uchar（ORIG body 0x822b456 `movzbl 0x6(%eax),%eax; shr $0x5,%al` 实证），4.4 调用点恒插 `movzbl %al,%eax`（ORIG 无）；改共享头返回类型会动 Inven_Item.cpp body 与本 TU 全部调用点，保留；ORIG 0x084fc541。
int _getIntegratedPvPItemAttr(const Inven_Item& item)
{
    CItem* it = G_CDataManager()->find_item(item.m_addInfo);
    if (it == 0)
    {
        return 0;
    }
    if (it->GetItemType() == 0xa)
    {
        return (int)item.GetReSealCount() << 5;
    }
    if (it->GetItemType() > 0xb && it->GetItemType() <= 0x15)
    {
        return (int)item.GetReSealCount() << 5;
    }
    return item.GetItemAttr();
}