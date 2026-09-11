void CParty::SendEachCardInfo()
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85b44d6
    // b28d-solo-CParty：ORIG 0x85b44f0 `lea -0x18(%ebp),%eax` 直接构造
    // PacketGuard（-0x18）⇒ 无 BattleData& bd 引用槽（P0：具名引用多占槽，
    // 使 PacketGuard 槽整体深移 0xc）。
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x4a);
    packet.put_byte(1);
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        // b28d-solo-CParty：ORIG 0x85b4510 形为 `movzbl mem,%eax; movsbl %al,%eax`
        // （P11 符号扩展）；(int)(char) 双 cast 被 4.4 折成 movzbl ⇒ 用隐式提升。
        packet.put_byte(m_field204[0][i]);   // +0x204[i]
        packet.put_byte(m_field204[1][i]);   // +0x208[i]
        if ((unsigned char)m_field204[1][i] == 0xff)
        {
            packet.put_byte(0);
        }
        else
        {
            int countIdx = packet.get_index();
            // b28d-solo-CParty：ORIG 0x85b4560 起对 item 数组逐次完整两级寻址
            // （P157/P183：无 Inven_Item& 引用槽，否则 PacketGuard/循环变量整体深移）。
            packet.put_byte(1);
            packet.put_int(0);
            // b28d-solo-CParty：ORIG 0x85b4580 索引前为 `movsbl %al,%eax`（P11
            // 有符号扩展）⇒ 索引表达式不得 (unsigned char) cast。
            packet.put_int(m_battleData.m_itemGroups2[m_field204[1][i]][0].get_add_info());
            if (m_battleData.m_itemGroups2[m_field204[1][i]][1].m_addInfo != 0)  // +0x7af int 读
            {
                packet.put_int(m_battleData.m_itemGroups2[m_field204[1][i]][1].m_addInfo);
                packet.put_int(m_battleData.m_itemGroups2[m_field204[1][i]][1].get_add_info());
                packet.put_byte(countIdx, 2);           // 回填选项组数
            }
        }
        packet.put_byte((unsigned char)m_battleData.m_arr60[i]);  // +0x38c
    }
    packet.finalize(true);
    send_to_party(packet);
}