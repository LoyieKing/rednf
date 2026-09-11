// ORIG 0x086591fe：0xac 包头 + cooltime 表项迭代下发。
// b28d-solo-W8_CU_G: IDENTICAL/AE — ①`it = list->begin()` 提前到 size()/put_byte
// 之前（ORIG 0x0865923a begin 的 sret 直落 -0x30 it 槽）；②循环体恢复两个具名
// 局部（ORIG 0x0865927d/0x0865928d 两次 operator-> 结果落 -0x10/-0xc 槽）；
// ③返回类型 unsigned int→bool（P248 档①，Main 代落 CUser.h:3574）。帧 0x40、
// 槽位（-0x2c guard/-0x30 it/-0x18 list/-0x11 count）与 ORIG 全等，106/106 EQ
//（ORIG 尾 1 条对齐 nop，§6.1 豁免）。
bool CUser::SendCoolTimeItemInfo(unsigned int nowMs)
{
    PacketGuard packet;
    packet.put_header(0, 0xac);
    std::vector<ContinuousItemInfo>* list = GetCurCharacCoolTimeItemList();
    // b28d-solo-W8_CU_G：ORIG 0x0865923a begin() 先于 size()/put_byte（P0 槽序
    // 与求值序判据：begin 的 sret 直接落 it 槽 -0x30，无默认 ctor）。
    std::vector<ContinuousItemInfo>::iterator it = list->begin();
    unsigned char count = static_cast<unsigned char>(list->size());
    packet.put_byte(static_cast<int>(count));
    for (; it != list->end(); it++)   // b28d-21：P167 差集 ORIG 为 ppEi（后置，
                                    // P28 增量侧推论），改 it++。
    {
        // b28d-solo-W8_CU_G：ORIG 0x0865927d/0x0865928d 两次 operator-> 结果
        // 各存具名局部（-0x10 idx / -0xc expire，P0 槽往返实证）。
        unsigned int idx = it->m_itemIdx;
        unsigned int expire = it->m_expireTime;
        packet.put_int(idx);
        packet.put_int(static_cast<int>(expire - nowMs));
    }
    packet.finalize(true);
    return Send(packet);
}