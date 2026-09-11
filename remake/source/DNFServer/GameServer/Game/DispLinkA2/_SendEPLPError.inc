// ============================================================================
// 2/3. Dispatcher_EPLPCommand::_SendEPLPError / _BroadCastPacket
//     报告 docs/class_func_reports/Dispatcher_EPLPCommand/{_SendEPLPError,_BroadCastPacket}.md
// ============================================================================
void Dispatcher_EPLPCommand::_SendEPLPError(CParty* party, unsigned char subCmd,
                                            unsigned char errCode)
{
    PacketGuard guard;                       // 0x0858dd4c
    guard.put_header(1, 0x4b);
    guard.put_byte(0);                       // 类型 0：错误应答
    guard.put_byte(static_cast<int>(errCode));   // -0x20 参数（先写）
    guard.put_byte(static_cast<int>(subCmd));    // -0x1c 参数（后写）
    guard.finalize(true);
    party->send_to_party(guard);             // 0x0859d14e
}