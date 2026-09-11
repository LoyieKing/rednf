// ORIG MakeNotipacketResponseCreature @ 0x0834097e：clear → put_header(0,0x68)
// → put_short(uid16) → finalize(true)。
void CPacketRespondent::MakeNotipacketResponseCreature()
{
    clear();
    put_header(0, 0x68);
    // ORIG 0x8340738：call 结果寄存器直取 movzwl %ax ⇒ 无具名 uid16 局部（P0 槽律）。
    put_short((int)(unsigned short)m_user->get_unique_id());
    finalize(true);
}