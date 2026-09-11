// ---------------------------------------------------------------------------
// meterial_item_change(int, SIG_LOAD_INVENTORY*, Inven_Item*)
// ORIG T _Z20meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item @0x084c3da5
// sz0x187。报告 docs/class_func_reports/_lt_global_gt_/meterial_item_change.md。
// sig/item 空判 → logFormat；ExceptionItem::find_index(item->m_addInfo) 且
// m_field1==3 命中 → m_field1=0xa；do-while i=0xc9..0xf8 扫 SIG 材料槽
// （0x5949 + i*0x3d，步长 0x3d）：空槽 setCopy+reset 返回；同 addInfo 槽
// m_addInfo2 累加后 reset 返回；溢出 logFormat 0x161d。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] NEAR | 已收：P183 去 dst 缓存（ORIG 每点重发
// imul 链，0x084c3e40/3e5e/3ed7/3efb 四组）+ P214① 正极性 if(A&&B) 包裹
// （0x084c3e11..e2e 短路物化 mov$1/mov$0 + test 无槽）+ P283 goto 固定物理
// 块序（入口 jmp→非空处理→++idx/尾测 jle 直跳→overflow→体头 isEmpty
// xor$1;jne→空处理）。残余 = ORIG `add $0x5950` vs 我们 `add $0x5940;
// mov 0x10(%eax)`（N4/P30 常量折叠）+ 2 条对齐 nop（§6.1）。
void meterial_item_change(int slot, SIG_LOAD_INVENTORY* sig, Inven_Item* item)
{
    if (sig == 0 || item == 0)
    {
        LogManager::logFormat(1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x1604, "selected_item | pInfo is null");
        return;
    }

    int idx = 0xc9;
    // ORIG 0x084c3e11..e2e：&& 短路物化 mov$1/mov$0 + test（无槽、无 xor）⇒
    // P214① 正极性包裹 `if (A && B) { 主流程 }`（负极性/取反形恒反演块序）。
    // ORIG 每处访问重发完整 imul/add 链（0x084c3e40/3e5e/3ed7/3efb 四组）⇒
    // 无 dst 指针缓存（P183 地址重物化铁律），逐点全表达式访问。
    if (ARAD::Singleton<ARAD::Arad_DataManager>::Get()->getExceptionItem()
            ->find_index(item->m_addInfo)
        && item->m_field1 == 3)
    {
        item->m_field1 = 0xa;
        // ORIG 物理块序：入口 jmp → 非空处理(+0x9b) → ++idx/尾测(+0xf7..102,
        // `jle` 直跳) → overflow(+0x104) → 体头 isEmpty(+0x132, `xor$1;jne`) →
        // 空处理(+0x156)。用 goto 固定物理顺序（P283），do-while 在 4.4 恒 setle。
        goto L_check;
    L_body:
        if (sig->m_invenType1[idx].m_addInfo == item->m_addInfo)
        {
            sig->m_invenType1[idx].m_addInfo2 += item->m_addInfo2;
            item->reset();
            return;
        }
        ++idx;
        if (idx <= 0xf8)
            goto L_check;

        LogManager::logFormat(1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x161d, "new_meterial_slot_index overflow");
        return;

    L_check:
        if (!sig->m_invenType1[idx].isEmpty())
            goto L_body;
        sig->m_invenType1[idx].setCopy(*item);
        item->reset();
        return;
    }
}