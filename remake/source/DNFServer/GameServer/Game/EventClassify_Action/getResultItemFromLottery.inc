// ============================================================================
// getResultItemFromLottery（ORIG 0x0810dc5a）
// 按 lotterycard/booster 两类物品展开为 <idx,count> 列表：
//   GetItemType()==6  → find_item + get_lotto_item → push(m_addInfo,m_addInfo2)
//   GetItemType()==16 → getBoosterItem(list) → 逐条 push(m_itemIdx,m_addInfo)
// 其它类型返回 0x17；find 失败或空结果返回 0x11。
// [b28d-solo-EventClassify_Action] ORIG 0x0810dc5a，176 vs 170（差 6 条=唯一残差），
//   N 封顶。残差=case 16 循环内 `if (it->m_itemIdx == 0) return 0x11;` 的 0/1 物化：
//   ORIG(4.1) 发「`call ptEv; mov (%eax),%eax; test; je L1; call ptEv(丢弃);
//   mov $0; jmp L2; L1: mov $1; L2: test %al,%al; je`」= 分支+常量且条件表达式二次
//   求值；4.4 恒折 `test; sete %al; test; je`（一次求值）。已试 `==0` / `!` /
//   具名 bool / while 四形，sete 恒不变 ⇒ setcc-fold 跨版本负例（P197④ 族）。
//   可修部分已落：CStackableItem* stackable 具名局部（ORIG -0xc）复现
//   `mov -0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` 两条。
// ============================================================================
int CEventActionMng::getResultItemFromLottery(unsigned int lotteryId,
                                              std::vector<std::pair<int, int> >& out)
{
    CItem* citem = G_CDataManager()->find_item((int)lotteryId);
    if (citem == 0)
        return 0x11;

    switch (citem->GetItemType())   // ORIG vtable slot +0x0c
    {
    case 6:
    {
        Inven_Item item;
        // [b28d-L14 实证] ORIG 0x0810dc5a 直取 m_itemList 成员（无 getItemList
        // call，P167 差集）；make_pair 实例为 <unsigned long&,int&>（m_addInfo
        // 以 unsigned long 左值视图入参，P61/P211）。
        G_CDataManager()->m_itemList->get_lotto_item(
            citem->get_index(), reinterpret_cast<char*>(&item));
        if (item.m_addInfo == 0 || item.m_addInfo == -1)
            return 0x11;
        out.push_back(std::make_pair(
            reinterpret_cast<unsigned long&>(item.m_addInfo),
            item.m_addInfo2));
        break;
    }
    case 16:
    {
        CStackableItem* stackable = static_cast<CStackableItem*>(citem);
        std::vector<stStackableBoosterItemInfo_t> boosters;
        stackable->getBoosterItem(boosters);
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
                 boosters.begin();
             it != boosters.end(); ++it)
        {
            // [N2-S4 2026-09-11] 复现 ORIG 的条件结果 0/1 物化（gap 6→1）：
            // ORIG 0x0810ddb1 `test %eax,%eax; je L1; <重算表达式>; mov $0x0,%eax;
            // jmp L2; L1: mov $0x1,%eax; L2: test %al,%al; je`（4.1 对带副作用的
            // bool 上下文二次求值）；4.4 直接初始化 `bool x = (p->m == 0)` 恒折
            // 单条 sete（our=171），改 if/else 显式赋值可复现「分支+常量」物化
            // （our=175）。负结论：三元 `? true : false`(171)、int if/else(175 同形)。
            // 残余 +1 = 4.4 不复现 4.1 的二次求值（operator-> 重调），跨版本封顶。
            bool emptyItem;
            if (it->m_itemIdx == 0)
                emptyItem = true;
            else
                emptyItem = false;
            if (emptyItem)
                return 0x11;
            // ORIG make_pair<unsigned int const&, unsigned short const&>
            // （m_itemIdx 字段以 unsigned int 左值视图入参，P61/P211）
            out.push_back(std::make_pair(
                reinterpret_cast<unsigned int const&>(it->m_itemIdx),
                it->m_addInfo));
        }
        break;
    }
    default:
        return 0x17;
    }
    return 0;
}