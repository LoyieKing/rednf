void CInventory::checkEquipmentState(int a, int b, int& c, int& d, int& e, int& f, int& g,
                                     int& h)
{
    // b28d-solo-CInventory [173/171；P223 岛已复现，余 2 条跨版本 codegen 封顶]
    // ORIG 0x850ce40：①两守卫 `cmpl $8; jg <nop 岛>`/`cmpl $4; jg <nop 岛>`
    // （+0x20c/+0x20f 两独立 landing）由双否定 goto 尾标签串复现；②for 内
    // `if (i <= 0x13)` 翻正条件 `i > 0x13` 复现 ORIG「else 直落 + then 后置跳转」。
    // [b28d-Inventory2 本轮] 循环内两岛（0x850d033 nop;jmp / 0x850d036 nop）由
    // `if (addInfo==0) goto l_ces_island1; … if (i>0x10) goto l_ces_island2;`
    // + 尾标签串 `l_ces_island1:; continue; l_ces_island2:;` 复现（P223），gap 6→2。
    // 残余 2 条跨版本 codegen 无源形旋钮（探针 f1/f2/f5/f6 实测）：①`mov 0x10(%ebp),%ecx`
    // 先载形参（P75，4.4 恒 `add mem,%eax`；unsigned cast 反产 3 条中转）②`shl $2`+
    // `mov TBL(%eax)` 强度归约（N4/N86b，4.4 恒 scaled `mov TBL(,%eax,4)`；指针算术
    // 反产 `add $TBL,%eax; mov (%eax)` 3 条）⇒ 判语义完成、byte 层余 2 条。
    if (a > 8) goto l_ces_skip1;
    if (b > 4) goto l_ces_skip2;
    {
        if (m_invenItem[10].m_addInfo == 0)
        {
            c++;
        }
        else if (m_invenItem[10].m_fieldb == 0)
        {
            f++;
        }
        for (int i = 0xc; i < 0x16; i++)
        {
            // b28d-solo-CInventory [P59/P222]：ORIG 为 else 块直落、then 块后置跳转
            // （`cmpl $0x13; jle <then>`）⇒ 源码翻转成正条件 `i > 0x13` 包 else 体，
            // 4.4 即产 `jle <else>` 与 ORIG 同布局。
            if (i > 0x13)
            {
                // b28d-Inventory2 [P223]：ORIG 0x850ceb9 `je 0850d033`（岛1 nop;jmp
                // 到循环尾）⇒ 裸条件 goto + 尾空标签串；标签序 = 守卫序。
                if (m_invenItem[i].m_addInfo == 0) goto l_ces_island1;
                g++;
            }
            else
            {
                if (m_invenItem[i].m_addInfo == 0)
                {
                    c++;
                }
                if (i > 0x10) goto l_ces_island2;
                {
                    CEquipItem* pItem = (CEquipItem*)G_CDataManager()->find_item(
                        m_invenItem[i].m_addInfo);
                    if (pItem != 0)
                    {
                        if (m_invenItem[i].m_fieldb == 0)
                        {
                            f++;
                        }
                        int subType = pItem->get_sub_type();
                        if (a == 4 && b == 3)
                        {
                            if (subType == 0 || subType == 3 || subType == 4)
                            {
                                h++;
                            }
                            else
                            {
                                d += 2;
                                e = 2;
                            }
                        }
                        // ORIG 0x850cfbe：mov _S_CHARAC_JOB_MASTARY(,%eax,4)（eax=a*5+b）
                        // —— int 数组下标读（checkEquipmentState.md）。
                        // [b28d-PoolMisc2 实测 2 形均不闭合] ①`b + a*5` 加数交换 ⇒ 4.4 仍
                        // `add 0x10(%ebp),%eax`（零效）；②`(a*5)` 改显式指针算术
                        // `*(int*)((char*)TBL + ((a*5)<<2))` ⇒ 复现 ORIG `shl $2` 但 4.4 改产
                        // `add $TBL,%eax; mov (%eax)`（1 条差异换成 1 条，ndiff 8→8，指令数
                        // 169→171 无收益，已回滚）。⇒ P75/N86b 封顶维持。
                        else if (_S_CHARAC_JOB_MASTARY[a * 5 + b] == subType)
                        {
                            h++;
                        }
                        // ORIG 0x850cfe6：mov _S_CHARAC_JOB_MASTARY(%eax)，eax=(a*5)*4。
                        else if (_S_CHARAC_JOB_MASTARY[a * 5] == subType)
                        {
                            d++;
                            e = e == 2 ? 2 : 1;
                        }
                        else
                        {
                            d += 2;
                            e = 2;
                        }
                    }
                }
            }
        l_ces_island1:;
            continue;
        l_ces_island2:;
        }
    }
l_ces_skip1:;
l_ces_skip2:;
}