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
