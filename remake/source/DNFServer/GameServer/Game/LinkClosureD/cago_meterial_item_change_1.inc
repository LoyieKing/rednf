void cago_meterial_item_change(int slot, SIG_LOAD_INVENTORY* sig, Inven_Item* item)
{
    // [b28c-lane24] ORIG 0x084c3f2c：两个失败路径各有 nop;jmp 中继岛
    //（+0x5c nop+jmp / +0x5f nop），赋值块尾另有 jmp 到统一 epilogue；
    // P223 goto 尾空标签形复现（4.4 对 && 短路恒 jump-thread 无岛）。
    if (sig == 0) goto L_sig;
    if (item == 0) goto L_item;
    if (ARAD::Singleton<ARAD::Arad_DataManager>::Get()->getExceptionItem()
            ->find_index(item->m_addInfo)
        && item->m_field1 == 3)
    {
        item->m_field1 = 0xa;
    }
L_item:;
L_sig:;
