int CCargo::_AddStackItem(const Inven_Item& item, int slot)
{
    if (m_pItems[slot].m_addInfo == item.m_addInfo) {
        m_sorted = 0;
        // [b28d-P0] ORIG 无槽往返（add 直入 sub 的寄存器）⇒ 内联实参。
        if (0x7fffffff - item.get_add_info() < m_pItems[slot].m_addInfo2) {
            m_pItems[slot].m_addInfo2 = 0;
        } else {
            // [b28d-P226] ORIG 两 get_add_info 在同一实参表达式内：首值升格 %ebx
            // 跨第二 call 保活（`call; mov %eax,%ebx; call; lea (%ebx,%eax,1),%edx`）
            // ⇒ 禁具名 cur/inc 局部（会槽化）。
            // [b28d-P168a①] ORIG `call; xor $0x1,%eax; test %al,%al; je <成功直落>`
            // ⇒ 否定守卫形（正极性 `if (call())` 4.4 产直 test 无 xor）。
            if (!checkStackableLimit((unsigned long)m_pItems[slot].m_addInfo,
                                     m_pItems[slot].get_add_info()
                                         + item.get_add_info()))
                return -10;
            // P48 去缓存：ORIG +0x0f3 对最后 += 的加数独立重调
            // item.get_add_info（不复用 inc）。
            m_pItems[slot].m_addInfo2 =
                m_pItems[slot].m_addInfo2 + item.get_add_info();
        }
        return slot;
    }
    return -1;
