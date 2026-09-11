// ORIG MakeNotipacketRenameCreature @ 0x08340672：clear → put_header(0,0x65)
// → put_short(uid 低 16 位) → put_int(len) → put_str(name,len) → finalize(true)。
void CPacketRespondent::MakeNotipacketRenameCreature(const char* name, int len)
{
    clear();
    put_header(0, 0x65);
    // ORIG 0x8340738：call 结果寄存器直取 movzwl %ax ⇒ 无具名 uid16 局部（P0 槽律）。
    put_short((int)(unsigned short)m_user->get_unique_id());
    put_int(len);
    put_str(name, len);
    finalize(true);
}