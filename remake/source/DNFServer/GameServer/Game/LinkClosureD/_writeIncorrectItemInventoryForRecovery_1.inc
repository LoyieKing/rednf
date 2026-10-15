// ---------------------------------------------------------------------------
// _writeIncorrectItemInventoryForRecovery(uint, const Inven_Item&, const char*,
//                                        int, int)
// ORIG W _Z39_writeIncorrectItemInventoryForRecoveryjRK10Inven_ItemPKcii
// @0x084f0075 sz0xd0。报告缺失（objdump 转写）：单条 ERROR_ITEM_LOG 日志，
// 字段序（含 m_amp 首 4 字节 int 视图）见 logFormat 实参。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] AE | 实参槽序按 ORIG 0x084f00b9..010d 修正为
// characNo,field0,field1,addInfo,GetItemAttr,addInfo2(+0x7),fieldb(+0xb),
// fieldd(+0xd),amp 首 int(+0x11),tag,slot,err；amp 内联表达式消具名槽（P0）后
// 帧 0x7c→0x6c 与 ORIG 全等，全部槽/寄存器分配逐条一致（残余仅 .o 重定位占位，
// compare_tu AE 归一化后 clean）。
void _writeIncorrectItemInventoryForRecovery(unsigned int characNo,
                                             const Inven_Item& item,
                                             const char* tag, int err,
                                             int slot)
{
    LogManager::logFormat(
        1, "InterDispatcher.cpp",
        __PRETTY_FUNCTION__,
        0x1645,
        "ERROR_ITEM_LOG %u,%d,%d,%u,%d,%d,%d,%u,%d - %s slot(%d), error_line(%d) \n",
        // ORIG 实参槽序（0x28=+0x7, 0x2c=+0xb, 0x30=+0xd, 0x34=+0x11）：
        // 第 6..9 参 = m_addInfo2, m_endurance, m_fieldd, amp 首 int（P0 内联）
        characNo, (int)item.m_seal, (int)item.m_itemType, item.m_addInfo,
        (int)item.GetItemAttr(), item.m_addInfo2, (int)item.m_endurance,
        item.m_extendInfo, *(const int*)(const void*)&item.m_amp, tag, slot, err);
}