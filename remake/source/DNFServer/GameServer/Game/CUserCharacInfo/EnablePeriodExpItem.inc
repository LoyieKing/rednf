bool CUserCharacInfo::EnablePeriodExpItem()
{
    std::vector<ContinuousItemInfo>* v = &CUR->m_effectItemList.m_list;
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
