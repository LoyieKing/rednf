void Dispatcher_ChangeEmotion::send(CUser* user, ParamBase& param)
{
    if (user != 0)
    {
        // [G1Frest 2026-09-10] ORIG 0x081d1d72 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`
        //   ⇒ param 具名引用局部 -0xc（P0 槽往返）；旧码把 param 内联进表达式，
        //   帧少 4B、PacketGuard 槽由 -0x18 漂到 -0x14。
        stChangeEmotionParam& p = static_cast<stChangeEmotionParam&>(param);
        PacketGuard guard;
        guard.clear();
        guard.put_header(1, 0x102);
        guard.put_byte(1);
        guard.put_short((int)*(short*)((char*)&p + 4));
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
