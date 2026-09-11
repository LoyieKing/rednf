// ---------------------------------------------------------------------------
// cago_meterial_item_change(int, SIG_LOAD_INVENTORY*, Inven_Item*)
// ORIG T _Z25cago_meterial_item_changeiP18SIG_LOAD_INVENTORYP10Inven_Item
// @0x084c3f2c sz0x66。报告 _lt_global_gt_/cago_meterial_item_change.md：
// cargo 版仅做 find_index && m_field1==3 命中后的 m_field1=0xa 标记。
// ---------------------------------------------------------------------------
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
}