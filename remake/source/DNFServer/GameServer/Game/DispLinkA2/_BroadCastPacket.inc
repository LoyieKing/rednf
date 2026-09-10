void Dispatcher_EPLPCommand::_BroadCastPacket(CParty* party, MSG_EPLP_COMMAND& msg)
{
    PacketGuard guard;
    guard.put_header(1, 0x4b);
    guard.put_byte(1);                       // 类型 1：广播
    guard.put_byte(static_cast<int>(         // msg+0x0d（ORIG 参数槽直读）
        reinterpret_cast<MSG_EPLP_COMMAND_A2&>(msg).m_cmd));
    guard.put_byte(static_cast<int>(         // msg+0x0e
        reinterpret_cast<MSG_EPLP_COMMAND_A2&>(msg).m_subCmd));
    guard.finalize(true);
    party->send_to_party(guard);
