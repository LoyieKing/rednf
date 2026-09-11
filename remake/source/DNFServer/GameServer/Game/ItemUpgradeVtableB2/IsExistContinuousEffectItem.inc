// ============================================================================
// expert_job::CAlchemist::IsExistContinuousEffectItem @ 0x085d121e
//   getCurCharacR 为空 → false；遍历当前角色持续道具表，命中 itemId 即以
//   「当前秒 < 到期秒」判定存在性。
// [b28d-solo-ItemUpgradeVtableB2] **AE 66/66**：旧判 N18b（成员==参数
//   mem-cmp 折叠）已翻正——ContinuousItemIdxView 同布局视图（unsigned 首字段
//   vs int 形参，P248 档③/P274）令 4.4 复现 ORIG 两段装载
//   `mov (%eax),%edx; mov 0xc(%ebp),%eax; cmp %eax,%edx`（探针 g 实证；
//   视图指针必须内联在条件里，具名会多地址槽）。
// ============================================================================
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
}