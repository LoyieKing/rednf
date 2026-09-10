bool expert_job::CAlchemist::IsExistContinuousEffectItem(CUser* user,
                                                         int itemId)
{
    // [b28d-64] P214①/P56：ORIG 0x085d1232 `test;setne;test;je <尾 mov$0>` =
    // 正向包裹（负形早返产 sete）；命中支 ORIG 0x085d12b5/12bc 两独立
    // `mov $1/$0 + jmp` 岛 ⇒ if/return 两条（直返比较结果会折 setb 直跳）。
    // 迭代器槽：ORIG begin→-0x10（浅）/end→-0x14（深）。
    if (((const CUserCharacInfo*)user)->getCurCharacR() != 0)
    {
        const std::vector<ContinuousItemInfo>* list =
            ((const CUserCharacInfo*)user)->GetCurCharacEffectItemListR();
        std::vector<ContinuousItemInfo>::const_iterator it = list->begin();
        std::vector<ContinuousItemInfo>::const_iterator last = list->end();
        for (; it != last; ++it)
        {
            // 视图指针必须**内联**在条件表达式里（P183）：具名指针会多一个
            // 地址槽（-0xc），使 it/last 槽整体下移 4（ORIG 直用 deEv 的 eax）。
            if (reinterpret_cast<const ContinuousItemIdxView*>(&(*it))
                    ->m_itemIdx == itemId)
            {
                // P167 新判据（b28c C28L8）：ORIG 全函数只调 `deEv`
                // （`(*it).m` 形），不调 `ptEv`（`it->m` 形）⇒ 此处写
                // `(*it).m_expireTime` 保持 deEv×2 / ptEv×0。
                if ((unsigned)GlobalData::s_systemTime_.getCurSec() <
                    (*it).m_expireTime)
                {
                    return true;
                }
                return false;
            }
        }
    }
    return false;
