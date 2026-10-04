// [b28d-22 N82] ORIG 0x08690226：p167=CLEAR，gap=6；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08690226：N82 封顶——帧差 0x10（sub $0x34 vs
// $0x44）+ 槽序（p167 CLEAR）。
// [K7 2026-09-11 §6.5 PRETTY 串盲区] ORIG .rodata 实串 =
// `bool CUser::setTagCharacInven(TAG_CHARAC_INVEN_DATA*)` ⇒ 返回类型 bool（同 CUser.h:3815）。
bool CUser::setTagCharacInven(TAG_CHARAC_INVEN_DATA* data)
{
    if (getTagCharacR() == 0) {
        // [FL5 2026-09-10 负结论·已回滚] P37/P227 链在本件 ndiff 87→87 中性 ⇒ 维持具名形。
        cMyTrace trace(__PRETTY_FUNCTION__, 0xb166, 5);
        trace("CUser::setTagCharacInven m_tag_selected is null");
        return 0;
    }
    // [G6Sub1 单旋钮·P0/P183/P52] ORIG 0x08690281..0x086902a1 为
    // `mov 0xc(%ebp),%ebx`（data 先入 callee-saved）+ `call getTagCharacInvenW`
    // + `add $0x1c,%eax` ⇒ 结果**无栈槽往返**（P0：原码无该具名局部），
    // 且 data 值跨接收者调用保活（P52 R2L 形）。旧形 `Inven_Item* invenRaw =
    // getTagCharacInvenW();` 多出 `mov %eax,-0x1c` + 回读 + `lea 0x1c(%eax)` 三处
    // 槽往返（我方独有槽写入，父泳道帧差根因），改内联接收者表达式消除。
    memcpy(reinterpret_cast<CInventory*>(getTagCharacInvenW())->m_invenItem, data, 0x53e);
    // ORIG：getTagCharacInvenW 在 memcpy/SetClearAvatar(ulong*)/循环内
    // SetClearAvatar(i,idaf) 三处各自重调——访问器结果禁缓存（战术 P183/P48）。
    reinterpret_cast<CInventory*>(getTagCharacInvenW())
        ->SetClearAvatar(reinterpret_cast<unsigned long*>(data->m_clearAvatar));  // +0x3ff8
    for (int i = 0; i < data->m_avatarCount; ++i) {
        int invenSlot = 0;
        if (data->m_avatarDetail[i].m_clearAvatarId != 0) {
            invenSlot = GetItemIdByAddInfo(
                &data->m_items2[0], 0x69,
                data->m_avatarDetail[i].m_clearAvatarId);
            for (int j = 0; j < 10; ++j) {
                if (data->m_items[j].m_addInfo2 ==
                    data->m_avatarDetail[i].m_key) {
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
    // [M8COB 2026-09-11 P290 试形·中性回滚] ORIG 0x086902xx else 支 `mov 0x4020(%eax),%ebx`
    // （callee-saved 升格）；按 P290 试 unsigned 形实测 123/124 **完全中性**（ndiff 同）
    // ⇒ 4.4 不因 unsigned 升格（该升格属 4.1 receiver-call 后按值实参族，C07③ 边界）⇒ 回退 int。
    int capacity = static_cast<int>(data->m_cargoCapacity);  // +0x4020
    if (capacity == 0) {
        getTagCharacCargoW()->SetCapacity(8);
    }
    else {
        getTagCharacCargoW()->SetCapacity(capacity);
    }
    return 1;
}