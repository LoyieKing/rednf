// ============================================================================
// send —— ORIG 0x081d4784
//   param.error（+0x04）非 0 → SendCmdErrorPacket(0x153, error)
//   否则构造 PacketGuard：header(1,0x153) | byte(1) | int(param.resultIdx)
//        | short(param.resultCount) | finalize(true) → CUser::Send
// ============================================================================
// [round3-L30 修] ORIG 0x081d4784：error **无具名槽**（P0）——
//   判空 `movzbl 0x4(%eax),%eax; test %al,%al`（寄存器直判），使用时**重新读**
//   `mov -0xc(%ebp),%eax; movzbl 0x4(%eax),%eax; movzbl %al,%eax`（P48 无 CSE，
//   两次独立字段读）⇒ 删我方 `unsigned char error` 具名局部（旧判词 P73/P248 作废）。
void Dispatcher_UseLimitCube::send(CUser* user, ParamBase& param)
{
    stUseLimitCube& st = *reinterpret_cast<stUseLimitCube*>(&param);
    if (st.m_4 != 0)                     // +0x04（ORIG movzbl + test %al 直判）
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x153, st.m_4);
        return;
    }
    PacketGuard packet;
    packet.put_header(1, 0x153);
    packet.put_byte(1);
    packet.put_int(st.m_8);
    packet.put_short((short)st.m_c);   // ORIG 0x081d4814 `mov 0xc(%eax),%eax; cwtl`
    packet.finalize(true);
    user->Send(packet);
}