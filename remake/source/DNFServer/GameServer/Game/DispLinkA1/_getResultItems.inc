int Dispatcher_DisJointAvatar::_getResultItems(
    CUser* user, CItem const* pItem, int jewelTypeIndex,
    std::vector<std::pair<int, int> >& results) const
{
    (void)user;
    int materialIdx = 0;
    // [b28d-solo-DispLinkA1 P47/P222] ORIG 布局：`je` 跳到函数后部的表查找块，
    // job 遍历块在 fall-through ⇒ 源码条件取反、两臂互换。
    if (jewelTypeIndex != -1) {
        char job = '\xb';
        for (int i = 0; i < 0xb; ++i) {
            if (pItem->check_job_type((char)i))
                job = (char)i;
        }
        if (job == '\xb')
            return 0x16;
        // [b28d-solo-DispLinkA1 P0] ORIG 无具名 dj/grade 槽（dj→%esi、job→%ebx
        // 跨调用保活），全部内联表达式。
        // §4.3 例外（P156 表寻址 intAtConst）：ORIG 把 DisJoint 对象首部起
        // +4 处的跨成员大表当 int 数组访问（dj+4+idx*4），DisjointScript 现有
        // 成员建模（vector 族）无法线性表达该视图，保留地址形式并注明。
        // [b28d-solo-DispLinkA1 P231] ORIG 无 dj 栈槽（`mov 0x1c(%eax),%esi`
        // 单段落 callee-saved）⇒ 纯 register 存储类提示（非 asm 绑定，P0 合规）。
        register DisJoint* const dj = G_CDataManager()->m_disJoint;
        materialIdx = reinterpret_cast<int*>(dj)
            [pItem->get_grade() * 5 + (int)job * 0x14 + jewelTypeIndex + 0x35];
    }
    else {
        register DisJoint* const dj = G_CDataManager()->m_disJoint;
        materialIdx = reinterpret_cast<int*>(dj)
            [pItem->get_grade() * 10 + pItem->GetItemType() + 0x111];
    }

    if (materialIdx == 0)
        return 0x16;

    CStackableItem* const mat =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(materialIdx));
    if (mat == 0 || mat->GetItemType() != 0x10)
        return 0x16;

    std::vector<stStackableBoosterItemInfo_t> boosters;
    mat->getBoosterItem(boosters);
    // [b28d-solo-DispLinkA1 P167/P13] ORIG begin()/end() 各求值一次并各触发一次
    // iterator→const_iterator 转换构造，循环条件用 ne<const*,const*>。
    std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
        boosters.begin();
    std::vector<stStackableBoosterItemInfo_t>::const_iterator end =
        boosters.end();
    for (; it != end; ++it) {
        results.push_back(std::make_pair(it->m_itemIdx, it->m_addInfo));
    }
    return 0;
