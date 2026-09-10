void CVillageMonsterArea::SendVillageMonsterList(CUser* user)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xee);
    int countIdx = packet.get_index();
    int count = 0;
    // ORIG 0x086b37ff：占位字节传的是**具名 count 变量**（0x81 `mov -0x10(%ebp),%eax;
    //   mov %eax,0x4(%esp)`），非立即数 0 ⇒ 源形 `put_byte(count)`（此时 count==0）。
    packet.put_byte(count);
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        if (it->second.MakeVillageMonsterListPacket(packet))
            ++count;
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    if (user == 0)
        SendPacket(packet);
    else
        user->Send(packet);
