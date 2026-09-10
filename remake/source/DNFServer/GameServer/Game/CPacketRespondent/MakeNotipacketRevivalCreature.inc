void CPacketRespondent::MakeNotipacketRevivalCreature()
{
    clear();
    put_header(0, 0x6b);
    // ORIG 0x8340738：call 结果寄存器直取 movzwl %ax ⇒ 无具名 uid16 局部（P0 槽律）。
    put_short((int)(unsigned short)m_user->get_unique_id());
    finalize(true);
