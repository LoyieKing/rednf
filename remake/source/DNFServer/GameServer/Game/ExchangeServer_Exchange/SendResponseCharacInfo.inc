// ---------------------------------------------------------------------------
// SendResponseCharacInfo（带 session 版）  ORIG 0x8491644
// 包：header(1,0x85) + f + a + b + len + binary(data)
// ---------------------------------------------------------------------------
bool CExchangeServer::SendResponseCharacInfo(int a, int b, void* data,
                                             int len, CSession* session,
                                             int f)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x85);
    guard.put_int(f);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_int(len);
    guard.put_binary((char*)data, len);
    guard.finalize(false);
    return m_sessionMgr.TrySend(session, guard);
}