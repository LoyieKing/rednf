// ==== Dispatcher_RequestAssaultPrice::send (ORIG 0x081df294) ====
void Dispatcher_RequestAssaultPrice::send(CUser* user, ParamBase& param)
{
    stRequestAssaultPrice& p = static_cast<stRequestAssaultPrice&>(param);

    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1bf);

    if (p.m_4 == 0)
    {
        guard.put_byte(1);
        guard.put_int(p.m_8);
    }
    else if (p.m_4 != (int)0x7fffffff)
    {
        guard.put_byte(0);
        guard.put_byte(p.m_4);
        if (p.m_4 == 10)
        {
            guard.put_int(p.m_8 - user->getCurCharacMoney());
        }
    }
    // ORIG 0x081df312：err==0x7fffffff 时既不 finalize 也不 Send，
    // else return 直落 dtor 岛（+0x11f），两正常分支 jmp/fall-through 共享 finalize 块。
    else
    {
        return;
    }
    guard.finalize(true);
    user->Send(guard);
}