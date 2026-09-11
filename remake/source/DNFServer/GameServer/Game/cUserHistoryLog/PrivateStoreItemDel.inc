// ORIG T 0x868412a（报告全文）："Item-,..."——注意 %s 只有一个名字参数（param_8）。
// b21 NEAR：语义与 ORIG 全等（%s 仅 name2=param_8 正确）。修：补齐 item+0x00 字节 vararg。
// b28c-L2 fresh 复验 ndiff 13（orig 163/our 164）：封顶族与 PrivateStoreItemAdd 同
//   （N80 具名 print ctor 前实参保活 + N82附 聚合/标量槽向 + N18a 单段直落），判词见上。
// b28d-65 复核：去 (int)m_addInfo 冗余 cast ⇒ 163=163；残余同 Add（N80+N82附+N107）。
// [b28d-solo-cUserHistoryLog] DIFF 164/163：P227 同 PrivateStoreItemAdd；残差仅 N107。
// [2026-09-09 复核] 同上：仅剩 N107 ×1（16 形探针封顶）。
// [SweepHistoryLog2 2026-09-09 复核] 同上 12 形复验（引用形参恒 add、指针形参才 lea）⇒ 维持封顶。
void cUserHistoryLog::PrivateStoreItemDel(INVEN_TYPE invenType, Inven_Item const& item,
                                          int delNew, int delOld, eItemDelReason reason,
                                          char const* name1, char const* name2)
{
    m_user->LogHistory(
        "Item-,%d,%d,%d,%d,%d,\"%s\",%d,%d,"
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
        (int)invenType, item.m_addInfo, delNew, delOld, (int)reason,
        name2,
        item.GetUpgrade(),
        (unsigned)item.m_fieldb,
        ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26,
        ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28,
        ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
        ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c,
        ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e,
        ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
        ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
        item.m_field0,            // ORIG IVar1 = *param_3（item+0x00 字节）
        item.GetReSealCount(),
        CItemGloballyUniqueIdentifierPrint()(
            *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
}