// ----------------------------------------------------------------------------
// ORIG T 0x081441d4 _ZN8WongWork10CBossTower14SendELPLPacketEv：
// 非 partyType==1 时逐有效成员 checkStartGameCondition(flag,false)，任一失败
// ok=false 提前跳出；随后无条件发包(0,0x1e4){ok}给全队；恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x081441d4：帧 0x30→0x40 + 槽平移（N82）+ 寄存器角色（N7）。
// [b28d-solo-CBossTower] AE：删 party 具名局部（P0 逐点重读 m_party）+ invalid
//   裸短路条件 + checkStartGameCondition bool 化（本类，ORIG 调用点 0x081441e6
//   `xor $1`）⇒ DIFF→AE。
bool CBossTower::SendELPLPacket()
{
    bool ok = true;                            // ebp-0x11
    if (m_party->m_battleField.m_bloodState != 1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = m_party->get_user(i);
            // ORIG 0x0814420e..0x08144221：短路物化岛无 bool 槽
            // （mov$1/jmp/mov$0 + test %al,%al;jne 循环尾）⇒ 裸条件内联（P0/P226）。
            if (member == 0 || m_party->checkValidUser(i) != 1)
                continue;

            unsigned char requireFlag = 0;       // ebp-0x21
            if (!checkStartGameCondition(requireFlag, false))
            {
                ok = false;
                break;
            }
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0x1e4);
    packet.put_byte(ok ? 1 : 0);
    packet.finalize(true);
    m_party->send_to_party(packet);
    return 1;
}