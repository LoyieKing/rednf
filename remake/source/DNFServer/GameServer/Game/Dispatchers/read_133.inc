int Dispatcher_Req_Urgent_Quest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
// ORIG 0x081df75a：PacketGuard put_header(1,0x1c6)；put_byte(param.m_found!=0)；
// found → put_short(param.m_questIdx)，否则 put_byte(param.m_errorCode)；
// finalize(true) 后 CUser::Send。
