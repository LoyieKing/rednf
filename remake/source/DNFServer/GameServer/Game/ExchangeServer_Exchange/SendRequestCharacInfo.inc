bool CExchangeServer::SendRequestCharacInfo(int a, int b, int serverId,
                                            int d, void* extra,
                                            int extraLen)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x84);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_int(serverId);
    guard.put_int(d);
    guard.put_int(extraLen);                       // ORIG: put_int(len) 后紧跟 binary
    guard.put_binary((char*)extra, extraLen);
    guard.finalize(false);
    if (TrySend(serverId, guard))
    {
        return true;
    }
    else
    {
        return false;
    }
