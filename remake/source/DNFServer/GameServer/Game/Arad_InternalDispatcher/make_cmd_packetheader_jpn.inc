// ============================================================================
// packetheader_jpn 【objdump】ORIG make_cmd_packetheader_jpn 0x08197812 T /
// make_noti_packetheader_jpn 0x08197862 T（无独立 md 报告，直接反汇编转写）：
//   bool 归一化(0/1) 后 put_header(typeFlag, (int)code) + put_byte(normalized)；
//   cmd 流 typeFlag=1，noti 流 typeFlag=0。
// ============================================================================
void make_cmd_packetheader_jpn(PacketGuard& guard, ENUM_CMDPACKET code, bool flag)
{
    signed char normalized = (flag != false) ? (signed char)1 : (signed char)0;
    guard.put_header(1, (int)code);
    guard.put_byte((int)normalized);
}