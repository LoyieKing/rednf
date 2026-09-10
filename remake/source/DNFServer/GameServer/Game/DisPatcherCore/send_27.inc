void DisPatcher_CheckSecurityProtection::send(CUser* user, ParamBase& param)
{
    CheckSecurityProtection& out = static_cast<CheckSecurityProtection&>(param);
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x176);
    guard.put_byte(0);
    guard.put_byte(out.m_4);
    guard.finalize(true);
    user->Send(guard);
