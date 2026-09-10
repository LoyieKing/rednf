void Dispatcher_Mercenary_Competition::send(CUser* user, ParamBase& param)    // ORIG 0x081dde20 (83 insn)
{
    SendMercenaryCompetitionResult& result = static_cast<SendMercenaryCompetitionResult&>(param);
    char* pResult = reinterpret_cast<char*>(&result);
    PacketGuard guard;
    guard.put_header(1, 0x1bb);
    guard.put_byte(pResult[0x04] != 0);
    if (pResult[0x04] == 0)
    {
        guard.put_byte(0x15);                                                  // 失败错误码
    }
    else
    {
        guard.put_int(*reinterpret_cast<int*>(pResult + 0x08));
        guard.put_byte(static_cast<unsigned int>(static_cast<char>(pResult[0x0d])));
        guard.put_byte(static_cast<unsigned int>(static_cast<char>(pResult[0x0c])));
    }
    guard.finalize(true);
    user->Send(guard);
