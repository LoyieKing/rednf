void TowerOfDespairMgr::SendMessageToCUser(CUser* user, char* msg)
{
    if (user->isGMUser())
    {
        PacketGuard guard;
        guard.put_header(0, 0xc);
        guard.put_byte(0);
        guard.put_short(0);
        guard.put_byte(0);
        guard.put_int((int)strlen(msg));
        guard.put_str(msg, (int)strlen(msg));
        guard.finalize(true);
        user->Send(guard);
    }
