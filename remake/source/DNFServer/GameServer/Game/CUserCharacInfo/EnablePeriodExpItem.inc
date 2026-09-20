// [b28d-L14 N18a/N95] ORIG 0x08659c50：增量段 ppEi ⇒ 源写 `it++`（P28，已改）；
// 残差 `mov (%eax),%eax; mov %eax,%ebx` 4.4 折单段 + 槽 -0x28 vs -0x24。
// [b28d-solo-CUserCharacInfo AE] 95=95：①`return 0;`→`break;`（P214② 共享尾块）已落；
// ②P254 enum 视角：ContinuousItemInfo 加匿名 union 成员 EContinuousItemIdx
// m_itemIdxEnum（4 字节等宽，原 m_itemIdx int 全仓消费点不变）⇒ 复现 ORIG
// `mov (%eax),%eax; mov %eax,%ebx` 两段中转。
bool CUserCharacInfo::EnablePeriodExpItem()
{
    std::vector<ContinuousItemInfo>* v = &m_selected->m_continuousItemListB;
    std::vector<ContinuousItemInfo>::iterator it = v->begin();
    unsigned int now = GlobalData::s_systemTime_.getCurSec();
    for (; it != v->end();)
    {
        // P254 enum 视角：复现 ORIG `mov (%eax),%eax; mov %eax,%ebx` 两段中转。
        CItem* item = G_CDataManager()->find_item(it->m_itemIdxEnum);
        if (item == 0)
            return 0;
        if (item->is_stackable())
        {
            CStackableItem* stack = (CStackableItem*)item;
            if (stack->IsExpAffect())
            {
                unsigned int expire = it->m_expireTime;
                if (now < expire)
                    return 1;
                it = v->erase(it);
                SetExpAffectItemEffect(false, 0.0f);
                break;
            }
        }
        // [b28d-L14 实证] ORIG 0x08659cf0 增量段调 ppEi（后置）⇒ 源写 it++（P28）。
        it++;
    }
    return 0;
}