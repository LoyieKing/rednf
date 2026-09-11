void CInventory::CalBlankSlot(int* out,
                              const std::vector<std::pair<int, int> >& items) const
{
    // [K8CInventory 2026-09-11 复核] 新鲜 .o 读数 **313/309**：唯一结构差 = 4 条 `nop`
    // （ORIG 对齐填充，实测位于 0x085085? 循环回边/0x0850857e 与 0x085088?? 段的 `jmp` 前），
    // 其余逐条一一对应；槽差 = start/end 一组整体浅 8（ORIG `lea -0x48/-0x4c`
    // vs 我方 `lea -0x40/-0x44`，见上述 -0x48:5↔-0x40:5 / -0x4c:3↔-0x44:3 引用计数互换）。
    // ⇒ N 定谳（对齐填充旗标 + 4.4 块内临时槽基），非声明序可调（本件 P212 已多形实测）。
    // b28d-36 [P159+P223]：ORIG 0x08508536 尾部 `nop; leave; ret`（无 mov$0）⇒ 返回 void；
    // 首守卫 ORIG 形 `sete %al; test; jne TAIL` + body 尾 `jmp`/TAIL `nop` ⇒
    // `if (items.size()==0) goto END;`（goto 尾标签产正向 jne；原地 return 产反向 je+eax 岛）。
    if (items.size() == 0)
    {
        goto CBLANKSLOT_END;
    }
    // [P28/P223] ORIG 0x08508536 布局 = begin 一次 + `jmp COND` + BODY→COND（while 形，非 for）；
    // 增量段调 ppEi（后置）⇒ 源写 it++；循环头 cur=std::pair(*it) + first/second 缓存（同 CalNeedSlot）。
    // [P223/BM8] 同 CalNeedSlot：goto 跨非平凡初始化非法 ⇒ 主体包入内层复合块、标签置块外。
    {
    // [P212] ORIG 0x08508536：itemIdx/count/type/start/end 在循环外声明（Ghidra local_1c/
    // local_14/local_10/local_48/local_4c 均为函数级），循环内仅赋值 ⇒ 提升后槽集对齐。
    int start;
    int end;
    // [判据⑩ 实测] ORIG 0x08508536 槽深序（深→浅）= found(-0x1d) / itemIdx(-0x1c) /
    // i(-0x18) / count(-0x14) / cnt(-0x10) / type(-0x0c)，start/end 最深(-0x48/-0x4c)
    // ⇒ 声明序照此排（§ 判据⑩：声明序 = 槽由深到浅）。原声明序 itemIdx/count/type/
    // i/cnt/found 使六个槽各自错位。
    bool found;
    int itemIdx;
    int i;
    unsigned int count;
    unsigned int cnt;
    // [CalNeedSlot 同法实证] type 声明为 enum ⇒ StartEndPos 实参复现 ORIG 的 %eax 串行装载。
    Inven_Item::ITEM_TYPE type;
    typedef std::vector<std::pair<int, int> >::const_iterator Iterator;
    Iterator it = items.begin();
    std::pair<int, int> cur;
    while (it != items.end())
    {
        found = false;   // ORIG 循环体首 `movb $0x0,-0x19`（每轮重置 local_21）
        cur = std::pair<int, int>(*it);
        itemIdx = cur.first;
        // [P196/P218] ORIG 0x085086a7 比较 `mov -0xc; cmp -0x10; jae`（无符号）⇒ count/found
        // 为无符号（Ghidra：uint local_10/local_14）；写 int 会产 jge（我们实测）。
        count = cur.second;
        if (itemIdx == 0 || itemIdx == 2)
        {
            it++;
            continue;
        }
        type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
        if (type == 0)
        {
            it++;
            continue;
        }
        // [P171 照抄] ORIG 0x08508536 在 StartEndPos 前**无** start/end 清零
        // （直证：`jmp`→`lea &end`→`mov 0xc(%esp)`→`lea &start`→`mov 0x8(%esp)`，
        // 无 `movl $0x0`）⇒ 我方原两句清零为多发，删除。
        StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
        if (type == 1)
        {
            cnt = 0;
            for (i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == itemIdx)
                {
                    cnt++;
                    // [b28d-tu-CInventory 复测] ORIG 0x08508671 段直证为 `mov slot,%eax;
                    // shl $0x2,%eax; add 0xc(%ebp),%eax` = **变量**索引 out[type]（常量
                    // out[1] 在 4.4 恒折 `mov out,%eax; add $0x4,%eax`，多失配）⇒ 改回变量形。
                    out[type]++;
                    if (cnt == count)
                    {
                        break;
                    }
                }
            }
            if (cnt < count)
            {
                for (i = 3; i < 9; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        cnt++;
                        out[0]++;
                        if (cnt == count)
                        {
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            // [ORIG 0x08508703 直证] 分支序 = `call IsCreatureItemType; test; je <非creature>`
            // ⇒ **creature 在前**（Ghidra C 显示为 `!IsCreatureItemType` 是其自身 if 反转）；
            // 内层 `call isEquipableItemType; test; je <非装备>` ⇒ 装备分支在前。
            if (IsCreatureItemType(type))
            {
                if (isEquipableItemType(type))
                {
                    cnt = 0;
                    for (i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            cnt++;
                            out[type]++;
                            if (cnt == count)
                            {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    for (i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                        {
                            out[type]++;
                            break;
                        }
                    }
                }
            }
            else
            {
                for (i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                        (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                    {
                        found = true;
                        out[type]++;
                        break;
                    }
                }
                if (!found)
                {
                    for (i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                        {
                            // [b28d-36 复测] 改 out[type] 实测 nd 256→326 反劣（本处确为
                            // ORIG 0x085088a3 base 直取形 out[0]）⇒ 回滚保留。
                            out[0]++;
                            break;
                        }
                    }
                }
            }
        }
        it++;
    }
    }
    // [P223] ORIG TAIL：body 尾 jmp 跳过 nop 到 leave;ret（ORIG 0x08508914 → 0x08508917）。
CBLANKSLOT_END:;
}