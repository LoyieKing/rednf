int CInventory::erase_repeated_item(const std::vector<std::pair<int, int> >& items,
                                    std::vector<std::pair<int, int> >& out)
{
    // b28d-solo-CInventory：158/159，帧/槽残差 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：帧/槽残差（158/159）。
    // [P28/P315] ORIG 0x85056ca 增量段为 ppEi（后置）、push_back 实参为 const 左值
    // （_ZNSt6vectorISt4pairIiiESaIS1_EE9push_backERKS1_）⇒ it++ + 具名 pair 局部。
    // [实测] 进一步改 while+两处 it+++entry.first 赋值可使 p167 CLEAR（仅 C1/C2 别名），
    // 但 TU 四数 identical+ae 392→390（ae 263→261）⇒ 按「四数不回退」保留本形。
    // [b28d-round5 复攻失败记录，ORIG 0x085056ca 直证] ORIG 为 while 尾条件 + 两处
    // 独立 it++（type==0 分支 ppEi sret -0x20；循环尾 sret -0x1c）+ 函数级具名
    // it(-0x30)/entry(-0x38, C1Ev @0x085056e5)；本形实测 158/154 且 **TU 四数
    // 395→393 回退**（4.4 对 pair 默认构造省略 C1Ev call ⇒ 净少 4 条）⇒ 回滚保持 for 形。
    // [b28d-round5 复攻失败记录②] 只把 pair 提到循环外（保留 for、保留循环内 pair 构造
    // 位置改动）实测 158/148（我方少 10 条）+ 该 TU 合计回退 ⇒ 已回滚，维持循环内
    // 双参构造形（ORIG 的 pair::C1Ev 在入口是 4.1 形，4.4 对 trivial pair 不发该 call）。
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         it++)
    {
        int itemIdx = it->first;
        int type = GetItemType(itemIdx);
        if (type == 0)
        {
            continue;
        }
        // [判据⑫ 直证 clr_l orig=0 our=2] ORIG 无 start/end 清零（无 `movl $0x0,-N(%ebp)`）
        // ⇒ 删初始化器，直接传引用给 StartEndPos（P171 照抄）。
        int start;
        int end;
        StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
        int need = it->second;
        for (int i = start; i <= end; i++)
        {
            if (IsCreatureItemType(type))
            {
                if (m_pCreatureSlot[i].m_addInfo == itemIdx)
                {
                    if (isEquipableItemType(type))
                    {
                        need--;
                    }
                    else
                    {
                        need -= m_pCreatureSlot[i].m_addInfo2;
                    }
                }
            }
            else if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                if (m_pEquipSlot[i].m_field1 == 1)
                {
                    need--;
                }
                else
                {
                    need -= m_pEquipSlot[i].m_addInfo2;
                }
            }
        }
        if (it->second > 0)
        {
            std::pair<int, int> newItem(itemIdx, need);
            out.push_back(newItem);
        }
    }
    return out.size();
