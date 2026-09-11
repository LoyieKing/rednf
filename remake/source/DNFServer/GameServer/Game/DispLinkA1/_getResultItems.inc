// ============================================================================
// 13. Dispatcher_DisJointAvatar::_getResultItems (const)  ORIG 0x0821873c
//     DisJoint 表选素材：jewelTypeIndex==-1 → [grade][type] 区（+0x444 起），
//     否则按 job 遍历定位（+0xd4 起）；素材须为 stackable(0x10)，收集
//     getBoosterItem 条目 (m_itemIdx, m_addInfo)。
// [b28d-solo-DispLinkA1] verdict=DIFF N 封顶（186 vs 190，ndiff=52；p167 23/23 CLEAR）。
//   已修：P315/P167 字段 m_itemIdx int→unsigned int（CStackableItem.h）⇒
//   make_pair<unsigned int const&,unsigned short const&> 与
//   pair<int,int>::pair<unsigned,unsigned short> 实例、2 次 iterator→const_iterator
//   转换构造、ne<const*,const*> 全部归位；P47/P222 if/else 反转（je 落表查找块）；
//   P0 去 dj/grade/type 具名槽；P231 纯 register dj（非 asm 绑定）消槽。
//   残余 N：4.4 算术 lowering/寄存器角色差（N86/N87/N7 族）——ORIG dj→%esi 且
//   job 先于 get_grade 装载、grade*5 用 edx 累加、`add $0x34; mov 0x4(%esi,%eax,4)`
//   单条 scaled（成员数组视图缺 +4 表建模）；4.4 恒 dj→%ebx、job 后装、指针下标
//   `[i+K]` 折 shl+lea。P226/裸 register/基址+4 三形实测均不翻。
// ============================================================================
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
        // [M1S3EmblemLink 2026-09-11 负结论] 表视图基址改写试过
        //   `(int*)dj + 1` + 索引 +0x34/+0x110，实测 190 不变（ORIG 0x082187d3
        //   `add $0x34,%eax; mov 0x4(%esi,%eax,4),%eax` 单条 folded disp，
        //   4.4 恒 `add $0x35; shl $2; lea (%ebx,%eax,1); mov (%eax)`）⇒ 回滚。
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
}