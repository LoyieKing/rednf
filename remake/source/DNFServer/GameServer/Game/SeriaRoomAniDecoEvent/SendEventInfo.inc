void SeriaRoomAniDecoEvent::SendEventInfo(CUser& user)  // 0x0827b99a
{
    PacketGuard pkt;
    pkt.put_header(0, 0x1c1);
    // [b28c-30] P0/限定①：ORIG 0x0827b9c8 段 deco 为 sret 匿名临时槽（-0xc）——
    // `call getSeriaRoomAniDecoInfo; movzwl -0xa(%ebp),%eax; call put_short` 单语句；
    // 具名 deco 局部改槽序（4.4 pkt -0x14/deco -0x18 反向）。
    // [b23S3C] ORIG movzwl base+2（= deco_flow）⇒ 取 m_field2
    pkt.put_short(user.getSeriaRoomAniDecoInfo().m_field2);
    pkt.finalize(true);
    user.Send(pkt);
