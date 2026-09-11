// ===========================================================================
// 连续效果道具（effect / cooltime）载入与下发
// ===========================================================================
// ORIG 0x08659366：0xae 包头 + 数量 + 逐条目{idx, expire-now}下发。
// b28d-solo-W8_CU_G: IDENTICAL/AE — ①循环体恢复两个具名局部（ORIG 0x086593de/
// 0x086593f5 两次 operator[] 结果落 -0x10/-0xc 槽，P0 槽往返判据），帧/PacketGuard
// 槽随之回到 -0x28；②循环变量 unsigned→int（P218 扩展：4.4 尾测由 `mov i,%edx;
// cmp %eax,%edx; setl` 折回 ORIG `movzbl count,%eax; cmp i,%eax; setg`）；
// ③返回类型 int→bool（P248 档①，Main 代落 CUser.h:2835）⇒ `mov %eax,%ebx` 直传。
// 90/90 逐条 EQ。
bool CUser::SendEffectItemInfo(unsigned int itemIdx)
{
    PacketGuard packet;
    packet.put_header(0, 0xae);
    std::vector<ContinuousItemInfo>* list = GetCurCharacEffectItemList();
    unsigned char count = static_cast<unsigned char>(list->size());
    packet.put_byte(static_cast<int>(count));
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        // b28d-solo-W8_CU_G：ORIG 0x086593de/0x086593f5 两次 operator[] 各存
        // 具名局部（-0x10 idx / -0xc expire，P0 槽往返实证）⇒ 恢复具名局部。
        unsigned int idx = (*list)[i].m_itemIdx;
        unsigned int expire = (*list)[i].m_expireTime;
        packet.put_int(idx);
        packet.put_int(static_cast<int>(expire - itemIdx));
    }
    packet.finalize(true);
    return Send(packet);
}