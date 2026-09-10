void Dispatcher_ChangeEmotion::send(CUser* user, ParamBase& param)
{
    if (user != 0)
    {
        PacketGuard guard;
        guard.clear();
        guard.put_header(1, 0x102);
        guard.put_byte(1);
        guard.put_short((int)*(short*)((char*)&param + 4));
        guard.finalize(true);
        user->Send(guard);

        guard.clear();
        guard.put_header(0, 2);
        guard.put_byte(0);
        guard.put_short(1);
        user->make_basic_info((char*)&guard, '\0');
        guard.finalize(true);
        G_GameWorld()->send_all(guard, user);
    }
