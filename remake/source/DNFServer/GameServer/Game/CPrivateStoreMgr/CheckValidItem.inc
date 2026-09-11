// CheckValidItem(PrivateStoreItem&, int prevAddInfo2) @0x85c9d4e：
// 物品存在/可售类型/生物类排除（vtable+0x14=IsCreatureItem）后按 item space
// 分派来源槽校验；可堆叠条目要求现库存 >= 上架前数量。
// [b28d-24] AE（P167 归零，clean 后 0 残差）。ORIG 0x085c9d4e
bool CPrivateStore::CheckValidItem(PrivateStoreItem& item, int prevAddInfo2)
{
    CItem* info = G_CDataManager()->find_item((int)item.m_item.m_addInfo);
    if (info == 0)
        return false;

    int attachType = info->GetAttachType();
    if (attachType == 0)
        ;    // ORIG je +0x92：放行继续
    else if (attachType == 3)
    {
        if ((int)item.m_item.m_field0 != 1)
            return false;
    }
    else if (attachType == 7)
    {
        if (!item.m_item.GetTradeLimitCount())
            return false;
    }
    else
        return false;    // ORIG default 失败岛（85c9dd6）在 7 分支体之后

    if (IsCreatureItemType((unsigned int)(unsigned char)item.m_item.m_field1))
        return false;

    // ORIG 经 CItem vtable+0x14 虚调（= IsCreatureItem），命中即拒。
    if (info->IsCreatureItem())
        return false;

    // ORIG 短路直跳形（test;je cont; cmp;jge cont; mov$0）——勿用 && 汇聚形
    if (info->is_stackable())
    {
        if ((int)item.m_item.m_addInfo2 < prevAddInfo2)
            return false;
    }

    // 真语义 bug 修复：ORIG 决策树 0x85c9e5c `test%eax;je 0x126`（case0）、
    // 0x85c9e51 `je 0x19d`（case1）、0x85c9e68 `je 0x17a`（case2）、
    // 0x85c9e6d `je 0x151`（case3）与各叶块实参联立 ⇒ case0=type1、
    // case1=type2、case2=int 版、case3=type0/count1。旧重建把 0/1/3 三标签
    // 的体循环错位。叶块物理序 = case 书写序（P46）⇒ 按 0,3,2,1 书写。
    switch ((signed char)item.m_field46)
    {
    case 0:
        return CheckValidItem((INVEN_TYPE)1, (short)item.m_field48,
                              (int)item.m_item.m_addInfo2);
    case 3:
        return CheckValidItem((INVEN_TYPE)0, (short)item.m_field48, 1);
    case 2:
        return CheckValidItem((short)item.m_field48, (int)item.m_item.m_addInfo2);
    case 1:
        return CheckValidItem((INVEN_TYPE)2, (short)item.m_field48,
                              (int)item.m_item.m_addInfo2);
    default:
        return false;
    }
}