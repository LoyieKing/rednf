void CInventory::CalNeedSlot(int* out,
                             const std::vector<std::pair<int, int> >& items) const
{
    // b28d-36 [P159+P223]：ORIG 0x0850891a 尾部 `nop; leave; ret`（无 mov$0）⇒ 返回 void；
    // 首守卫同 CalBlankSlot：ORIG `sete; test; jne TAIL` + body 尾 jmp ⇒ goto 尾标签形。
    if (items.size() == 0)
    {
        goto CNEEDSLOT_END;
    }
    // ORIG（0x0850891a）：begin 具名量 + pair 默认构造 cur、循环头 found=false、
    // cur = 临时 pair 右值赋值（范式 P-358）、it++ 后置自增 ×3。
    // [P223/BM8] 4.4 拒 goto 跨同层非平凡局部声明（crosses initialization）⇒ 主体包入
    // 内层复合块、尾标签置块外（跳出不进入作用域，合法）。
    {
    // [P212] ORIG 0x0850891a：start/end/i 在循环外声明（start -0x44 / end -0x48 / i -0x14
    // 各仅一份，两处 StartEndPos 与两处 for 复用）⇒ 提升为块级声明、循环内仅赋值
    // （消除两对 start/end 的重复槽：帧 0x68→0x58 对齐 ORIG）。
    // [G8InvenPvP 子代理 G8CInvSmall · 探针实证 /tmp/g8c_probeB.o] 迭代器/当前对先声明：
    // ORIG 0x08508938 `lea -0x38(%ebp),%eax`（begin 输出槽 = it）、0x0850894d
    // `lea -0x40(%ebp),%eax`（cur 的 pair::operator= this）、StartEndPos 实参
    // `lea -0x48→0xc(%esp)`(end) / `lea -0x44→0x8(%esp)`(start)、0x08508a7e
    // `mov -0x44(%ebp),%eax; mov %eax,-0x14`(i=start)。把 it/cur 声明提到 start/end
    // 之前后，4.4 逐槽复现 ORIG 的 it(-0x38)/cur(-0x40)/start(-0x44)/end(-0x48)；
    // 浅区(found -0x19 / itemIdx -0x18 / i -0x14 / count -0x10 / type -0xc)与帧
    // 0x58 均不变（探针 A=现形 复现我方旧槽组，B=本形 匹配 ORIG）。
    // [G8InvenPvP 子代理 G8CInvSmall · 实测归零] 改后串行编译产物 /tmp/tu_CInventory.o
    // （2026-09-10 23:49）对 ORIG 报告规范化逐条比对：239/239 指令，
    // raw ndiff = 0 / STRUCT ndiff = 0 ⇒ 本件 identical；改前 4 槽错位
    // （it -0x40/cur -0x48/start -0x38/end -0x3c）全消。勿再动本件声明序。
    typedef std::vector<std::pair<int, int> >::const_iterator Iterator;
    Iterator it = items.begin();
    std::pair<int, int> cur;
    int start;
    int end;
    // [判据⑩ 实测序] ORIG 槽深→浅 = found(-0x19) / i(-0x18) / count(-0x14) /
    // itemIdx(-0x10) / type(-0x0c) ⇒ 声明序照此排（found..itemIdx 提升块级、循环内仅赋值）。
    bool found;
    int itemIdx;
    int i;
    unsigned int count;
    while (it != items.end())
    {
        found = false;
        cur = std::pair<int, int>(*it);
        itemIdx = cur.first;
        count = cur.second;
        if (itemIdx == 0)
        {
            it++;
            continue;
        }
        // [GetInvenData 实证·P211 档①] type 声明为 enum ⇒ 复现 ORIG StartEndPos 实参的
        // %eax 串行装载（`lea end;mov 0xc(%esp);lea start;mov 0x8(%esp);mov type,%eax;mov 0x4(%esp)`）；
        // int 局部时 4.4 提前载 type 并使两个 lea 走 %edx。ORIG 0x08508? 直证。
        Inven_Item::ITEM_TYPE type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
        if (type == 0)
        {
            it++;
            continue;
        }
        if (type == 1)
        {
            out[type] += count;
        }
        else if (IsCreatureItemType(type))
        {
            if (isEquipableItemType(type))
            {
                out[type] += count;
            }
            else
            {
                StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
                for (i = start; i <= end; i++)
                {
                    if (m_pCreatureSlot[i].m_addInfo == itemIdx)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    out[type]++;
                }
            }
        }
        else
        {
            if (type == 8)
            {
                if (getAvatarEmptySlot(0, 0x68) != -1)
                {
                    out[type]++;
                }
            }
            else
            {
                StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
                for (i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    // [判据·P212] ORIG 全函数仅一个循环下标槽（local_18）⇒ 内层循环复用块级 i。
                    for (i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found)
                {
                    out[type]++;
                }
            }
        }
        it++;
    }
    }
    // [P223] ORIG TAIL：body 尾 jmp 跳过 nop 到 leave;ret。
CNEEDSLOT_END:;
