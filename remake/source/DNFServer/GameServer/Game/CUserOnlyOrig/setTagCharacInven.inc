int CUser::setTagCharacInven(TAG_CHARAC_INVEN_DATA* data)
{
    if (getTagCharacR() == 0) {
        // [FL5 2026-09-10 负结论·已回滚] P37/P227 链在本件 ndiff 87→87 中性 ⇒ 维持具名形。
        cMyTrace trace(__PRETTY_FUNCTION__, 0xb166, 5);
        trace("CUser::setTagCharacInven m_tag_selected is null");
        return 0;
    }
    Inven_Item* invenRaw = getTagCharacInvenW();
    memcpy(reinterpret_cast<char*>(invenRaw) + 0x1c, data, 0x53e);
    // ORIG：getTagCharacInvenW 在 memcpy/SetClearAvatar(ulong*)/循环内
    // SetClearAvatar(i,idaf) 三处各自重调——访问器结果禁缓存（战术 P183/P48）。
    reinterpret_cast<CInventory*>(getTagCharacInvenW())
        ->SetClearAvatar(reinterpret_cast<unsigned long*>(data->m_clearAvatar));  // +0x3ff8
    for (int i = 0; i < *reinterpret_cast<int*>(data->m_pad1e43); ++i) {  // +0x1e43 count（未建模）
        int invenSlot = 0;
        if (*reinterpret_cast<int*>(data->m_pad1e43 + 0x29 + i * 0x4b) != 0) {  // +0x1e6c（未建模）
            invenSlot = GetItemIdByAddInfo(
                &data->m_items2[0], 0x69,
                *reinterpret_cast<int*>(data->m_pad1e43 + 0x29 + i * 0x4b));
            for (int j = 0; j < 10; ++j) {
                if (data->m_items[j].m_addInfo2 ==
                    *reinterpret_cast<int*>(data->m_pad1e43 + 4 + i * 0x4b)) {  // +0x1e47（未建模）
                    reinterpret_cast<CInventory*>(getTagCharacInvenW())
                        ->SetClearAvatar(j, invenSlot);
                }
            }
        }
    }
    // [A06 2026-09-10 C07 新命中形 试行记录·已回退] ORIG 0x086902xx 条件为寄存器直比
    // （O100-102 `mov 0x4020(%eax),%eax; test %eax,%eax; jne <else>`），else 支**二次读字段**
    // 并入 %ebx 跨 receiver call 保活（O110-111 `mov 0x4020(%eax),%ebx` → O115 传参）。
    // 按 C07 删具名局部（改内联两次读）实测：123/129→123/**130**（+1 条）、delta 40→34；
    // 新序列 else 支仍 `mov 0x4020(%eax),%eax` **未升格 %ebx** ⇒ 4.4 不产该升格
    // （命中 Main 转发的 C07③ 边界：消费者为 receiver call 后的按值实参时不适用）⇒ 回退。
    int capacity = static_cast<int>(data->m_cargoCapacity);  // +0x4020
    if (capacity == 0) {
        getTagCharacCargoW()->SetCapacity(8);
    }
    else {
        getTagCharacCargoW()->SetCapacity(capacity);
    }
    return 1;
