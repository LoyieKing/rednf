void CUser::use_equipment(int slot)
{
    int result = getCurCharacInvenW()->use_equip(slot);

    PacketGuard packet;
    packet.put_header(1, 0x33);
    // ORIG 0x08?? cmpl $0,-0xc; jle <else> —— fall-through=「(0,result)」块在前（技巧17），
    // 条件正向写 result>=1（<=0 跳 else 岛）
    if (result >= 1)
    {
        packet.put_byte(0);
        packet.put_byte(result);
    }
    else
    {
        packet.put_byte(1);
        packet.put_byte(slot);
    }
    packet.finalize(true);
    Send(packet);
