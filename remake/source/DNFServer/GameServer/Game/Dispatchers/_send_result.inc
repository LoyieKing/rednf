// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08225c54 orig/ours=166/169 ndiff-blocks=27
//   帧差 sub $20 vs $30 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x20,%esp | lea    -0x18(%ebp),%eax | lea    -0x18(%ebp),%eax
// [SemA15 2026-09-12] P159/P27：ORIG 0x08225c54 尾段（0x8225e8f~0x8225ec2）全程无
//   `mov $0x0,%eax`（仅 `jmp` 共享尾声 + `call ~PacketGuard` + ret）⇒ 原码返回 void。
//   按 int 声明会多发 `mov $0x0,%ebx` + `mov %ebx,%eax` 两条（本波实测 166→164）。
void Dispatcher_Compound_ExtreamItem::_send_result()
{
    // [G1Frest 2026-09-10] ORIG 0x08225c54 区：guard 全程以 `lea -0x18(%ebp),%eax` 直取
    //   （帧 0x20），**无** `InterfacePacketBuf& pkt = *(InterfacePacketBuf*)&guard;`
    //   引用局部（旧码 -0x10 槽 + 帧 0x30、每处 `mov -0x10(%ebp),%eax`，P0/P183）。
    PacketGuard guard;
    guard.put_header(1, 0x146);
    guard.put_byte(1);
    for (int i = 0; i < 2; ++i)
    {
        guard.put_byte(0);
        guard.put_short((int)(unsigned short)m_slots[i]);          // +0x08/+0x0a
        guard.put_int(1);
    }
    guard.put_byte(0);
    guard.put_short((int)(unsigned short)m_field4);                // +0x04
    guard.put_int(1);
    guard.put_short((int)m_newSlot);                               // +0x06（short 符号扩展）
    guard.put_int(m_resultItem.m_addInfo);                         // +0x92（m_resultItem+0x02）
    guard.put_int(m_resultItem.m_addInfo2);                        // +0x97（m_resultItem+0x07）
    guard.put_byte((int)m_resultItem.GetItemAttr() & 0xff);        // m_resultItem（+0x90）
    guard.put_short((int)(unsigned short)m_resultItem.m_endurance);   // +0x9b（endurance）
    guard.put_byte((int)(unsigned char)m_resultItem.m_seal);     // m_resultItem+0x00
    // ORIG 该处调用点无 `xor $1` ⇒ 条件正极性（then=put_int(0) 先发射，else=m_fieldd）
    if (G_GameWorld()->IsEnchantRevisionChannel())
    {
        guard.put_int(0);
    }
    else
    {
        guard.put_int(m_resultItem.m_extendInfo);                      // +0x9d（m_resultItem+0x0d）
    }
    guard.put_byte((int)m_resultItem.m_amp.getAbilityType() & 0xff);       // +0xa1（m_amp）
    guard.put_short((int)m_resultItem.m_amp.getAbilityValue() & 0xffff);   // getAbilityValue
    guard.put_packet(m_resultItem);
    guard.finalize(true);
    m_user->Send(guard);                                           // m_user（+0xd0）
}